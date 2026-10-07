import * as Mocha from "mocha";
import { expect } from "chai";
import { Constants, NativeEngine, Scene, Texture } from "@babylonjs/core";

const fixtures: { name: string; png: string; pixels: number[]; width?: number; height?: number; tolerance?: number }[] = [
    {
        name: "16-bit RGBA",
        tolerance: 1,
        png: "iVBORw0KGgoAAAANSUhEUgAAAAQAAAABEAYAAACprNOOAAAAI0lEQVR4nGOo/38v9Ene//8MDP//NzBAeAwMIB4D4/9/90IBNYURsvcM43UAAAAASUVORK5CYII=",
        pixels: [128, 222, 227, 255, 0, 255, 128, 128, 222, 227, 0, 0, 255, 0, 255, 222],
    },
    {
        name: "16-bit RGB",
        tolerance: 1,
        png: "iVBORw0KGgoAAAANSUhEUgAAAAQAAAABEAIAAAAmzkTZAAAAG0lEQVR4nGOo/38v9EkeA8P//w0MMBYD4/9/AKWbDQOAUd17AAAAAElFTkSuQmCC",
        pixels: [128, 222, 227, 255, 0, 255, 128, 255, 222, 227, 0, 255, 255, 0, 255, 255],
    },
    {
        name: "16-bit grayscale",
        tolerance: 1,
        png: "iVBORw0KGgoAAAANSUhEUgAAAAQAAAABEAAAAACMx4xSAAAAEUlEQVR4nGOo/38v9Ene//8AGa4GAvbhooAAAAAASUVORK5CYII=",
        pixels: [128, 128, 128, 255, 222, 222, 222, 255, 227, 227, 227, 255, 255, 255, 255, 255],
    },
    {
        name: "16-bit grayscale with alpha",
        tolerance: 1,
        png: "iVBORw0KGgoAAAANSUhEUgAAAAQAAAABEAQAAAADpRsFAAAAF0lEQVR4nGOo/////73Q+v9P8hgYQCwAYP8KsdIXi8oAAAAASUVORK5CYII=",
        pixels: [128, 128, 128, 255, 222, 222, 222, 128, 227, 227, 227, 0, 255, 255, 255, 222],
    },
    {
        name: "8-bit RGB color key",
        png: "iVBORw0KGgoAAAANSUhEUgAAAAMAAAABCAIAAACUgoPjAAAABnRSTlMAAAAAAABupgeRAAAAD0lEQVR42mNgAAFGLhE5AAByAD7AYQaiAAAAAElFTkSuQmCC",
        width: 3,
        pixels: [0, 0, 0, 0, 0, 0, 1, 255, 10, 20, 30, 255],
    },
    {
        name: "8-bit grayscale color key",
        png: "iVBORw0KGgoAAAANSUhEUgAAAAMAAAABCAAAAAA+i0toAAAAAnRSTlMAf7YpoZUAAAAMSURBVHjaY2Co/w8AAgEBf4sbZGEAAAAASUVORK5CYII=",
        width: 3,
        pixels: [0, 0, 0, 255, 127, 127, 127, 0, 255, 255, 255, 255],
    },
    {
        name: "2-bit grayscale color key",
        png: "iVBORw0KGgoAAAANSUhEUgAAAAUAAAACAgAAAAD/sVEgAAAAAnRSTlMAApidrBQAAAAOSURBVHjaY5B2YJjcAAADMwFvhMae/wAAAABJRU5ErkJggg==",
        width: 5,
        height: 2,
        pixels: [
            0, 0, 0, 255, 85, 85, 85, 255, 170, 170, 170, 0, 255, 255, 255, 255, 85, 85, 85, 255,
            170, 170, 170, 0, 85, 85, 85, 255, 0, 0, 0, 255, 255, 255, 255, 255, 170, 170, 170, 0,
        ],
    },
    {
        name: "16-bit grayscale color key",
        png: "iVBORw0KGgoAAAANSUhEUgAAAAMAAAABEAAAAABuG5crAAAAAnRSTlMSNC/TSV4AAAAPSURBVHjaYxAyETJdfRYABIECBqQr4XkAAAAASUVORK5CYII=",
        width: 3,
        pixels: [18, 18, 18, 0, 18, 18, 18, 255, 171, 171, 171, 255],
    },
    {
        name: "16-bit RGB color key",
        png: "iVBORw0KGgoAAAANSUhEUgAAAAMAAAABEAIAAADEEl+gAAAABnRSTlMSNFZ4mryJ5E7mAAAAHElEQVR42mMQMgmrmLVHyDSsctbe1WdVMv7dAQBEhQi2aXh0TAAAAABJRU5ErkJggg==",
        width: 3,
        pixels: [18, 86, 154, 0, 18, 86, 154, 255, 171, 36, 254, 255],
    },
    {
        name: "8-bit opaque grayscale",
        png: "iVBORw0KGgoAAAANSUhEUgAAAAMAAAABCAAAAAA+i0toAAAADElEQVR42mNgqP8PAAIBAX+LG2RhAAAAAElFTkSuQmCC",
        width: 3,
        pixels: [0, 0, 0, 255, 127, 127, 127, 255, 255, 255, 255, 255],
    },
    {
        name: "8-bit grayscale with alpha",
        png: "iVBORw0KGgoAAAANSUhEUgAAAAMAAAABCAQAAACx6dw/AAAAD0lEQVR4nGNgYKh3+P8fAAXAAr4pW6ZDAAAAAElFTkSuQmCC",
        width: 3,
        pixels: [0, 0, 0, 0, 127, 127, 127, 64, 255, 255, 255, 255],
    },
];

