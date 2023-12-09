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

## Contributing

This quick overview will help you get started developing in the Babylon React Native repository. We support development on Windows and MacOS, but assume the use of [PowerShell](https://docs.microsoft.com/en-us/powershell/scripting/install/installing-powershell) in the instructions below (unless otherwise noted).

If you are interested in making contributions, be sure to also review [CONTRIBUTING.md](CONTRIBUTING.md).

### **Preparing a new Repo**

**Required Tools:** [git](https://git-scm.com/), [Node.js (16.13.0+)](https://nodejs.org/en/download/)

Step 1 for all development environments and targets is to clone the repo. Use a git-enabled terminal to follow the steps below.

```
git clone https://github.com/BabylonJS/BabylonReactNative
```

Then, a React Native target must be chosen. For a React-Native 0.64 build:

```
cd Apps/Playground
npm ci
npm run select 0.64
```
Selecting the React Native install will also install all NPM packages and its dependencies for the Playground sample/test app.

> :warning: When setting up the Playground app for UWP, the `npm run select` command must be run with Administrator rights, because of the symbolic links. Not doing so will result in this build error will popup when trying to run the Playground:
```
 The path cannot be traversed because it contains an untrusted mount point.
 ```

This will also automatically do the following to prepare your repo for development:

- Update git submodules to fetch Babylon Native and its dependencies
- [MacOS only] Run CMake to generate the iOS XCode project for Babylon React Native
- [MacOS only] Run `pod install` to install cocoa pod depdendencies

After merging upstream changes in the future, you will need to either run `npm install` again, or run individual commands for the above operations (e.g. `git submodule update --init --submodule` / `npm run iosCMake` / `pod install`).

### **Configuring a Mac Dev Environment**

**Required Tools:** [Android Studio](https://developer.android.com/studio/) (including NDK 21.4.7075529), [CMake](https://cmake.org/), [Ninja](https://ninja-build.org/), [JDK 13](https://www.oracle.com/java/technologies/javase-jdk13-downloads.html)

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

**Required Tools:** [Android Studio](https://developer.android.com/studio/) (including NDK 21.4.7075529), [CMake](https://cmake.org/), [Ninja](https://ninja-build.org/), [Visual Studio 2019](https://visualstudio.microsoft.com/vs/)

- The `PATH` environment variable must include the path to adb (typically %LOCALAPPDATA%/Android/sdk/platform-tools/).
- The `PATH` environment variable must include the path to Ninja, or Ninja must be [installed via a package manager](https://github.com/ninja-build/ninja/wiki/Pre-built-Ninja-packages).  
- The `ANDROID_HOME` environment variable must be defined (typically %LOCALAPPDATA%/Android/sdk).
- The `JAVA_HOME` environment variable must be defined (typically %ProgramFiles%/Android/Android Studio/jre).

### **Configuring a Linux Dev Environment**

**Required Tools:** [Android Studio](https://developer.android.com/studio/) (including NDK 21.4.7075529)

With Ubuntu, you can install needed packages by this command:

```
sudo apt-get install adb ninja-build openjdk-14-jdk android-sdk
```

Update PATH with this commands:

```
export ANDROID_HOME=$HOME/Android/Sdk
export PATH=$PATH:$ANDROID_HOME/emulator
export PATH=$PATH:$ANDROID_HOME/tools
export PATH=$PATH:$ANDROID_HOME/tools/bin
export PATH=$PATH:$ANDROID_HOME/platform-tools
```

### **Troubleshooting**
- If the Metro server is not started with `npm run android` , you can start it manually by running `npm run start` in a terminal.
- Android Studio is the tool of choice for downloading the various versions of NDK.
- If something goes wrong with the build `npm run android --verbose` can give some hints.
- If the emulator is not launched by the build, you can run `~/Android/Sdk/emulator/emulator @name_of_your_image`.
- If `ld: library not found for -lBabylonNative` appears when building for iOS, this is because of CMake 3.26+. use 3.24 or remove `CONFIGURATION_BUILD_DIR`entries for every `.pbxproj` file.
- For other emulator issues, follow the [instructions](https://github.com/BabylonJS/BabylonNative/blob/master/Documentation/AndroidEmulator.md) from Babylon Native documentation.
- Refer to the [Babylon Native documentation](https://github.com/BabylonJS/BabylonNative/tree/master/Documentation#babylon-native-documention) for additional information that may help troubleshoot issues.

### **Building and Running the Playground App**

On either Mac or Windows, NPM is used to build and run the Playground sample/test app from the command line. Open a command prompt at the root of the BabylonReactNative repo if you don't have one already open.

#### Android

```
cd Apps/Playground/Playground
npm run android
```

After having run the above commands, you can also open `Apps/Playground/android` in Android Studio and run the app from there.

#### iOS

iOS can only be built on a Mac.

```
cd Apps/Playground/Playground
npm run ios
```

After having run the above commands, you can also open `Apps/Playground/ios/Playground.xcworkspace` in XCode and run the app from there.

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

If you want to test using a local build of the NPM package with your own React Native app, you can do so with a `gulp` command on a Mac (this requires a Mac as it builds binaries for both iOS and Android).

```
cd Package
npm install
gulp pack
```

The NPM package will be built into the `Package` directory where the `gulp` command was run. Once the local NPM package has been built, it can be installed into a project using `npm`.

```
cd <directory of your React Native app>
npm install <root directory of your BabylonReactNative clone>/Package/Assembled/babylonjs-react-native-0.0.1.tgz
```

If you wish to test the locally-built NPM packages with the apps in the `PackageTest` directory, before running `npm install` be sure to run:

```
cd Apps\PackageTest\<package test app version>

npm uninstall @babylon/react-native

# If you're also updating the react-native-windows package:
npm uninstall @bablyon/react-native-windows
```

This will allow the local package dependencies to update without the package-lock.json file worrying about new content without a new version number. You can then run the above command to install the locally-built NPM modules located in `Package/Assembled`.

### **Debugging in Context**

If you want to consume `@babylonjs/react-native` as source in your React Native app (for debugging or for iterating on the code when making a contribution), you can install the package source directory as an npm package.

```
cd <directory of your React Native app>
npm install <root directory of your BabylonReactNative clone>/Modules/@babylonjs/react-native
cd ios
pod install
```

This will create a symbolic link in your `node_modules` directory to the `@babylonjs/react-native` source directory. However, this also requires a custom `metro.config.js` as the Metro bundler does not support symbolic links by default. See the [GitHub issue](https://github.com/react-native-community/cli/issues/1238#issue-673055870) on this for a solution.

For iOS the XCode project needs to be generated with `CMake` as described [above](#ios) and added to your `xcworkspace`.

## Supported Versions

| React Native | Babylon React Native |
|---|---|
| 0.64 | 1.4.0, 1.4.1, 1.4.2, 1.4.3, 1.4.4, 1.5.0, 1.5.1, 1.6.0, 1.6.1, 1.6.3, 1.8.1 |
| 0.65 -> 0.68 | 1.4.0, 1.4.1, 1.4.2, 1.4.3, 1.4.4, 1.5.0, 1.5.1, 1.6.0, 1.6.1, 1.6.3, 1.8.1 |
| 0.69 | 1.4.0, 1.4.1, 1.4.2, 1.4.3, 1.4.4, 1.5.0, 1.5.1, 1.6.0, 1.6.1, 1.6.3, 1.8.1 |
| 0.70 | 1.5.0, 1.5.1, 1.6.0, 1.6.1, 1.6.3, 1.8.1 |
| 0.71 -> 0.73 | 1.6.0, 1.6.1, 1.6.3, 1.8.1 |

Here are the package names for Android/iOS and Windows:

| React Native | Android/iOS Package | Windows Package |
|---|---|---|
| 0.64 | [@babylonjs/react-native-iosandroid-0-64](https://www.npmjs.com/package/@babylonjs/react-native-iosandroid-0-64) | [@babylonjs/react-native-windows-0-64](https://www.npmjs.com/package/@babylonjs/react-native-windows-0-64) |
| 0.65 -> 0.68 | [@babylonjs/react-native-iosandroid-0-65](https://www.npmjs.com/package/@babylonjs/react-native-iosandroid-0-65) | [@babylonjs/react-native-windows-0-65](https://www.npmjs.com/package/@babylonjs/react-native-windows-0-65) |
| 0.69 | [@babylonjs/react-native-iosandroid-0-69](https://www.npmjs.com/package/@babylonjs/react-native-iosandroid-0-69) | [@babylonjs/react-native-windows-0-69](https://www.npmjs.com/package/@babylonjs/react-native-windows-0-69) |
| 0.70 | [@babylonjs/react-native-iosandroid-0-70](https://www.npmjs.com/package/@babylonjs/react-native-iosandroid-0-70) | [@babylonjs/react-native-windows-0-70](https://www.npmjs.com/package/@babylonjs/react-native-windows-0-70) |
| 0.71 -> 0.73 | [@babylonjs/react-native-iosandroid-0-71](https://www.npmjs.com/package/@babylonjs/react-native-iosandroid-0-71) | [@babylonjs/react-native-windows-0-71](https://www.npmjs.com/package/@babylonjs/react-native-windows-0-71) |

Also, [@babylonjs/react-native](https://www.npmjs.com/package/@babylonjs/react-native) is a needed dependency for all platforms.

## Supported Babylon.js Versions

Depending on the Babylon React Native NPM package version, some Babylon.js NPM dependency versions may or may not be compatible. Here is a compatibility list that has been tested:

| Babylon React Native | Babylon.js ([@babylonjs/core](https://www.npmjs.com/package/@babylonjs/core), [@babylonjs/loaders](https://www.npmjs.com/package/@babylonjs/loaders), ...) |
|---|---|
| 1.4.0 | 5.27.1 |
| 1.4.1 | 5.32.2, 5.33.0, 5.33.1, 5.33.2, 5.34.0, 5.35.0 |
| 1.4.2 | 5.35.1, 5.36.0, 5.37.0, 5.38.0, 5.39.0, 5.42.0, 5.42.1 |
| 1.4.3 | 5.42.2, 5.43.0, 5.43.1, 5.43.2, 5.44.0, 5.45.0, 5.45.1, 5.45.2, 5.46.0, 5.47.0, 5.47.1, 5.48.0, 5.48.1, 5.49.0, 5.49.1, 5.49.2, 5.50.0, 5.50.1, 5.51.0, 5.52.0, 5.53.0, 5.54.0, 5.55.0, 5.56.0, 5.57.0, 5.57.1, 6.0.0, 6.1.0, 6.2.0, 6.3.1, 6.4.0, 6.4.1 |
| 1.4.4 | 5.42.2, 5.43.0, 5.43.1, 5.43.2, 5.44.0, 5.45.0, 5.45.1, 5.45.2, 5.46.0, 5.47.0, 5.47.1, 5.48.0, 5.48.1, 5.49.0, 5.49.1, 5.49.2, 5.50.0, 5.50.1, 5.51.0, 5.52.0, 5.53.0, 5.54.0, 5.55.0, 5.56.0, 5.57.0, 5.57.1, 6.0.0, 6.1.0, 6.2.0, 6.3.0, 6.3.1, 6.4.0, 6.4.1 |
| 1.5.0 | 5.42.2, 5.43.0, 5.43.1, 5.43.2, 5.44.0, 5.45.0, 5.45.1, 5.45.2, 5.46.0, 5.47.0, 5.47.1, 5.48.0, 5.48.1, 5.49.0, 5.49.1, 5.49.2, 5.50.0, 5.50.1, 5.51.0, 5.52.0, 5.53.0, 5.54.0, 5.55.0, 5.56.0, 5.57.0, 5.57.1, 6.0.0, 6.1.0, 6.2.0, 6.3.0, 6.3.1, 6.4.0, 6.4.1 |
| 1.5.1 | 5.42.2, 5.43.0, 5.43.1, 5.43.2, 5.44.0, 5.45.0, 5.45.1, 5.45.2, 5.46.0, 5.47.0, 5.47.1, 5.48.0, 5.48.1, 5.49.0, 5.49.1, 5.49.2, 5.50.0, 5.50.1, 5.51.0, 5.52.0, 5.53.0, 5.54.0, 5.55.0, 5.56.0, 5.57.0, 5.57.1, 6.0.0, 6.1.0, 6.2.0, 6.3.0, 6.3.1, 6.4.0, 6.4.1 |
| 1.6.0 | 5.42.2, 5.43.0, 5.43.1, 5.43.2, 5.44.0, 5.45.0, 5.45.1, 5.45.2, 5.46.0, 5.47.0, 5.48.0, 5.48.1, 5.49.1 |
| 1.6.1 | 5.42.2, 5.43.0, 5.43.1, 5.43.2, 5.44.0, 5.45.0, 5.45.1, 5.45.2, 5.46.0, 5.47.0, 5.48.0, 5.48.1, 5.49.1 |
| 1.6.3 | 5.53.1, 5.54.0, 5.55.0, 5.56.0, 5.57.0, 5.57.1, 6.0.0, 6.1.0, 6.2.0, 6.3.0, 6.3.1, 6.4.0, 6.4.1, 6.5.0, 6.5.1, 6.6.0, 6.6.1, 6.7.0, 6.8.1, 6.9.0, 6.11.2, 6.12.0, 6.12.1,6.12.2, 6.12.3, 6.14.0 |
| 1.8.1 | 5.53.1, 5.54.0, 5.55.0, 5.56.0, 5.57.0, 5.57.1, 6.0.0, 6.1.0, 6.2.0, 6.3.0, 6.3.1, 6.4.0, 6.4.1, 6.5.0, 6.5.1, 6.6.0, 6.6.1, 6.7.0, 6.8.1, 6.9.0, 6.11.2, 6.12.0, 6.12.1,6.12.2, 6.12.3, 6.14.0 |

## Security

If you believe you have found a security vulnerability in this repository, please see [SECURITY.md](SECURITY.md).
