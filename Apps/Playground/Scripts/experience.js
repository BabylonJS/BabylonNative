/// <reference path="../../node_modules/babylonjs/babylon.module.d.ts" />
/// <reference path="../../node_modules/babylonjs-loaders/babylonjs.loaders.module.d.ts" />
/// <reference path="../../node_modules/babylonjs-materials/babylonjs.materials.module.d.ts" />
/// <reference path="../../node_modules/babylonjs-gui/babylon.gui.module.d.ts" />

const url1 = "https://i.imgur.com/B93vzrL.jpeg";

const vertexShaderCode = `
uniform mat4 worldViewProjection;
uniform vec2 meshSize;

attribute vec3 position;

varying vec2 global_positionNormalized;

void main()
{
    global_positionNormalized=(position.xy/meshSize.xy)+vec2(0.5,0.5);
    gl_Position=worldViewProjection*vec4(position,1.0);
}
`;

function getFragmentShaderCode() {
  return `
varying vec2 global_positionNormalized;

uniform sampler2D mediaTexture0;

void main()
{
gl_FragColor=texture(mediaTexture0,global_positionNormalized);
}
`;
}

let populateScene = function (scene) {
  if (window) {
    engine.setHardwareScalingLevel(1 / window.devicePixelRatio);
  }
  let meshSize = new BABYLON.Vector2(0.8, 0.8);
  let texture1 = new BABYLON.Texture(url1, scene);
  let material = new BABYLON.ShaderMaterial(
    "customMaterial",
    scene,
    {
      vertexSource: vertexShaderCode,
      fragmentSource: getFragmentShaderCode(),
    },
    {
      attributes: ["position"],
      uniforms: ["worldViewProjection","meshSize"],
      samplers: ["mediaTexture0"],
    }
  );
  material.setVector2("meshSize", meshSize);
  material.setTexture("mediaTexture0", texture1);
  let rectMesh = BABYLON.MeshBuilder.CreatePlane(
    "rectMesh",
    { width: meshSize.x, height: meshSize.y },
    scene
  );
  rectMesh.setMaterialById(material.id);
  return scene;
};

function StartAsync(scene) {
    return Promise.resolve();
}

var engine = new BABYLON.NativeEngine();
var scene = new BABYLON.Scene(engine);
var lastRenderTime = -1;

StartAsync(scene).then(function () {
  try {
    populateScene(scene);
  } catch (e) {
    BABYLON.Tools.Log(e.message);
  }
  BABYLON.Tools.Log("Loaded");

    // This creates and positions a free camera (non-mesh)
    scene.createDefaultCamera(true, true, true);

      scene.createDefaultLight(true);

  engine.runRenderLoop(function () {
    try {
      scene.render();
      var renderTime = new Date().valueOf();
      var span = renderTime - lastRenderTime;
      if (lastRenderTime >= 0) {
        console.log(`${span} ms since last render${span>33?" - very long!":""}.`);
      }
      lastRenderTime = renderTime;
    } catch (e) {
      BABYLON.Tools.Log(e.message);
    }
    });

}, function (ex) {
    console.log(ex.message, ex.stack);
});
