interface CanvasCompositing {
    /**
     * Specifies the alpha (transparency) value that is applied to shapes and images
     * before they are drawn onto the canvas. The defaut value is `1.0`.
     * 
     * See also
     * [globalAlpha](https://developer.mozilla.org/en-US/docs/Web/API/CanvasRenderingContext2D/globalAlpha)
     * at MDN.
     */
    globalAlpha: number;

    /**
     * Sets the type of compositing operation to apply when drawing new shapes.
     * See the
     * [Compositing and clipping](https://developer.mozilla.org/en-US/docs/Web/API/Canvas_API/Tutorial/Compositing)
     * section of the
     * [Canvas tutorial](https://developer.mozilla.org/en-US/docs/Web/API/Canvas_API/Tutorial)
     * at MDN for a list of valid operations and their effect.
     */
    globalCompositeOperation: string;
}

interface CanvasDrawImage {
    /**
     * Draws an [ImageBitmap](/doc/imagebitmap) or an
     * [offscreen canvas](#CanvasRenderingContext2D) onto the main canvas in its original size at a given location.
     * @param image  The source image or canvas to draw.
     * @param dx  The horizontal coordinate in the destination canvas at which to place the top-left corner of the source image.
     * @param dy  The vertical coordinate in the destination canvas at which to place the top-left corner of the source image.
     */
    drawImage(image: CanvasImageSource, dx: number, dy: number): void;

    /**
     * Draws an [ImageBitmap](/doc/imagebitmap) or an
     * [offscreen canvas](#CanvasRenderingContext2D) onto the main canvas with a specific size and location.
     * @param image  The source image or canvas to draw.
     * @param dx  The horizontal coordinate in the destination canvas at which to place the top-left corner of the source image.
     * @param dy  The vertical coordinate in the destination canvas at which to place the top-left corner of the source image.
     * @param dw  The width to draw the source image in the destination canvas.
     * @param dh  The height to draw the source image in the destination canvas.
     */
    drawImage(image: CanvasImageSource, dx: number, dy: number, dw: number, dh: number): void;

    /**
     * Draws a sub region of an [ImageBitmap](/doc/imagebitmap) or an
     * [offscreen canvas](#CanvasRenderingContext2D) onto the main canvas with a specific size and location.
     * @param image  The source image or canvas to draw.
     * @param sx  The horizontal coordinate of the top-left corner of the sub region of the source image that will be drawn.
     * @param sy  The vertical coordinate of the top-left corner of the sub region of the source image that will be drawn.
     * @param sw  The width of the sub region of the source image that will be drawn.
     * @param sh  The height of the sub region of the source image that will be drawn.
     * @param dx  The horizontal coordinate in the destination canvas at which to place the top-left corner of the source image sub region.
     * @param dy  The vertical coordinate in the destination canvas at which to place the top-left corner of the source image sub region.
     * @param dw  The width to draw the source image sub region in the destination canvas.
     * @param dh  The height to draw the source image sub region in the destination canvas.
     */
    drawImage(image: CanvasImageSource, sx: number, sy: number, sw: number, sh: number, dx: number, dy: number, dw: number, dh: number): void;
}

interface CanvasDrawPath {
    /**
     * Starts a new path by emptying the list of sub-paths.
     * 
     * See also
     * [beginPath](https://developer.mozilla.org/en-US/docs/Web/API/CanvasRenderingContext2D/beginPath)
     * at MDN.
     */
    beginPath(): void;

    /**
     * Turns the current path into the current clipping region. The previous clipping
     * region, if any, is intersected with the current path to create the new clipping
     * region.
     * 
     * See also
     * [clip](https://developer.mozilla.org/en-US/docs/Web/API/CanvasRenderingContext2D/clip)
     * at MDN.
     * @param fillRule  The algorithm to determine if a point is inside or outside of the clipping region. Can be the `"nonzero"` winding rule (default), or the `"evenodd"` winding rule.
     */
    clip(fillRule?: CanvasFillRule): void;

    /**
     * Fills the current path with the current [fillStyle](#canvas.fillStyle).
     * 
     * See also
     * [fill](https://developer.mozilla.org/en-US/docs/Web/API/CanvasRenderingContext2D/fill)
     * at MDN.
     * @param fillRule  The algorithm to determine if a point is inside or outside of the clipping region. Can be the `"nonzero"` winding rule (default), or the `"evenodd"` winding rule.
     */
    fill(fillRule?: CanvasFillRule): void;

    /**
     * Returns whether the specified point is contained in the current path.
     * 
     * See also
     * [isPointInPath](https://developer.mozilla.org/en-US/docs/Web/API/CanvasRenderingContext2D/isPointInPath)
     * at MDN.
     * @param x  The horizontal coordinate of the point to check.
     * @param y  The vertical coordinate of the point to check.
     * @param fillRule  The algorithm to determine if a point is inside or outside of the clipping region. Can be the `"nonzero"` winding rule (default), or the `"evenodd"` winding rule.
     */
    isPointInPath(x: number, y: number, fillRule?: CanvasFillRule): boolean;

    /**
     * Returns whether the specified point is contained in the stroking of the current
     * path.
     * 
     * See also
     * [isPointInStroke](https://developer.mozilla.org/en-US/docs/Web/API/CanvasRenderingContext2D/isPointInStroke)
     * at MDN.
     * @param x  The horizontal coordinate of the point to check.
     * @param y  The vertical coordinate of the point to check.
     */
    isPointInStroke(x: number, y: number): boolean;

    /**
     * Strokes the outline of the current path, using the current
     * [strokeStyle](#canvas.strokeStyle).
     * 
     * Strokes are aligned to the center of a path: half of the stroke is drawn on the
     * inner side, and half on the outer side. The stroke is drawn using the non-zero
     * winding rule.
     * 
     * See also
     * [stroke](https://developer.mozilla.org/en-US/docs/Web/API/CanvasRenderingContext2D/stroke)
     * at MDN.
     */
    stroke(): void;
}

