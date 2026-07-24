var https = require('https');
var fs = require('fs');

function download(filename, url) {
  return new Promise(function (resolve, reject) {
    https.get(url, function (response) {
      if (response.statusCode !== 200) {
        response.resume();
        reject(new Error('GET ' + url + ' failed with status ' + response.statusCode));
        return;
      }
      var file = fs.createWriteStream(filename);
      file.on('error', reject);
      file.on('finish', resolve);
      response.pipe(file);
    }).on('error', reject);
  });
}

var files = [
  ['node_modules/babylonjs/babylon.max.js', 'https://preview.babylonjs.com/babylon.max.js'],
  ['node_modules/babylonjs/babylon.max.js.map', 'https://preview.babylonjs.com/babylon.max.js.map'],
  ['node_modules/babylonjs-materials/babylonjs.materials.js', 'https://preview.babylonjs.com/materialsLibrary/babylonjs.materials.js'],
  ['node_modules/babylonjs-materials/babylonjs.materials.js.map', 'https://preview.babylonjs.com/materialsLibrary/babylonjs.materials.js.map'],
  ['node_modules/babylonjs-loaders/babylonjs.loaders.js', 'https://preview.babylonjs.com/loaders/babylonjs.loaders.js'],
  ['node_modules/babylonjs-loaders/babylonjs.loaders.js.map', 'https://preview.babylonjs.com/loaders/babylonjs.loaders.js.map'],
  ['node_modules/babylonjs-gui/babylon.gui.js', 'https://preview.babylonjs.com/gui/babylon.gui.js'],
  ['node_modules/babylonjs-gui/babylon.gui.js.map', 'https://preview.babylonjs.com/gui/babylon.gui.js.map'],
  ['node_modules/babylonjs-serializers/babylonjs.serializers.js', 'https://preview.babylonjs.com/serializers/babylonjs.serializers.js'],
  ['node_modules/babylonjs-serializers/babylonjs.serializers.js.map', 'https://preview.babylonjs.com/serializers/babylonjs.serializers.js.map'],
];

console.log('Downloading babylon.js nightly');

// Awaited so a failed or partial download fails the script instead of silently leaving the previous
// file in place, and so the down-level step that runs next cannot race an incomplete write.
Promise.all(files.map(function (f) { return download(f[0], f[1]); })).then(function () {
  console.log('Downloaded ' + files.length + ' file(s)');
}, function (err) {
  console.error(err.message);
  process.exit(1);
});
