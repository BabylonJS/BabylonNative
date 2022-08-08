BabylonReactNative supports different versions of react-native.
For each major version (0.64, 0.65,...) there is a different Playground App.
Each version is built using the react-native template with different dependencies version.

# How to add a new react-native supported version

Use the react native template to generate the app:

```
npx react-native init Playground --version 0.65.0 --template react-native-template-typescript
```

This will create a new Playground app using the version passed a parameter.
Rename the `Playground` folder as the react-native version. Here : `0.65`
Add or change the dependencies version in `package.json`, also, add this line in dependencies:
`"@babylonjs/playground-shared": "file:../playground-shared",`
remove `App.tsx` and `App.json` and change `index.js` to use playground-shared package:

```
import {AppRegistry} from 'react-native';
import App from '../playground-shared/App';
import {name as appName} from '../playground-shared/app.json';

AppRegistry.registerComponent(appName, () => App);
```

## Set up JS project
Install the following packages into the project:
* @babylonjs/core
* @babylonjs/Loaders
* @react-native-community/slider
* react-native-permissions

Add the following packages via reference:
* npm install ../../../Modules/@babylonjs/react-native
* npm install ../../../Modules/@babylonjs/react-native-iosandroid
* npm install ../../../Modules/@babylonjs/react-native-windows
* npm install ../playground-shared

Since we consume the above packages via symlinks we need a custom Metro configuration to allow the Metro bundler to correctly process the packages. You should be able to directly copy the file from: [metro.config.js](./0.65/metro.config.js).

Copy over the [scripts/tools.js](./0.65/scripts/tools.js) which contains postinstall scripts used during npm and react-native tasks. Then in package.json set up the scripts to look like the below:
```
  "scripts": {
    "android": "react-native run-android",
    "ios": "react-native run-ios",
    "start": "react-native start",
    "test": "jest",
    "lint": "eslint . --ext .js,.jsx,.ts,.tsx",
    "postinstall": "node scripts/tools.js postinstall",
    "iosCmake": "node scripts/tools.js iosCMake",
    "windows": "react-native run-windows"
  }
```

## iOS
Add BabylonReactNative in the Xcode workspace:
```
...
<Workspace
   version = "1.0">
   <FileRef
      location = "group:../node_modules/@babylonjs/react-native-iosandroid/ios/ReactNativeBabylon.xcodeproj">
   </FileRef>
   ...
```

For camera access, add a key in `Info.plist` :

```
...
<dict>
    <key>NSCameraUsageDescription</key>
    <string></string>
    <key>CFBundleDevelopmentRegion</key>
    ...
```

and add permission in the pod file:
```
...
target 'Playground' do
  permissions_path = '../node_modules/react-native-permissions/ios'
  pod 'Permission-Camera', :path => "#{permissions_path}/Camera"
  ...
```

## Android

Add camera and AR permission in `android/app/.src/main/AndroidManifest.xml` :

```
...
<uses-permission android:name="android.permission.CAMERA"/>
...
```

and 

```
...
</activity>
      <meta-data android:name="com.google.ar.core" android:value="optional" />
    </application>
...
```

For Hermes, add these lines in `android/app/proguard-rules.pro` :

```
-keep class com.facebook.hermes.unicode.** { *; }
-keep class com.facebook.jni.** { *; }
```

You may also need to update `android/build.gradle` to downgrade the default Gradle version, build tools, and remove some react-native installed gradle tasks, the final result will look something like this:
```
    ext {
        buildToolsVersion = "30.0.2"
        minSdkVersion = 21
        compileSdkVersion = 30
        targetSdkVersion = 30
        ndkVersion = "20.1.5948944"
    }
    ...
    dependencies {
        classpath("com.android.tools.build:gradle:4.2.1")
        // NOTE: Do not place your application dependencies here; they belong
        // in the individual module build.gradle files
    }
```

## Windows

Create the Windows Playground app by running this command in the version folder:

```
npx react-native-windows-init --overwrite
```

Disable use of webDebugger in `Apps.cpp` because BabylonReactNative accesses the jsi runtime, which isn't possible with the web debugger.

```
InstanceSettings().UseWebDebugger(false);
```

## Update toolchain scripts

Build tools must be updated to reflect new version of React Native support (or removal).
- In `Apps/Playground/scripts/version.js`, a simple check for supported versions is done
- In `Package/gulpfile.js`, a test and dependency patch is done in function `patchPackageVersion`. 

## Troubleshooting

When running the Playground, if you encounter this error message:

```
TypeScript 'declare' fields must first be transformed by @babel/plugin-transform-typescript.
```

Modify `babel.config.js` with these lines:

```
module.exports = {
  presets: [
    'module:metro-react-native-babel-preset',
    ['@babel/preset-typescript', {allowDeclareFields: true}],
    ],
};
```