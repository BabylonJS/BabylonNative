/// <reference path="../../../../Babylon.js/dist/preview release/babylon.d.ts" />
/// <reference path="../../../../Babylon.js/dist/preview release/loaders/babylonjs.loaders.d.ts" />

let engine;
let currentScene;
let config;
let justOnce = false;
const saveResult = true;
const testWidth = 600;
const testHeight = 400;

function compare(test, renderData, referenceImage, threshold, errorRatio) {
    const size = renderData.length;
    const referenceData = TestUtils.getImageData(referenceImage);
    let differencesCount = 0;

    for (let index = 0; index < size; index += 4) {
        if (Math.abs(renderData[index] - referenceData[index]) < threshold &&
            Math.abs(renderData[index + 1] - referenceData[index + 1]) < threshold &&
            Math.abs(renderData[index + 2] - referenceData[index + 2]) < threshold) {
            continue;
        }

        if (differencesCount === 0) {
            console.log(`First pixel off at ${index}: Value: (${renderData[index]}, ${renderData[index + 1]}, ${renderData[index] + 2}) - Expected: (${referenceData[index]}, ${referenceData[index + 1]}, ${referenceData[index + 2]}) `);
        }

        referenceData[index] = 255;
        referenceData[index + 1] *= 0.5;
        referenceData[index + 2] *= 0.5;
        differencesCount++;
    }

    if (differencesCount) {
        console.log("Pixel difference: " + differencesCount + " pixels.");
    }

    const error = (differencesCount * 100) / (size / 4) > errorRatio;

    if (error) {
        TestUtils.writePNG(referenceData, testWidth, testHeight, TestUtils.getOutputDirectory() + "/Errors/" + test.title + ".png");
    }
    if (saveResult || error) {
        TestUtils.writePNG(renderData, testWidth, testHeight, TestUtils.getOutputDirectory() + "/Results/" + test.title + ".png");
    }
    return error;
}

function evaluate(test, referenceImage, done) {
    engine._native.getFramebufferData(function (screenshot) {
        let testRes = true;
        // Visual check
        if (!test.onlyVisual) {

            const defaultErrorRatio = 2.5

            if (compare(test, screenshot, referenceImage, test.threshold || 25, test.errorRatio || defaultErrorRatio)) {
                testRes = false;
                console.log('failed');
            } else {
                testRes = true;
                console.log('validated');
            }
        }

        currentScene.dispose();
        currentScene = null;
        engine.setHardwareScalingLevel(1);

        done(testRes);
    });
}

function processCurrentScene(test, renderImage, done) {
    currentScene.useConstantAnimationDeltaTime = true;
    let renderCount = test.renderCount || 1;

    console.log("Waiting for scene to be ready");
    currentScene.executeWhenReady(function () {
        console.log("Scene is ready");
        if (currentScene.activeCamera && currentScene.activeCamera.useAutoRotationBehavior) {
            currentScene.activeCamera.useAutoRotationBehavior = false;
        }
        engine.runRenderLoop(function () {
            try {
                console.log("render " + renderCount);
                currentScene.render();
                renderCount--;

                if (renderCount === 0) {
                    engine.stopRenderLoop();
                    evaluate(test, renderImage, done);
                }
            }
            catch (e) {
                console.error(e);
                done(false);
            }
        });
    });
}

function runTest(index, done) {
    if (index >= config.tests.length) {
        done(false);
    }

    const test = config.tests[index];
    if (test.onlyVisual || test.excludeFromAutomaticTesting) {
        done(true);
    }
    const testInfo = "Running " + test.title;
    console.log(testInfo);
    TestUtils.setTitle(testInfo);

    const onerror = function (request, exception) {
        console.log("Failed to retrieve " + url + ".", exception);
        done(false);
    };

    const onload = function (data, responseURL) {
        if (typeof (data) === "string") {
            throw new Error("Decode Image from string data not yet implemented.");
        }

        const referenceImage = TestUtils.decodeImage(data);

        if (test.playgroundId) {
            if (test.playgroundId[0] !== "#" || test.playgroundId.indexOf("#", 1) === -1) {
                console.error("Invalid playground id");
                done(false);
                return;
            }

            const snippetUrl = "https://snippet.babylonjs.com";
            const pgRoot = "https://playground.babylonjs.com";

            const url = snippetUrl + test.playgroundId.replace(/#/g, "/");
            console.log("Loading " + url);
            BABYLON.Tools.LoadFile(snippetUrl + test.playgroundId.replace(/#/g, "/"), function (data) {
                console.log("Loaded");
                try {
                    console.log("Parsing JSON");
                    const snippet = JSON.parse(data);
                    let code = JSON.parse(snippet.jsonPayload).code.toString();
                    code = code.replace(/\/textures\//g, pgRoot + "/textures/");
                    code = code.replace(/"textures\//g, "\"" + pgRoot + "/textures/");
                    code = code.replace(/\/scenes\//g, pgRoot + "/scenes/");
                    code = code.replace(/"scenes\//g, "\"" + pgRoot + "/scenes/");
                    if (test.replace) {
                        const split = test.replace.split(",");
                        for (let i = 0; i < split.length; i += 2) {
                            const source = split[i].trim();
                            const destination = split[i + 1].trim();
                            code = code.replace(source, destination);
                        }
                    }
                    console.log("Eval code");
                    currentScene = eval(code + "\r\ncreateScene(engine)");

                    if (currentScene.then) {
                        console.log("currentScene.then");
                        // Handle if createScene returns a promise
                        currentScene.then(function (scene) {
                            currentScene = scene;
                            processCurrentScene(test, referenceImage, done);
                        }).catch(function (e) {
                            console.error(e);
                            done(false);
                        })
                    } else {
                        // Handle if createScene returns a scene
                        processCurrentScene(test, referenceImage, done);
                    }
                }
                catch (e) {
                    console.error(e);
                    done(false);
                }
            });
        } else {
            throw new Error("Not implemented");
        }
    };

    const url = "https://raw.githubusercontent.com/BabylonJS/Babylon.js/master/tests/validation/ReferenceImages/" + test.referenceImage;
    BABYLON.Tools.LoadFile(url, onload, undefined, undefined, /*useArrayBuffer*/true, onerror);
}

_native.graphicsInitializationPromise.then(() => {
    engine = new BABYLON.NativeEngine();
    canvas = window;

    engine.getRenderingCanvas = function () {
        return window;
    }

    engine.getInputElement = function () {
        return 0;
    }

    OffscreenCanvas = function (width, height) {
        return {
            width: width
            , height: height
            , getContext: function (type) {
                return {
                    fillRect: function (x, y, w, h) { }
                    , measureText: function (text) { return 8; }
                    , fillText: function (text, x, y) { }
                };
            }
        };
    };

    document = {
        createElement: function (type) {
            if (type === "canvas") {
                return new OffscreenCanvas(64, 64);
            }
            return {};
        }
    }

    console.log("Starting");
    TestUtils.setTitle("Starting Native Validation Tests");
    TestUtils.updateSize(testWidth, testHeight);

    BABYLON.Tools.LoadFile(TestUtils.getResourceDirectory() + "config.json", function (data) {
        config = JSON.parse(data);

        // Run tests
        const recursiveRunTest = function (index) {
            runTest(index, function (status) {
                if (!status) {
                    TestUtils.exit(-1);
                    return;
                }

                if (justOnce || index + 1 >= config.tests.length) {
                    TestUtils.exit(0);
                    return;
                }

                recursiveRunTest(index + 1);
            });
        }

        recursiveRunTest(0);
    });
});
