---
layout: documentation
title: Window.js | Window
events:
  - blur
  - click
  - close
  - drop
  - focus
  - keydown
  - keypress
  - keyup
  - maximize
  - minimize
  - mousedown
  - mouseenter
  - mouseleave
  - mousemove
  - mouseup
  - resize
  - restore
  - wheel
object-name: window
object-properties:
  - canvas
  - cursor
  - cursorOffsetX
  - cursorOffsetY
  - decorated
  - focused
  - fonts
  - fullscreen
  - frameBottom
  - frameLeft
  - frameRight
  - frameTop
  - height
  - icon
  - js
  - keepAspectRatio
  - maximized
  - minimized
  - resizable
  - title
  - visible
  - vsync
  - width
  - x
  - y
  - debug.profileFrameTimes
  - debug.showOverlayConsole
  - debug.showOverlayConsoleOnErrors
  - screen.availHeight
  - screen.availWidth
  - screen.height
  - screen.width
object-methods:
  - addEventListener
  - close
  - focus
  - getClipboardText
  - loadFont
  - maximize
  - minimize
  - open
  - removeEventListener
  - requestAttention
  - restore
  - setClipboardText
---

Window
======

The main application window.

The `window` object is available on startup and can be used to customize the
window before the [first frame](/doc/runtime#first-frame) gets shown to the
user.

There is exactly one window per Window.js process.
Use [subprocesses](/doc/process) to create additional windows in their own
Window.js process.


{% include event name="blur" %}

This event is sent by the main window when it loses the input focus.


{% include event name="click" %}

This event is sent by the main window when it receives a mouse click.

The event listener receives an Object with these properties:

{: .parameters}
| x       | number | The x location in the window where the event occurred.    |
| y       | number | The y location in the window where the event occurred.    |
| clientX | number | The x location in the window where the event occurred.    |
| clientY | number | The y location in the window where the event occurred.    |
| button  | number | The button that triggered the event: 0 for the left button, 1 for the middle button, and 2 for the right button. |


{% include event name="close" %}

This event is sent by the main window when it receives a request to be
closed. For example, if the user clicks on the close button in the native UI.

By default, Window.js terminates when the close request is received.

The main application may return `true` from the "close" event listener to
prevent the termination. In that case, it's up to the application code to
eventually close by calling [window.close](/doc/window#window.close).


{% include event name="drop" %}

This event is sent by the main window when one or more files are dragged and
dropped on it.

The event listener receives an Object with this property:

{: .parameters}
| files | string[] | List of file paths that were dropped into the window.     |


{% include event name="focus" %}

This event is sent by the main window when it receives the input focus.


{% include event name="keydown" %}

This event is sent by the main window when it receives a key down event.

The event listener receives an Object with these properties:

{: .parameters}
| key      | string  | The [key value](https://developer.mozilla.org/en-US/docs/Web/API/KeyboardEvent/key/Key_Values) identifying the key that triggered the event. |
| code     | string  | The [code value](https://developer.mozilla.org/en-US/docs/Web/API/KeyboardEvent/code/code_values) identifying the key that triggered the event. |
| repeat   | boolean | Whether this event is a repeat of a previous "keydown". |
| altKey   | boolean | Whether the `Alt` key was pressed.                      |
| ctrlKey  | boolean | Whether the `Ctrl` key was pressed.                     |
| metaKey  | boolean | Whether the `Meta` key was pressed.                     |
| shiftKey | boolean | Whether the `Shift` key was pressed.                    |
| location | number  | The [keyboard location](https://developer.mozilla.org/en-US/docs/Web/API/KeyboardEvent/location) of the `Alt`, `Ctrl`, `Meta` and `Shift` keys, if the event was triggered by one of those. |


{% include event name="keypress" %}

This event is sent by the main window when it receives a key press event.

A key press is usually triggered by one or more key downs, and represents the
final character that was input by the user. This is used to input composite
characters, or characters that require multiple keystrokes to input.

The event listener receives an Object with this property:

{: .parameters}
| code | number | The Unicode codepoint for the character that was input.      |


{% include event name="keyup" %}

This event is sent by the main window when it receives a key up event.

The event listener receives an Object with these properties:

{: .parameters}
| key      | string  | The [key value](https://developer.mozilla.org/en-US/docs/Web/API/KeyboardEvent/key/Key_Values) identifying the key that triggered the event. |
| code     | string  | The [code value](https://developer.mozilla.org/en-US/docs/Web/API/KeyboardEvent/code/code_values) identifying the key that triggered the event. |
| altKey   | boolean | Whether the `Alt` key was pressed.                      |
| ctrlKey  | boolean | Whether the `Ctrl` key was pressed.                     |
| metaKey  | boolean | Whether the `Meta` key was pressed.                     |
| shiftKey | boolean | Whether the `Shift` key was pressed.                    |
| location | number  | The [keyboard location](https://developer.mozilla.org/en-US/docs/Web/API/KeyboardEvent/location) of the `Alt`, `Ctrl`, `Meta` and `Shift` keys, if the event was triggered by one of those. |


{% include event name="maximize" %}

This event is sent by the main window when it becomes maximized.


{% include event name="minimize" %}

This event is sent by the main window when it becomes minimized.


{% include event name="mousedown" %}

This event is sent by the main window when it receives a mouse button down
event.

The event listener receives an Object with these properties:

{: .parameters}
| x       | number | The x location in the window where the event occurred.    |
| y       | number | The y location in the window where the event occurred.    |
| clientX | number | The x location in the window where the event occurred.    |
| clientY | number | The y location in the window where the event occurred.    |
| button  | number | The button that triggered the event: 0 for the left button, 1 for the middle button, and 2 for the right button. |


{% include event name="mouseenter" %}

This event is sent by the main window when the mouse cursor enters its displayed
area.


{% include event name="mouseleave" %}

This event is sent by the main window when the mouse cursor leaves its displayed
area.


{% include event name="mousemove" %}

This event is sent by the main window when the mouse cursor moves within its
displayed area.

The event listener receives an Object with these properties:

{: .parameters}
| x       | number | The x location in the window where the event occurred.    |
| y       | number | The y location in the window where the event occurred.    |
| clientX | number | The x location in the window where the event occurred.    |
| clientY | number | The y location in the window where the event occurred.    |
| offsetX | number | The x location in the window where the event occurred.    |
| offsetY | number | The y location in the window where the event occurred.    |


{% include event name="mouseup" %}

This event is sent by the main window when it receives a mouse button up event.

The event listener receives an Object with these properties:

{: .parameters}
| x       | number | The x location in the window where the event occurred.    |
| y       | number | The y location in the window where the event occurred.    |
| clientX | number | The x location in the window where the event occurred.    |
| clientY | number | The y location in the window where the event occurred.    |
| button  | number | The button that triggered the event: 0 for the left button, 1 for the middle button, and 2 for the right button. |


{% include event name="resize" %}

This event is sent by the main window when it is resized by the user or the
platform.

**Note:** the size of the canvas is automatically adjusted when the window is
resized. If the main application supports [resizing](#window.resizable) then it
should also listen for "resize" events and draw a full frame after each resize.
Window.js will also invoke any pending
[requestAnimationFrame](/doc/global#requestAnimationFrame) callbacks when the
window is resized.


{% include event name="restore" %}

This event is sent by the main window when it becomes restored, after having
been maximized or minimized before.


{% include event name="wheel" %}

This event is sent by the main window when it receives a mouse wheel event.

The event listener receives an Object with these properties:

{: .parameters}
| deltaX | number | The amount that was scrolled in the horizontal axis.       |
| deltaY | number | The amount that was scrolled in the vertical axis.         |


{% include property object="window" name="canvas"
   type="CanvasRenderingContext2D"
%}

The 2D graphics drawing API, documented in the [canvas API](/doc/canvas).

There is one `canvas` object per `window`, and this object is available on
startup.


{% include property object="window" name="cursor"
   type="string|ImageData|null" %}

The cursor to show for the mouse pointer.

Setting to `null` resets to the default cursor.

A `string` value can be used to load one of the standard cursors:

{: .strings}
| `arrow`     | The default cursor arrow.                                      |
| `ibeam`     | The `I` beam, usually shown for text input fields.             |
| `crosshair` | A crosshair.                                                   |
| `hand`      | A pointing hand, usually shown in browser over hyperlinks.     |
| `hresize`   | A horizontal resizing indicator.                               |
| `vresize`   | A vertical resizing indicator.                                 |
| `hidden`    | Hides the cursor when it's inside the main window.             |
| `locked`    | Hides and locks the cursor in the main window.                 |

The cursor can also be set to an [ImageData](/doc/imagedata) instance, to show
a custom image.


{% include property object="window" name="cursorOffsetX" type="number" %}

The horizontal offset of the hotspot of custom cursors. This is only used for
custom cursors, when [cursor](#window.cursor) is set to an `ImageData` instance.


{% include property object="window" name="cursorOffsetY" type="number" %}

The vertical offset of the hotspot of custom cursors. This is only used for
custom cursors, when [cursor](#window.cursor) is set to an `ImageData` instance.


{% include property object="window" name="decorated" type="boolean" %}

Whether the native platform window should have native borders and title bars.
Defaults to true.


{% include property object="window" name="focused" type="boolean" %}

Whether the window is currently focused.


{% include property object="window" name="fonts" type="string[]" %}

The list of fonts available in the platform. These font names can be used
with [window.canvas.font](/doc/canvas#canvas.font).

Use [loadFont](#window.loadFont) to load custom fonts.


{% include property object="window" name="fullscreen" type="boolean" %}

Whether the main window should run in fullscreen mode. Defaults to false.


{% include property object="window" name="frameBottom" type="number" %}

The size, in pixels, of any decoration at the bottom of the native window frame.


{% include property object="window" name="frameLeft" type="number" %}

The size, in pixels, of any decoration at the left of the native window frame.


{% include property object="window" name="frameTop" type="number" %}

The size, in pixels, of any decoration at the top of the native window frame.


{% include property object="window" name="frameRight" type="number" %}

The size, in pixels, of any decoration at the right of the native window frame.


{% include property object="window" name="height" type="number" %}

The height, in pixels, of the native window.


{% include property object="window" name="icon" type="[ImageData]" %}

A list of [ImageData](/doc/imagedata) images to use as the window icon.

The image closest to the size desired by the system is selected. If the list
of images is empty then the window reverts to its default icon. Good sizes for
the icons include 16x16, 32x32 and 48x48.

**macOS**: the dock icon will be the same as the application bundle's icon.


{% include property object="window" name="js" type="???" %}

What could this be?


{% include property object="window" name="keepAspectRatio" type="boolean" %}

Whether the aspect ratio of the current [width](#window.width) and
[height](#window.height) should be kept during resizes (if the window is
[resizable](#window.resizable)).


{% include property object="window" name="maximized" type="boolean" %}

Whether the window is maximized.


{% include property object="window" name="minimized" type="boolean" %}

Whether the window is minimized.


{% include property object="window" name="resizable" type="boolean" %}

Whether the native window can be resized by the user. Defaults to true.


{% include property object="window" name="title" type="string" %}

The title of the native window. Defaults to the filename of the initial
module.


{% include property object="window" name="visible" type="boolean" %}

Whether the native window is visible.

This is initially set to true, but the window is only shown after the initial
module has finished executing and the [first frame](/doc/runtime#first-frame)
has been drawn. The application can set `window.visible` to false to keep
the window invisible until a later stage.


{% include property object="window" name="vsync" type="boolean" %}

Whether to wait for the display vsync before advancing to the next frame.
Defaults to true.

This is only relevant for applications that
[render continuously](/doc/global#requestAnimationFrame). By default, the number
of frames rendered per second matches the monitor refresh rate (for example,
Window.js draws at 60 FPS on 60 Hz monitors).

Setting `window.vsync` to `false` disables waiting for vsync, and draws frames
as fast as the graphics card can swap the main framebuffer.


{% include property object="window" name="width" type="number" %}

The width, in pixels, of the native window.


{% include property object="window" name="x" type="number" %}

The horizontal position, in pixels, of the native window in its current monitor.


{% include property object="window" name="y" type="number" %}

The vertical position, in pixels, of the native window in its current monitor.


{% include property object="window.debug" name="profileFrameTimes"
   type="boolean" %}

Whether to continuously log [frame times](/doc/console#frame-times) to the
console.


{% include property object="window.debug" name="showOverlayConsole"
   type="boolean" %}

Whether the overlay console is visible in the main window. It can also be
toggled with `F4`.


{% include property object="window.debug" name="showOverlayConsoleOnErrors"
   type="boolean" %}

Whether the overlay console is shown automatically whenever an error is logged.


{% include property object="window.screen" name="availHeight" type="number" %}

The total height available for non-fullscreen windows, in pixels.

This can be as much as the [monitor height](#window.screen.height).
Less than the total height may be available if the native platform has a
native menubar at the top or at the bottom of the screen, for example.


{% include property object="window.screen" name="availWidth" type="number" %}

The total width available for non-fullscreen windows, in pixels.

This can be as much as the [monitor width](#window.screen.width).
Less than the total width may be available if the native platform has a
native menubar on the side, for example.


{% include property object="window.screen" name="height" type="number" %}

The height of the current monitor, in pixels.


{% include property object="window.screen" name="width" type="number" %}

The width of the current monitor, in pixels.


{% include method object="window" name="addEventListener"
   type="(string, Function) => void"
%}

`addEventListener` registers a listener callback to receive input events.

```javascript
window.addEventListener('click', (event) {
  console.log(`Click at ${event.x}, ${event.y}`);
});
```


{% include method object="window" name="close" type="() => void" %}

Closes the main window. This results in terminating the main program.


{% include method object="window" name="focus" type="() => void" %}

Requests focus for the main window.

See [requestAttention](#window.requestAttention) for a less disruptive way of
notifying the user that an important event has occurred.


{% include method object="window" name="getClipboardText" type="() => string" %}

Returns the current content of the clipboard.


{% include method object="window" name="loadFont"
   type="(string, string) => Promise<void>" %}

Loads a font from a local file, and gives it a name that can be used with
the [window.canvas.font](/doc/canvas#canvas.font) property.

{: .parameters}
| path | string | The path to the local font file to load.                     |
| name | string | The name of the font.                                        |


{% include method object="window" name="maximize" type="() => void" %}

Maximizes the window.


{% include method object="window" name="minimize" type="() => void" %}

Minimizes the window.


{% include method object="window" name="open" type="(string) => void" %}

Opens the given file or URL in the default system handler.

For example, `window.open('http://windowjs.org')` opens the URL in the default
browser.


{% include method object="window" name="removeEventListener"
   type="(string, Function) => void"
%}

Removes an event listener that has previously been registered via
[window.addEventListener](#window.addEventListener).


{% include method object="window" name="requestAttention" type="() => void" %}

Highlights the specified window, on systems that support it.


{% include method object="window" name="restore" type="() => void" %}

Restores the window to its size before being minimized or maximized.


{% include method object="window" name="setClipboardText"
   type="(string) => void"
%}

Sets the content of the clipboard.
