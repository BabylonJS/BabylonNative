import * as Mocha from "mocha";
import { expect } from "chai";
import {
  RequestFile,
  NativeEngine,
  MeshBuilder,
  DefaultRenderingPipeline,
  RefractionPostProcess,
  ImageProcessingPostProcess,
  HighlightsPostProcess,
  PassPostProcess,
  TonemapPostProcess,
  TonemappingOperator,
  Color3,
  ConvolutionPostProcess,
  BlackAndWhitePostProcess,
  ShaderMaterial,
  Scene,
  Vector2,
  BlurPostProcess
} from "@babylonjs/core";
import { GradientMaterial } from "@babylonjs/materials";

declare var describe: typeof Mocha.describe;
declare var it: typeof Mocha.it;

Mocha.setup("bdd");
// @ts-ignore
Mocha.reporter("spec");

declare const hostPlatform: string;
declare const setExitCode: (code: number) => void;
declare const _native: any;

describe("RequestFile", function () {
  this.timeout(0);
  it("should throw when requesting a URL with no protocol", function () {
    function requestFile() {
      RequestFile("noprotocol.gltf", () => {});
    }
    expect(requestFile).to.throw();
  });
});

describe("ColorParsing", function () {
  expect(_native.Canvas.parseColor("")).to.equal(0);
  expect(_native.Canvas.parseColor("transparent")).to.equal(0);
  expect(_native.Canvas.parseColor("#123")).to.equal(0xff332211);
  expect(_native.Canvas.parseColor("#1234")).to.equal(0x44332211);
  expect(_native.Canvas.parseColor("#123456")).to.equal(0xff563412);
  expect(_native.Canvas.parseColor("#12345678")).to.equal(0x78563412);
  expect(_native.Canvas.parseColor("snow")).to.equal(0xfffafaff);
  expect(_native.Canvas.parseColor("rgb(16,32,48)")).to.equal(0xff302010);
  expect(_native.Canvas.parseColor("rgba(16,32,48,64)")).to.equal(0x40302010);
  expect(_native.Canvas.parseColor("rgb(16,     32   ,  48   )")).to.equal(
    0xff302010
  );
  expect(
    _native.Canvas.parseColor("rgba(    16,     32   ,  48 , 64  )")
  ).to.equal(0x40302010);

  it("should throw", function () {
    function incorrectColor() {
      _native.Canvas.parseColor("unknownColor");
    }
    expect(incorrectColor).to.throw();
  });

  it("should throw", function () {
    function incorrectColor() {
      _native.Canvas.parseColor("#");
    }
    expect(incorrectColor).to.throw();
  });

  it("should throw", function () {
    function incorrectColor() {
      _native.Canvas.parseColor("#12345");
    }
    expect(incorrectColor).to.throw();
  });

  it("should throw", function () {
    function incorrectColor() {
      _native.Canvas.parseColor("rgb(11)");
    }
    expect(incorrectColor).to.throw();
  });

  it("should throw", function () {
    function incorrectColor() {
      _native.Canvas.parseColor("rgb(11,22,33");
    }
    expect(incorrectColor).to.throw();
  });

  it("should throw", function () {
    function incorrectColor() {
      _native.Canvas.parseColor("rgb(11,22,33,");
    }
    expect(incorrectColor).to.throw();
  });

  it("should throw", function () {
    function incorrectColor() {
      _native.Canvas.parseColor("rgba(11,   22, 33,  )");
    }
    expect(incorrectColor).to.throw();
  });

  it("should throw", function () {
    function incorrectColor() {
      _native.Canvas.parseColor("rgba(11,   22, 33, 44,   55,   66 )");
    }
    expect(incorrectColor).to.throw();
  });

  it("should throw", function () {
    function incorrectColor() {
      _native.Canvas.parseColor("rgb");
    }
    expect(incorrectColor).to.throw();
  });
  it("should throw", function () {
    function incorrectColor() {
      _native.Canvas.parseColor("rgba");
    }
    expect(incorrectColor).to.throw();
  });
});

function createSceneAndWait(callback: (engine: NativeEngine, scene: Scene) => void, done: () => void) {
  const engine = new NativeEngine();
  const scene = new Scene(engine);
  scene.createDefaultCamera();
  callback(engine, scene);
  scene.executeWhenReady(() => {
    done();
  });
}

