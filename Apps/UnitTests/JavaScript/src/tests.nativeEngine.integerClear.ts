import * as Mocha from "mocha";
import { expect } from "chai";
import {
    Color4,
    Constants,
    FreeCamera,
    Mesh,
    MultiRenderTarget,
    NativeEngine,
    RenderTargetTexture,
    Scene,
    ShaderMaterial,
    Vector3,
    VertexBuffer,
} from "@babylonjs/core";

declare const _native: any;

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

        (enabled ? it : it.skip)("allocates clear palette entries only for the cleared attachments", async function () {
            const engine = new NativeEngine();
            const scene = new Scene(engine);
            try {
                const integerTarget = new RenderTargetTexture("integerPaletteTarget", 4, scene, false, true,
                    Constants.TEXTURETYPE_UNSIGNED_INTEGER, false, Constants.TEXTURE_NEAREST_SAMPLINGMODE,
                    false, false, false, Constants.TEXTUREFORMAT_RGBA_INTEGER);
                const readback = createReadback(scene, `
                    precision highp usampler2D;
                    uniform usampler2D unsignedTexture;
                    void main() {
                        gl_FragColor = vec4(texelFetch(unsignedTexture, ivec2(0, 0), 0)) / 255.0;
                    }`, ["unsignedTexture"]);
                readback.material.setTexture("unsignedTexture", integerTarget);
                await readback.whenReadyAsync();

                // Nine distinct colors fit the 16-entry palette only if the unused normalized color isn't allocated too.
                engine.bindFramebuffer(integerTarget.renderTarget!);
                for (let i = 1; i <= 9; ++i) {
                    engine.clear(new Color4(i / 32, 0, 0, 1), true, false, false);
                }
                engine.unBindFramebuffer(integerTarget.renderTarget!);

                // trunc(255 * 9 / 32)
                expect(await readback.readPixel(0)).to.deep.equal([71, 0, 0, 255]);
            } finally {
                scene.dispose();
                engine.dispose();
            }
        });

        (enabled ? it : it.skip)("rounds clearAttachments values like WebGL and skips unselected attachments", async function () {
            const engine = new NativeEngine();
            const scene = new Scene(engine);
            try {
                // RGBA + INT maps to RGBA32I on Native.
                const target = new MultiRenderTarget("clearAttachmentsTarget", 4, 3, scene, {
                    types: [Constants.TEXTURETYPE_INT, Constants.TEXTURETYPE_UNSIGNED_INTEGER, Constants.TEXTURETYPE_UNSIGNED_BYTE],
                    formats: [Constants.TEXTUREFORMAT_RGBA, Constants.TEXTUREFORMAT_RGBA_INTEGER, Constants.TEXTUREFORMAT_RGBA],
                    samplingModes: [Constants.TEXTURE_NEAREST_SAMPLINGMODE, Constants.TEXTURE_NEAREST_SAMPLINGMODE, Constants.TEXTURE_NEAREST_SAMPLINGMODE],
                    generateMipMaps: false,
                    generateDepthBuffer: false,
                    doNotChangeAspectRatio: true,
                });
                const readback = createReadback(scene, `
                    precision highp isampler2D;
                    precision highp usampler2D;
                    uniform isampler2D signedTexture;
                    uniform usampler2D unsignedTexture;
                    uniform sampler2D normalizedTexture;
                    void main() {
                        int x = int(gl_FragCoord.x);
                        gl_FragColor = x == 0 ? vec4(texelFetch(signedTexture, ivec2(0, 0), 0)) / 255.0
                            : x == 1 ? vec4(texelFetch(unsignedTexture, ivec2(0, 0), 0)) / 255.0
                            : texelFetch(normalizedTexture, ivec2(0, 0), 0);
                    }`, ["signedTexture", "unsignedTexture", "normalizedTexture"]);
                readback.material.setTexture("signedTexture", target.textures[0]);
                readback.material.setTexture("unsignedTexture", target.textures[1]);
                readback.material.setTexture("normalizedTexture", target.textures[2]);
                await readback.whenReadyAsync();

                engine.bindFramebuffer(target.renderTarget!);
                engine.clear(new Color4(0.2, 0.4, 0.6, 0.8), true, false, false);
                // 0.699999988079071 and 0.4999999701976776 are exact float32 values that round differently in float than in double.
                engine.clearAttachments(new Color4(0.5, 0.25, 0.699999988079071, 0.4999999701976776), engine.buildTextureLayout([true, true, false]), true, false);
                engine.unBindMultiColorAttachmentFramebuffer(target.renderTarget!, true);

                // Signed: Math.round(c) = [1, 0, 1, 0]. clear() would give trunc(255 * c) = [127, 63, 178, 127].
                expect(await readback.readPixel(0)).to.deep.equal([1, 0, 1, 0]);
                // Unsigned: Math.round(255 * c) = [128, 64, 178, 127].
                expect(await readback.readPixel(1)).to.deep.equal([128, 64, 178, 127]);
                // The unselected RGBA8 attachment keeps the first clear.
                expect(await readback.readPixel(2)).to.deep.equal([51, 102, 153, 204]);
            } finally {
                scene.dispose();
                engine.dispose();
            }
        });
    });
}

// Draws a fullscreen quad with the given fragment shader into a 4x4 RGBA8 target and reads pixels from its first row.
function createReadback(scene: Scene, fragmentSource: string, samplers: string[]) {
    const camera = new FreeCamera("camera", new Vector3(0, 0, -1), scene);
    camera.setTarget(Vector3.Zero());
    scene.activeCamera = camera;

    const material = new ShaderMaterial("readback", scene, {
        vertexSource: `
            precision highp float;
            attribute vec3 position;
            void main() { gl_Position = vec4(position, 1.0); }`,
        fragmentSource: "precision highp float;\n" + fragmentSource,
    }, { attributes: ["position"], uniforms: [], samplers });

    const quad = new Mesh("fullscreen", scene);
    quad.setVerticesData(VertexBuffer.PositionKind, [-1, -1, 0, 1, -1, 0, -1, 1, 0, -1, 1, 0, 1, -1, 0, 1, 1, 0]);
    quad.isUnIndexed = true;
    quad.material = material;

    const target = new RenderTargetTexture("readback", 4, scene, false);
    target.renderList = [quad];

    let pixels: Uint8Array | undefined;
    return {
        material,
        whenReadyAsync: async () => {
            await material.forceCompilationAsync(quad);
            await scene.whenReadyAsync();
        },
        readPixel: async (x: number) => {
            if (!pixels) {
                target.render();
                const data = await target.readPixels();
                if (!(data instanceof Uint8Array)) {
                    throw new Error("Expected RGBA8 readback");
                }
                pixels = data;
            }
            return Array.from(pixels.subarray(x * 4, x * 4 + 4));
        },
    };
}
