const os = require('os');
const shelljs = require('shelljs');
const chalk = require('chalk');

function iosCmake() {
  console.log(chalk.black.bgCyan('Running CMake for iOS...'));
  shelljs.exec('cmake -G Xcode -DCMAKE_TOOLCHAIN_FILE=../submodules/BabylonNative/Dependencies/ios-cmake/ios.toolchain.cmake -DPLATFORM=OS64COMBINED -DENABLE_ARC=0 -DENABLE_BITCODE=1 -DDEPLOYMENT_TARGET=12 -DENABLE_GLSLANG_BINARIES=OFF -DSPIRV_CROSS_CLI=OFF -DENABLE_PCH=OFF .', {cwd: 'node_modules/@babylonjs/react-native/ios'});
}

function postInstall() {
  const version = shelljs.exec('npm --version', {silent: true});

  if (!version.trim().match(/6\.\d+\.\d+/g)) {
    throw `Error: BabylonReactNative Playground development requires npm version 6.13.*, Your current npm version is ${version}. Run npm install -g npm@6.13 to update your npm version.`;
  }

  console.log(chalk.black.bgCyan('Installing Babylon React Native npm packages...'));
  shelljs.exec('npm install', {cwd: '../../Modules/@babylonjs/react-native'});

  console.log(chalk.black.bgCyan('Updating submodules...'));
  shelljs.exec('git submodule update --init --recursive', {cwd: '../../'});

  if (os.platform() === 'darwin') {
    iosCmake();

    console.log(chalk.black.bgCyan('Installing iOS pods...'));
    shelljs.exec('pod install', {cwd: 'ios'});
  }
}

// First arg will be 'node', second arg will be 'tools.js'
const [command] = process.argv.slice(2);

if (command === 'postinstall') {
  postInstall();
} else if (command === 'iosCMake') {
  iosCmake();
} else {
  console.error(chalk.black.bgRedBright(`Unkown command: ${command}`));
  process.exit(1);
}