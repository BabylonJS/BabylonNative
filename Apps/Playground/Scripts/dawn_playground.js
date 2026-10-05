(function () {
    "use strict";

    // Headless Playground loads shader helpers through the shared fetch polyfill.
    BABYLON.Tools.LoadScript = function (url, onSuccess, onError) {
        fetch(url).then(function (response) {
            if (!response.ok) {
                throw new Error("Script request failed with status " + response.status + ": " + url);
            }
            return response.text();
        }).then(function (data) {
            Function(data).apply(null);
            if (onSuccess) {
                onSuccess();
            }
        }).catch(function (error) {
            if (onError) {
                onError("Playground failed to load script " + url, error);
                return;
            }
            setTimeout(function () { throw error; }, 0);
        });
    };

    const canvas = document.getElementById("renderCanvas");
    const engine = new BABYLON.WebGPUEngine(canvas, {
        antialias: false,
        stencil: true,
        premultipliedAlpha: false,
        enableAllFeatures: false
    });
    engine.enableOfflineSupport = false;
    engine.disableManifestCheck = true;
    globalThis._playgroundWebGPUEngine = engine;
    globalThis._playgroundWebGPUReady = engine.initAsync().then(function () {
        let width = canvas.width;
        let height = canvas.height;
        engine.onResizeObservable.add(function () {
            width = canvas.width;
            height = canvas.height;
        });
        engine.onBeginFrameObservable.add(function () {
            if (canvas.width !== width || canvas.height !== height) {
                engine.setSize(canvas.width, canvas.height, true);
            }
        });

        // The validation host needs attachments resized before constructing scenes.
        const updateSize = TestUtils.updateSize;
        TestUtils.updateSize = function (w, h) {
            updateSize.call(TestUtils, w, h);
            engine.setSize(canvas.width, canvas.height, true);
        };
        BABYLON.NativeEngine = function () { return engine; };
        console.log("Playground WebGPUEngine ready");
        return engine;
    });
    globalThis._playgroundWebGPUReady.catch(function (error) {
        console.error("Playground WebGPUEngine initialization failed: " + (error.stack || error));
        TestUtils.exit(-1);
    });
})();
