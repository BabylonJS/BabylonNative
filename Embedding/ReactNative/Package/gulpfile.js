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

let assemblediOSAndroidDir = 'Assembled-iOSAndroid';
let assembledWindowsDir = 'Assembled-Windows';
let basekitBuild = false;
let cmakeBasekitBuildDefinition = '';
let basekitPackagePath = '';

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

  if (shelljs.test('-d', `${assemblediOSAndroidDir}`)) {
    shelljs.rm('-r', `${assemblediOSAndroidDir}`);
  }

  if (shelljs.test('-d', `${assembledWindowsDir}`)) {
    shelljs.rm('-r', `${assembledWindowsDir}`);
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
  exec(`cmake -B Build -G Xcode ${cmakeBasekitBuildDefinition}`, 'iOS');
};

const buildIphoneOS = async () => {
  exec('xcodebuild -sdk iphoneos -configuration Release -project ReactNativeBabylon.xcodeproj -scheme BabylonNative build CODE_SIGNING_ALLOWED=NO', 'iOS/Build');
};

const buildIphoneSimulator = async () => {
  exec('xcodebuild -sdk iphonesimulator -arch x86_64 -configuration Release -project ReactNativeBabylon.xcodeproj -scheme BabylonNative build CODE_SIGNING_ALLOWED=NO', 'iOS/Build');
};

const buildIOS = gulp.series(makeXCodeProj, buildIphoneOS, buildIphoneSimulator);

const buildAndroid = async () => {
  const basekitBuildProp = basekitBuild ? "-PBASEKIT_BUILD=1" : "";
  exec(`./gradlew babylonjs_react-native:assembleRelease --stacktrace --info ${basekitBuildProp}`, '../Apps/Playground/Playground/android');
};

const makeUWPProjectPlatform = async (name, arch) => {
  shelljs.mkdir('-p', `./../Modules/@babylonjs/react-native/Build/uwp_${name}`);
  exec(`cmake -G "Visual Studio 16 2019" -D CMAKE_SYSTEM_NAME=WindowsStore -D CMAKE_SYSTEM_VERSION=10.0 -DCMAKE_UNITY_BUILD=true ${cmakeBasekitBuildDefinition} -A ${arch} ./../../../react-native-windows/windows`, `./../Modules/@babylonjs/react-native/Build/uwp_${name}`);
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
    .pipe(gulp.src(`${basekitPackagePath}react-native-babylon.podspec`))
    .pipe(gulp.dest(`${assemblediOSAndroidDir}/`));
};

const copyIOSFiles = async () => {
  await new Promise(resolve => {
    gulp.src('../Apps/Playground/Playground/node_modules/@babylonjs/react-native-iosandroid/ios/*.h')
      .pipe(gulp.src('../Apps/Playground/Playground/node_modules/@babylonjs/react-native-iosandroid/ios/*.mm'))
      // This xcodeproj is garbage that we don't need in the package, but `pod install` ignores the package if it doesn't contain at least one xcodeproj. 🤷🏼‍♂️
      .pipe(gulp.src('iOS/Build/ReactNativeBabylon.xcodeproj**/**/*'))
      .pipe(gulp.dest(`${assemblediOSAndroidDir}/ios`))
      .on('end', resolve);
  });

  await new Promise(resolve => {
    gulp.src('../Package/iOS/Build/_deps/babylonnative-src/Dependencies/xr/Source/ARKit/Include/*')
      .pipe(gulp.dest(`${assemblediOSAndroidDir}/ios/include`))
      .on('end', resolve);
  });
};

const createIOSUniversalLibs = async () => {
  shelljs.mkdir('-p', `${assemblediOSAndroidDir}/ios/libs`);
  const libs = await readdirAsync('iOS/Build/Release-iphoneos');
  libs.map(lib => exec(`lipo -create iOS/Build/Release-iphoneos/${lib} iOS/Build/Release-iphonesimulator/${lib} -output ${assemblediOSAndroidDir}/ios/libs/${lib}`));
};

