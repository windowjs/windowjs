interface Codec {
    /**
     * Returns an `ArrayBuffer` with the binary content decoded from the given
     * base64 string.
     */
    base64ToArrayBuffer(encoded: string): ArrayBuffer;

    /**
     * Returns a string with the base64 encoding of the given string,
     * ArrayBuffer, Uint8Array or Uint8ClampedArray.
     */
    toBase64(data: string | ArrayBuffer | TypedArray): string;
}

declare var Codec: Codec;
