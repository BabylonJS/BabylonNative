var engine = null;

(function () {
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

    async function createEngineAsync() {
        var canUseWebGPU =
            typeof BABYLON.WebGPUEngine === "function" &&
            typeof navigator !== "undefined" && navigator.gpu &&
            typeof navigator.gpu.requestAdapter === "function";

        if (!canUseWebGPU) {
            throw new Error("WebGPU is required but navigator.gpu or WebGPUEngine is unavailable.");
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

    if (typeof createScene !== "function") {
        reportStatus("runner:createScene-missing");
        return;
    }

    (async function () {
        engine = await createEngineAsync();
        globalThis.engine = engine;
        reportStatus("runner:createScene-call");

        var createdScene = createScene(engine);
        var scene = (createdScene && typeof createdScene.then === "function") ? await createdScene : createdScene;
        if (!scene) {
            throw new Error("createScene returned no scene.");
        }

        reportStatus("runner:renderloop-start");
        engine.runRenderLoop(function () {
            scene.render();
        });
    })().catch(function (error) {
        reportStatus("runner:error:" + String(error && error.message ? error.message : error));
        if (typeof console !== "undefined" && typeof console.error === "function") {
            console.error("[PlaygroundRunner] error", error);
        }
    });
})();
