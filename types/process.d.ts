
interface Process {

    /**
     * `addEventListener` registers a listener callback to receive events in a given
     * process handle. Subprocess have a process handle to their parent processes
     * in [Process.parent](#Process.parent), and parent processes get handles to their
     * subprocesses returned from [Process.spawn](#Process.spawn).
     * 
     * Child processes receive only the [message](#event-message) event.
     * 
     * Parent processes receive [message](#event-message), [exit](#event-exit),
     * [exception](#event-exception) and [log](#event-log) events from their child
     * processes.
     * 
     * @param event 
     * @param callback 
     */
    addEventListener(event: string, callback: (...args: any[]) => void): void;

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
     * single argument to the [message](#event-message) event listener in the other
     * process.
     * 
     * @param value  The message to send.
     */
    postMessage(value: any): void;

    /**
     * Removes an event listener that has previously been registered via
     * [process.addEventListener](#Process.addEventListener).
     * 
     * @param event 
     * @param callback 
     */
    removeEventListener(event: string, callback: (...args: any[]) => void): void;
}

declare var Process: {

    /**
     * A handle to the parent process. This is only present in child processes, as the
     * main process doesn't have a `parent`.
     * 
     * `Process.parent` can be used to call
     * [addEventListener](#process.addEventListener) to receive
     * [messages](#event-message) from the parent process, or to call
     * [postMessage](#process.postMessage) to send messages.
     */
     readonly parent?: Process;

    /**
     * Terminates the current process immediately, with the given status code value.
     * 
     * If the current process is a child process then its parent process will receive
     * an ["exit"](#event-exit) event in its handle for this process.
     * 
     * `process.exit()` is an abrupt termination that is appropriate for unrecoverable\
     * errors. Normal process termination should call
     * [window.close](/doc/window#window.close) instead.
     * 
     * @param code  The exit code for the process.
     */
    exit(code?: number): void;
    
    /**
     * `spawn` creates a new subprocess and returns a handle to the parent.
     * 
     * @param module  The initial Javascript module to load in the child process.
     * @param args  Optional list of string arguments to pass to the child process. They will be available in [Process.args](#Process.args) in the child process.
     * @param options  Currently unused.
     */
     spawn(module: string, args?: string[], options?: Object): Process;
};