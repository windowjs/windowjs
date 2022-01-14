interface Window {

    /**
     * Whether to keep the window always on top of other windows.
     * 
     * This can also be toggled with `F6` during development.
     */
    alwaysOnTop: boolean;

    /**
     * The 2D graphics drawing API, documented in the [canvas API](/doc/canvas).
     * 
     * There is one `canvas` object per `window`, and this object is available on
     * startup.
     */
    readonly canvas: CanvasRenderingContext2D;

    /**
     * The cursor to show for the mouse pointer.
     * 
     * Setting to `null` resets to the default cursor.
     * 
     * A `string` value can be used to load one of the standard cursors:
     */
    cursor: Cursor | ImageData | null;

    /**
     * The horizontal offset of the hotspot of custom cursors. This is only used for
     * custom cursors, when [cursor](#window.cursor) is set to an `ImageData` instance.
     */
    cursorOffsetX: number;

    /**
     * The vertical offset of the hotspot of custom cursors. This is only used for
     * custom cursors, when [cursor](#window.cursor) is set to an `ImageData` instance.
     */
    cursorOffsetY: number;

    /**
     * Whether the native platform window should have native borders and title bars.
     * Defaults to true.
     */
    decorated: boolean;

    /**
     * Whether the window is currently focused.
     */
    readonly focused: boolean;

    /**
     * The list of fonts available in the platform. These font names can be used
     * with [window.canvas.font](/doc/canvas#canvas.font).
     * 
     * Use [loadFont](#window.loadFont) to load custom fonts.
     */
    readonly fonts: string[];

    /**
     * Whether the main window should run in fullscreen mode. Defaults to false.
     */
    fullscreen: boolean;

    /**
     * The size, in pixels, of any decoration at the bottom of the native window frame.
     */
    readonly frameBottom: number;

    /**
     * The size, in pixels, of any decoration at the left of the native window frame.
     */
    readonly frameLeft: number;

    /**
     * The size, in pixels, of any decoration at the top of the native window frame.
     */
    readonly frameTop: number;

    /**
     * The size, in pixels, of any decoration at the right of the native window frame.
     */
    readonly frameRight: number;

    /**
     * The height, in pixels, of the native window.
     */
    height: number;

    /**
     * Whether the aspect ratio of the current [width](#window.width) and
     * [height](#window.height) should be kept during resizes (if the window is
     * [resizable](#window.resizable)).
     */
    keepAspectRatio: boolean;

    /**
     * Whether the window is maximized.
     */
    readonly maximized: boolean;

    /**
     * Whether the window is minimized.
     */
    readonly minimized: boolean;

    /**
     * Whether the native window can be resized by the user. Defaults to true.
     */
    resizable: boolean;

    /**
     * On Retina displays, the window units may be different from the OpenGL (pixel)
     * units by a scaling factor, which is the value in `retinaScale`.
     * 
     * Window.js automatically converts all units to pixels, so that
     * [window.width](#window.width), [canvas.width](/doc/canvas#canvas.width) and
     * mouse [click events](#event-click) are all in pixel units and no further
     * conversions are needed.
     * 
     * Applications should still scale their content by
     * [devicePixelRatio](/doc/global#devicePixelRatio), to match the UI and text
     * sizes expected by the user.
     */
    readonly retinaScale: number;

    /**
     * The title of the native window. Defaults to the filename of the initial
     * module.
     */
    title: string;

    /**
     * The git hash and tag of the current Window.js build.
     */
    readonly version: string;

    /**
     * Whether the native window is visible.
     * 
     * This is initially set to true, but the window is only shown after the initial
     * module has finished executing and the [first frame](/doc/runtime#first-frame)
     * has been drawn. The application can set `window.visible` to false to keep\
     * the window invisible until a later stage.
     */
    visible: boolean;

    /**
     * Whether to wait for the display vsync before advancing to the next frame.
     * Defaults to true.
     * 
     * This is only relevant for applications that\
     * [render continuously](/doc/global#requestAnimationFrame). By default, the number
     * of frames rendered per second matches the monitor refresh rate (for example,
     * Window.js draws at 60 FPS on 60 Hz monitors).
     * 
     * Setting `window.vsync` to `false` disables waiting for vsync, and draws frames
     * as fast as the graphics card can swap the main framebuffer.
     */
    vsync: boolean;

    /**
     * The width, in pixels, of the native window.
     */
    width: number;

    /**
     * The horizontal position, in pixels, of the native window in its current monitor.
     */
    x: number;

    /**
     * The vertical position, in pixels, of the native window in its current monitor
     */
    y: number;

    readonly debug: {
        /**
         * Whether to continuously log [frame times](/doc/console#frame-times) to the
         * console.
         */
        profileFrameTimes: boolean;

        /**
         * Whether the overlay console is visible in the main window. It can also be
         * toggled with `F4`.
         */
        showOverlayConsole: boolean;

        /**
         * Whether the overlay console is shown automatically whenever an error is logged.
         */
        showOverlayConsoleOnErrors: boolean;
    };

    readonly screen: {
        /**
         * The total height available for non-fullscreen windows, in pixels.
         * 
         * This can be as much as the [monitor height](#window.screen.height).
         * Less than the total height may be available if the native platform has a
         * native menubar at the top or at the bottom of the screen, for example.
         */
        readonly availHeight: number;

        /**
         * The total width available for non-fullscreen windows, in pixels.
         * 
         * This can be as much as the [monitor width](#window.screen.width).
         * Less than the total width may be available if the native platform has a
         * native menubar on the side, for example.
         */
        readonly availWidth: number;

        /**
         * The height of the current monitor, in pixels.
         */
        readonly height: number;

        /**
         * The width of the current monitor, in pixels.
         */
        readonly width: number;
    };

    /**
     * `addEventListener` registers a listener callback to receive input events.
     * 
     * @param type 
     * @param listener 
     */
    addEventListener(type: "close", listener: () => boolean): void;
    addEventListener<K extends keyof WindowEventHandlersMap>(type: K, listener: (event: WindowEventHandlersMap[K]) => void): void;
    addEventListener(type: string, listener: () => void): void;

    /**
     * Closes the main window. This results in terminating the main program.
     */
    close(): void;

    /**
     * Requests focus for the main window.
     * 
     * See [requestAttention](#window.requestAttention) for a less disruptive way of
     * notifying the user that an important event has occurred.
     */
    focus(): void;

    /**
     * Returns the current content of the clipboard.
     */
    getClipboardText(): string;

    /**
     * Loads a font from a local file, and gives it a name that can be used with
     * the [window.canvas.font](/doc/canvas#canvas.font) property.
     * 
     * @param path  The path to the local font file to load.
     * @param name  The name of the font.
     */
    loadFont(path: string, name: string): Promise<void>;

    /**
     * Maximizes the window.
     */
    maximise(): void;

    /**
     * Minimizes the window.
     */
    minimise(): void;

    /**
     * Removes an event listener that has previously been registered via
     * [window.addEventListener](#window.addEventListener).
     * 
     * @param type  The event type.
     * @param listener  The previously added listener.
     */
    removeEventListener(type: string, listener: Function): void;

    /**
     * Highlights the specified window, on systems that support it.
     */
    requestAttention(): void;

    /**
     * Restores the window to its size before being minimized or maximized.
     */
    restore(): void;

    /**
     * Sets the content of the clipboard.
     * @param text 
     */
    setClipboardText(text: string): void;
}

