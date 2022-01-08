// From https://p5js.org/examples/image-load-and-display-image.html

let img; // Declare variable 'img'.

function setup() {
  createCanvas(720, 400);
  img = loadImage('assets/moonwalk.jpg'); // Load the image
}

function draw() {
  // Displays the image at its actual size at point (0,0)
  image(img, 0, 0);
  // Displays the image at point (0, height/2) at half size
  image(img, 0, height / 2, img.width / 2, img.height / 2);
}
