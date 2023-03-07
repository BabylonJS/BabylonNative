$ProgressPreference = "SilentlyContinue"

function Run-Command {
    Param(
        [string]$Command,
        [parameter(ValueFromRemainingArguments = $true)][string[]]$Arguments
    )

    $Output = (& $Command @Arguments 2>&1 | Out-String)
    if ($LASTEXITCODE -ne 0) {
        throw "Command failed with exit code ${LASTEXITCODE}:\n$Output"
    }
}

$OutputDir = "$PSScriptRoot\Output"
New-Item $OutputDir -ItemType "Directory" -Force | Out-Null

$GenerateFilesDir = "$PSScriptRoot\Generated Files"
New-Item $GenerateFilesDir -ItemType "Directory" -Force | Out-Null

$Nuget = "$OutputDir\nuget.exe"

if (-Not (Test-Path $Nuget -PathType Leaf)) {
    Write-Host "Download NuGet"
    Invoke-WebRequest "https://dist.nuget.org/win-x86-commandline/latest/nuget.exe" -OutFile $Nuget
}

Write-Host "Install Microsoft.WindowsAppSDK"
Run-Command $Nuget install "Microsoft.WindowsAppSDK" -OutputDirectory $OutputDir -DependencyVersion Ignore -Source "https://api.nuget.org/v3/index.json"

$LibDir = Resolve-Path "$OutputDir\Microsoft.WindowsAppSDK.*\lib"
$WindowsMetadataDir = Resolve-Path "${env:ProgramFiles(x86)}\Windows Kits\10\UnionMetadata\10.0.22000.0"
$Uap10LatestDir = Resolve-Path "$LibDir\uap10.0.*" | Select-Object -Last 1
$Uap10Dir = Resolve-Path "$LibDir\uap10.0"

$idlDir = "$OutputDir\idl"
New-Item $idlDir -ItemType "Directory" -Force | Out-Null

$WinmdFiles = @(
    Get-Item $Uap10LatestDir\Microsoft.Foundation.winmd
    Get-Item $Uap10LatestDir\Microsoft.Graphics.winmd
    Get-Item $Uap10LatestDir\Microsoft.UI.winmd
    Get-Item $Uap10Dir\Microsoft.UI.Xaml.winmd
    Get-Item $Uap10Dir\Microsoft.UI.Text.winmd
    Get-Item $Uap10Dir\Microsoft.Web.WebView2.Core.winmd
)

$RunCommandDef = ${function:Run-Command}.ToString()

$WinmdFiles | ForEach-Object -Parallel {
    ${function:Run-Command} = $using:RunCommandDef
    Write-Host "Run winmdidl $($_.Name)"
    Run-Command winmdidl /nologo /metadata_dir:$using:WindowsMetadataDir /metadata_dir:$using:Uap10LatestDir /metadata_dir:$using:Uap10Dir /outdir:$using:idlDir $_
}

Get-ChildItem -Path $idlDir | ForEach-Object -Parallel {
    ${function:Run-Command} = $using:RunCommandDef
    Write-Host "Run midlrt $($_.Name)"
    Run-Command midlrt /nologo /out "$using:generateFilesDir" /I $using:idlDir /nomd /nomidl /ns_prefix $_
}
