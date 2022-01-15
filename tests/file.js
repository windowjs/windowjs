// Tests for the File API: https://windowjs.org/doc/file

import { assert, assertEquals, getTmpDir } from './lib.js';

export async function cwd() {
  assert(await File.isDir(File.cwd));
  assertEquals(File.cwd, File.dirname(__dirname));
}

export async function home() {
  assert(await File.isDir(File.home));
}

export async function basename() {
  assertEquals('file.js', File.basename(__filename));
}

export async function copy() {
  const dir = await getTmpDir();
  const path = dir + '/copy.txt';
  assert(!await File.isFile(path));
  await File.copy(__filename, path);
  assert(await File.isFile(path));
  const original = await File.readText(__filename);
  const copy = await File.readText(path);
  assertEquals(copy, original);
}

export async function copyTree() {
  const dir = await getTmpDir();
  const path1 = dir + '/data/copyTree/file.txt';
  const path2 = dir + '/data/copyTree/subdir/file.txt';
  assert(!await File.isFile(path1));
  assert(!await File.isFile(path2));
  await File.copyTree(__dirname, dir);
  assert(await File.isFile(path1));
  assert(await File.isFile(path2));
  assertEquals('Test file.\n', await File.readText(path1));
  assertEquals('Another test file.\n', await File.readText(path2));
}

export async function dirname() {
  assertEquals(__dirname, File.dirname(__filename));
}

export async function isDir() {
  assert(await File.isDir(__dirname));
  assert(! await File.isDir(__filename));
  assert(! await File.isDir('no such thing'));
}

export async function isFile() {
  assert(await File.isFile(__filename));
  assert(! await File.isFile(__dirname));
  assert(! await File.isFile('no such thing'));
}

export async function list() {
  const files = await File.list(__dirname + '/data/copyTree');
  assert(files.length == 2);
  assert(files.includes('subdir'));
  assert(files.includes('file.txt'));
}

export async function listTree() {
  const files = await File.listTree(__dirname + '/data/copyTree');
  assert(files.length == 3);
  assert(files.includes('subdir'));
  assert(files.includes('file.txt'));
  assert(files.includes('subdir' + File.sep + 'file.txt'));
}

export async function mkdirs() {
  const dir = await getTmpDir();
  const path = dir + '/foo/bar/baz';
  assert(!await File.isDir(path));
  await File.mkdirs(path);
  assert(await File.isDir(path));
}

export async function readArrayBuffer() {
  const buffer = await File.readArrayBuffer(__dirname + '/data/binary.bin');
  assert(buffer instanceof ArrayBuffer);
  assert(buffer.byteLength == 6);
  const view = new Uint8Array(buffer);
  assert(view[0] == 1);
  assert(view[1] == 2);
  assert(view[2] == 3);
  assert(view[3] == 0xfd);
  assert(view[4] == 0xfe);
  assert(view[5] == 0xff);
}

export async function readImageBitmap() {
  const image = await File.readImageBitmap(__dirname + '/data/image.png');
  assert(image instanceof ImageBitmap);
  assert(image.width == 200);
  assert(image.height == 152);
}

export async function readImageData() {
  const image = await File.readImageData(__dirname + '/data/image.png');
  assert(image instanceof ImageData);
  assert(image.width == 200);
  assert(image.height == 152);
}

export async function readJSON() {
  const json = await File.readJSON(__dirname + '/data/object.json');
  assert(json instanceof Object);
  assert(json['string'] === 'hello world');
  assert(json['number'] === 123.456);
  assert(json['true'] === true);
  assert(json['false'] === false);
  assert(json['null'] === null);
  assert(json['array'] instanceof Array);
  assert(json['array'].length == 5);
  assert(json['array'][0] === 'hello world');
  assert(json['array'][1] === 123.456);
  assert(json['array'][2] === true);
  assert(json['array'][3] === false);
  assert(json['array'][4] === null);
  assert(json['object'] instanceof Object);
}

export async function readText() {
  const text = await File.readText(__dirname + '/data/copyTree/file.txt');
  assert(text === 'Test file.\n');
}

export async function remove() {
  const dir = await getTmpDir();
  const path = dir + '/copy.txt';
  assert(!await File.isFile(path));
  await File.copy(__filename, path);
  assert(await File.isFile(path));
  await File.remove(path);
  assert(!await File.isFile(path));
}

export async function removeTree() {
  const dir = await getTmpDir();
  assert(await File.isDir(dir));
  const path1 = dir + '/data/copyTree/file.txt';
  const path2 = dir + '/data/copyTree/subdir/file.txt';
  assert(!await File.isFile(path1));
  assert(!await File.isFile(path2));
  await File.copyTree(__dirname, dir);
  assert(await File.isFile(path1));
  assert(await File.isFile(path2));
  await File.removeTree(dir);
  assert(!await File.isDir(dir));
  assert(!await File.isFile(path1));
  assert(!await File.isFile(path2));
}

export async function rename() {
  const dir = await getTmpDir();
  const path1 = dir + '/copy.txt';
  const path2 = dir + '/renamed.txt';
  assert(!await File.isFile(path1));
  assert(!await File.isFile(path2));
  await File.copy(__filename, path1);
  assert(await File.isFile(path1));
  assert(!await File.isFile(path2));
  await File.rename(path1, path2);
  assert(!await File.isFile(path1));
  assert(await File.isFile(path2));
}

export async function size() {
  assert(await File.size(__dirname + '/data/binary.bin') == 6);
}

export async function writeString() {
  const dir = await getTmpDir();
  const path = dir + '/out.txt';
  assert(!await File.isFile(path));
  await File.write(path, 'Hello world!');
  assert(await File.isFile(path));
  const content = await File.readText(path);
  assertEquals(content, 'Hello world!');
}

export async function writeBuffer() {
  const data = new Uint8Array(10);
  data[0] = 123;
  data[5] = 0x80;
  data[9] = 0xff;
  const dir = await getTmpDir();
  const path = dir + '/out.bin';
  assert(!await File.isFile(path));
  await File.write(path, data);
  assert(await File.isFile(path));
  const buffer = await File.readArrayBuffer(path);
  assert(buffer.byteLength == 10);
  const view = new Uint8Array(buffer);
  assert(view[0] == 123);
  assert(view[1] == 0);
  assert(view[2] == 0);
  assert(view[3] == 0);
  assert(view[4] == 0);
  assert(view[5] == 0x80);
  assert(view[6] == 0);
  assert(view[7] == 0);
  assert(view[8] == 0);
  assert(view[9] == 0xff);
}
