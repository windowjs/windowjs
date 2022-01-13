type TypedArray = Int8Array | Uint8Array | Uint8ClampedArray | Int16Array | Uint16Array | Int32Array | Uint32Array | Float32Array | Float64Array | BigInt64Array | BigUint64Array;

declare var File: {
    /**
     * The current working directory.
     */
    readonly cwd: string;

    /**
     * The user home directory.
     */
    readonly home: string;

    /**
     * Returns the name of the file at the end of the given string path.
     */
    basename(path: string): string;

    /**
     * Copies a file from the path in the first parameter to the path in the second
     * parameter. See [copyTree](#File.copyTree) for a similar method that can copy
     * entire directories, recursively.
     */
    copy(src: string, dst: string): Promise<void>;

    /**
     * Copies the file or directory in the first parameter to the path in the second
     * parameter. This method will copy directories and their contents recursively.
     */
    copyTree(src: string, dst: string): Promise<void>;

    /**
     * Returns the path to the directory that contains the file path given in the
     * first parameter.
     */
    dirname(path: string): string;

    /**
     * Returns `true` if the given path is a directory. Returns false if the path
     * doesn't exist or isn't accessible to the current process.
     */
    isDir(path: string): Promise<boolean>;

    /**
     * Returns `true` if the given path is a file. Returns false if the path
     * doesn't exist or isn't accessible to the current process.
     */
    isFile(path: string): Promise<boolean>;

    /**
     * Returns a list of all the files in the given directory. See
     * [listTree](#File.listTree) for a similar method that also returns the contents
     * of subdirectories.
     */
    list(path: string): Promise<string[]>;

    /**
     * Returns a list of all the files in the given directory, recursively.
     */
    listTree(path: string): Promise<string[]>;

    /**
     * Creates all the missing directories in the given path.
     */
    mkdirs(path: string): Promise<void>;

    /**
     * Returns the contents of the given file as an `ArrayBuffer`.
     */
    readArrayBuffer(path: string): Promise<ArrayBuffer>;

    /**
     * Returns the contents of the given file as an [ImageBitmap](/doc/imagebitmap).
     */
    readImageBitmap(path: string): Promise<ImageBitmap>;

    /**
     * Returns the contents of the given file as an [ImageData](/doc/imagedata).
     */
    readImageData(path: string): Promise<ImageData>;

    /**
     * Returns the contents of the given file as a `JSON` object.
     */
    readJSON(path: string): Promise<Json>;

    /**
     * Returns the contents of the given file as a string.
     */
    readText(path: string): Promise<string>;

    /**
     * Removes the given file. See [removeTree](#File.removeTree) for a similar method
     * that can remove a directory and all of its contents, recursively.
     */
    remove(path: string): Promise<void>;

    /**
     * Removes the given file or directory, and all of its contents, recursively.
     */
    removeTree(path: string): Promise<void>;

    /**
     * Renames the file at the path in the first parameter to the path in the
     * second parameter.
     */
    rename(from: string, to: string): Promise<void>;

    /**
     * Returns the size, in bytes, of the file at the given path.
     */
    size(path: string): Promise<number>;

    /**
     * Writes the contents of the object in the second parameter to the file path in
     * the first parameter.
     */
    write(path: string, data: string | ArrayBuffer | TypedArray): Promise<void>;
};