function expectPixels(pixels: Uint8Array, fixture: typeof fixtures[number]): void {
    expect(pixels.length).to.equal(fixture.pixels.length);
    // WIC and bundled decoders quantize 16-bit channels to 8-bit with different rounding.
    for (let index = 0; index < pixels.length; ++index) {
        const expected = fixture.pixels[index];
        const tolerance = expected === 0 || expected === 255 ? 0 : fixture.tolerance ?? 0;
        expect(pixels[index], `channel ${index}`).to.be.closeTo(expected, tolerance);
    }
}

export function registerPngTests(
    describe: typeof Mocha.describe,
    it: typeof Mocha.it,
    enabled: boolean
): void {
    describe("NativeEngine PNG decoding", function () {
        this.timeout(10000);
        const test = enabled ? it : it.skip;
        for (const fixture of fixtures) {
            const width = fixture.width ?? 4;
            const height = fixture.height ?? 1;
            for (const generateMips of [false, true]) {
                test(`uploads ${fixture.name} PNG as browser RGBA8 (mips ${generateMips})`, async function () {
                    const engine = new NativeEngine();
                    const scene = new Scene(engine);
                    try {
                        const texture = await new Promise<Texture>((resolve, reject) => {
                            const image = new Texture(
                                "data:image/png;base64," + fixture.png, scene, !generateMips, false,
                                Constants.TEXTURE_NEAREST_SAMPLINGMODE, () => resolve(image),
                                message => reject(new Error(message || "PNG texture load failed"))
                            );
                        });
                        const pixels = await texture.readPixels();
                        if (!(pixels instanceof Uint8Array)) {
                            throw new Error("Expected unsigned-byte PNG texture readback");
                        }
                        expectPixels(pixels, fixture);
                        expect(texture.getSize().width).to.equal(width);
                        expect(texture.getSize().height).to.equal(height);
                        if (generateMips) {
                            const mipWidth = Math.max(1, width >> 1);
                            const mipHeight = Math.max(1, height >> 1);
                            // Read only the valid mip extent for odd-sized PNG fixtures.
                            const mip = await texture.readPixels(0, 1, null, true, false, 0, 0, mipWidth, mipHeight);
                            expect(mip instanceof Uint8Array).to.equal(true);
                            expect(mip?.byteLength).to.equal(mipWidth * mipHeight * 4);
                        }
                    } finally {
                        scene.dispose();
                        engine.dispose();
                    }
                });
            }
            test(`decodes ${fixture.name} PNG through Canvas images`, async function () {
                const engine = new NativeEngine();
                try {
                    const bitmap = await engine._createImageBitmapFromSource("data:image/png;base64," + fixture.png);
                    expect(bitmap.width).to.equal(width);
                    expect(bitmap.height).to.equal(height);
                    const pixels = engine.resizeImageBitmap(bitmap, width, height);
                    expectPixels(pixels, fixture);
                } finally {
                    engine.dispose();
                }
            });
        }
    });
}
