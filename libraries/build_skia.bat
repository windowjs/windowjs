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


for /f "tokens=* usebackq" %%f in (`where clang++.exe`) do (
  set "CLANG_PATH=%%f" & goto :CLANG_PATH_OUT
)
:CLANG_PATH_OUT

if "%CLANG_PATH%" == "" (
  echo.
  echo Couldn't find the LLVM installation. Is clang++ in the PATH?
  echo.
  exit /b 1
)

call :dirname "%CLANG_PATH%" LLVM_DIR
call :dirname "%LLVM_DIR%" LLVM_DIR


echo Building Skia in libraries/skia/out/Release with LLVM in %LLVM_DIR%
echo.

cd libraries/skia
mkdir out\Release

(for /f "delims=" %%i in (..\skia-args-windows.gn) do (
    set "line=%%i"
    setlocal enabledelayedexpansion
    set "line=!line:edit_llvm_path_here=%LLVM_DIR%!"
    echo(!line!
    endlocal
))>"out\Release\args.gn"

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


:dirname file varName
    setlocal ENABLEEXTENSIONS ENABLEDELAYEDEXPANSION
    set _dir=%~dp1
    set _dir=%_dir:~0,-1%
    endlocal & set %2=%_dir%
goto :EOF
