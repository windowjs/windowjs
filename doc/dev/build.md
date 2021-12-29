---
layout: development
title: Window.js | Build
---

Build
=====

Window.js can be built after [checking out the sources](/dev/checkout).

The build involves four steps:

1.  Building Skia
2.  Building v8
3.  Configuring the Window.js build
4.  Building Window.js

Skia and v8 build with `gn` and `ninja`; Window.js builds with `cmake` and
`ninja`.


Building Skia
-------------

**Windows**

```
$ libraries\build_skia.bat <path to LLVM directory>
```

**Mac and Linux**

```
$ ./libraries/build_skia.sh
```


Building v8
-----------

**Windows**

```
$ libraries\build_v8.bat <path to LLVM directory>
```

**Mac and Linux**

```
$ ./libraries/build_v8.sh
```


Configuring Window.js
---------------------

```shell
$ cmake -S. -B out -DCMAKE_BUILD_TYPE=Release -G Ninja
```

Additional cmake parameters are available to customize the build:

{: .strings}
| `-DCMAKE_BUILD_TYPE=Debug` | Builds with debug symbols. |
| `-DLLVM_DIR=c:/llvm` | **(Windows)** The LLVM installation directory. |

**Windows**: if `LLVM_DIR` is set then Window.js is built with clang; otherwise
it's built with MSVC.


Building Window.js
------------------

```shell
$ cmake --build out
```

Running Window.js
-----------------

Try running some of the examples once the build is finished:

```shell
$ out/src/windowjs.exe examples/breakout.js
```

```shell
$ out/src/windowjs examples/breakout.js
```
