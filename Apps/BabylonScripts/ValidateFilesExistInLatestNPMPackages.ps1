Push-Location $PSScriptRoot
Write-Host "Installing npm packages for comparison..."
npm install

$files = "babylon.max.js", "babylon.gui.js", "babylonjs.materials.js"
$succeeded = $true
foreach ($file in $files)
{
    $localFile = "$PSScriptRoot\$file"
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

Pop-Location

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