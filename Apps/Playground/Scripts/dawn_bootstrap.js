// dawn_bootstrap.js — makes the standard Playground scene scripts (experience.js
// and friends, written against BABYLON.NativeEngine) run on the Dawn/WebGPU
// backend provided by the NativeDawn plugin.
//
// This script is evaluated by the NativeDawn plugin (Babylon::Plugins::
// NativeDawn::Initialize) at global scope, BEFORE babylon.max.js and the scene
// scripts load. The native side has already installed navigator.gpu,
// _nativeDawnGetContext/Present/SurfaceSize, _nativeDawnDecodeImage,
// _nativeDawnReadFileBytes, and the standard polyfills. This script:
//   * installs the no-DOM canvas / image shims WebGPUEngine needs,
//   * drives requestAnimationFrame from the native per-frame globalThis.frame()
//     (pumped by the Embedding View::RenderFrame),
//   * installs a __dawnResize hook (called by Embedding View::Resize),
//   * defers WebGPUEngine creation until babylon.max.js has defined BABYLON,
//     then pre-creates + initializes a WebGPUEngine and aliases
//     BABYLON.NativeEngine so the scene scripts' synchronous
//     `new BABYLON.NativeEngine()` returns the ready WebGPU engine.
//
// Input is NOT handled here: it flows through the generic Win32 host
// (App.cpp WndProc -> Embedding View::OnMouse* -> NativeInput ->
// _native.DeviceInputSystem), identical to the bgfx/NativeEngine backend.

