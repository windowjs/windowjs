// The canvas is scaled at draw time to have exactly these dimensions.
// A square tile is exactly 1x1.
const WIDTH = 15;
const HEIGHT = 25;

// A tetris grid is 10x20.
const ROWS = 20;
const COLUMNS = 10;
const grid = new Array(ROWS * COLUMNS).fill(0);

const NPIECES = 7;

// UI positions.
const GRID_OFFSET_X = 1;
const GRID_OFFSET_Y = HEIGHT - ROWS - 1;

const NEXT_PIECE_X = 2 + COLUMNS;
const NEXT_PIECE_Y = HEIGHT - ROWS + 2;
const DRAW_SCALE = 0.92;

// Game state.
const START = 0;
const PLAYING = 1;
const PAUSED = 2;
const CLEARING_ROWS = 3;
const GAME_OVER = 4;
let state = START;
let changeStateTimestamp = 0;

// Current piece. 0 means invalid and not controllable.
let piece = 0;
let rotation = 0;
let x = 0;
let y = 0;
let nextPiece = 0;

// Fall rate. The piece moves one row down every "fallPeriod" milliseconds.
const MAX_FALL_PERIOD = 500;
const MIN_FALL_PERIOD = 50;
let fallPeriod = MAX_FALL_PERIOD;
let nextFallTimestamp = 0;
let completeRowTimestamp = 0;
const CLEAR_ROW_PERIOD = 500;

// Input. Timestamps of 0 means that the key is up.
let leftDownTimestamp = 0;
let rightDownTimestamp = 0;
let downDownTimestamp = 0;
const KEY_REPEAT_DELAY = 150;
const KEY_REPEAT_PERIOD = 1000 / 30;

// Lines cleared. This is used to adjust the falling rate.
let rowsCleared = 0;

let now = 0;
let closePending = false;

const canvas = window.canvas;

function randint(min, max) {
  return Math.floor(Math.random() * (max - min + 1)) + min;
}

function setState(s) {
  state = s;
  changeStateTimestamp = now;
}

function isPlayInputEnabled() {
  return state == PLAYING;
}

function onBlur() {
  leftDownTimestamp = 0;
  rightDownTimestamp = 0;
  downDownTimestamp = 0;
}

function onClose() {
  closePending = true;
  return true;
}

function onKeyDown(event) {
  if (event.repeat) {
    return;
  }
  const key = event.key;
  if (key == 'a' || key == 'ArrowLeft') {
    leftDownTimestamp = now;
    if (isPlayInputEnabled()) {
      moveLeft();
    }
  } else if (key == 'd' || key == 'ArrowRight') {
    rightDownTimestamp = now;
    if (isPlayInputEnabled()) {
      moveRight();
    }
  } else if (key == 'w' || key == 'ArrowUp') {
    if (isPlayInputEnabled()) {
      rotateRight();
    }
  } else if (key == 's' || key == 'ArrowDown') {
    downDownTimestamp = now;
    if (isPlayInputEnabled()) {
      moveDown();
    }
  } else if (key == ' ') {
    if (isPlayInputEnabled()) {
      moveDownCompletely();
    } else if (state == START) {
      startNewLevel();
    }
  } else if (key == 'Escape') {
    if (state == PLAYING) {
      setState(PAUSED);
      closePending = false;
    } else if (state == PAUSED) {
      setState(PLAYING);
      closePending = false;
    } else if (state == GAME_OVER) {
      setState(START);
      closePending = false;
    }
  } else if (key == 'r') {
    if (state == PAUSED || state == GAME_OVER) {
      setState(START);
    }
  }
}

function onKeyUp(event) {
  const key = event.key;
  if (key == 'a' || key == 'ArrowLeft') {
    leftDownTimestamp = 0;
  } else if (key == 'd' || key == 'ArrowRight') {
    rightDownTimestamp = 0;
  } else if (key == 's' || key == 'ArrowDown') {
    downDownTimestamp = 0;
  }
}

function rotateRight() {
  const nextRotation = (rotation + 1) % PIECES[piece].length;
  if (isLegalPosition(x, y, nextRotation)) {
    rotation = nextRotation;
  }
}

function moveLeft() {
  if (isLegalPosition(x - 1, y, rotation)) {
    x -= 1;
  } else {
    leftDownTimestamp = 0;
  }
}

function moveRight() {
  if (isLegalPosition(x + 1, y, rotation)) {
    x += 1;
  } else {
    rightDownTimestamp = 0;
  }
}

function settleCurrentPiece() {
  let gameOver = false;
  for (let i = 0; i < 8; i += 2) {
    const xx = x + PIECES[piece][rotation][i];
    const yy = y + PIECES[piece][rotation][i + 1];
    if (yy < 0) {
      gameOver = true;
    } else {
      grid[xx + yy * COLUMNS] = piece;
    }
  }

  if (gameOver) {
    setState(GAME_OVER);
  } else {
    goToNextPiece();
    if (hasCompleteRows()) {
      setState(CLEARING_ROWS);
      completeRowTimestamp = now;
    } else {
      nextFallTimestamp = now + fallPeriod;
    }
  }
}

