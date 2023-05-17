const util = require('util');
const fs = require('fs');
const path = require('path');
const readdirAsync = util.promisify(fs.readdir);
const log = require('fancy-log');
const gulp = require('gulp');
const shelljs = require('shelljs');
const rename = require('gulp-rename');
const glob = require('glob');
const chalk = require('chalk');

function exec(command, workingDirectory = '.', logCommand = true) {
  if (logCommand) {
    log(command);
  }

  if (shelljs.exec(command, { fatal: true, cwd: workingDirectory }).code) {
    throw `'${command}' finished with non-zero exit code.`;
  }
}

function checkDirectory(actualList, expectedList, directoryName) {
  const extras = actualList.filter(path => !expectedList.includes(path));
  const missing = expectedList.filter(path => !actualList.includes(path));

  let isValid = true;

  if (extras.length !== 0) {
    console.error(chalk.white.bgRedBright(`The ${directoryName} directory contains unexpected files:`));
    console.log(extras);
    isValid = false;
  }

  if (missing.length !== 0) {
    console.error(chalk.white.bgRedBright(`The ${directoryName} directory is missing some expected files:`));
    console.log(missing);
    isValid = false;
  }

  if (!isValid) {
    console.log(chalk.black.bgCyan(`If the ${directoryName} directory is correct, update the file validation list in gulpfile.js with the following:`))
    console.log(actualList);
    throw `The ${directoryName} directory does not contain the expected files.`;
  }
}

const clean = async () => {
  if (shelljs.test('-d', 'Assembled')) {
    shelljs.rm('-r', 'Assembled');
  }

  if (shelljs.test('-d', 'Assembled-iOSAndroid')) {
    shelljs.rm('-r', 'Assembled-iOSAndroid');
  }

  if (shelljs.test('-d', 'Assembled-Windows')) {
    shelljs.rm('-r', 'Assembled-Windows');
  }
};

const buildTypeScript = async () => {
  exec('node_modules/typescript/bin/tsc --noEmit false --outDir ../../../Package/Assembled', '../Modules/@babylonjs/react-native');

  // Update the 'main' property in package.json to be 'index.js' instead of 'index.ts'
  const packageJson = JSON.parse(fs.readFileSync('Assembled/package.json'));
  packageJson.main = `${path.basename(packageJson.main, '.ts')}.js`;
  fs.writeFileSync('Assembled/package.json', JSON.stringify(packageJson, null, 4));
};

const makeXCodeProj = async () => {
  shelljs.mkdir('-p', 'iOS/Build');
  exec('cmake -G Xcode -DCMAKE_TOOLCHAIN_FILE=../../../Apps/Playground/Playground/node_modules/@babylonjs/react-native-iosandroid/submodules/BabylonNative/Dependencies/ios-cmake/ios.toolchain.cmake -DPLATFORM=OS64COMBINED -DENABLE_ARC=0 -DENABLE_BITCODE=1 -DDEPLOYMENT_TARGET=12 -DENABLE_PCH=OFF ..', 'iOS/Build');
};

const buildIphoneOS = async () => {
  exec('xcodebuild -sdk iphoneos -configuration Release -project ReactNativeBabylon.xcodeproj -scheme BabylonNative build CODE_SIGNING_ALLOWED=NO', 'iOS/Build');
};

const buildIphoneSimulator = async () => {
  exec('xcodebuild -sdk iphonesimulator -arch x86_64 -configuration Release -project ReactNativeBabylon.xcodeproj -scheme BabylonNative build CODE_SIGNING_ALLOWED=NO', 'iOS/Build');
};

const buildIOS = gulp.series(makeXCodeProj, buildIphoneOS, buildIphoneSimulator);

const buildAndroid = async () => {
  exec('./gradlew babylonjs_react-native:assembleRelease --stacktrace --info', '../Apps/Playground/Playground/android');
};

const initializeSubmodulesWindowsAgent = async () => {
  // windows build agents don't support the path lengths required for initializing arcore dependencies,
  // so we manually initialize the submodules we need here.
  exec('git -c submodule."Dependencies/xr/Dependencies/arcore-android-sdk".update=none submodule update --init --recursive "./../Modules/@babylonjs/react-native-iosandroid/submodules/BabylonNative');
}

const initializeSubmodulesMostRecentBabylonNative = async () => {
  let shaFound = false;
  const shaOptionIndex = process.argv.indexOf('--sha');
  if (shaOptionIndex >= 0) {
    const shaIndex = shaOptionIndex + 1;
    if (process.argv.length > shaIndex) {
      shaFound = true;
      const sha = process.argv[shaIndex];
      console.log("Using provided commit: " + sha);
      exec('git submodule init ./../Modules/@babylonjs/react-native-iosandroid/submodules/BabylonNative');
      exec('git fetch origin ' + sha, './../Modules/@babylonjs/react-native-iosandroid/submodules/BabylonNative');
      exec('git checkout ' + sha, './../Modules/@babylonjs/react-native-iosandroid/submodules/BabylonNative');
    }
  }

  if (!shaFound) {
    exec('git submodule init ./../Modules/@babylonjs/react-native/submodules/BabylonNative');
    exec('git fetch origin master', './../Modules/@babylonjs/react-native/submodules/BabylonNative');
    exec('git checkout origin/master', './../Modules/@babylonjs/react-native/submodules/BabylonNative');
  }

  if (process.argv.indexOf('--windows') >= 0) {
    exec('git -c submodule."Dependencies/xr/Dependencies/arcore-android-sdk".update=none submodule update --init --recursive *', './../Modules/@babylonjs/react-native/submodules/BabylonNative');
  } else {
    exec('git submodule update --init --recursive', './../Modules/@babylonjs/react-native/submodules/BabylonNative');
  }

  exec('git status');
}

