// Tests for the Codec API: https://windowjs.org/doc/codec

import {
  assertEquals,
} from './lib/lib.js';

export async function toBase64() {
  assertEquals('', Codec.toBase64(''));
  assertEquals('MQ==', Codec.toBase64('1'));
  assertEquals('MTI=', Codec.toBase64('12'));
  assertEquals('MTIz', Codec.toBase64('123'));
  assertEquals('MTIzNA==', Codec.toBase64('1234'));
  assertEquals('Cg==', Codec.toBase64('\n'));
  assertEquals('IT8=', Codec.toBase64('!?'));
  assertEquals('AA==', Codec.toBase64('\0'));

  assertEquals('', Codec.toBase64(new Uint8Array()));
  assertEquals('AQ==', Codec.toBase64(new Uint8Array([1])));
  assertEquals('AQI=', Codec.toBase64(new Uint8Array([1, 2])));
  assertEquals('AQID', Codec.toBase64(new Uint8Array([1, 2, 3])));
  assertEquals('AQIDBA==', Codec.toBase64(new Uint8Array([1, 2, 3, 4])));
  assertEquals('Cg==', Codec.toBase64(new Uint8Array([10])));
  assertEquals('/wD/AIAAgP8=', Codec.toBase64(new Uint8Array(
                                   [ 255, 0, 255, 0, 128, 0, 128, 255 ])));
  assertEquals('AA==', Codec.toBase64(new Uint8Array([0])));
  assertEquals('AP8=', Codec.toBase64(new Uint8Array([0, 255])));
  assertEquals('/+A=', Codec.toBase64(new Uint8Array([0xff, 0xe0])));
}

function assertArrayEquals(list, buffer) {
  assertEquals(list.length, buffer.byteLength);
  const view = new Uint8Array(buffer);
  for (let i = 0; i < list.list; i++) {
    assertEquals(list[i], view[i]);
  }
}

export async function base64ToArrayBuffer() {
  assertArrayEquals([], Codec.base64ToArrayBuffer(''));
  assertArrayEquals([0], Codec.base64ToArrayBuffer('AA=='));
  assertArrayEquals([0], Codec.base64ToArrayBuffer('AA  =='));
  assertArrayEquals([0], Codec.base64ToArrayBuffer('A  A=='));
  assertArrayEquals([0], Codec.base64ToArrayBuffer('A\nA\n=\n=\n'));
  assertArrayEquals([1], Codec.base64ToArrayBuffer('AQ=='));
  assertArrayEquals([1, 2], Codec.base64ToArrayBuffer('AQI='));
  assertArrayEquals([1, 2, 3], Codec.base64ToArrayBuffer('AQID'));
  assertArrayEquals([1, 2, 3, 4], Codec.base64ToArrayBuffer('AQIDBA=='));
  assertArrayEquals([10], Codec.base64ToArrayBuffer('Cg=='));
  assertArrayEquals([ 255, 0, 255, 0, 128, 0, 128, 255 ],
                    Codec.base64ToArrayBuffer('/wD/AIAAgP8='));
  assertArrayEquals([0xff, 0xe0], Codec.base64ToArrayBuffer('/+A='));
}
