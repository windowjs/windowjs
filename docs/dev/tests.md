---
layout: development
title: Window.js | Tests
---

Tests
=====

The automated tests are in the `tests/` directory and are executed with
Window.js itself:

```shell
$ windowjs.exe tests/run_tests.js --log
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
$ windowjs.exe tests/run_tests.js --log -- test_file.js functionName
```


Details
-------

The main runner will execute each Javascript source under `tests/` in its own
subprocess. It runs up to [Process.cpus](/doc/process#Process.cpus) test files
in parallel.

Each test is a module and all of its exported functions will be executed as a
test. The test fails if the function throws, or if it returns a Promise that
gets rejected (e.g. async functions that throw).

The tests are executed with the
[build workflows](https://github.com/windowjs/windowjs/actions). The workflows
fail if the tests fail.

Test file can import `tests/lib.js` to use common test utilities.
