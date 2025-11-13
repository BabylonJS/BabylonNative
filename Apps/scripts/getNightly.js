var https = require('https');
var fs = require('fs');

function download(filename, url) {
  var file = fs.createWriteStream(filename);
  var request = https.get(url, function(response) {
    response.pipe(file);
  });
}

console.log('Downloading babylon.js nightly');
download('node_modules/babylonjs/babylon.max.js', 'https://preview.babylonjs.com/babylon.max.js');
download('node_modules/babylonjs/babylon.max.js.map', 'https://preview.babylonjs.com/babylon.max.js.map');
download('node_modules/babylonjs-materials/babylonjs.materials.js', 'https://preview.babylonjs.com/materialsLibrary/babylonjs.materials.js');
download('node_modules/babylonjs-materials/babylonjs.materials.js.map', 'https://preview.babylonjs.com/materialsLibrary/babylonjs.materials.js.map');
download('node_modules/babylonjs-loaders/babylonjs.loaders.js', 'https://preview.babylonjs.com/loaders/babylonjs.loaders.js');
download('node_modules/babylonjs-loaders/babylonjs.loaders.js.map', 'https://preview.babylonjs.com/loaders/babylonjs.loaders.js.map');
download('node_modules/babylonjs-gui/babylon.gui.js', 'https://preview.babylonjs.com/gui/babylon.gui.js');
download('node_modules/babylonjs-gui/babylon.gui.js.map', 'https://preview.babylonjs.com/gui/babylon.gui.js.map');
download('node_modules/babylonjs-serializers/babylonjs.serializers.js', 'https://preview.babylonjs.com/serializers/babylonjs.serializers.js');
download('node_modules/babylonjs-serializers/babylonjs.serializers.js.map', 'https://preview.babylonjs.com/serializers/babylonjs.serializers.js.map');
