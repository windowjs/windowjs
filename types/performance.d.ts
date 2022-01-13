interface Performance {

    readonly memory: {
        /** The maximum size of the heap, in bytes, that is available to the Javascript VM. */
        readonly jsHeapSizeLimit: number;

        /** The total heap size, in bytes, allocated by the Javascript VM. */
        readonly totalJSHeapSize: number;

        /** The currently active segment of the Javascript VM heap, in bytes. */
        readonly usedJSHeapSize: number;
    };

    /**
     * Returns the number of milliseconds since the current process started.
     * 
     * The resolution of the timer is platform dependent. It is usually on the order of
     * a few micro- or nanoseconds. It uses the highest-resolution monotonic time
     * source on each supported platform.
     */
    now(): number;
}
