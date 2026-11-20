(function(global) {
    "use strict";

    var headerStates = new WeakMap();
    var responseStates = new WeakMap();
    var streamTrackers = new WeakMap();
    var readerTrackers = new WeakMap();
    var iteratorTrackers = new WeakMap();
    var headerNamePattern = /^[!#$%&'*+\-.^_`|~0-9A-Za-z]+$/;
    var nullBodyStatuses = [204, 205, 304];

    function wrapMethod(target, name, createWrapper) {
        var descriptor = Object.getOwnPropertyDescriptor(target, name);
        var original = descriptor ? descriptor.value : target[name];
        if (typeof original !== "function") {
            return;
        }
        if (!descriptor) {
            descriptor = {
                configurable: true,
                enumerable: false,
                writable: true
            };
        }
        descriptor.value = createWrapper(original);
        Object.defineProperty(target, name, descriptor);
    }

    function trackStream(stream) {
        var tracker = streamTrackers.get(stream);
        if (!tracker) {
            tracker = { disturbed: false };
            streamTrackers.set(stream, tracker);
        }
        return tracker;
    }

    function trackReader(reader, tracker) {
        if (readerTrackers.has(reader)) {
            return reader;
        }
        readerTrackers.set(reader, tracker);
        for (var name of ["read", "cancel"]) {
            wrapMethod(reader, name, function(original) {
                return function() {
                    tracker.disturbed = true;
                    return original.apply(this, arguments);
                };
            });
        }
        return reader;
    }

    function trackIterator(iterator, tracker) {
        if (iteratorTrackers.has(iterator)) {
            return iterator;
        }
        iteratorTrackers.set(iterator, tracker);
        for (var name of ["next", "return", "throw"]) {
            wrapMethod(iterator, name, function(original) {
                return function() {
                    tracker.disturbed = true;
                    return original.apply(this, arguments);
                };
            });
        }
        return iterator;
    }

    function instrumentReadableStreams() {
        if (global.ReadableStream === undefined || global.ReadableStream === null) {
            return;
        }
        var prototype = global.ReadableStream.prototype;
        wrapMethod(prototype, "getReader", function(original) {
            return function() {
                return trackReader(original.apply(this, arguments), trackStream(this));
            };
        });
        for (var name of ["cancel", "pipeTo", "pipeThrough", "tee"]) {
            wrapMethod(prototype, name, function(original) {
                return function() {
                    var result = original.apply(this, arguments);
                    trackStream(this).disturbed = true;
                    return result;
                };
            });
        }
        wrapMethod(prototype, "values", function(original) {
            return function() {
                return trackIterator(original.apply(this, arguments), trackStream(this));
            };
        });
        if (typeof Symbol.asyncIterator === "symbol") {
            wrapMethod(prototype, Symbol.asyncIterator, function(original) {
                return function() {
                    return trackIterator(original.apply(this, arguments), trackStream(this));
                };
            });
        }
    }

    instrumentReadableStreams();

    function requireHeaderState(value) {
        var state = headerStates.get(value);
        if (!state) {
            throw new TypeError("Illegal invocation");
        }
        return state;
    }

    function requireResponseState(value) {
        var state = responseStates.get(value);
        if (!state) {
            throw new TypeError("Illegal invocation");
        }
        return state;
    }

    function toByteString(value, description) {
        var string = String(value);
        for (var index = 0; index < string.length; ++index) {
            if (string.charCodeAt(index) > 255) {
                throw new TypeError(description + " is not a valid ByteString");
            }
        }
        return string;
    }

    function normalizeHeaderName(value) {
        var name = toByteString(value, "Header name");
        if (!headerNamePattern.test(name)) {
            throw new TypeError("Invalid header name");
        }
        return name.toLowerCase();
    }

    function normalizeHeaderValue(value) {
        var normalized = toByteString(value, "Header value")
            .replace(/^[\t\n\r ]+|[\t\n\r ]+$/g, "");
        if (/[\0\r\n]/.test(normalized)) {
            throw new TypeError("Invalid header value");
        }
        return normalized;
    }

    function isForbiddenResponseHeader(name) {
        return name === "set-cookie" || name === "set-cookie2";
    }

    function appendHeader(state, name, value) {
        if (state.guard === "response" && isForbiddenResponseHeader(name)) {
            return;
        }
        state.list.push([name, value]);
        state.version++;
    }

    function removeHeader(state, name) {
        var writeIndex = 0;
        var removed = false;
        for (var readIndex = 0; readIndex < state.list.length; ++readIndex) {
            var entry = state.list[readIndex];
            if (entry[0] === name) {
                removed = true;
            } else {
                state.list[writeIndex++] = entry;
            }
        }
        state.list.length = writeIndex;
        return removed;
    }

    function fillHeaders(target, init) {
        var state = requireHeaderState(target);
        if (init === undefined) {
            return;
        }
        if ((typeof init !== "object" && typeof init !== "function") || init === null) {
            throw new TypeError("Headers initializer must be an object");
        }

        var iteratorMethod = init[Symbol.iterator];
        if (iteratorMethod !== undefined) {
            if (typeof iteratorMethod !== "function") {
                throw new TypeError("Headers initializer is not iterable");
            }
            for (var entry of init) {
                if ((typeof entry !== "object" && typeof entry !== "function") || entry === null) {
                    throw new TypeError("Each header pair must be iterable");
                }
                var pair = Array.from(entry);
                if (pair.length !== 2) {
                    throw new TypeError("Each header pair must contain exactly two items");
                }
                appendHeader(state, normalizeHeaderName(pair[0]), normalizeHeaderValue(pair[1]));
            }
            return;
        }

        for (var name of Object.keys(init)) {
            appendHeader(state, normalizeHeaderName(name), normalizeHeaderValue(init[name]));
        }
    }

    function combinedHeaderEntries(state) {
        if (state.cacheVersion === state.version) {
            return state.cache;
        }
        var valuesByName = new Map();
        for (var entry of state.list) {
            var values = valuesByName.get(entry[0]);
            if (!values) {
                values = [];
                valuesByName.set(entry[0], values);
            }
            values.push(entry[1]);
        }

        var result = [];
        var names = Array.from(valuesByName.keys()).sort();
        for (var name of names) {
            var values = valuesByName.get(name);
            if (name === "set-cookie") {
                for (var value of values) {
                    result.push([name, value]);
                }
            } else {
                result.push([name, values.join(", ")]);
            }
        }
        state.cache = result;
        state.cacheVersion = state.version;
        return state.cache;
    }

    function* iterateHeaders(headers, kind) {
        var index = 0;
        while (true) {
            var entries = combinedHeaderEntries(requireHeaderState(headers));
            if (index >= entries.length) {
                return;
            }
            var entry = entries[index++];
            if (kind === "key") {
                yield entry[0];
            } else if (kind === "value") {
                yield entry[1];
            } else {
                yield [entry[0], entry[1]];
            }
        }
    }

    class Headers {
        constructor(init) {
            headerStates.set(this, {
                cache: [],
                cacheVersion: -1,
                guard: "none",
                list: [],
                version: 0
            });
            fillHeaders(this, init);
        }

        append(name, value) {
            var state = requireHeaderState(this);
            appendHeader(state, normalizeHeaderName(name), normalizeHeaderValue(value));
        }

        delete(name) {
            var state = requireHeaderState(this);
            name = normalizeHeaderName(name);
            if (state.guard === "response" && isForbiddenResponseHeader(name)) {
                return;
            }
            if (removeHeader(state, name)) {
                state.version++;
            }
        }

        get(name) {
            var state = requireHeaderState(this);
            name = normalizeHeaderName(name);
            var value = null;
            for (var entry of state.list) {
                if (entry[0] === name) {
                    value = value === null ? entry[1] : value + ", " + entry[1];
                }
            }
            return value;
        }

        getSetCookie() {
            var state = requireHeaderState(this);
            var values = [];
            for (var entry of state.list) {
                if (entry[0] === "set-cookie") {
                    values.push(entry[1]);
                }
            }
            return values;
        }

        has(name) {
            var state = requireHeaderState(this);
            name = normalizeHeaderName(name);
            return state.list.some(function(entry) { return entry[0] === name; });
        }

        set(name, value) {
            var state = requireHeaderState(this);
            name = normalizeHeaderName(name);
            value = normalizeHeaderValue(value);
            if (state.guard === "response" && isForbiddenResponseHeader(name)) {
                return;
            }
            removeHeader(state, name);
            state.list.push([name, value]);
            state.version++;
        }

        entries() {
            requireHeaderState(this);
            return iterateHeaders(this, "entry");
        }

        keys() {
            requireHeaderState(this);
            return iterateHeaders(this, "key");
        }

        values() {
            requireHeaderState(this);
            return iterateHeaders(this, "value");
        }

        forEach(callback, thisArg) {
            requireHeaderState(this);
            if (typeof callback !== "function") {
                throw new TypeError("Headers.forEach callback must be a function");
            }
            for (var entry of this) {
                callback.call(thisArg, entry[1], entry[0], this);
            }
        }

        [Symbol.iterator]() {
            return this.entries();
        }
    }

    Object.defineProperty(Headers.prototype, Symbol.toStringTag, {
        configurable: true,
        value: "Headers"
    });

    function createResponseHeaders(init) {
        var headers = new Headers(init);
        var state = requireHeaderState(headers);
        state.guard = "response";
        var removedForbidden = removeHeader(state, "set-cookie");
        removedForbidden = removeHeader(state, "set-cookie2") || removedForbidden;
        if (removedForbidden) {
            state.version++;
        }
        return headers;
    }

    function isReadableStream(value) {
        return global.ReadableStream !== undefined && global.ReadableStream !== null &&
            value instanceof global.ReadableStream;
    }

    function streamFromBytes(bytes) {
        var chunk = bytes;
        return new global.ReadableStream({
            type: "bytes",
            pull: function(controller) {
                if (chunk !== null) {
                    var value = chunk;
                    chunk = null;
                    if (value.byteLength !== 0) {
                        controller.enqueue(value);
                    }
                }
                controller.close();
            },
            cancel: function() {
                chunk = null;
            }
        });
    }

    function copyBufferSource(value) {
        var source;
        if (value instanceof ArrayBuffer) {
            source = new Uint8Array(value);
        } else {
            source = new Uint8Array(value.buffer, value.byteOffset, value.byteLength);
        }
        var copy = new Uint8Array(source.byteLength);
        copy.set(source);
        return copy;
    }

    function extractBody(body) {
        if (body === null || body === undefined) {
            return { stream: null, contentType: null };
        }
        if (isReadableStream(body)) {
            if (body.locked || trackStream(body).disturbed) {
                throw new TypeError("Response body stream is already disturbed or locked");
            }
            return { stream: body, contentType: null };
        }
        if (global.Blob !== undefined && global.Blob !== null && body instanceof global.Blob) {
            return { stream: body.stream(), contentType: body.type || null };
        }
        if (body instanceof ArrayBuffer || ArrayBuffer.isView(body)) {
            return { stream: streamFromBytes(copyBufferSource(body)), contentType: null };
        }
        if (global.URLSearchParams !== undefined && global.URLSearchParams !== null && body instanceof global.URLSearchParams) {
            return {
                stream: streamFromBytes(new global.TextEncoder().encode(body.toString())),
                contentType: "application/x-www-form-urlencoded;charset=UTF-8"
            };
        }
        if (global.FormData !== undefined && global.FormData !== null && body instanceof global.FormData) {
            throw new TypeError("FormData response bodies are not supported by this runtime");
        }

        return {
            stream: streamFromBytes(new global.TextEncoder().encode(String(body))),
            contentType: "text/plain;charset=UTF-8"
        };
    }

    function bodyIsUnusable(state) {
        return state.used || (state.body !== null && (state.body.locked || state.bodyTracker.disturbed));
    }

    async function readBodyChunks(state) {
        if (state.body === null) {
            return [[], 0];
        }
        if (bodyIsUnusable(state)) {
            throw new TypeError("Response body is already used");
        }

        state.used = true;
        var chunks = [];
        var total = 0;
        var reader = state.body.getReader();
        while (true) {
            var result = await reader.read();
            if (result.done) {
                return [chunks, total];
            }
            if (!(result.value instanceof Uint8Array)) {
                throw new TypeError("Response body stream yielded a non-Uint8Array chunk");
            }
            if (result.value.byteLength === 0) {
                continue;
            }
            if (total > Number.MAX_SAFE_INTEGER - result.value.byteLength) {
                throw new RangeError("Response body is too large");
            }
            chunks.push(result.value);
            total += result.value.byteLength;
        }
    }

    function joinChunks(chunksAndTotal) {
        var chunks = chunksAndTotal[0];
        var total = chunksAndTotal[1];
        if (chunks.length === 0) {
            return new Uint8Array(0);
        }
        if (chunks.length === 1) {
            return chunks[0];
        }

        var bytes = new Uint8Array(total);
        var offset = 0;
        for (var chunk of chunks) {
            bytes.set(chunk, offset);
            offset += chunk.byteLength;
        }
        return bytes;
    }

    async function consumeText(state) {
        if (state.body === null) {
            return "";
        }
        if (bodyIsUnusable(state)) {
            throw new TypeError("Response body is already used");
        }

        state.used = true;
        var decoder = new global.TextDecoder();
        var pieces = [];
        var reader = state.body.getReader();
        while (true) {
            var result = await reader.read();
            if (result.done) {
                pieces.push(decoder.decode());
                return pieces.join("");
            }
            if (!(result.value instanceof Uint8Array)) {
                throw new TypeError("Response body stream yielded a non-Uint8Array chunk");
            }
            if (result.value.byteLength !== 0) {
                pieces.push(decoder.decode(result.value, { stream: true }));
            }
        }
    }

    function validateResponseInit(init) {
        if (init === undefined || init === null) {
            init = {};
        } else if (typeof init !== "object" && typeof init !== "function") {
            throw new TypeError("Response init must be an object");
        }

        var status = init.status === undefined ? 200 : Math.trunc(Number(init.status));
        if (!Number.isFinite(status) || status < 200 || status > 599) {
            throw new RangeError("Response status must be between 200 and 599");
        }
        var statusText = init.statusText === undefined ? "" :
            toByteString(init.statusText, "Response statusText");
        if (/[\r\n]/.test(statusText)) {
            throw new TypeError("Invalid Response statusText");
        }
        return {
            headers: createResponseHeaders(init.headers),
            status: status,
            statusText: statusText
        };
    }

    function initializeResponse(response, body, init, metadata) {
        var normalizedInit = validateResponseInit(init);
        var extracted = extractBody(body);
        if (extracted.stream !== null && nullBodyStatuses.indexOf(normalizedInit.status) !== -1) {
            throw new TypeError("Response with this status cannot have a body");
        }
        if (extracted.contentType !== null && !normalizedInit.headers.has("content-type")) {
            normalizedInit.headers.append("content-type", extracted.contentType);
        }

        responseStates.set(response, {
            body: extracted.stream,
            bodyTracker: extracted.stream === null ? null : trackStream(extracted.stream),
            headers: normalizedInit.headers,
            redirected: metadata && metadata.redirected === true,
            status: normalizedInit.status,
            statusText: normalizedInit.statusText,
            type: metadata && metadata.type ? String(metadata.type) : "default",
            url: metadata && metadata.url ? String(metadata.url) : "",
            used: false
        });
    }

    class Response {
        constructor(body, init) {
            initializeResponse(this, body === undefined ? null : body, init, null);
        }

        get body() { return requireResponseState(this).body; }
        get bodyUsed() {
            var state = requireResponseState(this);
            return state.used || (state.body !== null && state.bodyTracker.disturbed);
        }
        get headers() { return requireResponseState(this).headers; }
        get ok() {
            var status = requireResponseState(this).status;
            return status >= 200 && status <= 299;
        }
        get redirected() { return requireResponseState(this).redirected; }
        get status() { return requireResponseState(this).status; }
        get statusText() { return requireResponseState(this).statusText; }
        get type() { return requireResponseState(this).type; }
        get url() { return requireResponseState(this).url; }

        async arrayBuffer() {
            var bytes = joinChunks(await readBodyChunks(requireResponseState(this)));
            if (bytes.byteOffset === 0 && bytes.byteLength === bytes.buffer.byteLength) {
                return bytes.buffer;
            }
            return bytes.buffer.slice(bytes.byteOffset, bytes.byteOffset + bytes.byteLength);
        }

        async blob() {
            var state = requireResponseState(this);
            var bytes = joinChunks(await readBodyChunks(state));
            return new global.Blob([bytes], { type: state.headers.get("content-type") || "" });
        }

        async bytes() {
            return joinChunks(await readBodyChunks(requireResponseState(this)));
        }

        clone() {
            var state = requireResponseState(this);
            if (bodyIsUnusable(state)) {
                throw new TypeError("Cannot clone a used Response");
            }

            var cloneBody = null;
            if (state.body !== null) {
                var branches = state.body.tee();
                state.body = branches[0];
                state.bodyTracker = trackStream(state.body);
                cloneBody = branches[1];
            }
            var clone = new Response(cloneBody, {
                headers: state.headers,
                status: state.status,
                statusText: state.statusText
            });
            var cloneState = requireResponseState(clone);
            cloneState.redirected = state.redirected;
            cloneState.type = state.type;
            cloneState.url = state.url;
            return clone;
        }

        async json() {
            return JSON.parse(await consumeText(requireResponseState(this)));
        }

        async text() {
            return consumeText(requireResponseState(this));
        }

        static error() {
            var response = Object.create(Response.prototype);
            responseStates.set(response, {
                body: null,
                headers: createResponseHeaders(),
                redirected: false,
                status: 0,
                statusText: "",
                type: "error",
                url: "",
                used: false
            });
            return response;
        }

        static json(data, init) {
            var text = JSON.stringify(data);
            if (text === undefined) {
                throw new TypeError("Response.json data is not JSON serializable");
            }
            var hasExplicitContentType = init !== undefined && init !== null &&
                new Headers(init.headers).has("content-type");
            var response = new Response(text, init);
            if (!hasExplicitContentType) {
                response.headers.set("content-type", "application/json");
            }
            return response;
        }

        static redirect(url, status) {
            status = status === undefined ? 302 : Math.trunc(Number(status));
            if ([301, 302, 303, 307, 308].indexOf(status) === -1) {
                throw new RangeError("Invalid redirect status");
            }
            var location = global.URL !== undefined && global.URL !== null
                ? new global.URL(String(url), global.location && global.location.href || undefined).toString()
                : String(url);
            return new Response(null, { status: status, headers: { location: location } });
        }
    }

    Object.defineProperty(Response.prototype, Symbol.toStringTag, {
        configurable: true,
        value: "Response"
    });

    function createFetchResponse(ResponseConstructor, bytes, init, metadata) {
        var status = init && init.status;
        var body = nullBodyStatuses.indexOf(status) === -1 ? streamFromBytes(bytes) : null;
        var response = new ResponseConstructor(body, init);
        var state = responseStates.get(response);
        if (state) {
            state.redirected = metadata.redirected === true;
            state.type = metadata.type || "basic";
            state.url = metadata.url || "";
        }
        return response;
    }

    return {
        Headers: Headers,
        Response: Response,
        createFetchResponse: createFetchResponse
    };
})(globalThis)
