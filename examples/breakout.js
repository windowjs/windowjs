const canvas = window.canvas;
canvas.imageSmoothingEnabled = false;

const TYPE_CIRCLE = 0;
const TYPE_SQUARE = 1;

class Particle {
  constructor() {
    // Position (x, y).
    this.x = 0;
    this.y = 0;
    // Velocity (x, y).
    this.vx = 0;
    this.vy = 0;
    // Acceleration (x, y).
    this.ax = 0;
    this.ay = 0;
    // Size from, to.
    this.sf = 0;
    this.st = 0;
    // Age, Life (max age) in milliseconds.
    this.age = 0;
    this.life = 1000;
    // RGBA color from, to.
    this.rf = 0;
    this.gf = 0;
    this.bf = 0;
    this.af = 0;
    this.rt = 0;
    this.gt = 0;
    this.bt = 0;
    this.at = 0;
    this.type = TYPE_CIRCLE;
  }

  reset() {
    this.x = this.y = this.vx = this.vy = this.ax = this.ay = this.sf =
        this.st = this.age = this.rf = this.gf = this.bf = this.af = this.rt =
            this.gt = this.bt = this.at = 0;
    this.type = TYPE_CIRCLE;
    this.life = 1000;
  }

  isAlive() {
    return this.age <= this.life;
  }

  update(dt) {
    this.age += dt;
    if (this.age <= this.life) {
      if (this.ax == 0 && this.ay == 0) {
        this.x += this.vx * dt;
        this.y += this.vy * dt;
      } else {
        const x = this.ax * dt;
        const y = this.ay * dt;
        this.x += (this.vx + 0.5 * x) * dt;
        this.y += (this.vy + 0.5 * y) * dt;
        this.vx += x;
        this.vy += y;
      }
    }
  }

  draw() {
    const at = this.age / this.life;
    const af = 1 - at;
    const s = cubicOut(af);
    const size = s * this.sf + (1 - s) * this.st;
    const r = Math.round(af * this.rf + at * this.rt);
    const g = Math.round(af * this.gf + at * this.gt);
    const b = Math.round(af * this.bf + at * this.bt);
    const a = af * this.af + at * this.at;
    canvas.beginPath();
    canvas.fillStyle = 'rgba(' + r + ', ' + g + ', ' + b + ', ' + a + ')';
    if (this.type == TYPE_CIRCLE) {
      canvas.arc(this.x, this.y, size / 2, 0, Math.PI * 2);
      canvas.fill();
    } else if (this.type == TYPE_SQUARE) {
      canvas.fillRect(this.x - size / 2, this.y - size / 2, size, size);
    }
    canvas.closePath();
  }
}

class ParticleEmitter {
  constructor(maxParticles) {
    this.particles = [];
    this.particles.length = maxParticles;
    for (let i = 0; i < maxParticles; i++) {
      this.particles[i] = new Particle();
    }
    this.alive = 0;
    this.nextOverwrite = 0;
  }

  reset() {
    this.alive = 0;
    this.nextOverwrite = 0;
  }

  addParticle() {
    if (this.alive == this.particles.length) {
      // Full. Overwrite a particle.
      if (this.nextOverwrite >= this.particles.length) {
        this.nextOverwrite = 0;
      }
      const p = this.particles[this.nextOverwrite++];
      p.reset();
      return p;
    } else {
      const p = this.particles[this.alive++];
      p.reset();
      return p;
    }
  }

  update(dt) {
    // Move the next live particle to this position:
    let j = 0;
    for (let i = 0; i < this.alive; i++) {
      const p = this.particles[i];
      p.update(dt);
      if (p.isAlive()) {
        if (i > j) {
          this.particles[i] = this.particles[j];
          this.particles[j] = p;
        }
        j++;
      }
    }
    this.alive = j;
  }

  draw() {
    canvas.save();
    for (let i = 0; i < this.alive; i++) {
      this.particles[i].draw();
    }
    canvas.restore();
  }
}

let levels = [];

const WIDTH = 640;
const HEIGHT = 360;

let ballSpeed = 7;
const ballAngle = -0.25 * Math.PI;
let ballVelocity =
    makeVec(ballSpeed * Math.cos(ballAngle), ballSpeed * Math.sin(ballAngle));

