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
*  [CMake](https://cmake.org/) 3.15 or later


**Windows**

*  [Visual Studio 2022](https://visualstudio.microsoft.com/) (Choose the *Desktop development with C++* workflow in the installer)
*  [Windows 10 SDK](https://developer.microsoft.com/en-us/windows/downloads/windows-sdk/) (10.0.19041.0 -- can be installed with Visual Studio 2022)


**Linux**

*  `apt-get install cmake clang libfontconfig-dev libjpeg-turbo8-dev libwebp-dev libgl-dev libglx-dev libxrandr-dev xorg-dev`


**Mac**

*  [Xcode](https://developer.apple.com/xcode/)


2 Checkout the repository
-------------------------

```shell
$ git checkout https://github.com/windowjs/windowjs
```


3 Setup the build environment
-----------------------------

**Windows**

For Windows command prompt terminals, run this script:

```shell
$ libraries\setup_build_env.bat
```

For Windows Powershell terminals, run this script instead:

```shell
$ libraries\setup_build_env.ps1
```


**Linux and macOS**

```shell
$ source libraries/setup_build_env.sh
```


4 Fetching the dependencies
---------------------------

**Windows**

```shell
$ libraries\sync.bat
```


**Linux and macOS**

```shell
$ ./libraries/sync.sh
```


5 Build
-------

See the [build](/dev/build) instructions.
