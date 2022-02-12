@echo off


call :GETPARENT PARENT
if /I "%PARENT%" == "powershell" goto :POWERSHELL
if /I "%PARENT%" == "pwsh" goto :POWERSHELL
set DEPOT_TOOLS_WIN_TOOLCHAIN=0


echo.
echo.
echo Fetching dependencies (this may take a long time, the first time)
echo.
echo.
call gclient sync --shallow --no-history -D -R --force

if %errorlevel% neq 0 (
  echo.
  echo FAILED
  exit /b 1
)

echo.
echo FINISHED -- ready to build
exit /b 0


:POWERSHELL
echo.
echo Invoke libraries\sync.ps1 instead for Powershell.
echo.
echo FAILED
exit /b 1


:GETPARENT
set "PSCMD=$ppid=$pid;while($i++ -lt 3 -and ($ppid=(Get-CimInstance Win32_Process -Filter ('ProcessID='+$ppid)).ParentProcessId)) {}; (Get-Process -EA Ignore -ID $ppid).Name"

for /f "tokens=*" %%i in ('powershell -noprofile -command "%PSCMD%"') do SET %1=%%i