function moveDown() {
  if (isLegalPosition(x, y + 1, rotation)) {
    y += 1;
    nextFallTimestamp = now + fallPeriod;
  } else {
    downDownTimestamp = 0;
    settleCurrentPiece();
  }
}

function moveDownCompletely() {
  while (isLegalPosition(x, y + 1, rotation)) {
    y += 1;
  }
  settleCurrentPiece();
}

function isLegalPosition(x, y, rotation) {
  for (let i = 0; i < 8; i += 2) {
    const xx = x + PIECES[piece][rotation][i];
    const yy = y + PIECES[piece][rotation][i + 1];
    // Note: yy < 0 is OK.
    if (xx < 0 || xx >= COLUMNS || yy >= ROWS || grid[xx + yy * COLUMNS] > 0) {
      return false;
    }
  }
  return true;
}

function hasCompleteRows() {
  for (let r = 0; r < ROWS; r++) {
    let complete = true;
    for (let c = 0; c < COLUMNS; c++) {
      if (grid[c + r * COLUMNS] == 0) {
        complete = false;
        break;
      }
    }
    if (complete) {
      return true;
    }
  }
  return false;
}

function clearCompleteRows() {
  let nextToFill = ROWS - 1;
  for (let r = ROWS - 1; r >= 0; r--) {
    let complete = true;
    for (let c = 0; c < COLUMNS; c++) {
      if (grid[c + r * COLUMNS] == 0) {
        complete = false;
        break;
      }
    }
    if (!complete) {
      if (nextToFill != r) {
        for (let c = 0; c < COLUMNS; c++) {
          grid[c + nextToFill * COLUMNS] = grid[c + r * COLUMNS];
        }
      }
      nextToFill--;
    }
  }
  for (let r = nextToFill; r >= 0; r--) {
    for (let c = 0; c < COLUMNS; c++) {
      grid[c + nextToFill * COLUMNS] = 0;
    }
  }
  rowsCleared += nextToFill + 1;
  fallPeriod = Math.max(MAX_FALL_PERIOD - rowsCleared * 5, MIN_FALL_PERIOD);
}

function goToNextPiece() {
  piece = nextPiece;
  rotation = 0;
  nextPiece = randint(1, NPIECES);
  x = 4;
  y = -1;
}

function update() {
  if (closePending) {
    window.close();
  }

  // Handle input. This might change the state.
  if (isPlayInputEnabled()) {
    if (downDownTimestamp > 0 && now > downDownTimestamp + KEY_REPEAT_DELAY) {
      downDownTimestamp = now - KEY_REPEAT_DELAY + KEY_REPEAT_PERIOD;
      moveDown();
    } else if (leftDownTimestamp > 0 &&
               now > leftDownTimestamp + KEY_REPEAT_DELAY) {
      leftDownTimestamp = now - KEY_REPEAT_DELAY + KEY_REPEAT_PERIOD;
      moveLeft();
    } else if (rightDownTimestamp > 0 &&
               now > rightDownTimestamp + KEY_REPEAT_DELAY) {
      rightDownTimestamp = now - KEY_REPEAT_DELAY + KEY_REPEAT_PERIOD;
      moveRight();
    }
  }

  if (state == CLEARING_ROWS && now > completeRowTimestamp + CLEAR_ROW_PERIOD) {
    clearCompleteRows();
    setState(PLAYING);
    nextFallTimestamp = now;
  }

  // Now do updates unrelated to input.
  if (isPlayInputEnabled()) {
    if (nextFallTimestamp == 0) {
      nextFallTimestamp = now + fallPeriod;
    } else if (now >= nextFallTimestamp) {
      nextFallTimestamp = now + fallPeriod;
      moveDown();
    }
  }
}

function startNewLevel() {
  nextPiece = randint(1, NPIECES);
  goToNextPiece();
  setState(PLAYING);
  grid.fill(0);
  nextFallTimestamp = 0;
  fallPeriod = MAX_FALL_PERIOD;
  rowsCleared = 0;
}

function drawBackground() {
  canvas.fillStyle = '#000000';
  canvas.fillRect(0, 0, WIDTH, HEIGHT);
  canvas.fillStyle = '#101010';
  canvas.fillRect(GRID_OFFSET_X, GRID_OFFSET_Y, COLUMNS, ROWS);
}

function drawGrid() {
  for (let row = 0; row < ROWS; row++) {
    for (let column = 0; column < COLUMNS; column++) {
      const type = grid[column + row * COLUMNS];
      if (type > 0) {
        canvas.fillStyle = COLORS[type];
        canvas.fillRect(GRID_OFFSET_X + column, GRID_OFFSET_Y + row, DRAW_SCALE,
                        DRAW_SCALE);
      }
    }
  }
}

function drawCurrentPiece(piece, rotation, x, y) {
  canvas.fillStyle = COLORS[piece];
  for (let i = 0; i < 8; i += 2) {
    const xx = x + PIECES[piece][rotation][i];
    const yy = y + PIECES[piece][rotation][i + 1];
    if (xx >= 0 && xx < COLUMNS && yy >= 0 && yy < ROWS) {
      canvas.fillRect(GRID_OFFSET_X + xx, GRID_OFFSET_Y + yy, DRAW_SCALE,
                      DRAW_SCALE);
    }
  }
}

