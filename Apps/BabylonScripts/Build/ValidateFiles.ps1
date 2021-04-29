Push-Location $PSScriptRoot
Write-Host "Installing npm packages for comparison..."
npm install
Pop-Location

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

$succeeded = $true
foreach ($file in $files)
{
    $localFile = "$PSScriptRoot\..\$file"
    $fileInPackage = Get-ChildItem "$PSScriptRoot\node_modules" -Filter $file -Recurse | % { $_.FullName }
    if (!($(Get-FileHash $localFile).hash -eq $(Get-FileHash $fileInPackage).hash))
    {
        $errorString = "$file did not match the file in the most recent npm preview package ($fileInPackage)"
        $succeeded = $false
    }
    else
    {
        Write-Host "$file matched the file in the most recent npm preview package ($fileInPackage)" -ForegroundColor Green  
    }
}

if (!$succeeded)
{
    Write-Error $errorString
    exit -1
}
else
{
    Write-Host "File check succeeded!" -ForegroundColor Green
    exit 0
}