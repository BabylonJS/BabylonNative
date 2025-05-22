/// <reference path="../node_modules/babylonjs/babylon.module.d.ts" />
/// <reference path="../node_modules/babylonjs-loaders/babylonjs.loaders.module.d.ts" />

let engine = null;
let scene = null;
let outputTexture = null;

/**
 * Sets up the engine, scene, and output texture.
 */
function startup(nativeTexture, width, height) {
    // Create a new native engine.
    engine = new BABYLON.NativeEngine();

    // Create a scene with a white background.
    scene = new BABYLON.Scene(engine);

    // Wrap the input native texture in a render target texture for the output
    // render target of the camera used in `loadAndRenderAssetAsync` below.
    outputTexture = new BABYLON.RenderTargetTexture(
        "outputTexture",
        {
            width: width,
            height: height
        },
        scene,
        {
            colorAttachment: engine.wrapNativeTexture(nativeTexture),
            generateDepthBuffer: true,
            generateStencilBuffer: true
        }
    );

    scene.createDefaultLight(true);

    // Create a camera that looks at the asset from a specific angle
    // and outputs to the render target created in `startup` above.
    const camera = new BABYLON.ArcRotateCamera("camera", 2, 1.25, 2, new BABYLON.Vector3(0, 0.2, 0), scene);
    camera.lowerRadiusLimit = 0.5;
    camera.wheelPrecision = 50;
    camera.minZ = 0.02;
    camera.maxZ = 2000;
    camera.speed = 0.4;
    camera.outputRenderTarget = outputTexture;
    camera.attachControl();

    BABYLON.SceneLoader.AppendAsync("https://raw.githubusercontent.com/KhronosGroup/glTF-Sample-Models/master/2.0/FlightHelmet/glTF/FlightHelmet.gltf");

    engine.runRenderLoop(function () {
        scene.render();
    });
}