interface CanvasFillStrokeStyles {
    /**
     * The style to apply in the fill operations ([fill](#canvas.fill),
     * [fillRect](#canvas.fillRect) and [fillText](#canvas.fillText)).
     * 
     * Valid values are [CanvasGradient](/doc/canvasgradient) instances, or strings
     * representing a CSS color in one of these formats:
     * -  `#rgb`
     * -  `#rgba`
     * -  `#rrggbb`
     * -  `#rrggbbaa`
     * -  `rgb(r, g, b)`
     * -  `rgba(r, g, b, a)`
     * -  A
     * [CSS color name](https://developer.mozilla.org/en-US/docs/Web/CSS/color_value#color_keywords)
     * like `red` or `dodgerblue`.
     * 
     * See also
     * [fillStyle](https://developer.mozilla.org/en-US/docs/Web/API/CanvasRenderingContext2D/fillStyle)
     * at MDN.
     */
    fillStyle: string | CanvasGradient;

    /**
     * The style to apply in the stroke operations ([stroke](#canvas.stroke),
     * [strokeRect](#canvas.strokeRect) and [strokeText](#canvas.strokeText)).
     * 
     * Valid values are [CanvasGradient](/doc/canvasgradient) instances, or strings
     * representing a CSS color in one of these formats:
     * -  `#rgb`
     * -  `#rgba`
     * -  `#rrggbb`
     * -  `#rrggbbaa`
     * -  `rgb(r, g, b)`
     * -  `rgba(r, g, b, a)`
     * -  A
     * [CSS color name](https://developer.mozilla.org/en-US/docs/Web/CSS/color_value#color_keywords)
     * like `red` or `dodgerblue`.
     * 
     * See also
     * [strokeStyle](https://developer.mozilla.org/en-US/docs/Web/API/CanvasRenderingContext2D/fillStyle)
     * at MDN.
     */
    strokeStyle: string | CanvasGradient;

    /**
     * Creates a linear [CanvasGradient](/doc/canvasgradient) along the line connecting
     * the two given coordinates.
     * 
     * To be applied to a shape, the gradient must first be assigned to the
     * [fillStyle](#canvas.fillStyle) or [strokeStyle](#canvas.strokeStyle) properties.
     * 
     * See also
     * [createLinearGradient](https://developer.mozilla.org/en-US/docs/Web/API/CanvasRenderingContext2D/createLinearGradient)
     * at MDN.
     * @param x0  The horizontal coordinate of the start point.
     * @param y0  The vertical coordinate of the start point.
     * @param x1  The horizontal coordinate of the end point.
     * @param y1  The vertical coordinate of the end point.
     */
    createLinearGradient(x0: number, y0: number, x1: number, y1: number): CanvasGradient;

    /**
     * Creates a radial [CanvasGradient](/doc/canvasgradient) using the size and
     * coordinates of two circles.
     * 
     * To be applied to a shape, the gradient must first be assigned to the
     * [fillStyle](#canvas.fillStyle) or [strokeStyle](#canvas.strokeStyle) properties.
     * 
     * See also
     * [createRadialGradient](https://developer.mozilla.org/en-US/docs/Web/API/CanvasRenderingContext2D/createRadialGradient)
     * at MDN.
     * @param x0  The horizontal coordinate of the start circle's center.
     * @param y0  The vertical coordinate of the start circle's center.
     * @param r0  The radius of the start circle.
     * @param x1  The horizontal coordinate of the end circle's center.
     * @param y1  The vertical coordinate of the end circle's center.
     * @param r1  The radius of the end circle.
     */
    createRadialGradient(x0: number, y0: number, r0: number, x1: number, y1: number, r1: number): CanvasGradient;
}

/**
 * `CanvasGradient` objects are created via
 * [canvas.createLinearGradient](/doc/canvas#canvas.createLinearGradient) and
 * [canvas.createRadialGradient](/doc/canvas#canvas.createRadialGradient), and
 * can be assigned to the [canvas.fillStyle](/doc/canvas#canvas.fillStyle) and
 * [canvas.strokeStyle](/doc/canvas#canvas.strokeStyle) properties.
 * 
 * They can be used to customize the canvas drawing operations using a gradient.
 * 
 * The gradient object can be further customized using
 * [addColorStop](#gradient.addColorStop).
 * 
 * See also the
 * [CanvasGradient](https://developer.mozilla.org/en-US/docs/Web/API/CanvasGradient)
 * documentation at MDN.
 */
interface CanvasGradient {
    /**
     * Adds a new color stop, defined by an offset and a color.
     * 
     * See also
     * [addColorStop](https://developer.mozilla.org/en-US/docs/Web/API/CanvasGradient/addColorStop)
     * at MDN.
     * @param offset  A number between 0 and 1 (inclusive), representing the position of the color stop.
     * @param color  A CSS color representing the color of the stop.
     */
    addColorStop(offset: number, color: string): void;
}

declare var CanvasGradient: {
    prototype: CanvasGradient;
};

interface CanvasImageData {
    /**
     * Creates a new, blank [ImageData](/doc/imagedata) object with the specified
     * dimensions. All of the pixels in the new object are transparent black.
     * 
     * See also
     * [createImageData](https://developer.mozilla.org/en-US/docs/Web/API/CanvasRenderingContext2D/createImageData)
     * at MDN.
     * @param sw  The width for the new `ImageData` object.
     * @param sh  The height for the new `ImageData` object.
     */
    createImageData(sw: number, sh: number): ImageData;

    /**
     * @inheritdoc CanvasRenderingContext2D#createImageData()
     * @param imagedata  An existing `ImageData` object from which to copy the width and height. The image itself is **not** copied.
     */
    createImageData(imagedata: ImageData): ImageData;

