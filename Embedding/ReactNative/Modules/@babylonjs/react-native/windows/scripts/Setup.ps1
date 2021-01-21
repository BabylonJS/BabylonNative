Import-Module $PSScriptRoot\Utils.psm1

Restore-CMakeProject -Platform "x86"
Restore-CMakeProject -Platform "x64"
Restore-CMakeProject -Platform "arm"
Restore-CMakeProject -Platform "arm64"