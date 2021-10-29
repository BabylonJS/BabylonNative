### **Building on Windows, Targeting Linux via WSL**

WSL is the Windows Subsystem for Linux, which allows you to run Linux applications inside Windows. It is available in recent builds of Windows 10 (>19041) and in Windows 11. Because Babylon Native apps are GUI applications, you will need WSL 2. Follow [these instructions](https://docs.microsoft.com/en-us/windows/wsl/tutorials/gui-apps) to prepare a WSL environment with GUI support. Babylon Native has been tested on Ubuntu; other distributions may or may not work.

Once your environment is ready, you can open an Ubuntu shell and navigate to `/mnt` to view your Windows filesystem. (If you're using [Windows Terminal](https://www.microsoft.com/en-us/p/windows-terminal/9n0dx20hk701?activetab=pivot:overviewtab), new WSL tabs will open directly in %USERPROFILE%.) From there you can find your `BabylonNative` directory and create a new `ubuntu` directory under `build`. Then, proceed with the instructions in the [readme](../README.md#building-on-ubuntu-targeting-linux) for **Building on Ubuntu, Targeting Linux**, using `../..` as your path for CMake rather than `..`.

After you run `ninja`, you can launch the playground by running `/Apps/Playground/Playground`. You should see a playground window appear, and output will be displayed in your shell.
![image](https://user-images.githubusercontent.com/1342157/139491825-1ae992a3-d641-4ed6-b7df-f5126dc0d2e5.png)
