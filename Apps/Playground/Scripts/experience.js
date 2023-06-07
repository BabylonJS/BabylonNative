/// <reference path="../../node_modules/babylonjs/babylon.module.d.ts" />
/// <reference path="../../node_modules/babylonjs-loaders/babylonjs.loaders.module.d.ts" />
/// <reference path="../../node_modules/babylonjs-materials/babylonjs.materials.module.d.ts" />
/// <reference path="../../node_modules/babylonjs-gui/babylon.gui.module.d.ts" />

var engine = new BABYLON.NativeEngine();

// See https://playground.babylonjs.com/#1LK70I#30
//
var createScene = function () {
    const scene = new BABYLON.Scene(engine);

  // Create cameras
  let cameras = [];
  const numX = 2;
  const numY = 3;
  for (let i = 0; i < numX; ++i) {
    for (let j = 0; j < numY; ++j) {
      const x = 2 * i - numX + 1;
      const y = 2 * j - numY + 1;

      // This creates and positions a free camera (non-mesh)
      const camera = new BABYLON.FreeCamera(
        `camera-${i}-${j}`,
        new BABYLON.Vector3(x, y + 10, -10),
        scene,
      );
      camera.viewport = new BABYLON.Viewport(
        i / numX,
        j / numY,
        1 / numX,
        1 / numY,
      );
      camera.clearColor = new BABYLON.Color3(i / numX, j / numY, 0.5);

      camera.setTarget(new BABYLON.Vector3(x, y, 0));
      scene.activeCameras.push(camera);

      cameras.push(camera);
    }
  }

  // This creates a light, aiming 0,1,0 - to the sky (non-mesh)
  const light = new BABYLON.HemisphericLight(
    'light',
    new BABYLON.Vector3(0, 1, 0),
    scene,
  );

  // Default intensity is 1. Let's dim the light a small amount
  light.intensity = 0.7;

  // Our built-in 'sphere' shape.
  const sphere = BABYLON.MeshBuilder.CreateSphere(
    'sphere',
    {diameter: 2, segments: 32},
    scene,
  );

  // Move the sphere upward 1/2 its height
  sphere.position.y = 1;

  // Our built-in 'ground' shape.
  BABYLON.MeshBuilder.CreateGround('ground', {width: 6, height: 6}, scene);

    const renderWidth = engine.getRenderWidth();
    const renderHeight = engine.getRenderHeight();

    scene.onBeforeCameraRenderObservable.add(camera => {
        engine.enableScissor(
            renderWidth * (camera.viewport.x + 0.1),
            renderHeight * (camera.viewport.y + 0.1),
            renderWidth * (camera.viewport.width - 0.2),
            renderHeight * (camera.viewport.height - 0.2)
        );
        engine.clear(camera.clearColor, true, false, false);
    });
    scene.onAfterCameraRenderObservable.add(camera => {
        engine.scissorClear();
    });

  return scene;
};

var scene = createScene();

engine.runRenderLoop(function () {
    scene.render();
});
