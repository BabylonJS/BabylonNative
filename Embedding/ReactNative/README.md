[![](https://github.com/BabylonJS/BabylonReactNative/workflows/Publish%20Package/badge.svg)](https://github.com/BabylonJS/BabylonReactNative/actions?query=workflow%3A%22Publish+Package%22)
[![npm version](https://badge.fury.io/js/%40babylonjs%2Freact-native.svg)](https://badge.fury.io/js/%40babylonjs%2Freact-native)

# Babylon React Native

Babylon React Native brings the power and flexibility of [Babylon.js](https://github.com/BabylonJS/Babylon.js) into a [React Native](https://github.com/facebook/react-native) environment via [Babylon Native](https://github.com/BabylonJS/BabylonNative).

## Current Status

### Limitations

1. Android and iOS only - support for Windows is experimental.
1. Touch input only - mouse, keyboard, and controllers are not yet supported.
1. Single view only - multiple views are not yet supported (only a single view can be displayed).

### What is Supported from Babylon.js

See [this](https://github.com/BabylonJS/BabylonNative#what-is-supported-from-babylonjs) from the Babylon Native repo.

### Performance

React Native applications targeting [JavaScriptCore on iOS](https://reactnative.dev/docs/javascript-environment#javascript-runtime) or [Hermes](https://reactnative.dev/docs/hermes) will not have JIT enabled. This may cause performance issues for JavaScript heavy scenarios.

### Other

Babylon React Native relies heavily on newer React Native constructs including JSI to get the performance characteristics required for real time rendering. JSI allows for direct synchronous communication between native code and JavaScript code, but is incompatible with "remote debugging." If you need to debug your JavaScript code that uses Babylon React Native, you should enable Hermes and use "direct debugging" (e.g. chrome://inspect or edge://inspect). See the [React Native documentation](https://reactnative.dev/docs/hermes) for more info.

## Usage

See the [package usage](Modules/@babylonjs/react-native/README.md) for installation instructions and/or the Playground app's [App.tsx](Apps/Playground/App.tsx) for example usage. You can also clone [this sample repo](https://github.com/BabylonJS/BabylonReactNativeSample) to quickly get started.

## Frameworks (Expo, ...)

Note: Official support for React Native frameworks, like Expo, is not provided by BabylonReactNative. While BabylonReactNative may work with these frameworks, we do not conduct testing, bug fixes, or feature development to ensure compatibility.

## Contributing

This quick overview will help you get started developing in the Babylon React Native repository. We support development on Windows and macOS, but assume the use of [PowerShell](https://docs.microsoft.com/en-us/powershell/scripting/install/installing-powershell) in the instructions below (unless otherwise noted).

If you are interested in making contributions, be sure to also review [CONTRIBUTING.md](CONTRIBUTING.md).

## Using BabylonReactNative

The following section will describe how to build locally a package and how to develop BabylonReactNative. If your intent is to use the plugin, follow instructions in the NPM [BabylonReactNative readme](Modules/@babylonjs/react-native/README.md).

### **Preparing a new Repo**

**Required Tools:** [git](https://git-scm.com/), [Node.js (16.13.0+)](https://nodejs.org/en/download/)

Step 1 for all development environments and targets is to clone the repo. Use a git-enabled terminal to follow the steps below.

```
git clone https://github.com/BabylonJS/BabylonReactNative
```

Run setup_dev.js script :

```
cd Package
node setup_dev.js
```

This will also automatically do the following to prepare your repo for development:

- Update git submodules to fetch Babylon Native and its dependencies
- [macOS only] Run CMake to generate the iOS Xcode project for Babylon React Native
- [macOS only] Run `pod install` to install cocoa pod dependencies

Finally run Playground Test App using:

```
cd Apps/Playground
npm run ios
```

or 

```
cd Apps/Playground
npm run android
```

### **Configuring a Mac Dev Environment**

**Required Tools:** [Android Studio](https://developer.android.com/studio/) (including NDK 23.1.7779620), [CMake](https://cmake.org/), [Ninja](https://ninja-build.org/), [JDK 13](https://www.oracle.com/java/technologies/javase-jdk13-downloads.html)

- The `PATH` environment variable must include the path to adb (typically ~/Library/Android/sdk/platform-tools/).
- The `PATH` environment variable must include the path to Ninja, or Ninja must be [installed via a package manager](https://github.com/ninja-build/ninja/wiki/Pre-built-Ninja-packages).
- The `ANDROID_HOME` environment variable must be defined (typically ~/Library/Android/sdk).
- The `ANDROID_SDK_ROOT` environment variable must be defined (typically ~/Library/Android/sdk).
- The `ANDROID_AVD_HOME` environment variable must be defined if you plan to use Android emulators (typically ~/.android/avd).
- The `JAVA_HOME` environment variable must be defined to point to the correct version of the JDK (typically /usr/libexec/java_home -v 13).

You can typically configure your environment by editing `~/.zshrc` and adding the following:

```
export PATH=$PATH:~/Library/Android/sdk/platform-tools/
export PATH=$PATH:~/path_to_ninja_binary/ # Only for manual installations of Ninja (not package manager-based installations).
export ANDROID_HOME=~/Library/Android/sdk
export ANDROID_SDK_ROOT=~/Library/Android/sdk
export ANDROID_AVD_HOME=~/.android/avd
export JAVA_HOME=$(/usr/libexec/java_home -v 13)
```

### **Configuring a Windows Dev Environment**

**Required Tools:** [Android Studio](https://developer.android.com/studio/) (including NDK 23.1.7779620), [CMake](https://cmake.org/), [Ninja](https://ninja-build.org/), [Visual Studio 2022](https://visualstudio.microsoft.com/vs/)

- The `PATH` environment variable must include the path to adb (typically %LOCALAPPDATA%/Android/sdk/platform-tools/).
- The `PATH` environment variable must include the path to Ninja, or Ninja must be [installed via a package manager](https://github.com/ninja-build/ninja/wiki/Pre-built-Ninja-packages).  
- The `ANDROID_HOME` environment variable must be defined (typically %LOCALAPPDATA%/Android/sdk).
- The `JAVA_HOME` environment variable must be defined (typically %ProgramFiles%/Android/Android Studio/jre).

### **Configuring a Linux Dev Environment**

**Required Tools:** [Android Studio](https://developer.android.com/studio/) (including NDK 23.1.7779620)

With Ubuntu, you can install needed packages by this command:

```
sudo apt-get install adb ninja-build openjdk-14-jdk android-sdk
```

Update PATH with these commands:

```
export ANDROID_HOME=$HOME/Android/Sdk
export PATH=$PATH:$ANDROID_HOME/emulator
export PATH=$PATH:$ANDROID_HOME/tools
export PATH=$PATH:$ANDROID_HOME/tools/bin
export PATH=$PATH:$ANDROID_HOME/platform-tools
```

### **Troubleshooting**
- If the Metro server is not started with `npm run android`, you can start it manually by running `npm run start` in a terminal.
- Android Studio is the tool of choice for downloading the various versions of NDK.
- If something goes wrong with the build `npm run android --verbose` can give some hints.
- If the emulator is not launched by the build, you can run `~/Android/Sdk/emulator/emulator @name_of_your_image`.
- For other emulator issues, follow the [instructions](https://github.com/BabylonJS/BabylonNative/blob/master/Documentation/AndroidEmulator.md) from Babylon Native documentation.
- Refer to the [Babylon Native documentation](https://github.com/BabylonJS/BabylonNative/tree/master/Documentation#babylon-native-documention) for additional information that may help troubleshoot issues.

#### Universal Windows Platform (UWP)

UWP can only be built on a PC. `CMake` must be manually run to generate project definitions for BabylonNative dependencies.

1. Run `npm ci` in Apps\Playground\Playground.
1. Run `npm ci` in Package.
1. Run `npx gulp buildUWP` in Package. This command will run cmake and build BabylonNative dependencies. It may take a while to complete.
1. In Apps\Playground\Playground, run `npm run windows`.
> Note: if you experience build issues for Apps\Playground related to autolinking, try running `npx react-native autolink-windows` in the Apps\Playground\Playground folder. You can also run `npm run windows-verbose` to view logging.

### **Testing in the Playground App** ###

When making local changes, the following manual test steps should be performed within the Playground app to prevent regressions. These should be checked on Android and iOS, and ideally in both debug and release, but minimally in release.

1. **Basic rendering** - launch the Playground app and make sure the model loaded and is rendering at 60fps.
1. **Animation** - make sure the loaded model is animating.
1. **Input handling** - swipe across the display and make sure the model rotates around the y-axis.
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

Prepare tools:

```
cd Package
npm install
```

Prepare package dependencies
```
cd ../Modules/@babylonjs/react-native
export BABYLON_NO_CMAKE_POSTINSTALL=1
npm install
```

Build BabylonNative source tree, build TypeScript copy mandatory files:

```
cd Package
npx gulp buildAssembled
```

Package content is in `Package/Assembled` folder. 
Run `npm pack` to make a .tgz.

## Supported Versions

Package versions are listed on this npm.js page: [@babylonjs/react-native](https://www.npmjs.com/package/@babylonjs/react-native). Check supported Babylon.js version and corresponding BabylonNative dependency in package [README](Modules/@babylonjs/react-native/README.md)

## Security

If you believe you have found a security vulnerability in this repository, please see [SECURITY.md](SECURITY.md).