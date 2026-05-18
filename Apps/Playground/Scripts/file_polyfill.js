// Minimal File API polyfill for Babylon Native.
//
// The native JsRuntimeHost ships a Blob C++ ObjectWrap (window.Blob), but no
// File constructor or FileReader. Several Babylon.js serializer round-trip
// tests use
//   new File([blob], 'scene.glb')
// to wrap a serializer's Blob output before re-loading it through the scene
// loader, and the loader reads it via FileReader.readAsArrayBuffer. This
// script adds JS-level File and FileReader implementations on top of the
// native Blob, leaving Blob methods (size / type / arrayBuffer / text /
// bytes) intact.
//
// No-op when the constructors already exist (V8/JSC may expose native ones).
// No-op when window.Blob is missing (the prerequisite native polyfill).

(function (globalObject) {
    "use strict";

    if (typeof globalObject.Blob === "undefined") {
        return;
    }

    var BlobCtor = globalObject.Blob;

    if (typeof globalObject.File === "undefined") {
        var File = function File(parts, name, options) {
            if (!(this instanceof File)) {
                return new File(parts, name, options);
            }

            var opts = options || {};
            var type = typeof opts.type === "string" ? opts.type : "";
            var instance = new BlobCtor(parts || [], { type: type });

            var fileName = name === undefined || name === null ? "" : String(name);
            var lastModified = typeof opts.lastModified === "number"
                ? opts.lastModified
                : Date.now();

            // Decorate the native Blob with File-specific properties. We
            // deliberately do NOT call Object.setPrototypeOf on the Blob
            // instance: the native Blob is a napi ObjectWrap and pivoting
            // its prototype may strip its bound instance methods. Babylon's
            // File detection is duck-typed on `name` + `size`, so this is
            // sufficient for the loader path even though `instanceof File`
            // will return false.
            try {
                Object.defineProperty(instance, "name", {
                    value: fileName,
                    enumerable: true,
                    configurable: true,
                    writable: false
                });
                Object.defineProperty(instance, "lastModified", {
                    value: lastModified,
                    enumerable: true,
                    configurable: true,
                    writable: false
                });
            } catch (e) {
                instance.name = fileName;
                instance.lastModified = lastModified;
            }

            return instance;
        };

        // File.prototype intentionally not chained to Blob.prototype here:
        // construction returns the native Blob instance directly so
        // `instanceof File` is false. This keeps native bound methods
        // working without prototype pivot.
        File.prototype = {};
        File.prototype.constructor = File;

        globalObject.File = File;
    }

    if (typeof globalObject.FileReader === "undefined") {
        var EMPTY = 0;
        var LOADING = 1;
        var DONE = 2;

        var FileReader = function FileReader() {
            if (!(this instanceof FileReader)) {
                return new FileReader();
            }
            this.readyState = EMPTY;
            this.result = null;
            this.error = null;
            this.onloadstart = null;
            this.onprogress = null;
            this.onload = null;
            this.onabort = null;
            this.onerror = null;
            this.onloadend = null;
            this._listeners = {};
            // Monotonic token used to invalidate in-flight reads on abort()
            // or when a new read starts; the .then continuation in
            // startRead() captures this value and early-returns if it has
            // moved on (i.e. the read it belongs to was abandoned).
            this._readId = 0;
        };

        FileReader.EMPTY = EMPTY;
        FileReader.LOADING = LOADING;
        FileReader.DONE = DONE;
        FileReader.prototype.EMPTY = EMPTY;
        FileReader.prototype.LOADING = LOADING;
        FileReader.prototype.DONE = DONE;

        function dispatch(reader, eventType) {
            var handler = reader["on" + eventType];
            var event = {
                type: eventType,
                target: reader,
                currentTarget: reader,
                lengthComputable: false,
                loaded: reader.result ? (reader.result.byteLength || reader.result.length || 0) : 0,
                total: reader.result ? (reader.result.byteLength || reader.result.length || 0) : 0
            };
            if (typeof handler === "function") {
                try { handler.call(reader, event); } catch (e) { /* swallow */ }
            }
            var list = reader._listeners[eventType];
            if (list) {
                for (var i = 0; i < list.length; i++) {
                    try { list[i].call(reader, event); } catch (e) { /* swallow */ }
                }
            }
        }

        function startRead(reader, blob, mode) {
            if (reader.readyState === LOADING) {
                throw new Error("FileReader: read already in progress");
            }
            reader.readyState = LOADING;
            reader.result = null;
            reader.error = null;
            reader._readId = (reader._readId || 0) + 1;
            var myReadId = reader._readId;

            dispatch(reader, "loadstart");

            if (blob === null || blob === undefined) {
                reader.error = new Error("FileReader: argument is not a Blob");
                reader.readyState = DONE;
                dispatch(reader, "error");
                dispatch(reader, "loadend");
                return;
            }

            // Get an ArrayBuffer from the blob (works for native Blob, our
            // File, and any duck-typed object exposing arrayBuffer()).
            var bufPromise;
            try {
                if (typeof blob.arrayBuffer === "function") {
                    bufPromise = blob.arrayBuffer();
                } else if (blob instanceof ArrayBuffer) {
                    bufPromise = Promise.resolve(blob);
                } else {
                    bufPromise = Promise.reject(new Error("FileReader: argument has no arrayBuffer()"));
                }
            } catch (syncErr) {
                bufPromise = Promise.reject(syncErr);
            }

            Promise.resolve(bufPromise).then(function (buf) {
                if (reader._readId !== myReadId || reader.readyState !== LOADING) {
                    return;
                }
                if (mode === "arraybuffer") {
                    reader.result = buf;
                } else if (mode === "text") {
                    // Prefer TextDecoder (single-shot, native-speed). Only
                    // fall back to a manual byte->char path when TextDecoder
                    // is unavailable or throws; even the fallback uses
                    // chunked String.fromCharCode.apply + Array.join to
                    // avoid the previous O(n^2) `s += c` concatenation
                    // loop, which is expensive for multi-MB GLTF JSON.
                    var s;
                    if (typeof TextDecoder !== "undefined") {
                        try {
                            s = new TextDecoder("utf-8").decode(buf);
                        } catch (e) { /* fall through to manual fallback */ }
                    }
                    if (typeof s !== "string") {
                        var textBytes = new Uint8Array(buf);
                        var textChunkSize = 0x8000;
                        var textParts = [];
                        for (var ti = 0; ti < textBytes.length; ti += textChunkSize) {
                            textParts.push(String.fromCharCode.apply(
                                null,
                                textBytes.subarray(ti, ti + textChunkSize)
                            ));
                        }
                        s = textParts.join("");
                    }
                    reader.result = s;
                } else if (mode === "dataurl") {
                    var b64bytes = new Uint8Array(buf);
                    var chunkSize = 0x8000;
                    var binaryParts = [];
                    for (var j = 0; j < b64bytes.length; j += chunkSize) {
                        binaryParts.push(String.fromCharCode.apply(
                            null,
                            b64bytes.subarray(j, j + chunkSize)
                        ));
                    }
                    var b64;
                    try {
                        b64 = globalObject.btoa
                            ? globalObject.btoa(binaryParts.join(""))
                            : null;
                    } catch (e) { b64 = null; }
                    if (b64 === null) {
                        // No btoa available; fall back to a synthetic data
                        // URL flag so callers at least see something.
                        b64 = "";
                    }
                    var contentType = (blob && blob.type) ? blob.type : "application/octet-stream";
                    reader.result = "data:" + contentType + ";base64," + b64;
                } else if (mode === "binarystring") {
                    var bs = new Uint8Array(buf);
                    var out = "";
                    for (var k = 0; k < bs.length; k++) {
                        out += String.fromCharCode(bs[k]);
                    }
                    reader.result = out;
                } else {
                    reader.result = buf;
                }
                reader.readyState = DONE;
                dispatch(reader, "load");
                dispatch(reader, "loadend");
            }, function (err) {
                if (reader._readId !== myReadId || reader.readyState !== LOADING) {
                    return;
                }
                reader.error = err || new Error("FileReader: unknown error");
                reader.readyState = DONE;
                dispatch(reader, "error");
                dispatch(reader, "loadend");
            });
        }

        FileReader.prototype.readAsArrayBuffer = function (blob) {
            startRead(this, blob, "arraybuffer");
        };
        FileReader.prototype.readAsText = function (blob /*, encoding */) {
            startRead(this, blob, "text");
        };
        FileReader.prototype.readAsDataURL = function (blob) {
            startRead(this, blob, "dataurl");
        };
        FileReader.prototype.readAsBinaryString = function (blob) {
            startRead(this, blob, "binarystring");
        };
        FileReader.prototype.abort = function () {
            if (this.readyState !== LOADING) {
                return;
            }
            // Bump the read token so the in-flight .then continuation in
            // startRead() will early-return instead of clobbering state /
            // dispatching a phantom "load" after the user's abort.
            this._readId = (this._readId || 0) + 1;
            this.readyState = DONE;
            this.result = null;
            this.error = new Error("FileReader aborted");
            dispatch(this, "abort");
            dispatch(this, "loadend");
        };
        FileReader.prototype.addEventListener = function (type, listener) {
            if (typeof listener !== "function") {
                return;
            }
            if (!this._listeners[type]) {
                this._listeners[type] = [];
            }
            this._listeners[type].push(listener);
        };
        FileReader.prototype.removeEventListener = function (type, listener) {
            var list = this._listeners[type];
            if (!list) {
                return;
            }
            for (var i = list.length - 1; i >= 0; i--) {
                if (list[i] === listener) {
                    list.splice(i, 1);
                }
            }
        };
        FileReader.prototype.dispatchEvent = function (event) {
            // Route through the internal dispatch() helper so on* handlers
            // and listeners registered via addEventListener actually fire.
            // The internal helper constructs its own event payload (we
            // intentionally do not surface the caller's event object), so
            // this is a best-effort EventTarget-like contract sufficient
            // for duck-typed FileReader consumers.
            if (!event || typeof event.type !== "string") {
                return false;
            }
            dispatch(this, event.type);
            return true;
        };

        globalObject.FileReader = FileReader;
    }
})(typeof globalThis !== "undefined" ? globalThis : this);
