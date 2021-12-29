const WIDTH = 16;
const HEIGHT = 10;

// States.
const PLAYING = 0;
const SOLVED = 1;

class LevelState {
  constructor() {
    this.level = [];
    this.playerX = 0;
    this.playerY = 0;
    this.goalsLeft = 0;
    this.steps = 0;
    this.pushes = 0;
    this.lookingRight = true;
    this.previousState = null;
  }

  clone() {
    const copy = new LevelState();
    copy.playerX = this.playerX;
    copy.playerY = this.playerY;
    copy.goalsLeft = this.goalsLeft;
    copy.steps = this.steps;
    copy.pushes = this.pushes;
    copy.lookingRight = this.lookingRight;
    copy.previousState = this.previousState;
    copy.level = [];
    for (const row of this.level) {
      copy.level.push(row.slice());
    }
    return copy;
  }

  loadLevel(spec) {
    this.goalsLeft = 0;
    this.steps = 0;
    this.pushes = 0;
    this.level = [];
    this.previousState = null;
    for (let y = 0; y < spec.length; y++) {
      this.level.push(spec[y].split(''));
      for (let x = 0; x < this.level[y].length; x++) {
        if (this.level[y][x] == 'p' || this.level[y][x] == 'P') {
          this.playerX = x;
          this.playerY = y;
          if (this.level[y][x] == 'P') {
            this.level[y][x] = 'o';
            this.goalsLeft++;
          } else {
            this.level[y][x] = ' ';
          }
        } else if (this.level[y][x] == 'o') {
          this.goalsLeft++;
        }
      }
    }
  }

  isValid(x, y) {
    return y >= 0 && y < this.level.length && x >= 0 &&
           x < this.level[y].length;
  }

  isType(x, y, types) {
    return this.isValid(x, y) && types.includes(this.level[y][x]);
  }

  isEmpty(x, y) {
    return this.isType(x, y, ' o');
  }

  isBox(x, y) {
    return this.isType(x, y, 'bB');
  }

  move(dx, dy) {
    const level = this.level;
    let px = this.playerX;
    let py = this.playerY;

    if (this.isEmpty(px + dx, py + dy)) {
      this.previousState = this.clone();
      this.steps++;
      this.playerX += dx;
      this.playerY += dy;
    } else if (this.isBox(px + dx, py + dy) &&
               this.isEmpty(px + 2 * dx, py + 2 * dy)) {
      this.previousState = this.clone();
      this.pushes++;
      this.playerX += dx;
      this.playerY += dy;
      px = this.playerX;
      py = this.playerY;

      if (level[py][px] == 'B') {
        this.goalsLeft++;
        level[py][px] = 'o';
      } else {
        level[py][px] = ' ';
      }

      if (level[py + dy][px + dx] == 'o') {
        this.goalsLeft--
        level[py + dy][px + dx] = 'B';
      } else {
        level[py + dy][px + dx] = 'b';
      }
    } else {
      return;
    }

    if (dx != 0) {
      this.lookingRight = dx > 0;
    }
  }
}

const canvas = window.canvas;

let character = null;
let tilesheet = null;

let state = PLAYING;
let blink = true;
let levelIndex = 0;
let levelState = new LevelState();

function loadLevel(index) {
  levelIndex = index;
  state = PLAYING;
  blink = true;
  levelState.loadLevel(LEVELS[index]);
}

function drawTile(sheet, size, tx, ty, dx, dy) {
  canvas.drawImage(sheet, tx * size + 1, ty * size + 1, size - 2, size - 2, dx,
                   dy, 1, 1);
}

function drawTileByType(type, dx, dy) {
  if (type == '#') {
    drawTile(tilesheet, 128, 3, 4, dx, dy);
  }

  if (type == 'b' || type == 'B') {
    drawTile(tilesheet, 128, 7, 1, dx, dy);
  }

  if (type == 'o' || type == 'B') {
    drawTile(tilesheet, 128, 9, 0, dx, dy);
  }
}

function drawBackground(type, dx, dy) {
  const fudge = 0.05;
  if (type == ' ' || type == 'o' || type == 'b' || type == 'B') {

    canvas.fillRect(dx - fudge, dy - fudge, 1 + 2 * fudge, 1 + 2 * fudge);
  }
}