type Cursor = "arrow" | "ibeam" | "crosshair" | "hand" | "hresize" | "vresize" | "hidden" | "locked";

type Event = undefined;

interface MouseEvent {
    /** The x location in the window where the event occurred. */
    readonly x: number;
    /** The y location in the window where the event occurred. */
    readonly y: number;
    /** The x location in the window where the event occurred. */
    readonly clientX: number;
    /** The y location in the window where the event occurred. */
    readonly clientY: number;
}

interface ClickEvent extends MouseEvent {
    /** The button that triggered the event: 0 for the left button, 1 for the middle button, and 2 for the right button. */
    readonly button: 0 | 1 | 2;
}

interface MoveEvent extends MouseEvent {
    /** The x location in the window where the event occurred. */
    readonly offsetX: number;
    /** The y location in the window where the event occurred. */
    readonly offsetY: number;
}

interface DropEvent {
    /** List of file paths that were dropped into the window. */
    readonly files: string[];
}

interface KeyboardEvent {
    /** The [key value](https://developer.mozilla.org/en-US/docs/Web/API/KeyboardEvent/key/Key_Values) identifying the key that triggered the event. */
    readonly key: string;
    /** The [code value](https://developer.mozilla.org/en-US/docs/Web/API/KeyboardEvent/code/code_values) identifying the key that triggered the event. */
    readonly code: string;
    /** Whether the `Alt` key was pressed. */
    readonly altKey: boolean;
    /** Whether the `Ctrl` key was pressed. */
    readonly ctrlKey: boolean;
    /** Whether the `Shift` key was pressed. */
    readonly shiftKey: boolean;
    /** Whether the `Meta` key was pressed. */
    readonly metaKey: boolean;
    /** The [keyboard location](https://developer.mozilla.org/en-US/docs/Web/API/KeyboardEvent/location) of the `Alt`, `Ctrl`, `Meta` and `Shift` keys, if the event was triggered by one of those. */
    readonly location: number;
}

