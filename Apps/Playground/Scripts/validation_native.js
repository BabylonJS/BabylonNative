(function () {
    let currentScene;
    let config;
    const justOnce = false;
    const saveResult = true;
    const testWidth = 600;
    const testHeight = 400;
    const generateReferences = false;

    const engine = new BABYLON.NativeEngine();
    engine.getCaps().parallelShaderCompile = undefined;

    engine.getRenderingCanvas = function () {
        return window;
    }

    engine.getInputElement = function () {
        return 0;
    }

    const canvas = window;

    // Random replacement
    let seed = 1;
    Math.random = function () {
        const x = Math.sin(seed++) * 10000;
        return x - Math.floor(x);
    }

    function compare(test, renderData, referenceImage, threshold, errorRatio) {
        const referenceData = TestUtils.getImageData(referenceImage);
        if (referenceData.length != renderData.length) {
            throw new Error(`Reference data length (${referenceData.length}) must match render data length (${renderData.length})`);
        }

        const size = renderData.length;
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
        } else {
            console.log("No pixel difference!");
        }

        const error = (differencesCount * 100) / (size / 4) > errorRatio;

        const width = testWidth / engine.getHardwareScalingLevel();
        const height = testHeight / engine.getHardwareScalingLevel();

        if (error) {
            TestUtils.writePNG(referenceData, width, height, TestUtils.getOutputDirectory() + "/Errors/" + test.referenceImage);
        }
        if (saveResult || error) {
            TestUtils.writePNG(renderData, width, height, TestUtils.getOutputDirectory() + "/Results/" + test.referenceImage);
        }
        return error;
    }

    function saveRenderedResult(test, renderData) {
        const width = testWidth / engine.getHardwareScalingLevel();
        const height = testHeight / engine.getHardwareScalingLevel();
        TestUtils.writePNG(renderData, width, height, TestUtils.getOutputDirectory() + "/Results/" + test.referenceImage);
        return false; // no error
    }

    function evaluate(test, referenceImage, done, compareFunction) {
        TestUtils.getFrameBufferData(function (screenshot) {
            let testRes = true;

            if (!test.onlyVisual) {

                const defaultErrorRatio = 2.5;

                if (compareFunction(test, screenshot, referenceImage, test.threshold || 25, test.errorRatio || defaultErrorRatio)) {
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

            // This is necessary because of https://github.com/BabylonJS/Babylon.js/pull/15217 so that each test starts fresh.
            engine.releaseEffects();

            done(testRes);
        });
    }

    function processCurrentScene(test, renderImage, done, compareFunction) {
        currentScene.useConstantAnimationDeltaTime = true;
        let renderCount = test.renderCount || 1;

        currentScene.executeWhenReady(function () {
            if (currentScene.activeCamera && currentScene.activeCamera.useAutoRotationBehavior) {
                currentScene.activeCamera.useAutoRotationBehavior = false;
            }
            engine.runRenderLoop(function () {
                try {
                    currentScene.render();
                    renderCount--;

                    if (renderCount === 0) {
                        engine.stopRenderLoop();
                        evaluate(test, renderImage, done, compareFunction);
                    }
                }
                catch (e) {
                    console.error(e);
                    done(false);
                }
            });
        }, true);
    }

    function loadPlayground(test, done, referenceImage, compareFunction) {
        if (test.sceneFolder) {
            BABYLON.SceneLoader.Load(config.root + test.sceneFolder, test.sceneFilename, engine, function (newScene) {
                currentScene = newScene;
                processCurrentScene(test, referenceImage, done, compareFunction);
            },
                null,
                function (loadedScene, msg) {
                    console.error(msg);
                    done(false);
                });
        }
        else if (test.playgroundId) {
            if (test.playgroundId[0] !== "#" || test.playgroundId.indexOf("#", 1) === -1) {
                test.playgroundId += "#0";
            }

            const snippetUrl = "https://snippet.babylonjs.com";
            const pgRoot = "https://playground.babylonjs.com";

            const retryTime = 500;
            const maxRetry = 5;
            let retry = 0;

            const onError = function () {
                retry++;
                if (retry < maxRetry) {
                    setTimeout(function () {
                        loadPG();
                    }, retryTime);
                }
                else {
                    // Fail the test, something wrong happen
                    console.log("Running the playground failed.");
                    done(false);
                }
            }

            const loadPG = function () {
                const xmlHttp = new XMLHttpRequest();
                xmlHttp.addEventListener("readystatechange", function () {
                    if (xmlHttp.readyState === 4) {
                        try {
                            xmlHttp.onreadystatechange = null;
                            const snippet = JSON.parse(xmlHttp.responseText);
                            let code = JSON.parse(snippet.jsonPayload).code.toString();

                            // Check if this is a v2 manifest and extract the entry file's code
                            // TODO: Handle multi-file playgrounds
                            try {
                                const manifestPayload = JSON.parse(code);
                                if (manifestPayload.v === 2) {
                                    code = manifestPayload.files[manifestPayload.entry]
                                        .replace(/export +default +/g, "")
                                        .replace(/export +/g, "");
                                }
                            } catch (e) {
                                // Not a manifest, proceed as usual
                            }

                            code = code
                                .replace(/"\/textures\//g, '"' + pgRoot + "/textures/")
                                .replace(/"textures\//g, '"' + pgRoot + "/textures/")
                                .replace(/\/scenes\//g, pgRoot + "/scenes/")
                                .replace(/"scenes\//g, '"' + pgRoot + "/scenes/")
                                .replace(/"\.\.\/\.\.https/g, '"' + "https")
                                .replace("http://", "https://");

                            if (test.replace) {
                                const split = test.replace.split(",");
                                for (let i = 0; i < split.length; i += 2) {
                                    const source = split[i].trim();
                                    const destination = split[i + 1].trim();
                                    code = code.replace(source, destination);
                                }
                            }

                            currentScene = eval(code + "\r\ncreateScene(engine)");

                            if (currentScene.then) {
                                // Handle if createScene returns a promise
                                currentScene.then(function (scene) {
                                    currentScene = scene;
                                    processCurrentScene(test, referenceImage, done, compareFunction);
                                }).catch(function (e) {
                                    console.error(e);
                                    onError();
                                })
                            } else {
                                // Handle if createScene returns a scene
                                processCurrentScene(test, referenceImage, done, compareFunction);
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

            const request = new XMLHttpRequest();
            request.open('GET', config.root + test.scriptToRun, true);

            request.onreadystatechange = function () {
                if (request.readyState === 4) {
                    try {
                        request.onreadystatechange = null;

                        const scriptToRun = request.responseText.replace(/..\/..\/assets\//g, config.root + "/Assets/");
                        scriptToRun = scriptToRun.replace(/..\/..\/Assets\//g, config.root + "/Assets/");
                        scriptToRun = scriptToRun.replace(/\/assets\//g, config.root + "/Assets/");

                        if (test.replace) {
                            const split = test.replace.split(",");
                            for (let i = 0; i < split.length; i += 2) {
                                const source = split[i].trim();
                                const destination = split[i + 1].trim();
                                scriptToRun = scriptToRun.replace(source, destination);
                            }
                        }

                        if (test.replaceUrl) {
                            const split = test.replaceUrl.split(",");
                            for (let i = 0; i < split.length; i++) {
                                const source = split[i].trim();
                                const regex = new RegExp(source, "g");
                                scriptToRun = scriptToRun.replace(regex, config.root + test.rootPath + source);
                            }
                        }

                        currentScene = eval(scriptToRun + test.functionToCall + "(engine)");
                        processCurrentScene(test, renderImage, done, compareFunction);
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

        const test = config.tests[index];
        if (test.onlyVisual || test.excludeFromAutomaticTesting) {
            done(true);
            return;
        }
        if (test.excludedGraphicsApis && test.excludedGraphicsApis.includes(TestUtils.getGraphicsApiName())) {
            done(true);
            return;
        }
        const testInfo = "Running " + test.title;
        console.log(testInfo);
        TestUtils.setTitle(testInfo);

        seed = 1;

        if (generateReferences) {
            loadPlayground(test, done, undefined, saveRenderedResult);
        } else {
            const onLoadFileError = function (request, exception) {
                console.error("Failed to retrieve " + url + ".", exception);
                done(false);
            };

            const onload = function (data, responseURL) {
                if (typeof (data) === "string") {
                    throw new Error("Decode Image from string data not yet implemented.");
                }

                const referenceImage = TestUtils.decodeImage(data);
                loadPlayground(test, done, referenceImage, compare);
            };

            // run test and image comparison
            const url = "app:///ReferenceImages/" + test.referenceImage;
            BABYLON.Tools.LoadFile(url, onload, undefined, undefined, /*useArrayBuffer*/true, onLoadFileError);
        }
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

    const xhr = new XMLHttpRequest();
    xhr.open("GET", "app:///Scripts/config.json", true);

    xhr.addEventListener("readystatechange", function () {
        if (xhr.status === 200) {
            config = JSON.parse(xhr.responseText);

            // Run tests
            const recursiveRunTest = function (i) {
                runTest(i, function (status) {
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

            recursiveRunTest(0);
        }
    }, false);


    BABYLON.Tools.LoadFile("https://raw.githubusercontent.com/CedricGuillemet/dump/master/droidsans.ttf", (data) => {
        _native.Canvas.loadTTFAsync("droidsans", data).then(function () {
            _native.RootUrl = "https://playground.babylonjs.com";
            console.log("Starting");
            TestUtils.setTitle("Starting Native Validation Tests");
            TestUtils.updateSize(testWidth, testHeight);
            xhr.send();
        });
    }, undefined, undefined, true);
})();