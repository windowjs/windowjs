// Run processing files like this:
//
//   $ windowjs.exe examples/p5.js -- examples/p5/simulate-particles.js

const DEBUG = false;

// The current "window" is the Window.js main window;
// it will be replaced with a fake "DOM window" below, that will also be
// the global object.
const _window = window;
globalThis._window = _window;

const loadListeners = [];

const debugProxyHandler = {
  get : function(target, prop, receiver) {
    const name = typeof(prop) == 'symbol' ? prop.toString() : prop;
    console.log('-- PROXY get on ' + target._debugName + ': ' + name);
    return Reflect.get(...arguments);
  },
  set : function(target, prop, value) {
    const name = typeof(prop) == 'symbol' ? prop.toString() : prop;
    console.log('-- PROXY set on ' + target._debugName + ': ' + name + ' = ' +
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
    this._isMainCanvas = true;
    this._context = _window.canvas;
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
    this.body = new Element(this, 'body');
  }

  createElement(tag) {
    if (tag == 'canvas') {
      const canvas = debugProxy('<canvas>', new Element(this, tag));
      this._canvasList.push(canvas);
      return canvas;
    } else if (tag == 'script') {
      return debugProxy('<script>', new Element(this, tag));
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

  getElementById(id) {
    return this._elementsById.get(id);
  }

  getElementsByTagName(tag) {
    if (tag == 'main') {
      return [this._main];
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
    // TODO: fix the path.
    try {
      const image = await File.readImageBitmap('examples/p5/' + path);
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

function defineGetter(object, name, getter) {
  Object.defineProperty(object, name, {get : getter});
}

function setupEnvironment() {
  globalThis.window = globalThis;
  globalThis.Image = Image;
  globalThis.Request = Request;
  globalThis.fetch = fetch;
  globalThis.document = debugProxy('<document>', new Document());
  globalThis.navigator = {
    userAgent : '',
  };
  globalThis.screen = _window.screen;
  globalThis.location = {
    href: '',
    search: '',
    pathname: '',
  };

  defineGetter(globalThis, 'innerWidth', () => _window.width);
  defineGetter(globalThis, 'innerHeight', () => _window.height);

  globalThis.addEventListener = function(event, listener) {
    if (event === 'load') {
      loadListeners.push(listener);
    } else {
      _window.addEventListener(event, listener);
    }
  }

  globalThis.removeEventListener = _window.removeEventListener.bind(_window);

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

async function run() {
  // Note that "source" and "icon" are Promises.
  const path = Process.args[0] || 'examples/p5/hello.js';
  const source = File.readText(path);
  const icon = File.readImageData('examples/p5/p5.ico');

  setupEnvironment();

  if (DEBUG) {
    await import('./p5/p5.js');
  } else {
    await import('./p5/p5.min.js');
  }

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

  // The window will become visible after run() finishes.
  _window.title = 'Processing - p5.js';
  _window.icon = [ await icon ];
  _window.resizable = false;
}

await run();
