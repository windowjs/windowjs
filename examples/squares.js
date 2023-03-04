console.log('Loading squares.js');

let tilesheet = null;
let boxBitmap = null;
let animate = true;

let requestedDraw = false;

function requestDraw() {
  if (!requestedDraw) {
    requestAnimationFrame(draw);
    requestedDraw = true;
  }
}

function draw() {
  requestedDraw = false;

  const now = performance.now();

  if (animate) {
    requestDraw();
  }

  const canvas = window.canvas;

  canvas.clearRect(0, 0, canvas.width, canvas.height);
  canvas.fillStyle = '#000000b0';
  canvas.fillRect(0, 0, canvas.width, canvas.height);

  canvas.fillStyle = 'white';
  const size = canvas.width / 32;
  canvas.font = `${size}px sans`;
  canvas.fillText('Timing sans', 132, canvas.height - size / 2);
  canvas.font = `${size}px serif`;
  canvas.fillText('Timing serif', 132 + size * 8, canvas.height - size / 2);
  canvas.font = `${size}px monospace`;
  canvas.fillText('Timing monospace', 132 + size * 16,
                  canvas.height - size / 2);

  canvas.fillStyle = 'blue';
  canvas.fillRect(0, 0, 100, 100);
  canvas.fillRect(canvas.width - 100, 0, 100, 100);
  canvas.fillRect(0, canvas.height - 100, 100, 100);
  canvas.drawImage(boxBitmap, canvas.width - 100, canvas.height - 100, 100,
                   100);

  canvas.fillStyle = 'red';
  const width = canvas.width - 300;
  const offset = Math.cos(3 * now / 1000) * (width / 2);
  canvas.fillRect(canvas.width / 2 + offset - 50, canvas.height / 2 - 50, 100,
                  100);
}

async function saveScreenshot(format) {
  const path1 = 'screenshot1.' + format;
  const path2 = 'screenshot2.' + format;
  console.log('Encoding as ' + format + '...');
  const x = window.canvas.encode(format);
  const image = window.canvas.getImageData();
  const y = image.encode(format);
  const xx = await x;
  const yy = await y;
  console.log('Writing to "' + path1 + '" and "' + path2 + '"...');
  await File.write(path1, xx);
  await File.write(path2, yy);
  console.info('Saved screenshots to ' + path1 + ' and ' + path2 + '.');
}

window.addEventListener('keydown', function(event) {
  const key = event.key;
  console.log('-- key ' + key);
  if (key == 'Escape') {
    window.close();
  } else if (key == 'j') {
    saveScreenshot('jpg');
  } else if (key == 'p') {
    saveScreenshot('png');
  } else if (key == 'w') {
    saveScreenshot('webp');
  } else if (key == 'v') {
    window.vsync = !window.vsync;
    console.info('vsync is now ' + window.vsync);
  } else if (key == 'a') {
    animate = !animate;
    if (animate) {
      requestDraw();
    }
  } else if (key == 'e') {
    throw new Error('Test exception');
  } else if (key == 'u') {
    callingSomethingThatDoesntExist();
  } else if (key == 't') {
    window.requestAttention();
  } else if (key == 'm') {
    window.minimize();
  }
});

window.addEventListener('close', function(event) {
  console.error('Blocking close event! Use ESC to quit.');
  return true;
});

function logEvent(event) {
  console.log('Event: ' + event.type);
}

window.addEventListener('resize', requestDraw);
window.addEventListener('maximize', logEvent);
window.addEventListener('minimize', logEvent);
window.addEventListener('restore', logEvent);
window.addEventListener('focus', logEvent);
window.addEventListener('blur', logEvent);
window.addEventListener('mousedown', logEvent);
window.addEventListener('mouseup', logEvent);
window.addEventListener('click', logEvent);

window.addEventListener('wheel', function(event) {
  logEvent(event);
  window.keepAspectRatio = false;
  window.width = window.width == 1000 ? 2000 : 1000;
});

window.addEventListener('drop', function(event) {
  console.log('Event: drop: ' + JSON.stringify(event));
});

async function init() {
  window.title = 'Squares';
  window.width = window.screen.availWidth / 2;
  window.height = window.width / 16 * 10;
  window.keepAspectRatio = true;

  tilesheet = await File.readImageData('examples/data/tilesheet.png');
  const box = new ImageData(tilesheet, 6 * 128, 0, 128, 128);
  boxBitmap = new ImageBitmap(box);
  window.icon = [ box ];

  requestDraw();

  window.debug.showOverlayConsole = true;
  window.debug.overlayConsoleTextColor = 'yellow';
  window.debug.showOverlayStats = true;
  console.info('squares.js loaded. DevicePixelRatio ' + devicePixelRatio +
               ' RetinaScale ' + window.retinaScale);
  console.log('j/p/w: save screenshots as Jpeg, Png or Webp.');
  console.log('v: toggle vsync.');
  console.log('a: toggle animation.');
  console.log('e: throw an exception.');
  console.log('u: use undefined variable.');
  console.log('t: request attention.');
  console.log('m: minimize.');
}

await init();
