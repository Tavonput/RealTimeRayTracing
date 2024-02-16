// this is grabing the container element
const container = document.querySelector('.container');

// this is checking for slider movement.
document.querySelector('.slider').addEventListener('input', (e) => {
  // then it update the container's '--position' variable based on slider value.
  container.style.setProperty('--position', `${e.target.value}%`);
})