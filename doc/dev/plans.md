---
layout: development
title: Window.js | Plans
---

Plans
=====

Window.js is an open-source project and doesn't have an official roadmap.

This page lists potential future subprojects. Consider
[contributing](/dev/contributing) to the project and helping with these! Get
in [contact](/about/contact) to get started.


Build
-----

##### One-liner build

The current build involves multiple parts: fetching dependencies, building v8,
building skia, and then building Window.js.

Ideally, Window.js would build with a single `cmake` command, including fetching
any missing dependencies or updating as needed.


##### Compiler support

Window.js should support the major compilers: MSVC on Windows, and GCC on
Linux. Currently, Window.js has only been tested with LLVM.


##### Support for Mac arm64

New Macs have arm64 CPUs. They should be supported natively.


##### Continuous builds and tests

Ideally, every commit would trigger automated builds on each supported platform
and run the full test suite.


##### Reproducible builds

Ideally, building with the same compiler at the same checked out commit would
produce exacly the same Window.js binary, byte for byte.


##### Integrate with package managers

Make Window.js easy to install on Window, Mac and Linux via the more popular
package managers like chocolatey, homebrew, apt, rpm, etc.


Performance
-----------

##### Compiled binary size

Understanding the major contributions to the binary size and trimming it is
always a welcome contribution. The v8 library is particularly large.


##### GC pauses

Configuring and/or using v8 internally in a way that reduces or eliminates all
GC pauses.


##### Performance

Tune the v8 settings for maximal performance. For example, Window.js is not
suited to run third-party, untrusted code and can turn off mitigations for VM
exploits like Meltdown and Spectre.


##### AOT compilation

Instead of using JIT, force modules loaded by Window.js to be compiled during
startup or even ahead of time, to avoid JIT stalls later.


##### App startup performance

Window.js should paint the first frame as soon as possible.

Creating a window and the GL context on Windows is particularly expensive today.

See the [--profile-startup](/doc/args) command line flag to profile this.


Testing
-------

##### Test coverage

Window.js doesn't have as much test coverage as it should. Ideally, all APIs
would have coverage.

Testing should also cover performance: the startup time should not regress,
the time to produce reasonable frames should be consistent, etc.


##### Examples and demos

Sample code that demonstrates how to build useful applications with Window.js
is welcome. Especially code that is easy to read and understand, and that would
be helpful for new programmers.


##### Benchmarks

Similar to test coverage, ideally Window.js would also have benchmarks to track
performance over time.


##### Improve platform integration

There are always small details on each platform.

For example, the WGL context on Windows doesn't always show the first frame when
the native window is first shown to the user, causing a flicker. This is most
likely due to Aero.

Window.js should give consistent behavior on all platforms, and issues like this
one should be understood and fixed.


APIs
----

##### WebGL

Window.js should support the WebGL APIs.


##### Sound

Window.js should have a sound API.


##### Networking

Window.js should have a networking API. The minimal API would mimic the
[fetch API](https://developer.mozilla.org/en-US/docs/Web/API/Fetch_API) in
HTML5; further extensions should support TCP, UDP and WebSockets, both for
client and for server usage.


##### Crash reporting

Window.js should support collecting crash information and letting the
application code eventually report it, via a networking API.


##### Unicode support

Window.js should support text in any language.


##### Spawn native processes

To support writing a terminal emulator in Window.js?


##### Video playback

Support loading, displaying and controlling video.


##### Vulkan, WebGPU

Give access to low-level GPU programming APIs.


Features
--------

##### Support multiple monitors

This is untested.


##### Headless mode and restricted APIs

The [Process API](/doc/process) should support running subprocesses
without a window (headless), and should supporting restricting the APIs that
a subprocess can access (file, networking, etc.)


##### v8 snapshot support

v8 supports creating heap snapshots for faster startups. Window.js should
make it easy to create a snapshot and to load it. This feature is only worth
maintaining if it yields a reasonable speedup on startup.


##### FFI support

Window.js should have a foreign-function interface to make it possible to call
C functions loaded dynamically from DLLs. This makes it possible to call native
libraries from Window.js.


##### WASM support

v8 supports WASM, and Window.js should support it too.


##### Asset bundling

For ease of distribution, Window.js should support bundling Javascript sources
and any other asset files into a single `.zip` file, and start execution from
that `.zip` file later. This should work transparently for the application:
APIs like asset file loading and importing scripts should just work.

A step further would be to support merging the main scripts into the binary
itself, so that `windowjs.exe` could become a ready-to-distribute executable
for the main app. See [QuickJS](https://bellard.org/quickjs/) for an example.


Tools for Window.js users
-------------------------

##### Better tooling

Frame time graphs, CPU and heap profiles.


##### Better console

Better `console.dir` support, better input editing support, better object
inspection; `help()` as in Python to view API documentation in the console
would be great.

Support top-level `await` in the console.


##### Typescript integration

Make it easy to write Window.js application in Typescript.

Reloading with F5 should show any compilation errors in the main Window.js
window.


Tools for Window.js developers
------------------------------

##### Static analyzers

clang-format, lint, ASan, MSan, TSan.


Community
---------

##### Improve the documentation

Correct grammar; clearer and more succint documentation.

Documentation should be comprehensive: document every API available and all
of its variations.


##### Improve the website

Make the website clearer, easier to navigate.

Improve the design and color scheme.


##### Logo?

Have a logo for Window.js?


##### Help others

Answer any questions on StackOverflow, the mailing list and the discussion
forums.


##### Tutorials

There are excellent guides at
[webglfundamentals.org](https://webglfundamentals.org/) and
[learnopengl.com](https://learnopengl.com/) for getting started with
GL programming. Convert their code to run in Window.js and provide that code
as an example.
