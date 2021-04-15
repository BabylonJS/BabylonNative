Push-Location $PSScriptRoot
Write-Host "Installing npm packages..."
npm install

$files = "babylon.max.js", "babylon.max.js.map", "babylon.module.d.ts", "babylon.gui.js", "babylonjs.materials.js"
foreach ($file in $files)
{
    $localFile = "$PSScriptRoot\$file"
    $fileInPackage = Get-ChildItem "$PSScriptRoot\node_modules" -Filter $file -Recurse | % { $_.FullName }
    Write-Host "Copying $fileInPackage to $localFile"
    Copy-Item -Path $fileInPackage -Destination $localFile -Force
}