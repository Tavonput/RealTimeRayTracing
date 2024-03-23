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

