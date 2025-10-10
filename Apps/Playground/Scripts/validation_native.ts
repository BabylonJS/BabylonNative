/// <reference path="./testutils.d.ts" />
/// <reference path="../../node_modules/babylonjs/babylon.module.d.ts" />

import { NativeEngine, ArcRotateCamera, Scene, SceneLoader, Tools, WebRequest } from "babylonjs";

declare const _native: any;

interface TestConfig {
    title: string;
    playgroundId: string;
    sceneFolder?: string;
    sceneFilename?: string;
    referenceImage: string;
    excludedGraphicsApis?: string[];
    renderCount?: number;
    errorRatio?: number;
    onlyVisual?: boolean;
    excludeFromAutomaticTesting?: boolean;
    comment?: string;
    replace?: string;
    replaceUrl?: string;
    specificRoot?: string;
    scriptToRun?: string;
    functionToCall?: string;
    rootPath?: string;
    threshold?: number;
}

interface ConfigFile {
    root: string;
    tests: TestConfig[];
}
type compareFunctionType = (test: TestConfig, renderData: Uint8Array, threshold: number, errorRatio: number, referenceImage?: Uint8Array) => boolean;

(function () {
    let currentScene: any;
    let config: ConfigFile;
    const justOnce = false;
    const saveResult = true;
    const testWidth = 600;
    const testHeight = 400;
    const generateReferences = false;

    const engine = new NativeEngine();
    engine.getCaps().parallelShaderCompile = undefined;

    engine.getRenderingCanvas = function () {
        return null;
    }

    engine.getInputElement = function () {
        return null;
    }

    const canvas = window;

    // Random replacement
    let seed = 1;
    Math.random = function () {
        const x = Math.sin(seed++) * 10000;
        return x - Math.floor(x);
    }

    function compare(test: TestConfig, renderData: Uint8Array, threshold: number, errorRatio: number, referenceImage?: Uint8Array): boolean {
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

    function saveRenderedResult(test: TestConfig, renderData: Uint8Array) {
        const width = testWidth / engine.getHardwareScalingLevel();
        const height = testHeight / engine.getHardwareScalingLevel();
        TestUtils.writePNG(renderData, width, height, TestUtils.getOutputDirectory() + "/Results/" + test.referenceImage);
        return false; // no error
    }

    function evaluate(test: TestConfig, done: (done: boolean) => void, compareFunction: compareFunctionType, referenceImage?: Uint8Array) {
        TestUtils.getFrameBufferData(function (screenshot: Uint8Array) {
            let testRes = true;

            if (!test.onlyVisual) {

                const defaultErrorRatio = 2.5;

                if (compareFunction(test, screenshot, test.threshold || 25, test.errorRatio || defaultErrorRatio, referenceImage)) {
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

    function processCurrentScene(test: TestConfig, done: (done: boolean) => void, compareFunction: compareFunctionType, renderImage?: Uint8Array) {
        currentScene.useConstantAnimationDeltaTime = true;
        let renderCount = test.renderCount || 1;

        currentScene.executeWhenReady(function () {
            const activeCamera = currentScene.activeCamera;
            if (activeCamera && activeCamera instanceof ArcRotateCamera && (activeCamera as ArcRotateCamera).useAutoRotationBehavior) {
                (activeCamera as ArcRotateCamera).useAutoRotationBehavior = false;
            }
            engine.runRenderLoop(function () {
                try {
                    currentScene.render();
                    renderCount--;

                    if (renderCount === 0) {
                        engine.stopRenderLoop();
                        evaluate(test, done, compareFunction, renderImage);
                    }
                }
                catch (e) {
                    console.error(e);
                    done(false);
                }
            });
        }, true);
    }

    function loadPlayground(test: TestConfig, done: (done: boolean) => void, compareFunction: compareFunctionType, referenceImage?: Uint8Array) {
        if (test.sceneFolder) {
            SceneLoader.Load(config.root + test.sceneFolder, test.sceneFilename, engine, function (newScene: Scene) {
                currentScene = newScene;
                processCurrentScene(test, done, compareFunction, referenceImage);
            },
                null,
                function (loadedScene: Scene, msg: string) {
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
                            let code = JSON.parse(snippet.jsonPayload).code.toString()
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
                                currentScene.then(function (scene: Scene) {
                                    currentScene = scene;
                                    processCurrentScene(test, done, compareFunction, referenceImage);
                                }).catch(function (e: any) {
                                    console.error(e);
                                    onError();
                                })
                            } else {
                                // Handle if createScene returns a scene
                                processCurrentScene(test, done, compareFunction, referenceImage);
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
                Tools.BaseUrl = config.root + test.specificRoot;
            }

            const request = new XMLHttpRequest();
            request.open('GET', config.root + test.scriptToRun, true);

            request.onreadystatechange = function () {
                if (request.readyState === 4) {
                    try {
                        request.onreadystatechange = null;

                        let scriptToRun = request.responseText.replace(/..\/..\/assets\//g, config.root + "/Assets/");
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
                        processCurrentScene(test, done, compareFunction);//, renderImage);
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
    function runTest(index: number, done: (result: boolean) => void) {
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
            loadPlayground(test, done, saveRenderedResult);
        } else {
            const onLoadFileError = function(request?: WebRequest, exception?: any) {
                console.error("Failed to retrieve " + url + ".", exception);
                done(false);
            };

            const onload = function(data: string | ArrayBuffer, responseURL?: string) {
                if (typeof (data) === "string") {
                    throw new Error("Decode Image from string data not yet implemented.");
                }

                const referenceImage = TestUtils.decodeImage(data);
                loadPlayground(test, done, compare, referenceImage);
            };

            // run test and image comparison
            const url = "app:///ReferenceImages/" + test.referenceImage;
            Tools.LoadFile(url, onload, undefined, undefined, /*useArrayBuffer*/true, onLoadFileError);
        }
    }

    class OffscreenCanvas {
        width: number;
        height: number;
        constructor(width: number, height: number) {
            this.width = width;
            this.height = height;
        }
        getContext(type: string) {
            return {
                fillRect: function (x: number, y: number, w: number, h: number) { },
                measureText: function (text: string) { return 8; },
                fillText: function (text: string, x: number, y: number) { }
            };
        }
    }

    const document: any = {
        createElement: function (type: string): any {
            if (type === "canvas") {
                return new OffscreenCanvas(64, 64);
            }
            return {} as any;
        },
        removeEventListener: function () { }
    }

    const xhr = new XMLHttpRequest();
    xhr.open("GET", "app:///Scripts/config.json", true);

    xhr.addEventListener("readystatechange", function () {
        if (xhr.status === 200) {
            config = JSON.parse(xhr.responseText);

            // Run tests
            const recursiveRunTest = function (i: number) {
                runTest(i, function (status: boolean) {
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


    Tools.LoadFile("https://raw.githubusercontent.com/CedricGuillemet/dump/master/droidsans.ttf", (data: string | ArrayBuffer) => {
        _native.Canvas.loadTTFAsync("droidsans", data).then(function () {
            _native.RootUrl = "https://playground.babylonjs.com";
            console.log("Starting");
            TestUtils.setTitle("Starting Native Validation Tests");
            TestUtils.updateSize(testWidth, testHeight);
            xhr.send();
        });
    }, undefined, undefined, true);
})();