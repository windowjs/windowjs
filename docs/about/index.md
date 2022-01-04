---
layout: about
title: Window.js | About
---

Introduction
============

Window.js is an open-source Javascript runtime for desktop graphics programming.

It provides APIs for [window](/doc/window) creation and manipulation via
[GLFW](https://glfw.org) and the HTML5 [Canvas API](/doc/canvas) via the
[Skia](https://skia.org) graphics library, backed by the [v8](https://v8.dev)
Javascript engine.

Window.js comes with familiar APIs for web developers for
[input](/doc/window#window.addEventListener) event handling and
[animated rendering](/doc/global#requestAnimationFrame), and has additional,
convenient APIs for [file access](/doc/file).

Additional APIs for sound, networking and WebGL are planned for a future
release.

See the [FAQ](/about/faq) for more details.

**Note**: Window.js is a very recent project and hasn't been widely tested yet.
Please report any [issues](/about/issues) and get in
[contact](/about/contact) for help.


## Quick start

Window.js is distributed as a single binary that can be
[downloaded](/download) for Windows and macOS.

It can also be [built](/dev/build) from the sources for Windows, macOS and
Linux.

Window.js runs a Javascript source file directly:

```shell
$ windowjs examples/breakout.js
```

The source files can be edited and reloaded with `F5` in the main window.

A Javascript console is available by pressing `F1`, that can be used to
diagnose errors and explore the APIs. For example, submitting this:

```shell
window.title = "Hello!"
```

changes the title of the main window. Errors are highlighted in the main window:

```shell
window.title = noSuchVariable;
```

Uncaught exceptions and console log errors are also shown in the main window.
That overlay can be toggled with `F4`. See `help()` in the console for more
development tools.

See the [example](/about/examples) applications and the
[API documentation](/doc) for more.


## Contact

Contact the Window.js developers and other users via the
[Window.js mailing list](https://groups.google.com/u/3/g/windowjs/) at
Google Groups, or via its email address `windowjs@googlegroups.com`.

Github users can also use the
[Github Discussions](https://github.com/windowjs/windowjs/discussions) page
for the project.


## License

Window.js is licensed under the [MIT License](/about/license).
