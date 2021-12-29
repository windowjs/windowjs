#!/bin/sh

echo "Building Skia in libraries/skia/out/Release"

cd libraries/skia

if [ $? -ne 0 ]; then
  echo "Check out the Window.js dependencies first!"
  exit 1
fi

mkdir -p out/Release

if [ "`uname`" = "Darwin" ]; then
  cp ../skia-args-mac.gn out/Release/args.gn
else
  cp ../skia-args-linux.gn out/Release/args.gn
fi

gn gen out/Release

if [ $? -ne 0 ]; then
  echo "GN failed!"
  exit 1
fi

ninja -C out/Release skia

if [ $? -ne 0 ]; then
  echo "Ninja failed!"
  exit 1
fi

echo "Skia build done."