const copyAndroidFiles = async () => {
  await new Promise(resolve => {
    gulp.src(`${basekitPackagePath}Android/build.gradle`)
      .pipe(gulp.src('../Apps/Playground/Playground/node_modules/@babylonjs/react-native-iosandroid/android/src**/main/AndroidManifest.xml'))
      .pipe(gulp.src('../Apps/Playground/Playground/node_modules/@babylonjs/react-native-iosandroid/android/src**/main/java/**/*'))
      .pipe(gulp.dest(`${assemblediOSAndroidDir}/android`))
      .on('end', resolve);
  });

  await new Promise(resolve => {
    gulp.src('../Package/iOS/Build/_deps/babylonnative-src/Dependencies/xr/Source/ARCore/Include/*')
      .pipe(gulp.dest(`${assemblediOSAndroidDir}/android/include`))
      .on('end', resolve);
  });

  await new Promise(resolve => {
    const jnidir = '../Apps/Playground/Playground/node_modules/@babylonjs/react-native-iosandroid/android/build/intermediates/library_and_local_jars_jni/release/jni/**';
    gulp.src(`${jnidir}/libBabylonNative.so`)
      .pipe(gulp.dest(`${assemblediOSAndroidDir}/android/src/main/jniLibs/`))
      .on('end', resolve);
  });

  // This is no longer found in the directory above because it is explicitly excluded because Playground has been updated to RN 0.64 which includes
  // the real implementation of libturbomodulejsijni.so, but we still need to support RN 0.63 consumers, so grab this one explicitly to include it in the package.

  const versionIndex = process.argv.indexOf('--reactNative');
  if (versionIndex != -1 && process.argv[versionIndex + 1] != '0.71') {
    await new Promise(resolve => {
      gulp.src('../Apps/Playground/Playground/node_modules/@babylonjs/react-native-iosandroid/android/build/intermediates/cmake/release/obj/{arm64-v8a,armeabi-v7a,x86}/libturbomodulejsijni.so')
        .pipe(gulp.dest(`${assemblediOSAndroidDir}/android/src/main/jniLibs/`))
        .on('end', resolve);
    });
  }
};

const createUWPDirectories = async () => {
  shelljs.mkdir('-p', `${assembledWindowsDir}`);
  shelljs.mkdir('-p', `${assembledWindowsDir}/windows`);
  shelljs.mkdir('-p', `${assembledWindowsDir}/windows/libs`);
  shelljs.mkdir('-p', `${assembledWindowsDir}/windows/libs/arm`);
  shelljs.mkdir('-p', `${assembledWindowsDir}/windows/libs/arm/Debug`);
  shelljs.mkdir('-p', `${assembledWindowsDir}/windows/libs/arm/Release`);
  shelljs.mkdir('-p', `${assembledWindowsDir}/windows/libs/arm64`);
  shelljs.mkdir('-p', `${assembledWindowsDir}/windows/libs/arm64/Debug`);
  shelljs.mkdir('-p', `${assembledWindowsDir}/windows/libs/arm64/Release`);
  shelljs.mkdir('-p', `${assembledWindowsDir}/windows/libs/x86`);
  shelljs.mkdir('-p', `${assembledWindowsDir}/windows/libs/x86/Debug`);
  shelljs.mkdir('-p', `${assembledWindowsDir}/windows/libs/x86/Release`);
  shelljs.mkdir('-p', `${assembledWindowsDir}/windows/libs/x64`);
  shelljs.mkdir('-p', `${assembledWindowsDir}/windows/libs/x64/Debug`);
  shelljs.mkdir('-p', `${assembledWindowsDir}/windows/libs/x64/Release`);
  shelljs.mkdir('-p', `${assembledWindowsDir}/windows/BabylonReactNative`);
  shelljs.mkdir('-p', `${assembledWindowsDir}/windows/include`);
  shelljs.mkdir('-p', `${assembledWindowsDir}/windows/OpenXR-MixedReality/include/openxr`);
  shelljs.mkdir('-p', `${assembledWindowsDir}/windows/OpenXR-MixedReality/include/XrUtility`);
}

