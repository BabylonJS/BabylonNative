[![](https://github.com/BabylonJS/BabylonReactNative/workflows/Publish%20Package/badge.svg)](https://github.com/BabylonJS/BabylonReactNative/actions?query=workflow%3A%22Publish+Package%22)
[![npm version](https://badge.fury.io/js/%40babylonjs%2Freact-native.svg)](https://badge.fury.io/js/%40babylonjs%2Freact-native)

# Babylon React Native

Babylon React Native brings the power and flexibility of [Babylon.js](https://github.com/BabylonJS/Babylon.js) into a [React Native](https://github.com/facebook/react-native) environment via [Babylon Native](https://github.com/BabylonJS/BabylonNative).

## Current Status

### Limitations

1. Android and iOS are supported; support for Windows is experimental.
1. Touch input is supported on Android and iOS; Windows also supports mouse input. Keyboard and controller input are not yet supported.
1. Single view only - multiple views are not yet supported (only a single view can be displayed).

### What is Supported from Babylon.js

See [this](https://github.com/BabylonJS/BabylonNative#what-is-supported-from-babylonjs) from the Babylon Native repo.

### Performance

React Native applications targeting [JavaScriptCore on iOS](https://reactnative.dev/docs/javascript-environment#javascript-runtime) or [Hermes](https://reactnative.dev/docs/hermes) will not have JIT enabled. This may cause performance issues for JavaScript heavy scenarios.

### Other

Babylon React Native uses JSI for direct synchronous communication between native code and JavaScript code. Debugging must keep JavaScript running on the device: browser-based remote JavaScript debugging is incompatible with JSI. For Hermes applications, use [React Native DevTools](https://reactnative.dev/docs/0.81/react-native-devtools).

## Supported Versions

The latest stable npm release is **[@babylonjs/react-native 2.0.7](https://www.npmjs.com/package/@babylonjs/react-native/v/2.0.7)**. Since 2.0.0, a single package contains the sources for Android, iOS, and Windows; separate platform packages are no longer needed.

2.0.7 updates Babylon Native to [f4d9efbeeabcfec24105373b99e54fe147669fc1](https://github.com/BabylonJS/BabylonNative/commit/f4d9efbeeabcfec24105373b99e54fe147669fc1), including its JsRuntimeHost dependency update, and removes obsolete `minz` library links from the iOS and Windows builds. The Babylon.js and React Native baselines are unchanged from 2.0.6.

| Package version | Babylon.js peer dependency | React Native (Android / iOS) | React Native Windows |
| --------------- | -------------------------- | ---------------------------- | -------------------- |
| 2.0.7 / master  | >=9.0.0                    | 0.81.6                       | 0.81.35              |
| 2.0.6           | >=9.0.0                    | 0.81.6                       | 0.81.35              |

The React Native versions above are the resolved Playground lockfile baselines, not a guarantee of compatibility with every newer release. For 2.0.6 and 2.0.7, the Windows Playground locks React Native 0.81.6 with React Native Windows 0.81.35; 0.81.4 is only the lower bound of the manifest's `^0.81.4` React Native range. Windows support remains experimental; its npm version is pinned to match the published React Native Windows NuGet packages.

See the [package README](Modules/@babylonjs/react-native/README.md#supported-versions) for earlier releases and the Babylon Native commit used by each release, and the [release notes](https://github.com/BabylonJS/BabylonReactNative/releases) for changes.

## Usage

See the [package usage](Modules/@babylonjs/react-native/README.md) for installation instructions and/or the Playground app's [App.tsx](Apps/Playground/App.tsx) for example usage. You can also clone [this sample repo](https://github.com/BabylonJS/BabylonReactNativeSample) to quickly get started.

## Frameworks (Expo, ...)

Note: Official support for React Native frameworks, like Expo, is not provided by BabylonReactNative. While BabylonReactNative may work with these frameworks, we do not conduct testing, bug fixes, or feature development to ensure compatibility.

## Contributing

This quick overview will help you get started developing in the Babylon React Native repository. Android development is possible on Windows, macOS, and Linux; iOS requires macOS, and Windows builds require Windows. The cross-platform commands below use [PowerShell](https://learn.microsoft.com/en-us/powershell/scripting/install/installing-powershell) unless otherwise noted.

If you are interested in making contributions, be sure to also review [CONTRIBUTING.md](CONTRIBUTING.md).

## Using BabylonReactNative

The following section will describe how to build locally a package and how to develop BabylonReactNative. If your intent is to use the plugin, follow instructions in the NPM [BabylonReactNative readme](Modules/@babylonjs/react-native/README.md).

### **Preparing a new Repo**

**Required Tools:** [git](https://git-scm.com/), [Node.js (20.19.4+)](https://nodejs.org/en/download/), and [CMake](https://cmake.org/) on `PATH`. The Windows CI builds use Node.js 22. Install the platform-specific tools described below before running setup.

Step 1 for all development environments and targets is to clone the repo. Use a git-enabled terminal to follow the steps below.

```powershell
git clone https://github.com/BabylonJS/BabylonReactNative
cd BabylonReactNative
```

From a fresh checkout, run the developer setup script:

```powershell
cd Package
node setup_dev.js
```

This will also automatically do the following to prepare your repo for development:

- Install the build tools, Playground dependencies, and module dependencies
- Download the Babylon Native commit pinned in `Package/gulpfile.js` and use CMake to assemble its dependency sources (no git submodule setup is required)
- Compile the TypeScript module and copy the generated files back into the local module
- [macOS only] Run CMake to generate the iOS Xcode project for Babylon React Native

The Playground uses `react-native-test-app`. From `Package`, launch Android with:

```powershell
cd ..\Apps\Playground
npx react-native run-android
```

For iOS, install the CocoaPods dependencies separately before launching (from `Package`, on macOS):

```powershell
cd ..\Apps\Playground\ios
pod install
```

Then open `Apps\Playground\ios\BRNPlayground.xcworkspace` in Xcode, select the `Playground` scheme, and run it on an iOS device or simulator. Use this workspace rather than the CLI-selected `Playground.xcworkspace`: it includes the CMake-generated Babylon Native project needed to build and link the native libraries.

For Windows, use the manifest preparation and project generation steps in the [Windows section](#universal-windows-platform-uwp) instead.

### **Configuring a Mac Dev Environment**

For iOS, install [Xcode](https://developer.apple.com/xcode/) with its command line tools and [CocoaPods](https://cocoapods.org/). The current module requires **iOS 16.0 or later** and includes fixes for Xcode 26.

For Android, install [Android Studio](https://developer.android.com/studio/), [Ninja](https://ninja-build.org/), and **JDK 17**. Follow the [React Native 0.81 environment setup](https://reactnative.dev/docs/0.81/set-up-your-environment) and install the SDK, build tools, and NDK selected by the app's Gradle configuration rather than the older NDK 23 toolchain.

Add the following to `~/.zshrc` for Android development (these commands use zsh, not PowerShell):

```sh
export ANDROID_HOME="$HOME/Library/Android/sdk"
export PATH="$PATH:$ANDROID_HOME/emulator:$ANDROID_HOME/platform-tools"
export JAVA_HOME=$(/usr/libexec/java_home -v 17)
```

Ensure CMake and Ninja are also on `PATH`.

### **Configuring a Windows Dev Environment**

For Android, install [Android Studio](https://developer.android.com/studio/), [Ninja](https://ninja-build.org/), and **JDK 17**. Follow the [React Native 0.81 environment setup](https://reactnative.dev/docs/0.81/set-up-your-environment) for SDK and NDK installation.

For Windows/UWP builds, install [Visual Studio 2022](https://visualstudio.microsoft.com/vs/) with the C++ and UWP development tools. The current CI uses Windows SDK **10.0.22621.0**.

- Add CMake, Ninja, and `%LOCALAPPDATA%\Android\Sdk\platform-tools` to `PATH` as appropriate for your target.
- Set `ANDROID_HOME` to your SDK location (typically `%LOCALAPPDATA%\Android\Sdk`).
- Set `JAVA_HOME` to your JDK 17 installation, not an obsolete Android Studio `jre` directory.

### **Configuring a Linux Dev Environment**

Linux can be used for Android development. Install [Android Studio](https://developer.android.com/studio/) and follow the [React Native 0.81 environment setup](https://reactnative.dev/docs/0.81/set-up-your-environment) for the SDK and NDK.

On Ubuntu, install the additional build tools with:

```sh
sudo apt-get install ninja-build openjdk-17-jdk cmake
```

Configure your shell with your SDK and JDK paths:

```sh
export ANDROID_HOME="$HOME/Android/Sdk"
export PATH="$PATH:$ANDROID_HOME/emulator:$ANDROID_HOME/platform-tools"
export JAVA_HOME=/usr/lib/jvm/java-17-openjdk-amd64
```

Adjust `JAVA_HOME` for your distribution and architecture.

### **Troubleshooting**
- If Metro is not started with `npx react-native run-android`, run `npm start` from `Apps\Playground` in another terminal.
- Android Studio is the tool of choice for downloading the various versions of NDK.
- If something goes wrong with the Android build, `npx react-native run-android --verbose` can give some hints.
- Use an `x86_64` or `arm64-v8a` Android emulator. The 32-bit `x86` ABI is not supported and is filtered out by the module's Gradle configuration.
- See the [package's Android configuration](Modules/@babylonjs/react-native/README.md#android-configuration) for ARCore version overrides and the [iOS configuration](Modules/@babylonjs/react-native/README.md#ios-configuration) for CMake and deployment target overrides.
- If the emulator is not launched by the build, you can run `~/Android/Sdk/emulator/emulator @name_of_your_image`.
- For other emulator issues, follow the [instructions](https://github.com/BabylonJS/BabylonNative/blob/master/Documentation/AndroidEmulator.md) from Babylon Native documentation.
- Refer to the [Babylon Native documentation](https://github.com/BabylonJS/BabylonNative/tree/master/Documentation#babylon-native-documention) for additional information that may help troubleshoot issues.

#### Universal Windows Platform (UWP)

Windows support is experimental and currently uses a UWP consumer with the legacy renderer, Hermes, and React Native Windows NuGet packages. From the repository root, prepare the Windows-specific Playground manifests **before** running developer setup:

```powershell
Copy-Item Apps\Playground\package-windows.json Apps\Playground\package.json -Force
Copy-Item Apps\Playground\package-lock-windows.json Apps\Playground\package-lock.json -Force
cd Package
node setup_dev.js
cd ..\Apps\Playground
node generate-windows-test-app.mjs
```

These copies change tracked files locally; keep the Windows manifests and lockfiles in sync when intentionally updating dependencies. React Native Windows is pinned to **0.81.35** to match its published NuGet packages.

Project generation enables autolinking and NuGet mode. The module's MSBuild project now generates and builds its embedded Babylon Native sources with CMake and reuses the app's C++/WinRT projections; there is no `gulp buildUWP` task.

Use the [Windows source-build workflow](.github/workflows/windows.yml) as the reference for the current x64 Release build, including `UseExperimentalNuget=true`, `ReactNativeNuGet.targets`, and the generated test-app C++20 settings. The [packaged-consumer workflow](.github/workflows/test_windows.yml) checks the same integration using the npm tarball.

### **Testing in the Playground App** ###

When making local changes, the following manual test steps should be performed within the Playground app to prevent regressions. These should be checked on Android and iOS, and ideally in both debug and release, but minimally in release. Run the XR checks on compatible physical ARCore/ARKit devices with camera permissions enabled.

1. **Basic rendering** - launch the Playground app and make sure the model loaded and is rendering at 60fps.
1. **Animation** - make sure the loaded model is animating.
1. **Input handling** - swipe across the display (or drag with the left mouse button on Windows) and make sure the model rotates around the y-axis.
1. **Display rotation** - rotate the device 90 degrees and make sure the view rotates and renders correctly.
1. **View replacement** - tap the *Toggle EngineView* button twice to replace the render target view.
1. **Engine dispose** - tap the *Toggle EngineScreen* button twice to dispose and re-instantiate the Babylon engine.
1. **Suspend/resume** - switch to a different app and then back to the Playground and make sure it is still rendering correctly.
1. **Fast refresh** (debug only) - save the App.tsx file to trigger a fast refresh.
1. **Dev mode reload** (debug only) - in the Metro server console window, press the `R` key on the keyboard to reload the JS engine and make sure rendering restarts successfully.
1. **XR mode** - tap the *Start XR* button and make sure XR mode is working.
1. **XR display rotation** - rotate the device 90 degrees and make sure the view rotates and renders correctly.
1. **XR view replacement** - tap the *Toggle EngineView* button twice to replace the render target view.
1. **XR suspend/resume** - switch to a different app and then back to the Playground and make sure it is still rendering correctly.

### **Building the NPM Package**

Every change for a Pull Request will trigger the build of the NPM package. The artifact is named `BabylonReactNative` and can be found in the Artifact section of the corresponding GitHub Action.

> :warning: The zip contains a Tarball Gzipped `.tgz` that is automatically decompressed on macOS. When installing that directory ( `npm install /path/to/unzipped/untarred/package` ) npm creates symlinks that are not supported by the build system. Installing the .tgz instead of its decompressed version works correctly.

To get a `.tgz` that can be shared or published, follow these steps:

From a fresh checkout, install the packaging tools:

```powershell
cd Package
npm ci
```

Install the module dependencies, skipping iOS project generation until the native sources have been assembled:

```powershell
cd ..\Modules\@babylonjs\react-native
$env:BABYLON_NO_CMAKE_POSTINSTALL = "1"
npm ci
Remove-Item Env:\BABYLON_NO_CMAKE_POSTINSTALL
```

In a macOS/Linux shell, use `export BABYLON_NO_CMAKE_POSTINSTALL=1` and `unset BABYLON_NO_CMAKE_POSTINSTALL` instead of the PowerShell environment commands.

Download and assemble the Babylon Native source tree, copy the package files, compile TypeScript, and validate the assembled contents:

```powershell
cd ..\..\..\Package
npx gulp buildAssembled
cd Assembled
npm pack
```

Package content and the resulting `.tgz` are in `Package\Assembled`. The module's source `package.json` uses a development placeholder version; the [publish workflow](.github/workflows/publish.yml) sets the package version from the release tag before publishing.

The npm README is copied from `Modules\@babylonjs\react-native\README.md` by `copyCommonFiles` in `Package\gulpfile.js`. Update that file for package usage changes; npm receives the updated README when a new package version is published.

## Security

If you believe you have found a security vulnerability in this repository, please see [SECURITY.md](SECURITY.md).