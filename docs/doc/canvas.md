---
layout: documentation
title: Window.js | Canvas
constructors:
  - CanvasRenderingContext2D
object-name: canvas
object-properties:
  - antialias
  - fillStyle
  - font
  - globalAlpha
  - globalCompositeOperation
  - height
  - imageSmoothingEnabled
  - imageSmoothingQuality
  - lineCap
  - lineDashOffset
  - lineJoin
  - lineWidth
  - miterLimit
  - shadowBlur
  - shadowColor
  - shadowOffsetX
  - shadowOffsetY
  - strokeStyle
  - textAlign
  - textBaseline
  - width
object-methods:
  - arc
  - arcTo
  - beginPath
  - bezierCurveTo
  - clip
  - closePath
  - clearRect
  - createImageData
  - createLinearGradient
  - createRadialGradient
  - drawImage
  - ellipse
  - encode
  - fill
  - fillRect
  - fillText
  - getImageData
  - getLineDash
  - getTransform
  - isPointInPath
  - isPointInStroke
  - lineTo
  - measureText
  - moveTo
  - putImageData
  - quadraticCurveTo
  - rect
  - resetTransform
  - restore
  - rotate
  - save
  - scale
  - setLineDash
  - setTransform
  - stroke
  - strokeRect
  - strokeText
  - transform
  - translate
---

Canvas
======