const copyCommonFilesUWP = () => {
  return gulp.src('../Modules/@babylonjs/react-native-windows/package.json')
    .pipe(gulp.src('../Modules/@babylonjs/react-native-windows/README.md'))
    .pipe(gulp.src('../Modules/@babylonjs/react-native-windows/*.ts*'))
    .pipe(gulp.dest(`${assembledWindowsDir}`));
}

const copyx86DebugUWPFiles = () => {
  return gulp.src('../Modules/@babylonjs/react-native/Build/uwp_x86/**/Debug/**/*.{lib,pri}')
    .pipe(rename({ dirname: '' }))
    .pipe(gulp.dest(`${assembledWindowsDir}/windows/libs/x86/Debug`));
}

const copyx86ReleaseUWPFiles = () => {
  return gulp.src('../Modules/@babylonjs/react-native/Build/uwp_x86/**/Release/**/*.{lib,pri}')
    .pipe(rename({ dirname: '' }))
    .pipe(gulp.dest(`${assembledWindowsDir}/windows/libs/x86/Release`));
}

const copyx64DebugUWPFiles = () => {
  return gulp.src('../Modules/@babylonjs/react-native/Build/uwp_x64/**/Debug/**/*.{lib,pri}')
    .pipe(rename({ dirname: '' }))
    .pipe(gulp.dest(`${assembledWindowsDir}/windows/libs/x64/Debug`));
}

const copyx64ReleaseUWPFiles = () => {
  return gulp.src('../Modules/@babylonjs/react-native/Build/uwp_x64/**/Release/**/*.{lib,pri}')
    .pipe(rename({ dirname: '' }))
    .pipe(gulp.dest(`${assembledWindowsDir}/windows/libs/x64/Release`));
}

const copyARMDebugUWPFiles = () => {
  return gulp.src('../Modules/@babylonjs/react-native/Build/uwp_arm/**/Debug/**/*.{lib,pri}')
    .pipe(rename({ dirname: '' }))
    .pipe(gulp.dest(`${assembledWindowsDir}/windows/libs/arm/Debug`));
}

const copyARMReleaseUWPFiles = () => {
  return gulp.src('../Modules/@babylonjs/react-native/Build/uwp_arm/**/Release/**/*.{lib,pri}')
    .pipe(rename({ dirname: '' }))
    .pipe(gulp.dest(`${assembledWindowsDir}/windows/libs/arm/Release`));
}

const copyARM64DebugUWPFiles = () => {
  return gulp.src('../Modules/@babylonjs/react-native/Build/uwp_arm64/**/Debug/**/*.{lib,pri}')
    .pipe(rename({ dirname: '' }))
    .pipe(gulp.dest(`${assembledWindowsDir}/windows/libs/arm64/Debug`));
}

const copyARM64ReleaseUWPFiles = () => {
  return gulp.src('../Modules/@babylonjs/react-native/Build/uwp_arm64/**/Release/**/*.{lib,pri}')
    .pipe(rename({ dirname: '' }))
    .pipe(gulp.dest(`${assembledWindowsDir}/windows/libs/arm64/Release`));
}

const copyVCXProjUWPFiles = () => {
  const uwpFilesDir = '../Modules/@babylonjs/react-native-windows/windows/BabylonReactNative';
  return gulp.src([`${uwpFilesDir}/*.*`, `!${uwpFilesDir}/*.pfx`])
    .pipe(gulp.dest(`${assembledWindowsDir}/windows/BabylonReactNative`));
}

