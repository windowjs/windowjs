/**
 * The `Process` API creates new processes that have their own window and
 * Javascript VM. Processes created this way can communicate with each other.
 * All processes terminate when the main process exits.
 * 
 * Every process can create a subprocess via {@link Process.spawn},
 * indicating the main Javascript module that the subprocess loads on startup.
 * {@link Process.spawn} returns a `Process` instance used for
 * communications with the subprocess.
 * 
 * Subprocesses have a similar handle to their parent process in
 * {@link Process.parent}.
 * 
 * Because each process runs in its own Javascript VM, inter-process communications
 * can only send and receive JSON objects and TypedArrays.
 * 
 * Messages are sent via {@link Process.postMessage},
 * and received as ["message"](#event-message) events on the process handle.
 */
interface Process {

    /**
     * `addEventListener` registers a listener callback to receive events in a given
     * process handle. Subprocess have a process handle to their parent processes
     * in {@link Process.parent}, and parent processes get handles to their
     * subprocesses returned from {@link Process.spawn}.
     * 
     * Child processes receive only the {@link ProcessEventHandlersMap.message message} event.
     * 
     * Parent processes receive {@link ProcessEventHandlersMap.message message}, {@link ProcessEventHandlersMap.exit exit},
     * {@link ProcessEventHandlersMap.exception exception} and {@link ProcessEventHandlersMap.log log} events from their child
     * processes.
     * 
     * @param type 
     * @param listener 
     */
    addEventListener<K extends keyof ProcessEventHandlersMap>(type: K, listener: (event: ProcessEventHandlersMap[K]) => void): void;
    addEventListener(type: string, listener: () => void): void;

    /**
     * Closes the handle to a child process and terminates it.
     * 
     * **Note**: child processes *can't* close their parents via
     * `Process.parent.close()`.
     */
    close(): void;

    /**
     * Sends a message to the process represented by this handle.
     * 
     * The message can be a `JSON` object or a `TypedArray`, and is received as the
     * single argument to the {@link ProcessEventHandlersMap.message message} event listener in the other
     * process.
     * 
     * @param value  The message to send.
     */
    postMessage(value: Json | TypedArray): void;

    /**
     * Removes an event listener that has previously been registered via
     * {@link Process.addEventListener}.
     * 
     * @param type 
     * @param listener
     */
    removeEventListener(type: string, listener: Function): void;
}

declare var Process: {

    /**
     * The list of arguments passed to the process, via the command line or
     * {@link Process.spawn}. Does not include the executable name nor
     * arguments processed by Window.js internally.
     */
    readonly args: string[];

    /**
     * The number of logical CPUs in the current machine. This is usually the number
     * of CPU cores, or twice that for cores with HyperThreading.
     *
     * The CPU count gives an estimate of the maximum level of parallelism that can
     * be obtained in the current machine when using multiple processes.
     */
    readonly cpus: number;

    /**
     * A handle to the parent process. This is only present in child processes, as the
     * main process doesn't have a `parent`.
     * 
     * `Process.parent` can be used to call
     * {@link Process.addEventListener} to receive
     * {@link ProcessEventHandlersMap.message messages} from the parent process, or to call
     * {@link Process.postMessage} to send messages.
     */
    readonly parent?: Process;

    /**
     * Terminates the current process immediately, with the given status code value.
     * 
     * If the current process is a child process then its parent process will receive
     * an ["exit"](#event-exit) event in its handle for this process.
     * 
     * `process.exit()` is an abrupt termination that is appropriate for unrecoverable
     * errors. Normal process termination should call
     * {@link Window.close} instead.
     * 
     * @param code  The exit code for the process.
     */
    exit(code?: number): void;

    /**
     * `spawn` creates a new subprocess and returns a handle to the parent.
     * 
     * @param module  The initial Javascript module to load in the child process.
     * @param args  Optional list of string arguments to pass to the child process. They will be available in {@link Process.args} in the child process.
     * @param options  Currently unused.
     */
    spawn(module: string, args?: string[], options?: Object): Process;
};

interface ExceptionEvent {
    /** A string describing the exception. */
    readonly message: string;
    /** The stack trace where the exception occurred. */
    readonly stacktrace: string[];
}

interface ExitEvent {
    /** A string describing the termination cause, in case of errors. */
    readonly error?: string;
    /** The exit code of the child process. */
    readonly status: number;
}

interface LogEvent {
    readonly message: string;
    /** The log level corresponding to the `console` method used */
    readonly level: "debug" | "log" | "info" | "warn" | "error";
}

interface ProcessEventHandlersMap {
    /** Sent to parent processes when a child process throws an uncaught exception. */
    "exception": ExceptionEvent;

    /** Sent to parent processes when a child process terminates. */
    "exit": ExitEvent;

    /** Sent to parent processes when a child process logs to the `console`. */
    "log": LogEvent;

    /**
     * Sent to a process handle when the corresponding process posts a message to the
     * current process via {@link Process.postMessage}.
     */
    "message": Json | TypedArray;
}
