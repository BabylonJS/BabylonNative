[![Build Status](https://dev.azure.com/babylonjs/ContinousIntegration/_apis/build/status/BabylonNative%20CI?branchName=master)](https://dev.azure.com/babylonjs/ContinousIntegration/_build/latest?definitionId=6&branchName=master)

# Babylon Native

Build cross-platform native applications with the power of the Babylon.js JavaScript framework.

See [this blog entry](https://medium.com/@babylonjs/babylon-native-821f1694fffc) for more details.

*This project is under heavy development. Not all intended platforms are currently implemented. **DO NOT** use in production code.*

## Getting Started

### Windows

#### Prerequisites

- [CMake](https://cmake.org/download/) 3.12 or higher
- [Python](https://www.python.org/downloads/) 3.x
- [Visual Studio](https://visualstudio.microsoft.com/downloads/) 2019 (16.3 or higher)

#### Build

- Clone this repo.
- Update the submodules.
  ```
  C:\BabylonNative>git submodule update --init --recursive
  ```
- Create a new directory for the build files, e.g. `Build` at the root of the repo.
  ```
  C:\BabylonNative>mkdir Build
  ```
- Change your working directory to the new directory.
  ```
  C:\BabylonNative>cd Build
  ```
- Run CMake from the new directory and point to the root of the repo.

    NOTE: If you get the CMake error *No preprocessor test for "Intel"*, update your Visual Studio to 16.3 or higher.
  ```
  C:\BabylonNative\Build>cmake ..
  ```
- Open the generated solution `BabylonNative.sln`.
  ```
  C:\BabylonNative\Build>start BabylonNative.sln
  ```
- Once the solution opens, right click the TestApp project, click "Set as StartUp Project" and click the play button at the top to run the test app which should render a box

### Android / iOS / MacOS

*Planned but not yet implemented*

## Development Notes

### glslang and SPIRV-Cross

In order to compile the WebGL GLSL shader to the required bits for the target platform, this project utilizes [glslang](https://github.com/KhronosGroup/glslang) and [SPIRV-Cross](https://github.com/KhronosGroup/SPIRV-Cross). See [ShaderCompiler.h](./Library/Source/ShaderCompiler.h) and its corresponding implementation for details.

### arcana.cpp

This project makes substantial use of the utilities contained within the [arcana.cpp](https://github.com/microsoft/arcana.cpp) project, especially the support for asynchronous task execution and thread synchronization.

### N-API

This project uses a subset of [node-addon-api](https://github.com/nodejs/node-addon-api) and the JavaScript part of [N-API](https://github.com/nodejs/node/blob/master/src/js_native_api.h) to target either V8 or Chakra. See [this thread](https://github.com/nodejs/abi-stable-node/issues/354) for some context. There is also [work](https://github.com/nodejs/node-addon-api/issues/399) needed to factor out the JavaScript part of node-addon-api.

The code is located [here](./Library/Dependencies/napi). Some small modifications were made to avoid node dependencies and improve performance. The Chakra version [js_native_api_chakra.cc](./Library/Dependencies/napi/source/js_native_api_chakra.cc) came from [node_api_jsrt.cc](https://github.com/nodejs/node-chakracore/blob/master/src/node_api_jsrt.cc) and was modified to target Chakra directly. We will work on submitting these changes to the public version.

### bgfx

This project uses [bgfx](https://github.com/bkaradzic/bgfx) for the cross-platform rendering abstraction. It does not use the shader abstraction of bgfx, but instead [compiles the WebGL GLSL shader at runtime](#glslang-and-SPIRV-Cross) and generates the shader header that bgfx expects. See [NativeEngine.cpp](./Library/Source/NativeEngine.cpp) for implementation details.

### base-n

This project uses [base-n](https://github.com/azawadzki/base-n) to implement base64 decoding for parsing data URLs.

### curl

This project uses [curl](https://curl.haxx.se/) (or, more accurately, [libcurl](https://curl.haxx.se/libcurl/)) as the backend for the provided implementation of XMLHttpRequest. At present, only a "golden path" is supported, but additional features will be added as they are required.

## Contributing

Please read [CONTRIBUTING.md](./CONTRIBUTING.md) for details on our code of conduct, and the process for submitting pull requests.

## Reporting Security Issues

Security issues and bugs should be reported privately, via email, to the Microsoft Security
Response Center (MSRC) at [secure@microsoft.com](mailto:secure@microsoft.com). You should
receive a response within 24 hours. If for some reason you do not, please follow up via
email to ensure we received your original message. Further information, including the
[MSRC PGP](https://technet.microsoft.com/en-us/security/dn606155) key, can be found in
the [Security TechCenter](https://technet.microsoft.com/en-us/security/default).

## Android projects

2 Android Studio projects are available:
- A Java app using JNI (Android folder)
- A native activity app without Java (AndroidNativeActivity folder)

Both projects build with CI. The native activity app needs work to be runnable. Mainly .so packaging in APK
The Java app runs and supports screen rotation

## How to build and run TestApp on Android

- Checkout branch https://github.com/CedricGuillemet/BabylonNative/tree/AndroidRender
- Download and unzip Ninja (see [azure-pipelines.yml](azure-pipelines.yml#L226)) and **add it to the system path**
- Download V8 npm, untar/unxz and copy headers and .so into the expected folders (see see [azure-pipelines.yml](azure-pipelines.yml#L231))
- Download and install [Android Studio](https://developer.android.com/studio/) with the default installation configuration
- Open project with Android Studio (TestApp\Source\Android)
- Install the NDK by opening the SDK Manager (box with a down arrow icon in the upper right), checking the *NDK (Side by side)* item, then clicking *Apply*
- Sync the project with Gradle via *File->Sync Project with Gradle Files*
- Connect your Android device (Android 5.0+ with OpenGL ES 3.0+) with developer option enabled
- In Android Studio, click the *Run->Run App* menu item and select your device if prompted
- Wait 2-3 minutes for a complete build and APK installation
- App should be running and display a cube. You can rotate the cube by touching the screen

### Using the Android Emulator

- Install HAXM by again opening the SDK Manager, checking *Intel x86 Emulator Accelerator (HAXM installer)*, then clicking *Apply*. If the installer fails, see [HAXM Installation Troubleshooting](#HAXM-Installation-Troubleshooting).
- Create an Android Emulator through the AVD Manager (Android phone icon in the upper right). Pixel 2 with API 27 has been tested.
- In Android Studio, click the *Run->Run App* menu item and select your device if prompted.

### HAXM Installation Troubleshooting

The HAXM installer can fail for a number of reasons. Many solutions are discussed in [HAXM issue 105](https://github.com/intel/haxm/issues/105), including:

- Enabling Intel virtualization in the bios
- Disabling Hyper-V in Windows
- Disabling memory integrity in Windows
- Disabling virtualization security with the Device Guard and Credential Guard hardware readiness tool

## Android Known issues/ improvements

https://github.com/BabylonJS/BabylonNative/issues/102