describe("Materials", function () {
  this.timeout(0);
  it("Empty ShaderMaterial should compile", function (done) {
    function createEmptyShaderMat() {
      createSceneAndWait((engine, scene) => {
        const sphere = MeshBuilder.CreateSphere(
          "sphere",
          { diameter: 2, segments: 32 },
          scene
        );
        const shaders = {
          vertexSource: `void main() {}`,
          fragmentSource: `void main() {}`,
        };
        const mat = new ShaderMaterial("shader", scene, shaders, {});
        sphere.material = mat;
      }, done);
    }
    createEmptyShaderMat();
  });
  it("GradientMaterial should compile", function (done) {
    createSceneAndWait((engine, scene) => {
      const sphere = MeshBuilder.CreateSphere(
        "sphere",
        { diameter: 2, segments: 32 },
        scene
      );
      const gradientMaterial = new GradientMaterial("grad", scene);
      sphere.material = gradientMaterial;
    }, done);
  });
});

describe("PostProcesses", function () {
  this.timeout(0);
  it("PassPostProcess", function (done) {
    createSceneAndWait((engine, scene) => {
      const camera = scene._activeCamera;
      new PassPostProcess("Scene copy", 1.0, camera);
    }, done);
  });
  it("BlackAndWhitePostProcess", function (done) {
    createSceneAndWait((engine, scene) => {
      const camera = scene._activeCamera;
      new BlackAndWhitePostProcess("bandw", 1.0, camera);
    }, done);
  });
  it("BlurPostProcess", function (done) {
    createSceneAndWait((engine, scene) => {
      const camera = scene._activeCamera;
      new BlurPostProcess(
        "Horizontal blur",
        new Vector2(1.0, 0),
        32,
        0.25,
        camera
      );
    }, done);
  });
  it("ConvolutionPostProcess", function (done) {
    createSceneAndWait((engine, scene) => {
      const camera = scene._activeCamera;
      new ConvolutionPostProcess(
        "Sepia",
        ConvolutionPostProcess.EmbossKernel,
        1.0,
        camera
      );
    }, done);
  });
  it("HighlightsPostProcess", function (done) {
    createSceneAndWait((engine, scene) => {
      const camera = scene._activeCamera;
      new HighlightsPostProcess("highlights", 1.0, camera);
    }, done);
  });
  it("TonemapPostProcess", function (done) {
    createSceneAndWait((engine, scene) => {
      const camera = scene._activeCamera;
      new TonemapPostProcess("tonemap", TonemappingOperator.Hable, 1.0, camera);
    }, done);
  });
  it("ImageProcessingPostProcess", function (done) {
    createSceneAndWait((engine, scene) => {
      const camera = scene._activeCamera;
      new ImageProcessingPostProcess("processing", 1.0, camera);
    }, done);
  });
  it("RefractionPostProcess", function (done) {
    createSceneAndWait((engine, scene) => {
      const camera = scene._activeCamera;
      new RefractionPostProcess(
        "Refraction",
        "https://playground.babylonjs.com/textures/grass.jpg",
        new Color3(1.0, 1.0, 1.0),
        0.5,
        0.5,
        1.0,
        camera
      );
    }, done);
  });
  it("DefaultPipeline", function (done) {
    createSceneAndWait((engine, scene) => {
      const camera = scene._activeCamera!;
      new DefaultRenderingPipeline(
        "defaultPipeline", // The name of the pipeline
        true, // Do you want the pipeline to use HDR texture?
        scene, // The scene instance
        [camera] // The list of cameras to be attached to
      );
    }, done);
  });
  /*
    it("FxaaPostProcess", function(done) {
        createSceneAndWait((engine, scene) => {
            const camera = scene._activeCamera;
            new FxaaPostProcess("fxaa", 1.0, camera);
        }, done);
    });
    it("ColorCorrectionPostProcess", function(done) {
        createSceneAndWait((engine, scene) => {
            const camera = scene._activeCamera;
            new ColorCorrectionPostProcess("color_correction", "https://doc.babylonjs.com/_next/image?url=%2Fimg%2Fhow_to%2Fpost-processes%2Flut-inverted.png&w=256&q=16", 1.0, camera);
        }, done);
    });
    it("LensRenderingPipeline", function(done) {
        createSceneAndWait((engine, scene) => {
            const camera = scene._activeCamera;
            const parameters = {
                edge_blur: 1.0,
                chromatic_aberration: 1.0,
                distortion: 1.0,
            };              
            new LensRenderingPipeline('lensEffects', parameters, scene, 1.0, camera);
        }, done);
    });
    it("MotionBlurPostProcess", function(done) {
        createSceneAndWait((engine, scene) => {
            const camera = scene._activeCamera;
            new MotionBlurPostProcess(
                "mb", // The name of the effect.
                scene, // The scene containing the objects to blur according to their velocity.
                1.0, // The required width/height ratio to downsize to before computing the render pass.
                camera // The camera to apply the render pass to.
            );
        }, done);
    });
    it("ScreenSpaceReflectionPostProcess", function(done) {
        createSceneAndWait((engine, scene) => {
            const camera = scene._activeCamera;
            new ScreenSpaceReflectionPostProcess("ssr", scene, 1.0, camera);
        }, done);
    });*/
});

