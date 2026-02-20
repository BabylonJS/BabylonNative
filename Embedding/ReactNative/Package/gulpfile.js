const fs = require('fs');
const path = require('path');
const log = require('fancy-log');
const gulp = require('gulp');
const shelljs = require('shelljs');
const glob = require('glob');
const chalk = require('chalk');

const unzipper = require('unzipper');

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
};

const buildTypeScript = async () => {
  exec('node node_modules/typescript/bin/tsc --noEmit false --outDir ../../../Package/Assembled', '../Modules/@babylonjs/react-native');

  // Update the 'main' property in package.json to be 'index.js' instead of 'index.ts'
  const packageJson = JSON.parse(fs.readFileSync('Assembled/package.json'));

  const parsedMain = path.parse(packageJson.main);
  if (parsedMain.ext === '.ts') {
    packageJson.main = path.join(parsedMain.dir, `${parsedMain.name}.js`);
  }

  fs.writeFileSync('Assembled/package.json', JSON.stringify(packageJson, null, 4));
};

const buildIphoneOS = async () => {
  exec('xcodebuild -sdk iphoneos -configuration Release -workspace BRNPlayground.xcworkspace -scheme Playground build CODE_SIGNING_ALLOWED=NO', '../Apps/Playground/ios');
};

const buildIphoneSimulator = async () => {
  exec('xcodebuild -sdk iphonesimulator -arch x86_64 -configuration Release -workspace BRNPlayground.xcworkspace -scheme Playground build CODE_SIGNING_ALLOWED=NO', '../Apps/Playground/ios');
};

const buildAndroid = async () => {
  exec(`./gradlew babylonjs_react-native:assembleRelease --stacktrace --info`, '../Apps/Playground/android');
};

const copyCommonFiles = () => {
  return gulp.src('../Modules/@babylonjs/react-native/README.md')
    .pipe(gulp.src('../NOTICE.html'))
    .pipe(gulp.src(`../Modules/@babylonjs/react-native/react-native-babylon.podspec`))
    .pipe(gulp.src(`../Modules/@babylonjs/react-native/postinstall.js`))
    .pipe(gulp.dest('Assembled'));
};

const copyAndroidARCoreFiles = () => {
    return gulp.src('../Modules/@babylonjs/react-native/shared/BabylonNative/Repo/Dependencies/xr/Source/ARCore/Include/*')
      .pipe(gulp.dest(`Assembled/android/include`));
};

const copyiOSARKitFiles = () => {
    return gulp.src('../Modules/@babylonjs/react-native/shared/BabylonNative/Repo/Dependencies/xr/Source/ARKit/Include/*')
      .pipe(gulp.dest(`Assembled/ios/include`));
};

const copySharedFiles = () => {
  return gulp.src('../Modules/@babylonjs/react-native/shared/**')
    .pipe(gulp.dest('Assembled/shared'));
};

const copyIOSFiles = () => {
  return gulp.src('../Modules/@babylonjs/react-native/ios/**')
    .pipe(gulp.dest(`Assembled/ios`));
};

const copyAndroidFiles = () => {
  return gulp.src('../Modules/@babylonjs/react-native/android/**')
    .pipe(gulp.dest(`Assembled/android`));
};

const copyWindowsFiles = () => {
  return gulp.src('../Modules/@babylonjs/react-native/windows/**')
    .pipe(gulp.dest(`Assembled/windows`));
};

