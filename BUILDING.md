# Building

This quick overview will help you get started developing in the Babylon Native
repository. We support development on Windows, macOS, and Linux. This overview is intended
for developers familiar with common native development practices.

## **All Development Platforms**

**Required Tools:** [git](https://git-scm.com/), [CMake](https://cmake.org/), [node.js](https://nodejs.org/en/)

The first step for all development environments and targets is to clone the repo. Use a
git-enabled terminal to follow the steps below.

```
git clone https://github.com/BabylonJS/BabylonNative.git
```

Babylon Native's build system is based on CMake, which customarily uses a separate
build directory. Build directory location is up to you, but we highly recommend using
the `build` directory from the repository root. The `.gitignore` file is set up to
ignore this `build` directory.

**NOTE:** CMake considers what are sometimes called "build flavors" (Win32/x86 versus
Win32/x64 versus UWP/x64, etc.) to be entirely different build targets which should have
separate build folders. For cross-platform development, we commonly use multiple
subfolders inside the `build` folder, such as `build/win32_x86` and `build/uwp_x64`, to
house the builds for different platforms, with each subfolder treated just like the
`build` folder in the instructions below.

## **Building on Windows 10, Targeting Windows Desktop (Win32)**

_Follow the steps from [All Development Platforms](#all-development-platforms) before proceeding._

**Required Tools:** [Visual Studio 2022](https://visualstudio.microsoft.com/vs/) with
C++ development tools, [Python 3.0](https://www.python.org/) or newer (required by
dependencies)

For Windows development, CMake will generate a Visual Studio solution. From the root
of the repository on the command line, run the following command:

```
cmake -B build\win32
```

Note that, by default, this will target the same processor architecture that is being
used to build the solution. If your development environment uses a 64-bit CPU, CMake's
generated solution will target `x64`. To manually target specific architectures, tell
CMake the intended architecture using the `-A` flag, as shown below. Supported
arguments for using this flag with Babylon Native include `Win32` for 32-bit
processors and `x64` for 64-bit processors.

```
cmake -B build\win32_x86 -A Win32
```

CMake will generate a new `BabylonNative.sln` file in the specified build folder.
When CMake is complete, open `BabylonNative.sln` by double-clicking on it in
Windows Explorer or by running the following command:

```
start build\win32\BabylonNative.sln
```

By default, the `Playground` demo app will be set as the Visual Studio start-up
project.

## **Building on Windows 10, Targeting Universal Windows Platform (UWP)**

_Follow the steps from [All Development Platforms](#all-development-platforms) before proceeding._

**Required Tools:** [Visual Studio 2022](https://visualstudio.microsoft.com/vs/) with
C++ and UWP development tools, [Python 3.0](https://www.python.org/) or newer (required
by dependencies)

For Windows development, CMake will generate a Visual Studio solution. By default it
will target Win32. To target UWP (which CMake describes as `Windows Store`), you must
specify a couple CMake variables. From the root of the repository on the command line,
run the following command:

```
cmake -B build/uwp -D CMAKE_SYSTEM_NAME=WindowsStore -D CMAKE_SYSTEM_VERSION=10.0
```

Note that, by default, this will target the same processor architecture that is being
used to build the solution. If your development environment uses a 64-bit CPU, CMake's
generated solution will target `x64`. To manually target specific architectures, tell
CMake the intended architecture using the `-A` flag, as shown below. Supported
arguments for using this flag with Babylon Native include `Win32` for 32-bit
processors, `x64` for 64-bit processors, `arm` for ARM processors, and `arm64` for
ARM64 processors.

```
cmake -B build/uwp_arm64 -D CMAKE_SYSTEM_NAME=WindowsStore -D CMAKE_SYSTEM_VERSION=10.0 -A arm64
```

CMake will generate a new `BabylonNative.sln` file in the specified build folder.
When CMake is complete, open `BabylonNative.sln` by double-clicking on it in
Windows Explorer or by running the following command:

```
start build\uwp\BabylonNative.sln
```

By default, the `Playground` demo app will be set as the Visual Studio start-up
project.

## **Building on Windows 10, Targeting HoloLens 2**

_Follow the steps from [All Development Platforms](#all-development-platforms) before proceeding._

**Required Tools:** [Visual Studio 2022](https://visualstudio.microsoft.com/vs/) with
C++ and UWP development tools, [Python 3.0](https://www.python.org/) or newer (required
by dependencies)

HoloLens 2 supports `arm64` UWP applications. To create a HoloLens 2 Visual Studio
solution for a physical device, run the following command from the repository root:

```
cmake -B build/uwp_arm64 -D CMAKE_SYSTEM_NAME=WindowsStore -D CMAKE_SYSTEM_VERSION=10.0 -A arm64
```

**Additional notes for HoloLens 2 development**:
* At this time, running immersive applications on **HoloLens 2 emulators** is **NOT** supported. We are tracking support for **HoloLens 2 emulators** [here](https://github.com/BabylonJS/BabylonNative/issues/448).
* For the Playground app, setting the `hololens` javascript variable to true in `experience.js` will configure the experience for **HoloLens 2**.
* HoloLens 2 immersive experiences require the `Spatial Perception` UWP capability. This capability is enabled in the application's `Package.appxmanifest`. When creating a custom BabylonNative project, one will need to enable the `Spatial Perception` UWP cability in their app's `Package.appxmanifest`. For more information on UWP capabilities, see [here](https://docs.microsoft.com/en-us/windows/uwp/packaging/app-capability-declarations).
* HoloLens 2 immersive experiences are built on top of `OpenXR`. Updates to the `OpenXR` runtime should automtically be installed on physical **HoloLens 2** devices through the Windows Store. If for some reason updates are not automatically installed (custom device configurations, lack of network connectivity, etc), developers may need to manually install updates for the `OpenXR` runtime through the Windows Store. For more information on installing newer versions of `OpenXR`, see [here](https://docs.microsoft.com/en-us/windows/mixed-reality/develop/native/openxr-getting-started).

## **Building on macOS, Targeting macOS**

**Required Tools:** [Xcode 11](https://developer.apple.com/xcode/) or newer,
[Python 3.0](https://www.python.org/) or newer (required by dependencies)

For macOS development, CMake will generate a Makefile by default. It may be possible
to build Babylon Native for macOS using this approach, but only the Xcode method is
supported at present. To generate an Xcode project using CMake, you must specify the
correct build system generator for CMake to use. Run the following command from the
repository root:

```
cmake -B build/macOS -G Xcode
```

CMake will generate a new `BabylonNative.xcodeproj` file in the specified build folder.
Open the project by double-clicking on it in Finder or by entering the following command:

```
open build/macOS/BabylonNative.xcodeproj
```

To select which project to build with Xcode, select the correct project name in the
menu to the right of the greyed-out `Stop` button adjacent to the `Play` button in
the top-left corner of the Xcode window. For example, to build and run the Playground
demo app, click on the project selector and find `Playground` in the list of possible
selections. The `Play` button will subsequently allow you to build, run, and debug
the selected Babylon Native demo app.

## **Building on macOS, Targeting iOS**

_Follow the steps from [All Development Platforms](#all-development-platforms) before proceeding._

**Required Tools:** [Xcode 11](https://developer.apple.com/xcode/) or newer,
[Python 3.0](https://www.python.org/) or newer (required by dependencies)

For macOS development, CMake will generate a Makefile by default. It may be possible
to build Babylon Native for macOS using this approach, but only the Xcode method is
supported at present. To generate an Xcode project using CMake, you must specify the
correct build system generator for CMake to use. Additionally, you must tell CMake
what toolchain to use, which provides additional information about how to generate an
iOS Xcode project correctly. Run the following command from the repository root:

```
cmake -B build/iOS -G Xcode -D IOS=ON
```

To enable bitcode support, add this option to the cmake command line parameters:

```
-D ENABLE_BITCODE=ON
```

CMake will generate a new `BabylonNative.xcodeproj` file in the specified build folder.
Open the project by double-clicking on it in Finder or by entering the following command:

```
open build/iOS/BabylonNative.xcodeproj
```

To select which project to build with Xcode, select the correct project name in the
menu to the right of the greyed-out `Stop` button adjacent to the `Play` button in
the top-left corner of the Xcode window. For example, to build and run the Playground
demo app, click on the project selector and find `Playground` in the list of possible
selections. The `Play` button will subsequently allow you to build, run, and debug
the selected Babylon Native demo app.

### Troubleshooting
```
CMake Error at /Applications/CMake 2.app/Contents/share/cmake-3.26/Modules/Platform/iOS-Initialize.cmake:4 (message):
   is not an iOS SDK
```
If you see an error like this, it might be because you have updated Xcode and/or installed a new version of the iOS SDK since last time CMake was run. Try cleaning (e.g. `git clean -dxff`, but back up any changes you want to keep) and re-running the CMake command.

## **Building on macOS, Targeting visionOS**

_Follow the steps from [All Development Platforms](#all-development-platforms) before proceeding._

**Required Tools:** [Xcode 15](https://developer.apple.com/xcode/) or newer,
[Python 3.0](https://www.python.org/) or newer (required by dependencies)

For macOS development, CMake will generate a Makefile by default. It may be possible
to build Babylon Native for macOS using this approach, but only the Xcode method is
supported at present. To generate an Xcode project using CMake, you must specify the
correct build system generator for CMake to use. Additionally, you must tell CMake
what toolchain to use, which provides additional information about how to generate an
visionOS Xcode project correctly. Run the following command from the repository root:

```
cmake -B build/visionOS -G Xcode -D VISIONOS=ON
```

To enable bitcode support, add this option to the cmake command line parameters:

```
-D ENABLE_BITCODE=ON
```

CMake will generate a new `BabylonNative.xcodeproj` file in the specified build folder.
Open the project by double-clicking on it in Finder or by entering the following command:

```
open build/visionOS/BabylonNative.xcodeproj
```

To select which project to build with Xcode, select the correct project name in the
menu to the right of the greyed-out `Stop` button adjacent to the `Play` button in
the top-left corner of the Xcode window. For example, to build and run the Playground
demo app, click on the project selector and find `Playground` in the list of possible
selections. The `Play` button will subsequently allow you to build, run, and debug
the selected Babylon Native demo app.

## **Building on Windows, Targeting Android**

_Follow the steps from [All Development Platforms](#all-development-platforms) before proceeding._

**Required Tools:**
[Android Studio](https://developer.android.com/studio), [Node.js](https://nodejs.org/en/download/), [Ninja](https://ninja-build.org/)

The minimal requirement target is Android 5.0 with an OpenGL ES 3.0 compatible GPU.

Only building with Android Studio is supported. CMake is not used directly. Instead, Gradle
is used for building and CMake is automatically invocated for building the native part.
An `.apk` that can be executed on your device or simulator is the output.

First, download the latest release of Ninja, extract the binary, and add it to your system path.

Babylon Native on Android supports two JavaScript engines: V8 and JavaScriptCore. V8 is
used by default if no engine is specified. To change the engine to JavaScriptCore, open
the file `Apps\Playground\Android\gradle.properties` and add the following line:

```
jsEngine=JavaScriptCore
```

Open the project located at `Apps\Playground\Android` with Android Studio. Then in the menu,
select `Run` -> `Run 'app'`. If you don't have an Android device plugged in or no Android image
in the Android emulator, that option will be greyed and inaccessible. Instructions and tips
on how to install the emulator are [available here](Documentation/AndroidEmulator.md).

## **Building on Ubuntu, Targeting Linux**

_Follow the steps from [All Development Platforms](#all-development-platforms) before proceeding._

**Required Tools:**
[Clang](https://clang.llvm.org/) or [GCC](https://gcc.gnu.org/)

*See also: [**Building on Windows using WSL**](Documentation/WSL.md)*

The minimal requirement target is an OpenGL 3.3 compatible GPU. Clang 9+ or GCC 9+ are required for building.

First step is to install packages mandatory for building. For example, with Clang-9 toolchain:

```
sudo apt-get install libgl1-mesa-dev libcurl4-openssl-dev clang-9 libc++-9-dev libc++abi-9-dev lld-9 ninja-build
```

Depending on the JavaScript engine you want to use, you will have to install the package accordingly:

### JavaScriptCore

Install the following package:

```
sudo apt-get install libjavascriptcoregtk-4.1-dev
```

Then, run cmake targetting a Ninja make file:

```
cmake -G Ninja -D JAVASCRIPTCORE_LIBRARY=/usr/lib/x86_64-linux-gnu/libjavascriptcoregtk-4.1.so -D NAPI_JAVASCRIPT_ENGINE=JavaScriptCore
```

### V8

Install the following package:

```
sudo apt-get install libv8-dev
```

Then, run cmake targetting a Ninja make file:

```
cmake -G Ninja -D NAPI_JAVASCRIPT_ENGINE=V8
```

And finally, for any JavaScript engine, run a build:

```
ninja
```

You can switch compiler between GCC and Clang by defining shell variables.
And example for clang

```
export CC=/usr/bin/clang
export CXX=/usr/bin/clang++
```

and GCC

```
export CC=/usr/bin/gcc
export CXX=/usr/bin/g++
```

You will have to run CMake again to take changes into account.

## Selecting the Graphics API

For Win32, UWP, Android, and Linux, it's possible to build for different graphics API.
With CMake, add this parameter to the command line:
```
-D GRAPHICS_API=<XXX>
```

For Win32 and UWP, `<XXX>` can be `D3D11` (default), `D3D12`, or `Vulkan`.

For Linux, `<XXX>` can be `OpenGL` (default) or `Vulkan`.

For Android, the switch to Vulkan instead of OpenGL (default) is done with Gradle by adding a command line parameter:
```
-PGRAPHICS_API=Vulkan
```

If no `GRAPHICS_API` is provided, the build will use the default.

**Note**

- For Apple devices, Metal is the only possible choice.
- Vulkan and D3D12 are under development and might not be stable enough for production purposes.