const paddleHeight = 15;
const paddleWidth = 75;
const paddleColor = parseColor('#0288D1');
const paddleGlowColor = parseColor('#0244A0');
let paddleHitTimestamp = -1e6;
let brickHitTimestamp = -1e6;
let paddleX = (WIDTH - paddleWidth) / 2;

const ballRadius = 10;
let x = WIDTH / 2;
let y = HEIGHT - 30;

let rightPressed = false;
let leftPressed = false;

let mouseMoving = false;
let prevMouseX = 0;

// Game states:
const LOADING = 0;
const MENU = 1;
const STARTING_LEVEL = 2;
const PLAYING = 3;
const PAUSE = 4;
const LEVEL_CLEAR = 5;
const LEVEL_LOST = 6;
const WON = 7;
const LOST = 8;
let state = LOADING;

// Brick states:
const LIVE = 0;
const DESTROYED = 1;
const FIXED = 2;

const bricks = [];
const emitter = new ParticleEmitter(10000);

let score = 0;
let combo = 0;
let hintTimestamp = 0;
let timedHint = null;
let timedHintTimestamp = 0;
let destroyedAtLevelRestart = 0;

const CENTERED_TEXT_SIZE = 32;
let centeredText = null;

let lastDraw = 0;

let level = 0;
let lives = 3;

function makeVec(x, y) {
  return {x, y};
}

// Returns the angle of "a" with the x axis, in radians.
// Returns values in [-1, 1].
function vecAngle(a) {
  return Math.atan2(a.y, a.x);
}

// Reflects the vector "a" when hitting a surface with the given "normal".
// "normal" must be normalized.
function reflectVec(a, normal) {
  const dot2 = 2 * (a.x * normal.x + a.y * normal.y);
  return {
    x : a.x - dot2 * normal.x,
    y : a.y - dot2 * normal.y,
  };
}

function parseColor(color) {
  return [
    parseInt(color.substr(1, 2), 16),
    parseInt(color.substr(3, 2), 16),
    parseInt(color.substr(5, 2), 16),
  ];
}

function hex(x) {
  return x.toString(16).padStart(2, '0');
}

function toHex(color) {
  return '#' + hex(color[0]) + hex(color[1]) + hex(color[2]);
}

function lerpColor(a, b, t) {
  const t1 = 1 - t;
  return [
    Math.round(t1 * a[0] + t * b[0]),
    Math.round(t1 * a[1] + t * b[1]),
    Math.round(t1 * a[2] + t * b[2]),
  ];
}

function uniformFloat(from, to) {
  return Math.random() * (to - from) + from;
}

function loadLevel(level) {
  const data = levels[level];
  const colors = data.colors;
  const sizes = data.sizes;
  bricks.length = 0;
  for (let b = 0; b < data.bricks.length; b++) {
    const brick = data.bricks[b];
    const color = colors[brick.color || 0];
    const size = sizes[brick.size || 0];
    let rx = brick['repeat-x'] || 1;
    let ry = brick['repeat-y'] || 1;
    let sx = brick['step-x'] || 0;
    let sy = brick['step-y'] || 0;
    let y = brick.y;
    for (let iy = 0; iy < ry; iy++) {
      let x = brick.x;
      for (let ix = 0; ix < rx; ix++) {
        bricks.push({
          x : x,
          y : y,
          w : size[0],
          h : size[1],
          state : brick.fixed ? FIXED : LIVE,
          color : color,
        });
        x += sx;
      }
      y += sy;
    }
  }
}

function resetHintTimestamp(delay) {
  hintTimestamp = performance.now() + (delay || 0);
}

function resetPlayer() {
  paddleX = (WIDTH - paddleWidth) / 2;
  x = paddleX + paddleWidth / 2;
  y = HEIGHT - paddleHeight - ballRadius;
  ballSpeed = levels[level].startSpeed;
  ballVelocity.x = ballSpeed * Math.cos(ballAngle);
  ballVelocity.y = ballSpeed * Math.sin(ballAngle);
  combo = 0;
  rightPressed = false;
  leftPressed = false;
  hintTimestamp = 0;
  timedHint = null;
  timedHintTimestamp = 0;
  destroyedAtLevelRestart =
      bricks.reduce((a, b) => a + (b.state == DESTROYED ? 1 : 0), 0);
}

