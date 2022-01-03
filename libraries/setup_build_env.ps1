if (-not(Test-Path -Path 'libraries\setup_build_env.ps1' -PathType Leaf)) {
  Write-Host "Invoke this script from the root repository of the checkout."
  Write-Host "FAILED"
  exit 1
}


if ($(pwd) -like "* *") {
  Write-Host "This script doesn't work with paths that contain spaces."
  Write-Host "FAILED"
  exit 1
}


Write-Host "Setting up repository at $(pwd)"


if (-not($env:windowjs_visual_studio_ready -eq "1")) {
  Write-Host ""
  Write-Host "Setting up Visual Studio build tools environment"
  Write-Host ""

  try {
    $vswhere = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
    $vcvarspath = &$vswhere -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath

    cmd.exe /c "call `"$vcvarspath\VC\Auxiliary\Build\vcvars64.bat`" && set > %temp%\vcvars.txt"

    Get-Content "$env:temp\vcvars.txt" | Foreach-Object {
      if ($_ -match "^(.*?)=(.*)$") {
        Set-Content "env:\$($matches[1])" $matches[2]
      }
    }
  } catch {
    Write-Host ""
    Write-Host "Failed to locate Visual Studio -- is it installed?"
    Write-Host ""
    Write-Host "FAILED"
    exit 1
  }

  $env:windowjs_visual_studio_ready = "1"
}


try {
  Write-Host ""
  Write-Host "Checking CMake version"
  Write-Host ""
  cmake --version
} catch {
  Write-Host ""
  Write-Host "Failed to check cmake -- is it installed?"
  Write-Host ""
  Write-Host "FAILED"
  exit 1
}

if (-not(Test-Path -Path 'libraries\depot_tools' -PathType Container)) {
  Write-Host ""
  Write-Host "Checking out the Chrome depot_tools at libraries\depot_tools"
  Write-Host ""
  git clone https://chromium.googlesource.com/chromium/tools/depot_tools.git libraries/depot_tools
}


$depot_tools = "$(pwd)/libraries/depot_tools"

$env:DEPOT_TOOLS_WIN_TOOLCHAIN = "0"

Write-Host ""
Write-Host "Verifying depot_tools gclient version (this may download additional tools)"
Write-Host ""
Invoke-Expression "${depot_tools}/gclient.bat --version"


if (-not(Test-Path -Path 'libraries\gn' -PathType Container)) {
  Write-Host ""
  Write-Host "Checking out the gn build tool"
  Write-Host ""
  git clone https://gn.googlesource.com/gn libraries\gn
}


if (-not(Test-Path -Path 'libraries\gn\out\gn.exe' -PathType Leaf)) {
  Write-Host ""
  Write-Host "Building the gn build tool"
  Write-Host ""
  pushd libraries\gn
  try {
    Invoke-Expression "${depot_tools}/python.bat build/gen.py"
    Invoke-Expression "${depot_tools}/ninja.exe -C out gn.exe"
    popd
  } catch {
    Write-Host ""
    Write-Host "GN build failed."
    Write-Host ""
    Write-Host "FAILED"
    exit 1
  }
}

Write-Host ""
Write-Host "Updating PATH to use depot_tools and gn"
$env:path = Invoke-Expression "${depot_tools}/python.bat libraries\update_path.py $(pwd)"

Write-Host ""
Write-Host "Verifying gn and ninja in PATH"
Write-Host ""
gn --version
ninja --version

Write-Host ""
Write-Host "FINISHED -- ready to fetch dependencies with libraries\sync.ps1"
