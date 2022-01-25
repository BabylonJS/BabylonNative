Import-Module $PSScriptRoot\Utils.psm1

Compile-Solution -Platform "x64" -Configuration "Debug" -Solution "$PSScriptRoot\..\..\..\react-native\Build\uwp_x64\ReactNativeBabylon.sln"
nuget restore "$PSScriptRoot\..\..\..\..\..\Apps\Playground\Playground\windows\Playground.sln"
Compile-Solution -Platform "x64" -Configuration "Debug" -Solution "$PSScriptRoot\..\..\..\..\..\Apps\Playground\Playground\windows\Playground.sln"