    /**
     * Returns an [ImageData](/doc/imagedata) object containing the pixel data for a
     * specified region of the canvas. [ImageData](/doc/imagedata) can be painted
     * onto a canvas using the [putImageData](#canvas.putImageData) method.
     * 
     * This method is not affected by the canvas's transformation matrix.
     * 
     * See also
     * [getImageData](https://developer.mozilla.org/en-US/docs/Web/API/CanvasRenderingContext2D/getImageData)
     * at MDN.
     * @param sx  The horizontal coordinate of the top-left corner of the region to copy. Defaults to 0.
     * @param sy  The vertical coordinate of the top-left corner of the region to copy. Defaults to 0.
     * @param sw  The width of the region to copy. Defaults to the canvas's width.
     * @param sh  The height of the region to copy. Defaults to the canvas's height.
     */
    getImageData(sx?: number, sy?: number, sw?: number, sh?: number): ImageData;

    /**
     * Paints the data in the given [ImageData](/doc/imagedata) object onto the canvas.
     * 
     * See also
     * [putImageData](https://developer.mozilla.org/en-US/docs/Web/API/CanvasRenderingContext2D/putImageData)
     * at MDN.
     * @param imagedata  The pixel data to copy.
     * @param dx  The horizontal coordinate at which to place the image data in the canvas.
     * @param dy  The vertical coordinate at which to place the image data in the canvas.
     */
    putImageData(imagedata: ImageData, dx: number, dy: number): void;

    /**
     * Paints the data in the given [ImageData](/doc/imagedata) object onto the canvas.
     * 
     * See also
     * [putImageData](https://developer.mozilla.org/en-US/docs/Web/API/CanvasRenderingContext2D/putImageData)
     * at MDN.
     * @param imagedata  The pixel data to copy.
     * @param dx  The horizontal coordinate at which to place the image data in the canvas.
     * @param dy  The vertical coordinate at which to place the image data in the canvas.
     * @param dirtyX  The horizontal coordinate of the top-left corner of the `imageData` to start copying from.
     * @param dirtyY  The vertical coordinate of the top-left corner of the `imageData` to start copying from.
     * @param dirtyWidth  The width of the region to copy.
     * @param dirtyHeight  The height of the region to copy.
     */
    putImageData(imagedata: ImageData, dx: number, dy: number, dirtyX: number, dirtyY: number, dirtyWidth: number, dirtyHeight: number): void;
}

interface CanvasImageSmoothing {
    /**
     * Determines whether scaled images are smoothed or not. The default is true.
     * 
     * Images can be drawn via the [drawImage](#canvas.drawImage) API, and the
     * destination size in the canvas may be different from the source size in the
     * [ImageBitmap](/doc/imagebitmap). In those cases, the image gets scaled up or
     * down. This setting controls whether the rescaled image should look smoother
     * (when it's turned on), or sharper (when it's turned off).
     * 
     * Disabling `imageSmoothingEnabled` draws images with a more blocky, pixelated
     * style.
     * 
     * See also
     * [imageSmoothingEnabled](https://developer.mozilla.org/en-US/docs/Web/API/CanvasRenderingContext2D/imageSmoothingEnabled)
     * at MDN.
     */
    imageSmoothingEnabled: boolean;

    /**
     * Controls the quality of the smoothing applied to scaled images when
     * [imageSmoothingEnabled](#canvas.imageSmoothingEnabled) is enabled.
     * 
     * Valid values are `"low"`, `"medium"` and `"high"`.
     * 
     * See also
     * [imageSmoothingQuality](https://developer.mozilla.org/en-US/docs/Web/API/CanvasRenderingContext2D/imageSmoothingQuality)
     * at MDN.
     */
    imageSmoothingQuality: ImageSmoothingQuality;
}

interface CanvasPath {
    /**
     * Adds a circular arc to the current sub-path, centered at `(x, y)` and with the
     * given `radius`. The path starts at `startAngle` and ends at `endAngle`,
     * traveling in the clockwise direction.
     * 
     * See also
     * [arc](https://developer.mozilla.org/en-US/docs/Web/API/CanvasRenderingContext2D/arc)
     * at MDN.
     * @param x  The horizontal coordinate of the arc's center.
     * @param y  The vertical coordinate of the arc's center.
     * @param radius  The arc's radius.
     * @param startAngle  The arc's starting angle, in radians, measured from the x axis.
     * @param endAngle  The arc's ending angle, in radians, measured from the x axis.
     */
    arc(x: number, y: number, radius: number, startAngle: number, endAngle: number): void;

    /**
     * Adds a circular arc to the current sub-path, using the given control points and
     * radius. The arc is connected to the path's latest point with a straight line,
     * if necessary.
     * 
     * See also
     * [arcTo](https://developer.mozilla.org/en-US/docs/Web/API/CanvasRenderingContext2D/arcTo)
     * at MDN.
     * @param x1  The horizontal coordinate of the first control point.
     * @param y1  The vertical coordinate of the first control point.
     * @param x2  The horizontal coordinate of the second control point.
     * @param y2  The vertical coordinate of the second control point.
     * @param radius  The arc's radius.
     */
    arcTo(x1: number, y1: number, x2: number, y2: number, radius: number): void;

    /**
     * Adds a cubic BÃ©zier curve to the current sub-path. Requires three points: two
     * control points, and a final end point. The starting point is the latest point
     * in the current path (which can be changed using [moveTo](#canvas.moveTo) before
     * creating the curve).
     * @param cp1x  The horizontal coordinate of the first control point.
     * @param cp1y  The vertical coordinate of the first control point.
     * @param cp2x  The horizontal coordinate of the second control point.
     * @param cp2y  The vertical coordinate of the second control point.
     * @param x  The horizontal coordinate of the end point.
     * @param y  The vertical coordinate of the end point.
     */
    bezierCurveTo(cp1x: number, cp1y: number, cp2x: number, cp2y: number, x: number, y: number): void;