const makeUWPProjectPlatform = async (name, arch) => {
  shelljs.mkdir('-p', `./../Modules/@babylonjs/react-native/Build/uwp_${name}`);
  exec(`cmake -D CMAKE_SYSTEM_NAME=WindowsStore -D CMAKE_SYSTEM_VERSION=10.0 -DCMAKE_UNITY_BUILD=true -A ${arch} ./../../../react-native-windows/windows`, `./../Modules/@babylonjs/react-native/Build/uwp_${name}`);
};

const makeUWPProjectx86 = async () => makeUWPProjectPlatform('x86', 'Win32');
const makeUWPProjectx64 = async () => makeUWPProjectPlatform('x64', 'x64');
const makeUWPProjectARM = async () => makeUWPProjectPlatform('arm', 'arm');
const makeUWPProjectARM64 = async () => makeUWPProjectPlatform('arm64', 'arm64');

const makeUWPProject = gulp.parallel(
  makeUWPProjectx86,
  makeUWPProjectx64,
  makeUWPProjectARM,
  makeUWPProjectARM64
);

const buildUWPx86Debug = async () => {
  exec('.\\..\\Modules\\@babylonjs\\react-native-windows\\windows\\scripts\\Build.bat -Platform Win32 -Configuration Debug');
}

const buildUWPx86Release = async () => {
  exec('.\\..\\Modules\\@babylonjs\\react-native-windows\\windows\\scripts\\Build.bat -Platform Win32 -Configuration Release');
}

const buildUWPx64Debug = async () => {
  exec('.\\..\\Modules\\@babylonjs\\react-native-windows\\windows\\scripts\\Build.bat -Platform x64 -Configuration Debug');
}

const buildUWPx64Release = async () => {
  exec('.\\..\\Modules\\@babylonjs\\react-native-windows\\windows\\scripts\\Build.bat -Platform x64 -Configuration Release');
}

const buildUWPARMDebug = async () => {
  exec('.\\..\\Modules\\@babylonjs\\react-native-windows\\windows\\scripts\\Build.bat -Platform ARM -Configuration Debug');
}

const buildUWPARMRelease = async () => {
  exec('.\\..\\Modules\\@babylonjs\\react-native-windows\\windows\\scripts\\Build.bat -Platform ARM -Configuration Release');
}

const buildUWPARM64Debug = async () => {
  exec('.\\..\\Modules\\@babylonjs\\react-native-windows\\windows\\scripts\\Build.bat -Platform ARM64 -Configuration Debug');
}

const buildUWPARM64Release = async () => {
  exec('.\\..\\Modules\\@babylonjs\\react-native-windows\\windows\\scripts\\Build.bat -Platform ARM64 -Configuration Release');
}

const buildUWPProject = gulp.parallel(
  buildUWPx86Debug,
  buildUWPx86Release,
  buildUWPx64Debug,
  buildUWPx64Release,
  buildUWPARMDebug,
  buildUWPARMRelease,
  buildUWPARM64Debug,
  buildUWPARM64Release
);

const nugetRestoreUWPPlayground = async () => {
  exec('nuget restore Playground.sln', './../Apps/Playground/Playground/windows');
}

const buildUWPPlaygroundx86Debug = async () => {
  exec('.\\..\\Modules\\@babylonjs\\react-native-windows\\windows\\scripts\\BuildPlayground.bat -Platform x86 -Configuration Debug');
}

const buildUWPPlaygroundx86Release = async () => {
  exec('.\\..\\Modules\\@babylonjs\\react-native-windows\\windows\\scripts\\BuildPlayground.bat -Platform x86 -Configuration Release');
}

const buildUWPPlaygroundx64Debug = async () => {
  exec('.\\..\\Modules\\@babylonjs\\react-native-windows\\windows\\scripts\\BuildPlayground.bat -Platform x64 -Configuration Debug');
}

const buildUWPPlaygroundx64Release = async () => {
  exec('.\\..\\Modules\\@babylonjs\\react-native-windows\\windows\\scripts\\BuildPlayground.bat -Platform x64 -Configuration Release');
}

const buildUWPPlaygroundARMDebug = async () => {
  exec('.\\..\\Modules\\@babylonjs\\react-native-windows\\windows\\scripts\\BuildPlayground.bat -Platform ARM -Configuration Debug');
}

