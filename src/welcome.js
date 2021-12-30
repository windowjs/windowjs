window.title = 'Welcome';

const canvas = window.canvas;

let font = 'sans';
const fonts = [ 'Segoe UI', 'Verdana', 'Arial', 'DejaVu Sans' ];
for (const f of fonts) {
  if (window.fonts.includes(f)) {
    font = f;
    break;
  }
}

let titleFont = font;

let boxX = 0;
let boxY = 0;
let boxW = 0;
let boxH = 0;

let animStarted = 0;
let left = false;

function isInsideBox(x, y) {
  return x >= boxX && x <= boxX + boxW && y >= boxY && y <= boxY + boxH;
}

function isInsideSquare(x, y) {
  const xx = left ? 0 : canvas.width - 100;
  return x >= xx && x < xx + 100 && y >= canvas.height - 100;
}

function draw() {
  canvas.fillStyle = 'white';
  canvas.fillRect(0, 0, canvas.width, canvas.height);

  canvas.fillStyle = '#023047';
  canvas.fillRect(0, 0, canvas.width, 60 * devicePixelRatio);

  canvas.fillStyle = '#eb005a';
  canvas.font = `bold ${36 * devicePixelRatio}px "${titleFont}"`;
  canvas.fillText('Window.js', 12, 42 * devicePixelRatio);

  canvas.font = `bold ${18 * devicePixelRatio}px "${font}"`;
  canvas.fillText('Welcome to Window.js!', 12, 112 * devicePixelRatio);

  canvas.fillStyle = 'black';

  canvas.font = `${14 * devicePixelRatio}px "${font}"`;
  canvas.fillText('Window.js needs a source file to run.', 12,
                  168 * devicePixelRatio);
  canvas.fillText('Open the console with F1 to try out the APIs.', 12,
                  192 * devicePixelRatio);

  const text = 'See the Getting Started documentation at ';
  const dx = canvas.measureText(text).width;
  canvas.fillText('See the Getting Started documentation at ', 12,
                  216 * devicePixelRatio);

  const link = 'windowjs.org/doc';
  boxX = 12 + dx;
  boxY = 216 * devicePixelRatio;
  boxW = canvas.measureText(link).width;
  boxH = 24 * devicePixelRatio;

  canvas.fillStyle = '#007ab8';
  canvas.fillText('windowjs.org/doc', boxX, boxY);

  canvas.fillStyle = 'black';
  canvas.fillText('.', boxX + boxW, boxY);

  boxY -= 24;

  canvas.fillStyle = '#eb005a';
  if (animStarted > 0) {
    const y = canvas.height - 50;
    const w = canvas.width;
    const a = (performance.now() - animStarted) / 400;
    const t = Math.cos(a) * (left ? -1 : 1);
    const x = w / 2 + (w / 2 - 50) * t;
    canvas.save();
    canvas.translate(x, y);
    canvas.rotate(t * Math.PI);
    canvas.fillRect(-50, -50, 100, 100);
    canvas.restore();
    if (a >= Math.PI) {
      animStarted = 0;
      left = !left;
    }
    requestAnimationFrame(draw);
  } else if (left) {
    canvas.fillRect(0, canvas.height - 100, 100, 100);
  } else {
    canvas.fillRect(canvas.width - 100, canvas.height - 100, 100, 100);
  }
}

window.addEventListener('mousemove', function(event) {
  if (isInsideBox(event.x, event.y) || isInsideSquare(event.x, event.y)) {
    window.cursor = 'hand';
  } else {
    window.cursor = 'arrow';
  }
});

window.addEventListener('click', function(event) {
  if (event.button == 0 && isInsideBox(event.x, event.y)) {
    window.open('https://windowjs.org/doc');
  } else if (event.button == 0 && isInsideSquare(event.x, event.y) &&
             animStarted == 0) {
    animStarted = performance.now();
    requestAnimationFrame(draw);
  }
});

window.addEventListener('resize', draw);

async function loadOleoFont() {
  await window.loadFont('examples/data/OleoScriptSwashCaps.ttf',
                        'OleoScriptSwashCaps');
  titleFont = 'OleoScriptSwashCaps';
  requestAnimationFrame(draw);
}

loadOleoFont();
requestAnimationFrame(draw);
