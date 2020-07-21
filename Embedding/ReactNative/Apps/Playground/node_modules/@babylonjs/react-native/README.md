# @babylonjs/react-native

## Usage

This quick overview will help you understand the constructs provided by Babylon React Native and how to use them in a React Native application.

### Dependencies

This package has several **peer dependencies**. If these dependencies are unmet, `npm install` will emit warnings. Be sure to add these dependencies to your project.

The `react-native-permissions` dependency is required for XR capabilities of Babylon.js (to request camera permissions automatically). Be sure to follow the `react-native-permissions` [instructions](https://github.com/react-native-community/react-native-permissions#setup) to update your `Podfile` and `Info.plist` (iOS) and/or `AndroidManifest.xml` (Android).

### C++ Build Requirements

This package includes C++ source, so platform specific tooling to build C++ code must be installed.

### Android Configuration

The minimum Android SDK version is 18. This must be set as `minSdkVersion` in the consuming project's `build.gradle` file.

### iOS Configuration

The minimum deployment target version is 12. This must be set as `iOS Deployment Target` in the consuming project's `project.pbxproj`, and must also be set as `platform` in the consuming project's `podfile`.

When running from XCode (with the debugger attached), `API Metal Validation` must be disabled:

1. From within XCode, select from the main menu `Product -> Scheme -> Edit Scheme...`
1. Ensure the target for your app is selected in the upper left corner of the window.
1. Select `Run` from the scheme list.
1. Select the `Options` tab.
1. Change `API Metal Validation` to `Disabled`.

### `useEngine`

`useEngine` is a **custom React hook** that manages the lifecycle of a Babylon engine instance in the context of an owning React component. `useEngine` creates an engine instance **asynchronously** which is used to create and configure scenes. Typically scene initialization code should exist in a `useEffect` triggered by an `engine` state change. For example:

```tsx
import { useEngine } from '@babylonjs/react-native';
import { Engine, Scene } from '@babylonjs/core';

const MyComponent: FunctionComponent<MyComponentProps> = (props: MyComponentProps) => {
    const engine = useEngine();

    useEffect(() => {
        if (engine) {
            const scene = new Scene(engine);
            // Setup the scene!
        }
    }, [engine]);

    return (
        <>
        </>
    );
}
```

### `EngineView`

`EngineView` is a **custom React Native view** that presents a `camera` from a Babylon `scene`. A `camera` therefore is assigned to the `EngineView`. For example:

```tsx
import { useEngine, EngineView } from '@babylonjs/react-native';
import { Engine, Scene, Camera } from '@babylonjs/core';

const MyComponent: FunctionComponent<MyComponentProps> = (props: MyComponentProps) => {
    const engine = useEngine();
    const [camera, setCamera] = useState<Camera>();

    useEffect(() => {
        if (engine) {
            const scene = new Scene(engine);
            scene.createDefaultCamera(true);
            setCamera(scene.activeCamera!);
            // Setup the scene!
        }
    }, [engine]);

    return (
        <>
            <EngineView style={{flex: 1}} camera={camera} />
        </>
    );
}
```

Note: Currently only one `EngineView` can be active at any given time. Multi-view will be supported in a future release.