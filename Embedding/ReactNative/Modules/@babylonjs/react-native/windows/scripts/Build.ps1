Import-Module $PSScriptRoot\Utils.psm1

Compile-Solution -Platform "x86" -Configuration "Debug" -Solution "$PSScriptRoot\..\BabylonReactNative.sln"
Compile-Solution -Platform "x86" -Configuration "Release" -Solution "$PSScriptRoot\..\BabylonReactNative.sln"
Compile-Solution -Platform "x64" -Configuration "Release" -Solution "$PSScriptRoot\..\BabylonReactNative.sln"
Compile-Solution -Platform "x64" -Configuration "Debug" -Solution "$PSScriptRoot\..\BabylonReactNative.sln"
Compile-Solution -Platform "arm" -Configuration "Debug" -Solution "$PSScriptRoot\..\BabylonReactNative.sln"
Compile-Solution -Platform "arm" -Configuration "Release" -Solution "$PSScriptRoot\..\BabylonReactNative.sln"
Compile-Solution -Platform "ARM64" -Configuration "Release" -Solution "$PSScriptRoot\..\BabylonReactNative.sln"
Compile-Solution -Platform "ARM64" -Configuration "Debug" -Solution "$PSScriptRoot\..\BabylonReactNative.sln"