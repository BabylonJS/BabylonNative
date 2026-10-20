import { Engine } from '@babylonjs/core';
import { satisfies, minVersion } from 'semver'
import packageDefinition from './package.json';

if (__DEV__) {
    const babylonJSPackageName = "@babylonjs/core";
    const requiredVersion = packageDefinition.peerDependencies[babylonJSPackageName];
    const minRequiredVersion = minVersion(requiredVersion);
    const currentVersion = Engine.Version;
    if (!satisfies(currentVersion, requiredVersion)) {
        console.error(`${packageDefinition.name}@${packageDefinition.version} requires ${babylonJSPackageName}@${requiredVersion} but version ${currentVersion} is currently installed. Run 'npm install ${babylonJSPackageName}@${minRequiredVersion}' in your app's root directory to upgrade.`);
    }
}