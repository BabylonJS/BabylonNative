[![Build Status](https://dev.azure.com/babylonjs/ContinousIntegration/_apis/build/status/BabylonNative%20CI?branchName=master&label=Main%20branch)](https://dev.azure.com/babylonjs/ContinousIntegration/_build/latest?definitionId=6&branchName=master)
[![Nightly Build](https://dev.azure.com/babylonjs/ContinousIntegration/_apis/build/status%2FBabylonNative%20Nightly%20CI?branchName=master&label=Nightly)](https://dev.azure.com/babylonjs/ContinousIntegration/_build/latest?definitionId=36&branchName=master)

# Babylon Native

Welcome! Babylon Native is a collection of technologies intended to bring 
the power and flexibility of Babylon.js to cross-platform applications beyond the 
browser. The goal of this project is to allow the same JavaScript that powers Babylon.js 
apps on the Web to work identically in native apps on Windows, macOS, iOS, Android, and 
Linux.

Check out the [home page](https://aka.ms/Bnative) for a project overview.

## Quick Links

- [Project Status](#project-status)
  - [What is Supported from Babylon.js](#what-is-supported-from-babylonjs)
- [Building](BUILDING.md#building)
  - [All Development Platforms](BUILDING.md#all-development-platforms) (_Do this first!_)
  - [Building on Windows 10, Targeting Windows Desktop (Win32)](BUILDING.md#building-on-windows-10-targeting-windows-desktop-win32)
  - [Building on Windows 10, Targeting Universal Windows Platform (UWP)](BUILDING.md#building-on-windows-10-targeting-universal-windows-platform-uwp)
  - [Building on Windows 10, Targeting HoloLens 2](BUILDING.md#building-on-windows-10-targeting-hololens-2)
  - [Building on macOS, Targeting macOS](BUILDING.md#building-on-macos-targeting-macos)
  - [Building on macOS, Targeting iOS](BUILDING.md#building-on-macos-targeting-ios)
  - [Building on macOS, Targeting visionOS](BUILDING.md#building-on-macos-targeting-visionos)
  - [Building on Windows, Targeting Android](BUILDING.md#building-on-windows-targeting-android)
  - [Building on Ubuntu, Targeting Linux](BUILDING.md#building-on-ubuntu-targeting-linux)
  - [Selecting the Graphics API](BUILDING.md#selecting-the-graphics-api)
- [Documentation](Documentation/Readme.md)
  - [When to Use Babylon Native](Documentation/WhenToUseBabylonNative.md)
  - [Babylon Native Components](Documentation/Components.md)
  - [Extending Babylon Native](Documentation/Extending.md)
  - [Build System](Documentation/BuildSystem.md)
  - [Continuous Integration](Documentation/ContinuousIntegration.md)
  - [Android Emulator Tips](Documentation/AndroidEmulator.md)
  - [Debugging JavaScript on Mac and iOS](Documentation/DebugJavascriptMacIOS.md)
  - [Debugging iOS and MacOS rendering using Xcode](Documentation/DebugRenderedFrameMetal.md)
  - [Consuming JavaScript code in Babylon Native](Documentation/ConsumingJavaScript.md)
  - [FAQ](Documentation/FAQ.md)

## Project Status

Babylon Native is currently available as a public preview in source form only. While many
features are supported and functional, there are still features that are under active
development. Please note that this project is not at the point where updates are fully
backward compatible yet, and thus the contract for consuming Babylon Native can still
and probably will change in the future.

### What is Supported from Babylon.js

Babylon.js has many, many features; and while the goal of Babylon Native is to
support an overwhelming majority of those features, not all parts of Babylon.js
are equally well supported by Babylon Native at this time. This section is
intended to help readers "eyeball it," checking quickly whether a given type of
feature is likely supported by Babylon Native at this time. For more precise
information about current and upcoming support for particular features, please
don't hesitate to ask us on [the forum](https://forum.babylonjs.com/c/questions/).

#### Supported

Many core features from Babylon.js are supported. Here is a non-exhaustive list.

- 3D Assets (glTF, OBJ, etc.)
- Animations
- Behaviors
- Cameras
- Gizmos
- Materials
- Meshes
- Lights
- Physics
- Scenes
- Shaders
- Sprites
- Textures loaded from a PNG or JPEG
- XR

#### Partially Supported

The following are features that are not completely finished yet or have gaps.

- Environment (only `.env` supported)
- GUI (text rendering experimentally supported)
- Input (only single pointer supported)
- Instancing (only thin instances supported)
- Post Processing (some are supported)
- Media Devices (e.g., Webcams) (experimental)

#### Not Yet Supported

The following are features that don't work right now but will work in the future.

- Audio
- Serializers
- Particles
- Texture Loaders (KTX, DDS, etc.)

#### No Plan to Support

The following are features that depend on features (e.g. HTML) that are outside
the scope of what this project will support.

- Inspector
- Node Material Editor
- GUI Editor
- Performance Profiler
- 2D Controls

## Team Stories
- [Babylon Native](https://medium.com/@babylonjs/babylon-native-821f1694fffc)
- [Babylon Native: The Journey So Far](https://babylonjs.medium.com/babylon-native-the-journey-so-far-7dc44c049d90)
- [Babylon React Native: Bringing 3D and XR to React Native Applications](https://babylonjs.medium.com/babylon-react-native-bringing-3d-and-xr-to-react-native-applications-7928b55acc85)
- [Babylon Native: Continuing the Journey](https://babylonjs.medium.com/babylon-native-the-continuing-the-journey-e00a7dc8157b)
- [Audio in Babylon Native — Charting the way](https://babylonjs.medium.com/audio-in-babylon-native-charting-the-way-c6bc6d5c0d74)
- [A Mysterious Case of Skinned Mesh Disappearances](https://babylonjs.medium.com/a-mysterious-case-of-skinned-mesh-disappearances-5fee23dd9cd6)
- [A Babylon Native Backstage Tour](https://babylonjs.medium.com/a-babylon-native-backstage-tour-f9004bebc7fb)

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