const buildUWPPlaygroundARMRelease = async () => {
  exec('.\\..\\Modules\\@babylonjs\\react-native-windows\\windows\\scripts\\BuildPlayground.bat -Platform ARM -Configuration Release');
}

const buildUWPPlaygroundARM64Debug = async () => {
  exec('.\\..\\Modules\\@babylonjs\\react-native-windows\\windows\\scripts\\BuildPlayground.bat -Platform ARM64 -Configuration Debug');
}

const buildUWPPlaygroundARM64Release = async () => {
  exec('.\\..\\Modules\\@babylonjs\\react-native-windows\\windows\\scripts\\BuildPlayground.bat -Platform ARM64 -Configuration Release');
}

const buildUWPPlayground = gulp.parallel(
  buildUWPPlaygroundx86Debug,
  buildUWPPlaygroundx86Release,
  buildUWPPlaygroundx64Debug,
  buildUWPPlaygroundx64Release,
  buildUWPPlaygroundARMDebug,
  buildUWPPlaygroundARMRelease,
  buildUWPPlaygroundARM64Debug,
  buildUWPPlaygroundARM64Release
);

const buildUWP = gulp.series(makeUWPProject, buildUWPProject);

const copyCommonFiles = () => {
  return gulp.src('../Modules/@babylonjs/react-native/README.md')
    .pipe(gulp.dest('Assembled'));
};

const copySharedFiles = () => {
  return gulp.src('../Modules/@babylonjs/react-native/shared/BabylonNative.h')
    .pipe(gulp.src('../Modules/@babylonjs/react-native/shared/XrContextHelper.h'))
    .pipe(gulp.src('../Modules/@babylonjs/react-native/shared/XrAnchorHelper.h'))
    .pipe(gulp.dest('Assembled/shared'));
};

const copyIOSAndroidCommonFiles = () => {
  return gulp.src('../Modules/@babylonjs/react-native-iosandroid/package.json')
    .pipe(gulp.src('../Modules/@babylonjs/react-native-iosandroid/README.md'))
    .pipe(gulp.src('react-native-babylon.podspec'))
    .pipe(gulp.dest('Assembled-iOSAndroid/'));
};

const copyIOSFiles = async () => {
  await new Promise(resolve => {
    gulp.src('../Apps/Playground/Playground/node_modules/@babylonjs/react-native-iosandroid/ios/*.h')
      .pipe(gulp.src('../Apps/Playground/Playground/node_modules/@babylonjs/react-native-iosandroid/ios/*.mm'))
      // This xcodeproj is garbage that we don't need in the package, but `pod install` ignores the package if it doesn't contain at least one xcodeproj. 🤷🏼‍♂️
      .pipe(gulp.src('iOS/Build/ReactNativeBabylon.xcodeproj**/**/*'))
      .pipe(gulp.dest('Assembled-iOSAndroid/ios'))
      .on('end', resolve);
  });

  await new Promise(resolve => {
    gulp.src('../Apps/Playground/Playground/node_modules/@babylonjs/react-native-iosandroid/submodules/BabylonNative/Dependencies/xr/Source/ARKit/Include/*')
      .pipe(gulp.dest('Assembled-iOSAndroid/ios/include'))
      .on('end', resolve);
  });
};

const createIOSUniversalLibs = async () => {
  shelljs.mkdir('-p', 'Assembled-iOSAndroid/ios/libs');
  const libs = await readdirAsync('iOS/Build/Release-iphoneos');
  libs.map(lib => exec(`lipo -create iOS/Build/Release-iphoneos/${lib} iOS/Build/Release-iphonesimulator/${lib} -output Assembled-iOSAndroid/ios/libs/${lib}`));
};

const copyAndroidFiles = async () => {
  await new Promise(resolve => {
    gulp.src('Android/build.gradle')
      .pipe(gulp.src('../Apps/Playground/Playground/node_modules/@babylonjs/react-native-iosandroid/android/src**/main/AndroidManifest.xml'))
      .pipe(gulp.src('../Apps/Playground/Playground/node_modules/@babylonjs/react-native-iosandroid/android/src**/main/java/**/*'))
      .pipe(gulp.dest('Assembled-iOSAndroid/android'))
      .on('end', resolve);
  });

  await new Promise(resolve => {
    gulp.src('../Apps/Playground/Playground/node_modules/@babylonjs/react-native-iosandroid/submodules/BabylonNative/Dependencies/xr/Source/ARCore/Include/*')
      .pipe(gulp.dest('Assembled-iOSAndroid/android/include'))
      .on('end', resolve);
  });

  await new Promise(resolve => {
    const jnidir = '../Apps/Playground/Playground/node_modules/@babylonjs/react-native-iosandroid/android/build/intermediates/library_and_local_jars_jni/release/jni/**';
    gulp.src(`${jnidir}/libBabylonNative.so`)
      .pipe(gulp.dest('Assembled-iOSAndroid/android/src/main/jniLibs/'))
      .on('end', resolve);
  });

  // This is no longer found in the directory above because it is explicitly excluded because Playground has been updated to RN 0.64 which includes
  // the real implementation of libturbomodulejsijni.so, but we still need to support RN 0.63 consumers, so grab this one explicitly to include it in the package.

  const versionIndex = process.argv.indexOf('--reactNative');
  if (versionIndex != -1 && process.argv[versionIndex + 1] != '0.71') {
    await new Promise(resolve => {
      gulp.src('../Apps/Playground/Playground/node_modules/@babylonjs/react-native-iosandroid/android/build/intermediates/cmake/release/obj/{arm64-v8a,armeabi-v7a,x86}/libturbomodulejsijni.so')
        .pipe(gulp.dest('Assembled-iOSAndroid/android/src/main/jniLibs/'))
        .on('end', resolve);
    });
  }
};

