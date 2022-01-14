# Console

Window.js has a simple console that is similar to the Javascript console
in the Chrome devtools, and allows experimenting quickly with Javascript
code and the Window.js APIs.


## Keyboard shortcuts

These shortcuts work in the main window and the console window.

| Shortcut | Description                                                                      |
|----------|----------------------------------------------------------------------------------|
| Escape   | Closes the main window.                                                          |
| F1       | Opens the console for the current window.                                        |
| F2       | Overlays FPS and memory usage stats in the main window.                          |
| F3       | Continuously logs frame times to the console. See [details](#frame-times) below. |
| F4       | Overlays console logs in the main window.                                        |
| F5       | Reloads the initial module and refreshes the main window.                        |
| F6       | Keeps the window always on top.                                                  |
| F8       | Saves a screenshot named ScreenshotN.png to the current directory.               |

These shortcuts work in the console window only.

| Shortcut | Description                                               |
|----------|-----------------------------------------------------------|
| Escape   | Closes the console window.                                |
| Ctrl+r   | Reloads the initial module and refreshes the main window. |
| Ctrl+q   | Closes the main window.                                   |
| Ctrl+c   | Copies the current input line to the clipboard.           |
| Ctrl+v   | Pastes the current clipboard to the input line.           |

## Input

Javascript code can be written and submitted via the console. It gets executed
in the context of the main window.

For example, starting with an empty window:

```shell
./windowjs
```

The console can be opened by pressing `F1`. Then typing this code in the
console updates the title of the main window:

```javascript
window.title = 'Hello world!'
```

## Frame times

If enabled with `F3` or by setting
[window.debug.profileFrameTimes](window.md#window.debug.profileFrameTimes),
then a log line will be printed after each frame is finished. Example:

```
Input 0.141 GC 0.001 JS 0.003 RAF 0.200 Swap 7.749 Total frame 8.345 FPS 119.826
```

Elapsed times in milliseconds.

| Option | Description                                                                                                                                                           |
|--------|-----------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| Input  | Time processing input events from the operating system. This does not include time in Javascript event listeners, except for [resize](window.md#event-resize) events. |
| GC     | Time waiting for the Javascript lock used by the garbage collector thread.                                                                                            |
| JS     | Time spent in Javascript events. This includes event listeners, [setTimeout](global.md#setTimeout) callbacks and resolve callbacks for `Promises`.                    |
| RAF    | Time spent in [requestAnimationFrame](global.md#requestAnimationFrame) callbacks.                                                                                     |
| Swap   | Time spent swapping frames. This usually waits for [vsync](window.md#window.vsync).                                                                                   |
| F5     | Reloads the initial module and refreshes the main window.                                                                                                             |
| F6     | Keeps the window always on top.                                                                                                                                       |
| F8     | Saves a screenshot named ScreenshotN.png to the current directory.                                                                                                    |