function drawNextPiece(piece) {
  canvas.fillStyle = COLORS[piece];
  for (let i = 0; i < 8; i += 2) {
    const xx = NEXT_PIECE_X + PIECES[piece][0][i];
    const yy = NEXT_PIECE_Y + PIECES[piece][0][i + 1];
    canvas.fillRect(xx, yy, DRAW_SCALE, DRAW_SCALE);
  }
}

function drawBigText(text) {
  canvas.font = '1px PressStart2P';
  canvas.fillStyle = 'white';
  const w = Math.min(COLUMNS, canvas.measureText(text).width);
  canvas.fillText(text, GRID_OFFSET_X + (COLUMNS - w) / 2,
                  GRID_OFFSET_Y + ROWS / 2 - 2);
}

function drawSmallText(text) {
  canvas.font = '0.5px PressStart2P';
  canvas.fillStyle = 'white';
  const w = Math.min(COLUMNS, canvas.measureText(text).width);
  canvas.fillText(text, GRID_OFFSET_X + (COLUMNS - w) / 2,
                  GRID_OFFSET_Y + ROWS / 2);
}

function draw(drawNow) {
  requestAnimationFrame(draw);

  now = drawNow;

  update();

  canvas.resetTransform();
  const scale = window.width / WIDTH;
  canvas.scale(scale, scale);

  drawBackground();

  if (state != START) {
    drawGrid();
    drawCurrentPiece(piece, rotation, x, y);
    drawNextPiece(nextPiece);
  }

  const blinkOn = (now - changeStateTimestamp) % 1000 < 500;

  if (state == START) {
    drawBigText('TETRIS');
    if (blinkOn) {
      drawSmallText('SPACE to start');
    }
  } else if (state == PAUSED) {
    drawBigText('PAUSED');
    if (blinkOn) {
      drawSmallText('ESC to continue');
    }
  } else if (state == GAME_OVER) {
    drawBigText('GAME OVER');
    if (blinkOn) {
      drawSmallText('ESC to continue');
    }
  }
}

async function init() {
  await window.loadFont('examples/data/PressStart2P.ttf', 'PressStart2P');

  window.addEventListener('keydown', onKeyDown);
  window.addEventListener('keyup', onKeyUp);
  window.addEventListener('blur', onBlur);
  window.addEventListener('close', onClose);

  window.title = 'Tetris';
  window.height = window.screen.height / 2;
  window.width = window.height / HEIGHT * WIDTH;
  window.keepAspectRatio = true;

  requestAnimationFrame(draw);
}

// Each piece is a list of positions, in order of rotation, relative to
// the piece base position at (0, 0). Pieces rotate around (0, 0).
const PIECES = [
  // 0: Empty (to align with COLORS)
  [],

  // 1: Square block.
  [
    [ 0, 0, 1, 0, 0, -1, 1, -1 ],
  ],

  // 2: Bar.
  [
    [ 0, 0, 0, -1, 0, -2, 0, 1 ],
    [ 0, 0, 1, 0, 2, 0, -1, 0 ],
  ],

  // 3: L.
  [
    [ 0, 0, 1, 0, 0, -1, 0, -2 ],
    [ 0, 0, -1, 0, 1, 0, 1, -1 ],
    [ 1, 0, 1, -1, 1, -2, 0, -2 ],
    [ -1, 0, -1, -1, 0, -1, 1, -1 ],
  ],

  // 4: Flipped L.
  [
    [ 1, 0, 0, 0, 1, -1, 1, -2 ],
    [ 2, 0, 2, -1, 1, -1, 0, -1 ],
    [ 0, 0, 0, -1, 0, -2, 1, -2 ],
    [ 1, 0, 2, 0, 0, 0, 0, -1 ],
  ],

  // 5: S.
  [
    [ 0, -2, 0, -1, 1, -1, 1, 0 ],
    [ 0, 0, 1, 0, 1, -1, 2, -1 ],
  ],

  // 6: Flipped S.
  [
    [ 0, 0, 0, -1, 1, -1, 1, -2 ],
    [ 1, 0, 0, 0, 0, -1, -1, -1 ],
  ],

  // 7: Triangle.
  [
    [ 0, 0, 0, -1, 1, -1, 0, -2 ],
    [ -1, 0, 0, 0, 1, 0, 0, -1 ],
    [ 0, 0, -1, -1, 0, -1, 0, -2 ],
    [ -1, -1, 0, -1, 1, -1, 0, 0 ],
  ],
];

const COLORS = [
  '#000000', // 0: black (empty space)
  '#ff0000', // 1: red
  '#00ff00', // 2: green
  '#3040ff', // 3: blue
  '#ffff00', // 4: yellow
  '#ff00ff', // 5: magenta
  '#00a0ff', // 6: cyan
  '#ff8000', // 7: orange
];

await init();
