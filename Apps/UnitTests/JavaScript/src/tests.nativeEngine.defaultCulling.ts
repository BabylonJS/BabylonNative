import * as Mocha from "mocha";
import { expect } from "chai";
import {
    Color4,
    FreeCamera,
    Mesh,
    NativeEngine,
    RenderTargetTexture,
    Scene,
    ShaderMaterial,
    Vector3,
    VertexBuffer,
} from "@babylonjs/core";

export function registerDefaultCullingTests(
    describe: typeof Mocha.describe,
    it: typeof Mocha.it,
    enabled: boolean
): void {
    describe("NativeEngine default culling", function () {
        this.timeout(10000);
        (enabled ? it : it.skip)("starts with WebGL's culling and front-face defaults", async function () {
            const engine = new NativeEngine();
            engine.setState = () => {};

            const scene = new Scene(engine);
            try {
                scene.clearColor = new Color4(0, 0, 0, 1);
                const camera = new FreeCamera("camera", new Vector3(0, 0, -1), scene);
                camera.setTarget(Vector3.Zero());
                scene.activeCamera = camera;

                const material = new ShaderMaterial("defaultCulling", scene, {
                    vertexSource: `
                        precision highp float;
                        attribute vec3 position;
                        void main() { gl_Position = vec4(position, 1.0); }`,
                    fragmentSource: `
                        precision highp float;
                        void main() {
                            gl_FragColor = gl_FrontFacing
                                ? vec4(0.0, 1.0, 0.0, 1.0)
                                : vec4(1.0, 0.0, 0.0, 1.0);
                        }`,
                }, { attributes: ["position"], uniforms: [] });

                const mesh = new Mesh("oppositeWindings", scene);
                mesh.setVerticesData(VertexBuffer.PositionKind, [
                    -0.9, -0.6, 0, -0.1, -0.6, 0, -0.5, 0.6, 0,
                    0.1, -0.6, 0, 0.5, 0.6, 0, 0.9, -0.6, 0,
                ]);
                mesh.isUnIndexed = true;
                mesh.material = material;

                const target = new RenderTargetTexture("defaultCullingTarget", 64, scene, false);
                target.clearColor = new Color4(0, 0, 0, 1);
                target.renderList = [mesh];

                await material.forceCompilationAsync(mesh);
                await scene.whenReadyAsync();

                const renderAndSample = async (): Promise<number[][]> => {
                    target.render();
                    const pixels = await target.readPixels();
                    if (!(pixels instanceof Uint8Array)) {
                        throw new Error("Expected RGBA8 default-culling readback");
                    }

                    const sample = (x: number): number[] => {
                        const offset = (32 * 64 + x) * 4;
                        return Array.from(pixels.subarray(offset, offset + 4));
                    };
                    return [sample(16), sample(48)];
                };

                const before = await renderAndSample();
                expect(before[0], "CCW before cull-face selection").to.deep.equal([0, 255, 0, 255]);
                expect(before[1], "CW before cull-face selection").to.deep.equal([255, 0, 0, 255]);

                engine.setStateCullFaceType(false, true);
                const after = await renderAndSample();
                expect(after[0], "CCW after selecting front-face culling").to.deep.equal([0, 0, 0, 255]);
                expect(after[1], "CW after selecting front-face culling").to.deep.equal([255, 0, 0, 255]);
            } finally {
                scene.dispose();
                engine.dispose();
            }
        });
    });
}
