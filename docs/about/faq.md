---
layout: about
title: Window.js | FAQ
---

FAQ
===


What is Window.js?
------------------

Window.js is a Javascript runtime with access to desktop windowing, input,
file and graphics APIs.


What does Window.js build on?
-----------------------------

* The Javascript engine is [v8](http://v8.dev).
* Platform integration for [windowing](/doc/window) and input is built on
  [GLFW](http://glfw.org).
* The [canvas API](/doc/canvas) is largely built on the
  [Skia graphics library](http://skia.org).
* Access to the GL API is built on the [GLAD library](https://glad.dav1d.de/).
* [Subprocesses and IPC](/doc/process) is built on [libuv](http://libuv.org).
* Window.js itself is mostly written in C++17. The build system is
  [CMake](http://cmake.org).
* The documentation is built with [Jekyll](https://jekyllrb.com/) for its
  integration with [Github Pages](https://pages.github.com/).


Is Window.js compatible with web APIs?
--------------------------------------

Full compatibility with web APIs is not a goal of Window.js.

However, Window.js programs can benefit from the large number of useful
libraries written for Node.js and for the web. The web APIs are also extensively
documented and widely known.

Window.js *tries* to follow web APIs by default, and offers more convenient or
performant APIs where it makes sense.

For example, the [canvas API](/doc/canvas) mostly follows the web canvas API,
and the
[canvas documentation at MDN](https://developer.mozilla.org/en-US/docs/Web/API/CanvasRenderingContext2D)
applies to Window.js as well.

On the other hand, the [file API](/doc/file) is custom to Window.js. Is it more
straightforward and efficient for the expected use cases of Window.js.
The equivalent web APIs are either non-existent, or more cumbersome to use
due to web security requirements that Window.js doesn't observe.

A longer-term goal is to support the [three.js](https://threejs.org/) 3D
programming library in Window.js, which will require extensive compatibility
with web APIs.


Is Window.js stable?
--------------------

Window.js is implemented to be stable at runtime: memory shouldn't leak,
misuse of APIs shouldn't cause crashes, and the Window.js runtime should never
deadlock.

The APIs exposed by Window.js are *not* stable at this time. The current APIs
are at a very early stage in their design and will evolve if simpler and more
performant designs make sense.

API stability is a longer-term goal.


Is Window.js secure?
--------------------

Window.js does *not* implement the security features that are common in web
browsers. It does *not* sandbox the Javascript code, and it exposes full access
to the filesystem as a native program would have.

Given that, Window.js should not misbehave in unexpected ways and doesn't
read or write any files or other metadata in the system, like the registry on
Windows, other than via the exposed APIs.


Is Window.js performant?
------------------------

Window.js bundles the [v8](http://v8.dev) Javascript engine, and performance
of Window.js programs is comparable to other Javascript programs built on v8
such as Chrome, [Node.js](https://nodejs.org/) and
[Electron](http://electronjs.org).

The expected performance should be good enough for interactive 2D graphics
programming and for exploratory 3D programming, but proper benchmarking hasn't
been done yet.

Javascript is a garbage collected language, and high frame-rate programs may
see frame drops due to GC pauses if they make several allocations per frame.

Programs that need the highest performance (such as numerical computing, AI
model training and similar) won't reach the performance that native code can
achieve.


Does Window.js support WebGL?
-----------------------------

Not yet. Supporting the WebGL 2 API is a goal.


Does Window.js support sound and audio?
---------------------------------------

Not yet. Introducing a sound API is a goal.


Does Window.js support networking?
----------------------------------

Not yet. Supporting the fetch API and sockets (UDP, TCP, Websockets) is a goal.


Does Window.js support mobile platforms?
----------------------------------------

No, and that is not a goal of the project.


Does Window.js support autoupdates?
-----------------------------------

No, though that could be eventually implemented by the application code.


Does Window.js support crash reporting?
---------------------------------------

No, though that could be eventually supported via a crash handling API.


Does Window.js support Typescript?
----------------------------------

Typescript is not integrated into Window.js yet.

In the short-term, Window.js should provide Typescript typing declarations
for its APIs.

Longer-term, Window.js could integrate with the Typescript compiler to support
reloading with F5 and showing any Typescript compilation errors directly in
the main window.


Does Window.js install any files?
---------------------------------

No. Window.js is distributed as a single, statically-linked binary and doesn't
make any modifications to the host computer: no files are written, read or
modified, no keys are edited in the Windows registry, etc.

Window.js has no telemetry and doesn't open any network connections.

The applications built on Window.js have access to the local filesystem APIs
(and, eventually, to networking). Their usage of those APIs is outside the scope
of Window.js.


What kind of programs can be built on Window.js?
------------------------------------------------

* 2D and 3D exploratory programs, using the [2D canvas](/doc/canvas) and
  WebGL APIs.
* Interactive 2D applications like [Scratch](https://scratch.mit.edu/) and
  [Processing](https://processing.org/).
* Cross-platform 2D games for desktop platforms.
* Experimenting with 2D algorithms for layout engines, widget libraries, etc.
* A toy web browser and HTML renderer, as a learning exercise.


What kind of programs should not be built on Window.js?
-------------------------------------------------------

* **Security**. Window.js has no protections against malicious third-party code
  and shouldn't be used as a generic runtime for untrusted code.
* Highest performance. Javascript is compiled just-in-time, has a
  garbage-collected managed heap and is single threaded.
* Text-heavy programs. Window.js doesn't do any text layout and doesn't have
  any text editing widgets (though such a library could be built).
* A generic web browser to be distributed to end users.


How does Window.js compare to a web browser?
--------------------------------------------

* Window.js doesn't support HTML nor CSS, and doesn't have a DOM.
* Window.js doesn't have browser components like the URL bar, tabs, or menus.
* Window.js gives full control of the native window to the application.
* Window.js has **no security**: no same-origin policy, no sandbox, no
  protections against malicious code. Window.js programs have unrestricted
  access to the filesystem.
* Window.js has more convenient APIs for desktop applications.
* Window.js doesn't have the tooling that supports web development, like the
  DOM inspector. It has a very simplistic console for development.


How does Window.js compare to Node.js?
--------------------------------------

* Window.js and Node.js are very similar. Whereas Node.js was designed to build
  web servers and command-line tools, Window.js was designed to build 2D
  applications.
* Node.js was built before Promises and async/await, and its APIs rely heavily
  on callbacks. Window.js APIs use Promises.
* Window.js is a smaller binary and has less dependencies than Node.js.
* Window.js does *not* support NPM or `node_modules`.
* Window.js doesn't use node modules nor `require()`. Window.js supports ES6
  modules and `async import()`.
* Window.js does not support headless programs yet. This is planned for
  [subprocesses](/doc/process), and Window.js programs can run with an
  [invisible](/doc/window#window.visible) window.
* Window.js doesn't have a large community nor wide documentation and plugins
  as Node.js does.


How does Window.js compare to Electron?
---------------------------------------

* Electron is a mature product that has been deployed widely. Window.js is an
  untested and recent open-source program.
* Electron has very rich APIs for building web pages, widgets, text layout, and
  everything else offered by modern web APIs. Window.js doesn't have any of
  those tools or APIs.
* Window.js is a smaller binary with less dependencies than Electron.
* Window.js applications run in a single process. The Javascript code for the UI
  and any other operations runs in the same thread. I/O and other blocking APIs
  internally use worker threads and pass the results back to Javascript using
  Promises. Window.js supports [subprocesses](/doc/process).
* Window.js doesn't have the development tooling that Electron offers like the
  DOM inspector, CSS editor, etc.
* Window.js doesn't have a large community nor wide documentation and plugins
  as Electron does.


How does Window.js compare to Processing and p5.js?
---------------------------------------------------

Window.js could be a great way to execute [Processing](https://processing.org/)
programs on the desktop, by porting [p5.js](https://p5js.org/).

Window.js aligns well with the goals of Processing and p5.js: make graphics
programming more accessible and easier to learn.

Processing has better documentation and many examples on the web, and a large
community.

Window.js provides a better way to bundle the app as a standalone application,
and has more APIs for desktop integration.

There is [experimental support](/about/processing) to run p5.js scripts in
Window.js.