const copyOpenXRInfoFiles = () => {
  return gulp.src('../Modules/@babylonjs/react-native/Build/uwp_x64/_deps/openxr-mixedreality-src/LICENSE')
    .pipe(gulp.src('../Modules/@babylonjs/react-native/Build/uwp_x64/_deps/openxr-mixedreality-src/README.md'))
    .pipe(gulp.dest(`${assembledWindowsDir}/windows/OpenXR-MixedReality`));
}

const copyOpenXRPreviewHeaders = () => {
  return gulp.src('../Modules/@babylonjs/react-native/Build/uwp_x64/_deps/openxr-mixedreality-src/openxr_preview/include/openxr/*')
    .pipe(gulp.dest(`${assembledWindowsDir}/windows/OpenXR-MixedReality/include/openxr`));
}

const copyOpenXRUtilityHeaders = () => {
  return gulp.src('../Modules/@babylonjs/react-native/Build/uwp_x64/_deps/openxr-mixedreality-src/shared/XrUtility/*')
    .pipe(gulp.dest(`${assembledWindowsDir}/windows/OpenXR-MixedReality/include/XrUtility`));
}

const copyOpenXRHelperHeaders = () => {
  return gulp.src('../Modules/@babylonjs/react-native/Build/uwp_x64/_deps/babylonnative-src/Dependencies/xr/Source/OpenXR/Include/*')
    .pipe(gulp.src('../Modules/@babylonjs/react-native-windows/windows/include/*'))
    .pipe(gulp.dest(`${assembledWindowsDir}/windows/include`));
}

