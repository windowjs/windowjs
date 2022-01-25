---
layout: documentation
title: Window.js | ImageData
constructors:
  - ImageData
class-name: ImageData
class-methods:
  - decode
object-name: imageData
object-properties:
  - data
  - height
  - width
object-methods:
  - encode
---

ImageData
=========

An `ImageData` represents raw image pixels as a two-dimensional array in RAM.

An `ImageData` can be created in several ways:

*  By creating a `new ImageData` with a given size and blank pixels, another
   `ImageData`, or with a given array of pixel data.
*  By creating a `new ImageData` by calling
   [canvas.createPixelData](/doc/canvas#canvas.createPixelData).
*  By copying the pixels in the [canvas](/doc/canvas) via
   [canvas.getImageData](/doc/canvas#canvas.getImageData)
*  By decoding an image encoded as a `JPEG`, `PNG` or `WEBP` image in an array
   of bytes via [ImageData.decode](#ImageData.decode).
*  By decoding an image encoded as a `JPEG`, `PNG` or `WEBP` image in a file
   via [File.readImageData](/doc/file#File.readImageData).

An `ImageData` allows reading and writing the underlying pixels. See
[ImageBitmap](/doc/imagebitmap) for a similar class that is more efficient for
storing image content that is only going to be drawn.

See also the
[ImageData](https://developer.mozilla.org/en-US/docs/Web/API/ImageData)
documentation on MDN.


{% include constructor class="ImageData" %}

The `ImageData` constructor has four variants.

The first creates a blank `ImageData`:

{: .parameters}
| width  | number | The width of the `ImageData`.                              |
| height | number | The height of the `ImageData`.                             |

The second creates an `ImageData` with the same size as another `ImageData`.
Note that the `source` pixel data is *not* copied.

{: .parameters}
| imageData | ImageData | The `ImageData` whose *dimensions* are copied. The pixels of the new `ImageData` object are all blank. |

The third variant creates an `ImageData` with a copy of a subregion of another
`ImageData`.

{: .parameters}
| imageData | ImageData | The `ImageData` to copy pixels from.                 |
| x         | number    | The horizontal coordinate to start copying from.     |
| y         | number    | The vertical coordinate to start copying from.       |
| width     | number    | The number of pixels to copy per row.                |
| height    | number    | The number of rows to copy.                          |

The fourth variant takes an array of pixel data to copy:

{: .parameters}
| pixels | Uint8Array \| Uint8ClampedArray \| Uint16Array \| Float32Array | The pixel data to copy. Each pixel must have 4 entries in the array, representing the Red, Green, Blue and Alpha channels. The array size *must* be a multiple of `4 * width`. |
| width  | number  | The width of the 2D pixel array.                           |
| height | number? | Optionally, the height of the 2D pixel array. Defaults to `array.byteLength / (4 * width)` if not set. |


{% include method class="ImageData" name="decode"
   type="(Uint8Array | Uint8ClampedArray | ArrayBuffer) => Promise<ImageData>"
%}

Returns a new `ImageData`, decoded from the given image bytes.

The valid input formats are `JPEG`, `PNG` and `WEBP`.


{% include property object="imageData" name="data" type="Uint8ClampedArray" %}

The raw pixels stored in this `ImageData`.

Each pixel consist of 4 values, stored contiguously in the array:

{: .strings}
| `R` | The value of the red channel for this pixel.                           |
| `G` | The value of the blue channel for this pixel.                          |
| `B` | The value of the green channel for this pixel.                         |
| `A` | The value of the alpha channel for this pixel.                         |

Each of these values is in the `[0, 255]` range. The alpha channel represents
the transparency of the pixel: 0 is fully transparent, and 255 is fully opaque.

Pixels are stored in left-to-right, top-to-bottom order.


{% include property object="imageData" name="height" type="number" %}

The height of this `ImageData`.


{% include property object="imageData" name="width" type="number" %}

The width of this `ImageData`.


{% include method object="imageData" name="encode"
   type="(string?, number?) => Promise<ArrayBuffer>"
%}

Encodes this `ImageData` in a given image format and returns the bytes
representing that encoding.

{: .parameters}
| codec   | string? | The image codec to use for the encoding. Valid values are `"jpeg"`, `"png"` and `"webp"`. |
| quality | number? | For the `"jpeg"` codec, the `quality` parameter is a number from 0 to 100 indicating the quality of the output image. 0 is smaller but lower quality, 100 is the highest quality but also a larger encoding. |"
