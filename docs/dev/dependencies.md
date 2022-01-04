---
layout: development
title: Window.js | Dependencies
---

Dependencies
============

The Window.js dependencies are listed in `.gclient` and get fetched by
the `gclient` tool, which also fetches the skia and v8 dependencies.

The main dependencies are listed at the top, in the `vars` object:

*  [GLAD](https://github.com/Dav1dde/glad.git)
*  [GLFW](https://github.com/glfw/glfw.git)
*  [Skia](https://skia.googlesource.com/skia.git)
*  [V8](https://chromium.googlesource.com/v8/v8.git)
*  [libuv](https://github.com/libuv/libuv.git)

The commit hash for each one is the version that gets checked out.

The Skia and v8 dependencies are copied *manually* to `.gclient`, for a couple
of reasons: it's easier to skip unnecessary (and large) dependencies, and
`gclient --no-history` makes smaller checkouts this way.


Reused dependencies from v8 and Skia
------------------------------------

Some of the v8 and Skia dependencies are also used by Window.js and its build
directly:

*  `libraries/v8/third_party/llvm-build` is an LLVM and clang installation that
   is used on the Windows build.

*  `libraries/v8/third_party/zlib` is used for compression support.


Hooks
-----

The hooks listed at the bottom of `.gclient` run after each `sync`.

They apply custom patches to the libraries, mainly to make them
build for Window.js.

*  `libraries/v8.patch`: fixes the build with clang.
*  `libraries/v8_build.patch`: fixes the build with clang.
*  `libraries/v8_build.py`: removes a stale file.
*  `libraries/glfw.patch`: from https://github.com/glfw/glfw/pull/1426.
*  `libraries/glad.patch`: makes the build quiet.


Updating v8
-----------

*  Revert the v8 patches:
   ```shell
$ cd libraries/v8
$ git checkout HEAD .
$ cd build
$ git checkout HEAD .
```
*  Comment out the hooks that patch v8.
*  Update the `v8_commit` version.
*  Sync gclient:
   ```shell
$ gclient sync --shallow --no-history -D -R
```
*  Apply the v8 patches manually, and update them if needed.
*  Verify that v8 builds, and fix as needed:
   ```shell
$ libraries/build_v8.sh
```
*  Update the v8 patches with any changes:
   ```shell
$ git diff --patch --color=never HEAD --output ../v8.patch
$ cd build
$ git diff --patch --color=never HEAD --output ../../v8_build.patch
```
*  Restore the hooks and verify that `gclient sync` applies the patches
   correctly:
   ```shell
$ gclient sync --shallow --no-history -D -R
```
*  Verify that v8 builds:
   ```shell
$ libraries/build_v8.sh
```
*  Verify that Window.js builds and run the tests.


Updating Skia
-------------

*  Update the skia commit in `.gclient`
*  Sync gclient:
   ```shell
$ gclient sync --shallow --no-history -D -R
```
*  Update the skia `deps` from `libraries/skia/DEPS` to `skia_deps` in `.gclient`
*  Sync gclient again
*  Build skia:
   ```
$ libraries/build_skia.sh
```
*  Verify that Window.js builds and run the tests.
