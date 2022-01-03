@echo off &setlocal


if not exist libraries\build_skia.bat (
  echo Invoke this script from the root directory of the checkout.
  echo FAILED
  exit /b 1
)


if not exist libraries\depot_tools\ (
  echo.
  echo Run libraries\setup_build_env.bat first to set up the build environment.
  echo.
  exit /b 1
)


if not exist libraries\skia\ (
  echo.
  echo Run libraries\sync.bat first to check out the dependencies.
  echo.
  exit /b 1
)


echo Building Skia in libraries/skia/out/Release
echo.

cd libraries/skia
mkdir out\Release
copy ..\skia-args-windows.gn out\Release\args.gn /Y

gn.exe gen out\Release

if %errorlevel% neq 0 (
    echo.
    echo GN failed!
    echo.
    exit /b 1
)

ninja.exe -C out\Release skia

if %errorlevel% neq 0 (
    echo.
    echo Ninja failed!
    echo.
    exit /b 1
)

echo.
echo Skia build done.
exit /b 0