const copyJSFilesToModule = () => {
  return gulp.src('Assembled/**')
    .pipe(gulp.dest(`../Modules/@babylonjs/react-native`));
}

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
    'Assembled/NOTICE.html',
    'Assembled/shared',
    'Assembled/shared/BabylonNative.h',
    'Assembled/shared/BabylonNative.cpp',
    'Assembled/shared/BabylonNative',
    'Assembled/shared/XrAnchorHelper.h',
    'Assembled/shared/XrContextHelper.h',
    'Assembled/VersionValidation.d.ts',
    'Assembled/VersionValidation.js',
    'Assembled/VersionValidation.js.map',
    `Assembled/android`,
    `Assembled/android/build.gradle`,
    `Assembled/android/CMakeLists.txt`,
    `Assembled/android/ReactBabylonNative.cpp`,
    `Assembled/android/README.md`,
    `Assembled/android/include`, 
    `Assembled/android/include/IXrContextARCore.h`,
    `Assembled/android/src`,
    `Assembled/android/src/main`,
    `Assembled/android/src/main/AndroidManifest.xml`,
    `Assembled/android/src/main/java`,
    `Assembled/android/src/main/java/com`,
    `Assembled/android/src/main/java/com/babylonreactnative`,
    `Assembled/android/src/main/java/com/babylonreactnative/BabylonModule.java`,
    `Assembled/android/src/main/java/com/babylonreactnative/BabylonNativeInterop.java`,
    `Assembled/android/src/main/java/com/babylonreactnative/BabylonPackage.java`,
    `Assembled/android/src/main/java/com/babylonreactnative/EngineView.java`,
    `Assembled/android/src/main/java/com/babylonreactnative/EngineViewManager.java`,
    `Assembled/android/src/main/java/com/babylonreactnative/SnapshotDataReturnedEvent.java`,
    `Assembled/ios`,
    `Assembled/ios/BabylonModule.mm`,
    `Assembled/ios/BabylonNativeInterop.h`,
    `Assembled/ios/BabylonNativeInterop.mm`,
    `Assembled/ios/EngineViewManager.mm`,
    `Assembled/ios/include`,
    `Assembled/ios/include/IXrContextARKit.h`,
    `Assembled/package.json`,
    `Assembled/react-native-babylon.podspec`,
    `Assembled/postinstall.js`,

    'Assembled/android/src/main/cpp',
    'Assembled/android/src/main/cpp/BabylonNativeInterop.cpp',
    'Assembled/ios/CMakeLists.txt',
    'Assembled/shared/BabylonNative/CMakeLists.txt',
    'Assembled/shared/CMakeLists.txt',
    'Assembled/shared/DispatchFunction.h',
    'Assembled/windows',
    'Assembled/windows/BabylonReactNative',
    'Assembled/windows/BabylonReactNative/BabylonModule.cpp',
    'Assembled/windows/BabylonReactNative/BabylonModule.h',
    'Assembled/windows/BabylonReactNative/BabylonReactNative_TemporaryKey.pfx',
    'Assembled/windows/BabylonReactNative/BabylonReactNative.def',
    'Assembled/windows/BabylonReactNative/BabylonReactNative.rc',
    'Assembled/windows/BabylonReactNative/BabylonReactNative.vcxproj',
    'Assembled/windows/BabylonReactNative/BabylonReactNative.vcxproj.filters',
    'Assembled/windows/BabylonReactNative/EngineView.cpp',
    'Assembled/windows/BabylonReactNative/EngineView.h',
    'Assembled/windows/BabylonReactNative/EngineView.idl',
    'Assembled/windows/BabylonReactNative/EngineViewManager.cpp',
    'Assembled/windows/BabylonReactNative/EngineViewManager.h',
    'Assembled/windows/BabylonReactNative/packages.config',
    'Assembled/windows/BabylonReactNative/pch.cpp',
    'Assembled/windows/BabylonReactNative/pch.h',
    'Assembled/windows/BabylonReactNative/PropertySheet.props',
    'Assembled/windows/BabylonReactNative/ReactPackageProvider.cpp',
    'Assembled/windows/BabylonReactNative/ReactPackageProvider.h',
    'Assembled/windows/BabylonReactNative/ReactPackageProvider.idl',
    'Assembled/windows/BabylonReactNative/resource.h',
    'Assembled/windows/CMakeLists.txt'
  ];

  const actual = glob.sync('Assembled/**/*', {ignore: ['Assembled/shared/BabylonNative/Repo/**', 'Assembled/shared/BabylonNative/deps/**']});
  checkDirectory(actual, expected, `Assembled`);
}

const createPackage = async () => {
  exec('npm pack', 'Assembled');
};

