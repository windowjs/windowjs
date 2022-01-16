---
layout: documentation
title: Window.js | File
class-name: File
class-properties:
  - cwd
  - home
  - sep
  - tmp
class-methods:
  - basename
  - copy
  - copyTree
  - dirname
  - isDir
  - isFile
  - list
  - listTree
  - mkdirs
  - readArrayBuffer
  - readImageBitmap
  - readImageData
  - readJSON
  - readText
  - remove
  - removeTree
  - rename
  - size
  - write
---

File
====

The `File` API gives access to the local filesystem.

Most of the `File` methods return `Promises` that resolve with the data
requested, or throw on I/O failures.


{% include property class="File" name="cwd" type="string" %}

The current working directory.


{% include property class="File" name="home" type="string" %}

The user home directory.


{% include property class="File" name="sep" type="string" %}

The preferred path separator for the current platform. This is usually the
forward slash character `/`, but is a backslash `\` on Windows.


{% include property class="File" name="tmp" type="string" %}

The system directory for temporary files.


{% include method class="File" name="basename" type="(string) => string" %}

Returns the name of the file at the end of the given string path.


{% include method class="File" name="copy"
   type="(string, string) => Promise<void>"
%}

Copies a file from the path in the first parameter to the path in the second
parameter. See [copyTree](#File.copyTree) for a similar method that can copy
entire directories, recursively.


{% include method class="File" name="copyTree"
   type="(string, string) => Promise<void>"
%}

Copies the file or directory in the first parameter to the path in the second
parameter. This method will copy directories and their contents recursively.


{% include method class="File" name="dirname" type="(string) => string" %}

Returns the path to the directory that contains the file path given in the
first parameter.


{% include method class="File" name="isDir"
   type="(string) => Promise<boolean>"
%}

Returns `true` if the given path is a directory. Returns false if the path
doesn't exist or isn't accessible to the current process.


{% include method class="File" name="isFile"
   type="(string) => Promise<boolean>"
%}

Returns `true` if the given path is a file. Returns false if the path
doesn't exist or isn't accessible to the current process.


{% include method class="File" name="list"
   type="(string) => Promise<string[]>"
%}

Returns a list of all the files in the given directory. See
[listTree](#File.listTree) for a similar method that also returns the contents
of subdirectories.


{% include method class="File" name="listTree"
   type="(string) => Promise<string[]>"
%}

Returns a list of all the files in the given directory, recursively.


{% include method class="File" name="mkdirs"
   type="(string) => Promise<void>"
%}

Creates all the missing directories in the given path.


{% include method class="File" name="readArrayBuffer"
   type="(string) => Promise<ArrayBuffer>"
%}

Returns the contents of the given file as an `ArrayBuffer`.


{% include method class="File" name="readImageBitmap"
   type="(string) => Promise<ImageBitmap>"
%}

Returns the contents of the given file as an [ImageBitmap](/doc/imagebitmap).


{% include method class="File" name="readImageData"
   type="(string) => Promise<ImageData>"
%}

Returns the contents of the given file as an [ImageData](/doc/imagedata).


{% include method class="File" name="readJSON"
   type="(string) => Promise<Json>"
%}

Returns the contents of the given file as a `JSON` object.


{% include method class="File" name="readText"
   type="(string) => Promise<sTring>"
%}

Returns the contents of the given file as a string.


{% include method class="File" name="remove"
   type="(string) => Promise<void>"
%}

Removes the given file. See [removeTree](#File.removeTree) for a similar method
that can remove a directory and all of its contents, recursively.


{% include method class="File" name="removeTree"
   type="(string) => Promise<void>"
%}

Removes the given file or directory, and all of its contents, recursively.


{% include method class="File" name="rename"
   type="(string, string) => Promise<void>"
%}

Renames the file at the path in the first parameter to the path in the
second parameter.


{% include method class="File" name="size"
   type="(string) => Promise<number>"
%}

Returns the size, in bytes, of the file at the given path.


{% include method class="File" name="write"
   type="(string, string | ArrayBuffer | TypedArray) => Promise<void>"
%}

Writes the contents of the object in the second parameter to the file path in
the first parameter.

Valid object types for the second parameter:

{: .strings}
| `ArrayBuffer` | Writes all of the bytes in the buffer.                       |
| `TypedArray`  | Writes all of the bytes in the array.                        |
| `string`      | Writes the string encoded in UTF-8.                          |
