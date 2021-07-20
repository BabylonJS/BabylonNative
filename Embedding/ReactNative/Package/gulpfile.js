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

  if (shelljs.exec(command, {fatal: true, cwd: workingDirectory}).code) {
    throw `'${command}' finished with non-zero exit code.`;
  }
}

const clean = async () => {
  if (shelljs.test('-d', 'Assembled')) {
    shelljs.rm('-r', 'Assembled');
  }

  if (shelljs.test('-d', 'Assembled-Windows')) {
    shelljs.rm('-r', 'Assembled');
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
  exec('cmake -G Xcode -DCMAKE_TOOLCHAIN_FILE=../../../Apps/Playground/node_modules/@babylonjs/react-native/submodules/BabylonNative/Dependencies/ios-cmake/ios.toolchain.cmake -DPLATFORM=OS64COMBINED -DENABLE_ARC=0 -DENABLE_BITCODE=1 -DDEPLOYMENT_TARGET=12 -DENABLE_GLSLANG_BINARIES=OFF -DSPIRV_CROSS_CLI=OFF -DENABLE_PCH=OFF ..', 'iOS/Build');
};

const buildIphoneOS = async () => {
  exec('xcodebuild -sdk iphoneos -configuration Release -project ReactNativeBabylon.xcodeproj -scheme BabylonNative build CODE_SIGNING_ALLOWED=NO', 'iOS/Build');
};

const buildIphoneSimulator = async () => {
  exec('xcodebuild -sdk iphonesimulator -arch x86_64 -configuration Release -project ReactNativeBabylon.xcodeproj -scheme BabylonNative build CODE_SIGNING_ALLOWED=NO', 'iOS/Build');
};

const buildIOS = gulp.series(makeXCodeProj, buildIphoneOS, buildIphoneSimulator);

const buildAndroid = async () => {
  exec('./gradlew babylonjs_react-native:assembleRelease --stacktrace --info', '../Apps/Playground/android');
};

const initializeSubmodulesWindowsAgent = async () => {
  // windows build agents don't support the path lengths required for initializing arcore dependencies,
  // so we manually initialize the submodules we need here.
  exec('git -c submodule."Dependencies/xr/Dependencies/arcore-android-sdk".update=none submodule update --init --recursive "./../Modules/@babylonjs/react-native/submodules/BabylonNative');
}

const initializeSubmodulesMostRecentBabylonNative = async () => {
  let shaFound = false;
  const shaOptionIndex = process.argv.indexOf('--sha');
  if (shaOptionIndex >= 0)
  {
    const shaIndex = shaOptionIndex + 1;
    if (process.argv.length > shaIndex)
    {
      shaFound = true;
      const sha = process.argv[shaIndex];
      console.log("Using provided commit: " + sha);
      exec('git submodule init ./../Modules/@babylonjs/react-native/submodules/BabylonNative');
      exec('git fetch origin ' + sha, './../Modules/@babylonjs/react-native/submodules/BabylonNative');
      exec('git checkout ' + sha, './../Modules/@babylonjs/react-native/submodules/BabylonNative');
    }
  }

  if (!shaFound)
  {
    exec('git submodule init ./../Modules/@babylonjs/react-native/submodules/BabylonNative');
    exec('git fetch origin master', './../Modules/@babylonjs/react-native/submodules/BabylonNative');
    exec('git checkout origin/master', './../Modules/@babylonjs/react-native/submodules/BabylonNative');
  }

  if (process.argv.indexOf('--windows') >= 0)
  {
    exec('git -c submodule."Dependencies/xr/Dependencies/arcore-android-sdk".update=none submodule update --init --recursive *', './../Modules/@babylonjs/react-native/submodules/BabylonNative');
  }
  else
  {
    exec('git submodule update --init --recursive *', './../Modules/@babylonjs/react-native/submodules/BabylonNative');
  }

  exec('git status');
}

const makeUWPProjectx86 = async () => {
  shelljs.mkdir('-p', './../Modules/@babylonjs/react-native/Build/uwp_x86');
  exec('cmake -D CMAKE_SYSTEM_NAME=WindowsStore -D CMAKE_SYSTEM_VERSION=10.0 -D NAPI_JAVASCRIPT_ENGINE=JSI -A Win32 ./../../../react-native-windows/windows', './../Modules/@babylonjs/react-native/Build/uwp_x86');
}

const makeUWPProjectx64 = async () => {
  shelljs.mkdir('-p', './../Modules/@babylonjs/react-native/Build/uwp_x64');
  exec('cmake -D CMAKE_SYSTEM_NAME=WindowsStore -D CMAKE_SYSTEM_VERSION=10.0 -D NAPI_JAVASCRIPT_ENGINE=JSI ./../../../react-native-windows/windows', './../Modules/@babylonjs/react-native/Build/uwp_x64');
}

const makeUWPProjectARM = async () => {
  shelljs.mkdir('-p', './../Modules/@babylonjs/react-native/Build/uwp_arm');
  exec('cmake -D CMAKE_SYSTEM_NAME=WindowsStore -D CMAKE_SYSTEM_VERSION=10.0 -D NAPI_JAVASCRIPT_ENGINE=JSI -A arm ./../../../react-native-windows/windows', './../Modules/@babylonjs/react-native/Build/uwp_arm');
}

const makeUWPProjectARM64 = async () => {
  shelljs.mkdir('-p', './../Modules/@babylonjs/react-native/Build/uwp_arm64');
  exec('cmake -D CMAKE_SYSTEM_NAME=WindowsStore -D CMAKE_SYSTEM_VERSION=10.0 -D NAPI_JAVASCRIPT_ENGINE=JSI -A arm64 ./../../../react-native-windows/windows', './../Modules/@babylonjs/react-native/Build/uwp_arm64');
}

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
  exec('nuget restore Playground.sln', './../Apps/Playground/windows');
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
  return gulp.src('../Apps/Playground/node_modules/@babylonjs/react-native/README.md')
    .pipe(gulp.src('react-native-babylon.podspec'))
    .pipe(gulp.dest('Assembled'));
};

