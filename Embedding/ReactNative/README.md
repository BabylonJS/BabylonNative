# Babylon React Native

This project provides Babylon Native integration into React Native.

[![](https://github.com/BabylonJS/BabylonReactNative/workflows/Publish%20Package/badge.svg)](https://github.com/BabylonJS/BabylonReactNative/actions?query=workflow%3A%22Publish+Package%22)
[![npm version](https://badge.fury.io/js/%40babylonjs%2Freact-native.svg)](https://badge.fury.io/js/%40babylonjs%2Freact-native)

## Current Status

Babylon React Native is in the early phase of its development, and has the following limitations:

1. Android and iOS support only - support for Windows is planned, but the timeline is currently unknown.
1. Touch input only - mouse, keyboard, and controllers are not yet supported.
1. Single view only - multiple views are not yet supported (only a single view can be displayed).

It is also worth noting that Babylon React Native relies heavily on newer React Native constructs including JSI to get the performance characteristics required for real time rendering. JSI allows for direct synchronous communication between native code and JavaScript code, but is incompatible with "remote debugging." If you need to debug your JavaScript code that uses Babylon React Native, you should enable Hermes and use "direct debugging" (e.g. chrome://inspect or edge://inspect). See the [React Native documentation](https://reactnative.dev/docs/hermes) for more info.

## Usage

See the [package usage](Modules/@babylonjs/react-native/README.md) for installation instructions and/or the Playground app's [App.tsx](Apps/Playground/App.tsx) for example usage.

## Contributing

This quick overview will help you get started developing in the Babylon React Native repository. We support development on Windows and MacOS, but assume the use of [PowerShell](https://docs.microsoft.com/en-us/powershell/scripting/install/installing-powershell) in the instructions below (unless otherwise noted).

If you are interested in making contributions, be sure to also review [CONTRIBUTING.md](CONTRIBUTING.md).

### **Preparing a new Repo**

**Required Tools:** [git](https://git-scm.com/), [Node.js](https://nodejs.org/en/download/)

Step 1 for all development environments and targets is to clone the repo. Use a git-enabled terminal to follow the steps below.

```
git clone https://github.com/BabylonJS/BabylonReactNative
```

Babylon React Native makes extensive use of submodules to supply its dependencies, so it's also necessary to set up the submodules.

```
cd BabylonReactNative
git submodule update --init --recursive
```

The Playground sample/test app is a standard React Native app, and as such also makes extensive use of NPM packages to supply its dependencies, so it's also necessary to install these packages.

```
cd Apps/Playground
npm install
```

For iOS, CocoaPods are also used, and these must be installed.

```
cd Apps/Playground/ios
pod install --repo-update
```

### **Configuring a Mac Dev Environment**

**Required Tools:** [Android Studio](https://developer.android.com/studio/) (including NDK 21.3.6528147), [CMake](https://cmake.org/), [Ninja](https://ninja-build.org/), [JDK 13](https://www.oracle.com/java/technologies/javase-jdk13-downloads.html)

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

**Required Tools:** [Android Studio](https://developer.android.com/studio/) (including NDK 21.3.6528147), [CMake](https://cmake.org/), [Ninja](https://ninja-build.org/)

- The `PATH` environment variable must include the path to adb (typically %LOCALAPPDATA%/Android/sdk/platform-tools/).
- The `PATH` environment variable must include the path to Ninja, or Ninja must be [installed via a package manager](https://github.com/ninja-build/ninja/wiki/Pre-built-Ninja-packages).  
- The `ANDROID_HOME` environment variable must be defined (typically %LOCALAPPDATA%/Android/sdk).
- The `JAVA_HOME` environment variable must be defined (typically %ProgramFiles%/Android/Android Studio/jre).


### **Building and Running the Playground App**

On either Mac or Windows, NPM is used to build and run the Playground sample/test app from the command line. Open a command prompt at the root of the BabylonReactNative repo if you don't have one already open.

#### Android

```
cd Apps/Playground
npm run android
```

After having run the above commands, you can also open `Apps/Playground/android` in Android Studio and run the app from there.

#### iOS

iOS can only be built on a Mac. Additionally, `CMake` must manually be run to generate the XCode project that the [Playground XCode workspace](Apps/Playground/ios/Playground.xcworkspace/contents.xcworkspacedata) includes.

```
pushd Apps/Playground/node_modules/@babylonjs/react-native/ios
cmake -G Xcode -DCMAKE_TOOLCHAIN_FILE=../submodules/BabylonNative/Dependencies/ios-cmake/ios.toolchain.cmake -DPLATFORM=OS64COMBINED -DENABLE_ARC=0 -DENABLE_BITCODE=1 -DDEPLOYMENT_TARGET=12 -DENABLE_GLSLANG_BINARIES=OFF -DSPIRV_CROSS_CLI=OFF .
popd

cd Apps/Playground
npm run ios
```

After having run the above commands, you can also open `Apps/Playground/ios/Playground.xcworkspace` in XCode and run the app from there.

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

## Security

If you believe you have found a security vulnerability in this repository, please see [SECURITY.md](SECURITY.md).
