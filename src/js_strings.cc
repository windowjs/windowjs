#include "js_strings.h"

JsStrings::JsStrings(v8::Isolate* isolate) {
#define SET_STRING(string)                          \
  strings_[static_cast<int>(StringId::string)].Set( \
      isolate, v8::String::NewFromUtf8Literal(      \
                   isolate, #string, v8::NewStringType::kInternalized))

#define SET_SPECIAL(name, string)                 \
  strings_[static_cast<int>(StringId::name)].Set( \
      isolate, v8::String::NewFromUtf8Literal(    \
                   isolate, string, v8::NewStringType::kInternalized))

  SET_STRING(a);
  SET_STRING(actualBoundingBoxAscent);
  SET_STRING(actualBoundingBoxDescent);
  SET_STRING(actualBoundingBoxLeft);
  SET_STRING(actualBoundingBoxRight);
  SET_STRING(addColorStop);
  SET_STRING(addEventListener);
  SET_STRING(alphabetic);
  SET_STRING(Alt);
  SET_STRING(altKey);
  SET_STRING(AltLeft);
  SET_STRING(AltRight);
  SET_STRING(alwaysOnTop);
  SET_STRING(antialias);
  SET_STRING(arc);
  SET_STRING(arcTo);
  SET_STRING(args);
  SET_STRING(ArrowDown);
  SET_STRING(ArrowLeft);
  SET_STRING(ArrowRight);
  SET_STRING(ArrowUp);
  SET_STRING(availHeight);
  SET_STRING(availWidth);
  SET_STRING(b);
  SET_STRING(Backquote);
  SET_STRING(Backslash);
  SET_STRING(Backspace);
  SET_STRING(base64ToArrayBuffer);
  SET_STRING(basename);
  SET_STRING(beginPath);
  SET_STRING(bevel);
  SET_STRING(bezierCurveTo);
  SET_STRING(blur);
  SET_STRING(bottom);
  SET_STRING(BracketLeft);
  SET_STRING(BracketRight);
  SET_STRING(butt);
  SET_STRING(button);
  SET_STRING(c);
  SET_STRING(cancelAnimationFrame);
  SET_STRING(canvas);
  SET_STRING(CanvasGradient);
  SET_STRING(CanvasPattern);
  SET_STRING(CanvasRenderingContext2D);
  SET_STRING(CapsLock);
  SET_STRING(center);
  SET_STRING(clearRect);
  SET_STRING(clearTimeout);
  SET_STRING(click);
  SET_STRING(clientX);
  SET_STRING(clientY);
  SET_STRING(clip);
  SET_STRING(close);
  SET_STRING(closePath);
  SET_STRING(code);
  SET_STRING(Codec);
  SET_STRING(color);
  SET_STRING(Comma);
  SET_STRING(content);
  SET_STRING(ContextMenu);
  SET_STRING(Control);
  SET_STRING(ControlLeft);
  SET_STRING(ControlRight);
  SET_STRING(copy);
  SET_STRING(copyTree);
  SET_STRING(cpus);
  SET_STRING(createImageData);
  SET_STRING(createLinearGradient);
  SET_STRING(createPattern);
  SET_STRING(createRadialGradient);
  SET_STRING(ctrlKey);
  SET_STRING(currentTransform);
  SET_STRING(cursor);
  SET_STRING(cursorOffsetX);
  SET_STRING(cursorOffsetY);
  SET_STRING(cwd);
  SET_STRING(d);
  SET_STRING(darken);
  SET_STRING(data);
  SET_STRING(debug);
  SET_STRING(decode);
  SET_STRING(decorated);
  SET_STRING(Delete);
  SET_STRING(deltaX);
  SET_STRING(deltaY);
  SET_STRING(devicePixelRatio);
  SET_STRING(difference);
  SET_STRING(Digit0);
  SET_STRING(Digit1);
  SET_STRING(Digit2);
  SET_STRING(Digit3);
  SET_STRING(Digit4);
  SET_STRING(Digit5);
  SET_STRING(Digit6);
  SET_STRING(Digit7);
  SET_STRING(Digit8);
  SET_STRING(Digit9);
  SET_STRING(dirname);
  SET_STRING(drawImage);
  SET_STRING(drop);
  SET_STRING(e);
  SET_STRING(ellipse);
  SET_STRING(encode);
  SET_STRING(end);
  SET_STRING(End);  // Keep after | sort | uniq.
  SET_STRING(Enter);
  SET_STRING(Equal);
  SET_STRING(error);
  SET_STRING(Escape);
  SET_STRING(exception);
  SET_STRING(exclusion);
  SET_STRING(exit);
  SET_STRING(f);
  SET_STRING(F1);
  SET_STRING(F10);
  SET_STRING(F11);
  SET_STRING(F12);
  SET_STRING(F13);
  SET_STRING(F14);
  SET_STRING(F15);
  SET_STRING(F16);
  SET_STRING(F17);
  SET_STRING(F18);
  SET_STRING(F19);
  SET_STRING(F2);
  SET_STRING(F20);
  SET_STRING(F21);
  SET_STRING(F22);
  SET_STRING(F23);
  SET_STRING(F24);
  SET_STRING(F25);
  SET_STRING(F3);
  SET_STRING(F4);
  SET_STRING(F5);
  SET_STRING(F6);
  SET_STRING(F7);
  SET_STRING(F8);
  SET_STRING(F9);
  SET_STRING(File);
  SET_STRING(files);
  SET_STRING(fill);
  SET_STRING(fillRect);
  SET_STRING(fillStyle);
  SET_STRING(fillText);
  SET_STRING(focus);
  SET_STRING(focused);
  SET_STRING(font);
  SET_STRING(fonts);
  SET_STRING(frameBottom);
  SET_STRING(frameLeft);
  SET_STRING(frameRight);
  SET_STRING(frameTop);
  SET_STRING(fullscreen);
  SET_STRING(g);
  SET_STRING(getClipboardText);
  SET_STRING(getImageData);
  SET_STRING(getLineDash);
  SET_STRING(getTransform);
  SET_STRING(globalAlpha);
  SET_STRING(globalCompositeOperation);
  SET_STRING(h);
  SET_STRING(hanging);
  SET_STRING(height);
  SET_STRING(Home);
  SET_STRING(home);
  SET_STRING(hue);
  SET_STRING(i);
  SET_STRING(icon);
  SET_STRING(ideographic);
  SET_STRING(imageSmoothingEnabled);
  SET_STRING(imageSmoothingQuality);
  SET_STRING(ImageBitmap);
  SET_STRING(ImageData);
  SET_STRING(Insert);
  SET_STRING(isDir);
  SET_STRING(isFile);
  SET_STRING(isPointInPath);
  SET_STRING(isPointInStroke);
  SET_STRING(j);
  SET_STRING(js);
  SET_STRING(jsHeapSizeLimit);
  SET_STRING(k);
  SET_STRING(keepAspectRatio);
  SET_STRING(key);
  SET_STRING(KeyA);
  SET_STRING(KeyB);
  SET_STRING(KeyC);
  SET_STRING(KeyD);
  SET_STRING(keydown);
  SET_STRING(KeyE);
  SET_STRING(KeyF);
  SET_STRING(KeyG);
  SET_STRING(KeyH);
  SET_STRING(KeyI);
  SET_STRING(KeyJ);
  SET_STRING(KeyK);
  SET_STRING(KeyL);
  SET_STRING(KeyM);
  SET_STRING(KeyN);
  SET_STRING(KeyO);
  SET_STRING(KeyP);
  SET_STRING(keypress);
  SET_STRING(KeyQ);
  SET_STRING(KeyR);
  SET_STRING(KeyS);
  SET_STRING(KeyT);
  SET_STRING(KeyU);
  SET_STRING(keyup);
  SET_STRING(KeyV);
  SET_STRING(KeyW);
  SET_STRING(KeyX);
  SET_STRING(KeyY);
  SET_STRING(KeyZ);
  SET_STRING(l);
  SET_STRING(left);
  SET_STRING(level);
  SET_STRING(lighten);
  SET_STRING(lighter);
  SET_STRING(lineCap);
  SET_STRING(lineDashOffset);
  SET_STRING(lineJoin);
  SET_STRING(lineTo);
  SET_STRING(lineWidth);
  SET_STRING(list);
  SET_STRING(listTree);
  SET_STRING(loadFont);
  SET_STRING(location);
  SET_STRING(log);
  SET_STRING(luminosity);
  SET_STRING(m);
  SET_STRING(maximize);
  SET_STRING(maximized);
  SET_STRING(measureText);
  SET_STRING(memory);
  SET_STRING(message);
  SET_STRING(Meta);
  SET_STRING(metaKey);
  SET_STRING(MetaLeft);
  SET_STRING(MetaRight);
  SET_STRING(middle);
  SET_STRING(minimize);
  SET_STRING(minimized);
  SET_STRING(Minus);
  SET_STRING(miter);
  SET_STRING(miterLimit);
  SET_STRING(mkdirs);
  SET_STRING(mousedown);
  SET_STRING(mouseenter);
  SET_STRING(mouseleave);
  SET_STRING(mousemove);
  SET_STRING(mouseup);
  SET_STRING(moveTo);
  SET_STRING(multiply);
  SET_STRING(n);
  SET_STRING(now);
  SET_STRING(NumLock);
  SET_STRING(Numpad0);
  SET_STRING(Numpad1);
  SET_STRING(Numpad2);
  SET_STRING(Numpad3);
  SET_STRING(Numpad4);
  SET_STRING(Numpad5);
  SET_STRING(Numpad6);
  SET_STRING(Numpad7);
  SET_STRING(Numpad8);
  SET_STRING(Numpad9);
  SET_STRING(NumpadAdd);
  SET_STRING(NumpadDecimal);
  SET_STRING(NumpadDivide);
  SET_STRING(NumpadEnter);
  SET_STRING(NumpadMultiply);
  SET_STRING(NumpadSubtract);
  SET_STRING(o);
  SET_STRING(offsetX);
  SET_STRING(offsetY);
  SET_STRING(open);
  SET_STRING(overlay);
  SET_STRING(overlayConsoleTextColor);
  SET_STRING(p);
  SET_STRING(PageDown);
  SET_STRING(PageUp);
  SET_STRING(parent);
  SET_STRING(Pause);
  SET_STRING(performance);
  SET_STRING(Period);
  SET_STRING(platform);
  SET_STRING(postMessage);
  SET_STRING(PrintScreen);
  SET_STRING(Process);
  SET_STRING(profileFrameTimes);
  SET_STRING(putImageData);
  SET_STRING(q);
  SET_STRING(quadraticCurveTo);
  SET_STRING(Quote);
  SET_STRING(r);
  SET_STRING(readArrayBuffer);
  SET_STRING(readImageBitmap);
  SET_STRING(readImageData);
  SET_STRING(readJSON);
  SET_STRING(readText);
  SET_STRING(rect);
  SET_STRING(remove);
  SET_STRING(removeEventListener);
  SET_STRING(removeTree);
  SET_STRING(rename);
  SET_STRING(repeat);
  SET_STRING(requestAnimationFrame);
  SET_STRING(requestAttention);
  SET_STRING(resetTransform);
  SET_STRING(resizable);
  SET_STRING(resize);
  SET_STRING(restore);
  SET_STRING(retinaScale);
  SET_STRING(right);
  SET_STRING(rotate);
  SET_STRING(round);
  SET_STRING(s);
  SET_STRING(saturation);
  SET_STRING(save);
  SET_STRING(scale);
  SET_STRING(screen);
  SET_STRING(ScrollLock);
  SET_STRING(Semicolon);
  SET_STRING(sender);
  SET_STRING(sep);
  SET_STRING(setClipboardText);
  SET_STRING(setLineDash);
  SET_STRING(setTimeout);
  SET_STRING(setTransform);
  SET_STRING(shadowBlur);
  SET_STRING(shadowColor);
  SET_STRING(shadowOffsetX);
  SET_STRING(shadowOffsetY);
  SET_STRING(Shift);
  SET_STRING(shiftKey);
  SET_STRING(ShiftLeft);
  SET_STRING(ShiftRight);
  SET_STRING(showOverlayConsole);
  SET_STRING(showOverlayConsoleOnErrors);
  SET_STRING(showOverlayStats);
  SET_STRING(size);
  SET_STRING(Slash);
  SET_STRING(Space);
  SET_STRING(spawn);
  SET_STRING(square);
  SET_STRING(start);
  SET_STRING(status);
  SET_STRING(stroke);
  SET_STRING(strokeRect);
  SET_STRING(strokeStyle);
  SET_STRING(strokeText);
  SET_STRING(t);
  SET_STRING(Tab);
  SET_STRING(textAlign);
  SET_STRING(textBaseline);
  SET_STRING(title);
  SET_STRING(tmp);
  SET_STRING(toBase64);
  SET_STRING(top);
  SET_STRING(totalJSHeapSize);
  SET_STRING(transform);
  SET_STRING(translate);
  SET_STRING(type);
  SET_STRING(u);
  SET_STRING(Unidentified);
  SET_STRING(usedJSHeapSize);
  SET_STRING(v);
  SET_STRING(version);
  SET_STRING(visible);
  SET_STRING(vsync);
  SET_STRING(w);
  SET_STRING(wheel);
  SET_STRING(width);
  SET_STRING(window);
  SET_STRING(write);
  SET_STRING(x);
  SET_STRING(y);
  SET_STRING(z);
  SET_STRING(__dirname);
  SET_STRING(__filename);

  SET_SPECIAL(_0_, "0");
  SET_SPECIAL(_1_, "1");
  SET_SPECIAL(_2_, "2");
  SET_SPECIAL(_3_, "3");
  SET_SPECIAL(_4_, "4");
  SET_SPECIAL(_5_, "5");
  SET_SPECIAL(_6_, "6");
  SET_SPECIAL(_7_, "7");
  SET_SPECIAL(_8_, "8");
  SET_SPECIAL(_9_, "9");
  SET_SPECIAL(_BACK_QUOTE_, "`");
  SET_SPECIAL(_BACK_SLASH_, "\\");
  SET_SPECIAL(_COMMA_, ",");
  SET_SPECIAL(_DOT_, ".");
  SET_SPECIAL(_EMPTY_, "");
  SET_SPECIAL(_EQUAL_, "=");
  SET_SPECIAL(_FORWARD_SLASH_, "/");
  SET_SPECIAL(_LEFT_BRACKET_, "[");
  SET_SPECIAL(_MINUS_, "-");
  SET_SPECIAL(_PLUS_, "+");
  SET_SPECIAL(_QUOTE_, "'");
  SET_SPECIAL(_RIGHT_BRACKET_, "]");
  SET_SPECIAL(_SEMICOLON_, ";");
  SET_SPECIAL(_SPACE_, " ");
  SET_SPECIAL(_STAR_, "*");
  SET_SPECIAL(_XOR_, "xor");
  SET_SPECIAL(colorBurn, "color-burn");
  SET_SPECIAL(colorDodge, "color-dodge");
  SET_SPECIAL(destinationAtop, "destination-atop");
  SET_SPECIAL(destinationIn, "destination-in");
  SET_SPECIAL(destinationOut, "destination-out");
  SET_SPECIAL(destinationOver, "destination-over");
  SET_SPECIAL(hardLight, "hard-light");
  SET_SPECIAL(softLight, "soft-light");
  SET_SPECIAL(sourceAtop, "source-atop");
  SET_SPECIAL(sourceIn, "source-in");
  SET_SPECIAL(sourceOut, "source-out");
  SET_SPECIAL(sourceOver, "source-over");
}

JsStrings::~JsStrings() {}