The [window.canvas](/doc/window#window.canvas) object is the 2D canvas that
draws to the main window.

It implements the
[Canvas2DRenderingContext](https://developer.mozilla.org/en-US/docs/Web/API/CanvasRenderingContext2D)
API from HTML5, and its
[documentation at MDN](https://developer.mozilla.org/en-US/docs/Web/API/CanvasRenderingContext2D)
applies to the Window.js implementation as well. The
[Canvas tutorial](https://developer.mozilla.org/en-US/docs/Web/API/Canvas_API/Tutorial)
gives an overview of the capabilities of the API.

The [window.canvas](/doc/window#window.canvas) object is always present in
Window.js, and renders directly to the main window. It's also possible to create
additional [CanvasRenderingContext2D](#CanvasRenderingContext2D) instances;
each instance has its own framebuffer and can be used to draw off the main
screen. The contents of an off-screen canvas can be drawn to the main canvas
via [drawImage](#canvas.drawImage).


{% include constructor class="CanvasRenderingContext2D" %}

Creates a new off-screen canvas. Its contents can be drawn to the main canvas
in [window.canvas](/doc/window#window.canvas) via
[drawImage](#canvas.drawImage).

{: .parameters}
| width  | number | The width of the offscreen canvas.                         |
| height | number | The height of the offscreen canvas.                        |


{% include property object="canvas" name="antialias" type="boolean" %}

{% include tag extension=true %}

Whether shapes drawn in this canvas should be antialiased. Defaults to true.


{% include property object="canvas" name="fillStyle"
   type="string | CanvasGradient"
%}

The style to apply in the fill operations ([fill](#canvas.fill),
[fillRect](#canvas.fillRect) and [fillText](#canvas.fillText)).

Valid values are [CanvasGradient](/doc/canvasgradient) instances, or strings
representing a CSS color in one of these formats:

*  `#rgb`
*  `#rgba`
*  `#rrggbb`
*  `#rrggbbaa`
*  `rgb(r, g, b)`
*  `rgba(r, g, b, a)`
*  A
[CSS color name](https://developer.mozilla.org/en-US/docs/Web/CSS/color_value#color_keywords)
like `red` or `dodgerblue`.

See also
[fillStyle](https://developer.mozilla.org/en-US/docs/Web/API/CanvasRenderingContext2D/fillStyle)
at MDN.


{% include property object="canvas" name="font" type="string" %}

The font to use in text operations ([fillText](#canvas.fillText),
[strokeText](#canvas.strokeText) and [measureText](#canvas.measureText)).

The list of fonts available in the current platform are listed in
[window.fonts](/doc/window#window.fonts).

Custom fonts can be loaded with [window.loadFont](/doc/window#window.loadFont).

Font names with spaces like "Segoe UI" must be wrapped in double quotes.

The font name can be optionally preceeded by `italic`, `bold` and the font
size as `Npx`. These are all valid font settings:

*  `monospace`
*  `bold 16px "Segoe UI"`

See also
[font](https://developer.mozilla.org/en-US/docs/Web/API/CanvasRenderingContext2D/font)
at MDN.


{% include property object="canvas" name="globalAlpha" type="number" %}

Specifies the alpha (transparency) value that is applied to shapes and images
before they are drawn onto the canvas. The defaut value is `1.0`.

See also
[globalAlpha](https://developer.mozilla.org/en-US/docs/Web/API/CanvasRenderingContext2D/globalAlpha)
at MDN.


{% include property object="canvas" name="globalCompositeOperation"
   type="string"
%}

Sets the type of compositing operation to apply when drawing new shapes.
See the
[Compositing and clipping](https://developer.mozilla.org/en-US/docs/Web/API/Canvas_API/Tutorial/Compositing)
section of the
[Canvas tutorial](https://developer.mozilla.org/en-US/docs/Web/API/Canvas_API/Tutorial)
at MDN for a list of valid operations and their effect.

The following values are supported in Window.js:

*  `source-over`
*  `source-in`
*  `source-out`
*  `source-atop`
*  `destination-over`
*  `destination-in`
*  `destination-out`
*  `destination-atop`
*  `lighter`
*  `copy`
*  `xor`
*  `multiply`
*  `screen`
*  `overlay`
*  `darken`
*  `lighten`
*  `color-dodge`
*  `color-burn`
*  `hard-light`
*  `soft-light`
*  `difference`
*  `exclusion`
*  `hue`
*  `saturation`
*  `color`
*  `luminosity`

See also
[globalCompositeOperation](https://developer.mozilla.org/en-US/docs/Web/API/CanvasRenderingContext2D/globalCompositeOperation)
at MDN.


{% include property object="canvas" name="height" type="number" %}

{% include tag extension=true %}

The height of the canvas, in pixels.

**Note:** the size of the canvas is automatically adjusted when the window is
[resized](/doc/window#event-resize). If the main application supports
[resizing](/doc/window#window.resizable) then it should also listen for
["resize"](/doc/window#event-resize) events and draw a full frame after each
resize. Window.js will also invoke any pending
[requestAnimationFrame](/doc/global#requestAnimationFrame) callbacks when
the window is resized.


{% include property object="canvas" name="imageSmoothingEnabled"
   type="boolean"
%}

Determines whether scaled images are smoothed or not. The default is true.

Images can be drawn via the [drawImage](#canvas.drawImage) API, and the
destination size in the canvas may be different from the source size in the
[ImageBitmap](/doc/imagebitmap). In those cases, the image gets scaled up or
down. This setting controls whether the rescaled image should look smoother
(when it's turned on), or sharper (when it's turned off).

Disabling `imageSmoothingEnabled` draws images with a more blocky, pixelated
style.

See also
[imageSmoothingEnabled](https://developer.mozilla.org/en-US/docs/Web/API/CanvasRenderingContext2D/imageSmoothingEnabled)
at MDN.


{% include property object="canvas" name="imageSmoothingQuality"
   type="string"
%}

Controls the quality of the smoothing applied to scaled images when
[imageSmoothingEnabled](#canvas.imageSmoothingEnabled) is enabled.

Valid values are `"low"`, `"medium"` and `"high"`.

See also
[imageSmoothingQuality](https://developer.mozilla.org/en-US/docs/Web/API/CanvasRenderingContext2D/imageSmoothingQuality)
at MDN.


{% include property object="canvas" name="lineCap" type="string" %}

The shape used to draw the end points of lines.

{: .strings}
| `"butt"`   | (Default value) The ends of lines are squared off at the endpoints. |
| `"round"`  | The ends of lines are rounded.                                  |
| `"square"` | The ends of lines are squared off by adding a box with an equal width and half the height of the line's thickness. |

See also
[lineCap](https://developer.mozilla.org/en-US/docs/Web/API/CanvasRenderingContext2D/lineCap)
at MDN.


{% include property object="canvas" name="lineDashOffset" type="number" %}

The line dash offset. Defaults to 0.

See also
[lineDashOffset](https://developer.mozilla.org/en-US/docs/Web/API/CanvasRenderingContext2D/lineDashOffset)
at MDN.


{% include property object="canvas" name="lineJoin" type="string" %}

The shape used to join two line segments where they meet.

{: .strings}
| `"bevel"` | Fills an additional triangular area between the common endpoint of connected segments, and the separate outside rectangular corners of each segment. |
| `"round"` | Rounds off the corners of a shape by filling an additional sector of disc centered at the common endpoint of connected segments. The radius for these rounded corners is equal to the line width. |
| `"miter"` | (Default value) Connected segments are joined by extending their outside edges to connect at a single point, with the effect of filling an additional lozenge-shaped area. This setting is affected by the [miterLimit](#canvas.miterLimit) property. |

See also
[lineJoin](https://developer.mozilla.org/en-US/docs/Web/API/CanvasRenderingContext2D/lineJoin)
at MDN.


{% include property object="canvas" name="lineWidth" type="number" %}

The thickness of lines. The default value is 1.0.

See also
[lineWidth](https://developer.mozilla.org/en-US/docs/Web/API/CanvasRenderingContext2D/lineWidth)
at MDN.


{% include property object="canvas" name="miterLimit" type="number" %}

The miter limit ratio. The default value is 10.0.

See also
[miterLimit](https://developer.mozilla.org/en-US/docs/Web/API/CanvasRenderingContext2D/miterLimit)
at MDN.


{% include property object="canvas" name="shadowBlur" type="number" %}

The amount of blur applied to shadows. The default is 0 (i.e. no blur).

See also
[shadowBlur](https://developer.mozilla.org/en-US/docs/Web/API/CanvasRenderingContext2D/shadowBlur)
at MDN.


{% include property object="canvas" name="shadowColor" type="string" %}

The color of shadows, as a CSS color.

The shadow's rendered opacity is affected by the opacity of the
[fillStyle](#canvas.fillStyle) color when filling, and of the
[strokeStyle](#canvas.strokeStyle) color when stroking.

**Note:** Shadows are only drawn if the `shadowColor` property is set to a
non-transparent value. One of the [shadowBlur](#canvas.shadowBlur),
[shadowOffsetX](#canvas.shadowOffsetX) or [shadowOffsetY](#canvas.shadowOffsetY)
properties must be non-zero as well.

The CSS color must be in one of these formats:

*  `#rgb`
*  `#rgba`
*  `#rrggbb`
*  `#rrggbbaa`
*  `rgb(r, g, b)`
*  `rgba(r, g, b, a)`
*  A
[CSS color name](https://developer.mozilla.org/en-US/docs/Web/CSS/color_value#color_keywords)
like `red` or `dodgerblue`.

See also
[shadowColor](https://developer.mozilla.org/en-US/docs/Web/API/CanvasRenderingContext2D/shadowColor)
at MDN.


{% include property object="canvas" name="shadowOffsetX" type="number" %}

The distance that shadows will be offset horizontally.

See also
[shadowOffsetX](https://developer.mozilla.org/en-US/docs/Web/API/CanvasRenderingContext2D/shadowOffsetX)
at MDN.


{% include property object="canvas" name="shadowOffsetY" type="number" %}

The distance that shadows will be offset vertically.

See also
[shadowOffsetY](https://developer.mozilla.org/en-US/docs/Web/API/CanvasRenderingContext2D/shadowOffsetY)
at MDN.


{% include property object="canvas" name="strokeStyle" type="string" %}

The style to apply in the stroke operations ([stroke](#canvas.stroke),
[strokeRect](#canvas.strokeRect) and [strokeText](#canvas.strokeText)).

Valid values are [CanvasGradient](/doc/canvasgradient) instances, or strings
representing a CSS color in one of these formats:

*  `#rgb`
*  `#rgba`
*  `#rrggbb`
*  `#rrggbbaa`
*  `rgb(r, g, b)`
*  `rgba(r, g, b, a)`
*  A
[CSS color name](https://developer.mozilla.org/en-US/docs/Web/CSS/color_value#color_keywords)
like `red` or `dodgerblue`.

See also
[strokeStyle](https://developer.mozilla.org/en-US/docs/Web/API/CanvasRenderingContext2D/strokeStyle)
at MDN.


{% include property object="canvas" name="textAlign" type="string" %}

The text alignment used when drawing text. The default is `"left"`.

* `"left"`: aligns text to the left.
* `"right"`: aligns text to the right.
* `"center"`: text is centered.
* `"start"`: aligns text to the left.
* `"end"`: aligns text to the right.

See also
[textAlign](https://developer.mozilla.org/en-US/docs/Web/API/CanvasRenderingContext2D/textAlign)
at MDN.


{% include property object="canvas" name="textBaseline" type="string" %}

The text baseline used when drawing text. The default is `"alphabetic"`.

* `"top"`: The text baseline is the top of the em square.
* `"hanging"`: The text baseline is the hanging baseline.
* `"middle"`: The text baseline is the middle of the em square.
* `"alphabetic"`: The text baseline is the alphabetic baseline.
* `"ideographic"`: The text baseline is the ideographic baseline. This is the bottom of the body of the characters, if the main body of characters protrudes beneath the alphabetic baseline. (Used by Chinese, Japanese, and Korean scripts).
* `"bottom"`: The text baseline is the bottom of the bounding box. This differs from the ideographic baseline in that the ideographic baseline doesn't consider descenders.

See also
[textBaseline](https://developer.mozilla.org/en-US/docs/Web/API/CanvasRenderingContext2D/textBaseline)
at MDN.


{% include property object="canvas" name="width" type="number" %}

{% include tag extension=true %}

The width of the canvas, in pixels.

**Note:** the size of the canvas is automatically adjusted when the window is
[resized](/doc/window#event-resize). If the main application supports
[resizing](/doc/window#window.resizable) then it should also listen for
["resize"](/doc/window#event-resize) events and draw a full frame after each
resize. Window.js will also invoke any pending
[requestAnimationFrame](/doc/global#requestAnimationFrame) callbacks when
the window is resized.


{% include method object="canvas" name="arc"
   type="(number, number, number, number, number) => void"
%}

Adds a circular arc to the current sub-path, centered at `(x, y)` and with the
given `radius`. The path starts at `startAngle` and ends at `endAngle`,
traveling in the clockwise direction.

{: .parameters}
| x          | number | The horizontal coordinate of the arc's center.         |
| y          | number | The vertical coordinate of the arc's center.           |
| radius     | number | The arc's radius.                                      |
| startAngle | number | The arc's starting angle, in radians, measured from the x axis. |
| endAngle   | number | The arc's ending angle, in radians, measured from the x axis. |

See also
[arc](https://developer.mozilla.org/en-US/docs/Web/API/CanvasRenderingContext2D/arc)
at MDN.


{% include method object="canvas" name="arcTo"
   type="(number, number, number, number, number) => void"
%}

Adds a circular arc to the current sub-path, using the given control points and
radius. The arc is connected to the path's latest point with a straight line,
if necessary.

{: .parameters}
| x1     | number | The horizontal coordinate of the first control point.      |
| y1     | number | The vertical coordinate of the first control point.        |
| x2     | number | The horizontal coordinate of the second control point.     |
| y2     | number | The vertical coordinate of the second control point.       |
| radius | number | The arc's radius.                                          |

See also
[arcTo](https://developer.mozilla.org/en-US/docs/Web/API/CanvasRenderingContext2D/arcTo)
at MDN.


{% include method object="canvas" name="beginPath" type="() => void" %}

Starts a new path by emptying the list of sub-paths.

See also
[beginPath](https://developer.mozilla.org/en-US/docs/Web/API/CanvasRenderingContext2D/beginPath)
at MDN.


{% include method object="canvas" name="bezierCurveTo"
   type="(number, number, number, number, number, number) => void"
%}

Adds a cubic BÃ©zier curve to the current sub-path. Requires three points: two
control points, and a final end point. The starting point is the latest point
in the current path (which can be changed using [moveTo](#canvas.moveTo) before
creating the curve).

{: .parameters}
| x1 | number | The horizontal coordinate of the first control point.          |
| y1 | number | The vertical coordinate of the first control point.            |
| x2 | number | The horizontal coordinate of the second control point.         |
| y2 | number | The vertical coordinate of the second control point.           |
| x  | number | The horizontal coordinate of the end point.                    |
| y  | number | The vertical coordinate of the end point.                      |

See also
[bezierCurveTo](https://developer.mozilla.org/en-US/docs/Web/API/CanvasRenderingContext2D/bezierCurveTo)
at MDN.


{% include method object="canvas" name="clip" type="(string?) => void" %}

Turns the current path into the current clipping region. The previous clipping
region, if any, is intersected with the current path to create the new clipping
region.

{: .parameters}
| fillRule | string? | The algorithm to determine if a point is inside or outside of the clipping region. Can be the `"nonzero"` winding rule (default), or the `"evenodd"` winding rule. |

See also
[clip](https://developer.mozilla.org/en-US/docs/Web/API/CanvasRenderingContext2D/clip)
at MDN.


{% include method object="canvas" name="closePath" type="() => void" %}

Adds a straight line from the current point to the start of the current
sub-path. Does nothing if the shape has already been closed, or has only one
point.

This method doesn't draw anything to the canvas directly; the path can be
rendered via the [fill](#canvas.fill) and [stroke](#canvas.stroke) methods.

See also
[closePath](https://developer.mozilla.org/en-US/docs/Web/API/CanvasRenderingContext2D/closePath)
at MDN.


{% include method object="canvas" name="clearRect"
   type="(number, number, number, number) => void"
%}

Erases the pixels in a rectangular area by setting them to transparent black.

{: .parameters}
| x      | number | The horizontal coordinate of the starting corner.          |
| y      | number | The vertical coordinate of the starting corner.            |
| width  | number | The width of the rectangle. Positive values are to the right, and negative to the left.                                                        |
| height | number | The height of the rectangle. Positive values are down, and negative are up.                                                                    |

The main window supports transparency but is initially set to opaque black.
It must be cleared and filled with a transparent overlay to enable transparency:

```javascript
const canvas = window.canvas;
canvas.clearRect(0, 0, canvas.width, canvas.height);
canvas.fillStyle = '#000000b0';
canvas.fillRect(0, 0, canvas.width, canvas.height);
```

See
[`examples/squares.js`](https://github.com/windowjs/windowjs/blob/main/examples/squares.js)
for an example.


See also
[clearRect](https://developer.mozilla.org/en-US/docs/Web/API/CanvasRenderingContext2D/clearRect)
at MDN.


{% include method object="canvas" name="createImageData"
   type="(ImageData | number, number) => ImageData"
%}

Creates a new, blank [ImageData](/doc/imagedata) object with the specified
dimensions. All of the pixels in the new object are transparent black.

This method can be called in two different ways. Parameters for the first
variant:

{: .parameters}
| imageData | ImageData | An existing `ImageData` object from which to copy the width and height. The image itself is **not** copied.                              |

The second variant:

{: .parameters}
| width  | number | The width for the new `ImageData` object.                  |
| height | number | The height for the new `ImageData` object.                 |

See also
[createImageData](https://developer.mozilla.org/en-US/docs/Web/API/CanvasRenderingContext2D/createImageData)
at MDN.


{% include method object="canvas" name="createLinearGradient"
   type="(number, number, number, number) => CanvasGradient"
%}

Creates a linear [CanvasGradient](/doc/canvasgradient) along the line connecting
the two given coordinates.

To be applied to a shape, the gradient must first be assigned to the
[fillStyle](#canvas.fillStyle) or [strokeStyle](#canvas.strokeStyle) properties.

{: .parameters}
| x1 | number | The horizontal coordinate of the start point.                  |
| y1 | number | The vertical coordinate of the first point.                    |
| x2 | number | The horizontal coordinate of the end point.                    |
| y2 | number | The vertical coordinate of the end point.                      |

See also
[createLinearGradient](https://developer.mozilla.org/en-US/docs/Web/API/CanvasRenderingContext2D/createLinearGradient)
at MDN.


{% include method object="canvas" name="createRadialGradient"
   type="(number, number, number, number, number, number) => CanvasGradient"
%}

Creates a radial [CanvasGradient](/doc/canvasgradient) using the size and
coordinates of two circles.

To be applied to a shape, the gradient must first be assigned to the
[fillStyle](#canvas.fillStyle) or [strokeStyle](#canvas.strokeStyle) properties.

{: .parameters}
| x1 | number | The horizontal coordinates of the start circle.                |
| y1 | number | The vertical coordinates of the start circle.                  |
| r1 | number | The radius of the start circle.                                |
| x2 | number | The horizontal coordinates of the end circle.                  |
| y2 | number | The vertical coordinates of the end circle.                    |
| r2 | number | The radius of the end circle.                                  |

See also
[createRadialGradient](https://developer.mozilla.org/en-US/docs/Web/API/CanvasRenderingContext2D/createRadialGradient)
at MDN.


{% include method object="canvas" name="drawImage"
   type="(ImageBitmap|CanvasRenderingContext2D, number, number, number? number?, number?, number? => void"
%}

Draws an [ImageBitmap](/doc/imagebitmap) or an
[offscreen canvas](#CanvasRenderingContext2D) onto the main canvas.

This method can be called in three different ways.

The first version draws the image in its original size at a given location.
Parameters for the first version:

{: .parameters}
| image | ImageBitmap \| CanvasRenderingContext2D | The source image or canvas to draw. |
| dx    | number      | The horizontal coordinate in the destination canvas at which to place the top-left corner of the source image. |
| dy    | number      | The vertical coordinate in the destination canvas at which to place the top-left corner of the source image. |

The second version draws the image with a specific size.
Parameters for the second version:

{: .parameters}
| image | ImageBitmap \| CanvasRenderingContext2D | The source image or canvas to draw. |
| dx    | number      | The horizontal coordinate in the destination canvas at which to place the top-left corner of the source image. |
| dy    | number      | The vertical coordinate in the destination canvas at which to place the top-left corner of the source image. |
| dw    | number      | The width to draw the source image in the destination canvas. |
| dh    | number      | The height to draw the source image in the destination canvas. |

The third version draws a sub region of the source image into a given region of
the destination canvas. Parameters for the third version:

{: .parameters}
| image | ImageBitmap \| CanvasRenderingContext2D | The source image or canvas to draw. |
| sx    | number      | The horizontal coordinate of the top-left corner of the sub region of the source image that will be drawn. |
| sy    | number      | The vertical coordinate of the top-left corner of the sub region of the source image that will be drawn. |
| sw    | number      | The width of the sub region of the source image that will be drawn. |
| sh    | number      | The height of the sub region of the source image that will be drawn. |
| dx    | number      | The horizontal coordinate in the destination canvas at which to place the top-left corner of the source image sub region. |
| dy    | number      | The vertical coordinate in the destination canvas at which to place the top-left corner of the source image sub region. |
| dw    | number      | The width to draw the source image in the destination canvas. |
| dh    | number      | The height to draw the source image in the destination canvas. |


See also
[drawImage](https://developer.mozilla.org/en-US/docs/Web/API/CanvasRenderingContext2D/drawImage)
at MDN.


{% include method object="canvas" name="ellipse"
   type="(number, number, number, number, number, number, number) => void"
%}

Adds an elliptical arc to the current sub-path.

{: .parameters}
| x          | number | The horizontal coordinate of the center of the ellipse.         |
| y          | number | The vertical coordinate of the center of the ellipse.         |
| rx         | number | The radius of the major axis of the ellipse. |
| ry         | number | The radius of the minor axis of the ellipse. |
| rotation   | number | The rotation of the ellipse, in radians. |
| startAngle | number | The angle at which the ellipse starts, in radians, measured clockwise from the positive X axis. |
| endAngle   | number | The angle at which the ellipse end, in radians, measured clockwise from the positive X axis. |

See also
[ellipse](https://developer.mozilla.org/en-US/docs/Web/API/CanvasRenderingContext2D/ellipse)
at MDN.


{% include method object="canvas" name="encode"
   type="() => Promise<ArrayBuffer>"
%}

{% include tag extension=true %}

Encodes the content of the canvas into a compressed image format and returns
it as bytes in an `ArrayBuffer`.

{: .parameters}
| format  | string? | The image format to encode in. Valid values: `"png"` (default), `"jpeg"` and `"webp"`. |
| quality | number? | The encoding quality for the `"jpeg"` codec. |


{% include method object="canvas" name="fill" type="(string?) => void" %}

Fills the current path with the current [fillStyle](#canvas.fillStyle).

{: .parameters}
| fillRule | string? | An optional `string` indicating the winding rule used to determine if a point is inside or outside of the filling region. Valid values are `"nonzero"` (default) and `"evenodd"`. |

See also
[fill](https://developer.mozilla.org/en-US/docs/Web/API/CanvasRenderingContext2D/fill)
at MDN.


{% include method object="canvas" name="fillRect"
   type="(number, number, number, number) => void"
%}

Draws a rectangle using the current [fillStyle](#canvas.fillStyle).

This method draws directly to the canvas without modifying the current path.

{: .parameters}
| x      | number | The horizontal coordinate of the starting point.           |
| y      | number | The vertical coordinate of the starting point.             |
| width  | number | The width of the rectangle.                                |
| height | number | The height of the rectangle.                               |

See also
[fillRect](https://developer.mozilla.org/en-US/docs/Web/API/CanvasRenderingContext2D/fillRect)
at MDN.


{% include method object="canvas" name="fillText"
   type="(string, number, number) => void"
%}

Draws a text string at the specified coordinates using the current
[fillStyle](#canvas.fillStyle), [font](#canvas.font),
[textAlign](#canvas.textAlign) and [textBaseline](#canvas.textBaseline)
properties.

{: .parameters}
| text | string | The text string to draw.                                     |
| x    | number | The horizontal coordinate at which to begin drawing.         |
| y    | number | The vertical coordinate at which to begin drawing.           |

See also
[fillText](https://developer.mozilla.org/en-US/docs/Web/API/CanvasRenderingContext2D/fillText)
at MDN.


{% include method object="canvas" name="getImageData"
   type="(number?, number?, number?, number?) => ImageData"
%}

Returns an [ImageData](/doc/imagedata) object containing the pixel data for a
specified region of the canvas. [ImageData](/doc/imagedata) can be painted
onto a canvas using the [putImageData](#canvas.putImageData) method.

This method is not affected by the canvas's transformation matrix.

{: .parameters}
| x | number? | The horizontal coordinate of the top-left corner of the region to copy. Defaults to 0. |
| y | number? | The vertical coordinate of the top-left corner of the region to copy. Defaults to 0. |
| w | number? | The width of the region to copy. Defaults to the canvas's width. |
| h | number? | The height of the region to copy. Defaults to the canvas's height. |

See also
[getImageData](https://developer.mozilla.org/en-US/docs/Web/API/CanvasRenderingContext2D/getImageData)
at MDN.


{% include method object="canvas" name="getLineDash" type="() => number[]" %}

Returns the current line dash pattern.

See also
[getLineDash](https://developer.mozilla.org/en-US/docs/Web/API/CanvasRenderingContext2D/getLineDash)
at MDN.


{% include method object="canvas" name="getTransform" type="() => number[]" %}

Returns the transformation matrix, as an array with 6 elements:

{: .matrix }
| `a` | `c` | `e` |
| `b` | `d` | `f` |
| `0` | `0` | `1` |

See also
[getTransform](https://developer.mozilla.org/en-US/docs/Web/API/CanvasRenderingContext2D/getTransform)
at MDN.


{% include method object="canvas" name="isPointInPath"
   type="(number, number, string?) => boolean"
%}

Returns whether the specified point is contained in the current path.

{: .parameters}
| x        | number | The horizontal coordinate of the point to check.         |
| y        | number | The vertical coordinate of the point to check.           |
| fillRule | string? | An optional string indicating the winding rule. Valid values are `"nonzero"` (default) and `"evenodd"`. |

See also
[isPointInPath](https://developer.mozilla.org/en-US/docs/Web/API/CanvasRenderingContext2D/isPointInPath)
at MDN.


{% include method object="canvas" name="isPointInStroke"
   type="(number, number) => boolean"
%}

Returns whether the specified point is contained in the stroking of the current
path.

{: .parameters}
| x        | number | The horizontal coordinate of the point to check.         |
| y        | number | The vertical coordinate of the point to check.           |

See also
[isPointInStroke](https://developer.mozilla.org/en-US/docs/Web/API/CanvasRenderingContext2D/isPointInStroke)
at MDN.


{% include method object="canvas" name="lineTo"
   type="(number, number) => void"
%}

Connects the last point of the current sub-path with a straight line to the
given point.

{: .parameters }
| x | number | The horizontal coordinate of the end point.                     |
| y | number | The vertical coordinate of the end point.                       |

See also
[lineTo](https://developer.mozilla.org/en-US/docs/Web/API/CanvasRenderingContext2D/lineTo)
at MDN.


{% include method object="canvas" name="measureText" type="(string) => void" %}

Returns an object with the text metrics about the given text string.

| text | string | The text string to measure.                                  |

The object returned contains this data:

{: .parameters}
| width                    | number | The calculated width of the given text string, using the current font and styling properties. |
| actualBoundingBoxAscent  | number | The distance from the text baseline to the top of the highest bounding rectangle of the given text. |
| actualBoundingBoxDescent | number | The distance from the text baseline to the bottom of the lowest bounding rectangle of the given text. |
| actualBoundingBoxLeft    | number |The distance from the text alignment to the left of the given text. |
| actualBoundingBoxRight   | number |The distance from the text alignment to the right of the given text. |

See also
[measureText](https://developer.mozilla.org/en-US/docs/Web/API/CanvasRenderingContext2D/measureText)
at MDN.


{% include method object="canvas" name="moveTo"
   type="(number, number) => void"
%}

Begins a new sub-path at the given point.

{: .parameters }
| x | number | The horizontal coordinate of the point.                         |
| y | number | The vertical coordinate of the point.                           |

See also
[moveTo](https://developer.mozilla.org/en-US/docs/Web/API/CanvasRenderingContext2D/moveTo)
at MDN.


{% include method object="canvas" name="putImageData"
   type="(ImageData, number, number, number?, number?, number?, number?) => void"
%}

Paints the data in the given [ImageData](/doc/imagedata) object onto the canvas.

The last 4 parameters are optional, and indicate the rectangle region to copy;
otherwise, the entire `ImageData` gets copied.

{: .parameters}
| imageData | ImageData | The pixel data to copy.                              |
| x         | number    | The horizontal coordinate at which to place the image data in the canvas. |
| y         | number    | The vertical coordinate at which to place the image data in the canvas. |
| sx        | number?   | Optional parameter indicating the horizontal coordinate of the top-left corner of the `imageData` to start copying from. |
| sy        | number?   | Optional parameter indicating the vertical coordinate of the top-left corner of the `imageData` to start copying from. |
| sw        | number?   | Optional parameter indicating the width of the region to copy. |
| sh        | number?   | Optional parameter indicating the height of the region to copy. |

See also
[putImageData](https://developer.mozilla.org/en-US/docs/Web/API/CanvasRenderingContext2D/putImageData)
at MDN.


{% include method object="canvas" name="quadraticCurveTo"
   type="(number, number, number, number) => void"
%}

Adds a quadratic Bézier curve to the current sub-path.

The curve requires two points. The first is a control point, and the second is
the end point. The starting point is the latest point in the current path,
which can be changed using [moveTo](#canvas.moveTo) before creating the
quadratic Bézier curve.

{: .parameters}
| cx | number | The horizontal coordinate of the control point.                |
| cy | number | The vertical coordinate of the control point.                  |
| ex | number | The horizontal coordinate of the end point.                    |
| ey | number | The vertical coordinate of the end point.                      |

See also
[quadraticCurveTo](https://developer.mozilla.org/en-US/docs/Web/API/CanvasRenderingContext2D/quadraticCurveTo)
at MDN.


{% include method object="canvas" name="rect"
   type="(number, number, number, number) => void"
%}

Adds a rectangle to the current path.

{: .parameters}
| x | number | The horizontal coordinate of the rectangle's starting point.    |
| y | number | The vertical coordinate of the rectangle's starting point.      |
| w | number | The width of the rectangle.                                     |
| h | number | The height of the rectangle.                                    |

See also
[rect](https://developer.mozilla.org/en-US/docs/Web/API/CanvasRenderingContext2D/rect)
at MDN.


{% include method object="canvas" name="resetTransform" type="() => void" %}

Resets the current transform to the identity matrix.

See also
[resetTransform](https://developer.mozilla.org/en-US/docs/Web/API/CanvasRenderingContext2D/resetTransform)
at MDN.


{% include method object="canvas" name="restore" type="() => void" %}

Restores the most recently saved canvas state, from a previous call to
[save](#canvas.save). If there is no saved state then this method does nothing.

See also
[restore](https://developer.mozilla.org/en-US/docs/Web/API/CanvasRenderingContext2D/restore)
at MDN.


{% include method object="canvas" name="rotate" type="(number) => void" %}

Adds a rotation to the transformation matrix.

The rotation is always about the canvas origin. To change the rotation center,
move the canvas using [translate](#canvas.translate) before performing the
rotation.

{: .parameters}
| angle | number | The angle to rotate, clockwise in radians.                  |

See also
[rotate](https://developer.mozilla.org/en-US/docs/Web/API/CanvasRenderingContext2D/rotate)
at MDN.


{% include method object="canvas" name="save" type="() => void" %}

Saves the entire state of the canvas by pushing it into a stack.

This state can be restored again later by calling [restore](#canvas.restore).

The drawing state that gets saved includes:

*  The transformation matrix.
*  The clipping region.
*  The dash list.
*  The properties: strokeStyle, fillStyle, globalAlpha, lineWidth, lineCap,
   lineJoin, miterLimit, lineDashOffset, shadowOffsetX, shadowOffsetY,
   shadowBlur, shadowColor, globalCompositeOperation, font, textAlign,
   textBaseline, imageSmoothingEnabled.

See also
[save](https://developer.mozilla.org/en-US/docs/Web/API/CanvasRenderingContext2D/save)
at MDN.


{% include method object="canvas" name="scale"
   type="(number, number) => void"
%}

Adds a scaling transformation to the canvas.

{: .parameters}
| x | number | Horizontal scaling factor.                                      |
| y | number | Vertical scaling factor.                                        |

See also
[scale](https://developer.mozilla.org/en-US/docs/Web/API/CanvasRenderingContext2D/scale)
at MDN.


{% include method object="canvas" name="setLineDash"
   type="(number[]) => void"
%}

Sets the line dash pattern used when stroking lines. Uses an array of values
that specify alternating lengths of lines and gaps, which describe the pattern.

To return to using solid lines, set the line dash list to an empty array.

{: .parameters}
| pattern | number[] | An Array of numbers that specify distances to alternately draw a line and a gap. If the number of elements in the array is odd, the elements of the array get copied and concatenated. For example, [5, 15, 25] will become [5, 15, 25, 5, 15, 25]. If the array is empty, the line dash list is cleared and line strokes return to being solid. |

See also
[setLineDash](https://developer.mozilla.org/en-US/docs/Web/API/CanvasRenderingContext2D/setLineDash)
at MDN.


{% include method object="canvas" name="setTransform"
   type="(number[]) => void"
%}

Sets the current transformation matrix.

The matrix is specified with 6 values that can be passed in a single array or
as 6 separate parameters. They represent this transformation matrix:

{: .matrix }
| `a` | `c` | `e` |
| `b` | `d` | `f` |
| `0` | `0` | `1` |

See also
[setTransform](https://developer.mozilla.org/en-US/docs/Web/API/CanvasRenderingContext2D/setTransform)
at MDN.


{% include method object="canvas" name="stroke" type="() => void" %}

Strokes the outline of the current path, using the current
[strokeStyle](#canvas.strokeStyle).

Strokes are aligned to the center of a path: half of the stroke is drawn on the
inner side, and half on the outer side. The stroke is drawn using the non-zero
winding rule.

See also
[stroke](https://developer.mozilla.org/en-US/docs/Web/API/CanvasRenderingContext2D/stroke)
at MDN.


{% include method object="canvas" name="strokeRect"
   type="(number, number, number, number) => void"
%}

Draws a rectangle that is stroked using the current
[strokeStyle](#canvas.strokeStyle).

{: .parameters}
| x | number | The horizontal coordinate of the rectangle's starting point.    |
| y | number | The vertical coordinate of the rectangle's starting point.      |
| w | number | The width of the rectangle.                                     |
| h | number | The height of the rectangle.                                    |

See also
[strokeRect](https://developer.mozilla.org/en-US/docs/Web/API/CanvasRenderingContext2D/strokeRect)
at MDN.


{% include method object="canvas" name="strokeText"
   type="(string, number, number) => void"
%}

Strokes the outlines the characters of the given text string, at the specified
coordinates.

{: .parameters}
| text | string | The text to stroke.                                          |
| x    | number | The horizontal coordinate of the point at which to draw the text. |
| y    | number | The vertical coordinate of the point at which to draw the text. |

See also
[strokeText](https://developer.mozilla.org/en-US/docs/Web/API/CanvasRenderingContext2D/strokeText)
at MDN.


{% include method object="canvas" name="transform"
   type="(number, number, number, number, number, number) => void"
%}

Multiples the current transformation matrix by the matrix described by the
parameters:

{: .matrix }
| `a` | `c` | `e` |
| `b` | `d` | `f` |
| `0` | `0` | `1` |

See also
[transform](https://developer.mozilla.org/en-US/docs/Web/API/CanvasRenderingContext2D/transform)
at MDN.


{% include method object="canvas" name="translate"
   type="(number, number) => void"
%}

Adds a translation transformation to the current matrix.

| x | number | The distance to move in the horizontal direction.               |
| y | number | The distance to move in the vertical direction.                 |

See also
[translate](https://developer.mozilla.org/en-US/docs/Web/API/CanvasRenderingContext2D/translate)
at MDN.
