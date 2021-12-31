#!/bin/sh

echo
echo
echo "Fetching dependencies (this may take a long time, the first time)"
echo
echo
gclient sync --shallow --no-history -D -R

if [ $? -ne 0 ]; then
  echo.
  echo FAILED
  exit 1
fi

echo
echo "FINISHED -- ready to build"
