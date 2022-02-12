// Tests for the Canvas API: https://windowjs.org/doc/canvas
//
// Many of these tests compare the pixels in the canvas to a golden screenshot.
// The golden screenshots are generated with canvas.html.
// If this file is changed, then verify that canvas.html still works too.
//
// After adding a new test, open canvas.html to render its output in Chrome
// and save that canvas (right-click, Save image as...) as the golden for
// that test.

import {
  createCanvas,
  diffCanvasToFile,
  unwrapCanvas,
} from './lib/lib.js';

export async function initialCanvasIsOpaqueBlack() {
  const canvas = window.canvas;
  canvas.fillStyle = 'black';
  canvas.fillRect(0, 0, canvas.width, canvas.height);
  await diffCanvasToFile('data/black.png');
}

export async function clearedCanvasIsTransparent() {
  const canvas = window.canvas;
  canvas.clearRect(0, 0, canvas.width, canvas.height);
  await diffCanvasToFile('data/transparent.png');
}

export async function opaqueWhiteFill() {
  const canvas = window.canvas;
  canvas.fillStyle = 'white';
  canvas.fillRect(0, 0, canvas.width, canvas.height);
  await diffCanvasToFile('data/white.png');
}

export async function fillRect() {
  const canvas = window.canvas;
  canvas.fillStyle = 'white';
  canvas.fillRect(0, 0, canvas.width, canvas.height);
  canvas.fillStyle = 'red';
  canvas.fillRect(0, 0, 100, 200);
  canvas.fillStyle = 'green';
  canvas.fillRect(100, 0, 100, 200);
  canvas.fillStyle = 'blue';
  canvas.fillRect(200, 0, 100, 200);
  canvas.fillStyle = '#4080c080';
  canvas.fillRect(300, 0, 100, 200);
  canvas.fillStyle = '#00000080';
  canvas.fillRect(0, 100, 400, 100);
  await diffCanvasToFile('data/fill_rect.png');
}

export async function arcs() {
  const canvas = window.canvas;
  canvas.fillStyle = 'red';
  canvas.arc(50, 50, 20, 0, Math.PI / 4 * 3);
  canvas.fill();

  // TODO: removing beginPath does different glitches in Chrome and Window.js.
  canvas.beginPath();
  canvas.arc(100, 50, 20, 0, Math.PI / 4 * 3);
  canvas.fillStyle = 'green';
  canvas.fill();

  canvas.beginPath();
  canvas.arc(50, 30, 20, Math.PI, 0);
  canvas.strokeStyle = 'cyan';
  canvas.lineWidth = 10;
  canvas.stroke();

  canvas.beginPath();
  canvas.arc(110, 30, 20, Math.PI, 0);
  canvas.strokeStyle = 'yellow';
  canvas.lineCap = 'round';
  canvas.stroke();

  canvas.beginPath();
  canvas.moveTo(190, 20);
  canvas.arcTo(200, 110, 50, 20, 40);
  canvas.strokeStyle = 'magenta';
  canvas.stroke();

  await diffCanvasToFile('data/arcs.png', 540);
}

export async function bezierCurves() {
  const canvas = window.canvas;

  canvas.strokeStyle = 'red';
  canvas.lineWidth = 10;
  canvas.moveTo(10, 20);
  canvas.bezierCurveTo(190, 30, 110, 80, 210, 100);
  canvas.stroke();

  canvas.strokeStyle = 'green';
  canvas.beginPath();
  canvas.moveTo(100, 20);
  canvas.quadraticCurveTo(280, 30, 100, 100);
  canvas.stroke();

  await diffCanvasToFile('data/bezier_curves.png', 1000);
}

export async function clip() {
  const canvas = window.canvas;

  canvas.save();
  canvas.rect(100, 100, 100, 100);
  canvas.clip();
  canvas.fillStyle = 'red';
  canvas.fillRect(0, 0, 400, 200);

  canvas.restore();
  canvas.fillStyle = 'blue';
  canvas.fillRect(0, 0, 400, 100);

  await diffCanvasToFile('data/clip.png');
}

export async function drawImageBitmap() {
  const canvas = window.canvas;
  const image = await File.readImageBitmap(__dirname + '/data/image.png');
  canvas.drawImage(image, 0, 0);
  canvas.drawImage(image, 100, 0, 100, 50);
  await diffCanvasToFile('data/draw_image_bitmap.png', 4020);
}

export async function drawImageBitmapWithSmoothing() {
  const canvas = window.canvas;
  canvas.imageSmoothingEnabled = true;
  const image = await File.readImageBitmap(__dirname + '/data/image.png');
  canvas.drawImage(image, 0, 0);
  canvas.drawImage(image, 100, 0, 100, 50);
  await diffCanvasToFile('data/draw_image_bitmap_smooth.png', 4400);
}

export async function ellipses() {
  const canvas = window.canvas;

  canvas.scale(0.7, 0.7);

  canvas.fillStyle = 'red';
  canvas.beginPath();
  canvas.ellipse(60, 75, 50, 30, Math.PI * .25, 0, Math.PI * 1.5);
  canvas.fill();

  canvas.fillStyle = 'blue';
  canvas.beginPath();
  canvas.ellipse(150, 75, 50, 30, Math.PI * .25, 0, Math.PI);
  canvas.fill();

  canvas.fillStyle = 'green';
  canvas.beginPath();
  canvas.ellipse(240, 75, 50, 30, Math.PI * .25, 0, Math.PI, true);
  canvas.fill();

  await diffCanvasToFile('data/ellipses.png', 480);
}

