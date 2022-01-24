---
layout: documentation
title: Window.js | Path2D
constructors:
  - Path2D
object-name: path
object-methods:
  - addPath
  - arc
  - arcTo
  - bezierCurveTo
  - closePath
  - ellipse
  - lineTo
  - moveTo
  - quadraticCurveTo
  - rect
---

Path2D
======

`Path2D` objects represent paths that can be drawn into a [canvas](/doc/canvas),
using [canvas.fill](/doc/canvas#canvas.fill) and
[canvas.stroke](/doc/canvas#canvas.stroke).

See also the
[Path2D](https://developer.mozilla.org/en-US/docs/Web/API/Path2D)
documentation at MDN.


{% include constructor class="Path2D" %}

Creates a new `Path2D`. The constructor has 3 variants.

The first takes no parameters and builds an empty path.

The second takes another `Path2D` instance and makes a copy.

The third takes an
[SVG path](https://developer.mozilla.org/en-US/docs/Web/SVG/Tutorial/Paths)
as a `string` parameter, and builds the corresponding path.


{% include method object="path" name="addPath" type="(Path2D) => void" %}

Adds the path of an existing `Path2D` object to this one.

See also
[addPath](https://developer.mozilla.org/en-US/docs/Web/API/Path2D/addPath)
at MDN.


{% include method object="path" name="arc"
   type="(number, number, number, number, number) => void"
%}

Adds a circular arc to the current path, centered at `(x, y)` and with the
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


{% include method object="path" name="arcTo"
   type="(number, number, number, number, number) => void"
%}

Adds a circular arc to the current path, using the given control points and
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


{% include method object="path" name="bezierCurveTo"
   type="(number, number, number, number, number, number) => void"
%}

Adds a cubic Bézier curve to the current path. Requires three points: two
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


{% include method object="path" name="closePath" type="() => void" %}

Adds a straight line from the current point to the start of the current
path. Does nothing if the shape has already been closed, or has only one
point.

See also
[closePath](https://developer.mozilla.org/en-US/docs/Web/API/CanvasRenderingContext2D/closePath)
at MDN.


{% include method object="path" name="ellipse"
   type="(number, number, number, number, number, number, number) => void"
%}

Adds an elliptical arc to the current path.

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


{% include method object="path" name="lineTo" type="(number, number) => void" %}

Connects the last point of the current path with a straight line to the
given point.

{: .parameters }
| x | number | The horizontal coordinate of the end point.                     |
| y | number | The vertical coordinate of the end point.                       |

See also
[lineTo](https://developer.mozilla.org/en-US/docs/Web/API/CanvasRenderingContext2D/lineTo)
at MDN.


{% include method object="path" name="moveTo" type="(number, number) => void" %}

Begins a new path at the given point.

{: .parameters }
| x | number | The horizontal coordinate of the point.                         |
| y | number | The vertical coordinate of the point.                           |

See also
[moveTo](https://developer.mozilla.org/en-US/docs/Web/API/CanvasRenderingContext2D/moveTo)
at MDN.


{% include method object="path" name="quadraticCurveTo"
   type="(number, number, number, number) => void"
%}

Adds a quadratic Bézier curve to the current path.

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


{% include method object="path" name="rect"
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
