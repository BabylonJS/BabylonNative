const os = require("os");

function iosCMake() {
  const { spawn } = require('child_process');

  const cmake = spawn('npx', [
    'cmake',
    '-S', 'ios',
    '-B', 'Build/iOS',
    '-G', 'Xcode',
  ], { stdio: 'inherit' });

  cmake.on('exit', code => {
    if (code !== 0) {
      console.error(`CMake exited with code ${code}`);
      process.exit(code);
    }
  });
}

function postInstall() {
  if (
    os.platform() === "darwin" &&
    process.env.BABYLON_NO_CMAKE_POSTINSTALL !== "1"
  ) {
    iosCMake();
  }
}

postInstall();
