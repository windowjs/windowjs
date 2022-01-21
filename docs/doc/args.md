---
layout: documentation
title: Window.js | Command line
---

Command line
============

Window.js supports the following command-line arguments.


`--version`
-------

Outputs the git hash and tag of the Window.js build. This is also available
as [window.version](/doc/window#window.version).


`--no-log`
-------

By default, calls to `console.log` show their output in the
[console](/doc/console) and in the process stdout.

Passing `--no-log` also prevents console output to stdout.


`--profile-startup`
-------------------

Log startup times of various startup events to the [console](/doc/console).

This is used to profile the performance of internal operations in Window.js.


`--disable-dev-keys`
--------------------

Disables developer shortcuts, like `F5` to reload and `F1` to show the console.


`--`
----

The argument `--` indicates that any subsequent arguments should be passed
to the Javascript application.

Those arguments will be available to Javascript code in
[Process.args](/doc/process#Process.args).

This command line:

```shell
./windowjs main.js --disable-dev-keys -- hello world
```

With this Javascript content in `main.js`:

```javascript
for (let arg of Process.args) {
  console.log('Argument: ' + arg);
}
```

Outputs the two arguments passed to the Javascript process:

```
Argument: hello
Argument: world
```
