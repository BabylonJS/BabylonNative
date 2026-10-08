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
            expected.every(function (value, index) { return actual[index] === value; }),
            message + ": actual=" + JSON.stringify(Array.from(actual)) + " expected=" + JSON.stringify(expected));
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
        assert(!canvas.hasAttribute("tabindex") && canvas.getAttribute("tabindex") === null, "Missing canvas attribute");
        canvas.tabIndex = 0;
        assert(canvas.hasAttribute("TABINDEX") && canvas.getAttribute("tabindex") === "0", "Reflected canvas tabIndex");
        canvas.tabIndex = -2;
        assert(canvas.tabIndex === -2 && canvas.getAttribute("tabindex") === "-2", "Signed canvas tabIndex reflection");
        canvas.removeAttribute("tabindex");
        assert(!canvas.hasAttribute("tabindex") && canvas.tabIndex === -1, "Removed canvas tabIndex");
        canvas.setAttribute("data-animation-frozen", true);
        assert(canvas.dataset.animationFrozen === "true", "Canvas data attribute to dataset");
        canvas.dataset.animationFrozen = false;
        assert(canvas.getAttribute("data-animation-frozen") === "false", "Canvas dataset to data attribute");
        delete canvas.dataset.animationFrozen;
        assert(!canvas.hasAttribute("data-animation-frozen"), "Deleted dataset attribute");
        canvas.setAttribute("data-empty", "");
        assert(canvas.hasAttribute("data-empty") && canvas.getAttribute("data-empty") === "", "Present empty canvas attribute");
        canvas.removeAttribute("data-empty");
        assert(canvas.getAttribute("data-empty") === null, "Absent and empty attributes differ");
        canvas.setAttribute("__proto__", "attribute");
        assert(canvas.getAttribute("__proto__") === "attribute", "Canvas attribute names do not mutate prototypes");
        canvas.removeAttribute("__proto__");
        assertThrows(function () { canvas.setAttribute("bad name", "value"); }, "Invalid canvas attribute name");
        _native.Canvas.loadTTF("dawn-runtime-fallback", await loadXHR("app:///Scripts/DroidSans.ttf"));
        const textCanvas = document.createElement("canvas");
        const textContext = textCanvas.getContext("2d");
        textContext.font = "50px unavailable-family";
        assert(textContext.measureText("WW").width > textContext.measureText("ii").width * 2,
            "Canvas fallback-font measurement uses actual proportional glyph advances");
        const textMetrics = textContext.measureText("Hello world");
        assert(Number.isFinite(textMetrics.actualBoundingBoxAscent) && textMetrics.actualBoundingBoxAscent > 0 &&
            Number.isFinite(textMetrics.actualBoundingBoxDescent) &&
            textMetrics.fontBoundingBoxAscent > 0 && textMetrics.width > 0,
            "Canvas fallback-font measurement exposes actual ink and font extents");
        const originalFont = textContext.font;
        textContext.save();
        textContext.font = "10px dawn-runtime-fallback";
        textContext.restore();
        assert(textContext.font === originalFont &&
            textContext.measureText("Hello world").width === textMetrics.width,
            "Canvas restore keeps the measured and rendered font state synchronized");
        const colors = document.createElement("canvas");
        colors.width = 2;
        colors.height = 1;
        const colorContext = colors.getContext("2d");
        colorContext.fillStyle = "hsl(90,60%,30%)";
        colorContext.fillRect(0, 0, 1, 1);
        colorContext.fillStyle = "hsl(270,60%,30%)";
        colorContext.fillRect(1, 0, 1, 1);
        assertBytes(colorContext.getImageData(0, 0, 2, 1).data, [77, 122, 31, 255, 77, 31, 122, 255],
            "Canvas HSL byte conversion matches CSS rather than GPU ties-to-even rounding");
        const gradients = document.createElement("canvas");
        gradients.width = 32;
        gradients.height = 32;
        const gradientContext = gradients.getContext("2d");
        const linearGradient = gradientContext.createLinearGradient(0, 0, 32, 0);
        linearGradient.addColorStop(0, "black");
        linearGradient.addColorStop(1, "white");
        gradientContext.fillStyle = linearGradient;
        gradientContext.fillRect(0, 0, 32, 32);
        const linearPixel = gradientContext.getImageData(7, 0, 1, 1).data;
        assert(Math.abs(linearPixel[0] - 60) <= 1 && linearPixel[0] === linearPixel[1] &&
            linearPixel[1] === linearPixel[2] && linearPixel[3] === 255,
            "Canvas gradient quantization does not systematically truncate channel values");
        gradientContext.clearRect(0, 0, 32, 32);
        const radialGradient = gradientContext.createRadialGradient(16, 16, 0, 16, 16, 14);
        radialGradient.addColorStop(0, "hsla(90,60%,30%,1)");
        radialGradient.addColorStop(0.6, "hsla(90,60%,30%,0.8)");
        radialGradient.addColorStop(1, "hsla(90,60%,30%,0)");
        gradientContext.fillStyle = radialGradient;
        gradientContext.fillRect(0, 0, 32, 32);
        assert(gradientContext.getImageData(2, 16, 1, 1).data[3] === 18,
            "Canvas radial-gradient edge alpha rounds to the Chrome reference byte");
        gradientContext.clearRect(0, 0, 32, 32);
        const transparentLinear = gradientContext.createLinearGradient(0, 0, 32, 0);
        transparentLinear.addColorStop(0, "#ff0000ff");
        transparentLinear.addColorStop(1, "#00000000");
        gradientContext.fillStyle = transparentLinear;
        gradientContext.fillRect(0, 0, 32, 32);
        const transparentLinearPixel = gradientContext.getImageData(15, 16, 1, 1).data;
        assert(transparentLinearPixel[3] > 100 && transparentLinearPixel[3] < 150 &&
            Math.abs(transparentLinearPixel[0] - transparentLinearPixel[3]) <= 3,
            "Canvas linear gradients interpolate straight RGBA stops like Chrome");
        gradientContext.clearRect(0, 0, 32, 32);
        const transparentRadial = gradientContext.createRadialGradient(16, 16, 0, 16, 16, 14);
        transparentRadial.addColorStop(0, "#ff0000ff");
        transparentRadial.addColorStop(1, "#00000000");
        gradientContext.fillStyle = transparentRadial;
        gradientContext.fillRect(0, 0, 32, 32);
        const transparentRadialPixel = gradientContext.getImageData(9, 16, 1, 1).data;
        assert(transparentRadialPixel[3] > 100 && transparentRadialPixel[3] < 150 &&
            Math.abs(transparentRadialPixel[0] - transparentRadialPixel[3]) <= 3,
            "Canvas radial gradients interpolate straight RGBA stops like Chrome");
        const drawing = document.createElement("canvas");
        drawing.width = 2.5;
        assert(drawing.width === 2 && drawing.getAttribute("width") === "2", "Canvas property dimensions use integer coercion");
        drawing.width = -1;
        assert(drawing.width === 300, "Invalid canvas property dimensions use the HTML default");
        drawing.setAttribute("width", "3.2");
        assert(drawing.width === 3 && drawing.getAttribute("width") === "3.2", "Canvas dimension attributes preserve text and parse integer prefixes");
        drawing.setAttribute("width", "bad");
        assert(drawing.width === 300, "Invalid canvas dimension attributes use the HTML default");
        drawing.width = 3;
        drawing.height = 2;
        const drawingContext = drawing.getContext("2d");
        drawingContext.fillStyle = "#ff0000";
        drawingContext.fillRect(0, 0, 3, 2);
        assertBytes(drawingContext.getImageData(-1, 0, 2, 1).data, [0, 0, 0, 0, 255, 0, 0, 255],
            "Canvas getImageData captures GPU drawing and clips outside pixels");
        const drawingURL = drawing.toDataURL("image/png");
        const drawingImage = await createImageBitmap(drawingURL);
        assert(drawingImage.width === 3 && drawingImage.height === 2, "Canvas PNG contains actual dimensions");
        assertBytes(new Uint8Array(drawingImage.__pixels), [
            255, 0, 0, 255, 255, 0, 0, 255, 255, 0, 0, 255,
            255, 0, 0, 255, 255, 0, 0, 255, 255, 0, 0, 255
        ], "Canvas PNG contains rasterized pixels");
        drawingImage.close();
        drawing.width = 1;
        drawing.height = 1;
        assertBytes(drawingContext.getImageData(0, 0, 1, 1).data, [0, 0, 0, 0],
            "Canvas resizing clears the real GPU backing texture");
        const alphaCanvas = document.createElement("canvas");
        alphaCanvas.width = 2;
        alphaCanvas.height = 2;
        const alphaContext = alphaCanvas.getContext("2d");
        alphaContext.fillStyle = "rgba(255, 0, 0, 0.5)";
        alphaContext.fillRect(0, 0, 2, 1);
        alphaContext.fillStyle = "#00ff00";
        alphaContext.fillRect(0, 1, 2, 1);
        assertBytes(alphaContext.getImageData(0, 0, 2, 1).data, [255, 0, 0, 128, 255, 0, 0, 128],
            "Canvas getImageData exposes unpremultiplied colors without overlapping alpha fringes");
        const alphaBitmap = await createImageBitmap(alphaCanvas.toDataURL());
        assertBytes(new Uint8Array(alphaBitmap.__pixels).subarray(0, 4), [255, 0, 0, 128],
            "Canvas PNG preserves straight colors and alpha");
        const cropCanvas = document.createElement("canvas");
        cropCanvas.width = 2;
        cropCanvas.height = 1;
        const cropContext = cropCanvas.getContext("2d");
        cropContext.drawImage(alphaBitmap, 0, 1, 2, 1, 0, 0, 2, 1);
        assertBytes(cropContext.getImageData(0, 0, 2, 1).data, [0, 255, 0, 255, 0, 255, 0, 255],
            "Canvas drawImage source cropping is rasterized rather than returned from a CPU mirror");
        async function readFirstPixel(texture, byteLength) {
            const buffer = device.createBuffer({size: 256, usage: GPUBufferUsage.COPY_DST | GPUBufferUsage.MAP_READ});
            try {
                const encoder = device.createCommandEncoder();
                encoder.copyTextureToBuffer({texture: texture}, {buffer: buffer, bytesPerRow: 256}, [1, 1]);
                device.queue.submit([encoder.finish()]);
                await buffer.mapAsync(GPUMapMode.READ);
                const pixel = new Uint8Array(buffer.getMappedRange()).slice(0, byteLength);
                buffer.unmap();
                return pixel;
            } finally {
                buffer.destroy();
            }
        }
        for (const format of ["rgba8unorm", "bgra8unorm", "rgba16float", "rgba32float"]) {
            for (const premultipliedAlpha of [false, true]) {
                const texture = device.createTexture({
                    size: [1, 1], format: format,
                    usage: GPUTextureUsage.COPY_SRC | GPUTextureUsage.COPY_DST | GPUTextureUsage.RENDER_ATTACHMENT
                });
                try {
                    device.queue.copyExternalImageToTexture(
                        {source: alphaBitmap, origin: [1, 0], flipY: true},
                        {texture: texture, premultipliedAlpha: premultipliedAlpha}, [1, 1]);
                    if (format === "rgba8unorm" || format === "bgra8unorm") {
                        assertBytes(await readFirstPixel(texture, 4), format === "bgra8unorm" ?
                            [0, 0, premultipliedAlpha ? 128 : 255, 128] :
                            [premultipliedAlpha ? 128 : 255, 0, 0, 128],
                            "External image source origin, cropped flipY, alpha and " + format);
                    } else {
                        const bytes = await readFirstPixel(texture, format === "rgba16float" ? 8 : 16);
                        const values = format === "rgba16float" ?
                            Array.from(new Uint16Array(bytes.buffer), function (value) {
                                return value === 0 ? 0 : Math.pow(2, (value >> 10) - 15) * (1 + (value & 1023) / 1024);
                            }) : Array.from(new Float32Array(bytes.buffer));
                        const expected = [premultipliedAlpha ? 128 / 255 : 1, 0, 0, 128 / 255];
                        assert(values.every(function (value, index) {
                            return Math.abs(value - expected[index]) < 0.0001;
                        }), "External image alpha and " + format + " conversion");
                    }
                    await device.queue.onSubmittedWorkDone();
                } finally {
                    texture.destroy();
                }
            }
        }
        assertThrows(function () {
            device.queue.copyExternalImageToTexture({source: alphaBitmap, origin: [2, 0]}, {}, [1, 1]);
        }, "External image copy beyond source bounds rejects");
        assertThrows(function () {
            device.queue.copyExternalImageToTexture({source: {}}, {}, [1, 1]);
        }, "Missing external image pixels reject");
        const tooSmall = device.createTexture({
            size: [1, 1], format: "rgba8unorm",
            usage: GPUTextureUsage.COPY_DST | GPUTextureUsage.RENDER_ATTACHMENT
        });
        device.pushErrorScope("validation");
        device.queue.copyExternalImageToTexture({source: alphaBitmap}, {texture: tooSmall}, [2, 2]);
        const copyError = await device.popErrorScope();
        assert(copyError !== null && copyError.message.length > 0,
            "External image copy reports destination bounds errors rather than clamping");
        tooSmall.destroy();
        alphaBitmap.close();
        let invalidImageRejected = false;
        try {
            await createImageBitmap(new Blob([new Uint8Array([1, 2, 3])]));
        } catch (error) {
            invalidImageRejected = true;
        }
        assert(invalidImageRejected, "Invalid image decoding rejects instead of returning a placeholder");
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

        const computeModule = device.createShaderModule({ code:
            "@id(3) override count: u32 = 2u; override enabled: bool = false;" +
            "@group(0) @binding(0) var<storage, read_write> output: array<u32>;" +
            "@compute @workgroup_size(1) fn main() { output[0] = count; output[1] = select(0u, 1u, enabled); }"
        });
        assert((await computeModule.getCompilationInfo()).messages.length === 0, "Valid shader compilation diagnostics");
        device.pushErrorScope("validation");
        const invalidModule = device.createShaderModule({ code: "not valid WGSL" });
        const invalidCompilation = await invalidModule.getCompilationInfo();
        assert(invalidCompilation.messages.some(function (message) { return message.type === "error" && message.message.length > 0; }),
            "Invalid shader compilation diagnostics are not hidden");
        const shaderError = await device.popErrorScope();
        assert(shaderError !== null && shaderError.message.length > 0, "GPU validation error scope reports shader errors");
        device.pushErrorScope("validation");
        assert(await device.popErrorScope() === null, "Empty GPU validation scope");
        let emptyScopeRejected = false;
        try {
            await device.popErrorScope();
        } catch (error) {
            emptyScopeRejected = true;
        }
        assert(emptyScopeRejected, "Popping a missing GPU error scope rejects");
        assertThrows(function () { device.pushErrorScope("invalid"); }, "Invalid GPU error scope filter");
        await device.queue.onSubmittedWorkDone();
        checks++;
        const mapping = device.createBuffer({ size: 16, usage: GPUBufferUsage.COPY_DST | GPUBufferUsage.MAP_READ });
        try {
            assert(mapping.mapState === "unmapped", "New GPU buffers report actual map state");
            device.pushErrorScope("validation");
            let mappingFailure;
            try {
                await mapping.mapAsync(GPUMapMode.READ, 4, 4);
            } catch (error) {
                mappingFailure = error;
            }
            assert(mappingFailure && mappingFailure.name === "OperationError", "GPU buffer mapping failures reject");
            assert((await device.popErrorScope())._type === "GPUValidationError", "Invalid buffer mapping reports Dawn validation");
            await mapping.mapAsync(GPUMapMode.READ);
            assert(mapping.mapState === "mapped", "Completed GPU buffer mapping reports mapped state");
            mapping.unmap();
            assert(mapping.mapState === "unmapped", "GPU buffer unmap updates map state");
            device.pushErrorScope("validation");
            let rangeFailure;
            try {
                mapping.getMappedRange();
            } catch (error) {
                rangeFailure = error;
            }
            assert(rangeFailure && rangeFailure.name === "OperationError", "Unmapped buffers do not return fake pixel buffers");
            assert(await device.popErrorScope() === null, "Mapped-range JavaScript exceptions do not create GPU validation errors");
        } finally {
            mapping.destroy();
        }
        for (const asyncPipeline of [false, true]) {
            const storage = device.createBuffer({ size: 8, usage: GPUBufferUsage.STORAGE | GPUBufferUsage.COPY_SRC });
            const readback = device.createBuffer({ size: 8, usage: GPUBufferUsage.COPY_DST | GPUBufferUsage.MAP_READ });
            try {
                const descriptor = { layout: "auto", compute: {
                    module: computeModule, entryPoint: "main", constants: { "3": 17, enabled: true }
                } };
                const pipeline = asyncPipeline ? await device.createComputePipelineAsync(descriptor) : device.createComputePipeline(descriptor);
                const bindings = device.createBindGroup({ layout: pipeline.getBindGroupLayout(0),
                    entries: [{ binding: 0, resource: { buffer: storage } }] });
                const encoder = device.createCommandEncoder();
                const pass = encoder.beginComputePass();
                pass.setPipeline(pipeline);
                pass.setBindGroup(0, bindings);
                pass.dispatchWorkgroups(1);
                pass.end();
                encoder.copyBufferToBuffer(storage, 0, readback, 0, 8);
                device.queue.submit([encoder.finish()]);
                await readback.mapAsync(GPUMapMode.READ);
                assertBytes(new Uint32Array(readback.getMappedRange()), [17, 1], "Compute numeric-ID and boolean pipeline constants");
                readback.unmap();
            } finally {
                storage.destroy();
                readback.destroy();
            }
        }
        assertThrows(function () {
            device.createComputePipeline({ layout: "auto", compute: {
                module: computeModule, entryPoint: "main", constants: { count: Infinity }
            } });
        }, "Non-finite pipeline constants reject");

        const renderModule = device.createShaderModule({ code:
            "override scale: f32 = 0.0; @id(7) override green: f32 = 0.0;" +
            "@vertex fn vs(@builtin(vertex_index) i: u32) -> @builtin(position) vec4f {" +
            "let p = array(vec2f(-1,-1), vec2f(3,-1), vec2f(-1,3)); return vec4f(p[i] * scale, 0, 1); }" +
            "@fragment fn fs() -> @location(0) vec4f { return vec4f(0, green, 0, 1); }"
        });
        for (const asyncPipeline of [false, true]) {
            const texture = device.createTexture({ size: [1, 1], format: "rgba8unorm",
                usage: GPUTextureUsage.RENDER_ATTACHMENT | GPUTextureUsage.COPY_SRC });
            const readback = device.createBuffer({ size: 256, usage: GPUBufferUsage.COPY_DST | GPUBufferUsage.MAP_READ });
            try {
                const descriptor = { layout: "auto",
                    vertex: { module: renderModule, entryPoint: "vs", constants: { scale: 1 } },
                    fragment: { module: renderModule, entryPoint: "fs", constants: { "7": 1 }, targets: [{ format: "rgba8unorm" }] }
                };
                const pipeline = asyncPipeline ? await device.createRenderPipelineAsync(descriptor) : device.createRenderPipeline(descriptor);
                const encoder = device.createCommandEncoder();
                const pass = encoder.beginRenderPass({ colorAttachments: [{
                    view: texture.createView(), loadOp: "clear", storeOp: "store", clearValue: [0, 0, 0, 1]
                }] });
                pass.setPipeline(pipeline);
                pass.draw(3);
                pass.end();
                encoder.copyTextureToBuffer({ texture: texture }, { buffer: readback, bytesPerRow: 256 }, [1, 1]);
                device.queue.submit([encoder.finish()]);
                await readback.mapAsync(GPUMapMode.READ);
                assertBytes(new Uint8Array(readback.getMappedRange()).subarray(0, 4), [0, 255, 0, 255], "Vertex and fragment pipeline constants");
                readback.unmap();
            } finally {
                texture.destroy();
                readback.destroy();
            }
        }

        const liveURL = new URL("https://example.com/scene.html?live");
        assert(liveURL.search === "?live" && liveURL.searchParams.has("live"),
            "Empty-valued URL query flags retain presence and search");
        clearTimeout(timeout);
        console.log("NativeDawn runtime validated: " + checks + " checks");
        TestUtils.exit(0);
    } catch (error) {
        clearTimeout(timeout);
        console.error("NATIVEDAWN_RUNTIME_TEST_FAIL: " + (error.stack || error));
        TestUtils.exit(-1);
    }
})();
