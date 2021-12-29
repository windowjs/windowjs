@echo off

set FOO=NOTPOWER
set DEPOT_TOOLS_WIN_TOOLCHAIN=0

set vswhere="%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"

for /F "tokens=* USEBACKQ" %%F IN (`%vswhere% -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) DO (
SET vcvarspath=%%F
)

"%vcvarspath%\VC\Auxiliary\Build\vcvars64.bat"
