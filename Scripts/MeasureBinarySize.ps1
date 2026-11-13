<#
.SYNOPSIS
    Compares binary sizes between two build trees and emits a markdown report.

.DESCRIPTION
    Enumerates every .exe / .dll / .lib under each build directory's matching
    configuration subfolder, computes per-artifact size deltas, and writes a
    markdown report (Playground.exe delta headline, aggregate, top 10 other
    movers) to "<OutputDir>/body.md".

    Used by the size-check CI workflow. Also useful locally: build two trees
    (a master worktree and your feature branch worktree) with the same cmake
    flags and point this at them.

.PARAMETER BaseBuildDir
    Path to the "base" build tree (the reference / before).

.PARAMETER HeadBuildDir
    Path to the "head" build tree (the change / after).

.PARAMETER Config
    Build configuration name (e.g. 'Release', 'RelWithDebInfo'). Artifacts are
    selected by their path containing "\<Config>\".

.PARAMETER OutputDir
    Directory where body.md (and pr-number.txt, if -PrNumber is given) are
    written. Created if missing.

.PARAMETER Platform
    Free-form description of the build configuration shown in the report
    header. Defaults to "Win32 x64 D3D11 <Config>".

.PARAMETER BaseRef
    Optional base git ref / SHA. Short-form shown in the report header.

.PARAMETER HeadRef
    Optional head git ref / SHA. Short-form shown in the report header.

.PARAMETER PrNumber
    Optional PR number. When provided, written to OutputDir/pr-number.txt so
    the privileged comment workflow can locate the originating PR.

.EXAMPLE
    ./Scripts/MeasureBinarySize.ps1 `
        -BaseBuildDir ../master/build/win32 `
        -HeadBuildDir ../my-feature/build/win32 `
        -Config Release `
        -OutputDir ./size-report
#>

[CmdletBinding()]
param(
    [Parameter(Mandatory)]
    [string]$BaseBuildDir,

    [Parameter(Mandatory)]
    [string]$HeadBuildDir,

    [Parameter(Mandatory)]
    [string]$Config,

    [Parameter(Mandatory)]
    [string]$OutputDir,

    [string]$Platform,

    [string]$BaseRef,

    [string]$HeadRef,

    [int]$PrNumber
)

$ErrorActionPreference = 'Stop'

$marker = '<!-- binary-size-check -->'

if (-not $Platform) {
    $Platform = "Win32 x64 D3D11 $Config"
}

function Get-Artifacts {
    param([string]$Root, [string]$Cfg)

    if (-not (Test-Path $Root)) {
        return @()
    }
    Get-ChildItem $Root -Recurse -File -Include *.exe,*.dll,*.lib -ErrorAction SilentlyContinue |
        Where-Object { $_.FullName -match "\\$Cfg\\" } |
        ForEach-Object {
            [PSCustomObject]@{
                Rel  = $_.FullName.Substring($Root.Length + 1)
                Size = $_.Length
            }
        }
}

function Format-Bytes {
    param([long]$N)

    $sign = if ($N -lt 0) { '-' } else { '+' }
    $abs  = [math]::Abs($N)
    if ($abs -ge 1MB) {
        return "$sign$([math]::Round($abs / 1MB, 2)) MB"
    }
    if ($abs -ge 1KB) {
        return "$sign$([math]::Round($abs / 1KB, 1)) KB"
    }
    return "$sign$abs B"
}

function Format-Count {
    param([long]$N)
    '{0:N0}' -f $N
}

$baseArts = Get-Artifacts -Root $BaseBuildDir -Cfg $Config
$headArts = Get-Artifacts -Root $HeadBuildDir -Cfg $Config

if ($baseArts.Count -eq 0 -or $headArts.Count -eq 0) {
    throw "No artifacts found. base=$($baseArts.Count) head=$($headArts.Count). Build likely failed."
}

$basePg = $baseArts | Where-Object { $_.Rel -match 'Playground\.exe$' } | Select-Object -First 1
$headPg = $headArts | Where-Object { $_.Rel -match 'Playground\.exe$' } | Select-Object -First 1

if (-not $basePg -or -not $headPg) {
    throw "Playground.exe missing. base=$($null -ne $basePg) head=$($null -ne $headPg)"
}

$pgDelta = $headPg.Size - $basePg.Size
$pgPct   = if ($basePg.Size -gt 0) { [math]::Round(100 * $pgDelta / $basePg.Size, 3) } else { 0 }

