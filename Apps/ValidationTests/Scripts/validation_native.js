console.log("[RYANTREM] SCRIPT START");
var engine;
var currentScene;
var config;
var justOnce;
var saveResult = true;
var testWidth = 600;
var testHeight = 400;

function compare(test, renderData, referenceImage, threshold, errorRatio) {
    var size = renderData.length;
    var referenceData = TestUtils.getImageData(referenceImage);
    var differencesCount = 0;

    for (var index = 0; index < size; index += 4) {
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

    let error = (differencesCount * 100) / (size / 4) > errorRatio;

    if (error) {
        TestUtils.writePNG(referenceData, testWidth, testHeight, TestUtils.getOutputDirectory() + "/Errors/" + test.title + ".png");
    }
    if (saveResult || error) {
        TestUtils.writePNG(renderData, testWidth, testHeight, TestUtils.getOutputDirectory() + "/Results/" + test.title + ".png");
    }
    return error;
}

function evaluate(test, resultCanvas, result, referenceImage, index, waitRing, done) {
    /*var canvasImageData =*/ engine._native.getFramebufferData(function (screenshot) { 
        var testRes = true;
        // Visual check
        if (!test.onlyVisual) {

            var defaultErrorRatio = 2.5

            console.log(`[RYANTREM] comparing results for test ${index}`);
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

function processCurrentScene(test, resultCanvas, result, renderImage, index, waitRing, done) {
    console.log(`[RYANTREM] processCurrentScene for test ${index}`);
    currentScene.useConstantAnimationDeltaTime = true;
    var renderCount = test.renderCount || 1;

    currentScene.executeWhenReady(function() {
        console.log(`[RYANTREM] executeWhenReady for test ${index}`);
        if (currentScene.activeCamera && currentScene.activeCamera.useAutoRotationBehavior) {
            currentScene.activeCamera.useAutoRotationBehavior = false;
        }
        console.log(`[RYANTREM] about to run render loop for test ${index}`);
        engine.runRenderLoop(function() {
            console.log(`[RYANTREM] rendering frame ${renderCount} for test ${index}`);
            try {
                currentScene.render();
                renderCount--;

                if (renderCount === 0) {
                    console.log(`[RYANTREM] stopping render loop for test ${index}`);
                    engine.stopRenderLoop();
                    console.log(`[RYANTREM] evaluating results for test ${index}`);
                    evaluate(test, resultCanvas, result, renderImage, index, waitRing, done);
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

    var test = config.tests[index];
    if (test.onlyVisual || test.excludeFromAutomaticTesting) {
        done(true);
    }
    let testInfo = "Running " + test.title;
    console.log(testInfo);
    TestUtils.setTitle(testInfo);

    seed = 100000;

    let onLoadFileError = function(request, exception) {
        console.log("Failed to retrieve " + url + ".", exception);
    };
    var onload = function(data, responseURL) {
        if (typeof (data) === "string") {
            throw new Error("Decode Image from string data not yet implemented.");
        }

        var referenceImage = TestUtils.decodeImage(data);

        if (test.sceneFolder) {
            BABYLON.SceneLoader.Load(config.root + test.sceneFolder, test.sceneFilename, engine, function(newScene) {
                currentScene = newScene;
                processCurrentScene(test, resultCanvas, result, referenceImage, index, waitRing, done);
            },
                null,
                function(loadedScene, msg) {
                    console.error(msg);
                    done(false);
                });
        }
        else if (test.playgroundId) {
            if (test.playgroundId[0] !== "#" || test.playgroundId.indexOf("#", 1) === -1) {
                console.error("Invalid playground id");
                done(false);
                return;
            }

            var snippetUrl = "https://snippet.babylonjs.com";
            var pgRoot = "https://playground.babylonjs.com";

            var retryTime = 500;
            var maxRetry = 5;
            var retry = 0;

            var onError = function() {
                console.log(`[RYANTREM ${test.playgroundId}] onError executing`);
                retry++;
                if (retry < maxRetry) {
                    setTimeout(function() {
                        loadPG();
                    }, retryTime);
                }
                else {
                    // Skip the test as we can not fetch the source.
                    done(true);
                }
            }

            var loadPG = function() {
                console.log(`[RYANTREM] XHR 2`);
                var xmlHttp = new XMLHttpRequest();
                xmlHttp.addEventListener("readystatechange", function() {
                    console.log(`[RYANTREM] XHR 2 ${xmlHttp.readyState}`);
                    if (xmlHttp.readyState === 4) {
                        try {
                            console.log(`[RYANTREM] XHR 2 set onreadystatechange to null`);
                            xmlHttp.onreadystatechange = null;
                            var snippet = JSON.parse(xmlHttp.responseText);
                            console.log(`[RYANTREM] XHR 2 parsed responseText`);
                            var code = JSON.parse(snippet.jsonPayload).code.toString();
                            //console.log(`[RYANTREM] XHR 2 parsed code: ${code}`);
                            code = code.replace(/\/textures\//g, pgRoot + "/textures/");
                            code = code.replace(/"textures\//g, "\"" + pgRoot + "/textures/");
                            code = code.replace(/\/scenes\//g, pgRoot + "/scenes/");
                            code = code.replace(/"scenes\//g, "\"" + pgRoot + "/scenes/");
                            if (test.replace) {
                                var split = test.replace.split(",");
                                for (var i = 0; i < split.length; i += 2) {
                                    var source = split[i].trim();
                                    var destination = split[i + 1].trim();
                                    code = code.replace(source, destination);
                                }
                            }
                            currentScene = eval(code + "\r\ncreateScene(engine)");
                            var resultCanvas = 0;
                            var result;
                            var waitRing;

                            if (currentScene.then) {
                                console.log(`[RYANTREM] XHR 2 currentScene is a promise`);
                                // Handle if createScene returns a promise
                                currentScene.then(function(scene) {
                                    console.log(`[RYANTREM] XHR 2 currentScene continuation`);
                                    currentScene = scene;
                                    processCurrentScene(test, resultCanvas, result, referenceImage, index, waitRing, done);
                                    console.log(`[RYANTREM] XHR 2 processed scene async`);
                                }).catch(function(e) {
                                    console.log(`[RYANTREM] XHR 2 promise error`);
                                    console.error(e);
                                    onError();
                                })
                            } else {
                                console.log(`[RYANTREM] XHR 2 currentScene is not a promise`);
                                // Handle if createScene returns a scene
                                processCurrentScene(test, resultCanvas, result, referenceImage, index, waitRing, done);
                                console.log(`[RYANTREM] XHR 2 processed scene sync`);
                            }

                        }
                        catch (e) {
                            console.log(`[RYANTREM] XHR 2 processing error`);
                            console.error(e);
                            onError();
                        }
                    }
                }, false);
                xmlHttp.onerror = function() {
                    console.error("Network error during test load.");
                    onError();
                }
                xmlHttp.open("GET", snippetUrl + test.playgroundId.replace(/#/g, "/"));
                xmlHttp.send();
            }
            loadPG();
        } else {
            // Fix references
            if (test.specificRoot) {
                BABYLON.Tools.BaseUrl = config.root + test.specificRoot;
            }

//test2
            console.log(`[RYANTREM] XHR 3`);
            var request = new XMLHttpRequest();
            request.open('GET', config.root + test.scriptToRun, true);

            request.onreadystatechange = function() {
                console.log(`[RYANTREM] XHR 3 ${request.readyState}`);
                if (request.readyState === 4) {
                    try {
                        request.onreadystatechange = null;

                        var scriptToRun = request.responseText.replace(/..\/..\/assets\//g, config.root + "/Assets/");
                        scriptToRun = scriptToRun.replace(/..\/..\/Assets\//g, config.root + "/Assets/");
                        scriptToRun = scriptToRun.replace(/\/assets\//g, config.root + "/Assets/");

                        if (test.replace) {
                            var split = test.replace.split(",");
                            for (var i = 0; i < split.length; i += 2) {
                                var source = split[i].trim();
                                var destination = split[i + 1].trim();
                                scriptToRun = scriptToRun.replace(source, destination);
                            }
                        }

                        if (test.replaceUrl) {
                            var split = test.replaceUrl.split(",");
                            for (var i = 0; i < split.length; i++) {
                                var source = split[i].trim();
                                var regex = new RegExp(source, "g");
                                scriptToRun = scriptToRun.replace(regex, config.root + test.rootPath + source);
                            }
                        }

                        currentScene = eval(scriptToRun + test.functionToCall + "(engine)");
                        processCurrentScene(test, resultCanvas, result, renderImage, index, waitRing, done);
                    }
                    catch (e) {
                        console.error(e);
                        done(false);
                    }
                }
            };
            request.onerror = function() {
                console.error("Network error during test load.");
                done(false);
            }

            request.send(null);
        }
    };

    let url = "https://raw.githubusercontent.com/BabylonJS/Babylon.js/master/tests/validation/ReferenceImages/" + test.referenceImage;
    BABYLON.Tools.LoadFile(url, onload, undefined, undefined, /*useArrayBuffer*/true, onLoadFileError);
}

var engine;
var scene;
var canvas;
var xhr;

_native.graphicsInitializationPromise.then(() => {

    engine = new BABYLON.NativeEngine();
    scene = new BABYLON.Scene(engine);
    canvas = window;

    engine.getRenderingCanvas = function () {
        return window;
    }

    engine.getInputElement = function () {
        return 0;
    }

    OffscreenCanvas = function(width, height) {
        return {
            width: width
            , height: height
            , getContext: function(type) {
                return {
                    fillRect: function(x, y, w, h) { }
                    , measureText: function(text) { return 8; }
                    , fillText: function(text, x, y) { }
                };
            }
        };
    }

    document = {
        createElement: function (type) {
            if (type === "canvas") {
                return new OffscreenCanvas(64, 64);
            }
            return {};
        }
    }

    console.log("[RYANTREM] CREATING OUTERMOST XHR");
    xhr = new XMLHttpRequest();
    xhr.open("GET", TestUtils.getResourceDirectory() + "config.json", true);

    xhr.addEventListener("readystatechange", function() {
        console.log(`[RYANTREM] OUTERMOST XHR READYSTATECHANGE ${xhr.readyState}`);
        if (xhr.status === 200) {
            config = JSON.parse(xhr.responseText);

            // Run tests
            var index = 0;
            var recursiveRunTest = function(i) {
                runTest(i, function(status) {
                    console.log(`[RYANTREM] finished test ${i} with ${status}`);
                    if (!status) {
                        TestUtils.exit(-1);
                        return;
                    }
                    i++;
                    if (justOnce || i >= config.tests.length) {
                        TestUtils.exit(0);
                        return;
                    }
                    recursiveRunTest(i);
                });
            }

            recursiveRunTest(index);
        }
    }, false);

    _native.RootUrl = "https://playground.babylonjs.com";
    console.log("Starting");
    TestUtils.setTitle("Starting Native Validation Tests");
    TestUtils.updateSize(testWidth, testHeight);
    xhr.send();
});
