# Babylon React Native

This project provides Babylon Native integration into React Native.

## Current Status

Babylon React Native is in the early phase of its development, and has the following limitations:

1. Android support only - support for both iOS and Windows is planned.
1. JavaScriptCore only - support for Hermes is planned, and support for other JavaScript engines used by React Native is uncertain.

## Usage

See the [package usage](Apps/Playground/node_modules/@babylonjs/react-native/README.md) or the Playground app's [App.tsx](Apps/Playground/App.tsx) for example usage.

## Contributing

This quick overview will help you get started developing in the Babylon React Native repository. We support development on Windows and macOS, but assume the use of [PowerShell](https://docs.microsoft.com/en-us/powershell/scripting/install/installing-powershell) in the instructions below.

### **Preparing a new Repo**

**Required Tools:** [git](https://git-scm.com/), [Yarn](https://classic.yarnpkg.com/en/docs/install)

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
yarn install
```

For iOS, CocoaPods are also used, and these must be installed.

```
cd Apps/Playground/ios
pod install --repo-update
```

### **Configuring a Mac Dev Environment**

**Required Tools:** [Android Studio](https://developer.android.com/studio/) (including NDK 21.0.6113669), [CMake](https://cmake.org/), [Ninja](https://ninja-build.org/)

- The `PATH` environment variable must include the path to adb (typically ~/Library/Android/sdk/platform-tools/).
- The `PATH` environment variable must include the path to Ninja, or Ninja must be [installed via a package manager](https://github.com/ninja-build/ninja/wiki/Pre-built-Ninja-packages).
- The `ANDROID_HOME` environment variable must be defined (typically ~/Library/Android/sdk).
- The `ANDROID_SDK_ROOT` environment variable must be defined (typically ~/Library/Android/sdk).
- The `ANDROID_AVD_HOME` environment variable must be defined if you plan to use Android emulators (typically ~/.android/avd).

You can typically configure your environment by editing `~/.zshrc` and adding the following:

```
export PATH=$PATH:~/Library/Android/sdk/platform-tools/
export PATH=$PATH:~/path_to_ninja_binary/ # Only for manual installations of Ninja (not package manager-based installations).
export ANDROID_HOME=~/Library/Android/sdk
export ANDROID_SDK_ROOT=~/Library/Android/sdk
export ANDROID_AVD_HOME=~/.android/avd
```

### **Configuring a Windows Dev Environment**

**Required Tools:** [Android Studio](https://developer.android.com/studio/) (including NDK 21.0.6113669), [CMake](https://cmake.org/), [Ninja](https://ninja-build.org/)

- The `PATH` environment variable must include the path to adb (typically %LOCALAPPDATA%/Android/sdk/platform-tools/).
- The `PATH` environment variable must include the path to Ninja, or Ninja must be [installed via a package manager](https://github.com/ninja-build/ninja/wiki/Pre-built-Ninja-packages).  
- The `ANDROID_HOME` environment variable must be defined (typically %LOCALAPPDATA%/Android/sdk).
- The `JAVA_HOME` environment variable must be defined (typically %ProgramFiles%/Android/Android Studio/jre).


### **Building and Running the Playground App**

On either Mac or Windows, NPX is used to build and run the Playground sample/test app from the command line. Open a command prompt at the root of the BabylonReactNative repo if you don't have one already open.

#### Android

```
cd Apps/Playground
npx react-native run-android
```

After having run the above commands, you can also open `Apps/Playground/android` in Android Studio and run the app from there.

#### iOS

```
pushd Apps/Playground/ios
cmake -G Xcode -DCMAKE_TOOLCHAIN_FILE=../submodules/BabylonNative/Dependencies/ios-cmake/ios.toolchain.cmake -DPLATFORM=OS64COMBINED -DENABLE_ARC=0 -DDEPLOYMENT_TARGET=12 -DENABLE_GLSLANG_BINARIES=OFF -DSPIRV_CROSS_CLI=OFF .
popd

cd Apps/Playground
npx react-native run-ios
```

After having run the above commands, you can also open `Apps/Playground/ios/Playground.xcworkspace` in XCode and run the app from there.

### **Building the NPM Package**

If you want to test using a local build of the NPM package with your own React Native app, you can do so with the `npm pack` command on either Mac or Windows.

```
cd Apps/Playground/node_modules/@babylonjs/react-native
npm pack
```

This will produce a zipped local NPM package that can be installed into a React Native application for testing purposes.
