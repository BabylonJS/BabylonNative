// JS-side scaffolding for Tests.ExternalTexture.Restore.cpp.
//
// Tests the device-loss / device-restore flow for wrapped native textures:
//   1. Wrap an external GPU texture via wrapNativeTexture.
//   2. Attach it as the colorAttachment of an RTT, render a red plane, readback → "before" pixels.
//   3. Simulate device restore: receive a NEW native texture handle, call updateWrappedNativeTexture
//      to repoint the wrapper without losing the InternalTexture identity.
//   4. Render a BLUE plane into the restored RTT, readback → "after" pixels.
//   5. C++ verifies "before" has red pixels and "after" has blue pixels.

import { Camera, Color3, Color4, FreeCamera, MeshBuilder, NativeEngine, RenderTargetTexture, Scene, StandardMaterial, Vector3 } from "@babylonjs/core";

let engine: NativeEngine;
let scene: Scene;
let rtt: RenderTargetTexture;
let plane: any;
let mat: StandardMaterial;

function startup(outputNativeTexture: any, width: number, height: number): void {
    engine = new NativeEngine();
    delete engine.getCaps().parallelShaderCompile;

    scene = new Scene(engine);
    scene.clearColor = new Color4(1, 1, 1, 1);

    const camera = new FreeCamera("camera", new Vector3(0, 0, -3), scene);
    camera.setTarget(Vector3.Zero());
    camera.mode = Camera.ORTHOGRAPHIC_CAMERA;
    camera.orthoTop = 1;
    camera.orthoBottom = -1;
    camera.orthoLeft = -1;
    camera.orthoRight = 1;

    plane = MeshBuilder.CreatePlane("plane", { size: 1.8 }, scene);
    mat = new StandardMaterial("mat", scene);
    mat.emissiveColor = new Color3(1, 0, 0);
    mat.disableLighting = true;
    plane.material = mat;

    rtt = new RenderTargetTexture(
        "wrappedRtt",
        { width, height },
        scene,
        {
            colorAttachment: engine.wrapNativeTexture(outputNativeTexture),
            generateDepthBuffer: true,
            generateStencilBuffer: false,
        });
    rtt.clearColor = new Color4(1, 1, 1, 1);
    rtt.activeCamera = camera;
    rtt.renderList!.push(plane);
}

async function renderFrame(): Promise<void> {
    await scene.whenReadyAsync();
    rtt.render();
}

function restoreTexture(newNativeTexture: any): void {
    if (typeof (engine as any).updateWrappedNativeTexture !== "function") {
        throw new Error("updateWrappedNativeTexture not available -- requires @babylonjs/core >= 9.9.1");
    }
    const wrappedInternalTexture = rtt.renderTarget!.texture!;
    engine.updateWrappedNativeTexture(wrappedInternalTexture, newNativeTexture);

    // Switch to blue so the C++ side can distinguish "before" (red) from "after" (blue).
    mat.emissiveColor = new Color3(0, 0, 1);
}

(globalThis as any).startup = startup;
(globalThis as any).renderFrame = renderFrame;
(globalThis as any).restoreTexture = restoreTexture;
