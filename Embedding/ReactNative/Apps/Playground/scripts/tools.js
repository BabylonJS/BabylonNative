const os = require('os');
const shelljs = require('shelljs');
const chalk = require('chalk');

function iosCMake() {
  console.log(chalk.black.bgCyan('Running CMake for iOS...'));
  shelljs.exec('cmake -B ../../../../Build/iOS -G Xcode', {cwd: 'node_modules/@babylonjs/react-native-iosandroid/ios'});
}

function postInstall() {
  const version = shelljs.exec('npm --version', {silent: true});

  // TODO: This makes development easier as it provides type info when editing App.tsx,
  //       but it also somehow breaks the metro bundler (result in runtime errors).
  // console.log(chalk.black.bgCyan('Installing Playground Shared npm packages...'));
  // shelljs.exec('npm install', {cwd: '../playground-shared'});

  console.log(chalk.black.bgCyan('Installing Babylon React Native npm packages...'));
  shelljs.exec('npm install --legacy-peer-deps', {cwd: '../../../Modules/@babylonjs/react-native'});

  if (os.platform() === 'darwin') {
    iosCMake();

    console.log(chalk.black.bgCyan('Installing iOS pods...'));
    shelljs.exec('pod install', {cwd: 'ios'});
  }
}

// First arg will be 'node', second arg will be 'tools.js'
const [command] = process.argv.slice(2);

if (command === 'postinstall') {
  postInstall();
} else if (command === 'iosCMake') {
  iosCMake();
} else {
  console.error(chalk.black.bgRedBright(`Unkown command: ${command}`));
  process.exit(1);
}