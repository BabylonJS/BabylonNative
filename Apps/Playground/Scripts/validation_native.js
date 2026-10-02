// Non-enumerable ES2019 polyfills for Chakra.
(function () {
    function define(proto, name, fn) {
        if (!proto[name]) {
            Object.defineProperty(proto, name, { value: fn, writable: true, configurable: true, enumerable: false });
        }
    }
    define(String.prototype, "trimStart", function () { return this.replace(/^[\s\uFEFF\xA0]+/, ""); });
    define(String.prototype, "trimEnd", function () { return this.replace(/[\s\uFEFF\xA0]+$/, ""); });
    define(Array.prototype, "flat", function (depth) {
        var d = depth === undefined ? 1 : Math.floor(depth);
        if (isNaN(d) || d < 1) { return Array.prototype.slice.call(this); }
        return Array.prototype.reduce.call(this, function (acc, cur) {
            if (Array.isArray(cur)) {
                const values = cur.flat(d - 1);
                for (let i = 0; i < values.length; ++i) {
                    acc.push(values[i]);
                }
            } else {
                acc.push(cur);
            }
            return acc;
        }, []);
    });
    define(Array.prototype, "flatMap", function (cb, thisArg) {
        return Array.prototype.map.call(this, cb, thisArg).flat();
    });
})();

