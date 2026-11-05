var fs = require('fs').promises;
var path = require('path');

var bundles = [
  ['babylonjs', 'babylon.es5.js', 'babylon.max.js'],
  ['babylonjs-addons', 'babylonjs.addons.es5.js', 'babylonjs.addons.js'],
  ['babylonjs-materials', 'babylonjs.materials.es5.js', 'babylonjs.materials.js'],
  ['babylonjs-loaders', 'babylonjs.loaders.es5.js', 'babylonjs.loaders.js'],
  ['babylonjs-gui', 'babylon.gui.es5.js', 'babylon.gui.js'],
  ['babylonjs-serializers', 'babylonjs.serializers.es5.js', 'babylonjs.serializers.js'],
  ['babylonjs-procedural-textures', 'babylonjs.proceduralTextures.es5.js', 'babylonjs.proceduralTextures.js'],
];

Promise.all(bundles.map(function (bundle) {
  var directory = path.join('node_modules', bundle[0]);
  return fs.copyFile(path.join(directory, bundle[1]), path.join(directory, bundle[2]));
})).then(function () {
  console.log('Copied ' + bundles.length + ' ES5 bundle(s)');
}).catch(function (err) {
  console.error(err.message);
  process.exitCode = 1;
});
