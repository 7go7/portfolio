cmake_minimum_required(VERSION 3.10)
project(portfolio_cpp_raw)
set(CMAKE_CXX_STANDARD 20)

add_executable(portfolio
    backend/main.cpp
)
# on Linux usually nothing else is needed
