// backend/main.cpp  (POSIX/Linux)
// Build with C++17 or C++20. No external libs.

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

static constexpr int PORT = 8080;

// --- helpers ---------------------------------------------------------------
static std::string read_file(const std::string& path) {
    std::ifstream f(path, std::ios::binary);
    if (!f) return {};
    std::ostringstream ss; ss << f.rdbuf(); return ss.str();
}

static bool ends_with(const std::string& s, const std::string& suf) {
    return s.size() >= suf.size() && s.compare(s.size()-suf.size(), suf.size(), suf) == 0;
}
static std::string mime_type(const std::string& path) {
    if (ends_with(path, ".html")) return "text/html; charset=utf-8";
    if (ends_with(path, ".css"))  return "text/css";
    if (ends_with(path, ".js"))   return "application/javascript";
    if (ends_with(path, ".png"))  return "image/png";
    if (ends_with(path, ".jpg") || ends_with(path, ".jpeg")) return "image/jpeg";
    if (ends_with(path, ".ico"))  return "image/x-icon";
    if (ends_with(path, ".svg"))  return "image/svg+xml";
    return "text/plain; charset=utf-8";
}
// --------------------------------------------------------------------------

int main() {
    // prepare static roots relative to running from build/ directory
    const std::string STATIC_ROOT = "../frontend";
    const std::string PROFILE_JSON_PATH = "../backend/profile.json";
    const std::string profile_json = read_file(PROFILE_JSON_PATH);

    int server_fd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) { perror("socket"); return 1; }

    int opt = 1; ::setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in addr{}; addr.sin_family = AF_INET; addr.sin_addr.s_addr = INADDR_ANY; addr.sin_port = htons(PORT);
    if (::bind(server_fd, (sockaddr*)&addr, sizeof(addr)) < 0) { perror("bind"); ::close(server_fd); return 1; }
    if (::listen(server_fd, 16) < 0) { perror("listen"); ::close(server_fd); return 1; }

    std::cout << "C++ HTTP server running on http://0.0.0.0:" << PORT << "\n";

    while (true) {
        sockaddr_in client{}; socklen_t clen = sizeof(client);
        int client_fd = ::accept(server_fd, (sockaddr*)&client, &clen);
        if (client_fd < 0) { perror("accept"); continue; }

        char buf[8192];
        ssize_t n = ::read(client_fd, buf, sizeof(buf)-1);
        if (n <= 0) { ::close(client_fd); continue; }
        buf[n] = '\0';
        std::string req(buf);

        std::istringstream rs(req);
        std::string method, path, ver; rs >> method >> path >> ver;
        if (path.empty()) path = "/";

        auto write_all = [&](const std::string& s){
            const char* p = s.data(); size_t left = s.size();
            while (left) { ssize_t w = ::write(client_fd, p, left); if (w <= 0) break; p += w; left -= (size_t)w; }
        };

        if (path == "/api/profile") {
            const std::string body = profile_json.empty() ? "{}" : profile_json;
            std::ostringstream hdr;
            hdr << "HTTP/1.1 200 OK\r\n"
                << "Content-Type: application/json\r\n"
                << "Content-Length: " << body.size() << "\r\n"
                << "Connection: close\r\n\r\n";
            write_all(hdr.str()); write_all(body);
            ::close(client_fd); continue;
        }

        std::string rel = (path == "/") ? "/index.html" : path;
        std::string full = STATIC_ROOT + rel;
        std::string body = read_file(full);

        if (body.empty()) {
            static const std::string msg = "Not found";
            std::ostringstream hdr;
            hdr << "HTTP/1.1 404 Not Found\r\n"
                << "Content-Type: text/plain; charset=utf-8\r\n"
                << "Content-Length: " << msg.size() << "\r\n"
                << "Connection: close\r\n\r\n";
            write_all(hdr.str()); write_all(msg);
            ::close(client_fd); continue;
        }

        std::ostringstream hdr;
        hdr << "HTTP/1.1 200 OK\r\n"
            << "Content-Type: " << mime_type(full) << "\r\n"
            << "Content-Length: " << body.size() << "\r\n"
            << "Connection: close\r\n\r\n";
        write_all(hdr.str()); write_all(body);
        ::close(client_fd);
    }
    ::close(server_fd);
    return 0;
}
