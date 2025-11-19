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