const createUWPDirectories = async () => {
  shelljs.mkdir('-p', 'Assembled-Windows');
  shelljs.mkdir('-p', 'Assembled-Windows/windows');
  shelljs.mkdir('-p', 'Assembled-Windows/windows/libs');
  shelljs.mkdir('-p', 'Assembled-Windows/windows/libs/arm');
  shelljs.mkdir('-p', 'Assembled-Windows/windows/libs/arm/Debug');
  shelljs.mkdir('-p', 'Assembled-Windows/windows/libs/arm/Release');
  shelljs.mkdir('-p', 'Assembled-Windows/windows/libs/arm64');
  shelljs.mkdir('-p', 'Assembled-Windows/windows/libs/arm64/Debug');
  shelljs.mkdir('-p', 'Assembled-Windows/windows/libs/arm64/Release');
  shelljs.mkdir('-p', 'Assembled-Windows/windows/libs/x86');
  shelljs.mkdir('-p', 'Assembled-Windows/windows/libs/x86/Debug');
  shelljs.mkdir('-p', 'Assembled-Windows/windows/libs/x86/Release');
  shelljs.mkdir('-p', 'Assembled-Windows/windows/libs/x64');
  shelljs.mkdir('-p', 'Assembled-Windows/windows/libs/x64/Debug');
  shelljs.mkdir('-p', 'Assembled-Windows/windows/libs/x64/Release');
  shelljs.mkdir('-p', 'Assembled-Windows/windows/BabylonReactNative');
  shelljs.mkdir('-p', 'Assembled-Windows/windows/include');
  shelljs.mkdir('-p', 'Assembled-Windows/windows/OpenXR-MixedReality/include/openxr');
  shelljs.mkdir('-p', 'Assembled-Windows/windows/OpenXR-MixedReality/include/XrUtility');
}

const copyCommonFilesUWP = () => {
  return gulp.src('../Modules/@babylonjs/react-native-windows/package.json')
    .pipe(gulp.src('../Modules/@babylonjs/react-native-windows/README.md'))
    .pipe(gulp.src('../Modules/@babylonjs/react-native-windows/*.ts*'))
    .pipe(gulp.dest('Assembled-Windows'));
}

const copyx86DebugUWPFiles = () => {
  return gulp.src('../Modules/@babylonjs/react-native/Build/uwp_x86/**/Debug/**/*.{lib,pri}')
    .pipe(rename({ dirname: '' }))
    .pipe(gulp.dest('Assembled-Windows/windows/libs/x86/Debug'));
}

const copyx86ReleaseUWPFiles = () => {
  return gulp.src('../Modules/@babylonjs/react-native/Build/uwp_x86/**/Release/**/*.{lib,pri}')
    .pipe(rename({ dirname: '' }))
    .pipe(gulp.dest('Assembled-Windows/windows/libs/x86/Release'));
}

const copyx64DebugUWPFiles = () => {
  return gulp.src('../Modules/@babylonjs/react-native/Build/uwp_x64/**/Debug/**/*.{lib,pri}')
    .pipe(rename({ dirname: '' }))
    .pipe(gulp.dest('Assembled-Windows/windows/libs/x64/Debug'));
}

const copyx64ReleaseUWPFiles = () => {
  return gulp.src('../Modules/@babylonjs/react-native/Build/uwp_x64/**/Release/**/*.{lib,pri}')
    .pipe(rename({ dirname: '' }))
    .pipe(gulp.dest('Assembled-Windows/windows/libs/x64/Release'));
}

const copyARMDebugUWPFiles = () => {
  return gulp.src('../Modules/@babylonjs/react-native/Build/uwp_arm/**/Debug/**/*.{lib,pri}')
    .pipe(rename({ dirname: '' }))
    .pipe(gulp.dest('Assembled-Windows/windows/libs/arm/Debug'));
}

const copyARMReleaseUWPFiles = () => {
  return gulp.src('../Modules/@babylonjs/react-native/Build/uwp_arm/**/Release/**/*.{lib,pri}')
    .pipe(rename({ dirname: '' }))
    .pipe(gulp.dest('Assembled-Windows/windows/libs/arm/Release'));
}

const copyARM64DebugUWPFiles = () => {
  return gulp.src('../Modules/@babylonjs/react-native/Build/uwp_arm64/**/Debug/**/*.{lib,pri}')
    .pipe(rename({ dirname: '' }))
    .pipe(gulp.dest('Assembled-Windows/windows/libs/arm64/Debug'));
}

