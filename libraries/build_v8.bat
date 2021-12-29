@echo off &setlocal

if "%1" == "" (
    echo Usage: build_v8.bat PATH_TO_LLVM_DIR
    exit /b 1
)

echo Building v8 in libraries/v8/out/Release

cd libraries/v8

if %errorlevel% neq 0 (
    echo Check out the Window.js dependencies first!
    exit /b 1
)

mkdir out\Release

(for /f "delims=" %%i in (..\v8-args-windows.gn) do (
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

ninja.exe -C out\Release v8_monolith

if %errorlevel% neq 0 (
    echo Ninja failed!
    exit /b 1
)

echo v8 build done.
