// TODO: support top-level await in the console:
// const m = await import('./some/module.js');

const FONT_HEIGHT = 12;
const LINE_HEIGHT = 20;

const canvas = window.canvas;

let defaultFont = '';
let fontDescent = 0;

let drawPending = false;

// "blocks" is a list of block objects.
// Each block object has a list of strings in "lines",
// and a "color".
const blocks = [];
const blocksWithLineBreaks = [];
let blocksWithLineBreaksLineCount = 0;

let scrollSkipLines = 0;

const ZOOM_LEVELS = [
  0.25, 0.33, 0.5, 0.67, 0.75, 0.8, 0.9, 1.0, 1.1, 1.25, 1.5, 1.75, 2, 2.5, 3,
  4, 5
];
let zoom = 1.0;

const inputHistory = [];
let inputHistoryIndex = 0;
let input = '';
let inputIndex = 0;

let cursorVisible = false;
let cursorTimer = null;

function getFont() {
  const list = [
    'Consolas',
    'Lucida Console',
    'Monaco',
    'Fira Mono',
    'Cascadia Mono',
    'DejaVu Sans Mono',
    'Noto Sans Mono',
    'Courier New',
  ];
  const fonts = window.fonts;
  for (const f of list) {
    if (fonts.includes(f)) return f;
  }
  return 'Monospace';
}

function makeNormalFont() {
  return `${FONT_HEIGHT}px "${defaultFont}"`;
}

function makeBoldFont() {
  return `bold ${FONT_HEIGHT}px "${defaultFont}"`;
}

function updateBlocks() {
  blocksWithLineBreaks.length = 0;
  blocksWithLineBreaksLineCount = 0;

  const unit = canvas.measureText(' ').width;
  const width = canvas.width / zoom - unit;

  for (const block of blocks) {
    const lines = [];

    for (const line of block.lines) {
      if (line.length == 0) {
        lines.push(line);
        continue;
      }

      let remain = line;

      while (remain.length > 0) {
        if (remain.length * unit <= width) {
          lines.push(remain);
          break;
        }
        let k = Math.min(Math.floor(width / unit), remain.length);
        for (let i = k; i >= 10; i--) {
          if (remain[i] == ' ' || remain[i] == '\t' || remain[i] == '\n') {
            k = i + 1;
            break;
          }
        }
        lines.push(remain.substr(0, k));
        remain = remain.substr(k);
      }
    }

    const color = block.color;
    blocksWithLineBreaks.push({color, lines});
    blocksWithLineBreaksLineCount += lines.length;
  }
}

function getVisibleLineCount() {
  // -1 for the input line.
  return Math.ceil((canvas.height / zoom) / LINE_HEIGHT) - 1;
}

function drawCursor() {
  const prompt = '> ';
  const promptWidth = canvas.measureText(prompt).width;

  const metrics = canvas.measureText(' ');
  const x = 8 + promptWidth + metrics.width * inputIndex;
  const bottom = blocksWithLineBreaksLineCount >= getVisibleLineCount()
                     ? (canvas.height / zoom) - LINE_HEIGHT
                     : blocksWithLineBreaksLineCount * LINE_HEIGHT;
  const y = bottom;
  const visible = cursorVisible && window.focused;
  canvas.antialias = false;
  canvas.fillStyle = visible ? '#000000ff' : '#ffffffff';
  canvas.fillRect(x, y + LINE_HEIGHT - fontDescent - FONT_HEIGHT,
                  1 / devicePixelRatio, FONT_HEIGHT);
  canvas.antialias = true;
}

