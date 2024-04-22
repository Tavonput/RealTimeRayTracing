
// this is grabing the container element
const container = document.querySelector('.container');

// this is checking for slider movement.
document.querySelector('.slider').addEventListener('input', (e) => {
  // then it update the container's '--position' variable based on slider value.
  container.style.setProperty('--position', `${e.target.value}%`);
})

// function to toggle dark mode
document.getElementById('mode-slider').addEventListener('change', function(e) {
  if (e.target.checked) {
    document.body.classList.add('dark-mode');
  } else {
    document.body.classList.remove('dark-mode');
  }
});

// If you want to save the user's preference
window.addEventListener('load', function() {
  var modeSlider = document.getElementById('mode-slider');
  var isDarkMode = localStorage.getItem('darkMode') === 'true';
  modeSlider.checked = isDarkMode;
  document.body.classList.toggle('dark-mode', isDarkMode);
});

document.getElementById('mode-slider').addEventListener('change', function(e) {
  localStorage.setItem('darkMode', e.target.checked);
});



document.addEventListener("DOMContentLoaded", function() {
  // Callback function for the IntersectionObserver
  const observerCallback = (entries, observer) => {
    entries.forEach((entry) => {
      if (entry.isIntersecting) {
        // Add 'show' class when the element is in the viewport
        entry.target.classList.add('show');
      } else {
        // Remove 'show' class when the element is not in the viewport
        entry.target.classList.remove('show');
      }
    });
  };

  // Create a new IntersectionObserver with the callback
  const observer = new IntersectionObserver(observerCallback, {
    rootMargin: '0px',
    threshold: 0.1
  });

  // Select sections to be revealed
  const sections = document.querySelectorAll('.fade-in-section');

  // Observe the sections
  sections.forEach((section) => observer.observe(section));
});

// Example JavaScript for smooth scrolling with offset for a fixed header
document.querySelectorAll('nav a[href^="#"]').forEach(anchor => {
  anchor.addEventListener('click', function (e) {
    e.preventDefault();
    const targetElement = document.querySelector(this.getAttribute('href'));
    const headerOffset = 70; // Height of your fixed header
    const elementPosition = targetElement.getBoundingClientRect().top;
    const offsetPosition = elementPosition + window.pageYOffset - headerOffset;

    window.scrollTo({
      top: offsetPosition,
      behavior: 'smooth'
    });
  });
});