function draw() {
  canvas.resetTransform();
  const scale = window.width / WIDTH;
  canvas.scale(scale, scale);

  canvas.fillStyle = '#181f30';
  canvas.fillRect(0, 0, WIDTH, HEIGHT);

  const level = levelState.level;
  const levelWidth = level[0].length;
  const levelHeight = level.length;
  const offX = (WIDTH - levelWidth) / 2;
  const offY = (HEIGHT - levelHeight) / 2;

  canvas.fillStyle = '#333';
  for (let y = 0; y < levelHeight; y++) {
    for (let x = 0; x < levelWidth; x++) {
      drawBackground(level[y][x], offX + x, offY + y);
    }
  }

  for (let y = 0; y < levelHeight; y++) {
    for (let x = 0; x < levelWidth; x++) {
      drawTileByType(level[y][x], offX + x, offY + y);
    }
  }

  if (state == SOLVED) {
    drawTile(character, 192, 3, 1, offX + levelState.playerX,
             offY + levelState.playerY);
  } else if (levelState.lookingRight) {
    drawTile(character, 192, 0, 0, offX + levelState.playerX,
             offY + levelState.playerY);
  } else {
    canvas.save();
    canvas.translate(offX + levelState.playerX + 1, offY + levelState.playerY);
    canvas.scale(-1, 1);
    drawTile(character, 192, 0, 0, 0, 0);
    canvas.restore();
  }

  canvas.fillStyle = 'white';
  canvas.font = '0.3px PressStart2P';
  canvas.textAlign = 'right';
  canvas.fillText(`Level ${levelIndex + 1} / ${LEVELS.length}`, 15.9, 0.5);
  canvas.fillText(`Steps ${levelState.steps} / ${levelState.pushes}`, 15.9, 1);

  if (state == SOLVED) {
    canvas.textAlign = 'center';
    canvas.fillText('Solved!', 8, 9);
    if (blink) {
      canvas.fillText('Press SPACE to continue', 8, 9.8);
    }
    blink = !blink;
    setTimeout(draw, 600);
  }
}

function onKeyDown(event) {
  const key = event.key;
  let dx = 0;
  let dy = 0;

  if (state == PLAYING) {
    if (key == 'ArrowLeft') {
      levelState.move(-1, 0);
    } else if (key == 'ArrowRight') {
      levelState.move(1, 0);
    } else if (key == 'ArrowUp') {
      levelState.move(0, -1);
    } else if (key == 'ArrowDown') {
      levelState.move(0, 1);
    } else if (key == 'Backspace') {
      if (levelState.previousState) {
        levelState = levelState.previousState;
      }
    }

    if (levelState.goalsLeft == 0) {
      state = SOLVED;
    }

    draw();
  }

  if (state == SOLVED) {
    if (key == ' ') {
      loadLevel((levelIndex + 1) % LEVELS.length);
      draw();
    }
  }

  if (key == 'PageUp' || key == 'PageDown') {
    loadLevel((levelIndex + LEVELS.length + (key == 'PageUp' ? -1 : 1)) %
              LEVELS.length);
    draw();
  }
}

async function init() {
  const assets = await Promise.all([
    File.readImageData('examples/data/character.png'),
    File.readImageData('examples/data/tilesheet.png'),
    window.loadFont('examples/data/PressStart2P.ttf', 'PressStart2P'),
  ]);

  character = new ImageBitmap(assets[0]);
  tilesheet = new ImageBitmap(assets[1]);

  window.title = 'Sokoban!';
  window.icon = [ new ImageData(assets[1], 7 * 128, 1 * 128, 128, 128) ];

  window.width = window.screen.width / 2;
  window.height = window.width / 16 * 10;
  window.keepAspectRatio = true;

  window.addEventListener('resize', draw);
  window.addEventListener('keydown', onKeyDown);

  loadLevel(0);
  draw();
}

// #: wall
// p: initial player position
// b: box
// o: goal
// P: player over a goal
// B: box over a goal
const LEVELS = [
  [
    '#####..',
    '#p  #..',
    '# #b###',
    '# b oo#',
    '#######',
  ],
  [
    '#######',
    '#o   o#',
    '#  b  #',
    '# bpb #',
    '#  b  #',
    '#o   o#',
    '#######',
  ],
  [
    '#######',
    '#o   o#',
    '#obbbo#',
    '##bpb##',
    '#obbbo#',
    '#o   o#',
    '#######',
  ],
  [
    '#######',
    '#o   o#',
    '# bpb #',
    '# ### #',
    '# b b #',
    '#o   o#',
    '#######',
  ],
  [
    '##########.',
    '#  boob  ##',
    '#  boob   #',
    '#  boob   #',
    '#  boob  p#',
    '###########',
  ],
];

await init();