    /**
     * Adds a straight line from the current point to the start of the current
     * sub-path. Does nothing if the shape has already been closed, or has only one
     * point.
     * 
     * This method doesn't draw anything to the canvas directly; the path can be
     * rendered via the [fill](#canvas.fill) and [stroke](#canvas.stroke) methods.
     * 
     * See also
     * [closePath](https://developer.mozilla.org/en-US/docs/Web/API/CanvasRenderingContext2D/closePath)
     * at MDN.
     */
    closePath(): void;

    /**
     * Adds an elliptical arc to the current sub-path.
     * 
     * See also
     * [ellipse](https://developer.mozilla.org/en-US/docs/Web/API/CanvasRenderingContext2D/ellipse)
     * at MDN.
     * @param x  The horizontal coordinate of the center of the ellipse.
     * @param y  The vertical coordinate of the center of the ellipse.
     * @param radiusX  The radius of the major axis of the ellipse.
     * @param radiusY  The radius of the minor axis of the ellipse.
     * @param rotation  The rotation of the ellipse, in radians.
     * @param startAngle  The angle at which the ellipse starts, in radians, measured clockwise from the positive X axis.
     * @param endAngle  The angle at which the ellipse ends, in radians, measured clockwise from the positive X axis.
     */
    ellipse(x: number, y: number, radiusX: number, radiusY: number, rotation: number, startAngle: number, endAngle: number): void;

    /**
     * Connects the last point of the current sub-path with a straight line to the
     * given point.
     * 
     * See also
     * [lineTo](https://developer.mozilla.org/en-US/docs/Web/API/CanvasRenderingContext2D/lineTo)
     * at MDN.
     * @param x  The horizontal coordinate of the end point.
     * @param y  The vertical coordinate of the end point.
     */
    lineTo(x: number, y: number): void;

    /**
     * Begins a new sub-path at the given point.
     * 
     * See also
     * [moveTo](https://developer.mozilla.org/en-US/docs/Web/API/CanvasRenderingContext2D/moveTo)
     * at MDN.
     * @param x  The horizontal coordinate of the starting point.
     * @param y  The vertical coordinate of the starting point.
     */
    moveTo(x: number, y: number): void;

    /**
     * Adds a quadratic Bézier curve to the current sub-path.
     * 
     * The curve requires two points. The first is a control point, and the second is
     * the end point. The starting point is the latest point in the current path,
     * which can be changed using [moveTo](#canvas.moveTo) before creating the
     * quadratic Bézier curve.
     * 
     * See also
     * [quadraticCurveTo](https://developer.mozilla.org/en-US/docs/Web/API/CanvasRenderingContext2D/quadraticCurveTo)
     * at MDN.
     * @param cpx  The horizontal coordinate of the control point.
     * @param cpy  The vertical coordinate of the control point.
     * @param x  The horizontal coordinate of the end point.
     * @param y  The vertical coordinate of the end point.
     */
    quadraticCurveTo(cpx: number, cpy: number, x: number, y: number): void;

    /**
     * Adds a rectangle to the current path.
     * 
     * See also
     * [rect](https://developer.mozilla.org/en-US/docs/Web/API/CanvasRenderingContext2D/rect)
     * at MDN.
     * @param x  The horizontal coordinate of the rectangle's starting point.
     * @param y  The vertical coordinate of the rectangle's starting point.
     * @param w  The width of the rectangle.
     * @param h  The height of the rectangle.
     */
    rect(x: number, y: number, w: number, h: number): void;
}

interface CanvasPathDrawingStyles {
    /**
     * The shape used to draw the end points of lines.
     * 
     * See also
     * [lineCap](https://developer.mozilla.org/en-US/docs/Web/API/CanvasRenderingContext2D/lineCap)
     * at MDN.
     */
    lineCap: CanvasLineCap;

    /**
     * The line dash offset. Defaults to 0.
     * 
     * See also
     * [lineDashOffset](https://developer.mozilla.org/en-US/docs/Web/API/CanvasRenderingContext2D/lineDashOffset)
     * at MDN.
     */
    lineDashOffset: number;

    /**
     * The shape used to join two line segments where they meet.\
     * 
     * See also
     * [lineJoin](https://developer.mozilla.org/en-US/docs/Web/API/CanvasRenderingContext2D/lineJoin)
     * at MDN.
     */
    lineJoin: CanvasLineJoin;

    /**
     * The thickness of lines. The default value is 1.0.
     * 
     * See also
     * [lineWidth](https://developer.mozilla.org/en-US/docs/Web/API/CanvasRenderingContext2D/lineWidth)
     * at MDN.
     */
    lineWidth: number;

    /**
     * The miter limit ratio. The default value is 10.0.
     * 
     * See also
     * [miterLimit](https://developer.mozilla.org/en-US/docs/Web/API/CanvasRenderingContext2D/miterLimit)
     * at MDN.
     */
    miterLimit: number;

    /**
     * Returns the current line dash pattern.
     * 
     * See also
     * [getLineDash](https://developer.mozilla.org/en-US/docs/Web/API/CanvasRenderingContext2D/getLineDash)
     * at MDN.
     */
    getLineDash(): number[];

    /**
     * Sets the line dash pattern used when stroking lines. Uses an array of values
     * that specify alternating lengths of lines and gaps, which describe the pattern.
     * 
     * To return to using solid lines, set the line dash list to an empty array.
     * 
     * See also
     * [setLineDash](https://developer.mozilla.org/en-US/docs/Web/API/CanvasRenderingContext2D/setLineDash)
     * at MDN.
     * @param segments An Array of numbers that specify distances to alternately draw a line and a gap. If the number of elements in the array is odd, the elements of the array get copied and concatenated. For example, [5, 15, 25] will become [5, 15, 25, 5, 15, 25]. If the array is empty, the line dash list is cleared and line strokes return to being solid.
     */
    setLineDash(segments: number[]): void;
}

