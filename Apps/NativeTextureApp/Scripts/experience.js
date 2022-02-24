/// <reference path="../../node_modules/babylonjs/babylon.module.d.ts" />
/// <reference path="../../node_modules/babylonjs-loaders/babylonjs.loaders.module.d.ts" />
/// <reference path="../../node_modules/babylonjs-materials/babylonjs.materials.module.d.ts" />
/// <reference path="../../node_modules/babylonjs-gui/babylon.gui.module.d.ts" />

var engine = new BABYLON.NativeEngine();
var scene = new BABYLON.Scene(engine);

var box = BABYLON.MeshBuilder.CreateSphere("sphere", { diameter: 2, segments: 32 }, scene);

scene.createDefaultCamera(true, true, true);
scene.activeCamera.alpha += Math.PI;

scene.createDefaultLight(true);

engine.runRenderLoop(function () 
{
    scene.render();
});

function setBoxTexture(nativeTexture, width, height)
{
    nativeTexture.then(function ()
    {
        let internalTexture = new BABYLON.InternalTexture(engine, BABYLON.InternalTextureSource.Unknown, true);
        internalTexture._hardwareTexture = nativeTexture;
        internalTexture.generateMipMaps = false;
        internalTexture.baseWidth = internalTexture.width = width;
        internalTexture.baseHeight = internalTexture.height = height;
        internalTexture.isReady = true;
        internalTexture.baseDepth = 1;
        internalTexture.depth = 1;
        internalTexture.format = BABYLON.TextureFormat.RGBA8Uint;

        const thinTexture = new BABYLON.ThinTexture(internalTexture);
        thinTexture.isReady = true;

        var mat = new BABYLON.StandardMaterial("mat", scene);
        box.material = mat;
        mat.diffuseTexture = thinTexture;
    });
}
