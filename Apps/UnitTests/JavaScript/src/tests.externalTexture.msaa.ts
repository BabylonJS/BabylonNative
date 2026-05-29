// JS-side scaffolding for Tests.ExternalTexture.Msaa.cpp.
//
// Renders a single rotated red plane (with white clearColor) into a RenderTargetTexture whose
// colorAttachment is a NativeTexture wrapping an externally-created GPU texture. The C++ test
// reads back the resulting pixels and verifies MSAA behavior:
//   - samples=4 case: edge pixels should show partial coverage (a red/white blend), e.g. r~255,
//     g and b between 0 and 255.
//   - samples=1 case: edge pixels are either pure red or pure white -- no blends.
// This characterizes whether MSAA + ExternalTexture renders correctly end-to-end on Babylon Native
// today, before any plumbing or plugin-level fixes land.

import { Camera, Color3, Color4, FreeCamera, MeshBuilder, NativeEngine, RenderTargetTexture, Scene, StandardMaterial, Vector3 } from "@babylonjs/core";

let engine: NativeEngine;
let scene: Scene;
let rtt: RenderTargetTexture;

function startup(outputNativeTexture: any, width: number, height: number, samples: number): void {
    engine = new NativeEngine();
    delete engine.getCaps().parallelShaderCompile;

    scene = new Scene(engine);
    scene.clearColor = new Color4(1, 1, 1, 1);

    // Orthographic camera filling the RTT exactly.
    const camera = new FreeCamera("camera", new Vector3(0, 0, -3), scene);
    camera.setTarget(Vector3.Zero());
    camera.mode = Camera.ORTHOGRAPHIC_CAMERA;
    camera.orthoTop = 1;
    camera.orthoBottom = -1;
    camera.orthoLeft = -1;
    camera.orthoRight = 1;

    // Rotated red plane -- the rotation guarantees long aliased edges across the RTT, which is
    // what the readback later inspects for AA blends.
    const plane = MeshBuilder.CreatePlane("rotated", { size: 1.4 }, scene);
    plane.rotation.z = Math.PI / 7;
    const mat = new StandardMaterial("redMat", scene);
    mat.emissiveColor = new Color3(1, 0, 0);
    mat.disableLighting = true;
    plane.material = mat;

    // Wrap the external GPU texture as the RTT's colorAttachment.
    rtt = new RenderTargetTexture(
        "msaaExternalRtt",
        { width, height },
        scene,
        {
            colorAttachment: engine.wrapNativeTexture(outputNativeTexture),
            generateDepthBuffer: true,
            generateStencilBuffer: false,
            samples,
        });
    rtt.clearColor = new Color4(1, 1, 1, 1);
    rtt.activeCamera = camera;
    rtt.renderList!.push(plane);
}

async function renderFrame(): Promise<void> {
    await scene.whenReadyAsync();
    rtt.render();
}

(globalThis as any).startup = startup;
(globalThis as any).renderFrame = renderFrame;
