/**
 * Metro configuration for React Native
 * https://github.com/facebook/react-native
 *
 * @format
 */

const path = require('path');
const fs = require('fs');

// NOTE: The Metro bundler does not support symlinks (see https://github.com/facebook/metro/issues/1), which NPM uses for local packages.
//       To work around this, we explicity tell the metro bundler where to find local/linked packages.

// Create a mapping of package ids to linked directories.
function getModuleMappings() {
  const nodeModulesPath = path.resolve(__dirname, 'node_modules');
  let moduleMappings = {};

  function findPackageDirs(directory) {
    fs.readdirSync(directory).forEach(item => {
      const itemPath = path.resolve(directory, item);
      const itemStat = fs.lstatSync(itemPath);
      if (itemStat.isSymbolicLink()) {
        let linkPath = fs.readlinkSync(itemPath);
        // Sym links are relative in Unix, absolute in Windows.
        if (!path.isAbsolute(linkPath)) {
          linkPath = path.resolve(directory, linkPath);
        }
        const linkStat = fs.lstatSync(linkPath);
        if (linkStat.isDirectory()) {
          const packagePath = path.resolve(linkPath, "package.json");
          if (fs.existsSync(packagePath)) {
            const packageId = path.relative(nodeModulesPath, itemPath);
            moduleMappings[packageId] = linkPath;
          }
        }
      } else if (itemStat.isDirectory()) {
        findPackageDirs(itemPath);
      }
    });
  }

  findPackageDirs(nodeModulesPath);

  return moduleMappings;
}

const moduleMappings = getModuleMappings();
console.log("Mapping the following sym linked packages:");
console.log(moduleMappings);

module.exports = {
  transformer: {
    getTransformOptions: async () => ({
      transform: {
        experimentalImportSupport: false,
        inlineRequires: false,
      },
    }),
  },

  resolver: {
    // Register an "extra modules proxy" for resolving modules outside of the normal resolution logic.
    extraNodeModules: new Proxy(
        // Provide the set of known local package mappings.
        moduleMappings,
        {
            // Provide a mapper function, which uses the above mappings for associated package ids,
            // otherwise fall back to the standard behavior and just look in the node_modules directory.
            get: (target, name) => name in target ? target[name] : path.join(__dirname, `node_modules/${name}`),
        },
    ),
  },

  projectRoot: path.resolve(__dirname),

  // Also additionally watch all the mapped local directories for changes to support live updates.
  watchFolders: Object.values(moduleMappings),
};
