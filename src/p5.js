// Run processing files like this:
//
//   $ windowjs-p5.exe examples/p5/image-copy-method.js
//
// To work on this loader directly, it's easier to use windowjs.exe:
//
//   $ windowjs.exe src/p5.js -- examples/p5/image-copy-method.js

const DEBUG = false;

// The current "window" is the Window.js main window;
// it will be replaced with a fake "DOM window" below, that will also be
// the global object.
const _window = window;
globalThis._window = _window;

let _scriptDir = '.';

const loadListeners = [];

const debugProxyHandler = {
  get : function(target, prop, receiver) {
    const name = typeof (prop) == 'symbol' ? prop.toString() : prop;
    console.log('PROXY get on ' + target._debugName + ': ' + name);
    return Reflect.get(...arguments);
  },
  set : function(target, prop, value) {
    const name = typeof (prop) == 'symbol' ? prop.toString() : prop;
    console.log('PROXY set on ' + target._debugName + ': ' + name + ' = ' +
                value);
    return Reflect.set(...arguments);
  },
};

function debugProxy(name, object) {
  if (DEBUG) {
    object._debugName = name;
    return new Proxy(object, debugProxyHandler);
  } else {
    return object;
  }
}

function defineGetter(object, name, getter) {
  Object.defineProperty(object, name, {get : getter});
}

function defineGetterSetter(object, name, getter, setter) {
  Object.defineProperty(object, name, {get : getter, set : setter});
}

async function setCursor(value) {
  const parts = value.split(',')[0].split(' ');
  if (parts[0] == 'none') {
    _window.cursor = 'hidden';
  } else if (parts[0].startsWith('url(')) {
    const len = parts[0].length;
    const path = parts[0].substring(4, len - 1);
    const image = await File.readImageData(_scriptDir + '/' + path);
    _window.cursor = image;
    _window.cursorOffsetX = parseInt(parts[1] || '0');
    _window.cursorOffsetY = parseInt(parts[2] || '0');
  }
}

// Used by loadFont().
class XMLHttpRequest {
  constructor() {
    this.onload = null;
    this.onerror = null;
    this.responseType = '';
  }

  open(method, path) {
    this._path = path;
  }

  async send() {
    try {
      if (this.responseType == 'arraybuffer') {
        this.response =
            await File.readArrayBuffer(_scriptDir + '/' + this._path);
        if (this.onload) {
          try {
            this.onload();
          } catch (e) {
            console.log('Exception from onload: ', e);
          }
        }
      } else {
        if (this.onerror) {
          this.onerror('Unsupported responseType: ' + this.responseType);
        }
      }
    } catch (e) {
      if (this.onerror) {
        this.onerror(e);
      }
    }
  }
}

// A generic Element that takes that whole screen. This is used for the <canvas>
// instance, as well as <main> and <script> elements.
class Element {
  constructor(document, tag) {
    this.document = document;
    this.tagName = tag;
    this.classList = new Set();
    this.dataset = debugProxy('<' + tag + '>.dataset', {});
    this.style = debugProxy('<' + tag + '>.style', {});
    this._width = 100;
    this._height = 100;
    if (tag == 'canvas') {
      this._isMainCanvas = false;
      this._context = null;
    }
  }

  _setIsMainCanvas() {
    if (this._isMainCanvas) {
      return;
    }
    this._isMainCanvas = true;
    this._context = _window.canvas;
    defineGetterSetter(this.style, 'cursor', () => _window.cursor, setCursor);
  }

  get id() {
    return this._id;
  }

  set id(value) {
    this._id = value;
    this.document._registerElement(value, this);
  }

  appendChild(c) {
    if (this.tagName == 'main' && c.tagName == 'canvas') {
      c._setIsMainCanvas();
    }
    c.parentNode = this;
  }

  removeChild(c) {
    c.parentNode = null;
  }

  get width() {
    if (this._isMainCanvas) {
      return _window.width;
    } else {
      return this._width;
    }
  }

