const util = require('util');
const fs = require('fs');
const readdirAsync = util.promisify(fs.readdir);
const log = require('fancy-log');
const gulp = require('gulp');
const shelljs = require('shelljs');
const rename = require('gulp-rename');

function exec(command, workingDirectory = '.', logCommand = true) {
  if (logCommand) {
    log(command);
  }

  if (shelljs.exec(command, {fatal: true, cwd: workingDirectory}).code) {
    throw `'${command}' finished with non-zero exit code.`;
  }
}

const clean = async () => {
  if (shelljs.test('-d', 'Assembled')) {
    shelljs.rm('-r', 'Assembled');
  }
};

const makeXCodeProj = async () => {
  shelljs.mkdir('-p', 'iOS/Build');
  exec('cmake -G Xcode -DCMAKE_TOOLCHAIN_FILE=../../../Apps/Playground/node_modules/@babylonjs/react-native/submodules/BabylonNative/Dependencies/ios-cmake/ios.toolchain.cmake -DPLATFORM=OS64COMBINED -DENABLE_ARC=0 -DENABLE_BITCODE=1 -DDEPLOYMENT_TARGET=12 -DENABLE_GLSLANG_BINARIES=OFF -DSPIRV_CROSS_CLI=OFF ..', 'iOS/Build');
};

const buildIphoneOS = async () => {
  exec('xcodebuild -sdk iphoneos -configuration Release -project ReactNativeBabylon.xcodeproj -scheme BabylonNative build CODE_SIGNING_ALLOWED=NO', 'iOS/Build');
};

const buildIphoneSimulator = async () => {
  exec('xcodebuild -sdk iphonesimulator -arch x86_64 -configuration Release -project ReactNativeBabylon.xcodeproj -scheme BabylonNative build CODE_SIGNING_ALLOWED=NO', 'iOS/Build');
};

const buildIOS = gulp.series(makeXCodeProj, buildIphoneOS, buildIphoneSimulator);

const buildAndroid = async () => {
  exec('./gradlew babylonjs_react-native:assembleRelease', '../Apps/Playground/android');
};

const makeUWPProject = async () => {
  exec('.\\..\\Modules\\@babylonjs\\react-native\\windows\\scripts\\Setup.bat');
}

const buildUWPProject = async () => {
  exec('.\\..\\Modules\\@babylonjs\\react-native\\windows\\scripts\\Build.bat');
}

const buildUWP = gulp.series(makeUWPProject, buildUWPProject);

const copyCommonFiles = () => {
  return  gulp.src('../Apps/Playground/node_modules/@babylonjs/react-native/package.json')
    .pipe(gulp.src('../Apps/Playground/node_modules/@babylonjs/react-native/README.md'))
    .pipe(gulp.src('../Apps/Playground/node_modules/@babylonjs/react-native/*.ts*'))
    .pipe(gulp.src('react-native-babylon.podspec'))
    .pipe(gulp.dest('Assembled'));
};

const copySharedFiles = () => {
  return gulp.src('../Apps/Playground/node_modules/@babylonjs/react-native/shared/BabylonNative.h')
    .pipe(gulp.dest('Assembled/shared'));
};

const copyIOSFiles = () => {
  return  gulp.src('../Apps/Playground/node_modules/@babylonjs/react-native/ios/*.h')
    .pipe(gulp.src('../Apps/Playground/node_modules/@babylonjs/react-native/ios/*.mm'))
    // This xcodeproj is garbage that we don't need in the package, but `pod install` ignores the package if it doesn't contain at least one xcodeproj. 🤷🏼‍♂️
    .pipe(gulp.src('iOS/Build/ReactNativeBabylon.xcodeproj**/**/*'))
    .pipe(gulp.dest('Assembled/ios'));
};

const createIOSUniversalLibs = async () => {
  shelljs.mkdir('-p', 'Assembled/ios/libs');
  const libs = await readdirAsync('iOS/Build/Release-iphoneos');
  libs.map(lib => exec(`lipo -create iOS/Build/Release-iphoneos/${lib} iOS/Build/Release-iphonesimulator/${lib} -output Assembled/ios/libs/${lib}`));
};

const copyAndroidFiles = async () => {
  await new Promise(resolve => {
    gulp.src('Android/build.gradle')
      .pipe(gulp.src('../Apps/Playground/node_modules/@babylonjs/react-native/android/src**/main/AndroidManifest.xml'))
      .pipe(gulp.src('../Apps/Playground/node_modules/@babylonjs/react-native/android/src**/main/java/**/*'))
      .pipe(gulp.dest('Assembled/android'))
      .on('end', resolve);
  });

  await new Promise(resolve => {
          gulp.src('../Apps/Playground/node_modules/@babylonjs/react-native/android/build/intermediates/library_and_local_jars_jni/release/**/*')
    .pipe(gulp.dest('Assembled/android/src/main/jniLibs/'))
    .on('end', resolve);
  });
};