interface CanvasRect {
    /**
     * Erases the pixels in a rectangular area by setting them to transparent black.
     * 
     * See also
     * [clearRect](https://developer.mozilla.org/en-US/docs/Web/API/CanvasRenderingContext2D/clearRect)
     * at MDN.
     * @param x  The horizontal coordinate of the starting corner.
     * @param y  The vertical coordinate of the starting corner.
     * @param w  The width of the rectangle. Positive values are to the right, and negative to the left.
     * @param h  The height of the rectangle. Positive values are down, and negative are up. 
     */
    clearRect(x: number, y: number, w: number, h: number): void;

    /**
     * Draws a rectangle using the current [fillStyle](#canvas.fillStyle).
     * 
     * See also
     * [fillRect](https://developer.mozilla.org/en-US/docs/Web/API/CanvasRenderingContext2D/fillRect)
     * at MDN.
     * @param x  The horizontal coordinate of the starting corner.
     * @param y  The vertical coordinate of the starting corner.
     * @param w  The width of the rectangle. Positive values are to the right, and negative to the left.
     * @param h  The height of the rectangle. Positive values are down, and negative are up.
     */
    fillRect(x: number, y: number, w: number, h: number): void;

    /**
     * Draws a rectangle that is stroked using the current
     * [strokeStyle](#canvas.strokeStyle).
     * 
     * See also
     * [strokeRect](https://developer.mozilla.org/en-US/docs/Web/API/CanvasRenderingContext2D/strokeRect)
     * at MDN.
     * @param x  The horizontal coordinate of the starting corner.
     * @param y  The vertical coordinate of the starting corner.
     * @param w  The width of the rectangle. Positive values are to the right, and negative to the left.
     * @param h  The height of the rectangle. Positive values are down, and negative are up.
     */
    strokeRect(x: number, y: number, w: number, h: number): void;
}

/** The CanvasRenderingContext2D interface, part of the Canvas API, provides the 2D rendering context for the drawing surface of a <canvas> element. It is used for drawing shapes, text, images, and other objects. */
interface CanvasRenderingContext2D extends CanvasCompositing, CanvasDrawImage, CanvasDrawPath, CanvasFillStrokeStyles, CanvasImageData, CanvasImageSmoothing, CanvasPath, CanvasPathDrawingStyles, CanvasRect, CanvasShadowStyles, CanvasState, CanvasText, CanvasTextDrawingStyles, CanvasTransform {

    /**
     * Whether shapes drawn in this canvas should be antialiased. Defaults to true.
     * @extension
     */
    antialias: boolean;

    /**
     * The height of the canvas, in pixels.
     * 
     * **Note:** the size of the canvas is automatically adjusted when the window is
     * [resized](/doc/window#event-resize). If the main application supports
     * [resizing](/doc/window#window.resizable) then it should also listen for
     * ["resize"](/doc/window#event-resize) events and draw a full frame after each
     * resize. Window.js will also invoke any pending
     * [requestAnimationFrame](/doc/global#requestAnimationFrame) callbacks when
     * the window is resized.
     * 
     * @extension
     */
    height: number;

    /**
     * The width of the canvas, in pixels.
     * 
     * **Note:** the size of the canvas is automatically adjusted when the window is
     * [resized](/doc/window#event-resize). If the main application supports
     * [resizing](/doc/window#window.resizable) then it should also listen for
     * ["resize"](/doc/window#event-resize) events and draw a full frame after each
     * resize. Window.js will also invoke any pending
     * [requestAnimationFrame](/doc/global#requestAnimationFrame) callbacks when
     * the window is resized.
     * 
     * @extension
     */
    width: number;


    /**
     * Encodes the content of the canvas into a compressed image format and returns
     * it as bytes in an `ArrayBuffer`.
     * @extension
     * @param format  The image format to encode in. Valid values: `"png"` (default), `"jpeg"` and `"webp"`.
     * @param quality  The encoding quality for the `"jpeg"` codec.
     */
    encode(format?: ImageFormat, quality?: number): Promise<ArrayBuffer>;
}

declare var CanvasRenderingContext2D: {
    prototype: CanvasRenderingContext2D;

    /**
     * Creates a new off-screen canvas. Its contents can be drawn to the main canvas
     * in [window.canvas](/doc/window#window.canvas) via
     * [drawImage](#canvas.drawImage).
     * 
     * @param width  The width of the offscreen canvas.
     * @param height  The height of the offscreen canvas.
     */
    new(width: number, height: number): CanvasRenderingContext2D;
};

interface CanvasShadowStyles {
    /**
     * The amount of blur applied to shadows. The default is 0 (i.e. no blur).
     * 
     * See also
     * [shadowBlur](https://developer.mozilla.org/en-US/docs/Web/API/CanvasRenderingContext2D/shadowBlur)
     * at MDN.
     */
    shadowBlur: number;

    /**
     * The color of shadows, as a CSS color.
     * 
     * The shadow's rendered opacity is affected by the opacity of the
     * [fillStyle](#canvas.fillStyle) color when filling, and of the
     * [strokeStyle](#canvas.strokeStyle) color when stroking.
     * 
     * **Note:** Shadows are only drawn if the `shadowColor` property is set to a
     * non-transparent value. One of the [shadowBlur](#canvas.shadowBlur),
     * [shadowOffsetX](#canvas.shadowOffsetX) or [shadowOffsetY](#canvas.shadowOffsetY)
     * properties must be non-zero as well.
     * 
     * The CSS color must be in one of these formats:
     * 
     * *  `#rgb`
     * *  `#rgba`
     * *  `#rrggbb`
     * *  `#rrggbbaa`
     * *  `rgb(r, g, b)`
     * *  `rgba(r, g, b, a)`
     * *  A
     * [CSS color name](https://developer.mozilla.org/en-US/docs/Web/CSS/color_value#color_keywords)
     * like `red` or `dodgerblue`.
     * 
     * See also
     * [shadowColor](https://developer.mozilla.org/en-US/docs/Web/API/CanvasRenderingContext2D/shadowColor)
     * at MDN.
     */
    shadowColor: string;

