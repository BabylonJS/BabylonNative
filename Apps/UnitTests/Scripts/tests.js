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
        const xhr = await createRequest("GET", "https://httpstat.us/");
        expect(xhr.readyState).to.equal(4);
    });
    it("should have status=200 for a file that exists", async function () {
        const xhr = await createRequest("GET", "https://httpstat.us/200");
        expect(xhr.status).to.equal(200);
    });
    it("should load URLs with escaped unicode characters", async function () {
        const xhr = await createRequest("GET", "https://raw.githubusercontent.com/BabylonJS/Assets/master/meshes/%CF%83%CF%84%CF%81%CE%BF%CE%B3%CE%B3%CF%85%CE%BB%CE%B5%CE%BC%CE%AD%CE%BD%CE%BF%CF%82%20%25%20%CE%BA%CF%8D%CE%B2%CE%BF%CF%82.glb");
        expect(xhr.status).to.equal(200);
    });
    it("should load URLs with unescaped unicode characters", async function () {
        const xhr = await createRequest("GET", "https://raw.githubusercontent.com/BabylonJS/Assets/master/meshes/στρογγυλεμένος%20%25%20κύβος.glb");
        expect(xhr.status).to.equal(200);
    });
    it("should load URLs with unescaped unicode characters and spaces", async function () {
        const xhr = await createRequest("GET", "https://raw.githubusercontent.com/BabylonJS/Assets/master/meshes/στρογγυλεμένος %25 κύβος.glb");
        expect(xhr.status).to.equal(200);
    });
    it("should have status=404 for a file that does not exist", async function () {
        const xhr = await createRequest("GET", "https://httpstat.us/404");
        expect(xhr.status).to.equal(404);
    });
    it("should throw something when opening //", async function () {
        function openDoubleSlash() {
            const xhr = new XMLHttpRequest();
            xhr.open("GET", "//");
            xhr.send();
        }
        expect(openDoubleSlash).to.throw();
    });
    it("should throw something when opening a url with no scheme", function () {
        function openNoProtocol() {
            const xhr = new XMLHttpRequest();
            xhr.open("GET", "noscheme.glb");
            xhr.send();
        }
        expect(openNoProtocol).to.throw();
    });
    it("should throw something when sending before opening", function () {
        function sendWithoutOpening() {
            const xhr = new XMLHttpRequest();
            xhr.send();
        }
        expect(sendWithoutOpening).to.throw();
    });
    it("should open a local file", async function () {
        const xhr = await createRequest("GET", "app:///Scripts/tests.js");
        expect(xhr).to.have.property('readyState', 4);
        expect(xhr).to.have.property('status', 200);
        expect(xhr).to.have.property('responseText').with.lengthOf.above(0);
    });
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
    });
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
    /*
    TODO: Uncomment tests for materials as we implement more features
    it("GradientMaterial should compile", function(done) {
        createSceneAndWait((engine, scene) => {
            const sphere = BABYLON.MeshBuilder.CreateSphere("sphere", {diameter: 2, segments: 32}, scene);
            const gradientMaterial = new BABYLON.GradientMaterial("grad", scene);
            sphere.material = gradientMaterial;
        }, done);
    });*/
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
    /*
    TODO: Uncomment tests for post processes as we implement more features
    it("FxaaPostProcess", function(done) {
        createSceneAndWait((engine, scene) => {
            const camera = scene._activeCamera;
            new BABYLON.FxaaPostProcess("fxaa", 1.0, camera);
        }, done);
    });
    it("RefractionPostProcess", function(done) {
        createSceneAndWait((engine, scene) => {
            const camera = scene._activeCamera;
            new BABYLON.RefractionPostProcess("Refraction", "https://playground.babylonjs.com/textures/grass.jpg", new BABYLON.Color3(1.0, 1.0, 1.0), 0.5, 0.5, 1.0, camera);
        }, done);
    });
    it("ColorCorrectionPostProcess", function(done) {
        createSceneAndWait((engine, scene) => {
            const camera = scene._activeCamera;
            new BABYLON.ColorCorrectionPostProcess("color_correction", "https://doc.babylonjs.com/_next/image?url=%2Fimg%2Fhow_to%2Fpost-processes%2Flut-inverted.png&w=256&q=16", 1.0, camera);
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

describe("setTimeout", function () {
    this.timeout(1000);
    it("should return an id greater than zero", function () {
        const id = setTimeout(() => { }, 0);
        expect(id).to.be.greaterThan(0);
    });
    it("should return an id greater than zero when given an undefined function", function () {
        const id = setTimeout(undefined, 0);
        expect(id).to.be.greaterThan(0);
    });
    it("should call the given function after the given delay", function (done) {
        const startTime = new Date().getTime();
        setTimeout(() => {
            try {
                expect(new Date().getTime() - startTime).to.be.at.least(10);
                done();
            }
            catch (e) {
                done(e);
            }
        }, 10);
    });
    it("should call the given nested function after the given delay", function (done) {
        const startTime = new Date().getTime();
        setTimeout(() => {
            setTimeout(() => {
                try {
                    expect(new Date().getTime() - startTime).to.be.at.least(20);
                    done();
                }
                catch (e) {
                    done(e);
                }
            }, 10);
        }, 10);
    });
    it("should call the given function after the given delay when the delay is a string representing a valid number", function (done) {
        const startTime = new Date().getTime();
        setTimeout(() => {
            try {
                expect(new Date().getTime() - startTime).to.be.at.least(10);
                done();
            }
            catch (e) {
                done(e);
            }
        }, "10");
    });
    it("should call the given function after zero milliseconds when the delay is a string representing an invalid number", function (done) {
        setTimeout(() => {
            done();
        }, "a");
    });
    it("should call the given function after other tasks execute when the given delay is zero", function (done) {
        let trailingCodeExecuted = false;
        setTimeout(() => {
            try {
                expect(trailingCodeExecuted).to.be.true;
                done();
            }
            catch (e) {
                done(e);
            }
         }, 0);
        trailingCodeExecuted = true;
    });
    it("should call the given function after other tasks execute when the given delay is undefined", function (done) {
        let trailingCodeExecuted = false;
        setTimeout(() => {
            try {
                expect(trailingCodeExecuted).to.be.true;
                done();
            }
            catch (e) {
                done(e);
            }
         }, undefined);
        trailingCodeExecuted = true;
    });
    it("should call the given functions in the correct order", function (done) {
        const called = [];
        for (let i = 9; i >= 0; i--) {
            setTimeout(() => {
                called.push(i);
                if (called.length === 10) {
                    try {
                        expect(called).to.deep.equal([0, 1, 2, 3, 4, 5, 6, 7, 8, 9]);
                        done();
                    }
                    catch (e) {
                        done(e);
                    }
                }
            }, i * 10);
        }
    });
});

describe("clearTimeout", function () {
    this.timeout(0);
    it("should stop the timeout matching the given timeout id", function (done) {
        const id = setTimeout(() => {
            done(new Error("Timeout was not cleared"));
        }, 0);
        clearTimeout(id);
        setTimeout(done, 100);
    });
    it("should do nothing if the given timeout id is undefined", function (done) {
        setTimeout(() => { done(); }, 0);
        clearTimeout(undefined);
    });
});

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