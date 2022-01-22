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
  diffCanvasToFile,
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