    /**
     * The distance that shadows will be offset horizontally.
     * 
     * See also
     * [shadowOffsetX](https://developer.mozilla.org/en-US/docs/Web/API/CanvasRenderingContext2D/shadowOffsetX)
     * at MDN.
     */
    shadowOffsetX: number;

    /**
     * The distance that shadows will be offset vertically.
     * 
     * See also
     * [shadowOffsetY](https://developer.mozilla.org/en-US/docs/Web/API/CanvasRenderingContext2D/shadowOffsetY)
     * at MDN.
     */
    shadowOffsetY: number;
}

interface CanvasState {
    /**
     * Restores the most recently saved canvas state, from a previous call to
     * [save](#canvas.save). If there is no saved state then this method does nothing.
     */
    restore(): void;

    /**
     * Saves the entire state of the canvas by pushing it into a stack.
     * 
     * This state can be restored again later by calling [restore](#canvas.restore).
     * 
     * The drawing state that gets saved includes:
     * 
     * *  The transformation matrix.
     * *  The clipping region.
     * *  The dash list.
     * *  The properties: strokeStyle, fillStyle, globalAlpha, lineWidth, lineCap,
     *    lineJoin, miterLimit, lineDashOffset, shadowOffsetX, shadowOffsetY,
     *    shadowBlur, shadowColor, globalCompositeOperation, font, textAlign,
     *    textBaseline, imageSmoothingEnabled.
     * 
     * See also
     * [save](https://developer.mozilla.org/en-US/docs/Web/API/CanvasRenderingContext2D/save)
     * at MDN.
     */
    save(): void;
}

interface CanvasText {
    /**
     * Draws a text string at the specified coordinates using the current
     * [fillStyle](#canvas.fillStyle), [font](#canvas.font),
     * [textAlign](#canvas.textAlign) and [textBaseline](#canvas.textBaseline)\
     * properties.
     * 
     * See also
     * [fillText](https://developer.mozilla.org/en-US/docs/Web/API/CanvasRenderingContext2D/fillText)
     * at MDN.
     * @param text  The text string to draw. 
     * @param x  The horizontal coordinate at which to begin drawing.
     * @param y  The vertical coordinate at which to begin drawing.
     */
    fillText(text: string, x: number, y: number): void;

    /**
     * Returns an object with the text metrics about the given text string.
     * 
     * See also
     * [measureText](https://developer.mozilla.org/en-US/docs/Web/API/CanvasRenderingContext2D/measureText)
     * at MDN.
     * @param text  The text string to measure.
     */
    measureText(text: string): TextMetrics;

    /**
     * Strokes the outlines the characters of the given text string, at the specified\
     * coordinates.
     * 
     * See also
     * [strokeText](https://developer.mozilla.org/en-US/docs/Web/API/CanvasRenderingContext2D/strokeText)
     * at MDN.
     * @param text  The text to stroke.
     * @param x  The horizontal coordinate of the point at which to draw the text.
     * @param y  The vertical coordinate of the point at which to draw the text.
     */
    strokeText(text: string, x: number, y: number): void;
}

interface CanvasTextDrawingStyles {
    /**
     * The font to use in text operations ([fillText](#canvas.fillText),
     * [strokeText](#canvas.strokeText) and [measureText](#canvas.measureText)).
     * 
     * The list of fonts available in the current platform are listed in
     * [window.fonts](/doc/window#window.fonts).
     * 
     * Custom fonts can be loaded with [window.loadFont](/doc/window#window.loadFont).
     * 
     * Font names with spaces like "Segoe UI" must be wrapped in double quotes.
     * 
     * The font name can be optionally preceeded by `italic`, `bold` and the font
     * size as `Npx`. These are all valid font settings:
     * 
     * -  `monospace`
     * -  `bold 16px "Segoe UI"`
     * 
     * See also
     * [font](https://developer.mozilla.org/en-US/docs/Web/API/CanvasRenderingContext2D/font)
     * at MDN.
     */
    font: string;

    /**
     * The text alignment used when drawing text. The default is `"left"`.
     * 
     * * `"left"`: aligns text to the left.
     * * `"right"`: aligns text to the right.
     * * `"center"`: text is centered.
     * * `"start"`: aligns text to the left.
     * * `"end"`: aligns text to the right.
     * 
     * See also
     * [textAlign](https://developer.mozilla.org/en-US/docs/Web/API/CanvasRenderingContext2D/textAlign)
     * at MDN.
     */
    textAlign: CanvasTextAlign;
    
    /**
     * The text baseline used when drawing text. The default is `"alphabetic"`.
     * 
     * * `"top"`: The text baseline is the top of the em square.
     * * `"hanging"`: The text baseline is the hanging baseline.
     * * `"middle"`: The text baseline is the middle of the em square.
     * * `"alphabetic"`: The text baseline is the alphabetic baseline.
     * * `"ideographic"`: The text baseline is the ideographic baseline. This is the bottom of the body of the characters, if the main body of characters protrudes beneath the alphabetic baseline. (Used by Chinese, Japanese, and Korean scripts).
     * * `"bottom"`: The text baseline is the bottom of the bounding box. This differs from the ideographic baseline in that the ideographic baseline doesn't consider descenders.
     * 
     * See also
     * [textBaseline](https://developer.mozilla.org/en-US/docs/Web/API/CanvasRenderingContext2D/textBaseline)
     * at MDN.
     */
    textBaseline: CanvasTextBaseline;
}

interface CanvasTransform {
    /**
     * Returns the transformation matrix, as an array with 6 elements.
     * 
     * See also
     * [getTransform](https://developer.mozilla.org/en-US/docs/Web/API/CanvasRenderingContext2D/getTransform)
     * at MDN.
     */
    getTransform(): Matrix2D;

    /**
     * Resets the current transform to the identity matrix.
     * 
     * See also
     * [resetTransform](https://developer.mozilla.org/en-US/docs/Web/API/CanvasRenderingContext2D/resetTransform)
     * at MDN.
     */
    resetTransform(): void;

