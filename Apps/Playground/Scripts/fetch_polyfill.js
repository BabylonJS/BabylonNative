// Minimal `fetch` polyfill for Babylon Native.
//
// Provides global `fetch(url, options)` returning a Promise that resolves to a
// Response-like object with `.ok`, `.status`, `.statusText`, `.url`,
// `.text()`, `.arrayBuffer()`, `.json()`, `.blob()`, and a `headers`
// stub (always empty). Internally wraps XMLHttpRequest so any URL scheme
// the host XHR can resolve (http:, https:, app:, data:, ...) works.
//
// Skipped if a `fetch` global is already defined.

(function () {
    var g = (new Function('return this'))();
    if (typeof g.fetch === 'function') {
        return;
    }

    var XHR = g.XMLHttpRequest;
    if (typeof XHR !== 'function') {
        // Nothing we can do without XHR.
        return;
    }

    function HeadersStub() {}
    HeadersStub.prototype.get = function () { return null; };
    HeadersStub.prototype.has = function () { return false; };
    HeadersStub.prototype.forEach = function () {};

    function arrayBufferToString(buf) {
        var bytes = new Uint8Array(buf);
        // Chunk to avoid call stack limits on large buffers.
        var chunkSize = 0x8000;
        var chars = [];
        for (var i = 0; i < bytes.length; i += chunkSize) {
            chars.push(String.fromCharCode.apply(null, bytes.subarray(i, i + chunkSize)));
        }
        return chars.join('');
    }

    function makeResponse(xhr, requestUrl, effectiveResponseType) {
        var bodyUsed = false;
        var cachedArrayBuffer = null;
        var cachedText = null;

        function ensureNotConsumed() {
            if (bodyUsed) {
                throw new TypeError('Body has already been consumed.');
            }
            bodyUsed = true;
        }

        function getArrayBuffer() {
            if (cachedArrayBuffer !== null) {
                return cachedArrayBuffer;
            }
            var resp = xhr.response;
            if (resp instanceof ArrayBuffer) {
                cachedArrayBuffer = resp;
            } else if (typeof resp === 'string') {
                var len = resp.length;
                var buf = new ArrayBuffer(len);
                var view = new Uint8Array(buf);
                for (var i = 0; i < len; i++) {
                    view[i] = resp.charCodeAt(i) & 0xFF;
                }
                cachedArrayBuffer = buf;
            } else if (resp && resp.byteLength !== undefined) {
                cachedArrayBuffer = resp;
            } else {
                cachedArrayBuffer = new ArrayBuffer(0);
            }
            return cachedArrayBuffer;
        }

        function getText() {
            if (cachedText !== null) {
                return cachedText;
            }
            // Prefer responseText only when the request actually used the
            // String response type; otherwise BN's XHR returns an empty
            // string and we must decode the arraybuffer ourselves.
            if (effectiveResponseType === 'text' && typeof xhr.responseText === 'string') {
                cachedText = xhr.responseText;
            } else {
                cachedText = arrayBufferToString(getArrayBuffer());
            }
            return cachedText;
        }

        var status = xhr.status || 0;
        return {
            ok: status >= 200 && status < 300,
            status: status,
            statusText: xhr.statusText || '',
            url: requestUrl,
            redirected: false,
            type: 'basic',
            headers: new HeadersStub(),
            get bodyUsed() { return bodyUsed; },
            arrayBuffer: function () {
                ensureNotConsumed();
                return Promise.resolve(getArrayBuffer());
            },
            text: function () {
                ensureNotConsumed();
                return Promise.resolve(getText());
            },
            json: function () {
                ensureNotConsumed();
                try {
                    return Promise.resolve(JSON.parse(getText()));
                } catch (e) {
                    return Promise.reject(e);
                }
            },
            blob: function () {
                ensureNotConsumed();
                var buf = getArrayBuffer();
                if (typeof g.Blob === 'function') {
                    return Promise.resolve(new g.Blob([buf]));
                }
                return Promise.resolve(buf);
            },
            clone: function () {
                throw new TypeError('Response.clone() is not supported by this polyfill.');
            }
        };
    }

    g.fetch = function (input, init) {
        init = init || {};
        var url;
        var method = (init.method || 'GET').toUpperCase();
        var body = init.body;
        var headers = init.headers;
        var responseType = init.responseType;

        if (typeof input === 'string') {
            url = input;
        } else if (input && typeof input.url === 'string') {
            url = input.url;
            method = (input.method || method).toUpperCase();
            body = input.body || body;
            headers = input.headers || headers;
        } else {
            return Promise.reject(new TypeError('fetch: invalid input.'));
        }

        return new Promise(function (resolve, reject) {
            var xhr;
            try {
                xhr = new XHR();
            } catch (e) {
                reject(e);
                return;
            }

            try {
                xhr.open(method, url, true);
            } catch (e) {
                reject(e);
                return;
            }

            var effectiveResponseType = responseType === 'text' ? 'text' : 'arraybuffer';
            try {
                xhr.responseType = effectiveResponseType;
            } catch (e) {}

            if (headers) {
                if (typeof headers.forEach === 'function') {
                    headers.forEach(function (value, name) {
                        try { xhr.setRequestHeader(name, value); } catch (e) {}
                    });
                } else {
                    for (var k in headers) {
                        if (Object.prototype.hasOwnProperty.call(headers, k)) {
                            try { xhr.setRequestHeader(k, headers[k]); } catch (e) {}
                        }
                    }
                }
            }

            // Babylon Native's XMLHttpRequest only dispatches via addEventListener
            // and only fires 'readystatechange', 'loadend', and 'error' (no 'load').
            // Inspect status in loadend to decide success/failure.
            var settled = false;
            function settleFromLoadEnd() {
                if (settled) {
                    return;
                }
                settled = true;
                var status = xhr.status || 0;
                if (status >= 200 && status < 300) {
                    resolve(makeResponse(xhr, url, effectiveResponseType));
                } else {
                    reject(new TypeError('fetch: request failed with status ' + status + ' for ' + url));
                }
            }
            function settleAsError() {
                if (settled) {
                    return;
                }
                settled = true;
                reject(new TypeError('Network request failed: ' + url));
            }

            if (typeof xhr.addEventListener === 'function') {
                xhr.addEventListener('loadend', settleFromLoadEnd);
                xhr.addEventListener('error', settleAsError);
            } else {
                xhr.onload = settleFromLoadEnd;
                xhr.onloadend = settleFromLoadEnd;
                xhr.onerror = settleAsError;
                xhr.onabort = settleAsError;
            }

            try {
                xhr.send(body || null);
            } catch (e) {
                if (!settled) {
                    settled = true;
                    reject(e);
                }
            }
        });
    };
})();