const copyARM64ReleaseUWPFiles = () => {
  return gulp.src('../Modules/@babylonjs/react-native/Build/uwp_arm64/**/Release/**/*.{lib,pri}')
    .pipe(rename({ dirname: '' }))
    .pipe(gulp.dest('Assembled-Windows/windows/libs/arm64/Release'));
}

const copyVCXProjUWPFiles = () => {
  const uwpFilesDir = '../Modules/@babylonjs/react-native-windows/windows/BabylonReactNative';
  return gulp.src([`${uwpFilesDir}/*.*`, `!${uwpFilesDir}/*.pfx`])
    .pipe(gulp.dest('Assembled-Windows/windows/BabylonReactNative'));
}

const copyOpenXRInfoFiles = () => {
  return gulp.src('../Modules/@babylonjs/react-native-iosandroid/submodules/BabylonNative/Dependencies/xr/Dependencies/OpenXR-MixedReality/LICENSE')
    .pipe(gulp.src('../Modules/@babylonjs/react-native-iosandroid/submodules/BabylonNative/Dependencies/xr/Dependencies/OpenXR-MixedReality/README.md'))
    .pipe(gulp.dest('Assembled-Windows/windows/OpenXR-MixedReality'));
}

const copyOpenXRPreviewHeaders = () => {
  return gulp.src('../Modules/@babylonjs/react-native-iosandroid/submodules/BabylonNative/Dependencies/xr/Dependencies/OpenXR-MixedReality/openxr_preview/include/openxr/*')
    .pipe(gulp.dest('Assembled-Windows/windows/OpenXR-MixedReality/include/openxr'));
}

const copyOpenXRUtilityHeaders = () => {
  return gulp.src('../Modules/@babylonjs/react-native-iosandroid/submodules/BabylonNative/Dependencies/xr/Dependencies/OpenXR-MixedReality/shared/XrUtility/*')
    .pipe(gulp.dest('Assembled-Windows/windows/OpenXR-MixedReality/include/XrUtility'));
}

const copyOpenXRHelperHeaders = () => {
  return gulp.src('../Modules/@babylonjs/react-native-iosandroid/submodules/BabylonNative/Dependencies/xr/Source/OpenXR/Include/*')
    .pipe(gulp.src('../Modules/@babylonjs/react-native-windows/windows/include/*'))
    .pipe(gulp.dest('Assembled-Windows/windows/include'));
}

const copyUWPFiles = gulp.series(
  createUWPDirectories,
  gulp.parallel(
    copyCommonFilesUWP,
    copyx86DebugUWPFiles,
    copyx86ReleaseUWPFiles,
    copyx64DebugUWPFiles,
    copyx64ReleaseUWPFiles,
    copyARMDebugUWPFiles,
    copyARMReleaseUWPFiles,
    copyARM64DebugUWPFiles,
    copyARM64ReleaseUWPFiles,
    copyVCXProjUWPFiles,
    copyOpenXRInfoFiles,
    copyOpenXRPreviewHeaders,
    copyOpenXRUtilityHeaders,
    copyOpenXRHelperHeaders));

const validateAssembled = async () => {
  // When the package contents are updated *and validated*, update the expected below from the output of the failed validation console output (run `gulp validateAssembled`).
  // This helps ensure a bad package is not accidentally published due to tooling changes, etc.
  const expected = [
    'Assembled/BabylonModule.d.ts',
    'Assembled/BabylonModule.js',
    'Assembled/BabylonModule.js.map',
    'Assembled/EngineHook.d.ts',
    'Assembled/EngineHook.js',
    'Assembled/EngineHook.js.map',
    'Assembled/EngineView.d.ts',
    'Assembled/EngineView.js',
    'Assembled/EngineView.js.map',
    'Assembled/index.d.ts',
    'Assembled/index.js',
    'Assembled/index.js.map',
    'Assembled/NativeCapture.d.ts',
    'Assembled/NativeCapture.js',
    'Assembled/NativeCapture.js.map',
    'Assembled/NativeEngineHook.d.ts',
    'Assembled/NativeEngineHook.js',
    'Assembled/NativeEngineHook.js.map',
    'Assembled/NativeEngineView.d.ts',
    'Assembled/NativeEngineView.js',
    'Assembled/NativeEngineView.js.map',
    'Assembled/FontFace.d.ts',
    'Assembled/FontFace.js',
    'Assembled/FontFace.js.map',
    'Assembled/package.json',
    'Assembled/ReactNativeEngine.d.ts',
    'Assembled/ReactNativeEngine.js',
    'Assembled/ReactNativeEngine.js.map',
    'Assembled/README.md',
    'Assembled/shared',
    'Assembled/shared/BabylonNative.h',
    'Assembled/shared/XrAnchorHelper.h',
    'Assembled/shared/XrContextHelper.h',
    'Assembled/VersionValidation.d.ts',
    'Assembled/VersionValidation.js',
    'Assembled/VersionValidation.js.map'
  ];

  const actual = glob.sync('Assembled/**/*');
  checkDirectory(actual, expected, 'Assembled');
}

