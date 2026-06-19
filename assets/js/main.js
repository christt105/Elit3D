// Elit3D landing — light progressive enhancement (no dependencies).

// Current year in the footer.
document.getElementById("year").textContent = new Date().getFullYear();

// Add a border to the nav once the page is scrolled.
const nav = document.getElementById("nav");
const onScroll = () => nav.classList.toggle("is-scrolled", window.scrollY > 8);
onScroll();
window.addEventListener("scroll", onScroll, { passive: true });

// Reveal sections as they enter the viewport.
const revealTargets = document.querySelectorAll(
  ".section__head, .card, .gallery__placeholder, .cta-box, .tech-card, .roadmap__item, .strip__item"
);
revealTargets.forEach((el) => el.classList.add("reveal"));

if ("IntersectionObserver" in window) {
  const io = new IntersectionObserver(
    (entries) => {
      entries.forEach((entry) => {
        if (entry.isIntersecting) {
          entry.target.classList.add("is-visible");
          io.unobserve(entry.target);
        }
      });
    },
    { threshold: 0.12 }
  );
  revealTargets.forEach((el) => io.observe(el));
} else {
  revealTargets.forEach((el) => el.classList.add("is-visible"));
}
