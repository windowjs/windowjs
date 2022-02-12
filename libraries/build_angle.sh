#!/bin/sh

echo "Building ANGLE in libraries/angle/out/Release"

cd libraries/angle

if [ $? -ne 0 ]; then
  echo "Check out the Window.js dependencies first!"
  exit 1
fi

mkdir -p out/Release

if [ "`uname`" = "Darwin" ]; then
  cp ../angle-args-mac.gn out/Release/args.gn
else
  cp ../angle-args-linux.gn out/Release/args.gn
fi

gn gen out/Release

if [ $? -ne 0 ]; then
  echo "GN failed!"
  exit 1
fi

ninja -C out/Release libEGL_static

if [ $? -ne 0 ]; then
  echo "Ninja failed!"
  exit 1
fi

echo
echo "ANGLE build done."
