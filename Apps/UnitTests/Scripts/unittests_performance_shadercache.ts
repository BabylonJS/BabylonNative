import {
    Scene,
    NativeEngine,
    Mesh
} from "@babylonjs/core";

declare const setSceneReady: () => void;
declare const setReady: () => void;

console.log("Setting rendering to cache shader.");
var engine = new NativeEngine();
var scene = new Scene(engine);

var sphere = Mesh.CreateSphere("sphere", 32, 0.9, scene);

scene.createDefaultCamera(true, true, true);
scene.activeCamera.alpha += Math.PI;
scene.createDefaultLight(true);
scene.whenReadyAsync().then(function () {
    setSceneReady();
});
setReady();
