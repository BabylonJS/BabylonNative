# Babylon React Native

Babylon React Native brings [Babylon.js](https://github.com/BabylonJS/Babylon.js) to React Native through [Babylon Native](https://github.com/BabylonJS/BabylonNative). Since 2.0.0, a single `@babylonjs/react-native` package contains the native sources for Android, iOS, and Windows. Windows support remains experimental.

## Usage

This quick overview will help you understand the constructs provided by Babylon React Native and how to use them in a React Native application.

### Installation

The latest stable release is **2.0.7**. In an existing React Native application using the [versions below](#supported-versions), install the package and its peer dependencies:

```sh
npm install @babylonjs/react-native@2.0.7 @babylonjs/core@^9.0.0 react-native-permissions@^5.4.1
```

Use the React version required by your React Native application. React Native 0.81 requires **Node.js 20.19.4 or later**. Follow the platform configuration below before building the app.

To load formats such as glTF, also install `@babylonjs/loaders` at the same version as `@babylonjs/core` and import it in your application.

### Dependencies

The package declares `@babylonjs/core` (>=9.0.0), `react`, `react-native`, and `react-native-permissions` (>=3.0.0) as **peer dependencies**. Add them to your application and resolve any peer dependency errors or warnings during installation.

`useEngine` uses `react-native-permissions` to request camera access for XR and native camera use. Follow the [react-native-permissions setup instructions](https://github.com/zoontek/react-native-permissions#setup): enable the `Camera` permission handler in your iOS `Podfile`, provide `NSCameraUsageDescription` in `Info.plist`, and declare `android.permission.CAMERA` in `AndroidManifest.xml` when using these features.

The native camera and XR plugins can be [disabled at build time](#plugins-selection), but `react-native-permissions` is still a declared peer dependency and is imported by the JavaScript module.

### Supported Versions

These are the resolved React Native versions in the committed Playground lockfiles at each release tag, not the lower bounds of manifest ranges. The permissive `react` and `react-native` peer dependency ranges do **not** guarantee that every newer React Native version is compatible.

| Babylon React Native version | React Native (iOS / Android) | React Native Windows |
| ---------------------------- | --------------------------- | -------------------- |
| 2.0.7 / master               | 0.81.6                      | 0.81.35              |
| 2.0.6                        | 0.81.6                      | 0.81.35              |
| 2.0.5                        | 0.81.6                      | 0.81.21              |
| 2.0.0 - 2.0.4                | 0.79.4                      | 0.79.1               |

For 2.0.6 and 2.0.7, the Windows Playground locks React Native 0.81.6 with **React Native Windows pinned to 0.81.35** to match the published NuGet packages; 0.81.4 is only the lower bound of the manifest's `^0.81.4` React Native range. Windows project generation uses the legacy renderer and Hermes.

The Babylon.js column below records the package's `@babylonjs/core` peer dependency. The Babylon Native commits are the source pins in `Package/gulpfile.js` at the corresponding release tags.

| Babylon React Native version | Babylon.js peer dependency | Babylon Native commit |
| ---------------------------- | -------------------------- | --------------------- |
| 2.0.7 / master               | >=9.0.0                    | [f4d9efbeeabcfec24105373b99e54fe147669fc1](https://github.com/BabylonJS/BabylonNative/commit/f4d9efbeeabcfec24105373b99e54fe147669fc1) |
| 2.0.6                        | >=9.0.0                    | [feb0227be8890ec5dcd28c6bfc18275a98bf8d45](https://github.com/BabylonJS/BabylonNative/commit/feb0227be8890ec5dcd28c6bfc18275a98bf8d45) |
| 2.0.5                        | >=9.0.0                    | [ce2edf0851e0c8483559832a4f8eb9d39c6b2f53](https://github.com/BabylonJS/BabylonNative/commit/ce2edf0851e0c8483559832a4f8eb9d39c6b2f53) |
| 2.0.4                        | >=9.0.0                    | [ce2edf0851e0c8483559832a4f8eb9d39c6b2f53](https://github.com/BabylonJS/BabylonNative/commit/ce2edf0851e0c8483559832a4f8eb9d39c6b2f53) |
| 2.0.3                        | >=9.0.0                    | [ce2edf0851e0c8483559832a4f8eb9d39c6b2f53](https://github.com/BabylonJS/BabylonNative/commit/ce2edf0851e0c8483559832a4f8eb9d39c6b2f53) |
| 2.0.2                        | >=9.0.0                    | [887a0446c2ce5d379d931d802071cf6f9e008c6a](https://github.com/BabylonJS/BabylonNative/commit/887a0446c2ce5d379d931d802071cf6f9e008c6a) |
| 2.0.1                        | >=5.53.1                   | [3ccba7a6f741705ce611819aefacbbb8062393e7](https://github.com/BabylonJS/BabylonNative/commit/3ccba7a6f741705ce611819aefacbbb8062393e7) |
| 2.0.0                        | >=5.53.1                   | [9a50d722d493524e1b315dd48fed6b0ed6d062c8](https://github.com/BabylonJS/BabylonNative/commit/9a50d722d493524e1b315dd48fed6b0ed6d062c8) |

2.0.7 updates Babylon Native and its JsRuntimeHost dependency, and removes obsolete `minz` library links from the iOS podspec and Windows project. The Babylon.js and React Native baselines are unchanged from 2.0.6.

See the [release notes](https://github.com/BabylonJS/BabylonReactNative/releases) for changes and the [repository README](https://github.com/BabylonJS/BabylonReactNative#contributing) for local development and packaging instructions.

### Android Configuration

Use the SDK, NDK, and JDK required by your React Native version. For React Native 0.81, the effective minimum is **Android API 24**, even though this module's standalone Gradle fallback is API 21. Set `minSdkVersion` in the consuming project's `build.gradle` accordingly; do not lower React Native's minimum.

The supported Android ABIs are `armeabi-v7a`, `arm64-v8a`, and `x86_64`. The 32-bit `x86` ABI is not supported and is filtered out of `reactNativeArchitectures`; use an `x86_64` or `arm64-v8a` emulator instead.

#### ARCore

The module defaults to ARCore **1.22.0**. Since 2.0.4, consuming apps can override it in their root `android/build.gradle`, for example:

```groovy
ext.arcoreVersion = '1.56.0'
```

The Playground uses 1.56.0. The native build resolves the actually selected ARCore AAR, including when Gradle dependency resolution overrides the declared version. This can help resolve duplicate-class conflicts involving older ARCore versions and `installreferrer`.

Since 2.0.6, the module declares ARCore as optional in its Android manifest. Applications that require AR should configure their own manifest and check device support; camera permission setup is still required for XR.

The Android Gradle configuration also avoids applying `kotlin-android` when the Kotlin extension is already registered, including by Android Gradle Plugin 9's built-in Kotlin support. This does not change the React Native compatibility baselines above.

### iOS Configuration

The minimum deployment target is **iOS 16.0** for the current package. Set the app's `iOS Deployment Target` and the `Podfile` platform to at least `16.0`:

```ruby
platform :ios, '16.0'
```

After `npm install`, run `pod install` from your app's `ios` directory. The npm postinstall generates `Build/iOS/ReactNativeBabylon.xcodeproj` on macOS; it does not install CocoaPods dependencies.

#### Workspace
In your app workspace, add a reference to the Babylon React Native project by adding these lines:
```xml
<FileRef
    location = "group:../node_modules/@babylonjs/react-native/Build/iOS/ReactNativeBabylon.xcodeproj">
</FileRef>
```
to your app's `.xcworkspace/contents.xcworkspacedata` file. The path shown assumes the workspace is in the app's `ios` directory.

#### CMake
To disable post install CMake generation, set this variable before running `npm install`:
```sh
export BABYLON_NO_CMAKE_POSTINSTALL=1
```

Since 2.0.3, you can use a system CMake (e.g. a Homebrew installation) instead of the `cmake-runtime` 3.31.0 dependency. Set this variable before running `npm install`:
```sh
export BABYLON_USE_SYSTEM_CMAKE=1
```

Since 2.0.4, you can override the iOS deployment target used by the generated `ReactNativeBabylon.xcodeproj` (defaults to `16.0`). Set this variable before running `npm install` so it matches your app's `Podfile` platform:
```sh
export BABYLON_IOS_DEPLOYMENT_TARGET=16.0
```
The previous iOS 12 default caused Xcode 26's Clang to crash while compiling `DeviceImpl_iOS.mm`. The override changes the generated native project only; it does not lower the podspec's iOS 16 minimum or change your app's deployment target.

If you skipped project generation or changed these settings after installation, regenerate it on macOS from your app root:

```sh
node node_modules/@babylonjs/react-native/postinstall.js
```

Unset `BABYLON_NO_CMAKE_POSTINSTALL` first if it was enabled, then rerun `pod install` after regenerating the project.

### Plugins selection

The NativeCamera and NativeXR plugins are enabled by default. For Android and iOS, disable them with environment variables set before generating/building the native project:

To disable Camera plugin:
```sh
export BABYLON_NATIVE_PLUGIN_NATIVECAMERA=0
```

For Native XR:
```sh
export BABYLON_NATIVE_PLUGIN_NATIVEXR=0
```

Keep the variables set when running `pod install` and native builds. Disabling plugins does not remove the JavaScript peer dependency on `react-native-permissions`.

### Windows Configuration

Windows support is experimental. The 2.0.6 and 2.0.7 integrations use React Native Windows **0.81.35** and its matching NuGet packages. Keep the React Native Windows npm version aligned with its published NuGet version rather than allowing npm to select a newer release independently.

The module's MSBuild project builds the embedded Babylon Native sources using CMake and the React Native Windows project's generated C++/WinRT projections. See the [repository's Windows instructions](https://github.com/BabylonJS/BabylonReactNative#universal-windows-platform-uwp) and [packaged-consumer build workflow](https://github.com/BabylonJS/BabylonReactNative/blob/master/.github/workflows/test_windows.yml) for the current UWP, legacy-renderer, NuGet, and C++20 configuration.

### `useEngine`

`useEngine` is a **custom React hook** that manages the lifecycle of a Babylon engine instance in the context of an owning React component. `useEngine` creates an engine instance **asynchronously** which is used to create and configure scenes. Typically scene initialization code should exist in a `useEffect` triggered by an `engine` state change. For example:

```tsx
import React, { useEffect } from 'react';
import { useEngine } from '@babylonjs/react-native';
import { Scene } from '@babylonjs/core';

const MyComponent = () => {
    const engine = useEngine();

    useEffect(() => {
        if (engine) {
            const scene = new Scene(engine);
            // Setup the scene!
            return () => scene.dispose();
        }
    }, [engine]);

    return null;
}
```

`useEngine` initially returns `undefined` and disposes the engine when its owning component unmounts. Dispose scenes created by an effect in that effect's cleanup.

### `EngineView`

`EngineView` is a **custom React Native view** that presents a `camera` from a Babylon `scene`. A `camera` therefore is assigned to the `EngineView`. For example:

```tsx
import React, { useEffect, useState } from 'react';
import { useEngine, EngineView } from '@babylonjs/react-native';
import { Scene, Camera, MeshBuilder } from '@babylonjs/core';

const MyComponent = () => {
    const engine = useEngine();
    const [camera, setCamera] = useState<Camera>();

    useEffect(() => {
        if (engine) {
            const scene = new Scene(engine);
            scene.createDefaultCamera(true);
            scene.createDefaultLight(true);
            MeshBuilder.CreateBox('box', {}, scene);
            setCamera(scene.activeCamera!);
            return () => scene.dispose();
        }
    }, [engine]);

    return (
        <>
            <EngineView style={{flex: 1}} camera={camera} />
        </>
    );
}
```

Also the `EngineView` has a boolean `isTransparent` flag which defines whether the background of the scene should be transparent or not.

e.g.

```tsx
<EngineView style={{flex: 1}} camera={camera} isTransparent={true} />
```
To configure anti-aliasing, set `antiAliasing` to 0 or 1 (disabled; the default is 0), or to 2, 4, 8, or 16 (MSAA sample count).

e.g.

```tsx
<EngineView style={{flex: 1}} camera={camera} antiAliasing={4} />
```

Use `displayFrameRate` to control the FPS overlay (enabled by default in development builds). On Android and iOS, the `onInitialized` callback receives an `EngineViewCallbacks` object with `takeSnapshot(): Promise<string>`, which returns a base64-encoded JPEG. Snapshots are not implemented on Windows.

Note: Currently only one `EngineView` can be active at any given time. Multi-view will be supported in a future release.

The Android specific `androidView` property can help set the type of the view used for rendering. Depending on user needs and performance, refer to the table below. [`TextureView`](https://developer.android.com/reference/android/view/TextureView) can be inserted anywhere in the view hierarchy, but is less efficient. [`SurfaceView`](https://developer.android.com/reference/android/view/SurfaceView) can only be fully above or fully below the rest of the UI, but is more efficient.

| isTransparent | androidView        | Description |
| ----------- | ------------------------ | ----------- |
| False       | TextureView              | Opaque TextureView.
| False       | SurfaceView              | Simple SurfaceView (default when no `androidView` set with `isTransparent=false`).
| False       | SurfaceViewZTopMost      | SurfaceView with [ZTopMost](https://developer.android.com/reference/android/view/SurfaceView#setZOrderOnTop(boolean)) set to `true`.
| False       | SurfaceViewZMediaOverlay | SurfaceView with [ZMediaOverlay](https://developer.android.com/reference/android/view/SurfaceView#setZOrderMediaOverlay(boolean)) set to `true`.
| True        | TextureView              | Transparent TextureView.
| True        | SurfaceView              | SurfaceView will stay opaque
| True        | SurfaceViewZTopMost      | SurfaceView with [ZTopMost](https://developer.android.com/reference/android/view/SurfaceView#setZOrderOnTop(boolean)) set to `true`. Transparent but topmost. (default when no `androidView` set with `isTransparent=true`)
| True        | SurfaceViewZMediaOverlay | SurfaceView with [ZMediaOverlay](https://developer.android.com/reference/android/view/SurfaceView#setZOrderMediaOverlay(boolean)) set to `true`. Only transparent on top of other SurfaceViews.

More information on TextureView vs SurfaceView performance here:
https://developer.android.com/reference/android/view/TextureView