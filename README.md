[![Build Status](https://dev.azure.com/babylonjs/ContinousIntegration/_apis/build/status/BabylonNative%20CI?branchName=master)](https://dev.azure.com/babylonjs/ContinousIntegration/_build/latest?definitionId=6&branchName=master)
[![Nightly build](https://github.com/BabylonJS/BabylonNative/workflows/Nightly%20build/badge.svg)](https://github.com/BabylonJS/BabylonNative/actions?query=workflow%3A%22Nightly+build%22)

# Babylon Native

Welcome! Babylon Native is a collection of technologies intended to bring 
the power and flexibility of Babylon.js to cross-platform applications beyond the 
browser. The goal of this project is to allow the same JavaScript that powers Babylon.js 
apps on the Web to work identically in native apps on Windows, macOS, iOS, Android, and 
Linux.

Check out [the project's announcement](https://medium.com/@babylonjs/babylon-native-821f1694fffc) 
and [home page](https://aka.ms/Bnative) for more information.

*NOTE: **This project is under heavy development** and not all platforms/features 
are currently available. Please refer to the sections below for details.*

## Current Status

Babylon Native is currently available as a **public preview**. Many features are 
currently available, and additional features are being added regularly, but certain
features are currently unstable or not yet implemented.

The following major features are currently supported and expected to work. Note that 
this list is not exhaustive.

- Developing Babylon Native on the following platforms: Windows 10, macOS.
- Building and running the libraries and demo apps for the following platforms: Win32,
UWP, macOS, iOS, Android.
- Loading and executing JavaScript code on all supported platforms.
- Loading and rendering glTF objects on all supported platforms.
- Network requests (including accessing local files using the `file://` protocol) 
made throught the Babylon.js APIs.
- Extending JavaScript functionality using N-API and native plugins.
- Debugging JavaScript with V8 on the following platforms: Win32.

The following major features are partially implemented but not yet supported. Note 
that this list is not exhaustive.

- Developing Babylon Native on the following platforms: Linux.
- Mixed reality powered by OpenXR on the following platforms: Win32, UWP.

The following major features are not yet supported or implemented, even as previews, 
but are expected to be supported in the future. Note that this list is not exhaustive.

- Debugging JavaScript with V8 on the following platforms: Android, Apple, UWP, iOS, Linux.
- User input.
- Font rendering.
- Sub-window, multi-window, and out-of-process rendering.

This section will be updated frequently. If you have any questions, please reach out
to us on [the Babylon forum](https://forum.babylonjs.com).

## Documentation

Babylon Native documentation is available in this repo. From extending Babylon Native using your very own features to debugging the rendering on iOS, [you'll find extensive documentation here.](Documentation/Readme.md)

For Frequently Asked Questions (FAQ) regarding build and other issues, [You can check this page.](Documentation/faq.md)

## Build System and Extensions

The Babylon Native build system strives for simplicity, modularity, and scalability
using a lateral dependency management strategy founded on Git Submodules and CMake 
targets. For specifics, please read our detailed documentation on
[the Babylon Native build system](Documentation/BuildSystem.md) and
[extending Babylon Native](Documentation/Extending.md).

## Quick Start Guide

This quick overview will help you get started developing in the Babylon Native 
repository. We support development on Windows and macOS. (It is also possible, 
though not yet fully supported, to develop on Linux.) This overview is intended 
for reasonably experienced developers familiar with common native development 
principles.

## Graphics API

For Win32, Android and Linux, it's possible to build for different Graphics API.
With CMake, add this parameter to the command line:
```
-DGRAPHICS_API=XXX
```

Where `XXX` can be D3D11 (default), D3D12 or Vulkan for Windows. `Vulkan` or OpenGL (default) for Linux.

For Android, the switch to Vulkan instead of OpenGL (default) is done with Gradle by adding a command line parameter:
```
-PGRAPHICS_API=Vulkan
```

If no `GRAPHICS_API` is provided, the build will use the default.

**Note**

- For Apple devices, Metal is the only possible choice.
- Vulkan and D3D12 are under development and might not be stable enough for your production.

### **All Development Platforms, Common First Steps**

**Required Tools:** [git](https://git-scm.com/), [CMake](https://cmake.org/), [node.js](https://nodejs.org/en/)

The first step for all development environments and targets is to clone the repo. Use a 
git-enabled terminal to follow the steps below. The `--recursive` flag is necessary as
Babylon Native makes extensive use of submodules to supply its dependencies.

```
git clone --recursive https://github.com/BabylonJS/BabylonNative.git
```

Babylon Native requires Babylon.js. You will need to install NPM packages to resolve these dependencies.

```
cd <repo root>
cd Apps
npm install
```

Babylon Native's build system is based on CMake, which customarily uses a separate
build directory. Build directory location is up to you, but we recommend just creating 
a `Build` directory within your clone of the Babylon Native repository (Babylon 
Native's `.gitignore` file is already set up to ignore this `Build` directory).

```
cd <repo root>
mkdir Build
cd Build
```

**NOTE:** CMake considers what are sometimes called "build flavors" (Win32 x86 versus 
Win32 x64 versus UWP x64, etc.) to be entirely different build targets which should have
separate build folders. For cross-platform development, we commonly use multiple 
subfolders inside the Build folder, such as `Build/win32_x86` and `Build/uwp_x64`, to 
house the builds for different platforms, with each subfolder treated just like the
`Build` folder in the instructions below.

### **Building on Windows 10, Targeting Windows Desktop (Win32)**

**Required Tools:** [Visual Studio 2019](https://visualstudio.microsoft.com/vs/) with 
C++ development tools, [Python 3.0](https://www.python.org/) or newer (required by 
dependencies)

For Windows development, CMake will generate a Visual Studio Solution. From a 
terminal with access to CMake located in your `BabylonNative/Build` directory, type
the following (amend paths for alternative build directories):

```
cmake ..
```

Note that, by default, this will target the same processor architecture that is being
used to build the solution; so if your developer computer has 64-bit CPU, CMake's 
generated solution will target 64-bit CPUs by default. To manually target specific 
architectures, tell CMake the intended architecture using the `-A` flag, as shown below. 
Supported arguments for using this flag with Babylon Native include `Win32` for 32-bit 
processors and `x64` for 64-bit processors.

```
cmake -A Win32 ..
```

CMake will generate a new `BabylonNative.sln` file in your working directory. Please
be patient; this process can take several minutes. When the process is completed,
open `BabylonNative.sln` by double-clicking on it in Windows Explorer or by entering 
the following command:

```
start BabylonNative.sln
```

By default, the "Playground" demo app should be set as the Visual Studio start-up
project. Build and run this app by pressing the green "Play" button or by pressing
`F5` on your keyboard.

### **Building on Windows 10, Targeting the Universal Windows Platform (UWP)**

**Required Tools:** [Visual Studio 2019](https://visualstudio.microsoft.com/vs/) with 
C++ and UWP development tools, [Python 3.0](https://www.python.org/) or newer (required 
by dependencies)

For Windows development, CMake will generate a Visual Studio Solution. By default it 
will target Win32, so to target UWP (which CMake describes as "Windows Store") you have 
to specify certain CMake variables. From a terminal with access to CMake located in 
your `BabylonNative/Build` directory, type the following (amend paths for alternative 
build directories):

```
cmake -D CMAKE_SYSTEM_NAME=WindowsStore -D CMAKE_SYSTEM_VERSION=10.0 ..
```

Note that, by default, this will target the same processor architecture that is being
used to build the solution; so if your developer computer has 64-bit CPU, CMake's 
generated solution will target 64-bit CPUs by default. To manually target specific 
architectures, tell CMake the intended architecture using the `-A` flag, as shown below. 
Supported arguments for using this flag with Babylon Native include `Win32` for 32-bit 
processors, `x64` for 64-bit processors, `arm` for ARM processors, and `arm64` for 
ARM64 processors.

```
cmake -D CMAKE_SYSTEM_NAME=WindowsStore -D CMAKE_SYSTEM_VERSION=10.0 -A arm64 ..
```

CMake will generate a new `BabylonNative.sln` file in your working directory. Please
be patient; this process can take several minutes. When the process is completed,
open `BabylonNative.sln` by double-clicking on it in Windows Explorer or by entering 
the following command:

```
start BabylonNative.sln
```

By default, the "Playground" demo app should be set as the Visual Studio start-up
project. Build and run this app by pressing the green "Play" button or by pressing
`F5` on your keyboard.

### **Building on Windows 10, Targeting HoloLens 2**

**Required Tools:** [Visual Studio 2019](https://visualstudio.microsoft.com/vs/) with 
C++ and UWP development tools, [Python 3.0](https://www.python.org/) or newer (required 
by dependencies)

HoloLens 2 supports `arm64` UWP applications. To create a HoloLens 2 Visual Studio solution for a physical device, run the following command from the `BabylonNative/Build` directory:

```
cmake -D CMAKE_SYSTEM_NAME=WindowsStore -D CMAKE_SYSTEM_VERSION=10.0 -A arm64 ..
```

**Additional notes for HoloLens 2 development**:
* At this time, running immersive applications on **HoloLens 2 emulators** is **NOT** supported. We are tracking support for **HoloLens 2 emulators** [here](https://github.com/BabylonJS/BabylonNative/issues/448).
* For the Playground app, setting the `hololens` javascript variable to true in `experience.js` will configure the experience for **HoloLens 2**.
* HoloLens 2 immersive experiences require the `Spatial Perception` UWP capability. This capability is enabled in the application's `Package.appxmanifest`. When creating a custom BabylonNative project, one will need to enable the `Spatial Perception` UWP cability in their app's `Package.appxmanifest`. For more information on UWP capabilities, see [here](https://docs.microsoft.com/en-us/windows/uwp/packaging/app-capability-declarations).
* HoloLens 2 immersive experiences are built on top of `OpenXR`. Updates to the `OpenXR` runtime should automtically be installed on physical **HoloLens 2** devices through the Windows Store. If for some reason updates are not automatically installed (custom device configurations, lack of network connectivity, etc), developers may need to manually install updates for the `OpenXR` runtime through the Windows Store. For more information on installing newer versions of `OpenXR`, see [here](https://docs.microsoft.com/en-us/windows/mixed-reality/develop/native/openxr-getting-started).

### **Building on macOS, Targeting macOS**

**Required Tools:** [Xcode 11](https://developer.apple.com/xcode/) or newer, 
[Python 3.0](https://www.python.org/) or newer (required by dependencies)

This has been tested on MacOS Catalina (10.15).

For macOS development, CMake by default will generate a Makefile. It may be possible
to build Babylon Native for macOS using this approach, but at present only the Xcode
method is supported. To generate an Xcode project using CMake, you must specify the
correct build system generator for CMake to use, as follows:

```
cmake -G Xcode ..
```

Starting with Xcode 12, it's mandatory to set the targeted CPU architectures (X86_64 and/or arm64).

```
cmake .. -GXcode "-DCMAKE_OSX_ARCHITECTURES=x86_64;arm64"
```

CMake will generate a new `BabylonNative.xcodeproj` file in your working directory.
Please be patient; this process can take several minutes. When the process is 
completed, open the project by double-clicking on it in Finder or by entering the 
following command:

```
open BabylonNative.xcodeproj
```

To select which project to build with Xcode, select the correct project name in the 
menu to the right of the greyed-out "Stop" button adjacent to the "Play" button in
the top-left corner of the Xcode window. For example, to build and run the Playground
demo app, click on the project selector and find "Playground" in the list of possible
selections. The "Play" button will subsequently allow you to build, run, and debug
the selected Babylon Native demo app.

For macOS 11.0 Big Sur and ARM based CPU, you'll need to use XCode 12.
Also, The CMake command line is different to indicate the use of other architecture:

```
cmake -G Xcode .. "-DCMAKE_OSX_ARCHITECTURES=x86_64;arm64"
```

If CMake is not available on your platform, you'll have to clone it and build it. [CMake repo](https://gitlab.kitware.com/cmake/cmake)

### **Building on macOS, Targeting iOS**

**Required Tools:** [Xcode 11](https://developer.apple.com/xcode/) or newer, 
[Python 3.0](https://www.python.org/) or newer (required by dependencies)

This has been tested on MacOS Catalina (10.15) and iOS 13.

For macOS development, CMake by default will generate a Makefile. It may be possible
to build Babylon Native for macOS using this approach, but at present only the Xcode
method is supported. To generate an Xcode project using CMake, you must specify the
correct build system generator for CMake to use. Additionally, you must tell CMake
what toolchain to use, which provides additional information about how to generate an 
iOS Xcode project correctly. Furthermore, a number of capabilities within Babylon 
Native's dependencies must be deactivated in order for the project to build correctly;
these capabilities are set to their correct state by the additional CMake variables in 
the following command:

```
cmake -G Xcode -DCMAKE_TOOLCHAIN_FILE=../Dependencies/ios-cmake/ios.toolchain.cmake -DPLATFORM=OS64COMBINED -DENABLE_ARC=0 -DDEPLOYMENT_TARGET=12 -DENABLE_GLSLANG_BINARIES=OFF -DSPIRV_CROSS_CLI=OFF ..
```

To enable bitcode support, add this option to the cmake command line parameters:

```
-DENABLE_BITCODE=ON
```

CMake will generate a new `BabylonNative.xcodeproj` file in your working directory.
Please be patient; this process can take several minutes. When the process is 
completed, open the project by double-clicking on it in Finder or by entering the 
following command:

```
open BabylonNative.xcodeproj
```

To select which project to build with Xcode, select the correct project name in the 
menu to the right of the greyed-out "Stop" button adjacent to the "Play" button in
the top-left corner of the Xcode window. For example, to build and run the Playground
demo app, click on the project selector and find "Playground" in the list of possible
selections. The "Play" button will subsequently allow you to build, run, and debug
the selected Babylon Native demo app.

### **Building on Windows, Targeting Android**

**Required Tools:** 
[Android Studio](https://developer.android.com/studio), [Node.js](https://nodejs.org/en/download/), [Ninja](https://ninja-build.org/)

The minimal requirement target is Android 5.0 with an OpenGL ES 3.0 compatible GPU.

Only building with Android Studio is supported. CMake is not used directly. Instead, Gradle
is used for building and CMake is automatically invocated for building the native part.
An .apk that can be executed on your device or simulator is the output.

First download the latest release of Ninja, extract the binary, and add it to your system path.

Next install the Javascript engine dependencies. This is done by the Node.js npm package system.

```
cd Apps
npm install
```

Babylon Native on Android supports two Javascript engines: V8 and JavaScriptCore. V8 is 
used  by default if no engine is specified. To change the engine to JavaScriptCore, open 
the file *Apps\Playground\Android\gradle.properties* and add the following line:

```
JSEngine=jsc
```

Once the npm packages are installed, with AndroidStudio, open the project located at 
*Apps\Playground\Android*. Then in the menu, select Run->Run 'app'. If you don't have an 
Android device plugged in or no Android image in the Android simulator, that option will 
be greyed and inaccessible. Instructions and tips on how to install the simulator are
[available here](Documentation/AndroidSimulator.md).

### **Building on Ubuntu, Targeting Linux**

**Required Tools:** 
[Clang](https://clang.llvm.org/) or [GCC](https://gcc.gnu.org/)

The minimal requirement target is an OpenGL 3.3 compatible GPU. Clang 9+ or GCC 9+ are required for building.

First step is to install packages mandatory for building. For example, with Clang-9 toolchain:

```
sudo apt-get install libgl1-mesa-dev libcurl4-openssl-dev clang-9 libc++-9-dev libc++abi-9-dev lld-9 ninja-build
```

Depending on the JavaScript engine you wan't to use, you will have to install the package accordingly:

#### JavaScriptCore

Install the following package:

```
sudo apt-get install libjavascriptcoregtk-4.0-dev 
```

Then, run cmake targetting a Ninja make file:

```
cmake -GNinja -DJSCORE_LIBRARY=/usr/lib/x86_64-linux-gnu/libjavascriptcoregtk-4.0.so  -DNAPI_JAVASCRIPT_ENGINE=JavaScriptCore ..
```

#### V8

Install the following package:

```
sudo apt-get install libv8-dev 
```

Then, run cmake targetting a Ninja make file:

```
cmake -GNinja -DNAPI_JAVASCRIPT_ENGINE=V8 ..
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

## Included Components

For an overview of the major components included with the Babylon Native repository, 
please read the dedicated [Components](./Documentation/Components.md) documentation
page. Particularly important components are discussed in dedicated pages for 
[JsRuntime](./Documentation/JsRuntime.md), [AppRuntime](./Documentation/AppRuntime.md),
and [NativeEngine](./Documentation/NativeEngine.md).

## Contributing

Please read [CONTRIBUTING.md](./CONTRIBUTING.md) for details on our code of conduct, and 
the process for submitting pull requests.

## Reporting Security Issues

Security issues and bugs should be reported privately, via email, to the Microsoft 
Security Response Center (MSRC) at [secure@microsoft.com](mailto:secure@microsoft.com). 
You should receive a response within 24 hours. If for some reason you do not, please 
follow up via email to ensure we received your original message. Further information, 
including the [MSRC PGP](https://technet.microsoft.com/en-us/security/dn606155) key, can 
be found in the [Security TechCenter](https://technet.microsoft.com/en-us/security/default).
 