function draw() {
  drawPending = false;

  const width = canvas.width / zoom;
  const height = canvas.height / zoom;

  canvas.fillStyle = '#ffffffff';
  canvas.fillRect(0, 0, width, height);

  let bottom = blocksWithLineBreaksLineCount >= getVisibleLineCount()
                   ? height - LINE_HEIGHT
                   : blocksWithLineBreaksLineCount * LINE_HEIGHT;

  let y = bottom;

  let linesSkipped = 0;

  for (let i = blocksWithLineBreaks.length - 1; i >= 0 && y > 0; i--) {
    const block = blocksWithLineBreaks[i];

    let end = block.lines.length;

    if (linesSkipped < scrollSkipLines) {
      if (block.lines.length <= scrollSkipLines - linesSkipped) {
        linesSkipped += block.lines.length;
        continue;
      } else {
        end -= scrollSkipLines - linesSkipped;
        linesSkipped = scrollSkipLines;
      }
    }

    y -= LINE_HEIGHT * end;

    canvas.fillStyle = '#ddddddff';
    canvas.fillRect(0, y, width, 1);

    canvas.fillStyle = block.color;
    for (let j = 0; j < end; j++) {
      const line = block.lines[j];
      canvas.fillText(line, 8, y + j * LINE_HEIGHT + FONT_HEIGHT + fontDescent);
    }
  }

  y = bottom;
  canvas.fillStyle = '#ddddddff';
  canvas.fillRect(0, y, width, 1);

  const prompt = '> ';
  canvas.fillStyle = 'royalblue';
  canvas.font = makeBoldFont();
  canvas.fillText(prompt, 8, y + FONT_HEIGHT + fontDescent);

  const promptWidth = canvas.measureText(prompt).width;
  canvas.font = makeNormalFont();
  canvas.fillStyle = '#000000ff';
  canvas.fillText(input, 8 + promptWidth, y + FONT_HEIGHT + fontDescent);

  drawCursor();
}

function requestDraw() {
  if (drawPending) return;
  drawPending = true;
  requestAnimationFrame(draw);
}

function updateScrollLines(delta) {
  scrollSkipLines += delta;
  if (scrollSkipLines <= 0) {
    scrollSkipLines = 0;
  } else {
    const visibleCount = getVisibleLineCount();
    while (scrollSkipLines > 0 &&
           blocksWithLineBreaksLineCount - scrollSkipLines < visibleCount) {
      scrollSkipLines--;
    }
  }
  requestDraw();
}

function scrollToTop() {
  updateScrollLines(blocksWithLineBreaksLineCount);
}

function scrollToBottom() {
  updateScrollLines(-scrollSkipLines);
}

function clearConsole() {
  blocks.length = 0;
  updateBlocks();
  requestDraw();
}

function updateZoom(delta) {
  if (delta == 0) {
    zoom = 1.0;
  } else {
    let k = ZOOM_LEVELS.indexOf(zoom);
    if (k < 0) {
      zoom = 1.0;
    } else if (delta < 0 && k > 0) {
      zoom = ZOOM_LEVELS[k - 1];
    } else if (delta > 0 && k < ZOOM_LEVELS.length - 1) {
      zoom = ZOOM_LEVELS[k + 1];
    }
  }
  canvas.resetTransform();
  zoom *= devicePixelRatio;
  canvas.scale(zoom, zoom);
  updateBlocks();
  requestDraw();
}

function resetCursorSchedule() {
  if (!cursorVisible) {
    cursorVisible = true;
    drawCursor();
  }
  if (cursorTimer != null) clearTimeout(cursorTimer);
  cursorTimer = setTimeout(() => {
    cursorVisible = false;
    drawCursor();
    cursorTimer = setTimeout(resetCursorSchedule, 500);
  }, 500);
}

function insertInput(s) {
  input = input.substr(0, inputIndex) + s + input.substr(inputIndex);
  inputIndex += s.length;
  inputHistoryIndex = -1;
}

function copy() {
  window.setClipboardText(input);
}

function paste() {
  const s = window.getClipboardText();
  insertInput(s);
  requestDraw();
}

function pushBlock(block) {
  blocks.push(block);
  while (blocks.length > 1000) {
    blocks.shift();
  }
  updateBlocks();

  if (scrollSkipLines > 0) updateScrollLines(block.lines.length);

  requestDraw();
}

function getColorForMessage(message) {
  if (message.type == 'exception') return '#ff0000ff';
  if (message.type != 'log') return '#000000ff';
  switch (message.level) {
  case 'debug':
    return '#808080ff';
  case 'log':
    return '#000000ff';
  case 'info':
    return '#0000ffff';
  case 'warn':
    return 'darkorange';
  case 'error':
    return '#ff0000ff';
  case 'dev':
    return 'mediumvioletred';
  }
  return '#000000ff';
}

function onLog(message) {
  const color = getColorForMessage(message);
  const lines = message.message.trimRight().split('\n');
  pushBlock({lines, color});
}

function log(message) {
  onLog({type : 'log', message : message, level : 'DEBUG'});
}