# Build name -> {Base, Head} map so we can diff every artifact in either set.
$pairs = @{}
foreach ($a in $baseArts) {
    $pairs[$a.Rel] = @{ Base = $a.Size; Head = 0 }
}
foreach ($a in $headArts) {
    if (-not $pairs.ContainsKey($a.Rel)) {
        $pairs[$a.Rel] = @{ Base = 0; Head = 0 }
    }
    $pairs[$a.Rel].Head = $a.Size
}
$rows = $pairs.Keys | ForEach-Object {
    $p = $pairs[$_]
    [PSCustomObject]@{
        Rel   = $_
        Base  = $p.Base
        Head  = $p.Head
        Delta = $p.Head - $p.Base
    }
}

# Top movers, excluding Playground.exe (already featured). 256 B threshold trims
# COFF timestamp noise on libs whose source didn't change.
$top = $rows |
    Where-Object { $_.Rel -notmatch 'Playground\.exe$' -and [math]::Abs($_.Delta) -gt 256 } |
    Sort-Object { [math]::Abs($_.Delta) } -Descending |
    Select-Object -First 10

$baseTotal = ($baseArts | Measure-Object Size -Sum).Sum
$headTotal = ($headArts | Measure-Object Size -Sum).Sum
$totalDelta = $headTotal - $baseTotal
$totalPct   = if ($baseTotal -gt 0) { [math]::Round(100 * $totalDelta / $baseTotal, 3) } else { 0 }

$sb = [System.Text.StringBuilder]::new()
[void]$sb.AppendLine($marker)
[void]$sb.AppendLine('## Binary size impact')
[void]$sb.AppendLine('')

$refLine = "Platform: ``$Platform``"
if ($BaseRef) { $refLine += " &nbsp;|&nbsp; base ``$($BaseRef.Substring(0, [math]::Min(7, $BaseRef.Length)))``" }
if ($HeadRef) { $refLine += " &nbsp;|&nbsp; head ``$($HeadRef.Substring(0, [math]::Min(7, $HeadRef.Length)))``" }
[void]$sb.AppendLine($refLine)
[void]$sb.AppendLine('')

[void]$sb.AppendLine('### Final linked binary')
[void]$sb.AppendLine('')
[void]$sb.AppendLine('| Artifact | Base | Head | Δ bytes | Δ% |')
[void]$sb.AppendLine('|---|---:|---:|---:|---:|')
[void]$sb.AppendLine("| **Playground.exe** | $(Format-Count -N $basePg.Size) | $(Format-Count -N $headPg.Size) | **$(Format-Bytes -N $pgDelta)** | **$pgPct %** |")
[void]$sb.AppendLine("| Aggregate (.exe/.dll/.lib) | $(Format-Count -N $baseTotal) | $(Format-Count -N $headTotal) | $(Format-Bytes -N $totalDelta) | $totalPct % |")
[void]$sb.AppendLine('')

if ($top -and $top.Count -gt 0) {
    [void]$sb.AppendLine('### Top movers (other static libs / DLLs, |Δ| > 256 B)')
    [void]$sb.AppendLine('')
    [void]$sb.AppendLine('| Artifact | Base | Head | Δ bytes |')
    [void]$sb.AppendLine('|---|---:|---:|---:|')
    foreach ($r in $top) {
        $shortRel = $r.Rel -replace '\\', '/'
        [void]$sb.AppendLine("| ``$shortRel`` | $(Format-Count -N $r.Base) | $(Format-Count -N $r.Head) | $(Format-Bytes -N $r.Delta) |")
    }
    [void]$sb.AppendLine('')
} else {
    [void]$sb.AppendLine('_No other artifact moved by more than 256 B._')
    [void]$sb.AppendLine('')
}

[void]$sb.AppendLine('---')
[void]$sb.AppendLine('<sub>Comparison is informational only — no threshold enforcement. Small deltas on unchanged libs are typically COFF timestamps.</sub>')

$body = $sb.ToString()

New-Item -ItemType Directory -Force $OutputDir | Out-Null
$bodyFile = Join-Path $OutputDir 'body.md'
$body | Set-Content -Path $bodyFile -Encoding utf8

if ($PrNumber -gt 0) {
    $prFile = Join-Path $OutputDir 'pr-number.txt'
    $PrNumber | Set-Content -Path $prFile -Encoding utf8
}

# Surface in the GitHub Actions run summary when running in CI.
if ($env:GITHUB_STEP_SUMMARY) {
    $body | Add-Content -Path $env:GITHUB_STEP_SUMMARY -Encoding utf8
}

Write-Host '=== binary size report ==='
Write-Host $body
Write-Host '=========================='
Write-Host ''
Write-Host "Headline: Playground.exe $(Format-Bytes -N $pgDelta) ($pgPct %)"
Write-Host "Output:   $OutputDir"
