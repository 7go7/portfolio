document.getElementById("year").textContent = new Date().getFullYear();

fetch("/api/profile")
  .then((res) => res.json())
  .then((data) => {
    // About
    if (data.about) {
      document.getElementById("about-text").textContent = data.about;
    }

    // Experience
    const expList = document.getElementById("experience-list");
    data.experience.forEach((job) => {
      const div = document.createElement("div");
      div.className = "exp-item";
      div.innerHTML = `
        <div class="exp-header">
          <div>
            <div class="exp-role">${job.role}</div>
            <div class="exp-place">${job.company}</div>
          </div>
          <div class="exp-dates">${job.dates}</div>
        </div>
        <p class="exp-desc">${job.description}</p>
      `;
      expList.appendChild(div);
    });

    // Skills
    const skillsList = document.getElementById("skills-list");
    data.skills.forEach((skill) => {
      const li = document.createElement("li");
      li.textContent = skill;
      skillsList.appendChild(li);
    });

    // Education
    const eduList = document.getElementById("education-list");
    data.education.forEach((ed) => {
      const li = document.createElement("li");
      li.innerHTML = `<strong>${ed.title}</strong> — ${ed.place} (${ed.dates})`;
      eduList.appendChild(li);
    });
  })
  .catch((err) => console.error("API error:", err));
