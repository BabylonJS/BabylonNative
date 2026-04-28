#
# Build the Babylon Native AAR in release / MinSizeRel mode and print the
# size of the produced .aar and of every libBabylonNativeJNI.so it contains.
#
[CmdletBinding()]
param(
    [string] $JsEngine    = "QuickJS",
    [string] $GraphicsApi = "OpenGL",
    [string] $NdkVersion  = $null
)

$ErrorActionPreference = "Stop"
$scriptDir = $PSScriptRoot
Push-Location $scriptDir
try {
    $gradlewArgs = @(
        ":BabylonNative:assembleRelease"
        "-PjsEngine=$JsEngine"
        "-PGRAPHICS_API=$GraphicsApi"
    )
    if ($NdkVersion) {
        $gradlewArgs += "-PNDK_VERSION=$NdkVersion"
    }

    Write-Host "==> Building AAR (release / MinSizeRel)..." -ForegroundColor Cyan
    & "$scriptDir\gradlew.bat" @gradlewArgs
    if ($LASTEXITCODE -ne 0) {
        throw "Gradle build failed with exit code $LASTEXITCODE"
    }

    $aar = Join-Path $scriptDir "BabylonNative\build\outputs\aar\BabylonNative-release.aar"
    if (-not (Test-Path $aar)) {
        throw "AAR not found at $aar"
    }

    Write-Host ""
    Write-Host "==> Artifact sizes" -ForegroundColor Cyan
    $aarBytes = (Get-Item $aar).Length
    Write-Host ("AAR : {0,10:N0} bytes  ({1,7:F2} MB)  {2}" -f $aarBytes, ($aarBytes / 1MB), $aar)

    Add-Type -AssemblyName System.IO.Compression.FileSystem
    $zip = [System.IO.Compression.ZipFile]::OpenRead($aar)
    try {
        foreach ($entry in $zip.Entries | Where-Object { $_.FullName -like "jni/*libBabylonNativeJNI.so" } | Sort-Object FullName) {
            $compressed = $entry.CompressedLength
            $uncompressed = $entry.Length
            Write-Host ("  - {0,-50} {1,10:N0} B uncompressed ({2,6:F2} MB), {3,10:N0} B in AAR ({4,6:F2} MB)" -f `
                $entry.FullName, $uncompressed, ($uncompressed / 1MB), $compressed, ($compressed / 1MB))
        }
    } finally {
        $zip.Dispose()
    }
}
finally {
    Pop-Location
}