  get height() {
    if (this._isMainCanvas) {
      return _window.height;
    } else {
      return this._height;
    }
  }

  set width(value) {
    if (this._isMainCanvas) {
      _window.width = value;
    } else {
      this._width = value;
      if (this._context) {
        this._context.width = value;
      }
    }
  }

  set height(value) {
    if (this._isMainCanvas) {
      _window.height = value;
    } else {
      this._height = value;
      if (this._context) {
        this._context.height = value;
      }
    }
  }

  get offsetWidth() {
    return this.width;
  }

  get offsetHeight() {
    return this.height;
  }

  get scrollWidth() {
    return this.width;
  }

  get scrollHeight() {
    return this.height;
  }

  getBoundingClientRect() {
    return {
      top : 0,
      left : 0,
      right : this.width,
      bottom : this.height,
    };
  }

  getContext(type) {
    if (type == '2d') {
      if (this._isMainCanvas) {
        return _window.canvas;
      } else {
        if (!this._context) {
          this._context =
              new CanvasRenderingContext2D(this._width, this._height);
        }
        return this._context;
      }
    } else {
      throw new Error('Context not supported: ' + type);
    }
  }
}

class Document {
  constructor() {
    this._elementsById = new Map();
    this._main = debugProxy('<main>', new Element(this, 'main'));
    this._loading = debugProxy('<div#p5_loading>', new Element(this, 'div'));
    this._loading.id = 'p5_loading';
    this._main.appendChild(this._loading);
    this._canvasList = [];
    this.head = new Element(this, 'head');
    this.body = new Element(this, 'body');
  }

  createElement(tag) {
    if (tag == 'canvas') {
      const canvas = debugProxy('<canvas>', new Element(this, tag));
      this._canvasList.push(canvas);
      return canvas;
    } else if (tag == 'script') {
      return debugProxy('<script>', new Element(this, tag));
    } else if (tag == 'style') {
      return debugProxy('<style>', new Element(this, tag));
    } else {
      throw new Error('Unsupported element: ' + tag);
    }
  }

  createElementNS(ns, tag) {
    if (ns == 'http://www.w3.org/1999/xhtml' && tag == 'a') {
      return debugProxy('<a>', new Element(this, tag));
    } else {
      throw new Error('Unsupported element with namespace: ' + tag);
    }
  }

  createTextNode(text) {
    return new Element(this, '#text');
  }

  getElementById(id) {
    return this._elementsById.get(id);
  }

  getElementsByTagName(tag) {
    if (tag == 'main') {
      return [ this._main ];
    } else if (tag == 'canvas') {
      return this._canvasList;
    } else {
      throw new Error('Unsupported element by tagName: ' + tag);
    }
  }

  _registerElement(id, element) {
    this._elementsById.set(id, element);
  }

  hasFocus() {
    return window.focused;
  }
}

// Used by loadImage().
class Image {
  constructor() {
    this.onload = null;
    this.onerror = null;
    this._src = null;
    this._imageBitmap = null;
  }

  get src() {
    return this._src;
  }

  set src(value) {
    this._src = value;
    this._loadImage(value);
  }

  async _loadImage(path) {
    try {
      const image = await File.readImageBitmap(_scriptDir + '/' + path);
      this.width = image.width;
      this.height = image.height;
      this._imageBitmap = image;
      if (this.onload) {
        this.onload();
      }
    } catch (e) {
      if (this.onerror) {
        this.onerror(e);
      }
    }
  }
}

class Request {
  constructor(path, options) {
    this.path = path;
    this.options = options;
  }
}

async function fetch(path, request) {
  // Used by loadImage().
  const headers = new Map();
  headers.set('content-type', 'image/not-a-gif');
  return {
    headers,
  };
}

