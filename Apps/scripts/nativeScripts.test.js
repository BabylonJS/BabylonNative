const assert = require('node:assert/strict');
const { spawnSync } = require('node:child_process');
const { mkdtempSync, mkdirSync, readFileSync, rmSync, writeFileSync } = require('node:fs');
const { tmpdir } = require('node:os');
const path = require('node:path');
const { test } = require('node:test');
const vm = require('node:vm');

const bundles = [
  ['babylonjs/babylon.max.js', 'babylon.es5.js'],
  ['babylonjs-addons/babylonjs.addons.js', 'addons/babylonjs.addons.es5.js'],
  ['babylonjs-materials/babylonjs.materials.js', 'materialsLibrary/babylonjs.materials.es5.js'],
  ['babylonjs-loaders/babylonjs.loaders.js', 'loaders/babylonjs.loaders.es5.js'],
  ['babylonjs-gui/babylon.gui.js', 'gui/babylon.gui.es5.js'],
  ['babylonjs-serializers/babylonjs.serializers.js', 'serializers/babylonjs.serializers.es5.js'],
  ['babylonjs-procedural-textures/babylonjs.proceduralTextures.js', 'proceduralTexturesLibrary/babylonjs.proceduralTextures.es5.js'],
];

function runCopy(directory) {
  return spawnSync(process.execPath, [path.join(__dirname, 'copyEs5Bundles.js')], {
    cwd: directory,
    encoding: 'utf8',
  });
}

test('npm installs copy all seven ES5 bundles under historical names without changing companions', () => {
  const directory = mkdtempSync(path.join(tmpdir(), 'native-es5-'));
  try {
    for (const [destination, source] of bundles) {
      const target = path.join(directory, 'node_modules', destination);
      mkdirSync(path.dirname(target), { recursive: true });
      writeFileSync(path.join(path.dirname(target), path.basename(source)), source);
      writeFileSync(target, 'regular bundle');
      writeFileSync(target + '.map', 'companion');
    }

    for (let iteration = 0; iteration < 2; ++iteration) {
      const result = runCopy(directory);
      assert.equal(result.status, 0, result.stderr);
      for (const [destination, source] of bundles) {
        const target = path.join(directory, 'node_modules', destination);
        assert.equal(readFileSync(target, 'utf8'), source);
        assert.equal(readFileSync(target + '.map', 'utf8'), 'companion');
      }
    }
  } finally {
    rmSync(directory, { recursive: true, force: true });
  }
});

test('missing published ES5 bundles fail the npm install step', () => {
  const directory = mkdtempSync(path.join(tmpdir(), 'native-es5-'));
  try {
    const result = runCopy(directory);
    assert.equal(result.status, 1);
    assert.match(result.stderr, /ENOENT/);
    assert.doesNotMatch(result.stdout, /Copied/);
  } finally {
    rmSync(directory, { recursive: true, force: true });
  }
});

test('nightly downloads use ES5 sources and preserve every destination and source-map URL', () => {
  const context = {};
  vm.createContext(context);
  const script = readFileSync(path.join(__dirname, 'getNightly.js'), 'utf8');
  const start = script.indexOf('var files = [');
  assert.notEqual(start, -1);
  const end = script.indexOf('];', start);
  assert.notEqual(end, -1);
  vm.runInContext(script.slice(start, end + 2), context);
  const actual = JSON.parse(JSON.stringify(context.files));
  const expected = bundles.flatMap(([destination, source]) => {
    const sourceDirectory = source.slice(0, source.lastIndexOf('/') + 1);
    return [
      ['node_modules/' + destination, 'https://preview.babylonjs.com/' + source],
      ['node_modules/' + destination + '.map', 'https://preview.babylonjs.com/' + sourceDirectory + path.basename(destination) + '.map'],
    ];
  });
  assert.deepEqual(actual, expected);
});