(function () {
    "use strict";

    function log() {
        console.log("[dawn-bootstrap] " + Array.prototype.join.call(arguments, " "));
    }

    // ---- no-DOM canvas / DOM shims ------------------------------------------
    function makeCanvas(width, height) {
        var canvas = {
            width: width,
            height: height,
            clientWidth: width,
            clientHeight: height,
            style: {},
            getContext: function (type) {
                if (type === "webgpu") {
                    return _nativeDawnGetContext();
                }
                return null;
            },
            getBoundingClientRect: function () {
                return { x: 0, y: 0, left: 0, top: 0, right: this.width, bottom: this.height, width: this.width, height: this.height };
            },
            setAttribute: function () {},
            removeAttribute: function () {},
            addEventListener: function () {},
            removeEventListener: function () {},
            dispatchEvent: function () { return true; },
            setPointerCapture: function () {},
            releasePointerCapture: function () {},
            hasPointerCapture: function () { return false; },
            focus: function () {},
            getRootNode: function () { return this; },
        };
        return canvas;
    }

    if (typeof globalThis.document === "undefined") {
        globalThis.document = {
            createElement: function (tag) {
                if (tag === "canvas") { return makeCanvas(1280, 720); }
                if (tag === "img") { return new globalThis.Image(); }
                return { style: {}, setAttribute: function () {}, appendChild: function () {} };
            },
            addEventListener: function () {},
            removeEventListener: function () {},
            getElementById: function () { return null; },
            body: { appendChild: function () {}, style: {} },
        };
    }
    globalThis.addEventListener = function () {};
    globalThis.removeEventListener = function () {};

    if (typeof globalThis.location === "undefined") {
        globalThis.location = {
            href: "file:///", origin: "file://", protocol: "file:", host: "", hostname: "",
            pathname: "/", search: "", hash: "",
        };
    }

    // ---- image decoding via the native bimg decoder --------------------------
    (function installImageShim() {
        var blobRegistry = {};
        var blobSeq = 0;
        if (typeof URL !== "undefined") {
            if (typeof URL.createObjectURL !== "function") {
                URL.createObjectURL = function (blob) {
                    var id = "blob:nativedawn/" + (++blobSeq);
                    blobRegistry[id] = blob;
                    return id;
                };
            }
            if (typeof URL.revokeObjectURL !== "function") {
                URL.revokeObjectURL = function (url) { delete blobRegistry[url]; };
            }
        }
        function toArrayBuffer(src) {
            if (src instanceof ArrayBuffer) return Promise.resolve(src);
            if (ArrayBuffer.isView(src)) return Promise.resolve(src.buffer.slice(src.byteOffset, src.byteOffset + src.byteLength));
            if (src && typeof src.arrayBuffer === "function") return src.arrayBuffer();
            if (typeof src === "string") {
                var blob = blobRegistry[src];
                if (blob) return toArrayBuffer(blob);
                return fetch(src).then(function (r) { return r.arrayBuffer(); });
            }
            return Promise.reject(new Error("toArrayBuffer: unsupported source"));
        }
        if (typeof globalThis.createImageBitmap === "undefined") {
            globalThis.createImageBitmap = function (src) {
                return Promise.resolve().then(function () {
                    if (src && src.__pixels) return src;
                    return toArrayBuffer(src).then(function (ab) {
                        if (!ab || ab.byteLength === 0) {
                            return { width: 1, height: 1, __pixels: new ArrayBuffer(4), close: function () {} };
                        }
                        var bmp = _nativeDawnDecodeImage(ab);
                        bmp.close = function () {};
                        return bmp;
                    });
                });
            };
        }
        function ImageShim() {
            var img = {
                width: 0, height: 0, naturalWidth: 0, naturalHeight: 0,
                __pixels: null, onload: null, onerror: null, crossOrigin: null,
                complete: false, _src: "",
                decode: function () { return Promise.resolve(); },
                addEventListener: function (name, cb) { if (name === "load") img.onload = cb; else if (name === "error") img.onerror = cb; },
                removeEventListener: function () {},
            };
            Object.defineProperty(img, "src", {
                get: function () { return img._src; },
                set: function (v) {
                    img._src = v;
                    toArrayBuffer(v).then(function (ab) {
                        var d = _nativeDawnDecodeImage(ab);
                        img.width = img.naturalWidth = d.width;
                        img.height = img.naturalHeight = d.height;
                        img.__pixels = d.__pixels;
                        img.complete = true;
                        if (typeof img.onload === "function") img.onload({ target: img });
                    }).catch(function (e) {
                        if (typeof img.onerror === "function") img.onerror(e);
                    });
                },
            });
            return img;
        }
        if (typeof globalThis.Image === "undefined") { globalThis.Image = ImageShim; }
        if (typeof globalThis.HTMLImageElement === "undefined") { globalThis.HTMLImageElement = ImageShim; }
    })();

    // ---- requestAnimationFrame pump ------------------------------------------
    // WebGPUEngine.runRenderLoop schedules its frame via requestAnimationFrame.
    // The Embedding View::RenderFrame calls globalThis.frame() once per host
    // frame (on the JS thread); we flush queued rAF callbacks (which run the
    // engine's begin/render/end, submitting GPU work), then present.
    var rafQueue = [];
    globalThis.requestAnimationFrame = function (cb) { rafQueue.push(cb); return rafQueue.length; };
    globalThis.cancelAnimationFrame = function () {};
    globalThis.frame = function () {
        var q = rafQueue;
        rafQueue = [];
        for (var i = 0; i < q.length; ++i) {
            try { q[i](performance.now()); }
            catch (e) { console.error("[dawn-bootstrap] rAF error: " + (e && e.stack ? e.stack : e)); }
        }
        if (typeof _nativeDawnPresent === "function") { _nativeDawnPresent(); }
    };

    // The Window polyfill provides `window` but not the timer / animation
    // methods; forward them to the globals so scene scripts that call
    // window.setTimeout / window.requestAnimationFrame work.
    if (globalThis.window) {
        var w = globalThis.window;
        try { w.setTimeout = globalThis.setTimeout; } catch (e) {}
        try { w.clearTimeout = globalThis.clearTimeout; } catch (e) {}
        try { w.setInterval = globalThis.setInterval; } catch (e) {}
        try { w.clearInterval = globalThis.clearInterval; } catch (e) {}
        try { w.requestAnimationFrame = globalThis.requestAnimationFrame; } catch (e) {}
        try { w.cancelAnimationFrame = globalThis.cancelAnimationFrame; } catch (e) {}
        try { w.addEventListener = function () {}; } catch (e) {}
        try { w.removeEventListener = function () {}; } catch (e) {}
        try { w.dispatchEvent = function () { return true; }; } catch (e) {}
    }

    // Some Babylon input feature-detection reads window.PointerEvent etc.
    // Provide inert constructors so detection succeeds without a DOM.
    function makeEventClass() {
        return function (type, init) {
            init = init || {};
            this.type = type;
            for (var k in init) { if (Object.prototype.hasOwnProperty.call(init, k)) { this[k] = init[k]; } }
            if (typeof this.preventDefault !== "function") { this.preventDefault = function () {}; }
            if (typeof this.stopPropagation !== "function") { this.stopPropagation = function () {}; }
            if (typeof this.stopImmediatePropagation !== "function") { this.stopImmediatePropagation = function () {}; }
        };
    }
    globalThis.PointerEvent = globalThis.PointerEvent || makeEventClass();
    globalThis.MouseEvent = globalThis.MouseEvent || makeEventClass();
    globalThis.WheelEvent = globalThis.WheelEvent || makeEventClass();
    if (typeof globalThis.Event === "undefined") { globalThis.Event = makeEventClass(); }
    if (globalThis.window) {
        try { globalThis.window.PointerEvent = globalThis.PointerEvent; } catch (e) {}
        try { globalThis.window.MouseEvent = globalThis.MouseEvent; } catch (e) {}
        try { globalThis.window.WheelEvent = globalThis.WheelEvent; } catch (e) {}
    }

    // ---- resize bridge -------------------------------------------------------
    // Embedding View::Resize dispatches NativeDawn::ResizeSurface then calls this
    // (on the JS thread) so the engine's drawing buffer matches the surface.
    var g_canvas = null;
    globalThis.__dawnResize = function (width, height) {
        if (g_canvas) {
            g_canvas.width = width;
            g_canvas.height = height;
            g_canvas.clientWidth = width;
            g_canvas.clientHeight = height;
        }
        if (globalThis.__dawnEngine) {
            try { globalThis.__dawnEngine.setSize(width, height, true); }
            catch (e) { console.error("[dawn-bootstrap] setSize error: " + (e && e.stack ? e.stack : e)); }
        }
    };

    // ---- deferred WebGPUEngine creation --------------------------------------
    // This script runs before babylon.max.js, so BABYLON.WebGPUEngine does not
    // exist yet. Hook the global BABYLON assignment (babylon.max.js's UMD does
    // `global.BABYLON = factory()`); when it lands, pre-create + initialize a
    // WebGPUEngine and alias BABYLON.NativeEngine so the scene scripts'
    // synchronous `new BABYLON.NativeEngine()` returns the ready WebGPU engine.
    var g_started = false;
    function onBabylonReady(BABYLON) {
        if (g_started || !BABYLON || !BABYLON.WebGPUEngine) { return; }
        g_started = true;

        var surf = (typeof _nativeDawnSurfaceSize === "function") ? _nativeDawnSurfaceSize() : { width: 1280, height: 720 };
        g_canvas = makeCanvas(surf.width, surf.height);
        globalThis.__dawnCanvas = g_canvas;

        log("creating WebGPUEngine " + surf.width + "x" + surf.height);
        var engine = new BABYLON.WebGPUEngine(g_canvas, {
            antialias: false, stencil: true, premultipliedAlpha: false, enableAllFeatures: false,
        });
        engine.enableOfflineSupport = false;
        engine.disableManifestCheck = true;

        engine.initAsync().then(function () {
            globalThis.__dawnEngine = engine;
            // Scene scripts construct their engine with `new BABYLON.NativeEngine()`;
            // return the ready WebGPU engine instead so they run unmodified.
            BABYLON.NativeEngine = function () { return globalThis.__dawnEngine; };
            log("WebGPUEngine ready (" + engine.getCaps().maxTextureSize + " max tex)");
        }).catch(function (e) {
            console.error("[dawn-bootstrap] engine init failed: " + (e && e.stack ? e.stack : e));
        });
    }

    var _BABYLON = globalThis.BABYLON;
    if (_BABYLON && _BABYLON.WebGPUEngine) {
        onBabylonReady(_BABYLON);
    } else {
        Object.defineProperty(globalThis, "BABYLON", {
            configurable: true,
            get: function () { return _BABYLON; },
            set: function (v) {
                _BABYLON = v;
                try { onBabylonReady(v); }
                catch (e) { console.error("[dawn-bootstrap] onBabylonReady error: " + (e && e.stack ? e.stack : e)); }
            },
        });
    }
})();
