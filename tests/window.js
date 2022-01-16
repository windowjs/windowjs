// Tests for the window API: https://windowjs.org/doc/window

import {assert, assertEquals, getTmpDir, resolveOnNextEvent} from './lib.js';

export async function canvasSizeMatchesWindowSize() {
  window.width = 246;
  window.height = 357;
  assertEquals(window.canvas.width, 246);
  assertEquals(window.canvas.height, 357);
}

export async function maximize() {
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
