---
layout: documentation
title: Window.js | Process
events:
  - exception
  - exit
  - log
  - message
class-name: Process
class-properties:
  - args
  - parent
class-methods:
  - exit
  - spawn
object-name: process
object-methods:
  - addEventListener
  - close
  - postMessage
  - removeEventListener
---

Process
=======

The `Process` API creates new processes that have their own window and
Javascript VM. Processes created this way can communicate with each other.
All processes terminate when the main process exits.

Every process can create a subprocess via [Process.spawn](#Process.spawn),
indicating the main Javascript module that the subprocess loads on startup.
[Process.spawn](#Process.spawn) returns a `Process` instance used for
communications with the subprocess.

Subprocesses have a similar handle to their parent process in
[Process.parent](#Process.parent).

Because each process runs in its own Javascript VM, inter-process communications
can only send and receive JSON objects and TypedArrays.

Messages are sent via [process.postMessage](#process.postMessage),
and received as ["message"](#event-message) events on the process handle.


{% include event name="exception" %}

Sent to parent processes when a child process throws an uncaught exception.
Event listeners for `"exception"` events must be added first via
[process.addEventListener](#process.addEventListener):

```javascript
const child = Process.spawn('child_code.js');

child.addEventListener('exception', (event) => {
    console.log('Exception in child process', event.message);
    for (let frame of event.stacktrace) {
      console.log(frame);
    }
});
```

The `event` object passed to the event listener has these properties:

{: .parameters}
| message    | string   | A string describing the exception.                   |
| stacktrace | string[] | The stack trace where the exception occurred.        |


{% include event name="exit" %}

Sent to parent processes when a child process terminates.
Event listeners for `"exit"` events must be added first via
[process.addEventListener](#process.addEventListener):

```javascript
const child = Process.spawn('child_code.js');

child.addEventListener('exit', (event) => {
    console.log('Child process terminated with status ' + event.status);
});
```

The `event` object passed to the event listener has these properties:

{: .parameters}
| error  | string? | A string describing the termination cause, in case of errors. |
| status | number  | The exit code of the child process.                       |


{% include event name="log" %}

Sent to parent processes when a child process logs to the `console`.
Event listeners for `"log"` events must be added first via
[process.addEventListener](#process.addEventListener):

```javascript
const child = Process.spawn('child_code.js');

child.addEventListener('log', (event) => {
    console.log('Child log: ' + event.message);
});
```

The `event` object passed to the event listener has these properties:

{: .parameters}
| message | string | The string that was logged from the child process.        |
| level   | string | The log level corresponding to the `console` method used: "debug", "log", "info", "warn" or "error". |


{% include event name="message" %}

Sent to a process handle when the corresponding process posts a message to the
current process via [process.postMessage](#process.postMessage). Event listeners
for [message](#event-message) events must be added first via
[process.addEventListener](#process.addEventListener).

To receive messages from a child process:

```javascript
const child = Process.spawn('child_code.js');

child.addEventListener('message', (event) =>
  console.log('Message from the child: ' + event);
});
```

To receive messages from the parent process (in a child process):

```javascript
Process.parent.addEventListener('message', (event) => {
  console.log('Message from the parent: ' + event);
});
```

The `event` object passed to the listeners is either a `JSON` object or a
`TypedArray`, and has the value that was passed to
[process.postMessage](#process.postMessage) in the other process.


{% include property class="Process" name="args" type="string[]" %}

The list of arguments passed to the process, via the command line or
[Process.spawn](#Process.spawn). Does not include the executable name nor
arguments processed by Window.js internally, like `--log`.

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


{% include property class="Process" name="parent" type="Process?" %}

A handle to the parent process. This is only present in child processes, as the
main process doesn't have a `parent`.

`Process.parent` can be used to call
[addEventListener](#process.addEventListener) to receive
[messages](#event-message) from the parent process, or to call
[postMessage](#process.postMessage) to send messages.


{% include method class="Process" name="exit" type="(number) => void" %}

Terminates the current process immediately, with the given status code value.

If the current process is a child process then its parent process will receive
an ["exit"](#event-exit) event in its handle for this process.

`process.exit()` is an abrupt termination that is appropriate for unrecoverable
errors. Normal process termination should call
[window.close](/doc/window#window.close) instead.


{% include method class="Process" name="spawn"
   type="(string, string[]?, Object?) => Process"
%}

`spawn` creates a new subprocess and returns a handle to the parent.

It takes one to three arguments:

{: .parameters}
| module  | string    | The initial Javascript module to load in the child process. |
| args    | string[]? | Optional list of string arguments to pass to the child process. They will be available in [Process.args](#Process.args) in the child process. |
| options | Object?   | Optional object with options to `spawn`.               |

The `options` object isn't used currently. It is meant to support disabling
features in the client, such as restricting filesystem access and running in
headless mode (without a window).


{% include method object="process" name="addEventListener"
   type="(string, Function) => void"
%}

`addEventListener` registers a listener callback to receive events in a given
process handle. Subprocess have a process handle to their parent processes
in [Process.parent](#Process.parent), and parent processes get handles to their
subprocesses returned from [Process.spawn](#Process.spawn).

Child processes receive only the [message](#event-message) event.

Parent processes receive [message](#event-message), [exit](#event-exit),
[exception](#event-exception) and [log](#event-log) events from their child
processes.

To receive messages from a child process:

```javascript
const child = Process.spawn('child_code.js');

child.addEventListener('message', (event) =>
  console.log('Message from the child: ' + event);
});
```

To receive messages from the parent process (in a child process):

```javascript
Process.parent.addEventListener('message', (event) => {
  console.log('Message from the parent: ' + event);
});
```


{% include method object="process" name="close" type="() => void" %}

Closes the handle to a child process and terminates it.

**Note**: child processes *can't* close their parents via
`Process.parent.close()`.


{% include method object="process" name="postMessage"
   type="(Json | TypedArray) => void"
%}

Sends a message to the process represented by this handle.

The message can be a `JSON` object or a `TypedArray`, and is received as the
single argument to the [message](#event-message) event listener in the other
process.

```javascript
const child = Process.spawn('child_code.js');

child.postMessage({type: 'hello', target: 'world'});
```

The child process can receive the message with a listener:

```javascript
Process.parent.addEventListener('message', (event) {
  if (event.type == 'hello') {
    console.log(`Received hello from parent to ${event.target}`);
  }
});
```

```javascript
Process.parent.addEventListener('message', (event) => {
  console.log('Message from the parent: ' + event);
});
```



{% include method object="process" name="removeEventListener"
   type="(string, Function) => void"
%}

Removes an event listener that has previously been registered via
[process.addEventListener](#Process.addEventListener).
