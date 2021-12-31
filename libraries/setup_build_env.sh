#!/bin/sh


if [ ! -f "libraries/setup_build_env.sh" ]; then
  echo "Invoke this script from the root directory of the checkout."
  echo "FAILED"
  return 1
fi


echo "Setting up repository at $PWD"


echo
echo Checking CMake version
echo
cmake --version

if [ $? -ne 0 ]; then
  echo
  echo "Failed to check cmake -- is it installed?"
  echo
  echo "FAILED"
  return 1
fi


echo
echo "Checking Clang version"
echo
clang++ --version

if [ $? -ne 0 ]; then
  echo
  echo "Failed to check clang -- is it installed?"
  echo
  echo FAILED
  return 1
fi


if [ ! -d "libraries/depot_tools" ]; then
  echo
  echo "Checking out the Chrome depot_tools at libraries/depot_tools"
  echo
  git clone https://chromium.googlesource.com/chromium/tools/depot_tools.git libraries/depot_tools
  if [ $? -ne 0 ]; then
    echo
    echo FAILED
    echo
    return 1
  fi
fi

depot_tools="$PWD/libraries/depot_tools"


echo
echo "Verifying depot_tools gclient version (this may download additional tools)"
echo
${depot_tools}/gclient --version


if [ ! -d "libraries/gn" ]; then
  echo
  echo "Checking out the gn build tool"
  echo
  git clone https://gn.googlesource.com/gn libraries/gn
  if [ $? -ne 0 ]; then
    echo
    echo FAILED
    echo
    return 1
  fi
fi

if [ ! -f "libraries/gn/out/gn" ]; then
  echo
  echo "Building the gn build tool"
  echo
  pushd libraries/gn
  ${depot_tools}/vpython build/gen.py
  ${depot_tools}/ninja -C out gn
  popd
fi

if [ ! -f "libraries/gn/out/gn" ]; then
  echo
  echo "GN build failed."
  echo FAILED
  return 1
fi

echo
echo "Updating PATH to use depot_tools and gn"
echo

export PATH=`${depot_tools}/vpython libraries/update_path.py "$PWD"`
