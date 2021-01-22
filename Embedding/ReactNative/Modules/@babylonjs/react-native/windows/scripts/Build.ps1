Import-Module $PSScriptRoot\Utils.psm1

Compile-Solution -Platform "Win32" -Configuration "Debug" -Solution "$PSScriptRoot\..\..\submodules\BabylonNative\Build_uwp_x86\ReactNativeBabylon.sln"
Compile-Solution -Platform "Win32" -Configuration "Release" -Solution "$PSScriptRoot\..\..\submodules\BabylonNative\Build_uwp_x86\ReactNativeBabylon.sln"
Compile-Solution -Platform "x64" -Configuration "Release" -Solution "$PSScriptRoot\..\..\submodules\BabylonNative\Build_uwp_x64\ReactNativeBabylon.sln"
Compile-Solution -Platform "x64" -Configuration "Debug" -Solution "$PSScriptRoot\..\..\submodules\BabylonNative\Build_uwp_x64\ReactNativeBabylon.sln"
Compile-Solution -Platform "arm" -Configuration "Debug" -Solution "$PSScriptRoot\..\..\submodules\BabylonNative\Build_uwp_arm\ReactNativeBabylon.sln"
Compile-Solution -Platform "arm" -Configuration "Release" -Solution "$PSScriptRoot\..\..\submodules\BabylonNative\Build_uwp_arm\ReactNativeBabylon.sln"
Compile-Solution -Platform "ARM64" -Configuration "Release" -Solution "$PSScriptRoot\..\..\submodules\BabylonNative\Build_uwp_arm64\ReactNativeBabylon.sln"
Compile-Solution -Platform "ARM64" -Configuration "Debug" -Solution "$PSScriptRoot\..\..\submodules\BabylonNative\Build_uwp_arm64\ReactNativeBabylon.sln"