function resetLevel() {
  loadLevel(level);
  resetPlayer();
}

function resetMenu() {
  state = MENU;
  level = 0;
  lives = 3;
  score = 0;
  resetHintTimestamp(1000);
  resetLevel();
}

function destroyBrick(brick, normalx, normaly) {
  brick.state = DESTROYED;
  const XX = brick.w / 5;
  const YY = brick.h / 10;
  const w = brick.w / (XX + 1);
  const h = brick.h / (YY + 1);
  for (let c = 1; c <= XX; c++) {
    for (let r = 1; r <= YY; r++) {
      const p = emitter.addParticle();
      const speedx = uniformFloat(0.1, 0.2);
      const speedy = uniformFloat(0.5, 0.6);
      if (normalx == 0) {
        if (normaly > 0) {
          var angle = uniformFloat(0, 1) * Math.PI;
        } else {
          var angle = uniformFloat(1, 2) * Math.PI;
        }
      } else {
        if (normalx > 0) {
          var angle = uniformFloat(-0.5, 0.5) * Math.PI;
        } else {
          var angle = uniformFloat(0.5, 1.5) * Math.PI;
        }
      }
      p.x = brick.x + c * w;
      p.y = brick.y + r * h;
      p.vx = speedx * Math.cos(angle);
      p.vy = speedy * Math.sin(angle);
      p.sf = uniformFloat(2 * w, 4 * w);
      [p.rf, p.gf, p.bf] = [ p.rt, p.gt, p.bt ] = parseColor(brick.color);
      p.af = p.at = 1;
      p.life = 800;
      p.type = TYPE_SQUARE;
      p.ay = 0.002;
    }
  }
}

function destroyPaddle() {
  const XX = 15;
  const YY = 2;
  const w = paddleWidth / (XX + 1);
  const h = paddleHeight / (YY + 1);
  for (let c = 1; c <= XX; c++) {
    for (let r = 1; r <= YY; r++) {
      const p = emitter.addParticle();
      let speedx = uniformFloat(0.1, 0.2);
      const speedy = uniformFloat(0.8, 1);
      if (leftPressed) {
        var angle = uniformFloat(1.2, 1.6) * Math.PI;
        speedx *= 2;
      } else if (rightPressed) {
        var angle = uniformFloat(1.4, 1.8) * Math.PI;
        speedx *= 2;
      } else {
        var angle = uniformFloat(1.2, 1.8) * Math.PI;
      }
      p.x = paddleX + c * w;
      p.y = HEIGHT - paddleHeight + r * h;
      p.vx = speedx * Math.cos(angle);
      p.vy = speedy * Math.sin(angle);
      p.sf = uniformFloat(3 * w, 4 * w);
      [p.rf, p.gf, p.bf] = [ p.rt, p.gt, p.bt ] = paddleColor;
      p.af = p.at = 1;
      p.life = 1000;
      p.type = TYPE_SQUARE;
      p.ay = 0.002;
    }
  }
}

function adjustBallSpeed() {
  const angle = vecAngle(ballVelocity);
  const start = levels[level].startSpeed;
  const end = levels[level].endSpeed;
  const destroyed =
      bricks.reduce((a, b) => a + (b.state == DESTROYED ? 1 : 0), 0) -
      destroyedAtLevelRestart;
  ballSpeed = start + (end - start) * ((destroyed + 1) / bricks.length);
  ballVelocity.x = ballSpeed * Math.cos(angle);
  ballVelocity.y = ballSpeed * Math.sin(angle);
}

function rangeIntersects(begina, enda, beginb, endb) {
  return enda > beginb && endb > begina;
}

