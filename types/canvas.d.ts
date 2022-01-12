
interface CanvasExtension {
    /** Whether shapes drawn in this canvas should be antialiased. Defaults to true. */
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
     */
    height: number;
    
}

interface ImageDataSettings {
    colorSpace?: PredefinedColorSpace;
}

interface CanvasCompositing {
    globalAlpha: number;
    globalCompositeOperation: string;
}

interface CanvasDrawImage {
    drawImage(image: CanvasImageSource, dx: number, dy: number): void;
    drawImage(image: CanvasImageSource, dx: number, dy: number, dw: number, dh: number): void;
    drawImage(image: CanvasImageSource, sx: number, sy: number, sw: number, sh: number, dx: number, dy: number, dw: number, dh: number): void;
}

interface CanvasDrawPath {
    beginPath(): void;
    clip(fillRule?: CanvasFillRule): void;
    clip(path: Path2D, fillRule?: CanvasFillRule): void;
    fill(fillRule?: CanvasFillRule): void;
    fill(path: Path2D, fillRule?: CanvasFillRule): void;
    isPointInPath(x: number, y: number, fillRule?: CanvasFillRule): boolean;
    isPointInPath(path: Path2D, x: number, y: number, fillRule?: CanvasFillRule): boolean;
    isPointInStroke(x: number, y: number): boolean;
    isPointInStroke(path: Path2D, x: number, y: number): boolean;
    stroke(): void;
    stroke(path: Path2D): void;
}

interface CanvasFillStrokeStyles {
    fillStyle: string | CanvasGradient | CanvasPattern;
    strokeStyle: string | CanvasGradient | CanvasPattern;
    createLinearGradient(x0: number, y0: number, x1: number, y1: number): CanvasGradient;
    createPattern(image: CanvasImageSource, repetition: string | null): CanvasPattern | null;
    createRadialGradient(x0: number, y0: number, r0: number, x1: number, y1: number, r1: number): CanvasGradient;
}

interface CanvasFilters {
    filter: string;
}

/** An opaque object describing a gradient. It is returned by the methods CanvasRenderingContext2D.createLinearGradient() or CanvasRenderingContext2D.createRadialGradient(). */
interface CanvasGradient {
    /**
     * Adds a color stop with the given color to the gradient at the given offset. 0.0 is the offset at one end of the gradient, 1.0 is the offset at the other end.
     *
     * Throws an "IndexSizeError" DOMException if the offset is out of range. Throws a "SyntaxError" DOMException if the color cannot be parsed.
     */
    addColorStop(offset: number, color: string): void;
}

declare var CanvasGradient: {
    prototype: CanvasGradient;
    new(): CanvasGradient;
};

interface CanvasImageData {
    createImageData(sw: number, sh: number, settings?: ImageDataSettings): ImageData;
    createImageData(imagedata: ImageData): ImageData;
    getImageData(sx: number, sy: number, sw: number, sh: number, settings?: ImageDataSettings): ImageData;
    putImageData(imagedata: ImageData, dx: number, dy: number): void;
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
    arc(x: number, y: number, radius: number, startAngle: number, endAngle: number, counterclockwise?: boolean): void;
    arcTo(x1: number, y1: number, x2: number, y2: number, radius: number): void;
    bezierCurveTo(cp1x: number, cp1y: number, cp2x: number, cp2y: number, x: number, y: number): void;
    closePath(): void;
    ellipse(x: number, y: number, radiusX: number, radiusY: number, rotation: number, startAngle: number, endAngle: number, counterclockwise?: boolean): void;
    lineTo(x: number, y: number): void;
    moveTo(x: number, y: number): void;
    quadraticCurveTo(cpx: number, cpy: number, x: number, y: number): void;
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
    miterLimit: number;
    getLineDash(): number[];
    setLineDash(segments: number[]): void;
}

/** An opaque object describing a pattern, based on an image, a canvas, or a video, created by the CanvasRenderingContext2D.createPattern() method. */
interface CanvasPattern {
    /**
     * Sets the transformation matrix that will be used when rendering the pattern during a fill or stroke painting operation.
     */
    setTransform(transform?: DOMMatrix2DInit): void;
}

declare var CanvasPattern: {
    prototype: CanvasPattern;
    new(): CanvasPattern;
};

interface CanvasRect {
    clearRect(x: number, y: number, w: number, h: number): void;
    fillRect(x: number, y: number, w: number, h: number): void;
    strokeRect(x: number, y: number, w: number, h: number): void;
}

