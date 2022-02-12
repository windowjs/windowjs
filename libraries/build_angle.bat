@echo off &setlocal


if not exist libraries\build_angle.bat (
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


if not exist libraries\angle\ (
  echo.
  echo Run libraries\sync.bat first to check out the dependencies.
  echo.
  exit /b 1
)


echo Building ANGLE in libraries/angle/out/Release
echo.

cd libraries/angle
mkdir out\Release
copy ..\angle-args-windows.gn out\Release\args.gn /Y

gn.exe gen out\Release

if %errorlevel% neq 0 (
    echo.
    echo GN failed!
    echo.
    exit /b 1
)

ninja.exe -C out\Release libEGL_static

if %errorlevel% neq 0 (
    echo.
    echo Ninja failed!
    echo.
    exit /b 1
)

echo.
echo ANGLE build done.
exit /b 0