const validateAssemblediOSAndroid = async () => {
  let expectediosandroid = [
    'Assembled-iOSAndroid/android',
    'Assembled-iOSAndroid/android/build.gradle',
    'Assembled-iOSAndroid/android/include',
    'Assembled-iOSAndroid/android/include/IXrContextARCore.h',
    'Assembled-iOSAndroid/android/src',
    'Assembled-iOSAndroid/android/src/main',
    'Assembled-iOSAndroid/android/src/main/AndroidManifest.xml',
    'Assembled-iOSAndroid/android/src/main/java',
    'Assembled-iOSAndroid/android/src/main/java/com',
    'Assembled-iOSAndroid/android/src/main/java/com/babylonreactnative',
    'Assembled-iOSAndroid/android/src/main/java/com/babylonreactnative/BabylonModule.java',
    'Assembled-iOSAndroid/android/src/main/java/com/babylonreactnative/BabylonNativeInterop.java',
    'Assembled-iOSAndroid/android/src/main/java/com/babylonreactnative/BabylonPackage.java',
    'Assembled-iOSAndroid/android/src/main/java/com/babylonreactnative/EngineView.java',
    'Assembled-iOSAndroid/android/src/main/java/com/babylonreactnative/EngineViewManager.java',
    'Assembled-iOSAndroid/android/src/main/java/com/babylonreactnative/SnapshotDataReturnedEvent.java',
    'Assembled-iOSAndroid/android/src/main/jniLibs',
    'Assembled-iOSAndroid/android/src/main/jniLibs/arm64-v8a',
    'Assembled-iOSAndroid/android/src/main/jniLibs/arm64-v8a/libBabylonNative.so',
    'Assembled-iOSAndroid/android/src/main/jniLibs/armeabi-v7a',
    'Assembled-iOSAndroid/android/src/main/jniLibs/armeabi-v7a/libBabylonNative.so',
    'Assembled-iOSAndroid/android/src/main/jniLibs/x86',
    'Assembled-iOSAndroid/android/src/main/jniLibs/x86/libBabylonNative.so',
    'Assembled-iOSAndroid/android/src/main/jniLibs/x86_64',
    'Assembled-iOSAndroid/android/src/main/jniLibs/x86_64/libBabylonNative.so',
    'Assembled-iOSAndroid/ios',
    'Assembled-iOSAndroid/ios/BabylonModule.mm',
    'Assembled-iOSAndroid/ios/BabylonNativeInterop.h',
    'Assembled-iOSAndroid/ios/BabylonNativeInterop.mm',
    'Assembled-iOSAndroid/ios/EngineViewManager.mm',
    'Assembled-iOSAndroid/ios/include',
    'Assembled-iOSAndroid/ios/include/IXrContextARKit.h',
    'Assembled-iOSAndroid/ios/libs',
    'Assembled-iOSAndroid/ios/libs/libastc-encoder.a',
    'Assembled-iOSAndroid/ios/libs/libBabylonNative.a',
    'Assembled-iOSAndroid/ios/libs/libbgfx.a',
    'Assembled-iOSAndroid/ios/libs/libbimg.a',
    'Assembled-iOSAndroid/ios/libs/libbx.a',
    'Assembled-iOSAndroid/ios/libs/libCanvas.a',
    'Assembled-iOSAndroid/ios/libs/libGenericCodeGen.a',
    'Assembled-iOSAndroid/ios/libs/libglslang.a',
    'Assembled-iOSAndroid/ios/libs/libGraphics.a',
    'Assembled-iOSAndroid/ios/libs/libJsRuntime.a',
    'Assembled-iOSAndroid/ios/libs/libMachineIndependent.a',
    'Assembled-iOSAndroid/ios/libs/libnapi.a',
    'Assembled-iOSAndroid/ios/libs/libNativeCamera.a',
    'Assembled-iOSAndroid/ios/libs/libNativeCapture.a',
    'Assembled-iOSAndroid/ios/libs/libNativeEngine.a',
    'Assembled-iOSAndroid/ios/libs/libNativeInput.a',
    'Assembled-iOSAndroid/ios/libs/libNativeOptimizations.a',
    'Assembled-iOSAndroid/ios/libs/libNativeTracing.a',
    'Assembled-iOSAndroid/ios/libs/libNativeXr.a',
    'Assembled-iOSAndroid/ios/libs/libOGLCompiler.a',
    'Assembled-iOSAndroid/ios/libs/libOSDependent.a',
    'Assembled-iOSAndroid/ios/libs/libspirv-cross-core.a',
    'Assembled-iOSAndroid/ios/libs/libspirv-cross-glsl.a',
    'Assembled-iOSAndroid/ios/libs/libspirv-cross-msl.a',
    'Assembled-iOSAndroid/ios/libs/libSPIRV.a',
    'Assembled-iOSAndroid/ios/libs/libtinyexr.a',
    'Assembled-iOSAndroid/ios/libs/libetc1.a',
    'Assembled-iOSAndroid/ios/libs/libetc2.a',
    'Assembled-iOSAndroid/ios/libs/libnvtt.a',
    'Assembled-iOSAndroid/ios/libs/libsquish.a',
    'Assembled-iOSAndroid/ios/libs/libpvrtc.a',
    'Assembled-iOSAndroid/ios/libs/libiqa.a',
    'Assembled-iOSAndroid/ios/libs/libedtaa3.a',
    'Assembled-iOSAndroid/ios/libs/libUrlLib.a',
    'Assembled-iOSAndroid/ios/libs/libWindow.a',
    'Assembled-iOSAndroid/ios/libs/libXMLHttpRequest.a',
    'Assembled-iOSAndroid/ios/libs/libxr.a',
    'Assembled-iOSAndroid/ios/ReactNativeBabylon.xcodeproj',
    'Assembled-iOSAndroid/ios/ReactNativeBabylon.xcodeproj/project.pbxproj',
    'Assembled-iOSAndroid/ios/ReactNativeBabylon.xcodeproj/project.xcworkspace',
    'Assembled-iOSAndroid/ios/ReactNativeBabylon.xcodeproj/project.xcworkspace/xcshareddata',
    'Assembled-iOSAndroid/ios/ReactNativeBabylon.xcodeproj/project.xcworkspace/xcshareddata/WorkspaceSettings.xcsettings',
    'Assembled-iOSAndroid/package.json',
    'Assembled-iOSAndroid/react-native-babylon.podspec',
    'Assembled-iOSAndroid/README.md',
  ];

  const versionIndex = process.argv.indexOf('--reactNative');	
  if (versionIndex != -1) {	
    if (process.argv[versionIndex + 1] !== '0.71') {	
      const expectediosandroidNot071 = [
        'Assembled-iOSAndroid/android/src/main/jniLibs/arm64-v8a/libturbomodulejsijni.so',
        'Assembled-iOSAndroid/android/src/main/jniLibs/armeabi-v7a/libturbomodulejsijni.so',
        'Assembled-iOSAndroid/android/src/main/jniLibs/x86/libturbomodulejsijni.so',
      ];
      expectediosandroid = expectediosandroid.concat(expectediosandroidNot071);
    }
  }
  const actualiosandroid = glob.sync('Assembled-iOSAndroid/**/*');
  checkDirectory(actualiosandroid, expectediosandroid, 'Assembled-iOSAndroid');
}