(function () {
    let currentScene;
    let config;
    const opts = (typeof _playgroundOptions === "object" && _playgroundOptions) ? _playgroundOptions : {};
    const justOnce = !!opts.runOnce;
    const saveResult = (typeof opts.saveResults === "boolean") ? opts.saveResults : true;
    const testWidth = 600;
    const testHeight = 400;
    // Browser visualization tests create their engine with antialias=false.
    TestUtils.setMSAASamples(0);
    const generateReferences = !!opts.generateReferences;
    const breakOnFail = !!opts.breakOnFail;
    const stopOnFirstFailure = !!opts.stopOnFirstFailure;
    const listTests = !!opts.listTests;
    const includeExcluded = !!opts.includeExcluded;
    const testFilters = Array.isArray(opts.testFilters) ? opts.testFilters.map(s => String(s).toLowerCase()) : [];
    const testIndices = Array.isArray(opts.testIndices) ? opts.testIndices.map(n => +n) : [];
    // One-based capture frame; extend rendering to let RenderDoc finalize.
    const cliCaptureFrame = (typeof opts.captureFrame === "number" && opts.captureFrame > 0) ? (opts.captureFrame | 0) : 0;
    // Frames after the trigger to let RenderDoc finalize the .rdc.
    const POST_CAPTURE_FRAMES = 5;
    // Stopgap so native validation can pass. Examples should wait for their own
    // scene, material, GUI, and utility-scene resources; that belongs in the
    // examples, not this harness. Waiting here only because fixing each test
    // individually is a much larger task. Bound by elapsed time: a fast render
    // loop (Ubuntu GCC JavaScriptCore exhausted 240 callbacks in about 405 ms)
    // must not fail asynchronous GUI work before it can complete.
    const CONVERGENCE_DEADLINE_MS = 60 * 1000;
    const INITIAL_READINESS_TIMEOUT_MS = 10 * 60 * 1000;
    const READINESS_RECONCILE_INTERVAL_MS = 100;
    const utilityLayerRenderers = new WeakMap();
    const dracoDecoderConfiguration = Object.assign({}, BABYLON.DracoDecoder.DefaultConfiguration);
    const dracoEncoderConfiguration = Object.assign({}, BABYLON.DracoEncoder.DefaultConfiguration);
    const dracoDefaultNumWorkers = BABYLON.DracoCompression.DefaultNumWorkers;
    const dracoDecoderModule = globalThis.DracoDecoderModule;
    const dracoEncoderModule = globalThis.DracoEncoderModule;

    // Retain the renderer, not only its owner. manualRender never installs the
    // after-render observer; shouldRender can change after the layer is created.
    const updateUtilityLayerCamera = BABYLON.UtilityLayerRenderer.prototype._updateCamera;
    BABYLON.UtilityLayerRenderer.prototype._updateCamera = function () {
        const result = updateUtilityLayerCamera.apply(this, arguments);
        utilityLayerRenderers.set(this.utilityLayerScene, this);
        return result;
    };
    let havokInitializationPromise;

    function initializeHavokAsync() {
        if (typeof HK !== "undefined") {
            return Promise.resolve();
        }

        if (!havokInitializationPromise) {
            havokInitializationPromise = Promise.all([
                BABYLON.Tools.LoadFileAsync("app:///Scripts/HavokPhysics_umd.js", false),
                BABYLON.Tools.LoadFileAsync("app:///Scripts/HavokPhysics.wasm", true)
            ]).then(function (sources) {
                (0, eval)(sources[0]);
                if (typeof HavokPhysics !== "function") {
                    throw new Error("HavokPhysics_umd.js did not register HavokPhysics");
                }
                return HavokPhysics({ wasmBinary: new Uint8Array(sources[1]) });
            }).then(function (instance) {
                globalThis.HK = instance;
            });
        }

        return havokInitializationPromise;
    }

    function utilityLayerRendersAutomatically(renderer) {
        return !!(renderer && renderer._afterRenderObserver && renderer.shouldRender !== false);
    }

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
            // eslint-disable-next-line no-debugger
            debugger;
        }
        done(false);
    }

    // Reset the reused engine between tests.
    function cleanupAfterTest() {
        if (currentScene) {
            try { currentScene.dispose(); } catch (e) { console.error(e); }
            currentScene = null;
        }

        // Async loads may leave additional scenes registered on the engine.
        if (engine && engine.scenes) {
            const strayScenes = engine.scenes.slice();
            for (let i = 0; i < strayScenes.length; ++i) {
                try { strayScenes[i].dispose(); } catch (e) { console.error(e); }
            }
        }

        if (!engine) {
            return;
        }

        engine.setHardwareScalingLevel(1);

        engine.setStencilBuffer(false);
        engine.disableScissor();
        engine.useReverseDepthBuffer = false;
        engine.setDepthFunctionToLessOrEqual();

        // This is necessary because of https://github.com/BabylonJS/Babylon.js/pull/15217 so that each test starts fresh.
        engine.releaseEffects();

        // Cache keys omit load-time options; leaked textures can change later tests.
        const leakedTextures = engine.getLoadedTexturesCache();
        for (let i = leakedTextures.length - 1; i >= 0; --i) {
            engine._releaseTexture(leakedTextures[i]);
        }
        engine.clearInternalTexturesCache();

        // Global loader observers outlive scenes and can leak settings into later tests.
        BABYLON.SceneLoader.OnPluginActivatedObservable.clear();

        // Each scene must load matching Draco JS/WASM modules, not reuse another scene's factory.
        BABYLON.DracoCompression.ResetDefault();
        BABYLON.DracoDecoder.ResetDefault();
        BABYLON.DracoEncoder.ResetDefault();
        BABYLON.DracoCompression.DefaultNumWorkers = dracoDefaultNumWorkers;
        BABYLON.DracoDecoder.DefaultConfiguration = Object.assign({}, dracoDecoderConfiguration);
        BABYLON.DracoEncoder.DefaultConfiguration = Object.assign({}, dracoEncoderConfiguration);
        globalThis.DracoDecoderModule = dracoDecoderModule;
        globalThis.DracoEncoderModule = dracoEncoderModule;
    }

    // Every completion path must stop rendering and clean up exactly once.
    function makeTestDone(outerDone) {
        let finished = false;
        return function (status) {
            if (finished) {
                return;
            }
            finished = true;
            try {
                if (engine) {
                    engine.stopRenderLoop();
                }
            } catch (e) {
                console.error(e);
            }
            cleanupAfterTest();
            outerDone(status);
        };
    }

    function logFailureDiagnostics(test) {
        const outDir = TestUtils.getOutputDirectory();
        if (test.referenceImage) {
            console.log(`  Rendered result: ${outDir}/Results/${test.referenceImage}`);
            console.log(`  Diff overlay:    ${outDir}/Errors/${test.referenceImage}`);
        }
        if (test.playgroundId) {
            console.log(`  Note: this test loads playgroundId ${test.playgroundId} from the snippet server and pulls GUI/assets/fonts over the network, so async asset/font-load timing is one possible cause of a pixel diff.`);
        }
        console.log("  Re-run in isolation; a stable pixel-difference count on repeat runs is a reason to compare the saved result/diff images (not proof timing is ruled out):");
        console.log(`    Playground --headless --once --test "${test.title || ""}" app:///Scripts/validation_native.js`);
    }

    // Per-run counters surfaced as a final summary line on exit.
    let ranCount = 0;
    let passedCount = 0;
    let failedCount = 0;
    let skippedCount = 0;
    let missingRefCount = 0;
    const failedTitles = [];

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

    let engine;
    let useHighPrecisionMatrices = false;

    function createEngine(useLargeWorldRendering) {
        const nativeEngine = new BABYLON.NativeEngine({
            useLargeWorldRendering,
            useHighPrecisionMatrix: useHighPrecisionMatrices
        });
        nativeEngine.getCaps().parallelShaderCompile = undefined;
        nativeEngine.getRenderingCanvas = function () { return window; };
        nativeEngine.getInputElement = function () { return 0; };
        if (!window.screen) {
            window.screen = {
                width: nativeEngine.getRenderWidth(),
                height: nativeEngine.getRenderHeight(),
                availWidth: nativeEngine.getRenderWidth(),
                availHeight: nativeEngine.getRenderHeight(),
                colorDepth: 24,
                pixelDepth: 24,
                orientation: { angle: 0, type: "landscape-primary" }
            };
        }
        return nativeEngine;
    }

    // Retry network failures, transient server errors, and rate limits for all scene assets.
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

    // Preserve the window/canvas identity used by input handling.
    if (!window.style) {
        window.style = {};
    }
    if (typeof window.focus !== "function") {
        window.focus = function () { };
    }
    if (typeof window.blur !== "function") {
        window.blur = function () { };
    }
    // Dispatch through InputManager's picking paths; simulatePointer* bypasses skipPointer*Picking.
    const POINTER_INPUT_MOVE = 12;
    const domListeners = new Map();
    window.addEventListener = function (type, listener) {
        if (typeof listener !== "function") {
            return;
        }
        if (!domListeners.has(type)) {
            domListeners.set(type, []);
        }
        domListeners.get(type).push(listener);
    };
    window.removeEventListener = function (type, listener) {
        const list = domListeners.get(type);
        if (list) {
            const at = list.indexOf(listener);
            if (at !== -1) {
                list.splice(at, 1);
            }
        }
    };
    window.dispatchEvent = function (evt) {
        if (!evt) {
            return true;
        }

        if (evt.target === null || evt.target === undefined) {
            evt.target = window;
        }

        const list = domListeners.get(evt.type);
        if (list) {
            for (const listener of list.slice()) {
                listener.call(window, evt);
            }
        }

        const inputManager = currentScene && currentScene._inputManager;
        if (inputManager) {
            if (evt.button === undefined) {
                evt.button = 0;
            }
            switch (evt.type) {
                case "pointermove":
                    evt.inputIndex = POINTER_INPUT_MOVE;
                    inputManager._onPointerMove(evt);
                    break;
                case "pointerdown":
                    evt.inputIndex = evt.button + 2;
                    inputManager._onPointerDown(evt);
                    break;
                case "pointerup":
                    evt.inputIndex = evt.button + 2;
                    inputManager._onPointerUp(evt);
                    break;
                case "keydown":
                    inputManager._onKeyDown(evt);
                    break;
                case "keyup":
                    inputManager._onKeyUp(evt);
                    break;
            }
        }

        return !evt.defaultPrevented;
    };

    const canvas = window;
    globalThis.canvas = canvas;

    // Random replacement
    let seed = 1;
    function seededRandom() {
        const x = Math.sin(seed++) * 10000;
        return x - Math.floor(x);
    }
    Math.random = seededRandom;

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

    // Match browser screenshots: composite over the CSS canvas background, then the white page.
    // FrameGraph may copy transparent attachments regardless of scene.clearColor.
    // Legacy paths retain the clear-alpha gate because some write invalid alpha after opaque clears.
    const CANVAS_BACKGROUND = [173, 255, 47];

    function compositeOverCanvasBackground(data, canvasBackgroundColor) {
        let background = CANVAS_BACKGROUND;
        if (canvasBackgroundColor) {
            const rgba = _native.Canvas.parseColor(canvasBackgroundColor);
            const alpha = (rgba >>> 24) / 255;
            background = [rgba & 255, (rgba >>> 8) & 255, (rgba >>> 16) & 255]
                .map(channel => channel * alpha + 255 * (1 - alpha));
        }
        for (let index = 0; index < data.length; index += 4) {
            const alpha = data[index + 3];
            if (alpha === 255) {
                continue;
            }
            const src = alpha / 255;
            const dst = 1 - src;
            data[index] = Math.round(data[index] * src + background[0] * dst);
            data[index + 1] = Math.round(data[index + 1] * src + background[1] * dst);
            data[index + 2] = Math.round(data[index + 2] * src + background[2] * dst);
            data[index + 3] = 255;
        }
        return data;
    }

    function evaluateScreenshot(test, screenshot, referenceImage, done, compareFunction) {
        let testRes = true;

        if (test.canvasBackgroundColor || (currentScene && (currentScene.frameGraph || (currentScene.clearColor && currentScene.clearColor.a < 1)))) {
            compositeOverCanvasBackground(screenshot, test.canvasBackgroundColor);
        }

        if (!test.onlyVisual) {

            // Historical baseline; lower per-test exceptions as rendering fixes reach this target.
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

        // Inter-test cleanup + once-only guard live in makeTestDone (see runTest).
        done(testRes);
    }

    function evaluate(test, referenceImage, done, compareFunction) {
        TestUtils.getFrameBufferData(function (screenshot) {
            evaluateScreenshot(test, screenshot, referenceImage, done, compareFunction);
        }, function (error) {
            console.error(error);
            failTest(done);
        });
    }

    function areGuiTexturesReady(scene) {
        return scene.textures.every(function (texture) {
            return typeof texture.guiIsReady !== "function" || texture.guiIsReady();
        });
    }

    function hasPassId(value) {
        return value !== undefined && value !== null;
    }

    function addPass(passes, seen, id) {
        if (!hasPassId(id) || seen.has(id)) {
            return;
        }
        seen.add(id);
        passes.push(id);
    }

    function addRenderTargetPass(passes, seen, renderTarget) {
        if (!renderTarget) {
            return;
        }
        if (typeof renderTarget._shouldRender === "function" && !renderTarget._shouldRender()) {
            return;
        }
        if (hasPassId(renderTarget.renderPassId)) {
            addPass(passes, seen, renderTarget.renderPassId);
        }
    }

    function addCameraRenderTargetPasses(passes, seen, camera) {
        const targets = camera && camera.customRenderTargets;
        if (!targets) {
            return;
        }
        for (let i = 0; i < targets.length; i++) {
            addRenderTargetPass(passes, seen, targets[i]);
        }
    }

    // Matches Scene._renderForCamera: outputRenderTarget.renderPassId wins when set.
    // Multiview-to-single-view assigns the multiview texture only while rendering.
    function effectiveRenderPassId(camera) {
        if (camera._useMultiviewToSingleView && camera._multiviewTexture && hasPassId(camera._multiviewTexture.renderPassId)) {
            return camera._multiviewTexture.renderPassId;
        }
        if (camera.outputRenderTarget && hasPassId(camera.outputRenderTarget.renderPassId)) {
            return camera.outputRenderTarget.renderPassId;
        }
        if (hasPassId(camera.renderPassId)) {
            return camera.renderPassId;
        }
        return 0;
    }

    function renderPassesForNextFrame(scene) {
        const passes = [];
        const seen = new Set();
        const roots = scene.activeCameras && scene.activeCameras.length > 0
            ? scene.activeCameras
            : (scene.activeCamera ? [scene.activeCamera] : []);
        const rigModeNone = BABYLON.Constants && BABYLON.Constants.RIG_MODE_NONE !== undefined
            ? BABYLON.Constants.RIG_MODE_NONE
            : 0;
        for (let i = 0; i < roots.length; i++) {
            const camera = roots[i];
            if (!camera) {
                continue;
            }
            const rigCameras = camera._rigCameras || camera.rigCameras;
            const renderRigCameras = rigCameras && rigCameras.length > 0 &&
                camera.cameraRigMode !== rigModeNone &&
                !camera._renderingMultiview &&
                !camera._useMultiviewToSingleView;
            if (renderRigCameras) {
                addCameraRenderTargetPasses(passes, seen, camera);
                for (let j = 0; j < rigCameras.length; j++) {
                    const rigCamera = rigCameras[j];
                    if (!rigCamera) {
                        continue;
                    }
                    addPass(passes, seen, effectiveRenderPassId(rigCamera));
                    addCameraRenderTargetPasses(passes, seen, rigCamera);
                }
            } else {
                addPass(passes, seen, effectiveRenderPassId(camera));
                addCameraRenderTargetPasses(passes, seen, camera);
            }
        }
        if (scene.renderTargetsEnabled !== false && scene.customRenderTargets) {
            for (let i = 0; i < scene.customRenderTargets.length; i++) {
                addRenderTargetPass(passes, seen, scene.customRenderTargets[i]);
            }
        }
        return passes;
    }

    function meshesUseReadyEffects(scene) {
        let ready = true;
        for (let i = 0; i < scene.meshes.length; i++) {
            const mesh = scene.meshes[i];
            if (!mesh.isEnabled() || !mesh.subMeshes || mesh.subMeshes.length === 0) {
                continue;
            }
            for (let j = 0; j < mesh.subMeshes.length; j++) {
                const subMesh = mesh.subMeshes[j];
                const defines = subMesh.materialDefines;
                if (defines && defines.isDirty) {
                    ready = false;
                }
                const effect = subMesh.effect;
                if (effect && !effect.isReady()) {
                    ready = false;
                }
            }
        }
        return ready;
    }

    function isSceneConverged(scene) {
        const engine = scene.getEngine();
        const previousRenderPassId = engine.currentRenderPassId;
        let converged = true;
        try {
            if (!scene.isReady()) {
                converged = false;
            }
            if (!areGuiTexturesReady(scene)) {
                converged = false;
            }

            // Hot-swapping materials may report ready while their replacement effect is
            // still compiling. Inspect every pass the next frame can draw, not an unused one.
            const passes = renderPassesForNextFrame(scene);
            if (passes.length === 0) {
                passes.push(previousRenderPassId);
            }
            for (let i = 0; i < passes.length; i++) {
                engine.currentRenderPassId = passes[i];
                if (!meshesUseReadyEffects(scene)) {
                    converged = false;
                }
            }
        } finally {
            engine.currentRenderPassId = previousRenderPassId;
        }
        return converged;
    }

    function getConvergenceScenes(scene) {
        const scenes = [scene];
        const virtualScenes = scene.getEngine()._virtualScenes;
        for (let i = 0; i < virtualScenes.length; i++) {
            const virtualScene = virtualScenes[i];
            if (virtualScene === scene) {
                continue;
            }
            const renderer = utilityLayerRenderers.get(virtualScene);
            if (renderer) {
                // Ownership alone includes manual layers and layers with shouldRender false.
                if (renderer.originalScene === scene && utilityLayerRendersAutomatically(renderer)) {
                    scenes.push(virtualScene);
                }
                continue;
            }
            // Non-utility virtual scenes can still be associated through a shared camera.
            if (virtualScene.activeCamera && virtualScene.activeCamera.getScene() === scene) {
                scenes.push(virtualScene);
            }
        }
        return scenes;
    }

    function allScenesConverged(scenes) {
        // Do not stop at the first unready scene. Later scenes must start their
        // readiness and compilation work in the same callback.
        let converged = true;
        for (let i = 0; i < scenes.length; i++) {
            if (!isSceneConverged(scenes[i])) {
                converged = false;
            }
        }
        return converged;
    }

    function assertScheduling(condition, message) {
        if (!condition) {
            throw new Error("validation scheduling check failed: " + message);
        }
    }

    function passListContains(passes, id) {
        return passes.indexOf(id) !== -1;
    }

    function runSchedulingSelfCheck() {
        const outputScene = {
            activeCamera: { renderPassId: 3, outputRenderTarget: { renderPassId: 9 } },
            customRenderTargets: [
                { renderPassId: 4, _shouldRender: function () { return true; } },
                { renderPassId: 5, _shouldRender: function () { return false; } }
            ]
        };
        const outputPasses = renderPassesForNextFrame(outputScene);
        assertScheduling(passListContains(outputPasses, 9) && !passListContains(outputPasses, 3), "output render-target pass");
        assertScheduling(passListContains(outputPasses, 4) && !passListContains(outputPasses, 5), "scheduled custom render targets");

        const rigScene = {
            activeCamera: {
                renderPassId: 1,
                cameraRigMode: 1,
                customRenderTargets: [{ renderPassId: 6, _shouldRender: function () { return true; } }],
                _rigCameras: [
                    { renderPassId: 2 },
                    { renderPassId: 8, outputRenderTarget: { renderPassId: 11 } }
                ]
            }
        };
        const rigPasses = renderPassesForNextFrame(rigScene);
        assertScheduling(passListContains(rigPasses, 2) && passListContains(rigPasses, 11) && passListContains(rigPasses, 6) && !passListContains(rigPasses, 1), "rig-camera passes");

        const main = { getEngine: function () { return { _virtualScenes: virtualScenes }; } };
        const autoVirtual = {};
        const manualVirtual = {};
        const sharedVirtual = { activeCamera: { getScene: function () { return main; } } };
        const virtualScenes = [autoVirtual, manualVirtual, sharedVirtual];
        const autoRenderer = { originalScene: main, utilityLayerScene: autoVirtual, _afterRenderObserver: {}, shouldRender: false };
        utilityLayerRenderers.set(autoVirtual, autoRenderer);
        utilityLayerRenderers.set(manualVirtual, { originalScene: main, utilityLayerScene: manualVirtual, _afterRenderObserver: null, shouldRender: true });
        assertScheduling(getConvergenceScenes(main).indexOf(autoVirtual) === -1, "disabled utility layer excluded");
        autoRenderer.shouldRender = true;
        const enabled = getConvergenceScenes(main);
        assertScheduling(enabled.indexOf(autoVirtual) !== -1 && enabled.indexOf(sharedVirtual) !== -1 && enabled.indexOf(manualVirtual) === -1, "automatic utility layer and shared camera");
        autoRenderer.shouldRender = false;
        assertScheduling(getConvergenceScenes(main).indexOf(autoVirtual) === -1, "dynamic utility-layer disable");
        utilityLayerRenderers.delete(autoVirtual);
        utilityLayerRenderers.delete(manualVirtual);

        const polled = [];
        const combined = allScenesConverged([
            { isReady: function () { polled.push(1); return false; }, textures: [], meshes: [], getEngine: function () { return { currentRenderPassId: 0 }; } },
            { isReady: function () { polled.push(2); return true; }, textures: [], meshes: [], getEngine: function () { return { currentRenderPassId: 0 }; } }
        ]);
        assertScheduling(polled.join(",") === "1,2" && combined === false, "every associated scene is polled");
    }
    // Coverage for the scheduling helpers. A failure is reported, but it must not
    // abort the suite if a host embeds this script or Babylon internals shift.
    try {
        runSchedulingSelfCheck();
    } catch (e) {
        console.error(e);
    }

    function processCurrentScene(test, renderImage, done, compareFunction) {
        currentScene.useConstantAnimationDeltaTime = true;
        // Capture options must not shift the pixel-comparison frame.
        const compareFrame = test.renderCount || 1;
        // CLI capture overrides the legacy per-test capture flag.
        const captureFrame = cliCaptureFrame > 0
            ? cliCaptureFrame
            : (test.capture ? compareFrame : 0);
        // Allow RenderDoc to finalize after the capture frame.
        const stopFrame = captureFrame > 0
            ? Math.max(compareFrame, captureFrame + POST_CAPTURE_FRAMES)
            : compareFrame;

        let frameIndex = 0;
        let stopped = false;
        let pendingScreenshot = null;
        let evaluated = false;
        let convergenceStartedAt = 0;
        let readinessScenes = [];
        let readyScenes = [];
        let readinessReconcileTimer = null;
        let readinessTimeoutTimer = null;
        let waitingForReadiness = true;
        // Effect-layer RTTs need one submitted frame before composition, even with renderCount=1.
        let effectLayerPrimed = false;

        const runEvaluation = function (screenshot) {
            if (evaluated) {
                return;
            }
            evaluated = true;
            evaluateScreenshot(test, screenshot, renderImage, done, compareFunction);
        };

        const stopReadinessWait = function () {
            waitingForReadiness = false;
            if (readinessReconcileTimer !== null) {
                clearTimeout(readinessReconcileTimer);
                readinessReconcileTimer = null;
            }
            if (readinessTimeoutTimer !== null) {
                clearTimeout(readinessTimeoutTimer);
                readinessTimeoutTimer = null;
            }
            readinessScenes.length = 0;
            readyScenes.length = 0;
        };

        const failInitialReadiness = function () {
            if (stopped) {
                return;
            }
            stopped = true;
            evaluated = true;
            stopReadinessWait();
            console.error("Scene '" + (test.title || "?") + "' did not become ready within " +
                (INITIAL_READINESS_TIMEOUT_MS / 1000) + "s.");
            failTest(done);
        };

        const startRendering = function () {
            if (stopped) {
                return;
            }
            stopReadinessWait();
            if (currentScene.activeCamera && currentScene.activeCamera.useAutoRotationBehavior) {
                currentScene.activeCamera.useAutoRotationBehavior = false;
            }
            engine.runRenderLoop(function () {
                try {
                    if (stopped) {
                        return;
                    }
                    // Recompute because utility layers can be attached or disposed while
                    // convergence is pending, updating the engine's virtual-scene list.
                    const convergenceScenes = getConvergenceScenes(currentScene);
                    if (!allScenesConverged(convergenceScenes)) {
                        const now = Date.now();
                        if (convergenceStartedAt === 0) {
                            convergenceStartedAt = now;
                        }
                        if (now - convergenceStartedAt >= CONVERGENCE_DEADLINE_MS) {
                            stopped = true;
                            evaluated = true;
                            console.error("Scene '" + (test.title || "?") + "' did not converge within " +
                                (CONVERGENCE_DEADLINE_MS / 1000) + "s (scene, material, or GUI readiness).");
                            failTest(done);
                            return;
                        }
                        // Refresh material readiness without rendering extra animation/particle frames.
                        for (let i = 0; i < convergenceScenes.length; i++) {
                            convergenceScenes[i].incrementRenderId();
                        }
                        return;
                    }

                    if (!effectLayerPrimed && currentScene.effectLayers && currentScene.effectLayers.length > 0) {
                        effectLayerPrimed = true;
                        currentScene.render();
                        return;
                    }

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
                        }, function (error) {
                            evaluated = true;
                            stopped = true;
                            pendingScreenshot = null;
                            console.error(error);
                            failTest(done);
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
                    stopped = true;
                    evaluated = true;
                    console.error(e);
                    failTest(done);
                }
            });
        };

        // Resource loading belongs to the initial readiness budget, including
        // utility-scene models/textures; it must not consume convergence ticks.
        const reconcileReadinessScenes = function () {
            if (stopped || !waitingForReadiness) {
                return;
            }
            try {
                if (readinessReconcileTimer !== null) {
                    clearTimeout(readinessReconcileTimer);
                    readinessReconcileTimer = null;
                }

                const scenes = getConvergenceScenes(currentScene);
                const newScenes = [];
                for (let i = 0; i < scenes.length; i++) {
                    if (readinessScenes.indexOf(scenes[i]) === -1) {
                        newScenes.push(scenes[i]);
                    }
                }
                const retainedReadyScenes = [];
                for (let i = 0; i < readyScenes.length; i++) {
                    if (scenes.indexOf(readyScenes[i]) !== -1) {
                        retainedReadyScenes.push(readyScenes[i]);
                    }
                }
                readinessScenes = scenes;
                readyScenes = retainedReadyScenes;

                for (let i = 0; i < newScenes.length; i++) {
                    const scene = newScenes[i];
                    // Scene.executeWhenReady drops its callbacks on timeout or disposal.
                    // Keep a runner-owned deadline and reconcile virtual-scene membership
                    // independently so removed scenes cannot strand this wait.
                    scene.onReadyTimeoutDuration = INITIAL_READINESS_TIMEOUT_MS;
                    scene.onReadyTimeoutObservable.addOnce(function () {
                        if (!waitingForReadiness || readinessScenes.indexOf(scene) === -1) {
                            return;
                        }
                        reconcileReadinessScenes();
                        if (waitingForReadiness &&
                            readinessScenes.indexOf(scene) !== -1 &&
                            readyScenes.indexOf(scene) === -1) {
                            failInitialReadiness();
                        }
                    });
                    scene.executeWhenReady(function () {
                        if (!waitingForReadiness || readinessScenes.indexOf(scene) === -1) {
                            return;
                        }
                        if (readyScenes.indexOf(scene) === -1) {
                            readyScenes.push(scene);
                        }
                        reconcileReadinessScenes();
                    }, true);
                    if (!waitingForReadiness) {
                        return;
                    }
                }

                let allReady = readinessScenes.length > 0;
                for (let i = 0; i < readinessScenes.length; i++) {
                    // GUI image loads are not included in Scene.executeWhenReady.
                    if (readyScenes.indexOf(readinessScenes[i]) === -1 || !areGuiTexturesReady(readinessScenes[i])) {
                        allReady = false;
                        break;
                    }
                }
                if (allReady) {
                    startRendering();
                } else if (readinessReconcileTimer === null) {
                    readinessReconcileTimer = setTimeout(function () {
                        readinessReconcileTimer = null;
                        reconcileReadinessScenes();
                    }, READINESS_RECONCILE_INTERVAL_MS);
                }
            }
            catch (e) {
                stopped = true;
                evaluated = true;
                stopReadinessWait();
                console.error(e);
                failTest(done);
            }
        };

        readinessTimeoutTimer = setTimeout(failInitialReadiness, INITIAL_READINESS_TIMEOUT_MS);
        reconcileReadinessScenes();
        return function () {
            stopped = true;
            evaluated = true;
            pendingScreenshot = null;
            stopReadinessWait();
        };
    }

    function loadPlayground(test, done, referenceImage, compareFunction) {
        const outerDone = done;
        const testEngine = engine;
        let finished = false;
        let shaderFailure;
        let failureTimeoutId;
        let rejectSceneCreation;
        let stopSceneProcessing;
        const effectErrorObserver = testEngine.onEffectErrorObservable.add(function (event) {
            if (finished || shaderFailure || event.effect.isDisposed || !event.effect.allFallbacksProcessed() || event.effect.isReady()) {
                return;
            }
            shaderFailure = new Error("Shader compilation failed for '" + test.title + "': " + event.errors);
            console.error(shaderFailure.message);
            // Scene/effect disposal must run outside the compiler's notification stack.
            failureTimeoutId = setTimeout(function () {
                if (rejectSceneCreation) {
                    rejectSceneCreation(shaderFailure);
                } else {
                    failTest(done);
                }
            }, 0);
        });
        done = function (status) {
            if (finished) {
                return;
            }
            finished = true;
            testEngine.onEffectErrorObservable.remove(effectErrorObserver);
            clearTimeout(failureTimeoutId);
            if (stopSceneProcessing) {
                stopSceneProcessing();
            }
            outerDone(shaderFailure ? false : status);
        };

        if (test.sceneFolder) {
            BABYLON.SceneLoader.Load(config.root + test.sceneFolder, test.sceneFilename, engine, function (newScene) {
                if (finished) {
                    if (!newScene.isDisposed) {
                        newScene.dispose();
                    }
                    return;
                }
                currentScene = newScene;
                stopSceneProcessing = processCurrentScene(test, referenceImage, done, compareFunction);
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
                        if (finished) {
                            return;
                        }
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
                            // Leave the native XHR callback stack before constructing deep scenes.
                            setTimeout(async function () {
                                if (finished) {
                                    return;
                                }
                                // eslint-disable-next-line no-unused-vars
                                var name = ""; // see the note on the scriptToRun eval below
                                try {
                                    if (test.requiresHavok) {
                                        await initializeHavokAsync();
                                        if (finished) {
                                            return;
                                        }
                                    }

                                    let createdScene = eval(pgCode);

                                    if (createdScene && createdScene.then) {
                                        // Bound async creation before scene readiness begins; native blocking needs an external timeout.
                                        const createSceneTimeoutMs = 10 * 60 * 1000;
                                        let createSceneTimeoutId;
                                        try {
                                            createdScene = await Promise.race([
                                                Promise.resolve(createdScene).then(function (scene) {
                                                    if (finished && scene && !scene.isDisposed) {
                                                        scene.dispose();
                                                    }
                                                    return scene;
                                                }),
                                                new Promise(function (resolve, reject) {
                                                    rejectSceneCreation = reject;
                                                    createSceneTimeoutId = setTimeout(function () {
                                                        reject(new Error("createScene promise for " + test.playgroundId +
                                                            " did not resolve within " + (createSceneTimeoutMs / 1000) + "s."));
                                                    }, createSceneTimeoutMs);
                                                })
                                            ]);
                                        }
                                        finally {
                                            clearTimeout(createSceneTimeoutId);
                                            rejectSceneCreation = undefined;
                                        }
                                    } else {
                                        currentScene = createdScene;
                                    }

                                    if (finished) {
                                        return;
                                    }
                                    currentScene = createdScene;
                                    stopSceneProcessing = processCurrentScene(test, referenceImage, done, compareFunction);
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

            request.onreadystatechange = function () {
                if (request.readyState === 4) {
                    if (finished) {
                        return;
                    }
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

                        const scriptCode = scriptToRun + test.functionToCall + "(engine)";
                        // Leave the native XHR callback stack before constructing deep scenes.
                        setTimeout(function () {
                            if (finished) {
                                return;
                            }
                            // Direct eval sees the browser's default name through this local binding.
                            // eslint-disable-next-line no-unused-vars
                            var name = "";
                            try {
                                currentScene = eval(scriptCode);
                                stopSceneProcessing = processCurrentScene(test, referenceImage, done, compareFunction);
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
            };
            request.onerror = function () {
                console.error("Network error during test load.");
                failTest(done);
            }

            request.send(null);
        }
    }
    function runTest(index, outerDone) {
        const done = makeTestDone(outerDone);
        if (index >= config.tests.length) {
            done(false);
            return;
        }

        const test = config.tests[index];
        const testInfo = "Running " + test.title;
        console.log(testInfo);
        TestUtils.setTitle(testInfo);

        try {
            const useLargeWorldRendering = !!test.useLargeWorldRendering;
            if (!engine || !!engine.getCreationOptions().useLargeWorldRendering !== useLargeWorldRendering) {
                if (engine) {
                    engine.dispose();
                    engine = undefined;
                    globalThis.engine = undefined;
                }
                engine = createEngine(useLargeWorldRendering);
                globalThis.engine = engine;
            }
        } catch (e) {
            console.error(e);
            failTest(done);
            return;
        }

        seed = 1;
        Math.random = seededRandom;

        if (generateReferences) {
            loadPlayground(test, done, undefined, saveRenderedResult);
        } else {
            // Pixel comparisons require a catalog reference before loading the scene.
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
                // Keep the diagnostic tag used by CI.
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

    // Expose the native Canvas Image under its DOM name.
    if (typeof globalThis.Image === "undefined" && typeof _native !== "undefined" && _native.Image) {
        globalThis.Image = _native.Image;
    }

    if (typeof globalThis.KeyboardEvent === "undefined") {
        // InputManager only needs keyboard-event state, not a DOM event implementation.
        globalThis.KeyboardEvent = function (type, init) {
            this.type = type;
            for (const key in (init || {})) {
                this[key] = init[key];
            }
            if (this.key === undefined) { this.key = ""; }
            if (this.code === undefined) { this.code = ""; }
            if (this.keyCode === undefined) { this.keyCode = 0; }
            if (this.ctrlKey === undefined) { this.ctrlKey = false; }
            if (this.altKey === undefined) { this.altKey = false; }
            if (this.shiftKey === undefined) { this.shiftKey = false; }
            if (this.metaKey === undefined) { this.metaKey = false; }
            if (this.repeat === undefined) { this.repeat = false; }
            this.target = null;
            this.defaultPrevented = false;
            this.preventDefault = function () { this.defaultPrevented = true; };
            this.stopPropagation = function () { };
            this.stopImmediatePropagation = function () { };
        };
    }

    // InputManager only needs pointer-event state, not a DOM event implementation.
    if (typeof globalThis.PointerEvent === "undefined") {
        globalThis.PointerEvent = function (type, init) {
            this.type = type;
            for (const key in (init || {})) {
                this[key] = init[key];
            }
            if (this.pointerId === undefined) { this.pointerId = 1; }
            if (this.pointerType === undefined) { this.pointerType = "mouse"; }
            if (this.button === undefined) { this.button = 0; }
            if (this.buttons === undefined) { this.buttons = 0; }
            if (this.clientX === undefined) { this.clientX = 0; }
            if (this.clientY === undefined) { this.clientY = 0; }
            if (this.movementX === undefined) { this.movementX = 0; }
            if (this.movementY === undefined) { this.movementY = 0; }
            this.target = null;
            this.defaultPrevented = false;
            this.preventDefault = function () { this.defaultPrevented = true; };
            this.stopPropagation = function () { };
        };
    }

    document = {
        createElement: function (type) {
            if (type === "canvas") {
                // Image-processing snippets need a real 2D drawing/readback context.
                return engine.createCanvas(64, 64);
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
                // TSV exclusions reflect the catalog, regardless of --include-excluded.
                for (let i = 0; i < config.tests.length; ++i) {
                    const t = config.tests[i];
                    const reason = getExclusionReason(t) || "";
                    console.log(i + "\t" + (t.title || "") + "\t" + (t.referenceImage || "") + "\t" + reason);
                }
                TestUtils.exit(0);
                return;
            }

            const runnableTests = config.tests.filter((test, index) => shouldRunTest(test, index) && getSkipReason(test) === null);
            const selectedTests = justOnce ? runnableTests.slice(0, 1) : runnableTests;
            // Precision is global, and cached matrices can only be promoted before the first engine.
            useHighPrecisionMatrices = selectedTests.some(test => test.useLargeWorldRendering || test.useHighPrecisionMatrix);

            // Run tests
            const recursiveRunTest = function (i) {
                // Filter mismatches stay silent and do not consume --once.
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
                    if (engine) {
                        engine.dispose();
                    }
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
                        if (engine) {
                            engine.dispose();
                        }
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


    function loadFontAssetAsync(url) {
        return BABYLON.Tools.LoadFileAsync(url, true).then(function (data) {
            if (!(data instanceof ArrayBuffer) || data.byteLength === 0) {
                throw new Error("Invalid font response from " + url);
            }
            return data;
        });
    }

    Promise.all([
        loadFontAssetAsync("app:///Scripts/DroidSans.ttf"),
        loadFontAssetAsync("app:///Scripts/Arimo-Regular.ttf")
    ]).then(function (fonts) {
        _native.Canvas.loadTTF("droidsans", fonts[0]);
        _native.Canvas.loadTTF("monospace", fonts[0]);
        return _native.Canvas.loadTTFAsync("Arial", fonts[1]);
    }).then(function () {
        _native.RootUrl = "https://playground.babylonjs.com";
        console.log("Starting");
        TestUtils.setTitle("Starting Native Validation Tests");
        TestUtils.updateSize(testWidth, testHeight);
        xhr.send();
    }).catch(function (error) {
        console.error("Failed to initialize validation fonts: " + error);
        TestUtils.exit(1);
    });
})();
