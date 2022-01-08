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
    console.log('-- PROXY get on ' + target._debugName + ': ' + prop);
    return Reflect.get(...arguments);
  },
  set : function(target, prop, value) {
    console.log('-- PROXY set on ' + target._debugName + ': ' + prop + ' = ' +
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
  }

  get id() {
    return this._id;
  }

  set id(value) {
    this._id = value;
    this.document._registerElement(value, this);
  }

  appendChild(c) {
    // Ignore.
  }

  get offsetWidth() {
    return _window.width;
  }

  get offsetHeight() {
    return _window.height;
  }

  get width() {
    return _window.width;
  }

  get height() {
    return _window.height;
  }

  set width(value) {
    _window.width = value;
  }

  set height(value) {
    _window.height = value;
  }

  get scrollWidth() {
    return _window.width;
  }

  get scrollHeight() {
    return _window.height;
  }

  getBoundingClientRect() {
    return {
      top : 0,
      left : 0,
      right : _window.width,
      bottom : _window.height,
    };
  }

  getContext(type) {
    if (type == '2d') {
      return _window.canvas;
    } else {
      throw new Error('Context not supported: ' + type);
    }
  }
}

class Document {
  constructor() {
    this._elementsById = new Map();
    this._main = debugProxy('<main>', new Element(this, 'main'));
    this._canvasList = [];
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

function defineGetter(object, name, getter) {
  Object.defineProperty(object, name, {get : getter});
}

function setupEnvironment() {
  globalThis.window = globalThis;
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
}

async function run() {
  // Note that "source" and "icon" are Promises.
  const path = Process.args[0] || 'examples/p5/hello.js';
  const source = File.readText(path);
  const icon = File.readImageData('examples/p5/p5.ico');

  setupEnvironment();
  //await import('./p5/p5.js');
  await import('./p5/p5.min.js');
  // Calling eval() with the indirection makes it use a global scope (instead
  // of a local scope), which makes setup() and draw() available in globalThis.
  (0, eval)(await source);

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
