Write-Host ""
Write-Host ""
Write-Host "Fetching dependencies (this may take a long time, the first time)"
Write-Host ""
Write-Host ""

try {
  Invoke-Expression "gclient sync --shallow --no-history -D -R --force"
} catch {
  Write-Host ""
  Write-Host "FAILED (are the depot_tools in PATH? Try running libraries\setup_build_env.ps1 first)"
  exit 1
}

Write-Host ""
Write-Host "FINISHED -- ready to build"
