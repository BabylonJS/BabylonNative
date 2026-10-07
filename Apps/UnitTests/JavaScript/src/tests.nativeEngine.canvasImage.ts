import * as Mocha from "mocha";
import { expect } from "chai";
import { Constants, DynamicTexture, NativeEngine, Scene } from "@babylonjs/core";

type CanvasContext = ReturnType<DynamicTexture["getContext"]>;

const opaqueOrangePng = "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAAQAAAAECAYAAACp8Z5+AAAAFUlEQVR4nGP838DwnwEJMCFziBMAAKTRAobDsKmdAAAAAElFTkSuQmCC";

function fillImageData(context: CanvasContext, width: number, height: number, pixel: (x: number, y: number) => readonly number[], dx = 0, dy = 0): void {
    const imageData = context.getImageData(0, 0, width, height);
    for (let y = 0; y < height; ++y) {
        for (let x = 0; x < width; ++x) {
            imageData.data.set(pixel(x, y), (y * width + x) * 4);
        }
    }
    context.putImageData(imageData, dx, dy);
}

async function readTexture(texture: DynamicTexture): Promise<Uint8Array> {
    texture.update(false);
    const pixels = await texture.readPixels();
    if (!(pixels instanceof Uint8Array)) {
        throw new Error("Expected RGBA8 GPU readback for the canvas texture");
    }
    // Native readback uses WebGL's bottom-up row order; assertions use Canvas coordinates.
    const { width, height } = texture.getSize();
    const stride = width * 4;
    const topDown = new Uint8Array(pixels.length);
    for (let y = 0; y < height; ++y) {
        topDown.set(pixels.subarray(y * stride, (y + 1) * stride), (height - 1 - y) * stride);
    }
    return topDown;
}

export function registerCanvasImageTests(
    describe: typeof Mocha.describe,
    it: typeof Mocha.it,
    skipCanvasGpuTests: boolean
): void {
    describe("NativeEngine Canvas pixel-backed images", function () {
        this.timeout(10000);
        const test = skipCanvasGpuTests ? it.skip : it;

        test("keeps putImageData pixels alive through the GPU flush", async function () {
            const engine = new NativeEngine();
            const scene = new Scene(engine);
            try {
                const texture = new DynamicTexture(
                    "putImageData lifetime",
                    { width: 10, height: 8 },
                    scene,
                    false,
                    Constants.TEXTURE_NEAREST_SAMPLINGMODE
                );
                fillImageData(texture.getContext(), 4, 3, () => [0, 255, 255, 255], 3, 2);

                const pixels = await readTexture(texture);
                const pixel = (x: number, y: number) => Array.from(pixels.subarray((y * 10 + x) * 4, (y * 10 + x + 1) * 4));
                expect(pixel(4, 3), "inside putImageData rectangle").to.deep.equal([0, 255, 255, 255]);
                expect(pixel(2, 3), "left of putImageData rectangle").to.deep.equal([0, 0, 0, 0]);
                expect(pixel(4, 5), "below putImageData rectangle").to.deep.equal([0, 0, 0, 0]);
            } finally {
                scene.dispose();
                engine.dispose();
            }
        });

        test("draws an offscreen canvas at a nonzero destination", async function () {
            const engine = new NativeEngine();
            const scene = new Scene(engine);
            try {
                const source = engine.createCanvas(4, 4);
                fillImageData(source.getContext("2d"), 4, 4, () => [255, 0, 255, 255]);

                const texture = new DynamicTexture(
                    "offscreen canvas copy",
                    { width: 12, height: 10 },
                    scene,
                    false,
                    Constants.TEXTURE_NEAREST_SAMPLINGMODE
                );
                texture.getContext().drawImage(source, 3, 2);

                const pixels = await readTexture(texture);
                const pixel = (x: number, y: number) => Array.from(pixels.subarray((y * 12 + x) * 4, (y * 12 + x + 1) * 4));
                expect(pixel(4, 3), "inside copied canvas").to.deep.equal([255, 0, 255, 255]);
                expect(pixel(2, 3), "left of copied canvas").to.deep.equal([0, 0, 0, 0]);
                expect(pixel(4, 6), "below copied canvas").to.deep.equal([0, 0, 0, 0]);
            } finally {
                scene.dispose();
                engine.dispose();
            }
        });

        test("draws an ImageBitmap source through the GPU path", async function () {
            const engine = new NativeEngine();
            const scene = new Scene(engine);
            try {
                const bitmap = await engine._createImageBitmapFromSource(opaqueOrangePng);
                expect(bitmap.width).to.equal(4);
                expect(bitmap.height).to.equal(4);

                const texture = new DynamicTexture(
                    "ImageBitmap copy",
                    { width: 10, height: 8 },
                    scene,
                    false,
                    Constants.TEXTURE_NEAREST_SAMPLINGMODE
                );
                texture.getContext().drawImage(bitmap, 3, 2);

                const pixels = await readTexture(texture);
                const pixel = (x: number, y: number) => Array.from(pixels.subarray((y * 10 + x) * 4, (y * 10 + x + 1) * 4));
                expect(pixel(4, 3), "inside copied ImageBitmap").to.deep.equal([255, 128, 0, 255]);
                expect(pixel(2, 3), "left of copied ImageBitmap").to.deep.equal([0, 0, 0, 0]);
            } finally {
                scene.dispose();
                engine.dispose();
            }
        });

        test("crops and scales a nonzero offscreen-canvas source rectangle", async function () {
            const engine = new NativeEngine();
            const scene = new Scene(engine);
            try {
                const source = engine.createCanvas(18, 8);
                fillImageData(source.getContext("2d"), 18, 8, (x) => (
                    x < 6 ? [255, 0, 0, 255] :
                        x < 12 ? [0, 255, 0, 255] :
                            [0, 0, 255, 255]
                ));

                const texture = new DynamicTexture(
                    "offscreen canvas crop",
                    { width: 14, height: 12 },
                    scene,
                    false,
                    Constants.TEXTURE_NEAREST_SAMPLINGMODE
                );
                texture.getContext().drawImage(source, 7, 2, 4, 4, 3, 2, 8, 8);

                const pixels = await readTexture(texture);
                const pixel = (x: number, y: number) => Array.from(pixels.subarray((y * 14 + x) * 4, (y * 14 + x + 1) * 4));
                expect(pixel(4, 5), "left interior of scaled crop").to.deep.equal([0, 255, 0, 255]);
                expect(pixel(9, 5), "right interior of scaled crop").to.deep.equal([0, 255, 0, 255]);
                expect(pixel(2, 5), "outside destination rectangle").to.deep.equal([0, 0, 0, 0]);
            } finally {
                scene.dispose();
                engine.dispose();
            }
        });
    });
}