function onException(message) {
  const color = '#ff0000ff';
  const lines = message.message.trimRight().split('\n').concat(
      message.stacktrace.map(line => '  ' + line));
  pushBlock({lines, color});
}

function onEvalResult(result) {
  const color = '#000000ff';
  const lines = result.split('\n');
  pushBlock({lines, color});
}

function onSubmitted(input) {
  const color = '#808080ff';
  const lines = [ '> ' + input ];
  pushBlock({lines, color});
}

function onResize() {
  updateBlocks();
  requestDraw();
}

function onFocus() {
  cursorVisible = false;
  resetCursorSchedule();
}

function onWheel(event) {
  updateScrollLines(event.deltaY);
}

function onKeyPress(event) {
  resetCursorSchedule();
  const key = String.fromCodePoint(event.code);
  insertInput(key);
  requestDraw();
}

function requestEval(source) {
  if (source == 'help()' || source == 'help();') {
    log('Ctrl+L     Clears the console.\n' +
        'Ctrl+Q     Quits the main application.\n' +
        'Ctrl+R     Reloads the main application.\n' +
        'F1         Focuses on main window.\n' +
        'F2         Overlays FPS and memory usage stats in the main window.\n' +
        'F3         Toggles frame profiling.\n' +
        'F4         Overlays console logs in the main window.\n' +
        'F5         Reloads the main application.\n' +
        'F6         Toggles always on top.\n' +
        'F8         Saves a screenshot.\n' +
        'Escape     Closes the console.\n');
    return;
  }
  Process.parent.postMessage({"type" : "eval", "source" : source});
}

function sendRequest(type) {
  Process.parent.postMessage({"type" : type});
}

function isBlankInput(input) {
  return input.trim().length == 0;
}

function isSkipChar(c) {
  return c == ' ' || c == '.';
}

