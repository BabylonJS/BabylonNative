const os = require("os");
const shelljs = require("shelljs");
const chalk = require("chalk");

function exec(cmd, options) {
  const { code } = shelljs.exec(cmd, options);
  if (code !== 0) {
    shelljs.exit(code);
  }
}

function iosCMake() {
  console.log(chalk.black.bgCyan("Running CMake for iOS..."));
  exec("cmake -B ../../Build/iOS -G Xcode", {
    cwd: "node_modules/@babylonjs/react-native-iosandroid/ios",
  });
}

function postInstall() {
  // TODO: This makes development easier as it provides type info when editing App.tsx,
  //       but it also somehow breaks the metro bundler (result in runtime errors).
  // console.log(chalk.black.bgCyan('Installing Playground Shared npm packages...'));
  // shelljs.exec('npm install', {cwd: '../playground-shared'});

  console.log(
    chalk.black.bgCyan("Installing Babylon React Native npm packages...")
  );
  exec("npm install --legacy-peer-deps", {
    cwd: "../../Modules/@babylonjs/react-native",
  });

  if (os.platform() === "darwin") {
    iosCMake();

    exec("npm install && npx gulp buildIOSRNTA", {
      cwd: "../../Package",
    });

    console.log(chalk.black.bgCyan("Installing iOS pods..."));
    exec("pod install", { cwd: "ios" });
  }
}

postInstall();
