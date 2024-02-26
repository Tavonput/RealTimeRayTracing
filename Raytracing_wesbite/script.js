// this is grabing the container element
const container = document.querySelector('.container');

// this is checking for slider movement.
document.querySelector('.slider').addEventListener('input', (e) => {
  // then it update the container's '--position' variable based on slider value.
  container.style.setProperty('--position', `${e.target.value}%`);
})

// function to toggle dark mode
function toggleDarkMode() {
  var element = document.body;
  var themeToggleImg = document.getElementById('theme-toggle');
  element.classList.toggle("dark-mode");

  if (element.classList.contains("dark-mode")) {
      themeToggleImg.src = 'image\sun.png'; // path to your sun image
      themeToggleImg.alt = 'Toggle Light Mode';
  } else {
      themeToggleImg.src = 'image\moon.png'; // path to your moon image
      themeToggleImg.alt = 'Toggle Dark Mode';
  }
}

// checking for the saved user preference
document.addEventListener('DOMContentLoaded', (event) => {
  if(localStorage.getItem('darkMode') === 'enabled') {
      document.body.classList.add('dark-mode');
  }
});


