---
layout: development
title: Window.js | Memory
---

Memory
======

The [LLVM AddressSanitizer](https://clang.llvm.org/docs/AddressSanitizer.html)
can be used to run an instrumented version of Window.js that looks for memory
leaks, uses-after-free, uninitialized reads and other memory bugs.

This requires a debug build on Linux with Clang.

First, edit `src/CMakeLists.txt` and enable the `-fsanitize=address` options.
Then make a full rebuild of Window.js in `Debug` mode.

Some of the leaks detected by `ASAN` come from external library and are just
distracting; they can be suppressed by setting this environment variable:

```shell
$ export LSAN_OPTIONS=suppressions=libraries/asan-linux-suppressions.txt
```

Now just run `windowjs` as usual, and `ASAN` will report any memory issues found
at shutdown.
