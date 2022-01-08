---
layout: default
title: Window.js
---

<span class="title"><span class="magenta">**Window.js**</span> is an
open-source Javascript runtime for desktop graphics programming.
</span>

Window.js comes with APIs familiar to web and desktop developers:
*  [Canvas API](/doc/canvas) for 2D graphics and
   [animated rendering](/doc/global#requestAnimationFrame)
*  [Input](/doc/window#window.addEventListener) event handling and
   [window](/doc/window) manipulation
*  Additional APIs for [file access](/doc/file) and
   [subprocesses](/doc/process)
*  Distributed as a [single binary](/download) with no external dependencies

The [documentation](/doc) covers all APIs and how to
**[get started quickly](/doc)**.

Get in [contact](/about/contact) to report issues and to
[contribute](/dev/contributing) to the project.

```javascript
window.title = 'Hello!';

const canvas = window.canvas;

canvas.fillStyle = '#023047';
canvas.fillRect(0, 0, canvas.width, canvas.height);

canvas.fillStyle = '#eb005a';
canvas.fillRect(100, 100, 200, 200);

canvas.fillStyle = 'darkorange';
canvas.translate(450, 200);
canvas.rotate(Math.PI / 5);
canvas.fillRect(0, 0, 200, 200);
```

<p align="center"><img src="/window.png" style="width: 608px" /></p>