describe("NativeEncoding", function () {
  this.timeout(0);

  async function expectValidPNG(blob: Blob) {
    expect(blob).to.be.instanceOf(Blob);
    const arrayBuffer = await blob.arrayBuffer();
    expect(arrayBuffer.byteLength).to.be.greaterThan(0);

    const pngSignature = new Uint8Array(arrayBuffer.slice(0, 4));
    expect(pngSignature[0]).to.equal(137); // PNG signature bytes
    expect(pngSignature[1]).to.equal(80);  // 'P'
    expect(pngSignature[2]).to.equal(78);  // 'N'
    expect(pngSignature[3]).to.equal(71);  // 'G'
  }

  it("should encode a PNG", async function () {
    const pixelData = new Uint8Array(4).fill(255);
    const result = await _native.EncodeImageAsync(pixelData, 1, 1, "image/png", false);
    await expectValidPNG(result);
  });

  it("should handle multiple concurrent encoding tasks", async function () {
    const pixelDatas = [];
    for (let i = 0; i < 10; i++) {
      pixelDatas.push(new Uint8Array(4).fill(255));
    }
    const results = await Promise.all(pixelDatas.map((pixelData) =>
      _native.EncodeImageAsync(pixelData, 1, 1, "image/png", false)
    ));
    await Promise.all(results.map(b => expectValidPNG(b)));
  });
});

function hexToBytes(hex: string): Uint8Array {
  const out = new Uint8Array(hex.length / 2);
  for (let i = 0; i < out.length; ++i) {
    out[i] = parseInt(hex.substr(i * 2, 2), 16);
  }
  return out;
}

// Both plugins default to OFF, so report them as skipped rather than silently passing
// when the build did not opt in. CI enables both for the jobs that run UnitTests.
(typeof _native.DracoCodec !== "undefined" ? describe : describe.skip)("NativeDraco", function () {
  this.timeout(0);

  // Two triangles sharing an edge. Values are exact halves so they survive the float32
  // round trip bit-for-bit once quantization is disabled.
  const positions = new Float32Array([
    0, 0, 0,
    1, 0, 0,
    0, 1, 0,
    1, 1, 0,
  ]);
  const indices = new Uint16Array([0, 1, 2, 1, 3, 2]);

  function encodeFixture() {
    return _native.DracoCodec.Encode(
      [{ kind: "position", dracoName: "POSITION", data: positions, size: 3 }],
      indices);
  }

  it("publishes the codec version it was built against", function () {
    expect(_native.DracoCodec.Version).to.be.a("string");
    expect(_native.DracoCodec.Version).to.match(/^\d+\.\d+\.\d+$/);
  });

  it("round trips a mesh through encode and decode", function () {
    const encoded = encodeFixture();
    expect(encoded.data).to.be.instanceOf(Uint8Array);
    expect(encoded.data.length).to.be.greaterThan(0);
    expect(encoded.attributeIds).to.have.property("position");

    const decoded = _native.DracoCodec.Decode(encoded.data, encoded.attributeIds);

    expect(decoded.totalVertices).to.equal(positions.length / 3);
    expect(decoded.indices.length).to.equal(indices.length);

    // Draco reorders points, so compare the triangles as sets of resolved corner
    // positions rather than assuming the original vertex order survived. Rounded to
    // two decimals so the comparison tolerates quantization but still separates
    // coordinates that are a whole unit apart.
    const attribute = decoded.attributes.find((a: any) => a.kind === "position");
    expect(attribute, "decoded position attribute").to.not.equal(undefined);
    expect(attribute.size).to.equal(3);

    const corner = (buffer: any, i: number) =>
      [buffer[i * 3], buffer[i * 3 + 1], buffer[i * 3 + 2]]
        .map((v: number) => v.toFixed(2))
        .join(",");

    const expectedCorners: string[] = [];
    const actualCorners: string[] = [];
    for (let i = 0; i < indices.length; ++i) {
      expectedCorners.push(corner(positions, indices[i]));
      actualCorners.push(corner(attribute.data, decoded.indices[i]));
    }
    expect(actualCorners.sort()).to.deep.equal(expectedCorners.sort());
  });

  it("rejects malformed input", function () {
    const garbage = new Uint8Array(64);
    for (let i = 0; i < garbage.length; ++i) {
      garbage[i] = (i * 37) & 0xff;
    }
    expect(() => _native.DracoCodec.Decode(garbage)).to.throw();
  });

  it("rejects truncated input", function () {
    const encoded = encodeFixture();
    const truncated = encoded.data.slice(0, Math.floor(encoded.data.length / 2));
    expect(() => _native.DracoCodec.Decode(truncated)).to.throw();
  });

  it("rejects an empty buffer", function () {
    expect(() => _native.DracoCodec.Decode(new Uint8Array(0))).to.throw();
  });

  it("rejects encoding without a position attribute", function () {
    expect(() => _native.DracoCodec.Encode(
      [{ kind: "normal", dracoName: "NORMAL", data: positions, size: 3 }],
      indices)).to.throw();
  });

  it("rejects an index that is out of range for the vertex count", function () {
    const bad = new Uint16Array([0, 1, 99, 1, 3, 2]);
    expect(() => _native.DracoCodec.Encode(
      [{ kind: "position", dracoName: "POSITION", data: positions, size: 3 }],
      bad)).to.throw();
  });

  it("rejects a non-positive attribute size", function () {
    expect(() => _native.DracoCodec.Encode(
      [{ kind: "position", dracoName: "POSITION", data: positions, size: 0 }],
      indices)).to.throw();
  });

  it("rejects attribute data that is not a multiple of its component count", function () {
    expect(() => _native.DracoCodec.Encode(
      [{ kind: "position", dracoName: "POSITION", data: new Float32Array(7), size: 3 }],
      indices)).to.throw();
  });
});

