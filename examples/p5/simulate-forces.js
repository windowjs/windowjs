// From https://p5js.org/examples/simulate-forces.html

// Demonstration of multiple force acting on
// bodies (Mover class)
// Bodies experience gravity continuously
// Bodies experience fluid resistance when in "water"

// Five moving bodies
let movers = [];

// Liquid
let liquid;

function setup() {
  createCanvas(640, 360);
  reset();
  // Create liquid object
  liquid = new Liquid(0, height / 2, width, height / 2, 0.1);
}

function draw() {
  background(127);

  // Draw water
  liquid.display();

  for (let i = 0; i < movers.length; i++) {

    // Is the Mover in the liquid?
    if (liquid.contains(movers[i])) {
      // Calculate drag force
      let dragForce = liquid.calculateDrag(movers[i]);
      // Apply drag force to Mover
      movers[i].applyForce(dragForce);
    }

    // Gravity is scaled by mass here!
    let gravity = createVector(0, 0.1 * movers[i].mass);
    // Apply gravity
    movers[i].applyForce(gravity);

    // Update and display
    movers[i].update();
    movers[i].display();
    movers[i].checkEdges();
  }

}


function mousePressed() {
  reset();
}

// Restart all the Mover objects randomly
function reset() {
  for (let i = 0; i < 9; i++) {
    movers[i] = new Mover(random(0.5, 3), 40 + i * 70, 0);
  }
}

let Liquid = function(x, y, w, h, c) {
  this.x = x;
  this.y = y;
  this.w = w;
  this.h = h;
  this.c = c;
};

// Is the Mover in the Liquid?
Liquid.prototype.contains = function(m) {
  let l = m.position;
  return l.x > this.x && l.x < this.x + this.w &&
         l.y > this.y && l.y < this.y + this.h;
};

// Calculate drag force
Liquid.prototype.calculateDrag = function(m) {
  // Magnitude is coefficient * speed squared
  let speed = m.velocity.mag();
  let dragMagnitude = this.c * speed * speed;

  // Direction is inverse of velocity
  let dragForce = m.velocity.copy();
  dragForce.mult(-1);

  // Scale according to magnitude
  // dragForce.setMag(dragMagnitude);
  dragForce.normalize();
  dragForce.mult(dragMagnitude);
  return dragForce;
};

Liquid.prototype.display = function() {
  noStroke();
  fill(50);
  rect(this.x, this.y, this.w, this.h);
};

function Mover(m, x, y) {
  this.mass = m;
  this.position = createVector(x, y);
  this.velocity = createVector(0, 0);
  this.acceleration = createVector(0, 0);
}

// Newton's 2nd law: F = M * A
// or A = F / M
Mover.prototype.applyForce = function(force) {
  let f = p5.Vector.div(force, this.mass);
  this.acceleration.add(f);
};

Mover.prototype.update = function() {
  // Velocity changes according to acceleration
  this.velocity.add(this.acceleration);
  // position changes by velocity
  this.position.add(this.velocity);
  // We must clear acceleration each frame
  this.acceleration.mult(0);
};

Mover.prototype.display = function() {
  stroke(0);
  strokeWeight(2);
  fill(255,127);
  ellipse(this.position.x, this.position.y, this.mass * 16, this.mass * 16);
};

// Bounce off bottom of window
Mover.prototype.checkEdges = function() {
  if (this.position.y > (height - this.mass * 8)) {
    // A little dampening when hitting the bottom
    this.velocity.y *= -0.9;
    this.position.y = (height - this.mass * 8);
  }
};
