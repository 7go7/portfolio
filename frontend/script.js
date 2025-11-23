document.getElementById("year").textContent = new Date().getFullYear();

// Animation Observer setup
const observerOptions = {
  threshold: 0.1, // Trigger when 10% of the element is visible
  rootMargin: "0px 0px -50px 0px"
};

const observer = new IntersectionObserver((entries) => {
  entries.forEach((entry) => {
    if (entry.isIntersecting) {
      entry.target.classList.add("show");
      observer.unobserve(entry.target); // Only animate once
    }
  });
}, observerOptions);

// Function to attach observer to elements
function initAnimations() {
  const hiddenElements = document.querySelectorAll('.hidden');
  hiddenElements.forEach((el) => observer.observe(el));
}

// Initial animate for static elements (Hero)
initAnimations();

// Fetch Data
fetch("/api/profile")
  .then((res) => res.json())
  .then((data) => {
    // 1. About
    if (data.about) {
      document.getElementById("about-text").textContent = data.about;
    }

    // 2. Experience
    const expList = document.getElementById("experience-list");
    data.experience.forEach((job) => {
      const div = document.createElement("div");
      div.className = "exp-item"; // No animation class here, the parent container animates
      div.innerHTML = `
        <div class="exp-header">
          <div>
             <span class="exp-role">${job.role}</span>
             <span class="exp-place"> @ ${job.company}</span>
          </div>
          <div class="exp-dates">${job.dates}</div>
        </div>
        <p class="exp-desc">${job.description}</p>
      `;
      expList.appendChild(div);
    });

    // 3. Skills
    const skillsList = document.getElementById("skills-list");
    data.skills.forEach((skill) => {
      const li = document.createElement("li");
      li.textContent = skill;
      skillsList.appendChild(li);
    });

    // 4. Education
    const eduList = document.getElementById("education-list");
    data.education.forEach((ed) => {
      const li = document.createElement("li");
      li.innerHTML = `
        <strong>${ed.title}</strong> 
        <span style="color: var(--accent)">${ed.place}</span> 
        <br><span style="font-size: 0.8em">${ed.dates}</span>
      `;
      eduList.appendChild(li);
    });
    
    // Re-run animations or check if elements are already in view 
    // (Useful if API loads very fast and sections were already rendered)
    initAnimations();
  })
  .catch((err) => {
    console.error("API error:", err);
    // Even if API fails, show the sections so the structure remains
    document.querySelectorAll('.hidden').forEach(el => el.classList.add('show'));
  });