    /**
     * Adds a rotation to the transformation matrix.
     * 
     * See also
     * [rotate](https://developer.mozilla.org/en-US/docs/Web/API/CanvasRenderingContext2D/rotate)
     * at MDN.
     * @param angle  The angle to rotate, clockwise, in radians.
     */
    rotate(angle: number): void;

    /**
     * Adds a scaling transformation to the canvas.
     * 
     * See also
     * [scale](https://developer.mozilla.org/en-US/docs/Web/API/CanvasRenderingContext2D/scale)
     * at MDN.
     * @param x  Horizontal scaling factor. 
     * @param y  Vertical scaling factor.
     */
    scale(x: number, y: number): void;

    /**
     * Sets the current transformation matrix.
     * 
     * See also
     * [setTransform](https://developer.mozilla.org/en-US/docs/Web/API/CanvasRenderingContext2D/setTransform)
     * at MDN.
     * @param a 
     * @param b 
     * @param c 
     * @param d 
     * @param e 
     * @param f 
     */
    setTransform(a: number, b: number, c: number, d: number, e: number, f: number): void;

    /**
     * Sets the current transformation matrix.
     * 
     * See also
     * [setTransform](https://developer.mozilla.org/en-US/docs/Web/API/CanvasRenderingContext2D/setTransform)
     * at MDN.
     * @param transform  An array of 6 elements representing the transform matrix.
     */
    setTransform(transform: Matrix2D): void;

    /**
     * Multiples the current transformation matrix by the matrix described by the
     * parameters.
     * 
     * See also
     * [transform](https://developer.mozilla.org/en-US/docs/Web/API/CanvasRenderingContext2D/transform)
     * at MDN.
     * @param a 
     * @param b 
     * @param c 
     * @param d 
     * @param e 
     * @param f 
     */
    transform(a: number, b: number, c: number, d: number, e: number, f: number): void;

    /**
     * Adds a translation transformation to the current matrix.
     * 
     * See also
     * [translate](https://developer.mozilla.org/en-US/docs/Web/API/CanvasRenderingContext2D/translate)
     * at MDN.
     * @param x  The distance to move in the horizontal direction.
     * @param y  The distance to move in the vertical direction.
     */
    translate(x: number, y: number): void;
}

/**
 * | `a` | `c` | `e` |
 * |-----|-----|-----|
 * | `b` | `d` | `f` |
 * | `0` | `0` | `1` |
 */
type Matrix2D = [number, number, number, number, number, number];

/**
 * An `ImageBitmap` represents an image texture in GPU memory, that can be drawn
 * efficiently to the [canvas](/doc/canvas) via
 * [canvas.drawImage](/doc/canvas#canvas.drawImage).
 * 
 * An `ImageBitmap` can be created in several ways:
 * 
 * *  By copying an existing `ImageBitmap` or from the pixels in an
 *    [ImageData](/doc/imagedata) object via the
 *    [ImageBitmap constructor](#ImageBitmap).
 * *  By decoding an image encoded as a `JPEG`, `PNG` or `WEBP` image in an array
 *    of bytes via [ImageBitmap.decode](#ImageBitmap.decode).
 * *  By decoding an image encoded as a `JPEG`, `PNG` or `WEBP` image in a file
 *    via [File.readImageBitmap](/doc/file#File.readImageBitmap).
 * 
 * An `ImageBitmap` is appropriate to hold images loaded from files for fast
 * drawing. See [ImageData](/doc/imagedata) for a similar class that gives access
 * to the underlying pixels.
 * 
 * See also the
 * [ImageBitmap](https://developer.mozilla.org/en-US/docs/Web/API/ImageBitmap)
 * documentation on MDN.
 */
interface ImageBitmap {
    /**
     * Returns the intrinsic height of the image, in pixels.
     */
    readonly height: number;
    /**
     * Returns the intrinsic width of the image, in pixels.
     */
    readonly width: number;

    /**
     * Encodes this `ImageBitmap` in a given image format and returns the bytes
     * representing that encoding.
     * @param codec  The image codec to use for the encoding. Valid values are `"jpeg"`, `"png"` and `"webp"`.
     * @param quality  For the `"jpeg"` codec, the `quality` parameter is a number from 0 to 100 indicating the quality of the output image. 0 is smaller but lower quality, 100 is the highest quality but also a larger encoding.
     * @extension
     */
    encode(codec?: ImageFormat, quality?: number): Promise<ArrayBuffer>;
}

declare var ImageBitmap: {
    prototype: ImageBitmap;

    /**
     * Creates a new `ImageBitmap` with a copy of the pixels in another `ImageBitmap`
     * or in an [ImageData](/doc/imagedata).
     * @param source  The source image pixels to copy.
     * @param x  The horizontal coordinate of the source rectangle to copy. Defaults to 0.
     * @param y  The vertical coordinate of the source rectangle to copy. Defaults to 0.
     * @param w  The width of the source rectangle to copy. Defaults to the width of the `source`.
     * @param h  The height of the source rectangle to copy. Defaults to the height of the `source`.
     * @extension
     */
    new(source: ImageBitmap | ImageData, x?: number, y?: number, w?: number, h?: number): ImageBitmap;

    /**
     * Returns a new `ImageBitmap`, decoded from the given image bytes.
     * 
     * The valid input formats are `JPEG`, `PNG` and `WEBP`.
     * @param data 
     * @extension
     */
     decode(data: Uint8Array | Uint8ClampedArray | ArrayBuffer): Promise<ImageBitmap>;
};

