// TODO: use premade reporter (once Console Polyfill is fixed)
function BabylonReporter(runner) {
    const stats = runner.stats;

    runner.on("pass", test => {
        console.log(`Passed: ${test.fullTitle()}`);
    });

    runner.on("fail", (test, err) => {
        console.log(`Failed: ${test.fullTitle()} with error: ${err.message}`);
    });

    runner.on("end", () => {
        console.log(`Tests passed: ${stats.passes}/${stats.tests}`);
    });
}

mocha.setup({ ui: "bdd", reporter: BabylonReporter });

function createRequest(method, url) {
    return new Promise((resolve, reject) => {
        const xhr = new XMLHttpRequest();
        xhr.open(method, url);
        xhr.addEventListener("loadend", () => resolve(xhr));
        xhr.send();
    });
}

const expect = chai.expect;

describe("XMLHTTPRequest", function () {
    this.timeout(0);
    it("should have readyState=4 when load ends", async function () {
        const xhr = await createRequest("GET", "https://babylonjs.com");
        expect(xhr.readyState).to.equal(4);
    })
    it("should have status=200 for a file that exists", async function () {
        const xhr = await createRequest("GET", "https://babylonjs.com");
        expect(xhr.status).to.equal(200);
    })
    it("should load unescaped URLs", async function () {
        const xhr = await createRequest("GET", "https://github.com/BabylonJS/Assets/raw/master/meshes/στρογγυλεμένος % κύβος.glb");
        expect(xhr.status).to.equal(200);
    })
    it("should load partially unescaped URLs", async function () {
        const xhr = await createRequest("GET", "https://github.com/BabylonJS/Assets/raw/master/meshes/στρογγυλεμένος%20%%20κύβος.glb");
        expect(xhr.status).to.equal(200);
    })
    it("should load escaped URLs", async function () {
        const xhr = await createRequest("GET", "https://github.com/BabylonJS/Assets/raw/master/meshes/%CF%83%CF%84%CF%81%CE%BF%CE%B3%CE%B3%CF%85%CE%BB%CE%B5%CE%BC%CE%AD%CE%BD%CE%BF%CF%82%20%25%20%CE%BA%CF%8D%CE%B2%CE%BF%CF%82.glb");
        expect(xhr.status).to.equal(200);
    })
    it("should load URLs with unescaped %s", async function () {
        const xhr = await createRequest("GET", "https://github.com/BabylonJS/Assets/raw/master/meshes/%CF%83%CF%84%CF%81%CE%BF%CE%B3%CE%B3%CF%85%CE%BB%CE%B5%CE%BC%CE%AD%CE%BD%CE%BF%CF%82%20%%20%CE%BA%CF%8D%CE%B2%CE%BF%CF%82.glb");
        expect(xhr.status).to.equal(200);
    })
    it("should have status=404 for a file that does not exist", async function () {
        const xhr = await createRequest("GET", "https://babylonjs.com/invalid");
        expect(xhr.status).to.equal(404);
    })
    it("should throw something when opening //", async function () {
        function openDoubleSlash() {
            const xhr = new XMLHttpRequest();
            xhr.open("GET", "//");
            xhr.send();
        }
        expect(openDoubleSlash).to.throw();
    })
    it("should throw something when opening a url with no scheme", function () {
        function openNoProtocol() {
            const xhr = new XMLHttpRequest();
            xhr.open("GET", "noscheme.glb");
            xhr.send();
        }
        expect(openNoProtocol).to.throw();
    })
    it("should throw something when sending before opening", function () {
        function sendWithoutOpening() {
            const xhr = new XMLHttpRequest();
            xhr.send();
        }
        expect(sendWithoutOpening).to.throw();
    })
    it("should open a local file", async function () {
        const xhr = await createRequest("GET", "app:///Scripts/tests.js");
        expect(xhr).to.have.property('readyState', 4);
        expect(xhr).to.have.property('status', 200);
        expect(xhr).to.have.property('responseText').with.lengthOf.above(0);
    })
});
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
    })
})

