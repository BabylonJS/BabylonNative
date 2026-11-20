param(
    [Parameter(Mandatory)][string]$SuiteRoot,
    [Parameter(Mandatory)][string]$Playground,
    [Parameter(Mandatory)][string]$Artifacts
)
$ErrorActionPreference = 'Stop'
$env:TIMING_SUITE_ROOT = (Resolve-Path $SuiteRoot).Path
$env:BABYLON_NATIVE_PLAYGROUND = (Resolve-Path $Playground).Path
$env:TIMING_ARTIFACTS = [IO.Path]::GetFullPath($Artifacts)
New-Item -ItemType Directory -Force $env:TIMING_ARTIFACTS | Out-Null
$provenance = @{
    executableSha256 = (Get-FileHash $env:BABYLON_NATIVE_PLAYGROUND -Algorithm SHA256).Hash
    processor = @(Get-CimInstance Win32_Processor | Select-Object Name, NumberOfCores, NumberOfLogicalProcessors)
    graphics = @(Get-CimInstance Win32_VideoController | Select-Object Name, DriverVersion, AdapterRAM)
    os = Get-CimInstance Win32_OperatingSystem | Select-Object Caption, Version, BuildNumber
    runnerImage = $env:ImageVersion
    runnerArchitecture = $env:RUNNER_ARCH
    configuration = 'Release'
    engine = 'V8'
    graphicsApi = 'D3D11'
    phases = @()
}
Push-Location $PSScriptRoot
try {
    & node .\verify.cjs
    if ($LASTEXITCODE -ne 0) {
        throw 'Pinned workload verification failed.'
    }
    foreach ($mode in @('single', 'parallel')) {
        $env:TIMING_MODE = $mode
        $watch = [Diagnostics.Stopwatch]::StartNew()
        & node .\node_modules\@playwright\test\cli.js test --config .\playwright.config.cjs
        $code = $LASTEXITCODE
        $watch.Stop()
        $workers = if ($mode -eq 'single') { @('single') } else { @('first', 'second') }
        $reports = @($workers | ForEach-Object {
            $file = Join-Path $env:TIMING_ARTIFACTS "$mode\$_.json"
            if (Test-Path $file) {
                Get-Content $file -Raw | ConvertFrom-Json
            } else {
                throw "Missing worker report: $file"
            }
        })
        $provenance.phases += @{
            mode = $mode
            processWallMs = $watch.Elapsed.TotalMilliseconds
            exitCode = $code
            runnerWallMs = ($reports.elapsedMs | Measure-Object -Maximum).Maximum
            selected = ($reports.selectedCount | Measure-Object -Sum).Sum
            executed = (@($reports | ForEach-Object { $_.cases })).Count
            passed = ($reports.passed | Measure-Object -Sum).Sum
            failed = ($reports.failed | Measure-Object -Sum).Sum
            completed = @($reports | Where-Object { -not $_.completed }).Count -eq 0
        }
        $provenance | ConvertTo-Json -Depth 20 | Set-Content (Join-Path $env:TIMING_ARTIFACTS 'summary.json') -Encoding utf8
    }
    if (@($provenance.phases | Where-Object { $_.exitCode -ne 0 -or -not $_.completed -or $_.executed -ne 305 }).Count) {
        throw 'The Windows experiment has failures or incomplete coverage. Inspect the full reports.'
    }
} finally {
    Pop-Location
}
