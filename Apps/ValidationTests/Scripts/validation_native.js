var engine;
var canvas;
var currentScene;
var config;
var justOnce;
var saveResult = true;
var testWidth = 600;
var testHeight = 400;
var generateReferences = false;
// Random replacement
var seed = 1;
Math.random = function () {
    var x = Math.sin(seed++) * 10000;
    return x - Math.floor(x);
}

function compareStats(refStats, currentStats) {
    var keysToCompare = ["numCompute", "numDynamicIndexBuffers", "numDynamicVertexBuffers", "numFrameBuffers",
        "numIndexBuffers", "numOcclusionQueries", "numPrograms", "numShaders", "numTextures", "numUniforms", "numVertexBuffers", "numVertexLayouts",
        "textureMemoryUsed", "rtMemoryUsed", "transientVbUsed", "transientIbUsed", "numViews"];
    let error = false;
    keysToCompare.forEach(element => {
        if (currentStats[element] > refStats[element]) {
            console.log("Error on resources for ", element, " : got ", currentStats[element], " instead of", refStats[element]);
            error = true;
        }
    });
    return error;
}

function compare(test, renderData, referenceImage, threshold, errorRatio, refStats) {
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

    error |= compareStats(refStats, TestUtils.getStats());

    if (error) {
        TestUtils.writePNG(referenceData, testWidth, testHeight, TestUtils.getOutputDirectory() + "/Errors/" + test.referenceImage);
    }
    if (saveResult || error) {
        TestUtils.writePNG(renderData, testWidth, testHeight, TestUtils.getOutputDirectory() + "/Results/" + test.referenceImage);
    }
    return error;
}

function saveRenderedResult(test, renderData) {
    var imageName = TestUtils.getOutputDirectory() + "/Results/" + test.referenceImage;
    var profileName = imageName.replace(".png", ".json");
    
    TestUtils.writePNG(renderData, testWidth, testHeight, imageName);
    TestUtils.writeString(profileName, JSON.stringify(TestUtils.getStats()));
    return false; // no error
}

