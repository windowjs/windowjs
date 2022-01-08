function setup() {
  createCanvas(400, 400);
  background(220);
  fill(255, 204, 0);
  ellipse(50, 50, 80, 80);
}

function draw() {  
  fill(255, 204, 0);
  ellipse(50, 50, 80, 80);
  fill('red');
  ellipse(350, 350, 80, 80);
  fill('dodgerblue');
  ellipse(50, 350, 80, 80);
  fill('lime');
  ellipse(350, 50, 80, 80);
  if (mouseIsPressed) {
    fill('black');
  } else {
    fill('white');
  }
  ellipse(mouseX, mouseY, 80, 80);
}
