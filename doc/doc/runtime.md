---
layout: documentation
title: Window.js | Javascript runtime
---

Javascript runtime
==================

Window.js programs are written in Javascript files that are loaded as Javascript
modules.


Modules
-------

The initial module is the first file passed to Window.js to load on startup
via the command line:

```shell
./windowjs main.js
```

`main.js` can then import other modules, which are resolved relative to the
importing module's path.

```javascript
import * as images from './images.js';
import * as assets from './assets.js';

function init() {
  images.init();
  assets.init();
}

init();
```

Window.js supports dynamic `import()` and await on dynamic imports at the
top level.

Window.js *only* supports ES6 modules, and doesn't support `require()` nor
`node_modules`.


Global
------

The [global](/doc/global) object contains objects and functions that are used
to access other APIs. It is globally accessible as `globalThis`.

The [window](/doc/window) object is the main window, and has the main 2D canvas
in [window.canvas](/doc/canvas).


Render loop
-----------

Window.js is continuously running the render loop until the main window
is [closed](/doc/window#window.close):

1.  Run any scheduled or delayed tasks, such as native callbacks, promises
    resolving and `setTimeout` callbacks.

2.  Run any [requestAnimationFrame](/doc/global#requestAnimationFrame)
    callbacks.

3.  Once all of the `requestAnimationFrame` callbacks have returned, Window.js
    gives a hint to v8 to do any garbage collection tasks now and swaps the
    main framebuffer at this time. By default, this step wait for vsync before
    completing.

4.  Wait for any input events before looping back to step 1. Window.js will
    jump back to step 1 if a scheduled task is ready to run, or if a new frame
    has been requested by the application, or if any input events are ready to
    be dispatched.

    If the application isn't requesting new frames then the loop will wait
    at this step until an input event is received.

Every canvas object is backed by a GPU texture, and its contents are retained
across frames. When the app isn't continuously drawing then step (3) above is
just a texture blit.

First frame
-----------

The window is initially invisible until the initial module finishes executing.

If the initial module requests a frame via
[requestAnimationFrame](/doc/global#requestAnimationFrame) then that callback
is executed before making the window visible. This ensures that the first frame
has meaningful content to present to the user.

The main application may set [window.visible](/doc/window#window.visible) to
`false` to keep the window insivible until a later stage.
