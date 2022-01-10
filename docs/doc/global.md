---
layout: documentation
title: Window.js | Global
properties:
  - devicePixelRatio
  - performance
  - window
  - __dirname
  - __filename
functions:
  - cancelAnimationFrame
  - clearTimeout
  - requestAnimationFrame
  - setTimeout
---

Global
======

Window.js exposes its APIs via the global object, `globalThis`.

*  APIs like [File](/doc/file) and [Process](/doc/process) are accessed via
   their main class.
*  APIs like [window](/doc/window) and [window.canvas](/doc/canvas) are accessed
   via the prebuilt objects.
*  APIs like [setTimeout](#setTimeout) and
   [requestAnimationFrame](#requestAnimationFrame) are accessed as global
   functions, via the global object.

{% include global name="devicePixelRatio" type="number" %}

The ratio between the current DPI and the platform's default DPI.

On the web, this is the scale between physical pixels and CSS pixels. It is
1:1 on lower resolution monitors, and might be 1.5, 2, 3 and other values on
higher DPI ("Retina") displays.

In Window.js, this is the scaling factor set by the platform. Text and UI
elements should be scaled by this factor to have a reasonable size on any
display and platform, regardless of their physical DPI.


{% include global name="performance" type="Object" %}

Utilities to measure elapsed times and the Javascript VM memory usage. See the
[performance](/doc/performance) API.


{% include global name="window" type="Window" %}

The main [window](/doc/window).


{% include global name="__dirname" type="string" %}

`__dirname` is the path to the directory of the currently executing module.


{% include global name="__filename" type="string" %}

`__filename` is the path of the currently executing module.


{% include function name="cancelAnimationFrame" type="(number) => void" %}

Cancels an animation frame callback that has been previously scheduled by
[requestAnimationFrame](#requestAnimationFrame).

Invalid numbers and numbers assigned to animation callbacks that have already
executed are ignored.


{% include function name="clearTimeout" type="(number) => void" %}

Cancels a timeout callbacks that has been previously scheduled by
[setTimeout](#setTimeout).

Invalid numbers and numbers assigned to callbacks that have already executed
are ignored.


{% include function name="requestAnimationFrame" type="(Function) => number" %}

Requests a callback to draw a frame, before the next frame is swapped to the
main window framebuffer.

All callbacks registered via `requestAnimationFrame` will be executed before
the next frame is swapped. To draw continuously, request a new frame from
inside a frame callback:

```javascript
function draw() {
  // Draw continuously:
  requestAnimationFrame(draw);

  const canvas = window.canvas;
  canvas.fillStyle = 'red';
  const width = canvas.width;
  const height = canvas.height;
  const range = width - 300;
  const offset = Math.cos(3 * now / 1000) * (range / 2);
  canvas.fillRect(width / 2 + offset - 50, height / 2 - 50, 100, 100);
}

draw();
```

The callback is passed the current time, as returned by
[performance.now](/doc/performance#performance.now), as its single argument.

Continuous drawing tries to draw at every
[vsync](https://en.wikipedia.org/wiki/vsync). Set
[window.vsync](/doc/window#window.vsync) to `false` to draw as fast as possible,
without waiting for vsync.


{% include function name="setTimeout" type="(Function, number) => number" %}

Registers a callback function to be executed after the given number of
milliseconds. Returns a handle that can be passed to
[clearTimeout](#clearTimeout) to cancels that registration.