// Sweep AABB when "mover" is moving at (vx, vy) relative to "obstacle".
// "mover" and "obstacle" are rectangles.
//
// Returns [ratio, normal].
//
// "ratio" is negative if the "mover" is already colliding with the obstacle.
// "ratio" is 1 if the mover can move by (vx, vy) without colliding.
//
// Otherwise, "ratio" is in [0, 1) and indicates by how much of (vx, vy) can
// the "mover" move until it gets in contact with the "obstacle".
//
// In that case, "normal" is the normal of the collision surface.
function sweepAABB(mover, vx, vy, obstacle) {
  if (vx == 0) {
    // If there is no x intersection now then it will never happen, because
    // there is no vx.
    if (rangeIntersects(mover.x, mover.x + mover.w, obstacle.x,
                        obstacle.x + obstacle.w)) {
      // May intersect, if the Y axis intersects.
      var entryx = -100;
      var exitx = 100;
    } else {
      // No collision.
      return [ 1, {x : 0, y : 0} ];
    }
  } else if (vx > 0) {
    // Moving left. The X axis starts intersecting as soon as the mover's
    // right edge touches the obstacle's left edge. The intersection stops
    // once the opposite edges touch as well.
    var entryx = (obstacle.x - (mover.x + mover.w)) / vx;
    var exitx = ((obstacle.x + obstacle.w) - mover.x) / vx;
  } else {
    // Some as for vx > 0, but with the edges swapped since we're moving
    // in the opposite direction.
    var entryx = ((obstacle.x + obstacle.w) - mover.x) / vx;
    var exitx = (obstacle.x - (mover.x + mover.w)) / vx;
  }

  if (vy == 0) {
    // Same as for vx: if there is no vy then the y ranges must overlap now.
    if (rangeIntersects(mover.y, mover.y + mover.h, obstacle.y,
                        obstacle.y + obstacle.h)) {
      var entryy = -100;
      var exity = 100;
    } else {
      // No collision.
      return [ 1, {x : 0, y : 0} ];
    }
  } else if (vy > 0) {
    var entryy = (obstacle.y - (mover.y + mover.h)) / vy;
    var exity = ((obstacle.y + obstacle.h) - mover.y) / vy;
  } else {
    var entryy = ((obstacle.y + obstacle.h) - mover.y) / vy;
    var exity = (obstacle.y - (mover.y + mover.h)) / vy;
  }

  // Now the actual entry time is *after* *both* X and Y axis start
  // intersecting. The exit time is *after* *one* of X or Y stop
  // intersecting.
  const entry = Math.max(entryx, entryy);
  const exit = Math.min(exitx, exity);

  // We get a collision if entry < exit and the entry time is in [0, 1].
  if (entry > exit || entry < 0 || entry > 1) {
    // No collision.
    if (entry < 0 && exit > 0) {
      // Already inside! Return the (negative) time to move just enough to
      // stop colliding.
      return [ -exit, {x : 0, y : 0} ];
    }
    return [ 1, {x : 0, y : 0} ];
  }

  // Will start colliding at "entry" time.
  // Find the collision normal now.
  if (entryx > entryy) {
    // The last axis to intersect is X, so this is going in the X direction.
    var normalx = vx > 0 ? -1 : 1;
    var normaly = 0;
  } else {
    var normalx = 0;
    var normaly = vy > 0 ? -1 : 1;
  }
  return [ entry, {x : normalx, y : normaly} ];
}

function makeRect(x, y, w, h) {
  return {x, y, w, h};
}

function detectCollisions() {
  const ballRect =
      makeRect(x - ballRadius, y - ballRadius, ballRadius * 2, ballRadius * 2);
  let minRatio = 1;
  let minNormal = null;
  let minIndex = -1;
  for (let i = 0; i < bricks.length; i++) {
    const brick = bricks[i];
    if (brick.state == DESTROYED) continue;
    let [ratio, normal] =
        sweepAABB(ballRect, ballVelocity.x, ballVelocity.y, brick);
    if (ratio > 0 && ratio < minRatio) {
      minRatio = ratio;
      minNormal = normal;
      minIndex = i;
    } else if (ratio < 0) {
      // This can happen after reflecting on a wall and moving forward by
      // ballVelocity without checking for collisions before.
      minRatio = 0;
      minNormal = normal;
      minIndex = i;
    }
  }

  if (minIndex < 0) {
    return;
  }

  const doCollision = function() {
    const brick = bricks[minIndex];
    if (brick.state == LIVE) {
      destroyBrick(brick, -minNormal.x, -minNormal.y);
      brickHitTimestamp = performance.now();
      combo++;
      score += 100 * combo;
      if (combo > 1) {
        resetTimedHint(combo + 'X combo!');
      }
    }
    if (minNormal.x == 0) {
      ballVelocity.y = -ballVelocity.y;
    } else {
      ballVelocity.x = -ballVelocity.x;
    }
    adjustBallSpeed();
  };

  doCollision();
}

