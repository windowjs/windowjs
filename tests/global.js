// Tests for the global functions: https://windowjs.org/doc/global

import {assert, assertEquals} from './lib/lib.js';

export async function devicePixelRatioIsANumber() {
  assert(typeof(devicePixelRatio) == 'number');
}

export async function requestAnimationFrameGetsScheduled() {
  return new Promise((resolve) => requestAnimationFrame(resolve));
}

export async function cancelAnimationFrameCancels() {
  return new Promise((resolve, reject) => {
    const id = requestAnimationFrame(reject);
    requestAnimationFrame(resolve);
    cancelAnimationFrame(id);
  });
}

export async function setTimeoutGetsScheduled() {
  return new Promise((resolve) => setTimeout(resolve));
}

export async function clearTimeoutCancels() {
  return new Promise((resolve, reject) => {
    const id = setTimeout(reject, 1);
    setTimeout(resolve, 100);
    clearTimeout(id);
  });
}
