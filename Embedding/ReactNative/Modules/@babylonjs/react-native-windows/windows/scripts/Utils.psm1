# MSBuild lookup adapted from https://blog.lextudio.com/locate-msbuild-via-powershell-on-different-operating-systems-140757bb8e18
function Get-MSBuildPath {
    $msbuild = ""
    $MSBuildExistsInPath = $null -ne (Get-Command "msbuild" -ErrorAction SilentlyContinue)
    if ($MSBuildExistsInPath) {
        $msbuild = "msbuild.exe"
        return $msbuild
    }

    if (-not(Get-PackageProvider NuGet -ErrorAction silentlyContinue)) {
        Install-PackageProvider -Name NuGet -MinimumVersion 2.8.5.201 -Confirm:$False -Force
    }
  
    if (-not(Get-InstalledModule VSSetup -ErrorAction silentlyContinue)) {
        Install-Module VSSetup -Scope CurrentUser -Confirm:$False -Force
    }
  
    Write-Host "`nLooking for MSBuild.exe..."
    $instance = Get-VSSetupInstance -All -Prerelease | Select-VSSetupInstance -Require 'Microsoft.Component.MSBuild' -Latest
    $installDir = $instance.installationPath
    Write-Host "Visual Studio 2019 found at $installDir"
    $msBuild = $installDir + '\MSBuild\Current\Bin\MSBuild.exe'
    if (![System.IO.File]::Exists($msBuild)) {
        Write-Error "Visual Studio 2019 and MSBuild weren't found on your PC. Exiting"
        exit 1
    }
    Write-Host "`MSBuild.exe found at $msBuild"

    return $msbuild
}

function Restore-CMakeProject {
    param(
        $Platform
    )

    Write-Host "Running cmake for $Platform dependencies" -ForegroundColor Cyan

    $BuildDir = "$PSScriptRoot\..\..\Build\uwp_$Platform"
    if (!(Test-Path $BuildDir)) {
      mkdir $BuildDir
    }

    $Arch = $Platform
    if ($Arch -Eq "x86") {
        $Arch = "Win32"
    }
    
    cd $BuildDir
    cmake -D CMAKE_SYSTEM_NAME=WindowsStore -D CMAKE_SYSTEM_VERSION=10.0 -A $Arch ..\..\..\windows

    if ($? -Eq $False) {
      Write-Error "cmake failed. Make sure cmake is added to your PATH variable"
      exit 1
    }

    Write-Host "Completed cmake setup for $Platform dependencies`n" -ForegroundColor Green
}

function Compile-Solution {
    param(
        $Solution,
        $Platform,
        $Configuration
    )

    $MSBuild = Get-MSBuildPath
    & "$MSBuild" /p:Configuration="$Configuration" /p:Platform="$Platform" /m $Solution
    if ($? -Eq $False) {
        Write-Error "$Platform $Configuration Build failed."
        exit 1
    }
    else {
        Write-Host "Completed building $Platform $Configuration dependencies.`n" -ForegroundColor Green
    }
}

function Clean-Solution {
    param(
        $Solution,
        $Platform,
        $Configuration
    )

    $MSBuild = Get-MSBuildPath
    & "$MSBuild" /t:Clean /p:Configuration="$Configuration" /p:Platform="$Platform" $Solution
    if ($? -Eq $False) {
        Write-Error "$Platform $Configuration Clean failed."
        exit 1
    }
    else {
        Write-Host "Completed cleaning $Platform $Configuration dependencies.`n" -ForegroundColor Green
    }
}
