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
  // Alpha is a 0-1 number (or a percentage) per CSS Color, so any value above
  // 1 clamps to fully opaque. It is not a 0-255 channel like r/g/b.
  expect(_native.Canvas.parseColor("rgba(16,32,48,64)")).to.equal(0xff302010);
  expect(_native.Canvas.parseColor("rgb(16,     32   ,  48   )")).to.equal(
    0xff302010
  );
  expect(
    _native.Canvas.parseColor("rgba(    16,     32   ,  48 , 64  )")
  ).to.equal(0xff302010);
  expect(_native.Canvas.parseColor("rgba(16,32,48,1)")).to.equal(0xff302010);
  expect(_native.Canvas.parseColor("rgba(16,32,48,0)")).to.equal(0x00302010);
  // Fractional and percentage alpha, whitespace-separated components and the
  // "/ alpha" form all used to fall through to the "unable to parse" throw.
  expect(_native.Canvas.parseColor("rgba(16,32,48,0.5)")).to.equal(0x80302010);
  expect(_native.Canvas.parseColor("rgba(16 32 48 / 50%)")).to.equal(
    0x80302010
  );
  expect(_native.Canvas.parseColor("rgb(16 32 48)")).to.equal(0xff302010);
  expect(_native.Canvas.parseColor("rgb(100%,0%,0%)")).to.equal(0xff0000ff);
  expect(_native.Canvas.parseColor("hsl(0,100%,50%)")).to.equal(0xff0000ff);
  expect(_native.Canvas.parseColor("hsla(0,100%,50%,0.5)")).to.equal(
    0x800000ff
  );

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

