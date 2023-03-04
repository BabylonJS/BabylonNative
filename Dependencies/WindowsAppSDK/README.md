# Windows App SDK

Windows App SDK uses C++/WinRT generated headers which are not compatible between versions. To avoid this problem, we generate the ABI-stable headers from the winmd files.

## Build

_Building is only necessary if the code requires a new API from a newer version of the Windows App SDK._

1. Install the latest version of PowerShell.  
   For example, from the command prompt:
   ```
   winget install Microsoft.PowerShell
   ```
1. Install Visual Studio 2022.
1. Open a `Developer Command Prompt for VS 2022` from the start menu.  
   The build tools (`winmdidl` and `midlrt`) must be in the PATH.
1. Run [Build.cmd](./Build.cmd).

This will generate the ABI-stable header into the [Generated Files](./Generated%20Files) folder.
