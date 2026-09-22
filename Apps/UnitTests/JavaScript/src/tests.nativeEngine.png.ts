import * as Mocha from "mocha";
import { expect } from "chai";
import { Constants, NativeEngine, Scene, Texture } from "@babylonjs/core";

const fixtures: { name: string; png: string; pixels: number[]; width: number }[] = [
  {
    name: "16-bit RGBA", width: 4,
    png: "iVBORw0KGgoAAAANSUhEUgAAAAQAAAABEAYAAACprNOOAAAAI0lEQVR4nGOo/38v9Ene//8MDP//NzBAeAwMIB4D4/9/90IBNYURsvcM43UAAAAASUVORK5CYII=",
    pixels: [128, 222, 227, 255, 0, 255, 128, 128, 222, 227, 0, 0, 255, 0, 255, 222],
  },
  {
    name: "16-bit RGB", width: 4,
    png: "iVBORw0KGgoAAAANSUhEUgAAAAQAAAABEAIAAAAmzkTZAAAAG0lEQVR4nGOo/38v9EkeA8P//w0MMBYD4/9/AKWbDQOAUd17AAAAAElFTkSuQmCC",
    pixels: [128, 222, 227, 255, 0, 255, 128, 255, 222, 227, 0, 255, 255, 0, 255, 255],
  },
  {
    name: "16-bit grayscale", width: 4,
    png: "iVBORw0KGgoAAAANSUhEUgAAAAQAAAABEAAAAACMx4xSAAAAEUlEQVR4nGOo/38v9Ene//8AGa4GAvbhooAAAAAASUVORK5CYII=",
    pixels: [128, 128, 128, 255, 222, 222, 222, 255, 227, 227, 227, 255, 255, 255, 255, 255],
  },
  {
    name: "16-bit grayscale with alpha", width: 4,
    png: "iVBORw0KGgoAAAANSUhEUgAAAAQAAAABEAQAAAADpRsFAAAAF0lEQVR4nGOo/////73Q+v9P8hgYQCwAYP8KsdIXi8oAAAAASUVORK5CYII=",
    pixels: [128, 128, 128, 255, 222, 222, 222, 128, 227, 227, 227, 0, 255, 255, 255, 222],
  },
  {
    name: "8-bit opaque grayscale", width: 3,
    png: "iVBORw0KGgoAAAANSUhEUgAAAAMAAAABCAAAAAA+i0toAAAADElEQVR42mNgqP8PAAIBAX+LG2RhAAAAAElFTkSuQmCC",
    pixels: [0, 0, 0, 255, 127, 127, 127, 255, 255, 255, 255, 255],
  },
  {
    name: "8-bit grayscale with alpha", width: 3,
    png: "iVBORw0KGgoAAAANSUhEUgAAAAMAAAABCAQAAACx6dw/AAAAD0lEQVR4nGNgYKh3+P8fAAXAAr4pW6ZDAAAAAElFTkSuQmCC",
    pixels: [0, 0, 0, 0, 127, 127, 127, 64, 255, 255, 255, 255],
  },
];

export function registerPngTests(
  describe: typeof Mocha.describe,
  it: typeof Mocha.it,
  enabled: boolean
): void {
  describe("NativeEngine PNG decoding", function () {
    this.timeout(10000);
    const test = enabled ? it : it.skip;
    for (const fixture of fixtures) {
      for (const generateMips of [false, true]) {
        test(`uploads ${fixture.name} PNG as RGBA8 (mips ${generateMips})`, async function () {
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
            expect(Array.from(pixels)).to.deep.equal(fixture.pixels);
            expect(texture.getSize().width).to.equal(fixture.width);
            expect(texture.getSize().height).to.equal(1);
            if (generateMips) {
              const mipWidth = Math.max(1, fixture.width >> 1);
              const mip = await texture.readPixels(0, 1, null, true, false, 0, 0, mipWidth, 1);
              expect(mip instanceof Uint8Array).to.equal(true);
              expect(mip?.byteLength).to.equal(mipWidth * 4);
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
          expect(bitmap.width).to.equal(fixture.width);
          expect(bitmap.height).to.equal(1);
          const pixels = engine.resizeImageBitmap(bitmap, fixture.width, 1);
          expect(Array.from(pixels)).to.deep.equal(fixture.pixels);
        } finally {
          engine.dispose();
        }
      });
    }
  });
}