/**
 * An `ImageData` represents raw image pixels as a two-dimensional array in RAM.
 * 
 * An `ImageData` can be created in several ways:
 * 
 * *  By creating a `new ImageData` with a given size and blank pixels, another
 *    `ImageData`, or with a given array of pixel data.
 * *  By creating a `new ImageData` by calling
 *    [canvas.createPixelData](/doc/canvas#canvas.createPixelData).
 * *  By copying the pixels in the [canvas](/doc/canvas) via
 *    [canvas.getImageData](/doc/canvas#canvas.getImageData)
 * *  By decoding an image encoded as a `JPEG`, `PNG` or `WEBP` image in an array
 *    of bytes via [ImageData.decode](#ImageData.decode).
 * *  By decoding an image encoded as a `JPEG`, `PNG` or `WEBP` image in a file
 *    via [File.readImageData](/doc/file#File.readImageData).
 * 
 * An `ImageData` allows reading and writing the underlying pixels. See
 * [ImageBitmap](/doc/imagebitmap) for a similar class that is more efficient for
 * storing image content that is only going to be drawn.
 * 
 * See also the
 * [ImageData](https://developer.mozilla.org/en-US/docs/Web/API/ImageData)
 * documentation on MDN.
 */
interface ImageData {
    /**
     * Returns the one-dimensional array containing the data in RGBA order, as integers in the range 0 to 255.
     * 
     * The alpha channel represents the transparency of the pixel: 0 is fully transparent, and 255 is fully opaque.
     * 
     * Pixels are stored in left-to-right, top-to-bottom order.
     */
    readonly data: Uint8ClampedArray;
    /**
     * Returns the actual dimensions of the data in the ImageData object, in pixels.
     */
    readonly height: number;
    /**
     * Returns the actual dimensions of the data in the ImageData object, in pixels.
     */
    readonly width: number;

    /**
     * Encodes this `ImageData` in a given image format and returns the bytes
     * representing that encoding.
     * @param codec  The image codec to use for the encoding. Valid values are `"jpeg"`, `"png"` and `"webp"`.
     * @param quality  For the `"jpeg"` codec, the `quality` parameter is a number from 0 to 100 indicating the quality of the output image. 0 is smaller but lower quality, 100 is the highest quality but also a larger encoding.
     * @extension
     */
    encode(codec?: ImageFormat, quality?: number): Promise<ArrayBuffer>;
}

declare var ImageData: {
    prototype: ImageData;

    /**
     * Creates a blank `ImageData`
     * @param sw  The width of the `ImageData`. 
     * @param sh  The height of the `ImageData`. 
     */
    new(sw: number, sh: number): ImageData;

    /**
     * Creates an `ImageData` with the same size as another `ImageData`.
     * Note that the `source` pixel data is *not* copied.
     * @param source  The `ImageData` whose *dimensions* are copied. The pixels of the new `ImageData` object are all blank.
     * @extension
     */
    new(source: ImageData): ImageData;

    /**
     * Creates an `ImageData` with a copy of a subregion of another `ImageData`.
     * @param source  The `ImageData` to copy pixels from.
     * @param x  The horizontal coordinate to start copying from.
     * @param y  The vertical coordinate to start copying from.
     * @param width  The number of pixels to copy per row.
     * @param height  The number of rows to copy.
     * @extension
     */
    new(source: ImageData, x: number, y: number, width: number, height: number);

    /**
     * Creates an `ImageData` initialized with pixel data from an array.
     * 
     * @param data  The pixel data to copy. Each pixel must have 4 entries in the array, representing the Red, Green, Blue and Alpha channels. The array size *must* be a multiple of `4 * width`.
     * @param sw  The width of the 2D pixel array.
     * @param sh  Optionally, the height of the 2D pixel array. Defaults to `array.byteLength / (4 * width)` if not set.
     */
    new(data: Uint8ClampedArray | Uint8Array | Uint16Array | Float32Array, sw: number, sh?: number): ImageData;

    /**
     * Returns a new `ImageData`, decoded from the given image bytes.
     * 
     * The valid input formats are `JPEG`, `PNG` and `WEBP`.
     * @param data 
     * @extension
     */
    decode(data: Uint8Array | Uint8ClampedArray | ArrayBuffer): Promise<ImageData>;
};

/** The dimensions of a piece of text in the canvas, as created by the CanvasRenderingContext2D.measureText() method. */
interface TextMetrics {
    /**
     * The distance from the text baseline to the top of the highest bounding rectangle of the given text.
     */
    readonly actualBoundingBoxAscent: number;
    /**
     * The distance from the text baseline to the bottom of the lowest bounding rectangle of the given text.
     */
    readonly actualBoundingBoxDescent: number;
    /**
     * The distance from the text alignment to the left of the given text.
     */
    readonly actualBoundingBoxLeft: number;
    /**
     * The distance from the text alignment to the right of the given text.
     */
    readonly actualBoundingBoxRight: number;
    /**
     * The calculated width of the given text string, using the current font and styling properties.
     */
    readonly width: number;
}

type CanvasImageSource = ImageBitmap | CanvasRenderingContext2D;
type CanvasDirection = "inherit" | "ltr" | "rtl";
type CanvasFillRule = "evenodd" | "nonzero";
type CanvasFontKerning = "auto" | "none" | "normal";
type CanvasFontStretch = "condensed" | "expanded" | "extra-condensed" | "extra-expanded" | "normal" | "semi-condensed" | "semi-expanded" | "ultra-condensed" | "ultra-expanded";
type CanvasFontVariantCaps = "all-petite-caps" | "all-small-caps" | "normal" | "petite-caps" | "small-caps" | "titling-caps" | "unicase";
type CanvasLineCap = "butt" | "round" | "square";
type CanvasLineJoin = "bevel" | "miter" | "round";
type CanvasTextAlign = "center" | "end" | "left" | "right" | "start";
type CanvasTextBaseline = "alphabetic" | "bottom" | "hanging" | "ideographic" | "middle" | "top";
type CanvasTextRendering = "auto" | "geometricPrecision" | "optimizeLegibility" | "optimizeSpeed";
type ImageFormat = "jpeg" | "png" | "webp";
type ImageSmoothingQuality = "high" | "low" | "medium";
