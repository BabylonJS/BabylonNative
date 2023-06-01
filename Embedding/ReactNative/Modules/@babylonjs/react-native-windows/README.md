# Babylon React Native Windows Runtime

## Usage

This package contains windows dependencies for @babylonjs/react-native. See @babylonjs/react-native for usage.

### Dependencies

This package has several **peer dependencies**. If these dependencies are unmet, `npm install` will emit warnings. Be sure to add these dependencies to your project.

This package will not work without installing the `@babylonjs/react-native` peer dependency.
The `react-native-permissions` dependency is required for XR capabilities of Babylon.js.

### Toolset

Default toolset is v142. It's possible to change it using project variable named `BabylonReactNativeToolset`.
There are multiple ways to specify it:
- environment variable
- msbuild property (`msbuild /p:BabylonReactNativeToolset=v143` for example)
- customize the build by folder (https://learn.microsoft.com/en-us/visualstudio/msbuild/customize-by-directory?view=vs-2022)