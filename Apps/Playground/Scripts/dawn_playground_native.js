(async function () {
    "use strict";

    let checks = 0;
    const timeout = setTimeout(function () {
        console.error("DAWN_PLAYGROUND_TEST_FAIL: timed out after " + checks + " checks");
        TestUtils.exit(-1);
    }, 30000);

    function assert(condition, message) {
        if (!condition) {
            throw new Error(message);
        }
        checks++;
    }

    try {
        assert(TestUtils.getGraphicsApiName() === "WebGPU", "Explicit WebGPU backend identity");
        assert(typeof frame === "undefined" && typeof __dawnResize === "undefined" &&
            typeof _nativeDawnClear === "undefined", "No plugin lifecycle or milestone globals");
        const descriptor = Object.getOwnPropertyDescriptor(globalThis, "BABYLON");
        assert(descriptor && !descriptor.get && !descriptor.set, "BABYLON is not intercepted by NativeDawn");
        assert(typeof __dawnEngine === "undefined" && typeof __dawnPendingEngine === "undefined",
            "No plugin-owned Babylon engine globals");
        assert(!BABYLON.Tools.__dawnLoadScriptPatched, "Shader loading belongs to the Playground host");

        const engine = await _playgroundWebGPUReady;
        assert(engine === _playgroundWebGPUEngine && engine instanceof BABYLON.WebGPUEngine,
            "Explicit Playground engine initialization");
        assert(new BABYLON.NativeEngine() === engine, "Playground compatibility alias uses the ready engine");
        const canvas = document.getElementById("renderCanvas");
        assert(canvas === engine.getRenderingCanvas(), "Engine uses the plugin's presentation canvas");

        let scriptError = false;
        await new Promise(function (resolve, reject) {
            BABYLON.Tools.LoadScript("app:///Scripts/does-not-exist-dawn-regression.js",
                function () { reject(new Error("Missing shader helper script must not load")); },
                function () { scriptError = true; resolve(); });
        });
        assert(scriptError, "Host script loader reports failures");

        const scene = new BABYLON.Scene(engine);
        scene.createDefaultCameraOrLight();
        engine.runRenderLoop(function () { scene.render(); });
        async function checkFrame(width, height, color, expected) {
            scene.clearColor = color;
            const pixels = await new Promise(function (resolve) {
                requestAnimationFrame(function () { TestUtils.getFrameBufferData(resolve); });
            });
            assert(pixels.length === width * height * 4, "Host render attachments match the resized surface");
            for (let i = 0; i < pixels.length; i++) {
                if (pixels[i] !== expected[i % 4]) {
                    throw new Error("Host readback mismatch at byte " + i + ": " + pixels[i]);
                }
            }
            checks++;
        }

        TestUtils.updateSize(80, 48);
        assert(canvas.width === 80 && canvas.height === 48 &&
            canvas.clientWidth === 80 && canvas.clientHeight === 48, "Typed resize updates drawing and client sizes");
        await checkFrame(80, 48, new BABYLON.Color4(1, 0, 0, 1), [255, 0, 0, 255]);
        canvas.width = 48;
        canvas.height = 16;
        await checkFrame(48, 16, new BABYLON.Color4(0, 1, 0, 1), [0, 255, 0, 255]);
        engine.setHardwareScalingLevel(2);
        await checkFrame(40, 24, new BABYLON.Color4(0, 0, 1, 1), [0, 0, 255, 255]);
        engine.setHardwareScalingLevel(1);
        await checkFrame(80, 48, new BABYLON.Color4(1, 0, 0, 1), [255, 0, 0, 255]);

        engine.stopRenderLoop();
        await new Promise(function (resolve) { setTimeout(resolve, 100); });
        const frozenPixels = await new Promise(function (resolve) { TestUtils.getFrameBufferData(resolve); });
        assert(frozenPixels.length === 80 * 48 * 4, "Stopped render-loop readback keeps framebuffer dimensions");
        assert(Array.from(frozenPixels).every(function (value, index) { return value === [255, 0, 0, 255][index % 4]; }),
            "Stopped render-loop readback preserves the actual last presented frame");
        scene.dispose();
        engine.dispose();
        clearTimeout(timeout);
        console.log("Dawn Playground host validated: " + checks + " checks");
        TestUtils.exit(0);
    } catch (error) {
        clearTimeout(timeout);
        console.error("DAWN_PLAYGROUND_TEST_FAIL: " + (error.stack || error));
        TestUtils.exit(-1);
    }
})();
