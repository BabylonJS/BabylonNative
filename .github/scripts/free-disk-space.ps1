$ErrorActionPreference = 'Stop'

Get-PSDrive -PSProvider FileSystem | Format-Table Name, Used, Free

# Preserve native compilers, Java, Android tools on Unix hosts, and all build caches.
$paths = if ($IsLinux) {
    @('/usr/share/dotnet', '/usr/local/.ghcup', '/opt/ghc', '/usr/share/swift')
} elseif ($IsMacOS) {
    @((Join-Path $HOME '.dotnet'), '/usr/local/share/dotnet', '/usr/local/.ghcup')
} elseif ($IsWindows) {
    @('C:\Program Files (x86)\Android\android-sdk')
} else {
    throw 'Unsupported runner operating system.'
}

foreach ($path in $paths) {
    if (Test-Path -LiteralPath $path) {
        Write-Host "Removing unused toolchain: $path"
        if ($IsWindows) {
            Remove-Item -LiteralPath $path -Recurse -Force
        } else {
            sudo rm -rf -- $path
            if ($LASTEXITCODE -ne 0) {
                throw "Failed to remove unused toolchain: $path"
            }
        }
    }
}

Get-PSDrive -PSProvider FileSystem | Format-Table Name, Used, Free
