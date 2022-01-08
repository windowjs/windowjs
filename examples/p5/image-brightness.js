// From https://p5js.org/examples/image-brightness.html

// This program adjusts the brightness 
// of a part of the image by 
// calculating the distance of 
// each pixel to the mouse.
let img;
// preload() runs once, before setup()
// loadImage() needs to occur here instead of setup()
// preload() makes sure image is loaded before anything else occurs
function preload() {
  // load the original image
  img = loadImage("assets/rover_wide.jpg");  
}
// setup() runs after preload, once()
function setup() {
  createCanvas(710, 400);
  pixelDensity(1);
  frameRate(30);
}

function draw() {
    image(img,0,0);
    // Only need to load the pixels[] array once, because we're only
    // manipulating pixels[] inside draw(), not drawing shapes.
    loadPixels();
    // We must also call loadPixels() on the PImage since we are going to read its pixels.
    img.loadPixels();
    for (let x = 0; x < img.width; x++) {
        for (let y = 0; y < img.height; y++ ) {
        // Calculate the 1D location from a 2D grid
        let loc = (x + y*img.width)*4;
        // Get the R,G,B values from image
        let r,g,b;
        r = img.pixels[loc];
        // g = img.pixels[loc+1];
        // b = img.pixels[loc+2];
        // Calculate an amount to change brightness based on proximity to the mouse
        // The closer the pixel is to the mouse, the lower the value of "distance"
        let maxdist = 50;//dist(0,0,width,height);
        let d = dist(x, y, mouseX, mouseY);
        let adjustbrightness = 255*(maxdist-d)/maxdist;
        r += adjustbrightness;
        // g += adjustbrightness;
        // b += adjustbrightness;
        // Constrain RGB to make sure they are within 0-255 color range
        r = constrain(r, 0, 255);
        // g = constrain(g, 0, 255);
        // b = constrain(b, 0, 255);
        // Make a new color and set pixel in the window
        let pixloc = (y*width + x)*4;
        pixels[pixloc] = r;
        pixels[pixloc+1] = r;
        pixels[pixloc+2] = r;
        pixels[pixloc+3] = 255; // Always have to set alpha
        }
    }
    updatePixels();
}