const copyUWPFiles = gulp.series(
  createUWPDirectories,
  basekitBuild ? 
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
    copyVCXProjUWPFiles)
    :
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
    `${assemblediOSAndroidDir}/android`,
    `${assemblediOSAndroidDir}/android/build.gradle`,
    `${assemblediOSAndroidDir}/android/include`,
    `${assemblediOSAndroidDir}/android/include/IXrContextARCore.h`,
    `${assemblediOSAndroidDir}/android/src`,
    `${assemblediOSAndroidDir}/android/src/main`,
    `${assemblediOSAndroidDir}/android/src/main/AndroidManifest.xml`,
    `${assemblediOSAndroidDir}/android/src/main/java`,
    `${assemblediOSAndroidDir}/android/src/main/java/com`,
    `${assemblediOSAndroidDir}/android/src/main/java/com/babylonreactnative`,
    `${assemblediOSAndroidDir}/android/src/main/java/com/babylonreactnative/BabylonModule.java`,
    `${assemblediOSAndroidDir}/android/src/main/java/com/babylonreactnative/BabylonNativeInterop.java`,
    `${assemblediOSAndroidDir}/android/src/main/java/com/babylonreactnative/BabylonPackage.java`,
    `${assemblediOSAndroidDir}/android/src/main/java/com/babylonreactnative/EngineView.java`,
    `${assemblediOSAndroidDir}/android/src/main/java/com/babylonreactnative/EngineViewManager.java`,
    `${assemblediOSAndroidDir}/android/src/main/java/com/babylonreactnative/SnapshotDataReturnedEvent.java`,
    `${assemblediOSAndroidDir}/android/src/main/jniLibs`,
    `${assemblediOSAndroidDir}/android/src/main/jniLibs/arm64-v8a`,
    `${assemblediOSAndroidDir}/android/src/main/jniLibs/arm64-v8a/libBabylonNative.so`,
    `${assemblediOSAndroidDir}/android/src/main/jniLibs/armeabi-v7a`,
    `${assemblediOSAndroidDir}/android/src/main/jniLibs/armeabi-v7a/libBabylonNative.so`,
    `${assemblediOSAndroidDir}/android/src/main/jniLibs/x86`,
    `${assemblediOSAndroidDir}/android/src/main/jniLibs/x86/libBabylonNative.so`,
    `${assemblediOSAndroidDir}/android/src/main/jniLibs/x86_64`,
    `${assemblediOSAndroidDir}/android/src/main/jniLibs/x86_64/libBabylonNative.so`,
    `${assemblediOSAndroidDir}/ios`,
    `${assemblediOSAndroidDir}/ios/BabylonModule.mm`,
    `${assemblediOSAndroidDir}/ios/BabylonNativeInterop.h`,
    `${assemblediOSAndroidDir}/ios/BabylonNativeInterop.mm`,
    `${assemblediOSAndroidDir}/ios/EngineViewManager.mm`,
    `${assemblediOSAndroidDir}/ios/include`,
    `${assemblediOSAndroidDir}/ios/libs`,
    `${assemblediOSAndroidDir}/ios/libs/libastc-encoder.a`,
    `${assemblediOSAndroidDir}/ios/libs/libBabylonNative.a`,
    `${assemblediOSAndroidDir}/ios/libs/libbgfx.a`,
    `${assemblediOSAndroidDir}/ios/libs/libbimg.a`,
    `${assemblediOSAndroidDir}/ios/libs/libbx.a`,
    `${assemblediOSAndroidDir}/ios/libs/libCanvas.a`,
    `${assemblediOSAndroidDir}/ios/libs/libGenericCodeGen.a`,
    `${assemblediOSAndroidDir}/ios/libs/libglslang.a`,
    `${assemblediOSAndroidDir}/ios/libs/libglslang-default-resource-limits.a`,
    `${assemblediOSAndroidDir}/ios/libs/libGraphics.a`,
    `${assemblediOSAndroidDir}/ios/libs/libJsRuntime.a`,
    `${assemblediOSAndroidDir}/ios/libs/libMachineIndependent.a`,
    `${assemblediOSAndroidDir}/ios/libs/libnapi.a`,
    `${assemblediOSAndroidDir}/ios/libs/libNativeCapture.a`,
    `${assemblediOSAndroidDir}/ios/libs/libNativeEngine.a`,
    `${assemblediOSAndroidDir}/ios/libs/libNativeInput.a`,
    `${assemblediOSAndroidDir}/ios/libs/libNativeOptimizations.a`,
    `${assemblediOSAndroidDir}/ios/libs/libNativeTracing.a`,
    `${assemblediOSAndroidDir}/ios/libs/libNativeXr.a`,
    `${assemblediOSAndroidDir}/ios/libs/libOGLCompiler.a`,
    `${assemblediOSAndroidDir}/ios/libs/libOSDependent.a`,
    `${assemblediOSAndroidDir}/ios/libs/libspirv-cross-core.a`,
    `${assemblediOSAndroidDir}/ios/libs/libspirv-cross-glsl.a`,
    `${assemblediOSAndroidDir}/ios/libs/libspirv-cross-msl.a`,
    `${assemblediOSAndroidDir}/ios/libs/libSPIRV.a`,
    `${assemblediOSAndroidDir}/ios/libs/libtinyexr.a`,
    `${assemblediOSAndroidDir}/ios/libs/libetc1.a`,
    `${assemblediOSAndroidDir}/ios/libs/libetc2.a`,
    `${assemblediOSAndroidDir}/ios/libs/libnvtt.a`,
    `${assemblediOSAndroidDir}/ios/libs/libsquish.a`,
    `${assemblediOSAndroidDir}/ios/libs/libpvrtc.a`,
    `${assemblediOSAndroidDir}/ios/libs/libiqa.a`,
    `${assemblediOSAndroidDir}/ios/libs/libedtaa3.a`,
    `${assemblediOSAndroidDir}/ios/libs/libUrlLib.a`,
    `${assemblediOSAndroidDir}/ios/libs/libWindow.a`,
    `${assemblediOSAndroidDir}/ios/libs/libXMLHttpRequest.a`,
    `${assemblediOSAndroidDir}/ios/libs/libNativeCamera.a`,
    `${assemblediOSAndroidDir}/ios/libs/libxr.a`,
    `${assemblediOSAndroidDir}/ios/include/IXrContextARKit.h`,
    `${assemblediOSAndroidDir}/ios/ReactNativeBabylon.xcodeproj`,
    `${assemblediOSAndroidDir}/ios/ReactNativeBabylon.xcodeproj/project.pbxproj`,
    `${assemblediOSAndroidDir}/ios/ReactNativeBabylon.xcodeproj/project.xcworkspace`,
    `${assemblediOSAndroidDir}/ios/ReactNativeBabylon.xcodeproj/project.xcworkspace/xcshareddata`,
    `${assemblediOSAndroidDir}/ios/ReactNativeBabylon.xcodeproj/project.xcworkspace/xcshareddata/WorkspaceSettings.xcsettings`,
    `${assemblediOSAndroidDir}/package.json`,
    `${assemblediOSAndroidDir}/react-native-babylon.podspec`,
    `${assemblediOSAndroidDir}/README.md`,
  ];

  const versionIndex = process.argv.indexOf('--reactNative');	
  if (versionIndex != -1) {	
    if (process.argv[versionIndex + 1] !== '0.71') {	
      const expectediosandroidNot071 = [
        `${assemblediOSAndroidDir}/android/src/main/jniLibs/arm64-v8a/libturbomodulejsijni.so`,
        `${assemblediOSAndroidDir}/android/src/main/jniLibs/armeabi-v7a/libturbomodulejsijni.so`,
        `${assemblediOSAndroidDir}/android/src/main/jniLibs/x86/libturbomodulejsijni.so`,
      ];
      expectediosandroid = expectediosandroid.concat(expectediosandroidNot071);
    }
  }
  const actualiosandroid = glob.sync(`${assemblediOSAndroidDir}/**/*`);
  checkDirectory(actualiosandroid, expectediosandroid, `${assemblediOSAndroidDir}`);
}

