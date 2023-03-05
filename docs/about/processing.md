---
layout: about
title: Window.js | Processing with p5.js
---

Processing with p5.js
=====================

Window.js can run Processing sources written for [p5.js](https://p5js.org).

```shell
$ out/windowjs.exe src/p5/p5.js -- examples/p5/simulate-recursive-tree.js
```

Support for p5.js sources is **highly experimental**.


Implementation details
======================

p5.js builds on web APIs and mainly on the HTML5 Canvas API, which is also
implemented by Window.js.

Support for p5.js in Window.js is provided via a loader in
[src/p5/p5.js](https://github.com/windowjs/windowjs/blob/main/src/p5/p5.js)
that sets up the environment expected by p5.js:

*  Sets `window` as the global object (`globalThis === window`)
*  Fakes the `fetch` and `XMLHttpRequest` APIs to load local resources
*  Fakes DOM elements like `document`, `body` and `canvas`
*  Imports the minified p5.js sources
*  evals the processing file, passed as the command line argument after `--`,
   to make its `setup` and `draw` functions available globally
*  Fires the DOM `load` event on startup.

A future release could embed the loader and `p5.min.js` into the binary.


API support
===========

Window.js supports many features of p5.js: image and font loading, offscreen
drawing, custom cursors, etc.

The following APIs are not supported:

* `WebGL` and 3D APIs.
* Sound.
* DOM APIs, like `createButton` and `createSlider`.
* Loading resources over the network, with `loadJSON` or `loadImage`.

Some APIs are supported but very slow:

* Pixel-by-pixel reading via `Image.get(x, y)`; the pixels aren't cached in RAM
  and each read is a roundtrip to the GPU.


Examples
========

Many of the [`p5.js examples`](https://p5js.org/examples/) run without
modification. They are included in the Window.js checkout under
[examples/p5](https://github.com/windowjs/windowjs/tree/main/examples/p5).
