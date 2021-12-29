@echo off &setlocal

if "%1" == "" (
    echo Usage: build_skia.bat PATH_TO_LLVM_DIR
    exit /b 1
)

echo Building Skia in libraries/skia/out/Release

cd libraries/skia

if %errorlevel% neq 0 (
    echo Check out the Window.js dependencies first!
    exit /b 1
)

mkdir out\Release

(for /f "delims=" %%i in (..\skia-args-windows.gn) do (
    set "line=%%i"
    setlocal enabledelayedexpansion
    set "line=!line:edit_llvm_path_here=%1!"
    echo(!line!
    endlocal
))>"out\Release\args.gn"

gn.exe gen out\Release

if %errorlevel% neq 0 (
    echo GN failed!
    exit /b 1
)

ninja.exe -C out\Release skia

if %errorlevel% neq 0 (
    echo Ninja failed!
    exit /b 1
)

echo Skia build done.
