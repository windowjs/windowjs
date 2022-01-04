---
layout: development
title: Window.js | Build configuration
---

Build configuration
===================

Additional cmake parameters are available to customize the build:

{: .strings}
| `-DCMAKE_BUILD_TYPE=Debug` | Builds with debug symbols. |
| `-DLLVM_DIR=c:/llvm` | **(Windows)** The LLVM installation directory. |

**Windows**: if `LLVM_DIR` is set then Window.js is built with clang; otherwise
it's built with MSVC.
