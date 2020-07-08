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
  exec('cmake -G Xcode -DCMAKE_TOOLCHAIN_FILE=../../../Apps/Playground/node_modules/@babylonjs/react-native/submodules/BabylonNative/Dependencies/ios-cmake/ios.toolchain.cmake -DPLATFORM=OS64COMBINED -DENABLE_ARC=0 -DDEPLOYMENT_TARGET=12 -DENABLE_GLSLANG_BINARIES=OFF -DSPIRV_CROSS_CLI=OFF ..', 'iOS/Build');
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
          gulp.src('../Apps/Playground/node_modules/@babylonjs/react-native/android/build/intermediates/library_and_local_jars_jni/release/jni/**/*')
    .pipe(gulp.dest('Assembled/android/src/main/jniLibs/'))
    .on('end', resolve);
  });
};

const createPackage = async () => {
  exec('npm pack', 'Assembled');
};

const copyFiles = gulp.parallel(copyCommonFiles, copyIOSFiles, copyAndroidFiles);

const build = gulp.series(buildIOS, buildAndroid, createIOSUniversalLibs, copyFiles);
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