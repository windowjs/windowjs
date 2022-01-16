// Common utilities for tests, to make assertions and communicate with the
// parent process in run_tests.js.

function checkNotPromise(x) {
  if (x instanceof Promise) {
    throw new Error('Asserting on a Promise; is this missing an await?');
  }
}

let tmpDirForTests = null;

export function setTmpDirForTests(tmp) {
  tmpDirForTests = tmp;
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

export async function diffCanvasToFile(path) {
  const golden = await File.readImageData(File.dirname(__dirname) + '/' + path);
  const canvas = window.canvas;
  assertEquals(golden.width, canvas.width);
  assertEquals(golden.height, canvas.height);
  const pixels = canvas.getImageData();
  const a = golden.data;
  const b = pixels.data;
  assertEquals(a.length, b.length);
  const length = a.length;
  let diffs = 0;
  for (let i = 0; i < length; i++) {
    if (a[i] != b[i]) {
      diffs++;
    }
  }
  assertEquals(diffs, 0);
}
