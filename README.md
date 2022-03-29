[![Build Status](https://dev.azure.com/babylonjs/ContinousIntegration/_apis/build/status/BabylonNative%20CI?branchName=master)](https://dev.azure.com/babylonjs/ContinousIntegration/_build/latest?definitionId=6&branchName=master)
[![Nightly build](https://github.com/BabylonJS/BabylonNative/workflows/Nightly%20build/badge.svg)](https://github.com/BabylonJS/BabylonNative/actions?query=workflow%3A%22Nightly+build%22)

# Babylon Native

Welcome! Babylon Native is a collection of technologies intended to bring 
the power and flexibility of Babylon.js to cross-platform applications beyond the 
browser. The goal of this project is to allow the same JavaScript that powers Babylon.js 
apps on the Web to work identically in native apps on Windows, macOS, iOS, Android, and 
Linux.

Check out the [home page](https://aka.ms/Bnative) for a project overview.

## Quick Links

- [Project Status](project-status)
  - [Supported Features]()
- [Building](BUILDING.md#building)
  - [All Development Platforms](BUILDING.md#all-development-platforms) (_Do this first!_)
  - [Building on Windows 10, Targeting Windows Desktop (Win32)](BUILDING.md#building-on-windows-10-targeting-windows-desktop-win32)
  - [Building on Windows 10, Targeting Universal Windows Platform (UWP)](BUILDING.md#building-on-windows-10-targeting-universal-windows-platform-uwp)
  - [Building on Windows 10, Targeting HoloLens 2](BUILDING.md#building-on-windows-10-targeting-hololens-2)
  - [Building on macOS, Targeting macOS](BUILDING.md#building-on-macos-targeting-macos)
  - [Building on macOS, Targeting iOS](BUILDING.md#building-on-macos-targeting-ios)
  - [Building on Windows, Targeting Android](BUILDING.md#building-on-windows-targeting-android)
  - [Building on Ubuntu, Targeting Linux](BUILDING.md#building-on-ubuntu-targeting-linux)
  - [Selecting the Graphics API](BUILDING.md#selecting-the-graphics-api)
- [Documentation](Documentation/Readme.md)
  - [FAQ](Documentation/faq.md)
  - [Build System](Documentation/BuildSystem.md)
  - [Continuous Integration](ContinuousIntegration.md)
  - [Extending Babylon Native](Documentation/Extending.md)
  - [Babylon Native Components](Documentation/Components.md)
  - [Android Emulator Tips](Documentation/AndroidEmulator.md)
  - [Debugging JavaScript on Mac and iOS](DebugJavascriptMacIOS.md)

## Project Status

**TODO: rewrite**

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

If you have any questions, please reach out
to us on [the Babylon forum](https://forum.babylonjs.com).

## Team Stories
- [Babylon Native](https://medium.com/@babylonjs/babylon-native-821f1694fffc)
- [Babylon Native: The Journey So Far](https://babylonjs.medium.com/babylon-native-the-journey-so-far-7dc44c049d90)
- [Babylon React Native: Bringing 3D and XR to React Native Applications](https://babylonjs.medium.com/babylon-react-native-bringing-3d-and-xr-to-react-native-applications-7928b55acc85)
- [Babylon Native: Continuing the Journey](https://babylonjs.medium.com/babylon-native-the-continuing-the-journey-e00a7dc8157b)
- [Audio in Babylon Native — Charting the way](https://babylonjs.medium.com/audio-in-babylon-native-charting-the-way-c6bc6d5c0d74)
- [A Mysterious Case of Skinned Mesh Disappearances](https://babylonjs.medium.com/a-mysterious-case-of-skinned-mesh-disappearances-5fee23dd9cd6)

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