const COMMIT_ID = '9a50d722d493524e1b315dd48fed6b0ed6d062c8';
const ZIP_URL = `https://github.com/BabylonJS/BabylonNative/archive/${COMMIT_ID}.zip`;
const TARGET_DIR = path.resolve(__dirname, '../Modules/@babylonjs/react-native/shared/BabylonNative');
const ZIP_PATH = path.join(TARGET_DIR, `${COMMIT_ID}.zip`);
const UNZIP_FOLDER = path.join(TARGET_DIR, `Repo`);
const CMAKE_LISTS_PATH = path.join(TARGET_DIR, 'CMakeLists.txt');
const TEMP_BUILD_DIR = path.join(TARGET_DIR, 'tempBuild');
const DEPS_OUTPUT_DIR = path.join(TARGET_DIR, 'deps');

async function downloadZip(url, dest) {
  const res = await fetch(url);
  if (!res.ok) {
    throw new Error(`Failed to download zip: ${res.status} ${res.statusText}`);
  }

  const fileStream = fs.createWriteStream(dest);
  const reader = res.body.getReader();

  return new Promise((resolve, reject) => {
    function pump() {
      reader.read().then(({ done, value }) => {
        if (done) {
          fileStream.end();
          resolve();
          return;
        }
        fileStream.write(Buffer.from(value), pump);
      }).catch(reject);
    }

    fileStream.on('error', reject);
    pump();
  });
}

async function unzipFile(zipPath, destDir) {
  const dir = await unzipper.Open.file(zipPath);
  await dir.extract({ path: destDir });
}

function deleteFile(filePath) {
  return fs.promises.unlink(filePath);
}

function runCMake(buildDir) {
  let cmakeCommand = `cmake -S . -B ../tempBuild -DBABYLON_NATIVE_BUILD_SOURCETREE=ON -DBABYLON_NATIVE_BUILD_APPS=OFF -DBABYLON_NATIVE_PLUGIN_TESTUTILS=OFF -DBABYLON_NATIVE_INSTALL=OFF`;

  exec(cmakeCommand, buildDir);
}

function deleteFolderRecursive(folderPath, excludeSubFolders = []) {
  if (!fs.existsSync(folderPath)) return;

  const entries = fs.readdirSync(folderPath, { withFileTypes: true });

  for (const entry of entries) {
    const fullPath = path.join(folderPath, entry.name);

    if (entry.isDirectory()) {
      if (excludeSubFolders.includes(entry.name)) {
        continue; // Skip this folder
      }
      deleteFolderRecursive(fullPath, excludeSubFolders);
    } else {
      fs.unlinkSync(fullPath);
    }
  }

  // After deleting contents, delete the root folder if it's not in the excluded list
  const folderName = path.basename(folderPath);
  if (!excludeSubFolders.includes(folderName)) {
    const remaining = fs.readdirSync(folderPath);
    if (remaining.length === 0) {
      fs.rmdirSync(folderPath);
      console.log(`Deleted folder: ${folderPath}`);
    }
  }
}

function copyRecursiveExcludingGit(src, dest) {
  if (!fs.existsSync(src)) return;

  fs.mkdirSync(dest, { recursive: true });
  const entries = fs.readdirSync(src, { withFileTypes: true });

  for (const entry of entries) {
    if (entry.name === '.git') continue;

    const srcPath = path.join(src, entry.name);
    const destPath = path.join(dest, entry.name);

    if (entry.isDirectory()) {
      copyRecursiveExcludingGit(srcPath, destPath);
    } else {
      fs.copyFileSync(srcPath, destPath);
    }
  }
}