const createPackage = async () => {
  exec('npm pack', 'Assembled');
};

const createPackageiOSAndroid = async () => {
  exec('npm pack', `${assemblediOSAndroidDir}`);
};

const createPackageUWP = async () => {
  exec('npm pack', `${assembledWindowsDir}`);
}

const switchToBaseKit = async () => {
  assemblediOSAndroidDir = 'Assembled-BaseKit-iOSAndroid';
  assembledWindowsDir = 'Assembled-BaseKit-Windows';
  cmakeBasekitBuildDefinition = '-DBASEKIT_BUILD=1';
  basekitBuild = true;
  basekitPackagePath = 'BaseKit/';
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

        if (basekitBuild)
        {
          packageJsoniOSAndroid["name"] = "@babylonjs/react-native-basekit-iosandroid" + packageNamePostfix;
          packageJsonWindows["name"] = "@babylonjs/react-native-basekit-windows" + packageNamePostfix;
          delete packageJsoniOSAndroid['peerDependencies']['react-native-permissions'];
          delete packageJsonWindows['peerDependencies']['react-native-permissions'];
        } else {
          packageJsoniOSAndroid["name"] = "@babylonjs/react-native-iosandroid" + packageNamePostfix;
          packageJsonWindows["name"] = "@babylonjs/react-native-windows" + packageNamePostfix;
        }
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
const buildIOSAndroid = gulp.series(patchPackageVersion, buildIOS, buildAndroid, createIOSUniversalLibs, copyFiles, validateAssemblediOSAndroid);
const build = gulp.series(buildIOSAndroid, switchToBaseKit, buildIOSAndroid);
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
const buildUWPPublish = gulp.series(patchPackageVersion, buildUWP, copyPackageFilesUWP, switchToBaseKit, patchPackageVersion, buildUWP, copyPackageFilesUWP);
const packUWP = gulp.series(clean, buildUWP, copyPackageFilesUWP, createPackage, createPackageUWP);
const packUWPNoBuild = gulp.series(clean, copyPackageFilesUWP, createPackage, createPackageUWP);

exports.buildTS = buildTS;
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

exports.default = build;
