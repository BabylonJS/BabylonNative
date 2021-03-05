@ECHO OFF
SETLOCAL
SET PowerShellScriptPath=%~dpn0.ps1
PowerShell.exe -NoProfile -ExecutionPolicy Bypass -Command "& '%PowerShellScriptPath%' %1 %2 %3 %4;exit $LASTEXITCODE"