describe("Canvas2D", function () {
  function createContext(): any {
    const canvas = new _native.Canvas();
    canvas.width = 64;
    canvas.height = 64;
    return canvas.getContext("2d");
  }

  it("round-trips a string fillStyle and strokeStyle", function () {
    const ctx = createContext();
    ctx.fillStyle = "#ff0000";
    ctx.strokeStyle = "#00ff00";
    expect(ctx.fillStyle).to.equal("#ff0000");
    expect(ctx.strokeStyle).to.equal("#00ff00");
  });

  it("accepts a CanvasGradient as fillStyle", function () {
    const ctx = createContext();
    const gradient = ctx.createLinearGradient(0, 0, 64, 64);
    gradient.addColorStop(0, "red");
    gradient.addColorStop(1, "blue");
    expect(function () {
      ctx.fillStyle = gradient;
    }).to.not.throw();
    expect(ctx.fillStyle).to.not.equal("#ff0000");
  });

  it("accepts a CanvasGradient as strokeStyle", function () {
    // strokeStyle used to be string-only and threw "A string was expected",
    // which broke every GUI control that strokes with a gradient (Line, Button border).
    const ctx = createContext();
    const gradient = ctx.createLinearGradient(0, 0, 64, 64);
    gradient.addColorStop(0, "red");
    gradient.addColorStop(1, "blue");
    expect(function () {
      ctx.strokeStyle = gradient;
    }).to.not.throw();
  });

  it("accepts a radial CanvasGradient defined by two independent circles", function () {
    const ctx = createContext();
    // Neither concentric nor r0 == 0: both circles have to be honored.
    const gradient = ctx.createRadialGradient(10, 10, 5, 40, 32, 30);
    gradient.addColorStop(0, "yellow");
    gradient.addColorStop(0.5, "pink");
    gradient.addColorStop(1, "green");
    expect(function () {
      ctx.fillStyle = gradient;
      ctx.strokeStyle = gradient;
    }).to.not.throw();
  });

  it("restores a gradient strokeStyle across save/restore", function () {
    const ctx = createContext();
    const gradient = ctx.createLinearGradient(0, 0, 64, 64);
    gradient.addColorStop(0, "red");
    ctx.strokeStyle = "#0000ff";
    ctx.save();
    ctx.strokeStyle = gradient;
    ctx.restore();
    expect(ctx.strokeStyle).to.equal("#0000ff");
  });

  // The three parsers below all used to reach std::stof/std::stoi with a value the regex
  // admits but the target type cannot hold. The resulting std::out_of_range is not a
  // Napi::Error, so it escaped the N-API callback and terminated the host process outright
  // rather than surfacing as a JS exception. Reaching the assertion at all is the test.
  it("survives an out-of-range rgb() component", function () {
    const ctx = createContext();
    const huge = "9".repeat(400);
    expect(function () {
      ctx.fillStyle = `rgb(${huge}, 0, 0)`;
    }).to.not.throw();
    expect(function () {
      ctx.fillStyle = `rgba(0, 0, 0, ${huge})`;
    }).to.not.throw();
  });

  it("survives an out-of-range font size and weight", function () {
    const ctx = createContext();
    ctx.font = "18px Arial";
    // The size regex accepts an exponent, so this parses but does not fit a float.
    expect(function () {
      ctx.font = "18e999px Arial";
    }).to.not.throw();
    expect(function () {
      ctx.font = `${"9".repeat(400)} 18px Arial`;
    }).to.not.throw();
    // An unparseable font is ignored, so the previous one stays in effect.
    expect(ctx.font).to.contain("18px");
  });

  it("survives an out-of-range letterSpacing", function () {
    const ctx = createContext();
    expect(function () {
      ctx.letterSpacing = `${"9".repeat(400)}px`;
    }).to.not.throw();
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

  // Encoded by the reference draco3dgltf 1.5.7 encoder (the same package Babylon.js takes
  // its decoder from), standard edgebreaker, 14-bit position quantization. Using a fixture
  // from the reference encoder rather than our own output pins this decoder to the upstream
  // bitstream instead of to itself.
  const ENCODED = hexToBytes(
    "445241434f02020101000000040200020000011fff011101ff00000100090300000201010100030301300110030024824a0400000000ff3f00000000000000000000000000000000803f0e");
  const POSITION_ATTRIBUTE_ID = 0;

  // A 63-vertex UV sphere carrying POSITION, NORMAL and TEX_COORD, encoded by the same
  // reference encoder with per-attribute quantization. The single-triangle-pair fixture
  // above cannot exercise multi-attribute decoding or a non-degenerate edgebreaker
  // traversal, which are what the glTF-bitstream-only build actually restricts.
  //
  // The expected counts below are what the reference draco3dgltf decoder reports for this
  // exact buffer, not the pre-encode mesh: Draco merges points whose attributes all match
  // and drops the degenerate triangles at the poles, so 63 vertices / 96 triangles going
  // in becomes 62 vertices / 91 triangles coming out.
  const SPHERE = hexToBytes(
    "445241434f020201010000003a5b025b05001a5fd73e55ad3e55d5aa3e5555adaa3ea55455559faaaaaa565501ff0111" +
    "ff02694af8058097a3755f03ff0000000000010100010009030000020101090300010301030902000202010101000f2b" +
    "a106b907592e51030c141534f4dfc29a78ddaf7f80bed2ffff2bad28fedf4fcb03010030a7577c44104633030047e86d" +
    "00c02304008f08128a7a003c181d05009108c20010d48301102848f888fa0fdc030090d02b010050db9591d895901000" +
    "749b07f388bca224060084f91f49f408cd0c0088ee9110006466480800ba0d840110a80703405010e61175c5500b00ba" +
    "12a805005dc92b23d12b9dc1b6fbb400e0113dc2480c8007eb8a9208c2b43d000009bd1200f8b4a016007425510b00bc" +
    "525702009fd62b010050eb950040a8ed4a0040d48a5a00d095422d00f04aa0160078a54f0b00ba92570200a2b62b0180" +
    "50eb0cb89da8ed3600e0110200490409030580074762004098a00000e1aef8881e8cb7010090b0ed018047f8081f1100" +
    "80b0001075ea0e00cc0c00000000ff3f0000000080bf000080bf000080bf000000400e000301000b036904135103f109" +
    "a106b9270e3ecb87a50ecead84ceeaa8bdde65000002dc7542d24fdbb29af35e882de5dab6c48d3dd1bbeceebaec45c6" +
    "73b0bb6b772244eb067057e873dc4e3d72c57b21b6539244f9b4fe44ef620ee3d4cf9108b00b40340076034034000000" +
    "44130dc01c19673cc72e4413bd8b398c673cc72e00d10010057042d294b60000fc0fd127b6b63cbd9acf91f1ffff9492" +
    "621f37ff030000ff0100000a010101000d039532550a1b0901090109010a0eb9fbab2e93abef3f8700fcff00aceaff00" +
    "60a0001a8220220800400800820822089a0000000000ff0f000000000000000000000000803f0c");
  const SPHERE_VERTICES = 62;
  const SPHERE_INDICES = 273;

  it("publishes the codec version it was built against", function () {
    expect(_native.DracoCodec.Version).to.be.a("string");
    expect(_native.DracoCodec.Version).to.match(/^\d+\.\d+\.\d+$/);
  });

  it("decodes a mesh produced by the reference glTF encoder", function () {
    const decoded = _native.DracoCodec.Decode(ENCODED, { position: POSITION_ATTRIBUTE_ID });

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

  it("decodes without an explicit attribute id map", function () {
    const decoded = _native.DracoCodec.Decode(ENCODED);
    expect(decoded.totalVertices).to.equal(positions.length / 3);
    expect(decoded.attributes.find((a: any) => a.kind === "position")).to.not.equal(undefined);
  });

  it("decodes a multi-attribute mesh", function () {
    const decoded = _native.DracoCodec.Decode(SPHERE, { position: 0, normal: 1, uv: 2 });

    expect(decoded.totalVertices).to.equal(SPHERE_VERTICES);
    expect(decoded.indices.length).to.equal(SPHERE_INDICES);

    const byKind: any = {};
    for (const a of decoded.attributes) {
      byKind[a.kind] = a;
    }

    expect(byKind.position.size).to.equal(3);
    expect(byKind.normal.size).to.equal(3);
    expect(byKind.uv.size).to.equal(2);

    expect(byKind.position.data.length).to.equal(SPHERE_VERTICES * 3);
    expect(byKind.normal.data.length).to.equal(SPHERE_VERTICES * 3);
    expect(byKind.uv.data.length).to.equal(SPHERE_VERTICES * 2);

    // Every index must address a real vertex, and the geometry must actually be the
    // unit sphere that was encoded rather than plausible-looking noise.
    for (let i = 0; i < decoded.indices.length; ++i) {
      expect(decoded.indices[i]).to.be.lessThan(SPHERE_VERTICES);
    }

    for (let v = 0; v < SPHERE_VERTICES; ++v) {
      const x = byKind.position.data[v * 3];
      const y = byKind.position.data[v * 3 + 1];
      const z = byKind.position.data[v * 3 + 2];
      expect(Math.sqrt(x * x + y * y + z * z)).to.be.closeTo(1, 0.01);
    }
  });

  it("rejects malformed input", function () {
    const garbage = new Uint8Array(64);
    for (let i = 0; i < garbage.length; ++i) {
      garbage[i] = (i * 37) & 0xff;
    }
    expect(() => _native.DracoCodec.Decode(garbage)).to.throw();
  });

  it("rejects truncated input", function () {
    const truncated = ENCODED.slice(0, Math.floor(ENCODED.length / 2));
    expect(() => _native.DracoCodec.Decode(truncated)).to.throw();
  });

  it("rejects an empty buffer", function () {
    expect(() => _native.DracoCodec.Decode(new Uint8Array(0))).to.throw();
  });

  it("does not expose an encoder", function () {
    expect(_native.DracoCodec.Encode).to.equal(undefined);
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