export async function fillText() {
  if (window.platform != 'Windows') {
    return;
  }

  const canvas = window.canvas;

  canvas.font = '48px "Times New Roman"';
  canvas.fillStyle = 'white';
  canvas.fillText('Left', 0, 100);
  canvas.textAlign = 'right';
  canvas.fillText('Right', 400, 100);
  canvas.textAlign = 'center';
  canvas.fillText('Center', 200, 200);

  await diffCanvasToFile('data/fill_text.png', 2350);
}

export async function createPatternRepeat() {
  const canvas = window.canvas;
  const image = await File.readImageBitmap(__dirname + '/data/pattern.png');
  const pattern = canvas.createPattern(image, 'repeat');
  canvas.fillStyle = pattern;
  canvas.fillRect(0, 0, 400, 200);
  await diffCanvasToFile('data/create_pattern_repeat.png');
}

export async function createPatternRepeatX() {
  const canvas = window.canvas;
  const image = await File.readImageBitmap(__dirname + '/data/pattern.png');
  const pattern = canvas.createPattern(image, 'repeat-x');
  canvas.fillStyle = pattern;
  canvas.fillRect(0, 0, 400, 200);
  await diffCanvasToFile('data/create_pattern_repeat_x.png');
}

export async function createPatternRepeatY() {
  const canvas = window.canvas;
  const image = await File.readImageBitmap(__dirname + '/data/pattern.png');
  const pattern = canvas.createPattern(image, 'repeat-y');
  canvas.fillStyle = pattern;
  canvas.fillRect(0, 0, 400, 200);
  await diffCanvasToFile('data/create_pattern_repeat_y.png');
}

export async function createPatternRepeatNoRepeat() {
  const canvas = window.canvas;
  const image = await File.readImageBitmap(__dirname + '/data/pattern.png');
  const pattern = canvas.createPattern(image, 'no-repeat');
  canvas.fillStyle = pattern;
  canvas.fillRect(0, 0, 400, 200);
  await diffCanvasToFile('data/create_pattern_no_repeat.png');
}

export async function createPatternFromCanvas() {
  const canvas = window.canvas;

  const off = createCanvas(60, 60);
  off.fillStyle = 'white';
  off.fillRect(0, 0, 60, 60);
  off.fillStyle = 'black';
  off.fillRect(20, 0, 20, 20);
  off.fillRect(40, 20, 20, 20);
  off.fillRect(0, 40, 20, 20);
  off.fillRect(20, 40, 20, 20);
  off.fillRect(40, 40, 20, 20);

  const pattern = canvas.createPattern(unwrapCanvas(off), 'repeat');
  canvas.fillStyle = pattern;
  canvas.fillRect(0, 0, 400, 200);
  await diffCanvasToFile('data/create_pattern_canvas.png');
}

export async function createPatternAndReset() {
  const canvas = window.canvas;
  const image = await File.readImageBitmap(__dirname + '/data/pattern.png');
  const pattern = canvas.createPattern(image, 'repeat');
  canvas.fillStyle = pattern;
  canvas.fillRect(0, 0, 200, 200);
  canvas.fillStyle = 'red';
  canvas.fillRect(200, 0, 200, 200);
  await diffCanvasToFile('data/create_pattern_reset.png');
}

export async function fillPath() {
  const canvas = window.canvas;
  let region = new Path2D();
  region.moveTo(30, 90);
  region.lineTo(110, 20);
  region.lineTo(240, 130);
  region.lineTo(60, 130);
  region.lineTo(190, 20);
  region.lineTo(270, 90);
  region.closePath();
  canvas.fillStyle = 'green';
  canvas.fill(region, 'evenodd');
  await diffCanvasToFile('data/fill_path.png', 550);
}

export async function clipPath() {
  const canvas = window.canvas;
  const circlePath = new Path2D();
  circlePath.arc(150, 75, 75, 0, 2 * Math.PI);
  const squarePath = new Path2D();
  squarePath.rect(85, 10, 130, 130);
  canvas.clip(circlePath);
  canvas.clip(squarePath);
  canvas.fillStyle = 'blue';
  canvas.fillRect(0, 0, canvas.width, canvas.height);
  await diffCanvasToFile('data/clip_path.png', 200);
}

export async function copyPath() {
  const canvas = window.canvas;
  const path1 = new Path2D();
  path1.rect(10, 10, 100, 100);
  const path2 = new Path2D(path1);
  path2.moveTo(220, 60);
  path2.arc(170, 60, 50, 0, 2 * Math.PI);
  canvas.strokeStyle = 'white';
  canvas.stroke(path2);
  await diffCanvasToFile('data/copy_path.png', 1000);
}

export async function svgPath() {
  const canvas = window.canvas;
  const p = new Path2D('M10 10 h 80 v 80 h -80 Z');
  canvas.fillStyle = 'white';
  canvas.fill(p);
  await diffCanvasToFile('data/svg_path.png', 200);
}
