import {
    Scene,
    NativeEngine,
    Mesh
} from "@babylonjs/core";

declare const setSceneReady: () => void;
declare const setReady: () => void;

console.log("Setting up Performance test.");
var engine = new NativeEngine();
var scene = new Scene(engine);

var size = 12;
for (var i = 0; i < size; i++) {
    for (var j = 0; j < size; j++) {
        for (var k = 0; k < size; k++) {
            var sphere = Mesh.CreateSphere("sphere" + i + j + k, 32, 0.9, scene);
            sphere.position.x = i;
            sphere.position.y = j;
            sphere.position.z = k;
        }
    }
}

scene.createDefaultCamera(true, true, true);
scene.activeCamera.alpha += Math.PI;
scene.createDefaultLight(true);
engine.runRenderLoop(function () {
    scene.render();
});
console.log("Ready!");
setReady();
