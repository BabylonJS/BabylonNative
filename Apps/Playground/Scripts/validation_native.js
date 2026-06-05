(function () {
    let currentScene;
    let config;
    const opts = (typeof _playgroundOptions === "object" && _playgroundOptions) ? _playgroundOptions : {};
    const justOnce = !!opts.runOnce;
    const saveResult = (typeof opts.saveResults === "boolean") ? opts.saveResults : true;
    const testWidth = 600;
    const testHeight = 400;
    const generateReferences = !!opts.generateReferences;
    const breakOnFail = !!opts.breakOnFail;
    const listTests = !!opts.listTests;
    const includeExcluded = !!opts.includeExcluded;
    const testFilters = Array.isArray(opts.testFilters) ? opts.testFilters.map(s => String(s).toLowerCase()) : [];
    const testIndices = Array.isArray(opts.testIndices) ? opts.testIndices.map(n => +n) : [];
    // CLI --capture=N: 1-based frame index at which to call
    // TestUtils.captureNextFrame() for every executed test. The runner
    // extends each test's render budget so the .rdc finalizes.
    const cliCaptureFrame = (typeof opts.captureFrame === "number" && opts.captureFrame > 0) ? (opts.captureFrame | 0) : 0;
    // Frames after the trigger to let RenderDoc finalize the .rdc.
    const POST_CAPTURE_FRAMES = 5;

    function shouldRunTest(test, index) {
        if (testIndices.length > 0 && testIndices.indexOf(index) === -1) {
            return false;
        }
        if (testFilters.length > 0) {
            const title = (test.title || "").toLowerCase();
            for (let i = 0; i < testFilters.length; ++i) {
                if (title.indexOf(testFilters[i]) !== -1) {
                    return true;
                }
            }
            return false;
        }
        return true;
    }

    function failTest(done) {
        if (breakOnFail) {
            // Trigger the JS debugger if attached; on no-debugger runs the
            // host's bx exception filter prints a callstack on the next throw.
            // eslint-disable-next-line no-debugger
            debugger;
        }
        done(false);
    }

    // Per-run counters surfaced as a final summary line on exit.
    let ranCount = 0;
    let passedCount = 0;
    let failedCount = 0;
    let skippedCount = 0;
    let missingRefCount = 0;

    function getExclusionReason(t) {
        if (t.onlyVisual) {
            return "onlyVisual";
        }
        if (t.excludeFromAutomaticTesting) {
            return "excludeFromAutomaticTesting" + (t.reason ? ": " + t.reason : "");
        }
        if (t.excludedGraphicsApis && t.excludedGraphicsApis.includes(TestUtils.getGraphicsApiName())) {
            return "excludedGraphicsApis: " + TestUtils.getGraphicsApiName();
        }
        return null;
    }

    function getSkipReason(t) {
        if (includeExcluded) {
            return null;
        }
        return getExclusionReason(t);
    }

    function logRunSummary() {
        console.log("Run complete. ran=" + ranCount +
                    " passed=" + passedCount +
                    " failed=" + failedCount +
                    " missingRef=" + missingRefCount +
                    " skipped=" + skippedCount);
    }

    const engine = new BABYLON.NativeEngine();
    globalThis.engine = engine;
    engine.getCaps().parallelShaderCompile = undefined;

    // Broaden Babylon's default retry strategy for the test framework: in addition to
    // network drops (status 0, the default trigger), also retry transient HTTP errors
    // (5xx) and rate limits (429). Applies to every BABYLON.Tools.LoadFile request
    // including the snippet fetches in loadPG below and the texture/asset loads
    // initiated from inside each playground's createScene().
    BABYLON.Tools.DefaultRetryStrategy = function (url, request, retryIndex) {
        const maxRetries = 5;
        if (retryIndex >= maxRetries) {
            return -1;
        }
        if (url.indexOf("file:") !== -1) {
            return -1;
        }
        if (request.status === 0 ||
            request.status === 429 ||
            (request.status >= 500 && request.status < 600)) {
            return Math.pow(2, retryIndex) * 500;
        }
        return -1;
    };

    engine.getRenderingCanvas = function () {
        return window;
    }

    engine.getInputElement = function () {
        return 0;
    }

    const canvas = window;
    globalThis.canvas = canvas;

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

    function evaluateScreenshot(test, screenshot, referenceImage, done, compareFunction) {
        let testRes = true;

        if (!test.onlyVisual) {

            const defaultErrorRatio = 2.5;

            if (compareFunction(test, screenshot, referenceImage, test.threshold || 25, test.errorRatio || defaultErrorRatio)) {
                testRes = false;
                console.log("Test '" + (test.title || "(unnamed)") + "' failed");
            } else {
                testRes = true;
                console.log("Test '" + (test.title || "(unnamed)") + "' validated");
            }
        }

        currentScene.dispose();
        currentScene = null;
        engine.setHardwareScalingLevel(1);

        // This is necessary because of https://github.com/BabylonJS/Babylon.js/pull/15217 so that each test starts fresh.
        engine.releaseEffects();

        done(testRes);
    }

    function evaluate(test, referenceImage, done, compareFunction) {
        TestUtils.getFrameBufferData(function (screenshot) {
            evaluateScreenshot(test, screenshot, referenceImage, done, compareFunction);
        });
    }

    function processCurrentScene(test, renderImage, done, compareFunction) {
        currentScene.useConstantAnimationDeltaTime = true;
        // Frame at which to read back the framebuffer & validate. This is the
        // test's renderCount (default 1) and determines pass/fail. NOT shifted
        // by --capture.
        const compareFrame = test.renderCount || 1;
        // Frame at which to call TestUtils.captureNextFrame(), or 0 if no
        // capture is requested. CLI --capture=N takes precedence over the
        // per-test "capture" config flag; the legacy per-test flag triggers
        // at compareFrame.
        const captureFrame = cliCaptureFrame > 0
            ? cliCaptureFrame
            : (test.capture ? compareFrame : 0);
        // Stop after this many frames. With --capture we keep rendering past
        // compareFrame so RenderDoc can finalize the .rdc.
        const stopFrame = captureFrame > 0
            ? Math.max(compareFrame, captureFrame + POST_CAPTURE_FRAMES)
            : compareFrame;

        let frameIndex = 0;
        let stopped = false;
        let pendingScreenshot = null;
        let evaluated = false;

        const runEvaluation = function (screenshot) {
            if (evaluated) {
                return;
            }
            evaluated = true;
            evaluateScreenshot(test, screenshot, renderImage, done, compareFunction);
        };

        // Babylon's Scene.executeWhenReady gives up after Scene.onReadyTimeoutDuration
        // (default 120s): once that elapses it fires onReadyTimeoutObservable and
        // silently drops the executeWhenReady callback. Some validation scenes load
        // very large assets (e.g. the EXR Loader's 3240x4800 RGBA32F image, whose
        // gamma-correct CPU mip generation takes ~3 min under ASAN on the 2-core CI
        // runner), which legitimately exceeds 120s. Without this the callback is
        // dropped, the render loop never starts, and the test hangs until the CI
        // job times out. Extend the budget generously and convert a genuine
        // never-ready scene into a fast test failure instead of a silent hang.
        currentScene.onReadyTimeoutDuration = 10 * 60 * 1000;
        currentScene.onReadyTimeoutObservable.addOnce(function () {
            console.error("Scene '" + (test.title || "?") + "' did not become ready within " +
                (currentScene.onReadyTimeoutDuration / 1000) + "s.");
            failTest(done);
        });

        currentScene.executeWhenReady(function () {
            if (currentScene.activeCamera && currentScene.activeCamera.useAutoRotationBehavior) {
                currentScene.activeCamera.useAutoRotationBehavior = false;
            }
            engine.runRenderLoop(function () {
                try {
                    frameIndex++;

                    if (captureFrame > 0 && frameIndex === captureFrame && TestUtils.captureNextFrame) {
                        TestUtils.captureNextFrame();
                    }

                    currentScene.render();

                    if (frameIndex === compareFrame) {
                        // Queue the framebuffer readback. The callback runs
                        // asynchronously; safe to dispose the scene from it
                        // but only after stopRenderLoop() has been called.
                        TestUtils.getFrameBufferData(function (data) {
                            if (stopped) {
                                runEvaluation(data);
                            } else {
                                pendingScreenshot = data;
                            }
                        });
                    }

                    if (frameIndex >= stopFrame && !stopped) {
                        stopped = true;
                        engine.stopRenderLoop();
                        if (pendingScreenshot !== null) {
                            // Defer dispose to next tick so it runs outside
                            // this runRenderLoop iteration.
                            const data = pendingScreenshot;
                            pendingScreenshot = null;
                            setTimeout(function () { runEvaluation(data); }, 0);
                        }
                    }
                }
                catch (e) {
                    console.error(e);
                    failTest(done);
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
                    failTest(done);
                });
        }
        else if (test.playgroundId) {
            if (test.playgroundId[0] !== "#" || test.playgroundId.indexOf("#", 1) === -1) {
                test.playgroundId += "#0";
            }

            const snippetUrl = "https://snippet.babylonjs.com";
            const pgRoot = "https://playground.babylonjs.com";

            const loadPG = function () {
                const url = snippetUrl + test.playgroundId.replace(/#/g, "/");
                BABYLON.Tools.LoadFile(
                    url,
                    function (responseText) {
                        try {
                            const snippet = JSON.parse(responseText);
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
                                .replace(/'\/textures\//g, "'" + pgRoot + "/textures/")
                                .replace(/"textures\//g, '"' + pgRoot + "/textures/")
                                .replace(/'textures\//g, "'" + pgRoot + "/textures/")
                                .replace(/\/scenes\//g, pgRoot + "/scenes/")
                                .replace(/"scenes\//g, '"' + pgRoot + "/scenes/")
                                .replace(/'scenes\//g, "'" + pgRoot + "/scenes/")
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
                                    failTest(done);
                                });
                            } else {
                                // Handle if createScene returns a scene
                                processCurrentScene(test, referenceImage, done, compareFunction);
                            }
                        }
                        catch (e) {
                            console.error("Failed to evaluate playground snippet " + test.playgroundId + ": " + e);
                            failTest(done);
                        }
                    },
                    undefined,  // onProgress
                    undefined,  // database
                    false,      // useArrayBuffer (snippet response is JSON text)
                    function (request, exception) {
                        const status = request ? (request.status + " " + request.statusText) : "no response";
                        console.error("Failed to load playground snippet " + test.playgroundId + " after retries: " + status);
                        if (exception) {
                            console.error(exception);
                        }
                        failTest(done);
                    }
                );
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
                        processCurrentScene(test, renderImage, done, compareFunction);
                    }
                    catch (e) {
                        console.error(e);
                        failTest(done);
                    }
                }
            };
            request.onerror = function () {
                console.error("Network error during test load.");
                failTest(done);
            }

            request.send(null);
        }
    }
    function runTest(index, done) {
        if (index >= config.tests.length) {
            done(false);
        }

        const test = config.tests[index];
        const testInfo = "Running " + test.title;
        console.log(testInfo);
        TestUtils.setTitle(testInfo);

        seed = 1;

        if (generateReferences) {
            loadPlayground(test, done, undefined, saveRenderedResult);
        } else {
            // Config validation: missing 'referenceImage' field is a permanent
            // catalog error (not a runtime asset-missing case), so short-circuit
            // before issuing the load. onlyVisual tests skip pixel comparison
            // so they don't need the reference image to exist.
            if (!test.onlyVisual && !test.referenceImage) {
                console.error("MISSING_REFERENCE_IMAGE: Test '" + (test.title || "(unnamed)") +
                              "' has no 'referenceImage' field in config.json - cannot run pixel comparison.");
                missingRefCount++;
                failTest(done);
                return;
            }

            // run test and image comparison
            const url = "app:///ReferenceImages/" + test.referenceImage;

            const onLoadFileError = function (request, exception) {
                // Reference-image load failures (missing file on disk, etc.)
                // arrive here via JsRuntimeHost's XHR error event +
                // BABYLON.Tools.LoadFile's onLoadFileError callback. Tag with
                // MISSING_REFERENCE_IMAGE: so CI greps still match.
                console.error("MISSING_REFERENCE_IMAGE: Test '" + (test.title || "(unnamed)") +
                              "' failed to load reference at " + url + ". " +
                              (exception ? exception : "(no exception details)"));
                missingRefCount++;
                failTest(done);
            };

            const onload = function (data, responseURL) {
                if (typeof (data) === "string") {
                    throw new Error("Decode Image from string data not yet implemented.");
                }

                const referenceImage = TestUtils.decodeImage(data);
                loadPlayground(test, done, referenceImage, compare);
            };

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

            if (listTests) {
                // Canonical TSV: index<TAB>title<TAB>referenceImage<TAB>exclusionReason.
                // exclusionReason reflects config state (ignores --include-excluded)
                // so the listing is the same regardless of run flags.
                for (let i = 0; i < config.tests.length; ++i) {
                    const t = config.tests[i];
                    const reason = getExclusionReason(t) || "";
                    console.log(i + "\t" + (t.title || "") + "\t" + (t.referenceImage || "") + "\t" + reason);
                }
                engine.dispose();
                TestUtils.exit(0);
                return;
            }

            // Run tests
            const recursiveRunTest = function (i) {
                // Skip filtered-out tests cheaply (don't count toward --once
                // and don't re-init the engine).
                //
                // Skipped tests (excludeFromAutomaticTesting / onlyVisual /
                // excludedGraphicsApis) are logged loudly when a filter is
                // active so the user sees that --test "X" matched but was
                // skipped. Filter mismatches stay silent to avoid noise on
                // unfiltered runs.
                while (i < config.tests.length) {
                    const t = config.tests[i];
                    const matchesFilter = shouldRunTest(t, i);
                    if (!matchesFilter) {
                        i++;
                        continue;
                    }
                    const reason = getSkipReason(t);
                    if (reason !== null) {
                        console.log("Skipping '" + (t.title || "(unnamed)") + "' -- " + reason);
                        skippedCount++;
                        i++;
                        continue;
                    }
                    break;
                }
                if (i >= config.tests.length) {
                    logRunSummary();
                    engine.dispose();
                    TestUtils.exit(failedCount > 0 ? -1 : 0);
                    return;
                }
                const currentTitle = config.tests[i].title || "(unnamed)";
                runTest(i, function (status) {
                    ranCount++;
                    if (!status) {
                        failedCount++;
                        // failTest() already triggered the debugger before
                        // reaching this callback; no second `debugger` here.
                        logRunSummary();
                        TestUtils.exit(-1);
                        return;
                    }
                    passedCount++;
                    i++;
                    if (justOnce || i >= config.tests.length) {
                        logRunSummary();
                        engine.dispose();
                        TestUtils.exit(0);
                        return;
                    }
                    // Defer next iteration to avoid blowing Chakra's
                    // recursion stack on long test lists.
                    setTimeout(function () { recursiveRunTest(i); }, 0);
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
