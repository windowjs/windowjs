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
--------------------------------------------------------------------------------

The Window.js dependencies must be fetched before the build.
See [here](/dev/checkout#4-fetching-the-dependencies).


You must installWindows 10 SDK version 10.0.19041.0 including the "Debugging Tools for Windows" feature.
--------------------------------------------------------------------------------

See [these steps](https://stackoverflow.com/questions/66710286/you-must-installwindows-10-sdk-version-10-0-19041-0-including-the-debugging-too).


lld-link: error: could not open 'DbgHelp.lib': no such file or directory
------------------------------------------------------------------------

Run `setup_build_env` before building with `cmake`.


Operation did not complete successfully because the file contains a virus or potentially unwanted software.
--------------------------------------------------------------------------------

Windows Defender may detect a false positive in the `windowjs.exe` binary, also
during development. One option to disable this warning, and prevent Windows from
deleting the file, is to add the `windowjs` checkout directory to the exclusion
list of Windows Defender:

*  Open "Windows Security" via the Windows search box (Windows key + S)
*  Open "Virus & threat protection" on the right
*  Open "Manage Settings" under "Virus & threat protection settings"
*  Open "Add or remove exclusions" under "Exclusions"
*  Click on "Add an exclusion" to add the `windowjs` checkout directory.
