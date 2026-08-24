var https = require('https');
var fs = require('fs');
var path = require('path');
var spawnSync = require('child_process').spawnSync;

function download(filename, url) {
  return new Promise(function (resolve, reject) {
    var file = fs.createWriteStream(filename);
    var settled = false;

    function fail(err) {
      if (settled) {
        return;
      }
      settled = true;
      file.destroy();
      reject(err);
    }

    file.on('error', fail);
    // 'close', not 'finish': 'finish' fires once the data is flushed but before the descriptor is
    // released, and the down-level step runs against these files immediately afterwards.
    file.on('close', function () {
      if (!settled) {
        settled = true;
        resolve();
      }
    });

    var request = https.get(url, function (response) {
      if (response.statusCode !== 200) {
        response.resume();
        fail(new Error('GET ' + url + ' failed with status ' + response.statusCode));
        return;
      }
      response.on('error', fail);
      // A server-side abort emits neither 'finish' nor 'error' on the file, so without this the
      // promise would never settle.
      response.on('aborted', function () {
        fail(new Error('GET ' + url + ' aborted by the server'));
      });
      response.pipe(file);
    });

    request.on('error', fail);
    request.setTimeout(120000, function () {
      request.destroy(new Error('GET ' + url + ' timed out'));
    });
  });
}

var files = [
  ['node_modules/babylonjs/babylon.max.js', 'https://preview.babylonjs.com/babylon.max.js'],
  ['node_modules/babylonjs/babylon.max.js.map', 'https://preview.babylonjs.com/babylon.max.js.map'],
  ['node_modules/babylonjs-addons/babylonjs.addons.js', 'https://preview.babylonjs.com/addons/babylonjs.addons.js'],
  ['node_modules/babylonjs-addons/babylonjs.addons.js.map', 'https://preview.babylonjs.com/addons/babylonjs.addons.js.map'],
  ['node_modules/babylonjs-materials/babylonjs.materials.js', 'https://preview.babylonjs.com/materialsLibrary/babylonjs.materials.js'],
  ['node_modules/babylonjs-materials/babylonjs.materials.js.map', 'https://preview.babylonjs.com/materialsLibrary/babylonjs.materials.js.map'],
  ['node_modules/babylonjs-loaders/babylonjs.loaders.js', 'https://preview.babylonjs.com/loaders/babylonjs.loaders.js'],
  ['node_modules/babylonjs-loaders/babylonjs.loaders.js.map', 'https://preview.babylonjs.com/loaders/babylonjs.loaders.js.map'],
  ['node_modules/babylonjs-gui/babylon.gui.js', 'https://preview.babylonjs.com/gui/babylon.gui.js'],
  ['node_modules/babylonjs-gui/babylon.gui.js.map', 'https://preview.babylonjs.com/gui/babylon.gui.js.map'],
  ['node_modules/babylonjs-serializers/babylonjs.serializers.js', 'https://preview.babylonjs.com/serializers/babylonjs.serializers.js'],
  ['node_modules/babylonjs-serializers/babylonjs.serializers.js.map', 'https://preview.babylonjs.com/serializers/babylonjs.serializers.js.map'],
  ['node_modules/babylonjs-procedural-textures/babylonjs.proceduralTextures.js', 'https://preview.babylonjs.com/proceduralTexturesLibrary/babylonjs.proceduralTextures.js'],
  ['node_modules/babylonjs-procedural-textures/babylonjs.proceduralTextures.js.map', 'https://preview.babylonjs.com/proceduralTexturesLibrary/babylonjs.proceduralTextures.js.map'],
];

console.log('Downloading babylon.js nightly');

// Awaited so a failed or partial download fails the script instead of silently leaving the previous
// file in place, and so the down-level step that runs next cannot race an incomplete write.
Promise.all(files.map(function (f) { return download(f[0], f[1]); })).then(function () {
  console.log('Downloaded ' + files.length + ' file(s)');

  // Every bundle we just replaced needs the ES5 down-level, not just babylon.max.js: they all end up
  // in BABYLON_SCRIPTS and all have to run on Chakra. Deriving the list from `files` means adding a
  // new download above can never again silently skip this step.
  var scripts = files
    .map(function (f) { return f[0]; })
    .filter(function (p) { return /babylon.*\.js$/i.test(p); });

  if (scripts.length === 0) {
    return;
  }

  console.log('Down-leveling ' + scripts.length + ' script(s)');
  var result = spawnSync(
    process.execPath,
    [path.join(__dirname, 'downlevelNativeScripts.mjs')].concat(scripts),
    { stdio: 'inherit' });

  if (result.error) {
    throw result.error;
  }
  if (result.status !== 0) {
    throw new Error('downlevelNativeScripts.mjs exited with code ' + result.status);
  }
}).catch(function (err) {
  console.error(err.message);
  process.exit(1);
});
