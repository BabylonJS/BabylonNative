// dawn_bootstrap.js — makes the default Playground scene scripts (experience.js
// and friends, written against BABYLON.NativeEngine) run on the Dawn/WebGPU
// backend provided by the NativeDawn plugin.
//
// Loaded by Win32/App.cpp (NativeDawn build) after babylon.max.js + addons.
// The native side installs navigator.gpu, _nativeDawnGetContext/Present/
// SurfaceSize, _nativeDawnDecodeImage, _nativeDawnReadFileBytes, and the
// standard polyfills. This script:
//   * installs the no-DOM canvas / image shims WebGPUEngine needs,
//   * drives requestAnimationFrame from the native per-frame globalThis.frame(),
//   * pre-creates and initializes a WebGPUEngine, then aliases
//     BABYLON.NativeEngine so the scene scripts' synchronous
//     `new BABYLON.NativeEngine()` returns the ready WebGPU engine,
//   * reads the scene script (default experience.js) natively and evaluates it
//     once the engine is ready.

(function () {
    "use strict";

    function log() {
        console.log("[dawn-bootstrap] " + Array.prototype.join.call(arguments, " "));
    }

    // ---- shared DOM event registry ------------------------------------------
    // Babylon's WebDeviceInputSystem attaches its pointer/keyboard listeners to
    // whichever element engine.getInputElement() returns and (for some events) to
    // window/document. To route native Win32 input regardless of which target and
    // event-name prefix ("pointer" vs "mouse") Babylon picks, canvas, document and
    // window all funnel their addEventListener calls into this single registry,
    // and __dawnInput dispatches to it.
    var g_evtListeners = {};
    function addShared(name, cb) {
        if (typeof cb !== "function") { return; }
        (g_evtListeners[name] = g_evtListeners[name] || []).push(cb);
    }
    function removeShared(name, cb) {
        var a = g_evtListeners[name];
        if (!a) { return; }
        var i = a.indexOf(cb);
        if (i !== -1) { a.splice(i, 1); }
    }
    function dispatchShared(evt) {
        var a = g_evtListeners[evt.type];
        if (!a) { return; }
        var copy = a.slice();
        for (var i = 0; i < copy.length; ++i) {
            try { copy[i].call(evt.currentTarget || null, evt); }
            catch (e) { console.error("[dawn-bootstrap] listener error: " + (e && e.stack ? e.stack : e)); }
        }
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
            addEventListener: function (name, cb) { addShared(name, cb); },
            removeEventListener: function (name, cb) { removeShared(name, cb); },
            dispatchEvent: function (evt) { dispatchShared(evt); return true; },
            // Pointer-capture no-ops (Babylon's input system may call these).
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
            addEventListener: function (name, cb) { addShared(name, cb); },
            removeEventListener: function (name, cb) { removeShared(name, cb); },
            getElementById: function () { return null; },
            body: { appendChild: function () {}, style: {} },
        };
    }
    globalThis.addEventListener = function (name, cb) { addShared(name, cb); };
    globalThis.removeEventListener = function (name, cb) { removeShared(name, cb); };

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
    // The native App.cpp frame loop calls globalThis.frame() once per iteration;
    // we flush queued rAF callbacks (which run the engine's begin/render/end,
    // submitting GPU work), then present.
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

    // The Window polyfill provides `window` (addEventListener no-op, atob, ...)
    // but not the timer / animation methods and its addEventListener drops
    // handlers. The scene scripts call window.setTimeout / window.requestAnimation
    // Frame, and Babylon's input system attaches some listeners to window; forward
    // timers to the globals (setTimeout via the Scheduling polyfill) and route
    // window/global event registration into the shared registry.
    if (globalThis.window) {
        var w = globalThis.window;
        try { w.setTimeout = globalThis.setTimeout; } catch (e) {}
        try { w.clearTimeout = globalThis.clearTimeout; } catch (e) {}
        try { w.setInterval = globalThis.setInterval; } catch (e) {}
        try { w.clearInterval = globalThis.clearInterval; } catch (e) {}
        try { w.requestAnimationFrame = globalThis.requestAnimationFrame; } catch (e) {}
        try { w.cancelAnimationFrame = globalThis.cancelAnimationFrame; } catch (e) {}
        try { w.addEventListener = function (name, cb) { addShared(name, cb); }; } catch (e) {}
        try { w.removeEventListener = function (name, cb) { removeShared(name, cb); }; } catch (e) {}
        try { w.dispatchEvent = function (evt) { dispatchShared(evt); return true; }; } catch (e) {}
    }

    // Advertise pointer/mouse/wheel event constructors. Babylon's
    // Scene.simulatePointer* build events via `new PointerEvent(type, init)`, and
    // its input system uses `window.PointerEvent` for feature detection. Provide
    // real constructors that copy the init dictionary and expose the event API.
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
    globalThis.PointerEvent = makeEventClass();
    globalThis.MouseEvent = globalThis.MouseEvent || makeEventClass();
    globalThis.WheelEvent = globalThis.WheelEvent || makeEventClass();
    if (typeof globalThis.Event === "undefined") { globalThis.Event = makeEventClass(); }
    if (globalThis.window) {
        try { globalThis.window.PointerEvent = globalThis.PointerEvent; } catch (e) {}
        try { globalThis.window.MouseEvent = globalThis.MouseEvent; } catch (e) {}
        try { globalThis.window.WheelEvent = globalThis.WheelEvent; } catch (e) {}
    }

    // ---- engine + scene load -------------------------------------------------
    var surf = (typeof _nativeDawnSurfaceSize === "function") ? _nativeDawnSurfaceSize() : { width: 1280, height: 720 };
    var canvas = makeCanvas(surf.width, surf.height);
    globalThis.__dawnCanvas = canvas;

    log("creating WebGPUEngine " + surf.width + "x" + surf.height);
    var engine = new BABYLON.WebGPUEngine(canvas, {
        antialias: false, stencil: true, premultipliedAlpha: false, enableAllFeatures: false,
    });
    engine.enableOfflineSupport = false;
    engine.disableManifestCheck = true;

    // ---- native input + resize bridge ---------------------------------------
    // Win32/App.cpp drains the message-thread pointer/resize queue each frame
    // (on the JS thread) and calls these globals. Pointer input is injected
    // through Scene.simulatePointer* which drives scene.onPointerObservable (and
    // hence the camera controls) directly, independent of the DOM/device-input
    // system selection (which doesn't attach in this no-NativeInput WebGPU host).
    var pointerIdSeq = 1;
    var lastX = 0;
    var lastY = 0;
    var PET = (typeof BABYLON !== "undefined" && BABYLON.PointerEventTypes) || {};
    function buildPointerEventInit(name, x, y, button, buttons, deltaY, movementX, movementY) {
        return {
            type: name,
            clientX: x, clientY: y,
            offsetX: x, offsetY: y,
            x: x, y: y, pageX: x, pageY: y, screenX: x, screenY: y,
            movementX: movementX, movementY: movementY,
            pointerId: pointerIdSeq,
            pointerType: "mouse",
            button: button,
            buttons: buttons,
            deltaX: 0, deltaY: deltaY, deltaZ: 0, deltaMode: 0,
            detail: 0,
            ctrlKey: false, shiftKey: false, altKey: false, metaKey: false,
            preventDefault: function () {}, stopPropagation: function () {},
            stopImmediatePropagation: function () {},
        };
    }
    globalThis.__dawnInput = function (type, x, y, button, buttons, deltaY) {
        var sc = globalThis.__dawnEngine && globalThis.__dawnEngine.scenes && globalThis.__dawnEngine.scenes[0];
        if (!sc) { return; }
        var movementX = x - lastX;
        var movementY = y - lastY;
        lastX = x;
        lastY = y;
        if (type === "pointerdown") { pointerIdSeq++; }
        try {
            var pick = new BABYLON.PickingInfo();
            if (type === "pointerdown") {
                sc.simulatePointerDown(pick, buildPointerEventInit("pointerdown", x, y, button, buttons, deltaY, movementX, movementY));
            } else if (type === "pointermove") {
                sc.simulatePointerMove(pick, buildPointerEventInit("pointermove", x, y, button, buttons, deltaY, movementX, movementY));
            } else if (type === "pointerup") {
                sc.simulatePointerUp(pick, buildPointerEventInit("pointerup", x, y, button, buttons, deltaY, movementX, movementY));
            } else if (type === "wheel" && sc.onPointerObservable && PET.POINTERWHEEL !== undefined) {
                var wevt = buildPointerEventInit("wheel", x, y, button, buttons, deltaY, movementX, movementY);
                var pi = new BABYLON.PointerInfo(PET.POINTERWHEEL, wevt, pick);
                sc.onPointerObservable.notifyObservers(pi, PET.POINTERWHEEL);
            }
        } catch (e) {
            console.error("[dawn-bootstrap] input error: " + (e && e.stack ? e.stack : e));
        }
    };

    globalThis.__dawnResize = function (w, h) {
        canvas.width = w;
        canvas.height = h;
        canvas.clientWidth = w;
        canvas.clientHeight = h;
        if (globalThis.__dawnEngine) {
            try { globalThis.__dawnEngine.setSize(w, h, true); }
            catch (e) { console.error("[dawn-bootstrap] setSize error: " + (e && e.stack ? e.stack : e)); }
        }
        // Some Babylon code listens for the window "resize" event to re-read size.
        if (globalThis.window && typeof globalThis.window.dispatchEvent === "function") {
            try { globalThis.window.dispatchEvent({ type: "resize" }); } catch (e) {}
        }
    };

    function loadSceneSource() {
        var path = globalThis.__sceneFsPath;
        if (!path || typeof _nativeDawnReadFileBytes !== "function") {
            return null;
        }
        var bytes = _nativeDawnReadFileBytes(path);
        if (!bytes) {
            return null;
        }
        return new TextDecoder("utf-8").decode(new Uint8Array(bytes));
    }

    engine.initAsync().then(function () {
        globalThis.__dawnEngine = engine;
        // The scene scripts construct their engine with `new BABYLON.NativeEngine()`;
        // return the ready WebGPU engine instead so they run unmodified on WebGPU.
        BABYLON.NativeEngine = function () { return globalThis.__dawnEngine; };
        log("WebGPUEngine ready (" + engine.getCaps().maxTextureSize + " max tex); running scene");
        var code = loadSceneSource();
        if (code === null) {
            console.error("[dawn-bootstrap] could not read scene source at " + globalThis.__sceneFsPath);
            return;
        }
        (0, eval)(code + "\n//# sourceURL=" + (globalThis.__sceneName || "scene.js") + "\n");

        // Ensure each scene's InputManager is attached so simulatePointer* input
        // (from __dawnInput) is processed. The scene scripts attach camera
        // controls but don't necessarily call scene.attachControl().
        setTimeout(function () {
            (engine.scenes || []).forEach(function (sc) {
                try { if (sc.attachControl) { sc.attachControl(); } }
                catch (e) { console.error("[dawn-bootstrap] attachControl error: " + e); }
            });
        }, 0);
    }).catch(function (e) {
        console.error("[dawn-bootstrap] init/scene load failed: " + (e && e.stack ? e.stack : e));
    });
})();
