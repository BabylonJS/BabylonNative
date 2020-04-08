# Babylon React Native

This project provides Babylon Native integration into React Native.

## Current Status

Babylon React Native is in the early phase of its development, and has the following limitations:

1. Android support only - support for both iOS and Windows is planned.
1. JavaScriptCore only - support for Hermes is planned, and support for other JavaScript engines used by React Native is uncertain.

## Consuming

This quick overview will help you understand the constructs provided by Babylon React Native and how to use them in a React Native application. See the Playground app's [App.tsx](Apps/Playground/App.tsx) for example usage.

### `useEngine`

`useEngine` is a **custom React hook** that manages the lifecycle of a Babylon engine instance in the context of an owning React component. A callback is passed to `useEngine` that receives an engine instance which is used to create and configure scenes. For example:

```tsx
import { useEngine } from 'react-native-babylon';
import { Engine, Scene } from '@babylonjs/core';

const MyComponent: FunctionComponent<MyComponentProps> = (props: MyComponentProps) => {
    useEngine((engine: Engine) => {
        const scene = new Scene(engine);
        // Setup the scene!
    });

    return (
        <>
        </>
    );
}
```

### `EngineView`

`EngineView` is a **custom React Native view** that presents a `camera` from a Babylon `scene`. A `camera` therefore is assigned to the `EngineView`. For example:

```tsx
import { useEngine, EngineView } from 'react-native-babylon';
import { Engine, Scene, Camera } from '@babylonjs/core';

const MyComponent: FunctionComponent<MyComponentProps> = (props: MyComponentProps) => {
    const [camera, setCamera] = useState<Camera>();

    useEngine((engine: Engine) => {
        const scene = new Scene(engine);
        scene.createDefaultCamera(true);
        if (scene.activeCamera) {
            setCamera(scene.activeCamera);
        }
        // Setup the scene!
    });

    return (
        <>
            <EngineView style={{flex: 1}} camera={camera} />
        </>
    );
}
```

Note: Currently only one `EngineView` can be active at any given time. Multi-view will be supported in a future release.

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

### **Configuring a Mac Dev Environment**

**Required Tools:** [Android Studio](https://developer.android.com/studio/), [CMake](https://cmake.org/)

- The `PATH` environment variable must include the path to adb (typically ~/Library/Android/sdk/platform-tools/).
- The `ANDROID_HOME` environment variable must be defined (typically ~/Library/Android/sdk).
- The `ANDROID_SDK_ROOT` environment variable must be defined (typically ~/Library/Android/sdk).
- The `ANDROID_AVD_HOME` environment variable must be defined if you plan to use Android emulators (typically ~/.android/avd).

You can typically configure your environment by editing `~/.zshrc` and adding the following:

```
export PATH=$PATH:~/Library/Android/sdk/platform-tools/
export ANDROID_HOME=~/Library/Android/sdk
export ANDROID_SDK_ROOT=~/Library/Android/sdk
export ANDROID_AVD_HOME=~/.android/avd
```

### **Configuring a Windows Dev Environment**

**Required Tools:** [Android Studio](https://developer.android.com/studio/), [CMake](https://cmake.org/)

- The `PATH` environment variable must include the path to adb (typically %LOCALAPPDATA%/Android/sdk/platform-tools/).
- The `ANDROID_HOME` environment variable must be defined (typically %LOCALAPPDATA%/Android/sdk).

### **Building and Running the Playground App**

On either Mac or Windows, NPX is used to build and run the Playground sample/test app from the command line. Open a command prompt at the root of the BabylonReactNative repo if you don't have one already open.

```
cd Apps/Playground
npx react-native run-android
```

After having run the above command, you can also open `Apps/Playground/android` in Android Studio and run the app from there.

### **Building the NPM Package**

If you want to test using a local build of the NPM package with your own React Native app, you can do so with the `npm pack` command on either Mac or Windows.

```
cd Apps/Playground/node_modules/react-native-babylon
npm pack
```

This will produce a zipped local NPM package that can be installed into a React Native application for testing purposes.