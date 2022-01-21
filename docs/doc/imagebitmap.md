---
layout: documentation
title: Window.js | ImageBitmap
constructors:
  - ImageBitmap
class-name: ImageBitmap
class-methods:
  - decode
object-name: imageBitmap
object-properties:
  - height
  - width
object-methods:
  - encode
---

ImageBitmap
===========

An `ImageBitmap` represents an image texture in GPU memory, that can be drawn
efficiently to the [canvas](/doc/canvas) via
[canvas.drawImage](/doc/canvas#canvas.drawImage).

An `ImageBitmap` can be created in several ways:

*  By copying an existing `ImageBitmap` or from the pixels in an
   [ImageData](/doc/imagedata) object via the
   [ImageBitmap constructor](#ImageBitmap).
*  By decoding an image encoded as a `JPEG`, `PNG` or `WEBP` image in an array
   of bytes via [ImageBitmap.decode](#ImageBitmap.decode).
*  By decoding an image encoded as a `JPEG`, `PNG` or `WEBP` image in a file
   via [File.readImageBitmap](/doc/file#File.readImageBitmap).

An `ImageBitmap` is appropriate to hold images loaded from files for fast
drawing. See [ImageData](/doc/imagedata) for a similar class that gives access
to the underlying pixels.

See also the
[ImageBitmap](https://developer.mozilla.org/en-US/docs/Web/API/ImageBitmap)
documentation on MDN.


{% include constructor class="ImageBitmap" %}

Creates a new `ImageBitmap` with a copy of the pixels in another `ImageBitmap`
or in an [ImageData](/doc/imagedata). The constructor has 4 additional, optional
parameters:

{: .parameters}
| source | ImageBitmap\| ImageData | The source image pixels to copy.           |
| x      | number                   | The horizontal coordinate of the source rectangle to copy. Defaults to 0. |
| y      | number                   | The vertical coordinate of the source rectangle to copy. Defaults to 0. |
| w      | number                   | The width of the source rectangle to copy. Defaults to the width of the `source`. |
| h      | number                   | The height of the source rectangle to copy. Defaults to the height of the `source`. |


{% include method class="ImageBitmap" name="decode"
   type="(Uint8Array | Uint8ClampedArray | ArrayBuffer) => Promise<ImageBitmap>"
%}

Returns a new `ImageBitmap`, decoded from the given image bytes.

The valid input formats are `JPEG`, `PNG` and `WEBP`.


{% include property object="imageBitmap" name="height" type="number" %}

The height of this `ImageBitmap`.


{% include property object="imageBitmap" name="width" type="number" %}

The width of this `ImageBitmap`.


{% include method object="imageBitmap" name="encode"
   type="(string?, number?) => Promise<ArrayBuffer>"
%}

Encodes this `ImageBitmap` in a given image format and returns the bytes
representing that encoding.

{: .parameters}
| codec   | string? | The image codec to use for the encoding. Valid values are `"jpeg"`, `"png"` and `"webp"`. |
| quality | number? | For the `"jpeg"` codec, the `quality` parameter is a number from 0 to 100 indicating the quality of the output image. 0 is smaller but lower quality, 100 is the highest quality but also a larger encoding. |
