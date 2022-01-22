---
layout: development
title: Window.js | Tests
---

Tests
=====

The automated tests are in the `tests/` directory and are executed with
Window.js itself:

```shell
$ windowjs.exe tests/run_tests.js
```


Running tests
-------------

The runner indicates whether any tests failed, and show the stack trace for
each failure.

It support two optional parameters:

{:.strings}
| file | A RegExp pattern. Only matching filenames will be executed. |
| test | A RegExp pattern. Only matching test names will run. |

For example, if test `functionName` in `test_file.js` is failing, then these
flags can be used to run that single test:

```shell
$ windowjs.exe tests/run_tests.js -- test_file.js functionName
```


Pixel diffs
-----------

Tests can compare the pixels in their canvas to an image with the expected
output (the *golden* screenshot image).

The [canvas](https://github.com/windowjs/windowjs/blob/main/tests/canvas.js)
tests compare their output to the image rendered by Chrome, using the same code.

The [canvas.html](https://github.com/windowjs/windowjs/blob/main/tests/canvas.html)
tool generates goldens in Chrome directly, by executing the same test code.
It needs a web server to run:

```shell
$ npm install http-server
$ npx http-server
```

This starts a web server in the current directory, listening on port 8080.

Navigating to
[http://localhost:8080/tests/canvas.html](http://localhost:8080/tests/canvas.html)
shows an HTML `<canvas>` with the output of each test, and the golden image
next to it.

If that tests has executed and *failed*, then the test output is also shown
next to the golden. These output files should *not* be checked in.


Details
-------

The main runner will execute each Javascript source under `tests/` in its own
subprocess. It runs up to [Process.cpus](/doc/process#Process.cpus) test files
in parallel.

Each test is a module and all of its exported functions will be executed as a
test. The test fails if the function throws, or if it returns a Promise that
gets rejected (e.g. async functions that throw).

Test files can import `tests/lib.js` to use common test utilities.