function setupEnvironment() {
  globalThis.Image = Image;
  globalThis.Request = Request;
  globalThis.XMLHttpRequest = XMLHttpRequest;
  globalThis.fetch = fetch;
  globalThis.window = globalThis;
  globalThis.document = debugProxy('<document>', new Document());
  globalThis.navigator = {
    userAgent : '',
  };
  globalThis.screen = _window.screen;
  globalThis.location = {
    href : '',
    search : '',
    pathname : '',
  };

  defineGetter(globalThis, 'innerWidth', () => _window.width);
  defineGetter(globalThis, 'innerHeight', () => _window.height);

  globalThis.addEventListener =
      function(event, listener) {
    if (event === 'load') {
      loadListeners.push(listener);
    } else {
      _window.addEventListener(event, listener);
    }
  }

      globalThis.removeEventListener =
          _window.removeEventListener.bind(_window);

  // CanvasRenderingContext2D.drawImage takes a <canvas> argument in HTML5,
  // but another CanvasRenderingContext2D directly in Window.js.
  const drawImage = CanvasRenderingContext2D.prototype.drawImage;
  CanvasRenderingContext2D.prototype.drawImage = function(image, ...args) {
    if (image instanceof Element) {
      image = image._context;
    } else if (image instanceof Image) {
      image = image._imageBitmap;
    }
    return drawImage.call(this, image, ...args);
  };
}

function setupOverrides() {
  const resize = p5.Renderer2D.prototype.resize;
  p5.Renderer2D.prototype.resize = function(w, h) {
    resize.call(this, w, h);
    // Renderer2D.resize() is called from createCanvas(). It applies a scale
    // by _pixelDensity again, doubling the scaling; call resetMatrix() from
    // here to restore the intended scaling.
    this.resetMatrix();
  };
}

async function runP5Script(path) {
  _scriptDir = File.dirname(path);
  const source = File.readText(path);

  setupEnvironment();

  await loadP5();

  // Calling eval() with the indirection makes it use a global scope (instead
  // of a local scope), which makes setup() and draw() available in globalThis.
  (0, eval)(await source);

  setupOverrides();

  document.readyState = 'complete';
  for (const listener of loadListeners) {
    listener();
  }
  loadListeners.length = 0;

  // Run any other promise handlers now. This makes the Promise returned by
  // run() resolve after all the async work pending in p5.js has finished too.
  await Promise.all([]);
}

let linkX = 0;
let linkY = 0;
let linkW = 0;
let linkH = 0;

function isInsideLink(x, y) {
  return x >= linkX && x <= linkX + linkW && y >= linkY && y <= linkY + linkH;
}

function drawWelcomeScreen() {
  let font = 'sans';
  const fonts = [ 'Segoe UI', 'Verdana', 'Arial', 'DejaVu Sans' ];
  for (const f of fonts) {
    if (_window.fonts.includes(f)) {
      font = f;
      break;
    }
  }

  const canvas = _window.canvas;

  canvas.fillStyle = 'white';
  canvas.fillRect(0, 0, canvas.width, canvas.height);

  canvas.fillStyle = '#023047';
  canvas.fillRect(0, 0, canvas.width, 60 * devicePixelRatio);

  canvas.fillStyle = '#eb005a';
  canvas.font = `bold ${36 * devicePixelRatio}px "${font}"`;
  let dx = canvas.measureText('Window.js ').width;
  canvas.fillText('Window.js', 12, 42 * devicePixelRatio);
  canvas.fillStyle = 'white';
  canvas.fillText('+', 12 + dx, 42 * devicePixelRatio);
  dx += canvas.measureText('+ ').width;
  canvas.fillStyle = '#eb005a';
  canvas.fillText('p5.js', 12 + dx, 42 * devicePixelRatio);

  canvas.font = `bold ${18 * devicePixelRatio}px "${font}"`;
  canvas.fillText('Welcome to Window.js with p5.js!', 12,
                  112 * devicePixelRatio);

  canvas.fillStyle = 'black';

  canvas.font = `${14 * devicePixelRatio}px "${font}"`;
  canvas.fillText('Window.js is a Javascript runtime for the desktop.', 12,
                  168 * devicePixelRatio);
  canvas.fillText('With p5.js, it can run Processing scripts on the desktop.',
                  12, 192 * devicePixelRatio);

  canvas.fillText('See the ',
                  12, 240 * devicePixelRatio);
  dx = canvas.measureText('See the ').width;

  const link = 'online documentation';
  linkX = 12 + dx;
  linkY = 240 * devicePixelRatio;
  linkW = canvas.measureText(link).width;
  linkH = 24 * devicePixelRatio;

  canvas.fillStyle = '#007ab8';
  canvas.fillText(link, linkX, linkY);

  canvas.fillStyle = 'black';
  canvas.fillText(' to get started.', linkX + linkW, linkY);

  linkY -= 24;
}