const createUWPDirectories = async () => {
  shelljs.mkdir('-p', 'Assembled/windows');
  shelljs.mkdir('-p', 'Assembled/windows/libs');
  shelljs.mkdir('-p', 'Assembled/windows/libs/arm');
  shelljs.mkdir('-p', 'Assembled/windows/libs/arm/Debug');
  shelljs.mkdir('-p', 'Assembled/windows/libs/arm/Release');
  shelljs.mkdir('-p', 'Assembled/windows/libs/arm64');
  shelljs.mkdir('-p', 'Assembled/windows/libs/arm64/Debug');
  shelljs.mkdir('-p', 'Assembled/windows/libs/arm64/Release');
  shelljs.mkdir('-p', 'Assembled/windows/libs/x86');
  shelljs.mkdir('-p', 'Assembled/windows/libs/x86/Debug');
  shelljs.mkdir('-p', 'Assembled/windows/libs/x86/Release');
  shelljs.mkdir('-p', 'Assembled/windows/libs/x64');
  shelljs.mkdir('-p', 'Assembled/windows/libs/x64/Debug');
  shelljs.mkdir('-p', 'Assembled/windows/libs/x64/Release');
  shelljs.mkdir('-p', 'Assembled/windows/BabylonReactNative');
}

const copyx86DebugUWPFiles = () => {
  return gulp.src('../Modules/@babylonjs/react-native/submodules/BabylonNative/Build_uwp_x86/**/Debug/**/*.{lib,pri}')
    .pipe(rename({ dirname: '' }))
    .pipe(gulp.dest('Assembled/windows/libs/x86/Debug'));
}

const copyx86ReleaseUWPFiles = () => {
  return gulp.src('../Modules/@babylonjs/react-native/submodules/BabylonNative/Build_uwp_x86/**/Release/**/*.{lib,pri}')
    .pipe(rename({ dirname: '' }))
    .pipe(gulp.dest('Assembled/windows/libs/x86/Release'));
}

const copyx64DebugUWPFiles = () => {
  return gulp.src('../Modules/@babylonjs/react-native/submodules/BabylonNative/Build_uwp_x64/**/Debug/**/*.{lib,pri}')
    .pipe(rename({ dirname: '' }))
    .pipe(gulp.dest('Assembled/windows/libs/x64/Debug'));
}

const copyx64ReleaseUWPFiles = () => {
  return gulp.src('../Modules/@babylonjs/react-native/submodules/BabylonNative/Build_uwp_x64/**/Release/**/*.{lib,pri}')
    .pipe(rename({ dirname: '' }))
    .pipe(gulp.dest('Assembled/windows/libs/x64/Release'));
}

const copyARMDebugUWPFiles = () => {
  return gulp.src('../Modules/@babylonjs/react-native/submodules/BabylonNative/Build_uwp_arm/**/Debug/**/*.{lib,pri}')
    .pipe(rename({ dirname: '' }))
    .pipe(gulp.dest('Assembled/windows/libs/arm/Debug'));
}

const copyARMReleaseUWPFiles = () => {
  return gulp.src('../Modules/@babylonjs/react-native/submodules/BabylonNative/Build_uwp_arm/**/Release/**/*.{lib,pri}')
    .pipe(rename({ dirname: '' }))
    .pipe(gulp.dest('Assembled/windows/libs/arm/Release'));
}

const copyARM64DebugUWPFiles = () => {
  return gulp.src('../Modules/@babylonjs/react-native/submodules/BabylonNative/Build_uwp_arm64/**/Debug/**/*.{lib,pri}')
    .pipe(rename({ dirname: '' }))
    .pipe(gulp.dest('Assembled/windows/libs/arm64/Debug'));
}

const copyARM64ReleaseUWPFiles = () => {
  return gulp.src('../Modules/@babylonjs/react-native/submodules/BabylonNative/Build_uwp_arm64/**/Release/**/*.{lib,pri}')
    .pipe(rename({ dirname: '' }))
    .pipe(gulp.dest('Assembled/windows/libs/arm64/Release'));
}

const copyVCXProjUWPFiles = () => {
  return gulp.src('../Modules/@babylonjs/react-native/windows/BabylonReactNative/*.*')
    .pipe(gulp.dest('Assembled/windows/BabylonReactNative'));
}

const copyUWPFiles = gulp.parallel(
  createUWPDirectories,
  copyx86DebugUWPFiles,
  copyx86ReleaseUWPFiles,
  copyx64DebugUWPFiles,
  copyx64ReleaseUWPFiles,
  copyARMDebugUWPFiles,
  copyARMReleaseUWPFiles,
  copyARM64DebugUWPFiles,
  copyARM64ReleaseUWPFiles,
  copyVCXProjUWPFiles);

