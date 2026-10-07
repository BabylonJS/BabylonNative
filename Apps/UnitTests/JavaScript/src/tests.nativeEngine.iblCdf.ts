import * as Mocha from "mocha";
import { expect } from "chai";
import {
    Constants,
    FreeCamera,
    MeshBuilder,
    NativeEngine,
    RawTexture,
    RenderTargetTexture,
    Scene,
    ShaderMaterial,
    Vector3,
} from "@babylonjs/core";

export function registerIblCdfTests(
    describe: typeof Mocha.describe,
    it: typeof Mocha.it,
    skipCanvasGpuTests: boolean
): void {
    describe("NativeEngine IBL CDF texel bins", function () {
        this.timeout(10000);
        (skipCanvasGpuTests ? it.skip : it)("preserves boundary ties, adjacent bins, and clamped endpoints", async function () {
            const engine = new NativeEngine();
            const scene = new Scene(engine);
            try {
                const camera = new FreeCamera("camera", new Vector3(0, 0, -3), scene);
                camera.setTarget(Vector3.Zero());
                const data = new Uint8Array(4 * 4 * 4);
                for (let y = 0; y < 4; y++) {
                    for (let x = 0; x < 4; x++) {
                        data.set([x * 64, y * 64, 0, 255], (y * 4 + x) * 4);
                    }
                }
                const texture = RawTexture.CreateRGBATexture(data, 4, 4, scene, false, false, Constants.TEXTURE_NEAREST_SAMPLINGMODE);
                const material = new ShaderMaterial("cdfBins", scene, {
                    vertexSource: `
                        precision highp float;
                        attribute vec3 position;
                        void main() { gl_Position = vec4(position.xy, 0.0, 1.0); }`,
                    fragmentSource: `
                        precision highp float;
                        uniform sampler2D icdf;
                        #include<iblCdfFunctions>
                        void main() {
                            float index = floor(gl_FragCoord.x);
                            float coordinate = index / 4.0;
                            if (index == 5.0) coordinate = -1.0;
                            if (index == 6.0) coordinate = 1.5;
                            if (index == 7.0) coordinate = 0.5 - 1.0 / 1024.0;
                            if (index == 8.0) coordinate = 0.5 + 1.0 / 1024.0;
                            gl_FragColor = sampleIcdf(icdf, vec2(coordinate));
                        }`,
                }, { attributes: ["position"], uniforms: [], samplers: ["icdf"] });
                material.backFaceCulling = false;
                material.setTexture("icdf", texture);
                const plane = MeshBuilder.CreatePlane("plane", { size: 2 }, scene);
                plane.material = material;
                const target = new RenderTargetTexture("cdfBins", { width: 9, height: 1 }, scene, {
                    generateDepthBuffer: false,
                    samples: 1,
                });
                target.renderList = [plane];
                await scene.whenReadyAsync();
                target.render();
                const pixels = await target.readPixels();
                if (!(pixels instanceof Uint8Array)) {
                    throw new Error("Expected RGBA8 CDF bin readback");
                }
                const expectedBins = [0, 1, 2, 3, 3, 0, 3, 1, 2];
                for (let i = 0; i < expectedBins.length; i++) {
                    const value = expectedBins[i] * 64;
                    expect(Array.from(pixels.subarray(i * 4, i * 4 + 4)), `CDF query ${i}`).to.deep.equal([value, value, 0, 255]);
                }
            } finally {
                scene.dispose();
                engine.dispose();
            }
        });
    });
}
