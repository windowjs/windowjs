---
layout: documentation
title: Window.js | Command line
---

Command line
============

Window.js supports the following command-line arguments.


`--log`
-------

By default, calls to `console.log` show their output in the
[console](/doc/console) but not in the process stdout or stderr.

Passing `--log` also logs console output to `stderr`.


`--child-log`
-------------

By default, [child processes](/doc/process) don't log to `stderr`.

Passing `--child-log` makes child processes copy their output to `stdout` and
`stderr` to the parent process `stdout` and `stderr`.


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
./windowjs main.js --log -- hello world
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