const createPackage = async () => {
  exec('npm pack', 'Assembled');
};

const createPackageiOSAndroid = async () => {
  exec('npm pack', 'Assembled-iOSAndroid');
};

const createPackageUWP = async () => {
  exec('npm pack', 'Assembled-Windows');
}

const patchPackageVersion = async () => {
  const releaseVersionIndex = process.argv.indexOf('--releaseVersion');
  const versionIndex = process.argv.indexOf('--reactNative');
  if (releaseVersionIndex != -1 || versionIndex != -1) {

    const packageJsonPath = '../Modules/@babylonjs/react-native/package.json';
    const packageJsonPathWindows = '../Modules/@babylonjs/react-native-windows/package.json';
    const packageJsonPathiOSAndroid = '../Modules/@babylonjs/react-native-iosandroid/package.json';
    const packageJson = JSON.parse(fs.readFileSync(packageJsonPath));
    const packageJsonWindows = JSON.parse(fs.readFileSync(packageJsonPathWindows));
    const packageJsoniOSAndroid = JSON.parse(fs.readFileSync(packageJsonPathiOSAndroid));

    if (versionIndex != -1) {
      const version = process.argv[versionIndex + 1];
      if (version == '0.64' || version == '0.65' || version == '0.69' || version == '0.70' || version == '0.71') {
        console.log(chalk.black.bgCyan(`Updating Package.json for React Native ${version}.`));

        // default 0.64
        let peerDep = '>=0.63.1 <0.65.0';
        let packageNamePostfix = '-0-64';
        if (version == '0.65') {
          peerDep = '>=0.65.0 < 0.69.0';
          packageNamePostfix = '-0-65';
        } else if (version == '0.69') {
          peerDep = '>=0.69.0 < 0.70.0';
          packageNamePostfix = '-0-69';
        } else if (version == '0.70') {
          peerDep = '>=0.70.0 < 0.71.0';
          packageNamePostfix = '-0-70';
        } else if (version == '0.71') {
          peerDep = '>=0.71.0';
          packageNamePostfix = '-0-71';
        }

        packageJsoniOSAndroid["name"] = "@babylonjs/react-native-iosandroid" + packageNamePostfix;
        packageJsonWindows["name"] = "@babylonjs/react-native-windows" + packageNamePostfix;

        packageJson.peerDependencies['react-native'] = peerDep;
        packageJsoniOSAndroid.peerDependencies['react-native'] = peerDep;
        packageJsonWindows.peerDependencies['react-native'] = peerDep;
        packageJsonWindows.peerDependencies['react-native-windows'] = peerDep;
      }
    }
    // release version
    if (releaseVersionIndex !== -1) {
      const releaseVersion = process.argv[releaseVersionIndex + 1];
      console.log(chalk.black.bgCyan(`Updating Package.json for Release version ${releaseVersion}.`));
      packageJsonWindows.peerDependencies["@babylonjs/react-native"] = releaseVersion;
      packageJsoniOSAndroid.peerDependencies["@babylonjs/react-native"] = releaseVersion;

    /* version tag is done when publishing
      packageJsonWindows["version"] = releaseVersion;
      packageJsoniOSAndroid["version"] = releaseVersion;
      packageJson["version"] = releaseVersion;
      */
    }

    fs.writeFileSync(packageJsonPath, JSON.stringify(packageJson, null, 2));
    fs.writeFileSync(packageJsonPathWindows, JSON.stringify(packageJsonWindows, null, 2));
    fs.writeFileSync(packageJsonPathiOSAndroid, JSON.stringify(packageJsoniOSAndroid, null, 2));
  } else {
    console.log(chalk.black.bgCyan(`No valid React Native version set. Letting Package.json unchanged.`))
  }
}