const copySharedFiles = () => {
  return gulp.src('../Apps/Playground/node_modules/@babylonjs/react-native/shared/BabylonNative.h')
    .pipe(gulp.src('../Apps/Playground/node_modules/@babylonjs/react-native/shared/XrContextHelper.h'))
    .pipe(gulp.src('../Apps/Playground/node_modules/@babylonjs/react-native/shared/XrAnchorHelper.h'))
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
    gulp.src('../Apps/Playground/node_modules/@babylonjs/react-native/submodules/BabylonNative/Dependencies/xr/Source/ARCore/Include/*')
      .pipe(gulp.dest('Assembled/android/include'))
      .on('end', resolve);
  })

  await new Promise(resolve => {
          gulp.src('../Apps/Playground/node_modules/@babylonjs/react-native/android/build/intermediates/library_and_local_jars_jni/release/jni/**/*')
    .pipe(gulp.dest('Assembled/android/src/main/jniLibs/'))
    .on('end', resolve);
  });

  // This is no longer found in the directory above because it is explicitly excluded because Playground has been updated to RN 0.64 which includes
  // the real implementation of libturbomodulejsijni.so, but we still need to support RN 0.63 consumers, so grab this one explicitly to include it in the package.
  await new Promise(resolve => {
          gulp.src('../Apps/Playground/node_modules/@babylonjs/react-native/android/build/intermediates/cmake/release/obj/**/libturbomodulejsijni.so')
    .pipe(gulp.dest('Assembled/android/src/main/jniLibs/'))
    .on('end', resolve);
  });
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
  return gulp.src('../Modules/@babylonjs/react-native-windows/windows/BabylonReactNative/*.*')
    .pipe(gulp.dest('Assembled-Windows/windows/BabylonReactNative'));
}

const copyOpenXRInfoFiles = () => {
  return gulp.src('../Modules/@babylonjs/react-native/submodules/BabylonNative/Dependencies/xr/Dependencies/OpenXR-MixedReality/LICENSE')
  .pipe(gulp.src('../Modules/@babylonjs/react-native/submodules/BabylonNative/Dependencies/xr/Dependencies/OpenXR-MixedReality/README.md'))
  .pipe(gulp.dest('Assembled-Windows/windows/OpenXR-MixedReality'));
}