function evaluate(test, resultCanvas, result, referenceImage, index, waitRing, done, compareFunction, refStats) {
    /*var canvasImageData =*/ engine._native.getFrameBufferData(function (screenshot) { 
        var testRes = true;
        // Visual check
        if (!test.onlyVisual) {

            var defaultErrorRatio = 2.5

            if (compareFunction(test, screenshot, referenceImage, test.threshold || 25, test.errorRatio || defaultErrorRatio, refStats)) {
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

function processCurrentScene(test, resultCanvas, result, renderImage, index, waitRing, done, compareFunction, refStats) {
    currentScene.useConstantAnimationDeltaTime = true;
    var renderCount = test.renderCount || 1;

    currentScene.executeWhenReady(function() {
        if (currentScene.activeCamera && currentScene.activeCamera.useAutoRotationBehavior) {
            currentScene.activeCamera.useAutoRotationBehavior = false;
        }
        engine.runRenderLoop(function() {
            try {
                currentScene.render();
                renderCount--;

                if (renderCount === 0) {
                    engine.stopRenderLoop();
                    evaluate(test, resultCanvas, result, renderImage, index, waitRing, done, compareFunction, refStats);
                }
            }
            catch (e) {
                console.error(e);
                done(false);
            }
        });
    });
}

function loadPlayground(test, done, index, referenceImage, compareFunction, refStats) {
    if (test.sceneFolder) {
        BABYLON.SceneLoader.Load(config.root + test.sceneFolder, test.sceneFilename, engine, function (newScene) {
            currentScene = newScene;
            processCurrentScene(test, resultCanvas, result, referenceImage, index, waitRing, done, compareFunction, refStats);
        },
            null,
            function (loadedScene, msg) {
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

        var onError = function () {
            retry++;
            if (retry < maxRetry) {
                setTimeout(function () {
                    loadPG();
                }, retryTime);
            }
            else {
                // Skip the test as we can not fetch the source.
                done(true);
            }
        }

        var loadPG = function () {
            var xmlHttp = new XMLHttpRequest();
            xmlHttp.addEventListener("readystatechange", function () {
                if (xmlHttp.readyState === 4) {
                    try {
                        xmlHttp.onreadystatechange = null;
                        var snippet = JSON.parse(xmlHttp.responseText);
                        var code = JSON.parse(snippet.jsonPayload).code.toString();
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
                            // Handle if createScene returns a promise
                            currentScene.then(function (scene) {
                                currentScene = scene;
                                processCurrentScene(test, resultCanvas, result, referenceImage, index, waitRing, done, compareFunction, refStats);
                            }).catch(function (e) {
                                console.error(e);
                                onError();
                            })
                        } else {
                            // Handle if createScene returns a scene
                            processCurrentScene(test, resultCanvas, result, referenceImage, index, waitRing, done, compareFunction, refStats);
                        }

                    }
                    catch (e) {
                        console.error(e);
                        onError();
                    }
                }
            }, false);
            xmlHttp.onerror = function () {
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

        var request = new XMLHttpRequest();
        request.open('GET', config.root + test.scriptToRun, true);

        request.onreadystatechange = function () {
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
                    processCurrentScene(test, resultCanvas, result, renderImage, index, waitRing, done, compareFunction, refStats);
                }
                catch (e) {
                    console.error(e);
                    done(false);
                }
            }
        };
        request.onerror = function () {
            console.error("Network error during test load.");
            done(false);
        }

        request.send(null);
    }
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

    seed = 1;
    var referenceImage;

    let onLoadFileError = function(request, exception) {
        console.error("Failed to retrieve " + url + ".", exception);
        done(false);
    };

    var onloadedRef = function (data, responseURL) {
        if (typeof (data) !== "string") {
            throw new Error("Can't get string for statistics.");
        }

        loadPlayground(test, done, index, referenceImage, compare, JSON.parse(data));
    }


    var onload = function(data, responseURL) {
        if (typeof (data) === "string") {
            throw new Error("Decode Image from string data not yet implemented.");
        }

        referenceImage = TestUtils.decodeImage(data);

        const profileName = ("app:///Reference/" + test.referenceImage).replace(".png", ".json");
        BABYLON.Tools.LoadFile(profileName, onloadedRef, undefined, undefined, /*useArrayBuffer*/false, onLoadFileError);
    };

    if (generateReferences) {
        loadPlayground(test, done, index, undefined, saveRenderedResult);
    } else {
        // run test and image comparison
        const url = "app:///Reference/" + test.referenceImage;
        BABYLON.Tools.LoadFile(url, onload, undefined, undefined, /*useArrayBuffer*/true, onLoadFileError);
    }
}

engine = new BABYLON.NativeEngine();
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
    },
    removeEventListener: function () { }
}

var xhr = new XMLHttpRequest();
xhr.open("GET", "app:///Scripts/config.json", true);

xhr.addEventListener("readystatechange", function() {
    if (xhr.status === 200) {
        config = JSON.parse(xhr.responseText);

        // Run tests
        var index = 0;
        var recursiveRunTest = function(i) {
            runTest(i, function(status) {
                if (!status) {
                    TestUtils.exit(-1);
                    return;
                }
                i++;
                if (justOnce || i >= config.tests.length) {
                    engine.dispose();
                    TestUtils.exit(0);
                    return;
                }
                recursiveRunTest(i);
            });
        }

        recursiveRunTest(index);
    }
}, false);


BABYLON.Tools.LoadFile("https://raw.githubusercontent.com/CedricGuillemet/dump/master/droidsans.ttf", (data) => {
    _native.NativeCanvas.loadTTFAsync("droidsans", data).then(function () {
        _native.RootUrl = "https://playground.babylonjs.com";
        console.log("Starting");
        TestUtils.setTitle("Starting Native Validation Tests");
        TestUtils.updateSize(testWidth, testHeight);
        xhr.send();
    });
}, undefined, undefined, true);