function copyDepsFolders() {
  const depsSrc = path.join(TEMP_BUILD_DIR, '_deps');
  if (!fs.existsSync(depsSrc)) return;

  fs.mkdirSync(DEPS_OUTPUT_DIR, { recursive: true });

  const entries = fs.readdirSync(depsSrc, { withFileTypes: true });

  for (const entry of entries) {
    if (entry.isDirectory() && entry.name.endsWith('-src')) {
      const fullSrcPath = path.join(depsSrc, entry.name);
      const fullDestPath = path.join(DEPS_OUTPUT_DIR, entry.name);
      console.log(`Copying ${entry.name} to deps/`);
      copyRecursiveExcludingGit(fullSrcPath, fullDestPath);
    }
  }
}
const buildBabylonNativeSourceTree = async () => {
  fs.mkdirSync(TARGET_DIR, { recursive: true });

  console.log(`Downloading BabylonNative commit ${COMMIT_ID}...`);
  await downloadZip(ZIP_URL, ZIP_PATH);

  console.log('Unzipping...');
  await unzipFile(ZIP_PATH, TARGET_DIR);

  console.log('Deleting ZIP...');
  await deleteFile(ZIP_PATH);

  console.log('Renaming BabylonNative-xxx folder ...');
  await fs.rename(`${TARGET_DIR}/BabylonNative-${COMMIT_ID}`, `${TARGET_DIR}/Repo`, (err) => {
    if (err) throw err;
    console.log('Rename complete!');
  });

  console.log('Running CMake...');
  await runCMake(UNZIP_FOLDER);

  console.log('Copying *-src folders to deps/ (excluding .git)...');
  copyDepsFolders();

  console.log('Deleting tempBuild folder...');
  deleteFolderRecursive(TEMP_BUILD_DIR);

  console.log('Remove unnecessary sources');
  deleteFolderRecursive(`${UNZIP_FOLDER}/.github`);
  deleteFolderRecursive(`${UNZIP_FOLDER}/Apps`);
  deleteFolderRecursive(`${UNZIP_FOLDER}/Documentation`);
  deleteFolderRecursive(`${UNZIP_FOLDER}/Install`);

  // dependencies cleanup
  deleteFolderRecursive(`${DEPS_OUTPUT_DIR}/bgfx.cmake-src/bgfx/3rdparty`, ['renderdoc', 'stb', 'sdf']);
  deleteFolderRecursive(`${DEPS_OUTPUT_DIR}/bgfx.cmake-src/bgfx/examples`,['common']);
  deleteFolderRecursive(`${DEPS_OUTPUT_DIR}/bgfx.cmake-src/bgfx/bindings`);
  deleteFolderRecursive(`${DEPS_OUTPUT_DIR}/bgfx.cmake-src/bgfx/docs`);
  deleteFolderRecursive(`${DEPS_OUTPUT_DIR}/bgfx.cmake-src/bgfx/tools`);
  deleteFolderRecursive(`${DEPS_OUTPUT_DIR}/spirv-cross-src/reference`);
  deleteFolderRecursive(`${DEPS_OUTPUT_DIR}/spirv-cross-src/shaders`);
  deleteFolderRecursive(`${DEPS_OUTPUT_DIR}/spirv-cross-src/shaders-msl`);
  deleteFolderRecursive(`${DEPS_OUTPUT_DIR}/spirv-cross-src/shaders-hlsl`);
  deleteFolderRecursive(`${DEPS_OUTPUT_DIR}/spirv-cross-src/shaders-hlsl-no-opt`);
  deleteFolderRecursive(`${DEPS_OUTPUT_DIR}/glslang-src/Test`);
  deleteFolderRecursive(`${DEPS_OUTPUT_DIR}/arcore-android-sdk-src/assets`);
  deleteFolderRecursive(`${DEPS_OUTPUT_DIR}/arcore-android-sdk-src/media`);
  deleteFolderRecursive(`${DEPS_OUTPUT_DIR}/arcore-android-sdk-src/samples`);
  deleteFolderRecursive(`${DEPS_OUTPUT_DIR}/arcore-android-sdk-src/tools`);
  deleteFolderRecursive(`${DEPS_OUTPUT_DIR}/jsruntimehost-src/Tests`);
}

const copyFiles = gulp.series(copyCommonFiles, copySharedFiles, copyAndroidARCoreFiles , copyiOSARKitFiles, copyIOSFiles, copyAndroidFiles, copyWindowsFiles);
const buildAssembled = gulp.series(buildBabylonNativeSourceTree, copyFiles, buildTypeScript, validateAssembled);
const buildIOS = gulp.series(buildIphoneOS); // buildIphoneSimulator is optional. Build is slower and it should not make more checks
const setupDev = gulp.series(buildAssembled, copyJSFilesToModule);

exports.buildAssembled = buildAssembled;
exports.buildTypeScript = buildTypeScript;
exports.validateAssembled = validateAssembled;
exports.buildIOS = buildIOS;
exports.buildAndroid = buildAndroid;
exports.copyFiles = copyFiles;

exports.clean = clean;
exports.default = buildAssembled;

exports.buildBabylonNativeSourceTree = buildBabylonNativeSourceTree;
exports.setupDev = setupDev;
