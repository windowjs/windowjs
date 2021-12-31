@echo off


if not exist libraries\setup_build_env.bat (
  echo Invoke this script from the root directory of the checkout.
  echo FAILED
  exit /b 1
)


echo Setting up repository at %cd%


call :GETPARENT PARENT
if /I "%PARENT%" == "powershell" goto :POWERSHELL
if /I "%PARENT%" == "pwsh" goto :POWERSHELL


if "%windowjs_visual_studio_ready%" == "1" goto :VISUALSTUDIOOK

echo.
echo Setting up Visual Studio build tools environment
echo.

set vswhere="%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"

for /F "tokens=* USEBACKQ" %%F IN (`%vswhere% -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) DO (
  set vcvarspath=%%F
)

call "%vcvarspath%\VC\Auxiliary\Build\vcvars64.bat"

if %errorlevel% neq 0 (
  echo.
  echo Failed to locate Visual Studio -- is it installed?
  echo.
  echo FAILED
  exit /b 1
)

set windowjs_visual_studio_ready=1

:VISUALSTUDIOOK


echo.
echo Checking CMake version
echo.
cmake --version

if %errorlevel% neq 0 (
  echo.
  echo Failed to check cmake -- is it installed?
  echo.
  echo FAILED
  exit /b 1
)


echo.
echo Checking Clang version
echo.
clang-cl --version

if %errorlevel% neq 0 (
  echo.
  echo Failed to check clang -- is it installed?
  echo.
  echo FAILED
  exit /b 1
)


if not exist libraries\depot_tools\ (
  echo.
  echo Checking out the Chrome depot_tools at libraries\depot_tools
  echo.
  git clone https://chromium.googlesource.com/chromium/tools/depot_tools.git libraries/depot_tools
)

if %errorlevel% neq 0 (
  echo.
  echo FAILED
  exit /b 1
)

set depot_tools=%cd%/libraries/depot_tools

set DEPOT_TOOLS_WIN_TOOLCHAIN=0


echo.
echo Verifying depot_tools gclient version (this may download additional tools)
echo.
call %depot_tools%/gclient --version


if not exist libraries\gn\ (
  echo.
  echo Checking out the gn build tool
  echo.
  git clone https://gn.googlesource.com/gn libraries\gn
)


if not exist libraries\gn\out\gn.exe (
  echo.
  echo Building the gn build tool
  echo.
  pushd libraries\gn
  call %depot_tools%/python build/gen.py
  call %depot_tools%/ninja -C out gn.exe
  popd
)

if not exist libraries\gn\out\gn.exe (
  echo.
  echo GN build failed.
  echo FAILED
  exit /b 1
)


echo.
echo Updating PATH to use depot_tools and gn

for /f "tokens=* USEBACKQ" %%f in (`%depot_tools%/python libraries\win_update_path.py %cd%`) do (
  set PATH=%%f
)


echo.
echo Verifying gn and ninja in PATH
echo.
gn --version
ninja --version

echo.
echo FINISHED -- ready to fetch dependencies with libraries\sync.bat
exit /b 0


:POWERSHELL
echo.
echo Invoke libraries\setup_build_env.ps1 instead for Powershell.
echo.
echo FAILED
exit /b 1


:GETPARENT
set "PSCMD=$ppid=$pid;while($i++ -lt 3 -and ($ppid=(Get-CimInstance Win32_Process -Filter ('ProcessID='+$ppid)).ParentProcessId)) {}; (Get-Process -EA Ignore -ID $ppid).Name"

for /f "tokens=*" %%i in ('powershell -noprofile -command "%PSCMD%"') do SET %1=%%i
