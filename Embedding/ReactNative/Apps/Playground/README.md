# BabylonReactNative Playground App

This is a new example app for BabylonReactNative, based on `react-native-test-app`.

To run it (on an Android device):

1. `npm i`
2. `npm run android`

(assuming that you have your setup working for running [RN Android apps on device](https://reactnative.dev/docs/running-on-device?platform=android))

The Android test app uses ARCore SDK 1.56.0, configured through `ext.arcoreVersion` in `android/build.gradle`. This overrides the BabylonReactNative module's default for the Playground only.

React Native Windows is pinned to 0.81.35 in both Playground manifests to match its published NuGet packages. Keep this exact pin and both lockfiles in sync when updating RNW; installing the packaged BabylonReactNative artifact must not select a newer npm-only RNW release.
