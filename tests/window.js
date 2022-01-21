// Tests for the window API: https://windowjs.org/doc/window

import {
  assert,
  assertEquals,
  getTmpDir,
  resolveOnNextEvent,
} from './lib/lib.js';

export async function canvasSizeMatchesWindowSize() {
  if (window.platform == 'Linux') {
    // TODO: the width and height aren't updating immediately on Linux.
    // Figure out why and enable the test.
    return;
  }
  window.width = 246;
  window.height = 356;
  assertEquals(window.canvas.width, 246);
  assertEquals(window.canvas.height, 356);
}

export async function maximize() {
  if (window.platform == 'Linux' || window.platform == 'macOS') {
    return;
  }
  assert(!window.maximized);
  const maximizeNotification = resolveOnNextEvent('maximize');
  window.maximize();
  await maximizeNotification;
  assert(window.maximized);
  const restoreNotification = resolveOnNextEvent('restore');
  window.restore();
  await restoreNotification;
  assert(!window.maximized);
}

export async function minimize() {
  if (window.platform == 'Linux' || window.platform == 'macOS') {
    return;
  }
  assert(!window.minimized);
  const minimizeNotification = resolveOnNextEvent('minimize');
  window.minimize();
  await minimizeNotification;
  assert(window.minimized);
  const restoreNotification = resolveOnNextEvent('restore');
  window.restore();
  await restoreNotification;
  assert(!window.minimized);
}
