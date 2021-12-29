---
layout: development
title: Window.js | Checkout
---

Checkout
========

Window.js can be built on Windows, Mac and Linux.

A [build](/dev/build) of Window.js requires the following steps, with details
below:

1. Install build tools for the corresponding platform
2. Checkout the Window.js [repository](https://github.com/windowjs/windowjs)
   from Github
3. Setup the build environment
4. Fetch Window.js dependencies
5. [Build](/dev/build)

**Note**: the Skia and v8 checkouts and builds are very large. Expect at least
14GB of disk usage after a full checkout and build.

1 Install build tools
---------------------

These tools must be installed and available in the `PATH` in a terminal.

*  git
*  [depot_tools](https://commondatastorage.googleapis.com/chrome-infra-docs/flat/depot_tools/docs/html/depot_tools_tutorial.html#_setting_up) from Chrome
*  the `gn.exe` or `gn`  binary in the `PATH`; the version from `depot_tools` doesn't work.
   Download [here](https://gn.googlesource.com/gn/+/refs/heads/main#getting-a-binary)
*  [CMake](https://cmake.org/)


**Windows**

*  [LLVM](http://llvm.org) 12 or later
*  [Visual Studio 2022](https://visualstudio.microsoft.com/)
*  [Windows 10 SDK](https://developer.microsoft.com/en-us/windows/downloads/windows-sdk/) (10.0.19041.0)
*  [Python](https://python.org)


**Linux**

*  clang++-11 or later
*  libwebp-dev


**Mac**

*  [Xcode](https://developer.apple.com/xcode/)


2 Checkout the repository
-------------------------

```shell
$ git checkout https://github.com/windowjs/windowjs
```


3 (Windows) setup the build environment
---------------------------------------

Windows requires additional setup to have a build environment with the x64
native tools, and additional settings for the v8 and Skia builds.

For terminals using the Windows command prompt:

```
libraries\setup_build_env.bat
```

For terminals using Powershell:

```
libraries\setup_build_env.ps1
```


3 (Linux) setup the build environment
---------------------------------------

Window.js requires clang to build. The `CC` and `CXX` environment variables
must be set correctly:

```shell
$ export CC=/usr/bin/clang
$ export CXX=/usr/bin/clang++
```


4 Fetching the dependencies
---------------------------

The `gclient` tool (from the Chrome `depot_tools`) is used to fetch dependencies
for Skia and v8.

```shell
$ gclient sync --shallow --no-history -D -R
```

This command can take several minutes to complete.

5 Build
-------

See the [build](/dev/build) instructions.
