---
layout: about
title: Window.js | Examples
---

Examples
========

There are a few example applications in the `examples` directory of the
Window.js checkout.


Breakout
--------

A simple [Breakout](https://en.wikipedia.org/wiki/Breakout_(video_game)) clone,
that was ported from an older HTML5 version.

Use the left and right arrows to move and space to start, or use the mouse and
click to start.

```shell
$ out/src/windowjs.exe examples/breakout.js
```

Press `F2` to see the FPS and memory usage counters and `F5` to reload.

Pressing the digits 1 to 6 skips ahead to the corresponding level.

<p align="center"><img src="breakout.png" /></p>


Sokoban
-------

A simple [Sokoban](https://en.wikipedia.org/wiki/Sokoban) clone.

```shell
$ out/src/windowjs.exe examples/sokoban.js
```

<p align="center"><img src="sokoban.png" /></p>


Tetris
------

A simple [Tetris](https://en.wikipedia.org/wiki/Tetris) clone.

```shell
$ out/src/windowjs.exe examples/tetris.js
```

<p align="center"><img src="tetris.png" /></p>


Squares
-------

A debug application. It draws with a transparent window and makes various
calls to the canvas API, to quickly verify that they work as expected.

```shell
$ out/src/windowjs.exe examples/squares.js
```

<p align="center"><img src="squares.png" /></p>
