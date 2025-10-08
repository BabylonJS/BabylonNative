import {
    Tools,
    MeshBuilder,
    NativeEngine,
    Scene,
} from "@babylonjs/core";
dddd
function CreateBoxAsync(scene: Scene) {
    MeshBuilder.CreateBox("box1", {size: 0.2}, scene);
    return Promise.resolve();
}

const engine = new NativeEngine();
const scene = new Scene(engine);

CreateBoxAsync(scene).then(function () {
    Tools.Log("Loaded");

    // This creates and positions a free camera (non-mesh)
    scene.createDefaultCamera(true, true, true);
    scene.createDefaultLight(true);

    engine.runRenderLoop(function () {
        scene.render();
    });

}, function (ex) {
    console.log(ex.message, ex.stack);
});
