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
    const stopOnFirstFailure = !!opts.stopOnFirstFailure;
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

    // Emitted after a pixel-comparison failure to make triage faster. Prints the
    // rendered/diff PNG paths plus a re-run command. For scenes fetched from the
    // snippet server it also notes that assets/fonts arrive over the network, so
    // async load timing is one possible cause -- but it is only one of several,
    // and it is cheap to rule out: a timing flake varies run to run, while a real
    // regression reproduces with an identical pixel-difference count. Say that
    // explicitly. The previous wording called such diffs "often a transient flake",
    // which led to a genuine, perfectly reproducible motion-blur regression being
    // waved off for two weeks of nightlies.
    function logFailureDiagnostics(test) {
        const outDir = TestUtils.getOutputDirectory();
        if (test.referenceImage) {
            console.log(`  Rendered result: ${outDir}/Results/${test.referenceImage}`);
            console.log(`  Diff overlay:    ${outDir}/Errors/${test.referenceImage}`);
        }
        if (test.playgroundId) {
            console.log(`  Note: this test loads playgroundId ${test.playgroundId} from the snippet server and pulls GUI/assets/fonts over the network, so async asset/font-load timing is one possible cause of a pixel diff.`);
        }
        console.log("  Re-run in isolation; an identical pixel count on repeat runs means a real regression, not a timing flake:");
        console.log(`    Playground --headless --once --test "${test.title || ""}" app:///Scripts/validation_native.js`);
    }

    // Per-run counters surfaced as a final summary line on exit.
    let ranCount = 0;
    let passedCount = 0;
    let failedCount = 0;
    let skippedCount = 0;
    let missingRefCount = 0;
    const failedTitles = [];

    // BABYLON classes exposing a static ForceGLSL, discovered lazily once.
    let forceGlslOwners;

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
        if (failedTitles.length > 0) {
            console.log("Failed tests (" + failedTitles.length + "):");
            for (let n = 0; n < failedTitles.length; n++) {
                console.log("  - " + failedTitles[n]);
            }
        }
    }

    // Backend detection: the NativeDawn (WebGPU) backend pre-creates a
    // WebGPUEngine (aliased as BABYLON.NativeEngine) and drives its render loop
    // from the host frame pump, promoting it to globalThis.__dawnEngine once
    // initAsync (async, driven by host frames) completes. Reuse that same
    // instance so runRenderLoop targets the engine the host actually presents,
    // rather than constructing a second one.
    //
    // Detect the backend via the plugin-specific `_nativeDawnClear` global (the
    // bgfx NativeEngine backend has neither it nor navigator.gpu). Note `_native`
    // exists on BOTH backends here -- the Canvas polyfill provides it -- so it
    // can't be used to tell them apart.
    const isDawn = (typeof globalThis._nativeDawnClear === "function");
    const engine = isDawn
        ? (globalThis.__dawnEngine || globalThis.__dawnPendingEngine || new BABYLON.NativeEngine())
        : new BABYLON.NativeEngine();
    globalThis.engine = engine;
    // parallelShaderCompile is a WebGL2 (KHR_parallel_shader_compile) cap; on
    // Dawn the caps table isn't populated until initAsync completes, so this is
    // applied in the Dawn start path below once the engine is ready.
    if (!isDawn) {
        engine.getCaps().parallelShaderCompile = undefined;
    }

    // The default HTML loading screen pokes at DOM nodes (document head, an
    // <img> logo with a network fetch) that don't meaningfully exist in this
    // headless host. It's a pure overlay and never part of the captured 3D
    // frame, so disable it. Prevents SceneLoader (glTF imports) from calling
    // engine.displayLoadingUI().
    if (BABYLON.SceneLoader) {
        BABYLON.SceneLoader.ShowLoadingScreen = false;
    }
    engine.displayLoadingUI = function () { };
    engine.hideLoadingUI = function () { };

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

    // Random replacement. Deterministic so reference images are reproducible.
    // Reinstalled per-test (see runTest) because some playgrounds overwrite
    // Math.random with their own closure -- e.g. "Selection outline layer with
    // instances" (#UR9706#0) does `window.Math.random = ... window.seed ...`,
    // leaving a global RNG that the harness's `seed = 1` reset can no longer
    // touch. Left in place, every later test (notably GPU particle systems,
    // whose random textures are filled from Math.random) gets shifted random
    // values and drifts across the pixel-diff threshold.
    let seed = 1;
    const deterministicRandom = function () {
        const x = Math.sin(seed++) * 10000;
        return x - Math.floor(x);
    };
    Math.random = deterministicRandom;

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
                const pixel = index / 4;
                const width = Math.round(testWidth / engine.getHardwareScalingLevel());
                console.log(`First pixel off at ${index} (pixel ${pixel} @ x=${pixel % width}, y=${Math.floor(pixel / width)}): Value: (${renderData[index]}, ${renderData[index + 1]}, ${renderData[index + 2]}) - Expected: (${referenceData[index]}, ${referenceData[index + 1]}, ${referenceData[index + 2]}) `);
            }

            referenceData[index] = 255;
            referenceData[index + 1] *= 0.5;
            referenceData[index + 2] *= 0.5;
            differencesCount++;
        }

        if (differencesCount) {
            const pixelCount = size / 4;
            const diffRatio = (differencesCount * 100) / pixelCount;
            console.log(`Pixel difference: ${differencesCount} / ${pixelCount} pixels (${diffRatio.toFixed(3)}%, per-channel threshold ${threshold}); allowed errorRatio ${errorRatio}%.`);
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
                console.log("Test '" + (test.title || "(unnamed)") + "' failed (pixel comparison)");
                logFailureDiagnostics(test);
            } else {
                testRes = true;
                console.log("Test '" + (test.title || "(unnamed)") + "' validated");
            }
        }

        currentScene.dispose();
        currentScene = null;

        // A test can leave extra scenes behind (an async load that created its own scene, a scene
        // whose creation promise resolved after validation, ...). They stay registered on the
        // reused engine and keep their resources alive, so dispose them here.
        const strayScenes = engine.scenes.slice();
        for (let i = 0; i < strayScenes.length; ++i) {
            strayScenes[i].dispose();
        }

        engine.setHardwareScalingLevel(1);

        // Reset render state that persists on the reused engine so each test starts fresh.
        // A test that leaves the stencil test enabled or a scissor rect set would otherwise
        // corrupt later tests (e.g. the glow-layer test).
        engine.setStencilBuffer(false);
        engine.disableScissor();

        // This is necessary because of https://github.com/BabylonJS/Babylon.js/pull/15217 so that each test starts fresh.
        engine.releaseEffects();

        // Textures are cached on the engine by URL (BaseTexture._getFromCache), and the cache key
        // covers only url/noMipmap/isCube -- not the load-time options. A test that leaves a
        // reference behind (e.g. assigning one texture to both scene.environmentTexture and a
        // material's reflectionTexture) keeps its internal texture in that cache across
        // scene.dispose(), so a later test loading the same URL silently reuses the *previous*
        // test's texture along with its prefiltering/irradiance settings. Release whatever is
        // left so every test loads its own textures and results do not depend on run order.
        const leakedTextures = engine.getLoadedTexturesCache();
        for (let i = leakedTextures.length - 1; i >= 0; --i) {
            engine._releaseTexture(leakedTextures[i]);
        }
        engine.clearInternalTexturesCache();

        // SceneLoader.OnPluginActivatedObservable is global and outlives the scene. Snippets use it
        // to configure the glTF loader (animationStartMode, compileMaterials, ...) and never
        // unregister, so without this every later glTF test would inherit those settings. The
        // browser harness reloads the page per test and never sees this; here the engine is reused.
        BABYLON.SceneLoader.OnPluginActivatedObservable.clear();

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

                            const pgCode = code + "\r\ncreateScene(engine)";
                            // Defer scene construction to a fresh macrotask so
                            // eval()/createScene() run at a shallow native-stack
                            // depth instead of nested inside the native snippet
                            // load callback. Deep scenes otherwise pile onto the
                            // native XHR dispatch frames and can overflow engines
                            // with a small C stack (e.g. QuickJS).
                            setTimeout(async function () {
                                // eslint-disable-next-line no-unused-vars
                                var name = ""; // see the note on the scriptToRun eval below
                                try {
                                    // Runs before the first await, so the eval still happens at the
                                    // shallow stack depth this setTimeout exists to provide.
                                    currentScene = eval(pgCode);

                                    if (currentScene && currentScene.then) {
                                        // Handle if createScene returns a promise. Guard against a
                                        // snippet whose promise never resolves (e.g. a scene whose
                                        // utility-layer executeWhenReady never fires on Native): the
                                        // onReadyTimeout safety net lives inside processCurrentScene
                                        // and only applies AFTER the promise resolves, so without this
                                        // a pending createScene promise hangs the whole suite. Mirror
                                        // onReadyTimeoutDuration and convert it to a fast failure.
                                        // Note: this only fires if the JS event loop keeps running; a
                                        // snippet that blocks the JS thread natively (e.g. manual
                                        // setInterval frame-driving) is not rescued by this.
                                        const createSceneTimeoutMs = 10 * 60 * 1000;
                                        let createSceneTimeoutId;
                                        try {
                                            currentScene = await Promise.race([
                                                currentScene,
                                                new Promise(function (resolve, reject) {
                                                    createSceneTimeoutId = setTimeout(function () {
                                                        reject(new Error("createScene promise for " + test.playgroundId +
                                                            " did not resolve within " + (createSceneTimeoutMs / 1000) + "s."));
                                                    }, createSceneTimeoutMs);
                                                })
                                            ]);
                                        }
                                        finally {
                                            // Always clear it: a pending timer would otherwise keep the
                                            // event loop alive for the full timeout after a scene that
                                            // resolved normally.
                                            clearTimeout(createSceneTimeoutId);
                                        }
                                    }

                                    processCurrentScene(test, referenceImage, done, compareFunction);
                                }
                                catch (e) {
                                    console.error("Failed to evaluate playground snippet " + test.playgroundId + ": " + e);
                                    failTest(done);
                                }
                            }, 0);
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

            // Babylon Native's XMLHttpRequest polyfill only dispatches to
            // addEventListener; assigning the DOM on<event> properties silently
            // does nothing and the load hangs forever.
            let handled = false;
            request.addEventListener('readystatechange', function () {
                if (request.readyState === 4) {
                    try {
                        if (handled) {
                            return;
                        }
                        handled = true;

                        // The polyfill sets readyState=4 before raising 'error',
                        // so a failed fetch reaches here first.
                        if (request.status < 200 || request.status >= 300) {
                            console.error("Failed to load " + test.scriptToRun + ": status " + request.status);
                            failTest(done);
                            return;
                        }

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

                        const scriptCode = scriptToRun + test.functionToCall + "(engine)";
                        // Defer scene construction to a fresh macrotask so
                        // eval()/<functionToCall>() run at a shallow native-stack
                        // depth instead of nested inside the native XHR
                        // completion callback. Deep scenes otherwise pile onto
                        // the native XHR dispatch frames and can overflow engines
                        // with a small C stack (e.g. QuickJS).
                        setTimeout(function () {
                            // Browser scripts sometimes reference `name` without declaring it. In a
                            // page that silently resolves to window.name (""), so the mistake is
                            // invisible there but throws "ReferenceError: name is not defined"
                            // here. eval() below is a *direct* eval, so the evaluated script sees
                            // this function's scope and finds this binding -- same as it would on
                            // the web, without leaking an actual global. (A real global `name`
                            // is not an option: it breaks the Babylon UMD bundles at load time.)
                            // eslint-disable-next-line no-unused-vars
                            var name = "";
                            try {
                                currentScene = eval(scriptCode);
                                processCurrentScene(test, referenceImage, done, compareFunction);
                            }
                            catch (e) {
                                console.error(e);
                                failTest(done);
                            }
                        }, 0);
                    }
                    catch (e) {
                        console.error(e);
                        failTest(done);
                    }
                }
            });
            request.addEventListener('error', function () {
                if (handled) {
                    return;
                }
                handled = true;
                console.error("Network error during test load.");
                failTest(done);
            });

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
        // Reinstall the deterministic RNG: a prior test may have replaced
        // Math.random with its own function (see the definition above), which
        // would make `seed = 1` a no-op and leave later tests non-deterministic.
        Math.random = deterministicRandom;

        // Restore per-test isolation for global Babylon loader state. Some
        // playgrounds add a BABYLON.SceneLoader.OnPluginActivatedObservable
        // observer and never remove it -- e.g. "Yeti" (#QATUCH#32) forces the
        // glTF loader's animationStartMode to ALL. Left in place, every later
        // glTF scene auto-plays EVERY animation group instead of just the first,
        // blending all animations and rendering the wrong animated pose (this is
        // why "GLTF Serializer Skinning and Animation" failed only when a prior
        // test leaked such an observer). Clearing here drops leaked observers; a
        // test's own observer is (re)added later in its own createScene.
        if (BABYLON.SceneLoader && BABYLON.SceneLoader.OnPluginActivatedObservable) {
            BABYLON.SceneLoader.OnPluginActivatedObservable.clear();
        }

        // Reset global engine flags that some playgrounds set and never restore.
        // e.g. "Reverse depth buffer and shadows" (#WL4Q8J#20) and the CSM variant
        // set engine.useReverseDepthBuffer = true; left on, every later test renders
        // with a reversed depth test and depth-sensitive tests fail (e.g. "Sample
        // depth texture" rendered black). A test that needs it re-enables it in its
        // own createScene.
        if (typeof engine.useReverseDepthBuffer !== "undefined") {
            engine.useReverseDepthBuffer = false;
        }

        // Reset snapshot rendering. "FAST snapshot CPU particles" (#AW6Q7E#0)
        // uses BABYLON.SnapshotRenderingHelper.enableSnapshotRendering(), which
        // sets engine.snapshotRendering = true. Snapshot mode caches the render
        // command buffer, so every later test replays the snapshot's draws
        // instead of its own -- GPU particle systems in particular then render
        // stale/shifted output and drift across the pixel-diff threshold. A test
        // that needs snapshot mode re-enables it in its own createScene.
        if (typeof engine.snapshotRendering !== "undefined") {
            engine.snapshotRendering = false;
        }

        // Reset the per-class ForceGLSL statics. "Test code inlining" (#YG3BBF#51)
        // sets BABYLON.PBRBaseMaterial.ForceGLSL = true and never restores it. On
        // bgfx that is a no-op (GLSL is the only path), but on WebGPU it pushes
        // every later PBR material onto the GLSL transpiler, which then rejects
        // shader includes that rely on the WGSL path -- the Atmosphere scenes fail
        // to compile ("unexpected SAMPLER2D") and never become ready. Collect the
        // classes once, then restore the default before each test; a test that
        // wants GLSL sets it again in its own createScene.
        if (forceGlslOwners === undefined) {
            forceGlslOwners = [];
            for (const key of Object.keys(BABYLON)) {
                let value;
                try {
                    value = BABYLON[key];
                } catch (e) {
                    continue;
                }
                if ((typeof value === "function" || (value && typeof value === "object")) &&
                    Object.getOwnPropertyDescriptor(value, "ForceGLSL")) {
                    forceGlslOwners.push(value);
                }
            }
        }
        for (const owner of forceGlslOwners) {
            try {
                owner.ForceGLSL = false;
            } catch (e) {
                // Read-only on some classes; nothing to restore in that case.
            }
        }

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

    // Only define no-op DOM stubs if the host hasn't already provided functional
    // ones. The NativeDawn (WebGPU) backend installs a real 2D canvas + document
    // (needed for WebGPU texture upload); the bgfx backend provides neither, so
    // these fallbacks apply there.
    if (typeof OffscreenCanvas === "undefined") {
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
    }

    if (typeof document === "undefined") {
        document = {
            createElement: function (type) {
                if (type === "canvas") {
                    return new OffscreenCanvas(64, 64);
                }
                return {};
            },
            removeEventListener: function () { }
        }
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
                        failedTitles.push(currentTitle);
                        // failTest() already triggered the debugger before
                        // reaching this callback; no second `debugger` here.
                        if (stopOnFirstFailure) {
                            logRunSummary();
                            TestUtils.exit(-1);
                            return;
                        }
                    } else {
                        passedCount++;
                    }
                    i++;
                    if (justOnce || i >= config.tests.length) {
                        logRunSummary();
                        engine.dispose();
                        TestUtils.exit(failedCount > 0 ? -1 : 0);
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


    function startValidation() {
        console.log("Starting");
        TestUtils.setTitle("Starting Native Validation Tests");
        TestUtils.updateSize(testWidth, testHeight);
        xhr.send();
    }

    // The canvas font is registered globally (NativeCanvas::loadTTF populates a
    // static font table that every 2D context reads), so both rendering paths
    // need it before any GUI/DynamicTexture text can rasterize.
    const loadFontThen = function (next) {
        BABYLON.Tools.LoadFile("https://raw.githubusercontent.com/CedricGuillemet/dump/master/droidsans.ttf", (data) => {
            _native.Canvas.loadTTFAsync("droidsans", data).then(next, next);
        }, undefined, undefined, true);
    };

    // The WebGPU engine loads its GLSL -> SPIR-V -> WGSL transpilers (the glslang
    // and twgsl WASM modules) lazily, on the first effect that is authored in
    // GLSL: _preparePipelineContextAsync awaits prepareGlslangAndTintAsync()
    // whenever shaderLanguage is GLSL and _glslangAndTintAreFullyLoaded is false.
    // That await makes the *first* GLSL effect compile asynchronously no matter
    // what, even with disableParallelShaderCompilation, so a scene that probes
    // effect.isReady() right after createEffect sees false and takes its "not
    // ready" branch. Whether it sees true then depends purely on whether some
    // earlier test already warmed the modules, which makes results depend on test
    // ordering (a test can pass in a full run and fail in isolation). Warm the
    // transpilers once up front so every test starts from the same state.
    const warmShaderTranspilersThen = function (engine, next) {
        if (typeof engine.prepareGlslangAndTintAsync !== "function") {
            next();
            return;
        }
        engine.prepareGlslangAndTintAsync().then(next, function (e) {
            // Non-fatal: only GLSL-authored shaders need these, and they will
            // retry the load on first use.
            console.error("Failed to preload glslang/twgsl: " + e);
            next();
        });
    };

    if (isDawn) {
        // The WebGPU engine completes initAsync asynchronously, pumped by the
        // host frame loop (RenderFrame -> frame() -> requestAnimationFrame).
        // Wait until the NativeDawn plugin promotes it to __dawnEngine before
        // starting: runRenderLoop needs a fully initialized engine and getCaps()
        // is only populated post-init. Playground assets load via absolute https
        // URLs (see loadPG), so _native.RootUrl is left alone here.
        const waitForEngine = function () {
            if (globalThis.__dawnEngine) {
                globalThis.__dawnEngine.getCaps().parallelShaderCompile = undefined;
                warmShaderTranspilersThen(globalThis.__dawnEngine, function () {
                    loadFontThen(startValidation);
                });
            } else {
                setTimeout(waitForEngine, 16);
            }
        };
        waitForEngine();
    } else {
        loadFontThen(function () {
            _native.RootUrl = "https://playground.babylonjs.com";
            startValidation();
        });
    }
})();