const validate = async () => {
  // When the package contents are updated *and validated*, update the expected below by running 'find Assembled | pbcopy' and pasting it over the expected string.
  // This helps ensure a bad package is not accidentally published due to tooling changes, etc.
  const expected =
`Assembled
Assembled/EngineHook.ts
Assembled/shared
Assembled/shared/BabylonNative.h
Assembled/EngineView.tsx
Assembled/ios
Assembled/ios/BabylonNativeInterop.mm
Assembled/ios/libs
Assembled/ios/libs/libxr.a
Assembled/ios/libs/libMachineIndependent.a
Assembled/ios/libs/libWindow.a
Assembled/ios/libs/libbimg.a
Assembled/ios/libs/libOGLCompiler.a
Assembled/ios/libs/libastc.a
Assembled/ios/libs/libNativeEngine.a
Assembled/ios/libs/libNativeXr.a
Assembled/ios/libs/libspirv-cross-glsl.a
Assembled/ios/libs/libNativeInput.a
Assembled/ios/libs/libJsRuntime.a
Assembled/ios/libs/libGraphics.a
Assembled/ios/libs/libOSDependent.a
Assembled/ios/libs/libXMLHttpRequest.a
Assembled/ios/libs/libUrlLib.a
Assembled/ios/libs/libastc-codec.a
Assembled/ios/libs/libGenericCodeGen.a
Assembled/ios/libs/libspirv-cross-core.a
Assembled/ios/libs/libspirv-cross-msl.a
Assembled/ios/libs/libspirv-cross-hlsl.a
Assembled/ios/libs/libbx.a
Assembled/ios/libs/libnapi.a
Assembled/ios/libs/libBabylonNative.a
Assembled/ios/libs/libSPIRV.a
Assembled/ios/libs/libbgfx.a
Assembled/ios/libs/libglslang.a
Assembled/ios/EngineViewManager.mm
Assembled/ios/BabylonNativeInterop.h
Assembled/ios/ReactNativeBabylon.xcodeproj
Assembled/ios/ReactNativeBabylon.xcodeproj/project.pbxproj
Assembled/ios/ReactNativeBabylon.xcodeproj/project.xcworkspace
Assembled/ios/ReactNativeBabylon.xcodeproj/project.xcworkspace/xcshareddata
Assembled/ios/ReactNativeBabylon.xcodeproj/project.xcworkspace/xcshareddata/WorkspaceSettings.xcsettings
Assembled/ios/BabylonModule.mm
Assembled/README.md
Assembled/package.json
Assembled/android
Assembled/android/build.gradle
Assembled/android/src
Assembled/android/src/main
Assembled/android/src/main/AndroidManifest.xml
Assembled/android/src/main/java
Assembled/android/src/main/java/com
Assembled/android/src/main/java/com/babylonreactnative
Assembled/android/src/main/java/com/babylonreactnative/BabylonNativeInterop.java
Assembled/android/src/main/java/com/babylonreactnative/SnapshotDataReturnedEvent.java
Assembled/android/src/main/java/com/babylonreactnative/BabylonModule.java
Assembled/android/src/main/java/com/babylonreactnative/BabylonPackage.java
Assembled/android/src/main/java/com/babylonreactnative/EngineViewManager.java
Assembled/android/src/main/java/com/babylonreactnative/EngineView.java
Assembled/android/src/main/jniLibs
Assembled/android/src/main/jniLibs/armeabi-v7a
Assembled/android/src/main/jniLibs/armeabi-v7a/libturbomodulejsijni.so
Assembled/android/src/main/jniLibs/armeabi-v7a/libBabylonNative.so
Assembled/android/src/main/jniLibs/x86
Assembled/android/src/main/jniLibs/x86/libturbomodulejsijni.so
Assembled/android/src/main/jniLibs/x86/libBabylonNative.so
Assembled/android/src/main/jniLibs/arm64-v8a
Assembled/android/src/main/jniLibs/arm64-v8a/libturbomodulejsijni.so
Assembled/android/src/main/jniLibs/arm64-v8a/libBabylonNative.so
Assembled/react-native-babylon.podspec
Assembled/index.ts
Assembled/BabylonModule.ts
Assembled/ReactNativeEngine.ts
`;

  const result = shelljs.exec('find Assembled', {silent: true});
  if (result.stdout != expected) {
    throw `Expected:\n${expected}\n\nActual:\n${result.stdout}`;
  }
}

const createPackage = async () => {
  exec('npm pack', 'Assembled');
};

const copyFiles = gulp.parallel(copyCommonFiles, copySharedFiles, copyIOSFiles, copyAndroidFiles);

const build = gulp.series(buildIOS, buildAndroid, createIOSUniversalLibs, copyFiles, validate);
const rebuild = gulp.series(clean, build);
const pack = gulp.series(rebuild, createPackage);

exports.buildIOS = buildIOS;
exports.buildAndroid = buildAndroid;
exports.createIOSUniversalLibs = createIOSUniversalLibs;
exports.copyFiles = copyFiles;

exports.buildUWP = buildUWP;

exports.clean = clean;
exports.build = build;
exports.rebuild = rebuild;
exports.pack = pack;

const packUWP = gulp.series(clean, buildUWP, copyCommonFiles, copySharedFiles, copyUWPFiles, createPackage);
const packUWPNoBuild = gulp.series(clean, copyCommonFiles, copySharedFiles, copyUWPFiles, createPackage);

exports.buildUWP = buildUWP;
exports.copyUWPFiles = copyUWPFiles;
exports.packUWP = packUWP;
exports.packUWPNoBuild = packUWPNoBuild;

exports.default = build;