function showWelcomeScreen() {
  drawWelcomeScreen();
  _window.addEventListener('mousemove', function(event) {
    if (isInsideLink(event.x, event.y)) {
      _window.cursor = 'hand';
    } else {
      _window.cursor = 'arrow';
    }
  });
  _window.addEventListener('click', function(event) {
    if (event.button == 0 && isInsideLink(event.x, event.y)) {
      window.open('https://windowjs.org/about/processing');
    }
  });
}

const ICON = `
  iVBORw0KGgoAAAANSUhEUgAAACEAAAAhCAMAAABgOjJdAAAAGXRFWHRTb2Z0d2FyZQBBZG9iZSBJ
  bWFnZVJlYWR5ccllPAAAAAlwSFlzAAALEwAACxMBAJqcGAAAAIRQTFRFR3BM7SJd7SJd7SJd7SJd
  7SJd7SJd7SJd7SJd7SJd7SJd7SJd7SJd7SJd7SJd7SJd7SJd7SJd7SJd7SJd7SJd7SJd7SJd7SJd
  7SJd7SJd7SJd7SJd7SJd7SJd7SJd7SJd7SJd7SJd7SJd7SJd7SJd7SJd7SJd7SJd7SJd7SJd7SJd
  7SJdsG996AAAACt0Uk5TADD47yB5gPxAEOCexPvu4t+QYPBQiP2/+vnOauXz2NS43MyXyffs9luy
  gtyZqmcAAAD2SURBVDjLzZTHFoMgEEWFQJBYsCS29F74//+LIihiyXGXt2Jm7pkmYlmzBA4roWiU
  sPdcaDlOLP6bQGeMhgn7Wgcw52TnAIOg0Ql6HAuCCS/xHdAQCBecVCcminClHMqN3b3Gh+oiE9qW
  xGGSKMqO+LRoOcAbkrEw2X2omCa6DYZ9sQIpumVZJ5x3wnK1gQY8wcBHeendJLgPINipkvWTBEaf
  zARCc2LiGDdDAdBuEFsHKGzdOJHnmGqEq4BQM3jaAmvlO4qEvjLXLSJvAKzzApkwQPr/GJcuT/WG
  PVVR79UlZNNYaTVUb2Wbi8Y/XPrjFUBznowvykRIOJGyB6IAAAAASUVORK5CYII=`;

async function run() {
  const icon = ImageData.decode(Codec.base64ToArrayBuffer(ICON));
  const path = Process.args[0];
  let title = 'Welcome!';

  if (path) {
    await runP5Script(path);
    title = File.basename(path);
  } else {
    await showWelcomeScreen();
  }

  // The window will become visible after run() finishes.
  _window.title = 'Window.js P5 - ' + title;
  _window.icon = [ await icon ];
  _window.resizable = false;
}

await run();

// WINDOWJS_P5_IMPORT_MARKER

// Everything below the marker above gets cut at build time, and subsituted
// with a copy of libraries/p5.min.js wrapped in a loadP5() function.
// The function below is used when this file gets executed directly,
// for ease of development and debugging.

async function loadP5() {
  await import('../libraries/p5.js');
}
