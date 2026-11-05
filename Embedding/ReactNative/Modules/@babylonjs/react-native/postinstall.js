const os = require("os");
const path = require("path");

function getCmakeExecutable() {
  // When BABYLON_USE_SYSTEM_CMAKE=1, skip the npm cmake package and use whatever
  // cmake is found on PATH (e.g. a Homebrew or system install).
  if (process.env.BABYLON_USE_SYSTEM_CMAKE === '1') {
    return 'cmake';
  }
  try {
    // cmake-runtime ships the cmake binary; resolve it directly to avoid
    // relying on npx or PATH.
    const pkgDir = path.dirname(require.resolve('cmake-runtime/package.json'));
    const pkg = require('cmake-runtime/package.json');
    // cmake-runtime exposes the binary path via its "bin" field
    const binRelative = typeof pkg.bin === 'string' ? pkg.bin : pkg.bin['cmake'];
    return path.join(pkgDir, binRelative);
  } catch (e) {
    // Fall back to a cmake on PATH (e.g. homebrew or system install)
    return 'cmake';
  }
}

function iosCMake() {
  const { spawn } = require('child_process');

  const cmakeArgs = [
    '-S', path.join(__dirname, 'ios'),
    '-B', path.join(__dirname, 'Build/iOS'),
    '-G', 'Xcode',
  ];

  // Allow the consuming project to override the iOS deployment target used
  // when generating the Xcode project. The CMakeLists.txt default is 16.0
  // (iOS 12 used to be the default but is known to crash Xcode 26's Clang
  // when compiling DeviceImpl_iOS.mm); set this if you need a different
  // minimum to match your Podfile `platform :ios` value.
  const deploymentTarget = process.env.BABYLON_IOS_DEPLOYMENT_TARGET;
  if (deploymentTarget && deploymentTarget.trim() !== '') {
    cmakeArgs.push(`-DDEPLOYMENT_TARGET=${deploymentTarget.trim()}`);
  }

  const cmake = spawn(getCmakeExecutable(), cmakeArgs, { stdio: 'inherit', cwd: __dirname });

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
