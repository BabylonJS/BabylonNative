/// <reference path="../../node_modules/babylonjs/babylon.module.d.ts" />
/// <reference path="../../node_modules/babylonjs-loaders/babylonjs.loaders.module.d.ts" />
/// <reference path="../../node_modules/babylonjs-materials/babylonjs.materials.module.d.ts" />
/// <reference path="../../node_modules/babylonjs-gui/babylon.gui.module.d.ts" />

var engine = new BABYLON.NativeEngine();

// See https://playground.babylonjs.com/#1LK70I#33  <- for viewports
// See https://playground.babylonjs.com/#BCYE7J#41  <- for multiple cameras
//
var createScene = function () {
  var scene = new BABYLON.Scene(engine);

  var cameraDefault = new BABYLON.FreeCamera("camera1", new BABYLON.Vector3(0, 5, -10), scene);
  cameraDefault.setTarget(BABYLON.Vector3.Zero());
  // cameraDefault.attachControl(canvas, true);
  cameraDefault.layerMask = 0xFFFFFFFF;

  var cameraDefault2 = new BABYLON.FreeCamera("camera2", new BABYLON.Vector3(-4, 8, -10), scene);
  cameraDefault2.setTarget(new BABYLON.Vector3(1,1,3));
  // cameraDefault2.attachControl(canvas, true);
  cameraDefault2.layerMask = 0xFFFFFFFF;

  var cameraRTT = new BABYLON.FreeCamera("cameraRTT1", new BABYLON.Vector3(0, 5, -10), scene);
  cameraRTT.setTarget(BABYLON.Vector3.Zero());

  var cameraRTT2 = new BABYLON.FreeCamera("cameraRTT2", new BABYLON.Vector3(4, 8, -10), scene);
  cameraRTT2.setTarget(new BABYLON.Vector3(1,2,3));

  const renderTarget = new BABYLON.RenderTargetTexture('render target', 512, scene, false, false);
  renderTarget.clearColor = new BABYLON.Color4(0, 1, 0, 1);
  renderTarget.renderPassId = undefined;

  renderTarget.onClearObservable.add(() => {
      if (scene.activeCamera === cameraRTT) {
          engine.clear(renderTarget.clearColor || scene.clearColor, true, true, true);
      }
  });

  cameraRTT.outputRenderTarget = renderTarget;
  cameraRTT2.outputRenderTarget = renderTarget;

  var light = new BABYLON.HemisphericLight("light", new BABYLON.Vector3(0, 1, 0), scene);
  light.intensity = 0.7;

  var sphere = BABYLON.MeshBuilder.CreateSphere("sphere", {diameter: 2, segments: 32}, scene);
  sphere.position.y = 1;

  var plane = BABYLON.MeshBuilder.CreatePlane("plane", { size: 3 }, scene);
  plane.position.set(-3, 2.5, 0);
  plane.layerMask = 0x10000000;

  const mat = new BABYLON.StandardMaterial("mat", scene);
  mat.disableLighting = true;
  mat.emissiveTexture = renderTarget;

  plane.material = mat;

  var ground = BABYLON.MeshBuilder.CreateGround("ground", {width: 6, height: 6}, scene);
  // ground.material = new BABYLON.StandardMaterial("ground.material", scene);
  // ground.material.wireframe = true;

  scene.onBeforeRenderObservable.add(() => {
      ground.rotate(BABYLON.Axis.Y, 0.01);
  });

  scene.activeCameras = [cameraDefault, cameraRTT, cameraRTT2, cameraDefault2];

  //   scene.onBeforeCameraRenderObservable.add(camera => {
  //       let renderWidth = -1;
  //       let renderHeight = -1;
  //       if (!camera.logged) console.log(`camera: "${camera.name}:`);
  //       if (camera.outputRenderTarget) {
  //           if (!camera.logged) console.log(`    outputRenderTarget:`);
  //           renderWidth = camera.outputRenderTarget.getRenderWidth();
  //           renderHeight = camera.outputRenderTarget.getRenderHeight();
  //           return;
  //       }
  //       else {
  //           if (!camera.logged) console.log(`    engine:`);
  //           renderWidth = engine.getRenderWidth();
  //           renderHeight = engine.getRenderHeight();
  //       }
  //       if (!camera.logged) console.log(`        render size = ${renderWidth} x ${renderHeight}`);
  //       camera.logged = true;
  //       engine.enableScissor(
  //           renderWidth * (camera.viewport.x + 0.2),
  //           renderHeight * (camera.viewport.y + 0.2),
  //           renderWidth * (camera.viewport.width - 0.4),
  //           renderHeight * (camera.viewport.height - 0.4)
  //       );
  //       // engine.clear(camera.clearColor, true, false, false);
  // });
  // scene.onAfterCameraRenderObservable.add(camera => {
  //     engine.disableScissor();
  // });

  return scene;
};

var scene = createScene();

engine.runRenderLoop(function () {
    scene.render();
});
