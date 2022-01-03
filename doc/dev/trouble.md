---
layout: development
title: Window.js | Troubleshooting
---

Troubleshooting
===============


Error: out is not a directory
-----------------------------

The cmake build must be configured before building.
See [here](/dev/build#configuring-windowjs).


CMake Error at libraries/CMakeLists.txt:9 (add_subdirectory): add_subdirectory given source "glad" which is not an existing directory.
-------------------------------------------------

The Window.js dependencies must be fetched before the build.
See [here](/dev/checkout#4-fetching-the-dependencies).


You must installWindows 10 SDK version 10.0.19041.0 including the "Debugging Tools for Windows" feature.
-------------------

See [these steps](https://stackoverflow.com/questions/66710286/you-must-installwindows-10-sdk-version-10-0-19041-0-including-the-debugging-too).
