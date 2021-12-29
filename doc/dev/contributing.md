---
layout: development
title: Window.js | Contributing
---

Contributing
============

Consider contributing to Window.js!

See how to get in [contact](/about/contact) to get started.

There are many ways to help the project:

* Improve performance of the main loop, drawing, GPU code, ...
* Add new APIs: webgl, sound, networking, ...
* Improve the developer tools: better console, better error diagnosis, ...
* Test on more platforms and file issues
* Port web libraries to Window.js: [Three.js](https://threejs.org/),
  [Pixi.js](https://pixijs.com/), [Processing (p5.js)](https://p5js.org/), ...
* Make better example apps
* Make interesting educational apps: a sample HTML renderer, a sample web
  browser, a widget engine, a game engine, ...
* Improve the build: automate more parts, integrate with Github Actions,
  support reproducible builds
* Improve distribution: signed binaries for Mac and Windows, integrate with
  Linux package managers, ...
* Improve the documentation! Better examples, guides, improve the text grammar,
  ...
* Help diagnose issues in the issue tracker and in the discussion forums

See the list of [planned features](/dev/plans) for ideas.

Long-term goals of the project:

*  As few steps as possible to run `hello_world.js`. Ideally, Window.js will be
   always distributed as a single binary.
*  Window.js should feel familiar for Web and Javascript developers. APIs should
   be similar to existing HTML5 APIs where it makes sense.
*  Window.js makes no security guarantees; it's a tool for developing desktop
   applications and it's up to the application to ensure any security
   requirements. Window.js is *not* suited to run unstrusted third-party code.
*  Window.js should start fast. Code that isn't used to draw the first frame
   should not delay the critical startup path.
*  Window.js should be a lean binary. Any dependencies have to add significant
   benefits to justify their weight.
*  Window.js should give good tooling for developers. The Chrome devtools are
   a great example of excellent tooling.
*  User-visible APIs must be documented and accessible online.
*  No telemetry, no files left on the user's device. Window.js doesn't do
   anything other than what is documented.
*  Window.js should be stable and reliable at runtime: memory usage is stable,
   no crashes, all failures get exposed to Javascript, etc.


What Window.js is not:

*  A stable programming environment. The APIs exposed by Window.js may change at
   any time. This should change in the future, but at the current project stage
   there are no guarantees.
*  Window.js doesn't support mobile platforms and doesn't aim to.
*  Window.js is not a game engine. While game engines can be built on top,
   Window.js doesn't bundle any particular engine.
*  Window.js is not a web browser. It won't support CSS or HTML natively,
   though CSS and HTML parsers and a renderer could be built on top.
*  Window.js doesn't integrate with `NPM` and other Javascript package managers.
