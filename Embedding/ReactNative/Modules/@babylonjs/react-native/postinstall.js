const os = require("os");
const path = require("path");

function getCmakeExecutable() {
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

  const cmake = spawn(getCmakeExecutable(), [
    '-S', path.join(__dirname, 'ios'),
    '-B', path.join(__dirname, 'Build/iOS'),
    '-G', 'Xcode',
  ], { stdio: 'inherit', cwd: __dirname });

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