function destroyBricks(destroy, destroyFixed) {
  for (let i = 0; i < bricks.length; i++) {
    const brick = bricks[i];
    if (brick.state == LIVE) {
      if (destroy) {
        destroyBrick(brick, 0, -1);
      } else {
        destroy = true;
      }
    } else if (brick.state == FIXED && destroyFixed) {
      destroyBrick(brick, 0, -1);
    }
  }
}

function destroyAllBricksButOne() {
  destroyBricks(false, false);
}

function destroyAllBricks() {
  destroyBricks(true, true);
}

function reflectBallOnPaddle() {
  const pos = (x - paddleX) / paddleWidth - 0.5;
  const normalAngle = -Math.PI / 2 + pos;
  const normal = makeVec(Math.cos(normalAngle), Math.sin(normalAngle));
  ballVelocity = reflectVec(ballVelocity, normal);
  if (ballVelocity.y > -2.5) {
    ballVelocity.y = -2.5;
    const angle = vecAngle(ballVelocity);
    ballVelocity.x = ballSpeed * Math.cos(angle);
    ballVelocity.y = ballSpeed * Math.sin(angle);
  }
}

function explodeCenteredText() {
  if (!centeredText) {
    return;
  }
  const w = getTextWidth(centeredText, CENTERED_TEXT_SIZE);
  const x = (WIDTH - w) / 2;
  const y = HEIGHT / 2 - CENTERED_TEXT_SIZE;
  const XX = 2 * centeredText.length;
  const YY = 4;
  const stepw = w / (XX + 1);
  const steph = CENTERED_TEXT_SIZE / (YY + 1);
  for (let c = 1; c <= XX; c++) {
    const k = Math.floor((c - 1) / 2);
    if (centeredText[k] == ' ') continue;
    for (let r = 1; r <= YY; r++) {
      const p = emitter.addParticle();
      p.x = x + c * stepw;
      p.y = y + r * steph;
      p.vy = -uniformFloat(0.2, 0.4);
      p.ay = 0.002;
      p.sf = 10;
      [p.rf, p.gf, p.bf] = [ p.rt, p.gt, p.bt ] = [ 255, 255, 255 ];
      p.af = p.at = 1;
      p.life = 500;
      p.type = TYPE_SQUARE;
    }
  }
}

function hasWon() {
  return bricks.every(b => b.state == DESTROYED || b.state == FIXED);
}

function onKeyDownInternal(event) {
  const key = event.key;

  if (key == 'Escape') {
    resetMenu();
    return;
  }

  if (key == 'v') {
    window.vsync = !window.vsync;
    return;
  }

  if (state == PAUSE || state == STARTING_LEVEL || state == MENU) {
    if (key == '1' || key == '2' || key == '3' || key == '4' || key == '5' ||
        key == '6' || key == '7' || key == '8' || key == '9') {
      const n = parseInt(key) - 1;
      if (n < levels.length) {
        level = n;
        resetLevel();
        state = STARTING_LEVEL;
      }
    }
  }

  if (state == PLAYING || state == STARTING_LEVEL) {
    if (key == 'ArrowRight') {
      rightPressed = true;
    } else if (key == 'ArrowLeft') {
      leftPressed = true;
    }
    if (state == PLAYING) {
      if (key == ' ') {
        state = PAUSE;
      }
    } else if (state == STARTING_LEVEL) {
      if (key == ' ') {
        explodeCenteredText();
        state = PLAYING;
      }
    }
  } else if (state == PAUSE) {
    if (key == ' ') {
      state = PLAYING;
      leftPressed = false;
      rightPressed = false;
      explodeCenteredText();
    } else if (key == 't') {
      destroyAllBricksButOne();
    }
  } else if (state == WON || state == LOST) {
    if (key == ' ') {
      explodeCenteredText();
      resetMenu();
    }
  } else if (state == MENU) {
    if (key == ' ') {
      state = STARTING_LEVEL;
      explodeCenteredText();
    }
  } else if (state == LEVEL_CLEAR) {
    if (key == ' ') {
      explodeCenteredText();
      resetLevel();
      state = STARTING_LEVEL;
    }
  } else if (state == LEVEL_LOST) {
    if (key == ' ') {
      explodeCenteredText();
      resetPlayer();
      state = STARTING_LEVEL;
    }
  }
}

