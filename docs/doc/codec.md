---
layout: documentation
title: Window.js | Codec
class-name: Codec
class-methods:
  - base64ToArrayBuffer
  - toBase64
---

Codec
=====

The `Codec` API converts strings and ArrayBuffers to binary formats and back.


{% include method class="Codec" name="base64ToArrayBuffer"
   type="(string) => ArrayBuffer" %}

Returns an `ArrayBuffer` with the binary content decoded from the given base64
string.


{% include method class="Codec" name="toBase64"
   type="(string | ArrayBuffer | TypedArray) => string" %}

Returns a string with the base64 encoding of the given string, ArrayBuffer,
Uint8Array or Uint8ClampedArray.
