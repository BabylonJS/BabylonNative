param (
    [Parameter(Mandatory = $false)]
    [string]$Platform,
    [Parameter(Mandatory = $false)]
    [string]$Configuration
)

Import-Module $PSScriptRoot\Utils.psm1

if ((!$Platform -And $Configuration) -Or
    ($Platform -And !$Configuration)) {
    Write-Error "Platform and Configuration flags must be provided together"
    exit 1;
}

$PlaygroundSolution = "$PSScriptRoot\..\..\..\..\..\Apps\Playground\Playground\windows\Playground.sln";

if (!$Platform -And !$Configuration) {
    Compile-Solution -Platform "x86" -Configuration "Debug" -Solution 
    Compile-Solution -Platform "x86" -Configuration "Release" -Solution $PlaygroundSolution
    Compile-Solution -Platform "x64" -Configuration "Release" -Solution $PlaygroundSolution
    Compile-Solution -Platform "x64" -Configuration "Debug" -Solution $PlaygroundSolution
    Compile-Solution -Platform "ARM" -Configuration "Debug" -Solution $PlaygroundSolution
    Compile-Solution -Platform "ARM" -Configuration "Release" -Solution $PlaygroundSolution
    Compile-Solution -Platform "ARM64" -Configuration "Release" -Solution $PlaygroundSolution
    Compile-Solution -Platform "ARM64" -Configuration "Debug" -Solution $PlaygroundSolution
}
else {
    Compile-Solution -Platform $Platform -Configuration $Configuration -Solution $PlaygroundSolution
}