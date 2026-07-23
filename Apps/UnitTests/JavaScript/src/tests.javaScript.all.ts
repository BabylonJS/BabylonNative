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

describe("WebAudio", function () {
  this.timeout(0);

  function createSilentWavArrayBuffer(sampleRate: number, frameCount: number): ArrayBuffer {
    const bytesPerSample = 2;
    const channelCount = 1;
    const dataSize = frameCount * channelCount * bytesPerSample;
    const buffer = new ArrayBuffer(44 + dataSize);
    const view = new DataView(buffer);
    const writeString = (offset: number, value: string) => {
      for (let i = 0; i < value.length; i++) {
        view.setUint8(offset + i, value.charCodeAt(i));
      }
    };

    writeString(0, "RIFF");
    view.setUint32(4, 36 + dataSize, true);
    writeString(8, "WAVE");
    writeString(12, "fmt ");
    view.setUint32(16, 16, true);
    view.setUint16(20, 1, true);
    view.setUint16(22, channelCount, true);
    view.setUint32(24, sampleRate, true);
    view.setUint32(28, sampleRate * channelCount * bytesPerSample, true);
    view.setUint16(32, channelCount * bytesPerSample, true);
    view.setUint16(34, 8 * bytesPerSample, true);
    writeString(36, "data");
    view.setUint32(40, dataSize, true);
    return buffer;
  }

  function createSineWavArrayBuffer(sampleRate: number, frameCount: number, frequency: number, amplitude: number = 0.5): ArrayBuffer {
    const bytesPerSample = 2;
    const channelCount = 1;
    const dataSize = frameCount * channelCount * bytesPerSample;
    const buffer = new ArrayBuffer(44 + dataSize);
    const view = new DataView(buffer);
    const writeString = (offset: number, value: string) => {
      for (let i = 0; i < value.length; i++) {
        view.setUint8(offset + i, value.charCodeAt(i));
      }
    };

    writeString(0, "RIFF");
    view.setUint32(4, 36 + dataSize, true);
    writeString(8, "WAVE");
    writeString(12, "fmt ");
    view.setUint32(16, 16, true);
    view.setUint16(20, 1, true);
    view.setUint16(22, channelCount, true);
    view.setUint32(24, sampleRate, true);
    view.setUint32(28, sampleRate * channelCount * bytesPerSample, true);
    view.setUint16(32, channelCount * bytesPerSample, true);
    view.setUint16(34, 8 * bytesPerSample, true);
    writeString(36, "data");
    view.setUint32(40, dataSize, true);

    for (let frame = 0; frame < frameCount; frame++) {
      const sample = Math.sin((2 * Math.PI * frequency * frame) / sampleRate) * amplitude;
      const intSample = Math.max(-32768, Math.min(32767, Math.round(sample * 32767)));
      view.setInt16(44 + frame * bytesPerSample, intSample, true);
    }

    return buffer;
  }

  async function waitForTime(context: AudioContext, targetTime: number, timeoutMs: number = 5000): Promise<void> {
    const start = Date.now();
    while (context.currentTime < targetTime) {
      if (Date.now() - start > timeoutMs) {
        throw new Error(`Timed out waiting for currentTime >= ${targetTime}`);
      }
      await new Promise((resolve) => setTimeout(resolve, 10));
    }
  }

  it("should expose AudioContext and play a decoded buffer through the node graph", async function () {
    // JavaScriptCore reports some host constructors as typeof "object".
    expect(AudioContext).to.exist;
    expect(webkitAudioContext).to.exist;
    expect(AudioContext).to.equal(webkitAudioContext);

    const context = new AudioContext();
    expect(context.state).to.equal("suspended");
    await context.resume();
    expect(context.state).to.equal("running");
    expect(context.sampleRate).to.be.greaterThan(0);

    const sampleRate = 44100;
    const frameCount = sampleRate; // 1 second
    const audioBuffer = await context.decodeAudioData(createSilentWavArrayBuffer(sampleRate, frameCount));
    expect(audioBuffer.sampleRate).to.equal(sampleRate);
    expect(audioBuffer.length).to.equal(frameCount);
    expect(audioBuffer.duration).to.be.closeTo(1, 0.01);
    expect(audioBuffer.numberOfChannels).to.equal(1);
    expect(audioBuffer.getChannelData(0).length).to.equal(frameCount);

    const source = context.createBufferSource();
    const gain = context.createGain();
    const panner = context.createPanner();

    source.buffer = audioBuffer;
    gain.gain.value = 0.5;
    panner.setPosition(1, 0, 0);
    context.listener.setPosition(0, 0, 0);

    source.connect(gain);
    gain.connect(panner);
    panner.connect(context.destination);

    let ended = false;
    source.addEventListener("ended", () => {
      ended = true;
    });

    source.start();
    await context.resume();

    // Allow the audio thread to mix at least one callback.
    await new Promise((resolve) => setTimeout(resolve, 50));
    expect(ended).to.equal(false);

    source.stop();
    await new Promise((resolve) => setTimeout(resolve, 50));
    expect(ended).to.equal(true);

    await context.close();
    expect(context.state).to.equal("closed");
  });

  it("should automate AudioParam values over time", async function () {
    const context = new AudioContext();
    const gain = context.createGain();
    gain.connect(context.destination);

    const scheduleStart = context.currentTime;
    gain.gain.cancelScheduledValues(scheduleStart);
    gain.gain.setValueAtTime(0, scheduleStart);
    gain.gain.linearRampToValueAtTime(1, scheduleStart + 1);
    expect(gain.gain.value).to.be.closeTo(0, 0.01);

    const sampleRate = context.sampleRate;
    const frameCount = Math.ceil(sampleRate * 1.5);
    const audioBuffer = await context.decodeAudioData(createSilentWavArrayBuffer(sampleRate, frameCount));
    const source = context.createBufferSource();
    source.buffer = audioBuffer;
    source.connect(gain);
    source.start(scheduleStart);
    await context.resume();

    await waitForTime(context, scheduleStart + 0.5);
    expect(gain.gain.value).to.be.closeTo(0.5, 0.15);

    const curve = new Float32Array([0, 0.25, 0.75, 1]);
    const curveStart = context.currentTime;
    gain.gain.cancelScheduledValues(curveStart);
    gain.gain.setValueCurveAtTime(curve, curveStart, 0.2);
    await waitForTime(context, curveStart + 0.1);
    expect(gain.gain.value).to.be.greaterThan(0.1);
    expect(gain.gain.value).to.be.lessThan(0.9);

    source.stop();
    await context.close();
  });

  it("should return analyser frequency and time-domain data for non-silent audio", async function () {
    const context = new AudioContext();
    const sampleRate = context.sampleRate;
    const frequency = 440;
    const frameCount = sampleRate;
    const audioBuffer = await context.decodeAudioData(createSineWavArrayBuffer(sampleRate, frameCount, frequency));

    const source = context.createBufferSource();
    const analyser = context.createAnalyser();
    analyser.fftSize = 2048;
    analyser.smoothingTimeConstant = 0;

    source.buffer = audioBuffer;
    source.connect(analyser);
    analyser.connect(context.destination);

    const startTime = context.currentTime;
    source.start(startTime);
    await context.resume();

    await waitForTime(context, startTime + 0.25);

    const freqBytes = new Uint8Array(analyser.frequencyBinCount);
    const freqFloats = new Float32Array(analyser.frequencyBinCount);
    const timeBytes = new Uint8Array(analyser.fftSize);
    const timeFloats = new Float32Array(analyser.fftSize);

    analyser.getByteFrequencyData(freqBytes);
    analyser.getFloatFrequencyData(freqFloats);
    analyser.getByteTimeDomainData(timeBytes);
    analyser.getFloatTimeDomainData(timeFloats);

    const maxFreqByte = Math.max(...freqBytes);
    const maxFreqFloat = Math.max(...freqFloats);
    expect(maxFreqByte).to.be.greaterThan(0);
    expect(maxFreqFloat).to.be.greaterThan(analyser.minDecibels);

    let flatTimeDomain = true;
    for (let i = 0; i < timeFloats.length; i++) {
      if (Math.abs(timeFloats[i]) > 0.001) {
        flatTimeDomain = false;
        break;
      }
    }
    expect(flatTimeDomain).to.equal(false);

    let flatByteTimeDomain = true;
    for (let i = 0; i < timeBytes.length; i++) {
      if (timeBytes[i] !== 128) {
        flatByteTimeDomain = false;
        break;
      }
    }
    expect(flatByteTimeDomain).to.equal(false);

    source.stop();
    await context.close();
  });

  it("should support Web Audio constructors", async function () {
    const context = new AudioContext();

    const gainNode = new GainNode(context);
    expect(gainNode.gain.value).to.equal(1);

    const pannerNode = new PannerNode(context);
    expect(pannerNode.positionX.value).to.equal(0);

    const stereoPannerNode = new StereoPannerNode(context);
    expect(stereoPannerNode.pan.value).to.equal(0);

    const analyserNode = new AnalyserNode(context);
    expect(analyserNode.fftSize).to.equal(2048);

    const buffer = new AudioBuffer({ length: 128, sampleRate: 44100, numberOfChannels: 2 });
    expect(buffer.length).to.equal(128);
    expect(buffer.sampleRate).to.equal(44100);
    expect(buffer.numberOfChannels).to.equal(2);

    const sourceNode = new AudioBufferSourceNode(context, { buffer });
    expect(sourceNode.buffer).to.equal(buffer);

    await context.close();
  });

  it("should fire ended naturally when a short buffer finishes", async function () {
    const context = new AudioContext();
    const sampleRate = context.sampleRate;
    const frameCount = Math.ceil(sampleRate * 0.2);
    const audioBuffer = await context.decodeAudioData(createSilentWavArrayBuffer(sampleRate, frameCount));

    const source = context.createBufferSource();
    source.buffer = audioBuffer;
    source.connect(context.destination);

    let ended = false;
    source.addEventListener("ended", () => {
      ended = true;
    });

    const startTime = context.currentTime;
    source.start(startTime);
    await context.resume();
    await waitForTime(context, startTime + audioBuffer.duration + 0.2);

    // Poll briefly in case the ended callback is still marshaling to the JS thread.
    const pollStart = Date.now();
    while (!ended && Date.now() - pollStart < 1000) {
      await new Promise((resolve) => setTimeout(resolve, 10));
    }

    expect(ended).to.equal(true);

    await context.close();
  });

  it("should start buffer sources at a scheduled time", async function () {
    const context = new AudioContext();
    await context.resume();

    const sampleRate = context.sampleRate;
    const frameCount = Math.ceil(sampleRate * 0.1);
    const audioBuffer = await context.decodeAudioData(createSilentWavArrayBuffer(sampleRate, frameCount));

    const source = context.createBufferSource();
    source.buffer = audioBuffer;
    source.connect(context.destination);

    let ended = false;
    source.addEventListener("ended", () => {
      ended = true;
    });

    const startTime = context.currentTime + 0.2;
    source.start(startTime);
    await waitForTime(context, startTime + 0.05);
    expect(ended).to.equal(false);

    await waitForTime(context, startTime + audioBuffer.duration + 0.2);
    const pollStart = Date.now();
    while (!ended && Date.now() - pollStart < 1000) {
      await new Promise((resolve) => setTimeout(resolve, 10));
    }
    expect(ended).to.equal(true);

    await context.close();
  });

  it("should report supported mime types from Audio.canPlayType", function () {
    const audio = new Audio();
    expect(audio.canPlayType("audio/wav")).to.equal("probably");
    expect(audio.canPlayType("audio/mpeg")).to.equal("probably");
    expect(audio.canPlayType("audio/unsupported-format")).to.equal("");
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
