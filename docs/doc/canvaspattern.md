---
layout: documentation
title: Window.js | CanvasPattern
object-name: pattern
object-methods:
  - setTransform
---

CanvasPattern
=============

`CanvasPattern` objects are created via
[canvas.createPattern](/doc/canvas#canvas.createPattern) and can be assigned to
the [canvas.fillStyle](/doc/canvas#canvas.fillStyle) and
[canvas.strokeStyle](/doc/canvas#canvas.strokeStyle) properties.

They can be used to customize the canvas drawing operations using a repeatable
pattern.

The pattern object can be further customized using
[setTransform](#pattern.setTransform).

See also the
[CanvasPattern](https://developer.mozilla.org/en-US/docs/Web/API/CanvasPattern)
documentation at MDN.


{% include method object="pattern" name="setTransform"
   type="(number, number, number, number, number, number) => void"
%}

Applies a matrix transformation to the pattern.

See also
[setTransform](https://developer.mozilla.org/en-US/docs/Web/API/CanvasPattern/setTransform)
at MDN.
