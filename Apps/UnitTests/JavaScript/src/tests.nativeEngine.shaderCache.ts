import { MeshBuilder, NativeEngine, Scene } from "@babylonjs/core";

declare const setSceneReady: () => void;

console.log("Setting rendering to cache shader.");
const engine = new NativeEngine();
const scene = new Scene(engine);

MeshBuilder.CreateSphere("sphere", { segments: 32, diameter: 0.9 }, scene);

scene.createDefaultCameraOrLight(true, true, true);
scene.executeWhenReady(setSceneReady);
