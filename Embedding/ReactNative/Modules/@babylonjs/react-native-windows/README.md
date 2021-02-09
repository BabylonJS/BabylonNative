# @babylonjs/react-native-windows

## Usage

This package contains windows dependencies for @babylonjs/react-native. See @babylonjs/react-native for usage.

### Dependencies

This package has several **peer dependencies**. If these dependencies are unmet, `npm install` will emit warnings. Be sure to add these dependencies to your project.

This package will not work without installing the `@babylonjs/react-native` peer dependency.
The `react-native-permissions` dependency is required for XR capabilities of Babylon.js.

### Universal Windows Platform (UWP) Configuration

The minimum UWP target platform version is 10.0.18362.0. UWP BabylonReactNative requires the following NPM dependencies. These versions contradict the peer dependencies declared in package.json in order to support Android and iOS consumers using older versions of react and react-native.