function onKeyDown(event) {
  const prevState = state;
  onKeyDownInternal(event);
  if (state != prevState) {
    resetHintTimestamp();
  }
}

function onKeyUp(event) {
  const key = event.key;
  if (state == PLAYING || state == STARTING_LEVEL) {
    if (key == 'ArrowRight') {
      rightPressed = false;
    } else if (key == 'ArrowLeft') {
      leftPressed = false;
    }
  }
}

function onMouseMove(event) {
  if (window.cursor != 'locked') {
    return;
  }

  const dx = event.x - prevMouseX;
  prevMouseX = event.x;

  if (!mouseMoving) {
    mouseMoving = true;
    return;
  }

  if (state == PLAYING || state == STARTING_LEVEL) {
    paddleX += dx / ((window.width / WIDTH) * 1);
    if (paddleX < 0) {
      paddleX = 0;
    }
    if (paddleX + paddleWidth > WIDTH) {
      paddleX = WIDTH - paddleWidth;
    }
  }
}

function onClick(event) {
  if (event.button == 0) {
    if (window.cursor == 'locked') {
      if (state == PLAYING) {
        state = PAUSE;
      } else if (state == STARTING_LEVEL) {
        state = PLAYING;
        explodeCenteredText();
      } else if (state == PAUSE) {
        state = PLAYING;
        explodeCenteredText();
      } else if (state == WON || state == LOST) {
        explodeCenteredText();
        resetMenu();
      } else if (state == MENU) {
        state = STARTING_LEVEL;
        explodeCenteredText();
      } else if (state == LEVEL_CLEAR) {
        explodeCenteredText();
        resetLevel();
        state = STARTING_LEVEL;
      } else if (state == LEVEL_LOST) {
        explodeCenteredText();
        resetPlayer();
        state = STARTING_LEVEL;
      }
    } else {
      window.cursor = 'locked';
    }
  } else if (event.button == 2) {
    window.cursor = 'arrow';
    mouseMoving = false;
  }
}

function cubicOut(t) {
  return --t * t * t + 1;
}

function drawPaddle(timestamp) {
  const duration = 400;
  const elapsed = timestamp - paddleHitTimestamp;
  if (elapsed > duration) {
    var c = paddleColor;
  } else {
    var t = elapsed / duration;
    if (t < 0.2) {
      t = cubicOut(t * 5);
      c = lerpColor(paddleColor, paddleGlowColor, t);
    } else {
      t = cubicOut((t - 0.2) / 0.8);
      c = lerpColor(paddleGlowColor, paddleColor, t);
    }
  }

  canvas.beginPath();
  canvas.rect(paddleX, HEIGHT - paddleHeight, paddleWidth, paddleHeight);
  canvas.fillStyle = toHex(c);
  canvas.fill();
  canvas.closePath();
}

function drawBall(emit) {
  const COLOR = '#fcd737';
  const FAST_COLOR = '#ff2700';
  canvas.beginPath();
  canvas.arc(x, y, ballRadius, 0, Math.PI * 2);
  canvas.fillStyle = COLOR;
  canvas.fill();
  canvas.closePath();

  if (emit) {
    const emitsPerFrame = 2;
    const colorFrom = parseColor(COLOR);
    const colorTo = parseColor(FAST_COLOR);
    const startSpeed = levels[level].startSpeed;
    const endSpeed = levels[level].endSpeed;
    if (startSpeed >= endSpeed) {
      var a = 1;
    } else {
      var a = (ballSpeed - startSpeed) / (endSpeed - startSpeed);
    }
    for (let i = 0; i < emitsPerFrame; i++) {
      const p = emitter.addParticle();
      p.x = x;
      p.y = y;
      p.vx = -uniformFloat(0.7, 1.3) * ballVelocity.x * 0.01;
      p.vy = -uniformFloat(0.7, 1.3) * ballVelocity.y * 0.01;
      p.sf = ballRadius * 2;
      p.st = ballRadius / 2;
      [p.rf, p.gf, p.bf] = colorFrom;
      [p.rt, p.gt, p.bt] = colorTo;
      p.af = 1;
      p.at = 0;
      p.life = 300 + 300 * a;
      p.type = TYPE_CIRCLE;
    }
  }
}