interface KeyDownEvent extends KeyboardEvent {
    /** Whether this event is a repeat of a previous "keydown". */
    readonly repeat: boolean;
}

interface KeyPressEvent {
    /** The Unicode codepoint for the character that was input. */
    readonly code: number;
}

interface WheelEvent {
    /** The amount that was scrolled in the horizontal axis. */
    readonly deltaX: number;
    /** The amount that was scrolled in the vertical axis. */
    readonly deltaY: number;
}

interface WindowEventHandlersMap {
    /** This event is sent by the main window when it loses the input focus. */
    "blur": Event,

    /** This event is sent by the main window when it receives a mouse click. */
    "click": ClickEvent;

    /**
     * This event is sent by the main window when it receives a request to be
     * closed. For example, if the user clicks on the close button in the native UI.
     * 
     * By default, Window.js terminates when the close request is received.
     * 
     * The main application may return `true` from the "close" event listener to
     * prevent the termination. In that case, it's up to the application code to
     * eventually close by calling [window.close](/doc/window#window.close).
     */
    "close": Event;

    /** 
     * This event is sent by the main window when one or more files are dragged and
     * dropped on it.
     */
    "drop": DropEvent;

    /** This event is sent by the main window when it receives the input focus. */
    "focus": Event;

    /** This event is sent by the main window when it receives a key down event. */
    "keydown": KeyDownEvent;

    /**
     * This event is sent by the main window when it receives a key press event.
     * 
     * A key press is usually triggered by one or more key downs, and represents the
     * final character that was input by the user. This is used to input composite
     * characters, or characters that require multiple keystrokes to input.
     */
    "keypress": KeyboardEvent;

    /** This event is sent by the main window when it receives a key up event. */
    "keyup": KeyboardEvent;

    /** This event is sent by the main window when it becomes maximized. */
    "maximize": Event;

    /** This event is sent by the main window when it becomes minimized. */
    "minimize": Event;

    /** This event is sent by the main window when it receives a mouse button down event. */
    "mousedown": ClickEvent;
    
    /** This event is sent by the main window when the mouse cursor enters its displayed area. */
    "mouseenter": Event;
    
    /** This event is sent by the main window when the mouse cursor leaves its displayed area. */
    "mouseleave": Event;
    
    /** This event is sent by the main window when the mouse cursor moves within its displayed area. */
    "mousemove": MoveEvent;
    
    /** This event is sent by the main window when it receives a mouse button up event. */
    "mouseup": ClickEvent;
    
    /**
     * This event is sent by the main window when it is resized by the user or the
     * platform.
     * 
     * **Note:** the size of the canvas is automatically adjusted when the window is
     * resized. If the main application supports [resizing](#window.resizable) then it
     * should also listen for "resize" events and draw a full frame after each resize.
     * Window.js will also invoke any pending
     * [requestAnimationFrame](/doc/global#requestAnimationFrame) callbacks when the
     * window is resized.
     */
    "resize": Event;
    
    /** This event is sent by the main window when it becomes restored, after having been maximized or minimized before. */
    "restore": Event;
    
    /** This event is sent by the main window when it receives a mouse wheel event. */
    "wheel": WheelEvent;
}
