// Common utilities for tests, to make assertions and communicate with the
// parent process in run_tests.js.

function checkNotPromise(x) {
  if (x instanceof Promise) {
    throw new Error('Asserting on a Promise; is this missing an await?');
  }
}

let tmpDirForTests = null;
let diffCallback = null;

export function setTmpDirForTests(tmp) {
  tmpDirForTests = tmp;
}

export function setDiffCallback(callback) {
  diffCallback = callback;
}

export async function getTmpDir() {
  if (!tmpDirForTests) {
    throw new Error('Internal failure: tmpDirForTests not configured');
  }
  await File.mkdirs(tmpDirForTests);
  return tmpDirForTests;
}

export function assert(cond) {
  checkNotPromise(cond);
  if (!cond) {
    throw new Error('Assertion failed');
  }
}

export function assertEquals(a, b) {
  checkNotPromise(a);
  checkNotPromise(b);
  if (a != b) {
    throw new Error('Assertion failed: ' + a + ' != ' + b);
  }
}

export function resolveOnNextEvent(eventType) {
  return new Promise(function(resolve) {
    const listener = function(event) {
      window.removeEventListener(eventType, listener);
      resolve(event);
    };
    window.addEventListener(eventType, listener);
  });
}

export async function diffCanvasToFile(path, tolerance) {
  if (diffCallback) {
    // Running in a browser e.g. in canvas.html; skip this.
    diffCallback(path);
    return;
  }

  const goldenPath = File.dirname(__dirname) + '/' + path;
  const golden = await File.readImageData(goldenPath);
  const canvas = window.canvas;
  assertEquals(golden.width, canvas.width);
  assertEquals(golden.height, canvas.height);
  const pixels = canvas.getImageData();
  const a = new Uint32Array(golden.data.buffer);
  const b = new Uint32Array(pixels.data.buffer);
  assertEquals(a.length, b.length);
  assertEquals(golden.data.byteLength, golden.width * golden.height * 4);
  assertEquals(a.length, golden.width * golden.height);

  const length = a.length;
  let diffs = 0;
  for (let i = 0; i < length; i++) {
    if (a[i] != b[i]) {
      diffs++;
    }
  }

  // Pixel diffs should really run without antialias.
  // Unfortunately, antialiasing can't be turned off for the HTML5 <canvas>
  // element for 2D contents (only for WebGL).
  const outPath = goldenPath + '__test_output.png';
  if (diffs > 0) {
    await File.write(outPath, await canvas.encode('png'));
  } else {
    await File.remove(outPath);
  }

  if (diffs > (tolerance || 0)) {
    throw new Error(diffs + ' pixels changed.\n\n       Expected: ' +
                    goldenPath + '\n       Actual:   ' + outPath + '\n');
  }
}