function createSceneAndWait(callback, done) {
    const engine = new BABYLON.NativeEngine();
    const scene = new BABYLON.Scene(engine);
    scene.createDefaultCameraOrLight();
    callback(engine, scene);
    scene.executeWhenReady(() => {
        done();
    })
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
    })
    it("GradientMaterial should compile", function(done) {
        createSceneAndWait((engine, scene) => {
            const sphere = BABYLON.MeshBuilder.CreateSphere("sphere", {diameter: 2, segments: 32}, scene);
            const gradientMaterial = new BABYLON.GradientMaterial("grad", scene);
            gradientMaterial.topColor = BABYLON.Color3.Red(); // Set the gradient top color
            gradientMaterial.bottomColor = BABYLON.Color3.Blue(); // Set the gradient bottom color
            gradientMaterial.offset = 0.25;
            sphere.material = gradientMaterial;
        }, done);
    });
})

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
    // it("FxaaPostProcess", function(done) {
    //     createSceneAndWait((engine, scene) => {
    //         const camera = scene._activeCamera;
    //         new BABYLON.FxaaPostProcess("fxaa", 1.0, camera);
    //     }, done);
    // });
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
    // it("RefractionPostProcess", function(done) {
    //     createSceneAndWait((engine, scene) => {
    //         const camera = scene._activeCamera;
    //         new BABYLON.RefractionPostProcess("Refraction", "https://playground.babylonjs.com/textures/grass.jpg", new BABYLON.Color3(1.0, 1.0, 1.0), 0.5, 0.5, 1.0, camera);
    //     }, done);
    // });
    // it("ColorCorrectionPostProcess", function(done) {
    //     createSceneAndWait((engine, scene) => {
    //         const camera = scene._activeCamera;
    //         new BABYLON.ColorCorrectionPostProcess("color_correction", "https://doc.babylonjs.com/_next/image?url=%2Fimg%2Fhow_to%2Fpost-processes%2Flut-inverted.png&w=256&q=16", 1.0, camera);
    //     }, done);
    // });
    // it("DefaultPipeline", function(done) {
    //     createSceneAndWait((engine, scene) => {
    //         const camera = scene._activeCamera;
    //         new BABYLON.DefaultRenderingPipeline(
    //             "defaultPipeline", // The name of the pipeline
    //             true, // Do you want the pipeline to use HDR texture?
    //             scene, // The scene instance
    //             [camera] // The list of cameras to be attached to
    //         );
    //     }, done);
    // });
    // it("LensRenderingPipeline", function(done) {
    //     createSceneAndWait((engine, scene) => {
    //         const camera = scene._activeCamera;
    //         const parameters = {
    //             edge_blur: 1.0,
    //             chromatic_aberration: 1.0,
    //             distortion: 1.0,
    //         };              
    //         new BABYLON.LensRenderingPipeline('lensEffects', parameters, scene, 1.0, camera);
    //     }, done);
    // });
    // it("MotionBlurPostProcess", function(done) {
    //     createSceneAndWait((engine, scene) => {
    //         const camera = scene._activeCamera;
    //         new BABYLON.MotionBlurPostProcess(
    //             "mb", // The name of the effect.
    //             scene, // The scene containing the objects to blur according to their velocity.
    //             1.0, // The required width/height ratio to downsize to before computing the render pass.
    //             camera // The camera to apply the render pass to.
    //         );
    //     }, done);
    // });
    // it("ScreenSpaceReflectionPostProcess", function(done) {
    //     createSceneAndWait((engine, scene) => {
    //         const camera = scene._activeCamera;
    //         new BABYLON.ScreenSpaceReflectionPostProcess("ssr", scene, 1.0, camera);
    //     }, done);
    // });
})

mocha.run(failures => {
    // Test program will wait for code to be set before exiting
    if (failures > 0) {
        // Failure
        SetExitCode(1);
    } else {
        // Success
        SetExitCode(0);
    }
});