function drawBricks() {
  for (let i = 0; i < bricks.length; i++) {
    const brick = bricks[i];
    if (brick.state == DESTROYED) {
      continue;
    }
    canvas.beginPath();
    canvas.rect(brick.x, brick.y, brick.w, brick.h);
    canvas.fillStyle = brick.color;
    canvas.fill();
    canvas.closePath();
  }
}

function getTextWidth(t, size) {
  canvas.font = size + 'px PressStart2P';
  return Math.floor(canvas.measureText(t).width);
}

function drawText(t, size, x, y) {
  canvas.font = size + 'px PressStart2P';
  canvas.fillStyle = 'white';
  canvas.fillText(t, x, y);
}

function drawHint(t) {
  const s = 16;
  const w = getTextWidth(t, s);
  drawText(t, s, 4, 20);
}

function drawBlinkingHint(t) {
  timedHint = null;
  timedHintTimestamp = 0;
  const elapsed = performance.now() - hintTimestamp;
  if (elapsed > 0 && elapsed % 1000 < 500) {
    drawHint(t);
  }
}

function drawTimedHint() {
  if (timedHintTimestamp) {
    if (performance.now() > timedHintTimestamp + 1000) {
      timedHint = null;
      timedHintTimestamp = 0;
    } else {
      drawHint(timedHint);
    }
  }
}

function resetTimedHint(t) {
  timedHint = t;
  timedHintTimestamp = performance.now();
}

function drawScore() {
  const t = 'SCORE ' + score.toString().padStart(8, '0');
  const s = 16;
  const w = getTextWidth(t, s);
  drawText(t, s, WIDTH - w - 4, 20);
}

function drawCenteredText(t) {
  const w = getTextWidth(t, CENTERED_TEXT_SIZE);
  centeredText = t;
  drawText(t, CENTERED_TEXT_SIZE, (WIDTH - w) / 2, HEIGHT / 2);
}

function drawLevelClear() {
  drawCenteredText('CLEAR!');
  drawBlinkingHint('Space to continue');
}

function drawLevelLost() {
  drawCenteredText('MISS!');
  drawBlinkingHint('Space to retry');
  const t2 = lives + ' retries left';
  const s2 = 16;
  const w2 = getTextWidth(t2, s2);
  drawText(t2, s2, (WIDTH - w2) / 2, HEIGHT / 2 + 30);
}

function drawWon() {
  drawCenteredText('YOU WON!');
  drawBlinkingHint('The galaxy is safe again!');
}

function drawLost() {
  drawCenteredText('GAME OVER');
  drawBlinkingHint('The galaxy was destroyed!');
}

function drawPause() {
  drawCenteredText('PAUSE');
  drawBlinkingHint('Space to resume');
}

function drawMenu() {
  drawCenteredText('BRICK DESTROYER');
  drawBlinkingHint('Space to start');
  const t2 = 'Arrows or mouse to move';
  const s2 = 10;
  const w2 = getTextWidth(t2, s2);
  drawText(t2, s2, (WIDTH - w2) / 2, HEIGHT - 28);
  const t3 = lives + ' lives, ' + levels.length + ' levels';
  const s3 = 10;
  const w3 = getTextWidth(t3, s3);
  drawText(t3, s3, (WIDTH - w3) / 2, HEIGHT - 10);
}

function drawLoading() {
  drawCenteredText('LOADING');
}

function drawStartingLevel() {
  drawCenteredText('LEVEL ' + (level + 1));
  drawBlinkingHint('Space to shoot');
}

function shakeScreen(timestamp) {
  if (timestamp - brickHitTimestamp < 100) {
    canvas.translate(WIDTH / 2, HEIGHT / 2);
    canvas.rotate(uniformFloat(-0.05, 0.05));
    canvas.translate(-WIDTH / 2, -HEIGHT / 2);
  }
}

