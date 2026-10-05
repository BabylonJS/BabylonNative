(async function () {
    "use strict";

    let checks = 0;
    const timeout = setTimeout(function () {
        console.error("NATIVEDAWN_RUNTIME_TEST_FAIL: asynchronous runtime work timed out after " + checks + " checks");
        TestUtils.exit(-1);
    }, 30000);

    function assert(condition, message) {
        if (!condition) {
            throw new Error(message);
        }
        checks++;
    }

    function assertBytes(actual, expected, message) {
        assert(actual.length === expected.length &&
            expected.every(function (value, index) { return actual[index] === value; }), message);
    }

    function assertThrows(callback, message) {
        let threw = false;
        try {
            callback();
        } catch (error) {
            threw = true;
        }
        assert(threw, message);
    }

    function loadXHR(url) {
        return new Promise(function (resolve, reject) {
            const xhr = new XMLHttpRequest();
            xhr.open("GET", url, true);
            xhr.responseType = "arraybuffer";
            xhr.addEventListener("readystatechange", function () {
                if (xhr.readyState !== 4) {
                    return;
                }
                if (xhr.status >= 200 && xhr.status < 300) {
                    resolve(xhr.response);
                } else {
                    reject(new Error("XHR failed: " + url + " (status " + xhr.status + ")"));
                }
            });
            xhr.addEventListener("error", function () { reject(new Error("XHR failed: " + url)); });
            xhr.send();
        });
    }

    try {
        assert(typeof navigator.gpu === "object", "NativeDawn WebGPU is not installed");
        assert(!TextDecoder.__dawnUtf16, "NativeDawn must not replace the shared TextDecoder");
        assert(!WebAssembly.__dawnSyncInstantiate, "NativeDawn must not replace asynchronous WebAssembly.instantiate");
        assert(!Blob.__dawnStash, "NativeDawn must not replace the native Blob constructor");
        assert(!fetch.__dawnRewrite, "NativeDawn must not rewrite application asset URLs");
        assert(typeof _nativeDawnReadFileBytes === "undefined", "NativeDawn must not install the obsolete file-loading fallback");
        assert(typeof _nativeDawnResize === "undefined", "NativeDawn must not install the obsolete test resize hook");
        assert(typeof _nativeDawnExit === "undefined", "NativeDawn must not install a duplicate process-exit hook");
        assert(typeof frame === "undefined", "NativeDawn must not install a global frame driver");
        assert(typeof __dawnResize === "undefined", "NativeDawn must not install a global resize bridge");
        assert(typeof _nativeDawnClear === "undefined", "NativeDawn must not install the milestone clear hook");
        assert(typeof BABYLON === "undefined" &&
            !Object.getOwnPropertyDescriptor(globalThis, "BABYLON"), "Standalone NativeDawn must not intercept or load BABYLON");
        assert(typeof _playgroundWebGPUEngine === "undefined", "Standalone runtime must not pre-create a Babylon.js engine");
        const nativeEngine = function () { };
        const loadScript = function () { };
        globalThis.BABYLON = {
            NativeEngine: nativeEngine,
            WebGPUEngine: function () { throw new Error("NativeDawn must not construct WebGPUEngine"); },
            Tools: { LoadScript: loadScript }
        };
        globalThis.frame = function () { throw new Error("The host must not invoke an application's global frame"); };
        await new Promise(function (resolve) { requestAnimationFrame(resolve); });
        assert(BABYLON.NativeEngine === nativeEngine && BABYLON.Tools.LoadScript === loadScript,
            "Assigning BABYLON must not replace its engine or script loader");
        delete globalThis.frame;
        delete globalThis.BABYLON;

        const le = new TextDecoder("utf-16le");
        const be = new TextDecoder("utf-16be");
        assert(le.decode(new Uint8Array([255, 254, 65, 0])) === "A", "UTF-16LE BOM stripping");
        assert(be.decode(new Uint8Array([254, 255, 0, 65])) === "A", "UTF-16BE BOM stripping");
        assert(le.decode(new Uint8Array([0, 216])) === "\ufffd", "UTF-16 unpaired surrogate replacement");
        assert(le.decode(new Uint8Array([65, 0, 66])) === "A\ufffd", "UTF-16 trailing byte replacement");
        assert(le.decode(new Uint8Array([0, 65, 0, 0]).subarray(1, 3)) === "A", "TextDecoder typed-array range");
        assert(be.decode(new Uint8Array([216, 61, 222, 0])) === "\ud83d\ude00", "UTF-16 surrogate pair");

        // Completion is posted to V8's foreground task runner, not just its microtask queue.
        const wasm = new Uint8Array([
            0, 97, 115, 109, 1, 0, 0, 0,
            1, 5, 1, 96, 0, 1, 127,
            3, 2, 1, 0,
            7, 10, 1, 6, 97, 110, 115, 119, 101, 114, 0, 0,
            10, 6, 1, 4, 0, 65, 42, 11
        ]);
        const compiled = await WebAssembly.instantiate(wasm);
        assert(compiled.instance.exports.answer() === 42, "Asynchronous WebAssembly byte compilation");
        const instance = await WebAssembly.instantiate(compiled.module);
        assert(instance.exports.answer() === 42, "WebAssembly Module overload");

        const source = new Uint8Array([99, 1, 2, 3, 99]);
        const blob = new Blob([source.subarray(1, 4)], { type: "application/octet-stream" });
        source.fill(0);
        assert(blob.size === 3 && !("__dawnU8" in blob), "Native Blob must own only its selected bytes");
        assertBytes(new Uint8Array(await blob.arrayBuffer()), [1, 2, 3], "Blob typed-array slice");
        const url = URL.createObjectURL(blob);
        try {
            assert(url.indexOf("blob:") === 0, "Native object URL scheme");
            const response = await fetch(url);
            assertBytes(new Uint8Array(await response.arrayBuffer()), [1, 2, 3], "Fetch native blob URL");
            assertBytes(new Uint8Array(await loadXHR(url)), [1, 2, 3], "XHR native blob URL");
        } finally {
            URL.revokeObjectURL(url);
        }
        let revoked = false;
        try {
            await fetch(url);
        } catch (error) {
            revoked = true;
        }
        assert(revoked, "Revoked blob URLs must reject fetch");

        const png = await loadXHR("app:///ReferenceImages/lite-scene1.png");
        const imageBlob = new Blob([png], { type: "image/png" });
        const imageURL = URL.createObjectURL(imageBlob);
        try {
            const image = await createImageBitmap(imageURL);
            assert(image.width === 600 && image.height === 400, "Image decoding through native blob URL");
            image.close();
        } finally {
            URL.revokeObjectURL(imageURL);
        }

        const adapter = await navigator.gpu.requestAdapter();
        const device = await adapter.requestDevice();
        const canvas = document.getElementById("renderCanvas");
        assert(canvas === document.querySelector("canvas"), "Standalone presentation canvas identity");
        const context = canvas.getContext("webgpu");
        context.configure({ device: device, format: navigator.gpu.getPreferredCanvasFormat() });
        async function checkSurface(width, height, color, expected) {
            const pixels = await new Promise(function (resolve, reject) {
                requestAnimationFrame(function () {
                    try {
                        const texture = context.getCurrentTexture();
                        assert(texture.width === width && texture.height === height, "Surface size after resize");
                        const encoder = device.createCommandEncoder();
                        const pass = encoder.beginRenderPass({
                            colorAttachments: [{
                                view: texture.createView(),
                                loadOp: "clear",
                                storeOp: "store",
                                clearValue: color
                            }]
                        });
                        pass.end();
                        device.queue.submit([encoder.finish()]);
                        TestUtils.getFrameBufferData(resolve);
                    } catch (error) {
                        reject(error);
                    }
                });
            });
            assert(pixels.length === width * height * 4, "Readback dimensions after resize");
            for (let i = 0; i < pixels.length; i++) {
                if (pixels[i] !== expected[i % 4]) {
                    throw new Error("Surface readback mismatch at byte " + i + ": " + pixels[i]);
                }
            }
            checks++;
        }
        globalThis.__dawnResize = function () { throw new Error("Typed resizing must not call a global resize hook"); };
        TestUtils.updateSize(32, 24);
        assert(canvas.width === 32 && canvas.height === 24 &&
            canvas.clientWidth === 32 && canvas.clientHeight === 24, "Typed resize updates the standalone canvas");
        await checkSurface(32, 24, { r: 1, g: 0, b: 0, a: 1 }, [255, 0, 0, 255]);
        canvas.width = 48;
        canvas.height = 16;
        await checkSurface(48, 16, { r: 0, g: 1, b: 0, a: 1 }, [0, 255, 0, 255]);
        delete globalThis.__dawnResize;

        async function checkWrite(data, offset, size, expected, message) {
            const buffer = device.createBuffer({
                size: expected.length * 4,
                usage: GPUBufferUsage.COPY_DST | GPUBufferUsage.MAP_READ
            });
            try {
                device.queue.writeBuffer(buffer, 0, data, offset, size);
                await buffer.mapAsync(GPUMapMode.READ);
                assertBytes(new Uint32Array(buffer.getMappedRange()), expected, message);
                buffer.unmap();
            } finally {
                buffer.destroy();
            }
        }
        const words = new Uint32Array([1, 2, 3, 4, 5, 6, 7, 8]);
        await checkWrite(words, 4, 4, [5, 6, 7, 8], "writeBuffer typed-array element units");
        await checkWrite(words.subarray(2), 1, 2, [4, 5], "writeBuffer typed-array byteOffset");
        await checkWrite(words, 6, undefined, [7, 8], "writeBuffer omitted element count");
        await checkWrite(words.buffer, 4, 8, [2, 3], "writeBuffer ArrayBuffer byte units");
        await checkWrite(new DataView(words.buffer, 4, 12), 4, 8, [3, 4], "writeBuffer DataView byte units");

        const buffer = device.createBuffer({ size: 32, usage: GPUBufferUsage.COPY_DST });
        try {
            assertThrows(function () { device.queue.writeBuffer(buffer, 0, words, 9); }, "writeBuffer excessive offset");
            assertThrows(function () { device.queue.writeBuffer(buffer, 0, words, 7, 2); }, "writeBuffer excessive count");
            assertThrows(function () { device.queue.writeBuffer(buffer, 0, words, -1); }, "writeBuffer negative offset");
            assertThrows(function () { device.queue.writeBuffer(buffer, 0, words, NaN); }, "writeBuffer NaN offset");
            assertThrows(function () { device.queue.writeBuffer(buffer, 0, new Uint8Array(3)); }, "writeBuffer unaligned byte count");
            assertThrows(function () { device.queue.writeBuffer(buffer, 0, "invalid"); }, "writeBuffer invalid source");
            device.queue.writeBuffer(buffer, 0, new ArrayBuffer(0));
            checks++;
        } finally {
            buffer.destroy();
        }

        clearTimeout(timeout);
        console.log("NativeDawn runtime validated: " + checks + " checks");
        TestUtils.exit(0);
    } catch (error) {
        clearTimeout(timeout);
        console.error("NATIVEDAWN_RUNTIME_TEST_FAIL: " + (error.stack || error));
        TestUtils.exit(-1);
    }
})();
