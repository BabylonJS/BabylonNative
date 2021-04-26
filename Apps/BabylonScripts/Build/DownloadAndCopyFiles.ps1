Push-Location $PSScriptRoot
Write-Host "Installing npm packages..."
npm install

$files = "babylon.gui.js",
    "babylon.gui.js.map",
    "babylon.gui.module.d.ts",
    "babylon.max.js",
    "babylon.max.js.map",
    "babylon.module.d.ts",
    "babylonjs.loaders.js",
    "babylonjs.loaders.js.map",
    "babylonjs.loaders.module.d.ts",
    "babylonjs.materials.js",
    "babylonjs.materials.js.map",
    "babylonjs.materials.module.d.ts";

foreach ($file in $files)
{
    $localFile = "$PSScriptRoot\..\$file"
    $fileInPackage = Get-ChildItem "$PSScriptRoot\node_modules" -Filter $file -Recurse | % { $_.FullName }
    Write-Host "Copying $fileInPackage to $localFile"
    Copy-Item -Path $fileInPackage -Destination $localFile -Force
}