function draw(timestamp) {
  requestAnimationFrame(draw);

  if (lastDraw == 0) {
    var delta = 0;
  } else {
    var delta = timestamp - lastDraw;
  }
  lastDraw = timestamp;

  // "dt" is the time scaling factor. It should be 1 at 60 FPS, 0.5 at 120, etc.
  const dt = 60 * delta / 1000;

  // Input.
  if (state == PLAYING || state == LOST || state == LEVEL_LOST) {
    if (x + dt*ballVelocity.x > WIDTH - ballRadius ||
        x + dt*ballVelocity.x < ballRadius) {
      ballVelocity.x = -ballVelocity.x;
    }
    if (y + dt*ballVelocity.y <= ballRadius) {
      ballVelocity.y = -ballVelocity.y;
    } else if (y + dt*ballVelocity.y > HEIGHT - ballRadius) {
      if (state == PLAYING) {
        lives--;
        state = lives <= 0 ? LOST : LEVEL_LOST;
        if (state == LOST) {
          destroyAllBricks();
        }
        destroyPaddle();
        resetHintTimestamp(1000);
      }
    } else if (y + dt*ballVelocity.y > HEIGHT - ballRadius - paddleHeight &&
               (x >= paddleX && x <= paddleX + paddleWidth)) {
      combo = 0;
      reflectBallOnPaddle();
      paddleHitTimestamp = timestamp;
    }

    x += dt*ballVelocity.x;
    y += dt*ballVelocity.y;
  }

  if (state == PLAYING || state == STARTING_LEVEL) {
    if (rightPressed && paddleX < WIDTH - paddleWidth) {
      paddleX += 10 * dt;
    } else if (leftPressed && paddleX > 0) {
      paddleX -= 10 * dt;
    }

    if (state == PLAYING) {
      detectCollisions();
    } else if (state == STARTING_LEVEL) {
      x = paddleX + paddleWidth / 2;
    }
  }

  emitter.update(delta);

  if (state == PLAYING && hasWon()) {
    if (level + 1 < levels.length) {
      level++;
      state = LEVEL_CLEAR;
    } else {
      state = WON;
    }
    destroyAllBricks();
    resetHintTimestamp(1000);
  }

  // Draw.
  canvas.resetTransform();
  const scale = window.width / WIDTH;
  canvas.scale(scale, scale);

  canvas.clearRect(0, 0, WIDTH, HEIGHT);

  shakeScreen(timestamp);

  if (state == PLAYING || state == PAUSE || state == WON || state == LOST ||
      state == STARTING_LEVEL || state == LEVEL_LOST) {
    if (state != LOST && state != LEVEL_LOST) {
      drawPaddle(timestamp);
    }
    drawBricks();
  }

  emitter.draw();

  if (state == PLAYING || state == PAUSE || state == LOST ||
      state == LEVEL_LOST || state == STARTING_LEVEL) {
    const emit = state != STARTING_LEVEL && state != PAUSE;
    drawBall(emit);
  }

  drawScore();

  if (state == STARTING_LEVEL) {
    drawStartingLevel();
  } else if (state == LEVEL_CLEAR) {
    drawLevelClear();
  } else if (state == LEVEL_LOST) {
    drawLevelLost();
  } else if (state == WON) {
    drawWon();
  } else if (state == LOST) {
    drawLost();
  } else if (state == PAUSE) {
    drawPause();
  } else if (state == LOADING) {
    drawLoading();
  } else if (state == MENU) {
    drawMenu();
  }
  drawTimedHint();
}

async function init() {
  const assets = await Promise.all([
    File.readJSON('examples/data/breakout-levels.json'),
    window.loadFont('examples/data/PressStart2P.ttf', 'PressStart2P'),
  ]);

  levels = assets[0];

  window.title = 'Brick Destroyer';
  window.width = window.screen.width / 2;
  window.height = window.width / WIDTH * HEIGHT;
  window.keepAspectRatio = true;

  window.addEventListener('keydown', onKeyDown);
  window.addEventListener('keyup', onKeyUp);
  window.addEventListener('mousemove', onMouseMove);
  window.addEventListener('click', onClick);

  resetMenu();
  requestAnimationFrame(draw);
}

await init();
