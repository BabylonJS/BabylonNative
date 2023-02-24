/// <reference path="../../node_modules/babylonjs/babylon.module.d.ts" />
/// <reference path="../../node_modules/babylonjs-loaders/babylonjs.loaders.module.d.ts" />
/// <reference path="../../node_modules/babylonjs-materials/babylonjs.materials.module.d.ts" />
/// <reference path="../../node_modules/babylonjs-gui/babylon.gui.module.d.ts" />


const url1 = "https://i.imgur.com/B93vzrL.jpeg";

function glslFloat(float) {
  return ` ${float.toString(10)}${float % 1 === 0 ? '.' : ''} `;
}

function randomGlslFloat() {
  return glslFloat(Math.random());
}

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
  var ifs = [];
  for (var i = 0; i < 30; i++) {
    ifs.push(`if(col.x${"<>"[Math.floor(Math.random() * 2)]}${randomGlslFloat()} && col.y${"<>"[Math.floor(Math.random() * 2)]}${randomGlslFloat()} && col.z${"<>"[Math.floor(Math.random() * 2)]}${randomGlslFloat()})
{
return vec4(${randomGlslFloat()},${randomGlslFloat()},${randomGlslFloat()},1.);
}`);
  }
  return `
varying vec2 global_positionNormalized;

uniform sampler2D mediaTexture0;

vec4 getColor()
{
vec4 col=texture(mediaTexture0,global_positionNormalized);
${ifs.join("\nelse ")}
return vec4(${randomGlslFloat()},${randomGlslFloat()},${randomGlslFloat()},1.);
}

void main()
{
gl_FragColor=getColor();
}
`;
}

var meshSize = new BABYLON.Vector2(0.8, 0.8);
var engine;
var scene;
var texture;
var material;
var waitingMaterial;
var rectMesh;
var lastRefreshTime = -1;

let refreshShader = function () {
  console.log(`Start refresh shader.`);
  lastRefreshTime = new Date().valueOf();
  var oldMaterial = material;
  material = waitingMaterial;
  if (material) {
    rectMesh.setMaterialById(material.id);
  }
  if (oldMaterial && oldMaterial!=material) {
    oldMaterial.dispose(true);
  }
  waitingMaterial = new BABYLON.ShaderMaterial(
    `customMaterial${lastRefreshTime.toString(36)}${Math.random().toString(36)}`,
    scene,
    {
      vertexSource: vertexShaderCode,
      fragmentSource: getFragmentShaderCode(),
    },
    {
      attributes: ["position"],
      uniforms: ["worldViewProjection", "meshSize"],
      samplers: ["mediaTexture0"],
    },
    false
  );
  waitingMaterial.setVector2("meshSize", meshSize);
  waitingMaterial.setTexture("mediaTexture0", texture);
  if (!material) {
    material = waitingMaterial;
    if (material) {
      rectMesh.setMaterialById(material.id);
    }
  }
}

function populateScene() {
  if (window) {
    engine.setHardwareScalingLevel(1 / window.devicePixelRatio);
  }
  texture = new BABYLON.Texture(url1, scene);
  rectMesh = BABYLON.MeshBuilder.CreatePlane(
    "rectMesh",
    { width: meshSize.x, height: meshSize.y },
    scene
  );
  refreshShader();
};

function StartAsync() {
    return Promise.resolve();
}

engine = new BABYLON.NativeEngine();
scene = new BABYLON.Scene(engine);
var lastRenderTime = -1;

StartAsync(scene).then(function () {
  try {
    populateScene();
  } catch (e) {
    BABYLON.Tools.Log(e.message);
  }
  BABYLON.Tools.Log("Loaded");

    // This creates and positions a free camera (non-mesh)
  scene.createDefaultCamera(true, true, true);

  scene.createDefaultLight(true);

  engine.runRenderLoop(function () {
    try {
      if (material && material.isReady()) {
        scene.render();
        var renderTime = new Date().valueOf();
        var span = renderTime - lastRenderTime;
        if (lastRenderTime >= 0) {
          console.log(`${span} ms since last render${span>33?" - too long!":""}.`);
        }
        lastRenderTime = renderTime;
        if (lastRefreshTime>=0 && renderTime-lastRefreshTime > 1000) {
          refreshShader();
        }
      }
    } catch (e) {
      BABYLON.Tools.Log(e.message);
    }
  });

}, function (ex) {
    console.log(ex.message, ex.stack);
});
