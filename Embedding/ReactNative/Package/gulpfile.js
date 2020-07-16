const util = require('util');
const fs = require('fs');
const readdirAsync = util.promisify(fs.readdir);
const log = require('fancy-log');
const gulp = require('gulp');
const shelljs = require('shelljs');

function exec(command, workingDirectory = '.', logCommand = true) {
  if (logCommand) {
    log(command);
  }

  shelljs.pushd('-q', workingDirectory);
  try {
    if (shelljs.exec(command, {fatal: true}).code) {
      throw `'${command}' finished with non-zero exit code.`;
    }
  } finally {
    shelljs.popd('-q');
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
  exec('xcodebuild -sdk iphonesimulator -configuration Release -project ReactNativeBabylon.xcodeproj -scheme BabylonNative build CODE_SIGNING_ALLOWED=NO', 'iOS/Build');
};

const buildIOS = gulp.series(makeXCodeProj, buildIphoneOS, buildIphoneSimulator);

const buildAndroid = async () => {
  exec('./gradlew babylonjs_react-native:assembleRelease', '../Apps/Playground/android');
};

const copyCommonFiles = () => {
  return  gulp.src('../Apps/Playground/node_modules/@babylonjs/react-native/package.json')
    .pipe(gulp.src('../Apps/Playground/node_modules/@babylonjs/react-native/README.md'))
    .pipe(gulp.src('../Apps/Playground/node_modules/@babylonjs/react-native/*.ts*'))
    .pipe(gulp.src('react-native-babylon.podspec'))
    .pipe(gulp.dest('Assembled'));
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

const validate = async () => {
  // When the package contents are updated *and validated*, update the expected below by running 'find Assembled | pbcopy' and pasting it over the expected string.
  // This helps ensure a bad package is not accidentally published due to tooling changes, etc.
  const expected =
`Assembled
Assembled/EngineHook.ts
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
Assembled/ios/libs/libNativeWindow.a
Assembled/ios/libs/libNativeEngine.a
Assembled/ios/libs/libNativeXr.a
Assembled/ios/libs/libspirv-cross-glsl.a
Assembled/ios/libs/libNativeInput.a
Assembled/ios/libs/libJsRuntime.a
Assembled/ios/libs/libOSDependent.a
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
Assembled/ios/BabylonNative.h
Assembled/README.md
Assembled/EngineHelpers.ts
Assembled/package.json
Assembled/android
Assembled/android/build.gradle
Assembled/android/src
Assembled/android/src/main
Assembled/android/src/main/AndroidManifest.xml
Assembled/android/src/main/java
Assembled/android/src/main/java/com
Assembled/android/src/main/java/com/reactlibrary
Assembled/android/src/main/java/com/reactlibrary/BabylonNativeInterop.java
Assembled/android/src/main/java/com/reactlibrary/BabylonModule.java
Assembled/android/src/main/java/com/reactlibrary/BabylonPackage.java
Assembled/android/src/main/java/com/reactlibrary/EngineViewManager.java
Assembled/android/src/main/java/com/reactlibrary/EngineView.java
Assembled/android/src/main/jniLibs
Assembled/android/src/main/jniLibs/armeabi-v7a
Assembled/android/src/main/jniLibs/armeabi-v7a/libBabylonNative.so
Assembled/android/src/main/jniLibs/x86
Assembled/android/src/main/jniLibs/x86/libBabylonNative.so
Assembled/android/src/main/jniLibs/arm64-v8a
Assembled/android/src/main/jniLibs/arm64-v8a/libBabylonNative.so
Assembled/react-native-babylon.podspec
Assembled/index.ts
Assembled/BabylonModule.ts
`;

  const result = shelljs.exec('find Assembled', {silent: true});
  if (result.stdout != expected) {
    throw `Expected:\n${expected}\n\nActual:\n${result.stdout}`;
  }
}

const createPackage = async () => {
  exec('npm pack', 'Assembled');
};

const copyFiles = gulp.parallel(copyCommonFiles, copyIOSFiles, copyAndroidFiles);

const build = gulp.series(buildIOS, buildAndroid, createIOSUniversalLibs, copyFiles, validate);
const rebuild = gulp.series(clean, build);
const pack = gulp.series(rebuild, createPackage);

exports.buildIOS = buildIOS;
exports.buildAndroid = buildAndroid;
exports.createIOSUniversalLibs = createIOSUniversalLibs;
exports.copyFiles = copyFiles;

exports.clean = clean;
exports.build = build;
exports.rebuild = rebuild;
exports.pack = pack;

exports.default = build;