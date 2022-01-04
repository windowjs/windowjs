// From https://p5js.org/examples/image-create-image.html

let img; // Declare variable 'img'.

function setup() {
  createCanvas(720, 400);
  img = createImage(230, 230);
  img.loadPixels();
  for (let x = 0; x < img.width; x++) {
    for (let y = 0; y < img.height; y++) {
      let a = map(y, 0, img.height, 255, 0);
      img.set(x, y, [0, 153, 204, a]);
    }
  }
  img.updatePixels();
}

function draw() {
  background(0);
  image(img, 90, 80);
  image(img, mouseX - img.width / 2, mouseY - img.height / 2);
}
