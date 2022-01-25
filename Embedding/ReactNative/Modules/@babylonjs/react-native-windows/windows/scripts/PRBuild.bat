@ECHO OFF
SETLOCAL
SET NODE_OPTIONS="--max-old-space-size=8192"
SET PowerShellScriptPath=%~dpn0.ps1
PowerShell.exe -NoProfile -ExecutionPolicy Bypass -Command "& '%PowerShellScriptPath%' %1 %2;exit $LASTEXITCODE"