function onKeyDown(event) {
  resetCursorSchedule();

  const key = event.key;

  if (key == 'Escape') {
    window.visible = false;
  } else if (key == 'F1') {
    sendRequest('focus');
  } else if (key == 'F2') {
    sendRequest('fps');
  } else if (key == 'F3') {
    sendRequest('profile-frames');
  } else if (key == 'F4') {
    sendRequest('overlay-console');
  } else if (key == 'F5') {
    sendRequest('reload');
  } else if (key == 'F6') {
    sendRequest('always-on-top');
  } else if (key == 'F8') {
    sendRequest('screenshot');
  } else if (key == 'PageUp') {
    updateScrollLines(10);
  } else if (key == 'PageDown') {
    updateScrollLines(-10);
  } else if (key == 'Home') {
    if (event.ctrlKey) {
      scrollToTop();
    } else {
      inputIndex = 0;
      requestDraw();
    }
  } else if (key == 'End') {
    if (event.ctrlKey) {
      scrollToBottom();
    } else {
      inputIndex = input.length;
      requestDraw();
    }
  } else if (key == '=' && event.ctrlKey && !event.repeat) {
    updateZoom(1);
  } else if (key == '-' && event.ctrlKey && !event.repeat) {
    updateZoom(-1);
  } else if (key == '0' && event.ctrlKey) {
    updateZoom(0);
  } else if (key == 'l' && event.ctrlKey) {
    sendRequest('clear-logs');
  } else if (key == 'q' && event.ctrlKey) {
    sendRequest('close');
  } else if (key == 'r' && event.ctrlKey && !event.repeat) {
    sendRequest('reload');
  } else if (key == 'Backspace') {
    if ((event.ctrlKey && event.altKey) || event.metaKey) {
      input = input.substr(inputIndex);
      inputIndex = 0;
    } else if (event.ctrlKey || event.altKey) {
      if (inputIndex > 0) {
        let index = inputIndex - 1;
        while (index > 0 && input[index] == ' ') {
          index--;
        }
        while (index > 0 && !isSkipChar(input[index - 1])) {
          index--;
        }
        input = input.substr(0, index) + input.substr(inputIndex);
        inputIndex = index;
      }
    } else if (inputIndex > 0) {
      input = input.substr(0, inputIndex - 1) + input.substr(inputIndex);
      inputIndex--;
    }
    requestDraw();
  } else if (key == 'Delete') {
    if ((event.ctrlKey && event.altKey) || event.metaKey) {
      input = input.substr(0, inputIndex);
    } else if (event.ctrlKey || event.altKey) {
      if (inputIndex < input.length) {
        let index = inputIndex + 1;
        while (index < input.length && input[index] == ' ') {
          index++;
        }
        while (index < input.length && !isSkipChar(input[index])) {
          index++;
        }
        input = input.substr(0, inputIndex) + input.substr(index);
      }
    } else if (inputIndex < input.length) {
      input = input.substr(0, inputIndex) + input.substr(inputIndex + 1);
    }
    requestDraw();
  } else if (key == 'Enter' && !event.repeat) {
    if (!isBlankInput(input)) {
      onSubmitted(input);
      requestEval(input);
      const k = inputHistory.indexOf(input);
      if (k >= 0) inputHistory.splice(k, 1);
      inputHistory.push(input);
      inputHistoryIndex = -1;
    }
    scrollToBottom();
    input = '';
    inputIndex = 0;
    requestDraw();
  } else if (key == 'ArrowUp') {
    if (inputHistory.length == 0) return;
    if (inputHistoryIndex < 0 || inputHistoryIndex >= inputHistory.length) {
      inputHistoryIndex = inputHistory.length - 1;
      input = inputHistory[inputHistoryIndex];
      inputIndex = input.length;
      requestDraw();
    } else if (inputHistoryIndex > 0) {
      inputHistoryIndex--;
      input = inputHistory[inputHistoryIndex];
      inputIndex = input.length;
      requestDraw();
    }
  } else if (key == 'ArrowDown') {
    if (inputHistoryIndex < 0 || inputHistoryIndex >= inputHistory.length) {
      return;
    }
    inputHistoryIndex++;
    if (inputHistoryIndex == inputHistory.length) {
      input = '';
      inputIndex = 0;
      inputHistoryIndex = -1;
    } else {
      input = inputHistory[inputHistoryIndex];
      inputIndex = input.length;
    }
    requestDraw();
  } else if (key == 'ArrowLeft') {
    if (inputIndex > 0) {
      inputIndex--;
      if (event.ctrlKey || event.altKey) {
        while (inputIndex > 0 && input[inputIndex] == ' ') {
          inputIndex--;
        }
        while (inputIndex > 0 && !isSkipChar(input[inputIndex - 1])) {
          inputIndex--;
        }
      }
      requestDraw();
    }
  } else if (key == 'ArrowRight') {
    if (inputIndex < input.length) {
      inputIndex++;
      if (event.ctrlKey || event.altKey) {
        while (inputIndex < input.length && input[inputIndex] == ' ') {
          inputIndex++;
        }
        while (inputIndex < input.length && !isSkipChar(input[inputIndex])) {
          inputIndex++;
        }
      }
      requestDraw();
    }
  } else if (key == 'v' && event.ctrlKey) {
    paste();
  } else if (key == 'c' && event.ctrlKey) {
    copy();
  }
}

function onMessage(message) {
  if (message.type == 'init') {
    input = '';
    inputIndex = 0;
    updateScrollLines(-scrollSkipLines);
    window.x = message.x;
    window.y = message.y + message.height + window.frameTop + 8;
    if (window.y + 600 > window.screen.availHeight) {
      window.y -= window.y + 600 - window.screen.availHeight + 1;
    }
    window.width = message.width;
    window.height = 600;
    window.title = 'Console - ' + message.title;
  } else if (message.type == 'show') {
    window.visible = true;
    window.focus();
  } else if (message.type == 'log') {
    onLog(message);
  } else if (message.type == 'exception') {
    onException(message);
  } else if (message.type == 'evalResponse') {
    onEvalResult(message.result);
  } else if (message.type == 'titleChanged') {
    window.title = 'Console - ' + message.title;
  } else if (message.type == 'clearLogs') {
    clearConsole();
  }
}

function init() {
  log('Welcome to the console. Type help() to see a list of commands.');

  defaultFont = getFont();
  canvas.font = makeNormalFont();

  const metrics = canvas.measureText('Ap');
  fontDescent = Math.abs(metrics.actualBoundingBoxDescent);

  Process.parent.addEventListener('message', onMessage);

  window.addEventListener('keydown', onKeyDown);
  window.addEventListener('keypress', onKeyPress);
  window.addEventListener('wheel', onWheel);
  window.addEventListener('resize', onResize);
  window.addEventListener('focus', onFocus);

  resetCursorSchedule();
  updateZoom(0);
  requestDraw();
}

init();