const copyFiles = gulp.parallel(copyIOSAndroidCommonFiles, copyIOSFiles, copyAndroidFiles);

const buildTS = gulp.series(patchPackageVersion, copyCommonFiles, copySharedFiles, buildTypeScript, validateAssembled);
const build = gulp.series(patchPackageVersion, buildIOS, buildAndroid, createIOSUniversalLibs, copyFiles, validateAssemblediOSAndroid);
const rebuild = gulp.series(clean, build);
const pack = gulp.series(rebuild, createPackage);

exports.validateAssembled = validateAssembled;
exports.validateAssemblediOSAndroid = validateAssemblediOSAndroid;

exports.buildIOS = buildIOS;
exports.buildAndroid = buildAndroid;
exports.createIOSUniversalLibs = createIOSUniversalLibs;
exports.copyFiles = copyFiles;

exports.clean = clean;
exports.build = build;
exports.rebuild = rebuild;
exports.pack = pack;

const packAndroid = gulp.series(clean, buildAndroid, copyFiles, createPackage, createPackageiOSAndroid);
exports.buildAndroid = buildAndroid;
exports.packAndroid = packAndroid;

const copyPackageFilesUWP = gulp.series(copyUWPFiles);
const buildUWPPublish = gulp.series(buildUWP, copyPackageFilesUWP);
const packUWP = gulp.series(clean, buildUWP, copyPackageFilesUWP, createPackage, createPackageUWP);
const packUWPNoBuild = gulp.series(clean, copyPackageFilesUWP, createPackage, createPackageUWP);

exports.buildTS = buildTS;
exports.initializeSubmodulesWindowsAgent = gulp.series(patchPackageVersion, initializeSubmodulesWindowsAgent);
exports.makeUWPProjectx86 = makeUWPProjectx86;
exports.makeUWPProjectx64 = makeUWPProjectx64;
exports.makeUWPProjectARM = makeUWPProjectARM;
exports.makeUWPProjectARM64 = makeUWPProjectARM64;
exports.makeUWPProject = makeUWPProject;

exports.buildUWPx86Debug = buildUWPx86Debug;
exports.buildUWPx86Release = buildUWPx86Release;
exports.buildUWPx64Debug = buildUWPx64Debug;
exports.buildUWPx64Release = buildUWPx64Release;
exports.buildUWPARMDebug = buildUWPARMDebug;
exports.buildUWPARMRelease = buildUWPARMRelease;
exports.buildUWPARM64Debug = buildUWPARM64Debug;
exports.buildUWPARM64Release = buildUWPARM64Release;
exports.buildUWPProject = buildUWPProject;

exports.nugetRestoreUWPPlayground = nugetRestoreUWPPlayground;
exports.buildUWPPlaygroundx86Debug = buildUWPPlaygroundx86Debug;
exports.buildUWPPlaygroundx86Release = buildUWPPlaygroundx86Release;
exports.buildUWPPlaygroundx64Debug = buildUWPPlaygroundx64Debug;
exports.buildUWPPlaygroundx64Release = buildUWPPlaygroundx64Release;
exports.buildUWPPlaygroundARMDebug = buildUWPPlaygroundARMDebug;
exports.buildUWPPlaygroundARMRelease = buildUWPPlaygroundARMRelease;
exports.buildUWPPlaygroundARM64Debug = buildUWPPlaygroundARM64Debug;
exports.buildUWPPlaygroundARM64Release = buildUWPPlaygroundARM64Release;
exports.buildUWPPlayground = buildUWPPlayground;

exports.buildUWP = buildUWP;
exports.buildUWPPublish = buildUWPPublish;

exports.copyUWPFiles = copyUWPFiles;
exports.packUWP = packUWP;
exports.packUWPNoBuild = packUWPNoBuild;

exports.initializeSubmodulesMostRecentBabylonNative = initializeSubmodulesMostRecentBabylonNative;

exports.default = build;
