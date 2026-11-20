import * as Mocha from "mocha";
import { expect } from "chai";
import {
    Color4,
    Constants,
    FreeCamera,
    Mesh,
    NativeEngine,
    RenderTargetTexture,
    Scene,
    ShaderMaterial,
    Vector3,
    VertexBuffer,
} from "@babylonjs/core";

export function registerIntegerClearTests(
    describe: typeof Mocha.describe,
    it: typeof Mocha.it,
    enabled: boolean
): void {
    describe("NativeEngine integer clears", function () {
        this.timeout(10000);
        (enabled ? it : it.skip)("scales the clear color by 255 like WebGL clearBufferuiv", async function () {
            const engine = new NativeEngine();
            const scene = new Scene(engine);
            try {
                const camera = new FreeCamera("camera", new Vector3(0, 0, -1), scene);
                camera.setTarget(Vector3.Zero());
                scene.activeCamera = camera;

                const integerTarget = new RenderTargetTexture("integerClearTarget", 4, scene, false, true,
                    Constants.TEXTURETYPE_UNSIGNED_INTEGER, false, Constants.TEXTURE_NEAREST_SAMPLINGMODE,
                    false, false, false, Constants.TEXTUREFORMAT_RGBA_INTEGER);
                // 255x gives fractional values, exercising truncation as well as scaling.
                integerTarget.clearColor = new Color4(0.25, 0.004, 0.001, 1);
                integerTarget.renderList = [];

                const material = new ShaderMaterial("integerClear", scene, {
                    vertexSource: `
                        precision highp float;
                        attribute vec3 position;
                        void main() { gl_Position = vec4(position, 1.0); }`,
                    fragmentSource: `
                        precision highp float;
                        precision highp usampler2D;
                        uniform usampler2D integerTexture;
                        void main() {
                            gl_FragColor = vec4(texelFetch(integerTexture, ivec2(0, 0), 0)) / 255.0;
                        }`,
                }, { attributes: ["position"], uniforms: [], samplers: ["integerTexture"] });
                material.setTexture("integerTexture", integerTarget);

                const quad = new Mesh("fullscreen", scene);
                quad.setVerticesData(VertexBuffer.PositionKind, [-1, -1, 0, 1, -1, 0, -1, 1, 0, -1, 1, 0, 1, -1, 0, 1, 1, 0]);
                quad.isUnIndexed = true;
                quad.material = material;

                const target = new RenderTargetTexture("integerClearReadback", 4, scene, false);
                target.renderList = [quad];

                await material.forceCompilationAsync(quad);
                await scene.whenReadyAsync();

                integerTarget.render();
                target.render();
                const pixels = await target.readPixels();
                if (!(pixels instanceof Uint8Array)) {
                    throw new Error("Expected RGBA8 integer-clear readback");
                }
                // trunc(255 * (0.25, 0.004, 0.001, 1)), as Babylon.js stores them into a Uint32Array.
                expect(Array.from(pixels.subarray(0, 4))).to.deep.equal([63, 1, 0, 255]);
            } finally {
                scene.dispose();
                engine.dispose();
            }
        });
    });
}
