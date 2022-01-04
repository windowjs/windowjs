// From https://p5js.org/examples/math-noise2d.html

let noiseVal;
let noiseScale = 0.02;

function setup() {
  createCanvas(640, 360);
}

function draw() {
  background(0);
  // Draw the left half of image
  for (let y = 0; y < height - 30; y++) {
    for (let x = 0; x < width / 2; x++) {
      // noiseDetail of the pixels octave count and falloff value
      noiseDetail(2, 0.2);
      noiseVal = noise((mouseX + x) * noiseScale, (mouseY + y) * noiseScale);
      stroke(noiseVal * 255);
      point(x, y);
    }
  }
  // Draw the right half of image
  for (let y = 0; y < height - 30; y++) {
    for (let x = width / 2; x < width; x++) {
      // noiseDetail of the pixels octave count and falloff value
      noiseDetail(5, 0.5);
      noiseVal = noise((mouseX + x) * noiseScale, (mouseY + y) * noiseScale);
      stroke(noiseVal * 255);
      point(x, y);
    }
  }
  //Show the details of two partitions
  textSize(18);
  fill(255, 255, 255);
  text('Noise2D with 2 octaves and 0.2 falloff', 10, 350);
  text('Noise2D with 1 octaves and 0.7 falloff', 330, 350);
}
