// From https://p5js.org/examples/hello-p5-simple-shapes.html.

function setup() {
  // Create the canvas
  createCanvas(720, 400);
  background(200);
}

// TODO: originally this was all in the body of setup(),
// but the scaling is off in that case.
function draw() {
  // TODO: the alpha transparency isn't working.
  // Set colors
  fill(204, 101, 192, 127);
  stroke(127, 63, 120);

  // A rectangle
  rect(40, 120, 120, 40);
  // An ellipse
  ellipse(240, 240, 80, 80);
  // A triangle
  triangle(300, 100, 320, 100, 310, 80);

  // A design for a simple flower
  translate(580, 200);
  noStroke();
  for (let i = 0; i < 10; i ++) {
    ellipse(0, 30, 20, 80);
    rotate(PI/5);
  }
}
