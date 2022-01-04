// From https://p5js.org/examples/math-graphing-2d-equations.html

function setup() {
  createCanvas(710, 400);
  pixelDensity(1);
}

function draw() {
  loadPixels();
  let n = (mouseX * 10.0) / width;
  const w = 16.0; // 2D space width
  const h = 16.0; // 2D space height
  const dx = w / width; // Increment x this amount per pixel
  const dy = h / height; // Increment y this amount per pixel
  let x = -w / 2; // Start x at -1 * width / 2
  let y;

  let r;
  let theta;
  let val;

  let bw; //variable to store grayscale
  let i;
  let j;
  let cols = width;
  let rows = height;

  for (i = 0; i < cols; i += 1) {
    y = -h / 2;
    for (j = 0; j < rows; j += 1) {
      r = sqrt(x * x + y * y); // Convert cartesian to polar
      theta = atan2(y, x); // Convert cartesian to polar
      // Compute 2D polar coordinate function
      val = sin(n * cos(r) + 5 * theta); // Results in a value between -1 and 1
      //var val = cos(r);                            // Another simple function
      //var val = sin(theta);                        // Another simple function
      bw = color(((val + 1) * 255) / 2);
      index = 4 * (i + j * width);
      pixels[index] = red(bw);
      pixels[index + 1] = green(bw);
      pixels[index + 2] = blue(bw);
      pixels[index + 3] = alpha(bw);

      y += dy;
    }
    x += dx;
  }
  updatePixels();
}
