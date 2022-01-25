---
layout: documentation
title: Window.js | CanvasGradient
object-name: gradient
object-methods:
  - addColorStop
---

CanvasGradient
==============

`CanvasGradient` objects are created via
[canvas.createLinearGradient](/doc/canvas#canvas.createLinearGradient) and
[canvas.createRadialGradient](/doc/canvas#canvas.createRadialGradient), and
can be assigned to the [canvas.fillStyle](/doc/canvas#canvas.fillStyle) and
[canvas.strokeStyle](/doc/canvas#canvas.strokeStyle) properties.

They can be used to customize the canvas drawing operations using a gradient.

The gradient object can be further customized using
[addColorStop](#gradient.addColorStop).

See also the
[CanvasGradient](https://developer.mozilla.org/en-US/docs/Web/API/CanvasGradient)
documentation at MDN.


{% include method object="gradient" name="addColorStop"
   type="(number, string) => void"
%}

Adds a new color stop, defined by an offset and a color.

{: .parameters}
| offset | number | A number between 0 and 1 (inclusive), representing the position of the color stop. |
| color  | string | A CSS color representing the color of the stop.            |

See also
[addColorStop](https://developer.mozilla.org/en-US/docs/Web/API/CanvasGradient/addColorStop)
at MDN.