/** The CanvasRenderingContext2D interface, part of the Canvas API, provides the 2D rendering context for the drawing surface of a <canvas> element. It is used for drawing shapes, text, images, and other objects. */
interface CanvasRenderingContext2D extends CanvasCompositing, CanvasDrawImage, CanvasDrawPath, CanvasFillStrokeStyles, CanvasFilters, CanvasImageData, CanvasImageSmoothing, CanvasPath, CanvasPathDrawingStyles, CanvasRect, CanvasShadowStyles, CanvasState, CanvasText, CanvasTextDrawingStyles, CanvasTransform {
}

declare var CanvasRenderingContext2D: {
    prototype: CanvasRenderingContext2D;
    new(): CanvasRenderingContext2D;
};

interface CanvasShadowStyles {
    shadowBlur: number;
    shadowColor: string;
    shadowOffsetX: number;
    shadowOffsetY: number;
}

interface CanvasState {
    restore(): void;
    save(): void;
}

interface CanvasText {
    fillText(text: string, x: number, y: number, maxWidth?: number): void;
    measureText(text: string): TextMetrics;
    strokeText(text: string, x: number, y: number, maxWidth?: number): void;
}

interface CanvasTextDrawingStyles {
    direction: CanvasDirection;
    font: string;
    textAlign: CanvasTextAlign;
    textBaseline: CanvasTextBaseline;
}

interface CanvasTransform {
    getTransform(): DOMMatrix;
    resetTransform(): void;
    rotate(angle: number): void;
    scale(x: number, y: number): void;
    setTransform(a: number, b: number, c: number, d: number, e: number, f: number): void;
    setTransform(transform?: DOMMatrix2DInit): void;
    transform(a: number, b: number, c: number, d: number, e: number, f: number): void;
    translate(x: number, y: number): void;
}

type DOMMatrix = [number, number, number, number, number, number];

type DOMMatrix2DInit = DOMMatrix;

interface ImageBitmap {
    /**
     * Returns the intrinsic height of the image, in CSS pixels.
     */
    readonly height: number;
    /**
     * Returns the intrinsic width of the image, in CSS pixels.
     */
    readonly width: number;
    /**
     * Releases imageBitmap's underlying bitmap data.
     */
    close(): void;
}

declare var ImageBitmap: {
    prototype: ImageBitmap;
    new(): ImageBitmap;
};

/** The underlying pixel data of an area of a <canvas> element. It is created using the ImageData() constructor or creator methods on the CanvasRenderingContext2D object associated with a canvas: createImageData() and getImageData(). It can also be used to set a part of the canvas by using putImageData(). */
interface ImageData {
    /**
     * Returns the one-dimensional array containing the data in RGBA order, as integers in the range 0 to 255.
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
}

declare var ImageData: {
    prototype: ImageData;
    new(sw: number, sh: number, settings?: ImageDataSettings): ImageData;
    new(data: Uint8ClampedArray, sw: number, sh?: number, settings?: ImageDataSettings): ImageData;
};

/** This Canvas 2D API interface is used to declare a path that can then be used on a CanvasRenderingContext2D object. The path methods of the CanvasRenderingContext2D interface are also present on this interface, which gives you the convenience of being able to retain and replay your path whenever desired. */
interface Path2D extends CanvasPath {
    /**
     * Adds to the path the path given by the argument.
     */
    addPath(path: Path2D, transform?: DOMMatrix2DInit): void;
}

declare var Path2D: {
    prototype: Path2D;
    new(path?: Path2D | string): Path2D;
};

/** The dimensions of a piece of text in the canvas, as created by the CanvasRenderingContext2D.measureText() method. */
interface TextMetrics {
    /**
     * Returns the measurement described below.
     */
    readonly actualBoundingBoxAscent: number;
    /**
     * Returns the measurement described below.
     */
    readonly actualBoundingBoxDescent: number;
    /**
     * Returns the measurement described below.
     */
    readonly actualBoundingBoxLeft: number;
    /**
     * Returns the measurement described below.
     */
    readonly actualBoundingBoxRight: number;
    /**
     * Returns the measurement described below.
     */
    readonly fontBoundingBoxAscent: number;
    /**
     * Returns the measurement described below.
     */
    readonly fontBoundingBoxDescent: number;
    /**
     * Returns the measurement described below.
     */
    readonly width: number;
}

declare var TextMetrics: {
    prototype: TextMetrics;
    new(): TextMetrics;
};

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
type ImageSmoothingQuality = "high" | "low" | "medium";
type PredefinedColorSpace = "display-p3" | "srgb";