const copyOpenXRPreviewHeaders = () => {
  return gulp.src('../Modules/@babylonjs/react-native/submodules/BabylonNative/Dependencies/xr/Dependencies/OpenXR-MixedReality/openxr_preview/include/openxr/*')
  .pipe(gulp.dest('Assembled-Windows/windows/OpenXR-MixedReality/include/openxr'));
}

const copyOpenXRUtilityHeaders = () => {
  return gulp.src('../Modules/@babylonjs/react-native/submodules/BabylonNative/Dependencies/xr/Dependencies/OpenXR-MixedReality/shared/XrUtility/*')
  .pipe(gulp.dest('Assembled-Windows/windows/OpenXR-MixedReality/include/XrUtility'));
}

const copyOpenXRHelperHeaders = () => {
  return gulp.src('../Modules/@babylonjs/react-native/submodules/BabylonNative/Dependencies/xr/Source/OpenXR/Include/*')
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

const validate = async () => {
  // When the package contents are updated *and validated*, update the expected below from the output of the failed validation console output (run `gulp validate`).
  // This helps ensure a bad package is not accidentally published due to tooling changes, etc.
  const expected = [
    'Assembled/android',
    'Assembled/android/build.gradle',
    'Assembled/android/include',
    'Assembled/android/include/IXrContextARCore.h',
    'Assembled/android/src',
    'Assembled/android/src/main',
    'Assembled/android/src/main/AndroidManifest.xml',
    'Assembled/android/src/main/java',
    'Assembled/android/src/main/java/com',
    'Assembled/android/src/main/java/com/babylonreactnative',
    'Assembled/android/src/main/java/com/babylonreactnative/BabylonModule.java',
    'Assembled/android/src/main/java/com/babylonreactnative/BabylonNativeInterop.java',
    'Assembled/android/src/main/java/com/babylonreactnative/BabylonPackage.java',
    'Assembled/android/src/main/java/com/babylonreactnative/EngineView.java',
    'Assembled/android/src/main/java/com/babylonreactnative/EngineViewManager.java',
    'Assembled/android/src/main/java/com/babylonreactnative/SnapshotDataReturnedEvent.java',
    'Assembled/android/src/main/jniLibs',
    'Assembled/android/src/main/jniLibs/arm64-v8a',
    'Assembled/android/src/main/jniLibs/arm64-v8a/libBabylonNative.so',
    'Assembled/android/src/main/jniLibs/arm64-v8a/libturbomodulejsijni.so',
    'Assembled/android/src/main/jniLibs/armeabi-v7a',
    'Assembled/android/src/main/jniLibs/armeabi-v7a/libBabylonNative.so',
    'Assembled/android/src/main/jniLibs/armeabi-v7a/libturbomodulejsijni.so',
    'Assembled/android/src/main/jniLibs/x86',
    'Assembled/android/src/main/jniLibs/x86/libBabylonNative.so',
    'Assembled/android/src/main/jniLibs/x86/libturbomodulejsijni.so',
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
    'Assembled/ios',
    'Assembled/ios/BabylonModule.mm',
    'Assembled/ios/BabylonNativeInterop.h',
    'Assembled/ios/BabylonNativeInterop.mm',
    'Assembled/ios/EngineViewManager.mm',
    'Assembled/ios/libs',
    'Assembled/ios/libs/libastc-codec.a',
    'Assembled/ios/libs/libastc.a',
    'Assembled/ios/libs/libBabylonNative.a',
    'Assembled/ios/libs/libbgfx.a',
    'Assembled/ios/libs/libbimg.a',
    'Assembled/ios/libs/libbx.a',
    'Assembled/ios/libs/libCanvas.a',
    'Assembled/ios/libs/libGenericCodeGen.a',
    'Assembled/ios/libs/libglslang.a',
    'Assembled/ios/libs/libGraphics.a',
    'Assembled/ios/libs/libJsRuntime.a',
    'Assembled/ios/libs/libMachineIndependent.a',
    'Assembled/ios/libs/libnapi.a',
    'Assembled/ios/libs/libNativeCapture.a',
    'Assembled/ios/libs/libNativeEngine.a',
    'Assembled/ios/libs/libNativeInput.a',
    'Assembled/ios/libs/libNativeXr.a',
    'Assembled/ios/libs/libOGLCompiler.a',
    'Assembled/ios/libs/libOSDependent.a',
    'Assembled/ios/libs/libspirv-cross-core.a',
    'Assembled/ios/libs/libspirv-cross-glsl.a',
    'Assembled/ios/libs/libspirv-cross-hlsl.a',
    'Assembled/ios/libs/libspirv-cross-msl.a',
    'Assembled/ios/libs/libSPIRV.a',
    'Assembled/ios/libs/libUrlLib.a',
    'Assembled/ios/libs/libWindow.a',
    'Assembled/ios/libs/libXMLHttpRequest.a',
    'Assembled/ios/libs/libxr.a',
    'Assembled/ios/ReactNativeBabylon.xcodeproj',
    'Assembled/ios/ReactNativeBabylon.xcodeproj/project.pbxproj',
    'Assembled/ios/ReactNativeBabylon.xcodeproj/project.xcworkspace',
    'Assembled/ios/ReactNativeBabylon.xcodeproj/project.xcworkspace/xcshareddata',
    'Assembled/ios/ReactNativeBabylon.xcodeproj/project.xcworkspace/xcshareddata/WorkspaceSettings.xcsettings',
    'Assembled/NativeCapture.d.ts',
    'Assembled/NativeCapture.js',
    'Assembled/NativeCapture.js.map',
    'Assembled/package.json',
    'Assembled/react-native-babylon.podspec',
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

  const extras = actual.filter(path => !expected.includes(path));
  const missing = expected.filter(path => !actual.includes(path));

  let isValid = true;

  if (extras.length !== 0) {
    console.error(chalk.white.bgRedBright(`The Assembled directory contains unexpected files:`));
    console.log(extras);
    isValid = false;
  }

  if (missing.length !== 0) {
    console.error(chalk.white.bgRedBright(`The Assembled directory is missing some expected files:`));
    console.log(missing);
    isValid = false;
  }

  if (!isValid) {
    console.log(chalk.black.bgCyan(`If the Assembled directory is correct, update the file validation list in gulpfile.js with the following:`))
    console.log(actual);
    throw `The Assembled directory does not contain the expected files.`;
  }
}

const createPackage = async () => {
  exec('npm pack', 'Assembled');
};

const createPackageUWP = async () => {
  exec('npm pack', 'Assembled-Windows');
}

const copyFiles = gulp.parallel(copyCommonFiles, copySharedFiles, copyIOSFiles, copyAndroidFiles);

const build = gulp.series(buildTypeScript, buildIOS, buildAndroid, createIOSUniversalLibs, copyFiles, validate);
const rebuild = gulp.series(clean, build);
const pack = gulp.series(rebuild, createPackage);

exports.validate = validate;

exports.buildTypeScript = buildTypeScript;
exports.buildIOS = buildIOS;
exports.buildAndroid = buildAndroid;
exports.createIOSUniversalLibs = createIOSUniversalLibs;
exports.copyFiles = copyFiles;

exports.clean = clean;
exports.build = build;
exports.rebuild = rebuild;
exports.pack = pack;

const packAndroid = gulp.series(clean, buildAndroid, copyFiles, createPackage);
exports.buildAndroid = buildAndroid;
exports.packAndroid = packAndroid;

const copyPackageFilesUWP = gulp.series(copyCommonFiles, copySharedFiles, copyUWPFiles);
const buildUWPPublish = gulp.series(buildUWP, copyPackageFilesUWP);
const packUWP = gulp.series(clean, buildUWP, copyPackageFilesUWP, createPackage, createPackageUWP);
const packUWPNoBuild = gulp.series(clean, copyPackageFilesUWP, createPackage, createPackageUWP);

exports.initializeSubmodulesWindowsAgent = initializeSubmodulesWindowsAgent;
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
