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


'gclient' is not recognized as an internal or external command, operable program or batch file.
----------

The Chrome
[depot_tools](https://commondatastorage.googleapis.com/chrome-infra-docs/flat/depot_tools/docs/html/depot_tools_tutorial.html#_setting_up)
must be in the `PATH`.


You must installWindows 10 SDK version 10.0.19041.0 including the "Debugging Tools for Windows" feature.
-------------------

See [these steps](https://stackoverflow.com/questions/66710286/you-must-installwindows-10-sdk-version-10-0-19041-0-including-the-debugging-too).
