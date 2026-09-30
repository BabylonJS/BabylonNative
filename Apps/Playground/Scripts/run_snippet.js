// Standalone single-playground runner for Babylon Native (NativeDawn / WebGPU).
//
// Fetches ONE Babylon.js Playground snippet by its playgroundId and renders it
// live in the Playground window -- no pixel comparison, no config.json, no exit.
// It reuses the same snippet-fetch / URL-rewrite / createScene(engine) logic the
// validation harness uses, so anything that renders under validation also runs
// here.
//
// Usage (from the folder containing Playground.exe):
//   Playground.exe --test "#U8O4EP#1" Scripts\run_snippet.js
//
// The playgroundId is taken from the --test value (first one). If none is given
// the DEFAULT_PLAYGROUND_ID below is used. IDs look like "#XXXXXX#N" (the "#N"
// revision is optional; "#0" is assumed when omitted), exactly as they appear in
// the "playgroundId" field of Apps/Playground/Scripts/config.json.
(function () {
    const DEFAULT_PLAYGROUND_ID = "#U8O4EP#1"; // Gaussian Splatting Compressed ply SH

    const opts = (typeof _playgroundOptions === "object" && _playgroundOptions) ? _playgroundOptions : {};
    const filters = Array.isArray(opts.testFilters) ? opts.testFilters : [];
    let playgroundId = (filters.length > 0 && filters[0]) ? String(filters[0]) : DEFAULT_PLAYGROUND_ID;
    if (playgroundId[0] !== "#" || playgroundId.indexOf("#", 1) === -1) {
        playgroundId += "#0";
    }

    // Backend detection + engine acquisition mirrors validation_native.js: under
    // NativeDawn the WebGPUEngine is pre-created by the plugin and promoted to
    // globalThis.__dawnEngine once its async init (driven by host frames)
    // completes, so we must wait for it rather than construct a second engine.
    const isDawn = (typeof globalThis._nativeDawnClear === "function");

    const snippetUrl = "https://snippet.babylonjs.com";
    const pgRoot = "https://playground.babylonjs.com";

    function runScene(engine) {
        globalThis.engine = engine;
        if (BABYLON.SceneLoader) {
            BABYLON.SceneLoader.ShowLoadingScreen = false;
        }
        engine.displayLoadingUI = function () { };

        const url = snippetUrl + playgroundId.replace(/#/g, "/");
        console.log("Loading playground " + playgroundId + " from " + url);

        BABYLON.Tools.LoadFile(
            url,
            function (responseText) {
                let code;
                try {
                    const snippet = JSON.parse(responseText);
                    code = JSON.parse(snippet.jsonPayload).code.toString();

                    // v2 multi-file manifest: run the entry file.
                    try {
                        const manifest = JSON.parse(code);
                        if (manifest.v === 2) {
                            code = manifest.files[manifest.entry]
                                .replace(/export +default +/g, "")
                                .replace(/export +/g, "");
                        }
                    } catch (e) { /* not a manifest */ }

                    // Rewrite relative asset URLs to the playground CDN.
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
                } catch (e) {
                    console.error("Failed to parse snippet " + playgroundId + ": " + e);
                    return;
                }

                const pgCode = code + "\r\ncreateScene(engine)";
                // Defer to a fresh macrotask so eval()/createScene() run at a
                // shallow native-stack depth (see validation_native.js).
                setTimeout(function () {
                    let scene;
                    try {
                        scene = eval(pgCode);
                    } catch (e) {
                        console.error("Failed to evaluate playground " + playgroundId + ": " + e);
                        return;
                    }
                    const start = function (s) {
                        // Some snippets create the scene as a side effect and
                        // return nothing (or a non-scene); fall back to the
                        // engine's last scene so we never render an empty frame.
                        if (!s || typeof s.render !== "function") {
                            const scenes = engine.scenes || [];
                            s = scenes[scenes.length - 1];
                        }
                        if (!s) {
                            console.error("Playground " + playgroundId + " produced no scene.");
                            return;
                        }

                        let started = false;
                        const beginRendering = function (why) {
                            if (started) {
                                return;
                            }
                            started = true;
                            engine.runRenderLoop(function () {
                                s.render();
                            });
                            console.log("Rendering playground " + playgroundId + " (" + why + ")");
                        };

                        // Only start presenting once the scene is ready. Calling
                        // scene.render() on a not-yet-ready scene returns early
                        // without clearing or presenting, so on WebGPU the
                        // swapchain never receives a frame and the window stays
                        // black for as long as the scene is unready (NodeMaterial
                        // GLSL shaders compile asynchronously through the glslang
                        // WASM module, which can take a while on first run).
                        s.onReadyTimeoutDuration = 10 * 60 * 1000;
                        s.onReadyTimeoutObservable.addOnce(function () {
                            // Never-ready scene: render anyway so the user sees
                            // whatever the scene can produce instead of black.
                            console.error("Playground " + playgroundId + " did not become ready within " +
                                (s.onReadyTimeoutDuration / 1000) + "s; rendering anyway.");
                            beginRendering("timeout");
                        });
                        s.executeWhenReady(function () {
                            beginRendering("ready");
                        });
                    };
                    if (scene && scene.then) {
                        scene.then(start).catch(function (e) { console.error(e); });
                    } else {
                        start(scene);
                    }
                }, 0);
            },
            undefined,  // onProgress
            undefined,  // database
            false,      // useArrayBuffer (snippet response is JSON text)
            function (request, exception) {
                const status = request ? (request.status + " " + request.statusText) : "no response";
                console.error("Failed to load snippet " + playgroundId + ": " + status + (exception ? (" " + exception) : ""));
            });
    }

    if (isDawn) {
        // Wait for the plugin to finish async engine init before rendering.
        const waitForEngine = function () {
            if (globalThis.__dawnEngine) {
                runScene(globalThis.__dawnEngine);
            } else {
                setTimeout(waitForEngine, 16);
            }
        };
        waitForEngine();
    } else {
        runScene(new BABYLON.NativeEngine({ adaptToDeviceRatio: true }));
    }
})();
