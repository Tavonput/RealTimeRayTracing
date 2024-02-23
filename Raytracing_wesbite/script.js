// this is grabing the container element
const container = document.querySelector('.container');

// this is checking for slider movement.
document.querySelector('.slider').addEventListener('input', (e) => {
  // then it update the container's '--position' variable based on slider value.
  container.style.setProperty('--position', `${e.target.value}%`);
})

// function to toggle dark mode
function toggleDarkMode() {
  const body = document.body;
  body.classList.toggle('dark-mode');
  
  // save the user preference in a localStorage
  if(body.classList.contains('dark-mode')) {
      localStorage.setItem('darkMode', 'enabled');
  } else {
      localStorage.setItem('darkMode', 'disabled');
  }
}

// checking for the saved user preference
document.addEventListener('DOMContentLoaded', (event) => {
  if(localStorage.getItem('darkMode') === 'enabled') {
      document.body.classList.add('dark-mode');
  }
});


