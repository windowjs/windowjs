window.title = 'Hello!';

const canvas = window.canvas;

window.addEventListener('click', function(event) {
  console.log(`Clicked on ${event.x}, ${event.y}`);
});

window.addEventListener('keydown', function(event) {
  console.log(`Key down: ${event.key}`);
});

function draw() {
  canvas.fillStyle = '#023047';
  canvas.fillRect(0, 0, canvas.width, canvas.height);

  canvas.fillStyle = '#eb005a';
  canvas.fillRect(100, 100, 200, 200);

  canvas.fillStyle = 'darkorange';
  const y = canvas.height / 2;
  const w = canvas.width;
  const t = Math.cos(performance.now() / 300);
  const x = w / 2 + (w / 4) * t;
  canvas.save();
  canvas.translate(x, y);
  canvas.rotate(t * Math.PI / 2);
  canvas.fillRect(-100, -100, 200, 200);
  canvas.restore();

  // Request to draw again when the next frame is ready.
  requestAnimationFrame(draw);
}

requestAnimationFrame(draw);
