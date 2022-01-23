---
layout: development
title: Window.js | Build
---

Build
=====

Window.js can be built after [checking out the sources](/dev/checkout) and
setting up the build environment.

The build involves four steps:

1.  Building Skia
2.  Building v8
3.  Configuring the Window.js build
4.  Building Window.js


1 Building Skia
-------------

**Windows**

```
$ libraries\build_skia.bat
```

**Linux and macOS**

```
$ ./libraries/build_skia.sh
```


2 Building v8
-----------

**Windows**

```
$ libraries\build_v8.bat
```

**Linux and macOS**

```
$ ./libraries/build_v8.sh
```


3 Configuring Window.js
---------------------

Run this once to set up the build directory, *`out`*:

```shell
$ cmake -S. -B out -DCMAKE_BUILD_TYPE=Release -G Ninja
```


4 Building Window.js
------------------

This is the main build command, to build Window.js and rebuild again after code
modifications:

```shell
$ cmake --build out
```


Running Window.js
-----------------

Try running some of the examples once the build is finished:

```shell
$ out/windowjs examples/breakout.js
```