(typeof _native.MeshoptCodec !== "undefined" ? describe : describe.skip)("NativeMeshopt", function () {
  this.timeout(0);

  // Produced by the reference meshoptimizer 0.22 JavaScript encoder
  // (MeshoptEncoder.encodeVertexBuffer) over 6 vertices of 16-byte stride, so this
  // pins our native decoder against the upstream bitstream rather than against itself.
  const ENCODED = hexToBytes(
    "a00000013ff000007fffa0606001380000007e0000013ff0000020ff9070480130800000800000013ff0000080ff" +
    "a0606001320000007e012aa000000000000000000000000000000000000000000000000000000000800000000000beadde");
  const EXPECTED = hexToBytes(
    "00000000000000800000000000beadde0000c03f000010c00000403f01beadde00004040000090c00000c03f02beadde" +
    "000090400000d8c00000104003beadde0000c040000010c10000404004beadde0000f040000034c10000704005beadde");
  const COUNT = 6;
  const STRIDE = 16;

  it("publishes the codec version it was built against", function () {
    expect(_native.MeshoptCodec.Version).to.be.a("string");
    expect(_native.MeshoptCodec.Version).to.match(/^\d+\.\d+$/);
  });

  it("decodes a reference stream byte for byte", function () {
    const decoded = _native.MeshoptCodec.Decode(ENCODED, COUNT, STRIDE, "ATTRIBUTES");
    expect(decoded.length).to.equal(EXPECTED.length);
    expect(Array.from(decoded)).to.deep.equal(Array.from(EXPECTED));
  });

  it("rejects malformed input", function () {
    const garbage = new Uint8Array(ENCODED.length);
    for (let i = 0; i < garbage.length; ++i) {
      garbage[i] = (i * 37) & 0xff;
    }
    expect(() => _native.MeshoptCodec.Decode(garbage, COUNT, STRIDE, "ATTRIBUTES")).to.throw();
  });

  it("rejects truncated input", function () {
    const truncated = ENCODED.slice(0, Math.floor(ENCODED.length / 2));
    expect(() => _native.MeshoptCodec.Decode(truncated, COUNT, STRIDE, "ATTRIBUTES")).to.throw();
  });

  it("rejects an unknown mode", function () {
    expect(() => _native.MeshoptCodec.Decode(ENCODED, COUNT, STRIDE, "NOT_A_MODE")).to.throw();
  });

  it("rejects a stride outside [1, 256]", function () {
    expect(() => _native.MeshoptCodec.Decode(ENCODED, COUNT, 0, "ATTRIBUTES")).to.throw();
    expect(() => _native.MeshoptCodec.Decode(ENCODED, COUNT, 257, "ATTRIBUTES")).to.throw();
  });

  it("rejects an ATTRIBUTES stride that is not a multiple of 4", function () {
    expect(() => _native.MeshoptCodec.Decode(ENCODED, COUNT, 6, "ATTRIBUTES")).to.throw();
  });

  it("rejects a negative count", function () {
    expect(() => _native.MeshoptCodec.Decode(ENCODED, -1, STRIDE, "ATTRIBUTES")).to.throw();
  });

  it("rejects a TRIANGLES count that is not a multiple of 3", function () {
    expect(() => _native.MeshoptCodec.Decode(ENCODED, 4, 2, "TRIANGLES")).to.throw();
  });

  it("rejects a non-typed-array source", function () {
    expect(() => _native.MeshoptCodec.Decode(null, COUNT, STRIDE, "ATTRIBUTES")).to.throw();
  });
});

mocha.run((failures) => {
  // Test program will wait for code to be set before exiting
  if (failures > 0) {
    // Failure
    setExitCode(1);
  } else {
    // Success
    setExitCode(0);
  }
});
