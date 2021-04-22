const shelljs = require('shelljs');
const version = shelljs.exec('npm --version');

if (!version.trim().match(/6\.\d+\.\d+/g)) {
    throw `Error: BabylonReactNative Playground development requires npm version 6.13.*, Your current npm version is ${version}. Run npm install -g npm@6.13 to update your npm version.`;
}