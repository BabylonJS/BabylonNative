var engine = null;

// Playground runner: bootstraps BabylonJS WebGPU rendering in BabylonNative.
//
// Initialization guarantee: AppRuntime::Dispatch uses a FIFO WorkQueue, and
// NativeWebGPU::Initialize + Canvas::Initialize run in a Dispatch callback
// before ScriptLoader evaluates any JS. This means navigator.gpu and
// _native.Canvas are ALWAYS synchronously available by the time this script
// executes. No polling or readiness promises are needed.
//
// webgpu_smoke.js is loaded by ScriptLoader before this file and defines
// createScene at script scope, so it is also available synchronously.
//
// The reload-safety logic (epoch tracking, stale runtime detection) IS needed
// because Android surface recreation and the macOS refresh button can re-run
// this script against a recycled JS context.
(function () {
    var SMOKE_READY_WAIT_TIMEOUT_MS = 2000;

    function releaseRuntimeResources(runtime, reason) {
        if (!runtime) {
            return;
        }

        reportStatus("runner:stop:" + String(reason || "unknown"));

        try {
            if (runtime.engine && typeof runtime.engine.stopRenderLoop === "function") {
                runtime.engine.stopRenderLoop();
            }
        } catch (error) {
            void error;
        }

        if (runtime.fallbackRenderIntervalId !== null) {
            try {
                clearInterval(runtime.fallbackRenderIntervalId);
            } catch (error) {
                void error;
            }
            runtime.fallbackRenderIntervalId = null;
        }

        try {
            if (runtime.scene && typeof runtime.scene.dispose === "function") {
                runtime.scene.dispose();
            }
        } catch (error) {
            void error;
        }

        try {
            if (typeof globalThis.__webgpuSmokeDispose === "function") {
                globalThis.__webgpuSmokeDispose();
            }
        } catch (error) {
            void error;
        }

        try {
            if (runtime.engine && typeof runtime.engine.dispose === "function") {
                runtime.engine.dispose();
            }
        } catch (error) {
            void error;
        }

        if (globalThis.engine === runtime.engine) {
            globalThis.engine = null;
        }
        engine = null;
        runtime.engine = null;
        runtime.scene = null;
    }

    function stopRuntime(runtime, reason) {
        if (!runtime || runtime.disposed) {
            return;
        }

        runtime.disposed = true;
        releaseRuntimeResources(runtime, reason);
    }

    function createRuntimeObject() {
        return {
            disposed: false,
            scene: null,
            engine: null,
            fallbackRenderIntervalId: null,
            startupEpoch: 0
        };
    }

    var existingRuntime = globalThis.__babylonPlaygroundRuntime;
    if (existingRuntime) {
        stopRuntime(existingRuntime, "reload");
    }
    globalThis.__babylonPlaygroundRuntime = null;

    var runtime = createRuntimeObject();
    globalThis.__babylonPlaygroundRuntime = runtime;

    function reportStatus(message) {
        try {
            if (typeof globalThis.__nativePlaygroundStatus === "function") {
                globalThis.__nativePlaygroundStatus(String(message));
            }
        } catch (error) {
            void error;
        }
    }

    function createNativeCanvasForWebGPU() {
        if (typeof navigator === "undefined" || !navigator.gpu || typeof navigator.gpu._createCanvasContext !== "function") {
            return null;
        }

        var width = (typeof window !== "undefined" && window.innerWidth) ? window.innerWidth : 1280;
        var height = (typeof window !== "undefined" && window.innerHeight) ? window.innerHeight : 720;
        var gpuContext = navigator.gpu._createCanvasContext();
        return {
            style: {
                width: width + "px",
                height: height + "px"
            },
            ownerDocument: (typeof document !== "undefined" ? document : undefined),
            width: width,
            height: height,
            clientWidth: width,
            clientHeight: height,
            addEventListener: function () { },
            removeEventListener: function () { },
            setAttribute: function () { },
            focus: function () { },
            requestPointerLock: function () { },
            requestFullscreen: function () { return Promise.resolve(); },
            getBoundingClientRect: function () {
                return {
                    x: 0,
                    y: 0,
                    top: 0,
                    left: 0,
                    right: width,
                    bottom: height,
                    width: width,
                    height: height
                };
            },
            getContext: function (contextName) {
                if (contextName === "webgpu") {
                    return gpuContext;
                }

                return null;
            }
        };
    }

    function assertNativeApis() {
        if (typeof navigator === "undefined" || !navigator.gpu ||
            typeof navigator.gpu.requestAdapter !== "function" ||
            typeof navigator.gpu._createCanvasContext !== "function") {
            throw new Error(
                "navigator.gpu is not available. " +
                "NativeWebGPU::Initialize must run before this script. " +
                "See AppContext.cpp for the correct initialization sequence.");
        }
    }

    async function createEngineAsync() {
        assertNativeApis();

        if (typeof BABYLON.WebGPUEngine !== "function") {
            throw new Error("BABYLON.WebGPUEngine is not available. Ensure babylon.max.js is loaded.");
        }

        var canvas = createNativeCanvasForWebGPU();
        if (!canvas) {
            throw new Error("WebGPU requested but no native canvas context is available.");
        }

        var webgpuEngine = new BABYLON.WebGPUEngine(canvas, {
            antialias: false,
            adaptToDeviceRatio: true
        });
        await webgpuEngine.initAsync();
        reportStatus("runner:using-webgpu-engine");
        return webgpuEngine;
    }

    function getCreateSceneFunction() {
        // webgpu_smoke.js resolves the scene factory signal synchronously at
        // script scope, and ScriptLoader guarantees it runs before this file.
        // Check the signal first (provides the function directly), then fall
        // back to the global createScene.
        var sceneFactorySignal = globalThis.__babylonPlaygroundSceneFactoryReady;
        if (sceneFactorySignal && sceneFactorySignal.promise && typeof sceneFactorySignal.promise.then === "function") {
            var signalVersion = Number(sceneFactorySignal.version || 0);
            reportStatus("runner:createScene-await-signal:" + String(signalVersion));
            return sceneFactorySignal.promise.then(function (createSceneFn) {
                if (globalThis.__babylonPlaygroundRuntime !== runtime || runtime.disposed) {
                    throw new Error("createScene signal resolved for stale runtime.");
                }

                if (typeof createSceneFn === "function") {
                    reportStatus("runner:createScene-ready-signal:" + String(signalVersion));
                    return createSceneFn;
                }

                if (typeof createScene === "function") {
                    reportStatus("runner:createScene-ready-global:" + String(signalVersion));
                    return createScene;
                }

                throw new Error("createScene signal resolved without a callable createScene function.");
            });
        }

        if (typeof createScene === "function") {
            return Promise.resolve(createScene);
        }

        throw new Error(
            "createScene is not defined and no scene factory signal is available. " +
            "Ensure webgpu_smoke.js (or a script defining createScene) is loaded before playground_runner.js.");
    }

    function waitForSmokeReadySignalIfPresent() {
        var smokeReadySignal = globalThis.__babylonPlaygroundWebGpuSmokeReady;
        if (!smokeReadySignal || !smokeReadySignal.promise || typeof smokeReadySignal.promise.then !== "function") {
            return Promise.resolve();
        }

        var signalVersion = Number(smokeReadySignal.version || 0);
        reportStatus("runner:smoke-ready-await:" + String(signalVersion));

        return new Promise(function (resolve) {
            var settled = false;
            var timer = setTimeout(function () {
                if (settled) {
                    return;
                }
                settled = true;
                reportStatus("runner:smoke-ready-timeout:" + String(signalVersion));
                resolve();
            }, SMOKE_READY_WAIT_TIMEOUT_MS);

            Promise.resolve(smokeReadySignal.promise).then(function (result) {
                if (settled) {
                    return;
                }
                settled = true;
                clearTimeout(timer);
                var reason = result && result.reason ? String(result.reason) : "resolved";
                reportStatus("runner:smoke-ready:" + String(signalVersion) + ":" + reason);
                resolve();
            }).catch(function (error) {
                if (settled) {
                    return;
                }
                settled = true;
                clearTimeout(timer);
                reportStatus("runner:smoke-ready-error:" + String(error && error.message ? error.message : error));
                resolve();
            });
        });
    }

    function runStartup() {
        runtime.startupEpoch += 1;
        var startupEpoch = runtime.startupEpoch;
        reportStatus("runner:startup:epoch:" + String(startupEpoch));

        function isCurrentStartup() {
            return !runtime.disposed &&
                globalThis.__babylonPlaygroundRuntime === runtime &&
                runtime.startupEpoch === startupEpoch;
        }

        function disposePartialScene(scene) {
            if (!scene || typeof scene.dispose !== "function") {
                return;
            }

            try {
                scene.dispose();
            } catch (error) {
                void error;
            }
        }

        function disposePartialEngine(engineObject) {
            if (!engineObject || typeof engineObject.dispose !== "function") {
                return;
            }

            try {
                if (typeof engineObject.stopRenderLoop === "function") {
                    engineObject.stopRenderLoop();
                }
            } catch (error) {
                void error;
            }

            try {
                engineObject.dispose();
            } catch (error) {
                void error;
            }
        }

        (async function () {
            var createdEngine = null;
            var createdScene = null;
            var createSceneFn = await getCreateSceneFunction();
            if (!isCurrentStartup()) {
                return;
            }

            createdEngine = await createEngineAsync();
            if (!isCurrentStartup()) {
                disposePartialEngine(createdEngine);
                return;
            }

            runtime.engine = createdEngine;
            engine = createdEngine;
            globalThis.engine = createdEngine;
            reportStatus("runner:createScene-call");

            createdScene = createSceneFn(createdEngine);
            var scene = (createdScene && typeof createdScene.then === "function") ? await createdScene : createdScene;
            if (!scene) {
                throw new Error("createScene returned no scene.");
            }
            if (!isCurrentStartup()) {
                disposePartialScene(scene);
                disposePartialEngine(createdEngine);
                return;
            }
            runtime.scene = scene;
            await waitForSmokeReadySignalIfPresent();
            if (!isCurrentStartup()) {
                return;
            }

            reportStatus("runner:renderloop-start");
            var renderLoopErrorCount = 0;
            var renderLoopFrameLogged = false;
            var renderTick = function (fromFallbackInterval) {
                if (!isCurrentStartup()) {
                    return;
                }

                try {
                    if (!renderLoopFrameLogged) {
                        renderLoopFrameLogged = true;
                        reportStatus("runner:renderloop-frame:first");
                    }

                    if (!fromFallbackInterval && runtime.fallbackRenderIntervalId !== null) {
                        clearInterval(runtime.fallbackRenderIntervalId);
                        runtime.fallbackRenderIntervalId = null;
                        reportStatus("runner:renderloop-fallback-disabled");
                    }

                    scene.render();
                    renderLoopErrorCount = 0;
                } catch (error) {
                    renderLoopErrorCount += 1;
                    reportStatus("runner:renderloop-error:" + String(error && error.message ? error.message : error));

                    if (renderLoopErrorCount >= 5) {
                        reportStatus("runner:renderloop-error-persistent");
                        renderLoopErrorCount = 0;
                    }
                }
            };

            runtime.engine.runRenderLoop(function () {
                renderTick(false);
            });

            setTimeout(function () {
                if (!isCurrentStartup()) {
                    return;
                }

                if (renderLoopFrameLogged || runtime.fallbackRenderIntervalId !== null) {
                    return;
                }

                reportStatus("runner:renderloop-fallback-enabled");
                runtime.fallbackRenderIntervalId = setInterval(function () {
                    renderTick(true);
                }, 16);
            }, 300);
        })().catch(function (error) {
            var errorMessage = String(error && error.message ? error.message : error);
            reportStatus("runner:error:" + errorMessage);
            if (typeof console !== "undefined" && typeof console.error === "function") {
                console.error("[PlaygroundRunner] error", error);
            }

            stopRuntime(runtime, "startup-error");
        });
    }

    runStartup();
})();
