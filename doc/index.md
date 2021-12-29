---
layout: default
title: Window.js
---

<p>&nbsp;</p>

<span class="title"><span class="magenta">**Window.js**</span> is an
open-source Javascript runtime for desktop graphics programming.
</span>

<p>&nbsp;</p>

It provides APIs for [window](/doc/window) creation and manipulation via
[GLFW](https://glfw.org) and the HTML5 [Canvas API](/doc/canvas) via the
[Skia](https://skia.org) graphics library, backed by the [v8](https://v8.dev)
Javascript engine.

Window.js comes with familiar APIs for web developers for
[input](/doc/window#window.addEventListener) event handling and
[animated rendering](/doc/global#requestAnimationFrame), and has additional,
convenient APIs for [file access](/doc/file).

See the **[documentation](/doc)** and the **[quick start](/about#quick-start)**
guide to get started.

<p>&nbsp;</p>

```javascript
window.title = 'Hello!';

const canvas = window.canvas;

canvas.fillStyle = '#023047';
canvas.fillRect(0, 0, canvas.width, canvas.height);

canvas.fillStyle = 'magenta';
canvas.fillRect(100, 100, 200, 200);
```

<p align="center"><img src="/window.png" style="width: 601px" /></p>

<p>&nbsp;</p>

Window.js is developed on its
[Github project](https://github.com/windowjs/windowjs) page.

