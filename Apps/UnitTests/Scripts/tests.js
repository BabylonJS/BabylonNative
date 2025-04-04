mocha.setup({ ui: "bdd", reporter: "spec", retries: 5 });

const expect = chai.expect;

describe("RequestFile", function () {
    this.timeout(0);
    it("should throw when requesting a URL with no protocol", function () {
        function RequestFile() {
            BABYLON.FileTools.RequestFile(
                "noprotocol.gltf",
                () => {},
            );
        }
        expect(RequestFile).to.throw();
    });
});

describe("CanvasAndContext", function () {
    const engine = new BABYLON.NativeEngine();
    const scene = new BABYLON.Scene(engine);

    const texSize = 512;
    const dynamicTexture = new BABYLON.DynamicTexture("dynamic texture", texSize, scene);
    const context = dynamicTexture.getContext();
    const otherContext = dynamicTexture.getContext();

    expect(context).to.equal(context.canvas.getContext());
    expect(context).to.equal(otherContext);
    expect(context).to.equal(otherContext.canvas.getContext());
});

describe("ColorParsing", function () {
    expect(_native.Canvas.parseColor("")).to.equal(0);
    expect(_native.Canvas.parseColor("transparent")).to.equal(0);
    expect(_native.Canvas.parseColor("#123")).to.equal(0xFF332211);
    expect(_native.Canvas.parseColor("#1234")).to.equal(0x44332211);
    expect(_native.Canvas.parseColor("#123456")).to.equal(0xFF563412);
    expect(_native.Canvas.parseColor("#12345678")).to.equal(0x78563412);
    expect(_native.Canvas.parseColor("snow")).to.equal(0xFFFAFAFF);
    expect(_native.Canvas.parseColor("rgb(16,32,48)")).to.equal(0xFF302010);
    expect(_native.Canvas.parseColor("rgba(16,32,48,64)")).to.equal(0x40302010);
    expect(_native.Canvas.parseColor("rgb(16,     32   ,  48   )")).to.equal(0xFF302010);
    expect(_native.Canvas.parseColor("rgba(    16,     32   ,  48 , 64  )")).to.equal(0x40302010);
    
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

function createSceneAndWait(callback, done) {
    const engine = new BABYLON.NativeEngine();
    const scene = new BABYLON.Scene(engine);
    scene.createDefaultCamera();
    callback(engine, scene);
    scene.executeWhenReady(() => {
        done();
    });
}

describe("Materials", function() {
    this.timeout(0);
    it("Empty ShaderMaterial should compile", function(done) {
        function createEmptyShaderMat() {
            createSceneAndWait((engine, scene) => {
                const sphere = BABYLON.MeshBuilder.CreateSphere("sphere", {diameter: 2, segments: 32}, scene);
                const shaders = {
                    vertexSource: `void main() {}`,
                    fragmentSource: `void main() {}`
                }
                const mat = new BABYLON.ShaderMaterial("shader", scene, shaders, {});
                sphere.material = mat;
            }, done);
        }
        createEmptyShaderMat()
    });
    it("GradientMaterial should compile", function(done) {
        createSceneAndWait((engine, scene) => {
            const sphere = BABYLON.MeshBuilder.CreateSphere("sphere", {diameter: 2, segments: 32}, scene);
            const gradientMaterial = new BABYLON.GradientMaterial("grad", scene);
            sphere.material = gradientMaterial;
        }, done);
    });
});

describe("PostProcesses", function() {
    this.timeout(0);
    it("PassPostProcess", function(done) {
        createSceneAndWait((engine, scene) => {
            const camera = scene._activeCamera;
            new BABYLON.PassPostProcess("Scene copy", 1.0, camera);
        }, done);
    });
    it("BlackAndWhitePostProcess", function(done) {
        createSceneAndWait((engine, scene) => {
            const camera = scene._activeCamera;
            new BABYLON.BlackAndWhitePostProcess("bandw", 1.0, camera);
        }, done);
    });
    it("BlurPostProcess", function(done) {
        createSceneAndWait((engine, scene) => {
            const camera = scene._activeCamera;
            new BABYLON.BlurPostProcess("Horizontal blur", new BABYLON.Vector2(1.0, 0), 32, 0.25, camera);
        }, done);
    });
    it("ConvolutionPostProcess", function(done) {
        createSceneAndWait((engine, scene) => {
            const camera = scene._activeCamera;
            new BABYLON.ConvolutionPostProcess("Sepia", BABYLON.ConvolutionPostProcess.EmbossKernel, 1.0, camera);
        }, done);
    });
    it("HighlightsPostProcess", function(done) {
        createSceneAndWait((engine, scene) => {
            const camera = scene._activeCamera;
            new BABYLON.HighlightsPostProcess("highlights", 1.0, camera);
        }, done);
    });
    it("TonemapPostProcess", function(done) {
        createSceneAndWait((engine, scene) => {
            const camera = scene._activeCamera;
            new BABYLON.TonemapPostProcess("tonemap", BABYLON.TonemappingOperator.Hable, 1.0, camera);
        }, done);
    });
    it("ImageProcessingPostProcess", function(done) {
        createSceneAndWait((engine, scene) => {
            const camera = scene._activeCamera;
            new BABYLON.ImageProcessingPostProcess("processing", 1.0, camera);
        }, done);
    });
    it("RefractionPostProcess", function(done) {
        createSceneAndWait((engine, scene) => {
            const camera = scene._activeCamera;
            new BABYLON.RefractionPostProcess("Refraction", "https://playground.babylonjs.com/textures/grass.jpg", new BABYLON.Color3(1.0, 1.0, 1.0), 0.5, 0.5, 1.0, camera);
        }, done);
    });
    it("DefaultPipeline", function(done) {
        createSceneAndWait((engine, scene) => {
            const camera = scene._activeCamera;
            new BABYLON.DefaultRenderingPipeline(
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
            new BABYLON.FxaaPostProcess("fxaa", 1.0, camera);
        }, done);
    });
    it("ColorCorrectionPostProcess", function(done) {
        createSceneAndWait((engine, scene) => {
            const camera = scene._activeCamera;
            new BABYLON.ColorCorrectionPostProcess("color_correction", "https://doc.babylonjs.com/_next/image?url=%2Fimg%2Fhow_to%2Fpost-processes%2Flut-inverted.png&w=256&q=16", 1.0, camera);
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
            new BABYLON.LensRenderingPipeline('lensEffects', parameters, scene, 1.0, camera);
        }, done);
    });
    it("MotionBlurPostProcess", function(done) {
        createSceneAndWait((engine, scene) => {
            const camera = scene._activeCamera;
            new BABYLON.MotionBlurPostProcess(
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
            new BABYLON.ScreenSpaceReflectionPostProcess("ssr", scene, 1.0, camera);
        }, done);
    });*/
});

mocha.run(failures => {
    // Test program will wait for code to be set before exiting
    if (failures > 0) {
        // Failure
        setExitCode(1);
    } else {
        // Success
        setExitCode(0);
    }
});