#!/bin/sh

echo "Building v8 in libraries/v8/out/Release"

cd libraries/v8

if [ $? -ne 0 ]; then
  echo "Check out the Window.js dependencies first!"
  exit 1
fi

mkdir -p out/Release

if [ "`uname`" = "Darwin" ]; then
  cp ../v8-args-mac.gn out/Release/args.gn
else
  cp ../v8-args-linux.gn out/Release/args.gn
fi

gn gen out/Release

if [ $? -ne 0 ]; then
  echo "GN failed!"
  exit 1
fi

ninja -C out/Release v8_monolith

if [ $? -ne 0 ]; then
  echo "Ninja failed!"
  exit 1
fi

echo "v8 build done."
