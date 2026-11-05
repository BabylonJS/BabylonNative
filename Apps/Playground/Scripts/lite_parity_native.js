(function () {
    "use strict";

    const nativeFetch = globalThis.fetch;
    const events = [];
    const canvas = document.getElementById("renderCanvas");
    let endpoint;
    let sequence = 0;

    function encode(value) {
        if (value === undefined) {
            return { kind: "undefined" };
        }
        if (typeof value === "number" && (!Number.isFinite(value) || Object.is(value, -0))) {
            return { kind: "number", value: Object.is(value, -0) ? "-0" : String(value) };
        }
        if (Array.isArray(value)) {
            return { kind: "array", value: value.map(encode) };
        }
        if (value !== null && typeof value === "object") {
            return { kind: "object", value: Object.keys(value).map(function (key) {
                return [key, encode(value[key])];
            }) };
        }
        return { kind: "primitive", value: value };
    }

    globalThis.__liteParityDecode = function decode(value) {
        if (value.kind === "undefined") {
            return undefined;
        }
        if (value.kind === "number") {
            return Number(value.value);
        }
        if (value.kind === "array") {
            return value.value.map(decode);
        }
        if (value.kind === "object") {
            const result = {};
            value.value.forEach(function (entry) { result[entry[0]] = decode(entry[1]); });
            return result;
        }
        return value.value;
    };

    function describe(error) {
        return { message: String(error), stack: error && error.stack ? String(error.stack) : "" };
    }

    async function request(path, body) {
        const response = await nativeFetch(endpoint + path, {
            method: "POST",
            headers: { "Content-Type": "application/json" },
            body: JSON.stringify(body),
        });
        if (!response.ok) {
            throw new Error("Parity bridge HTTP " + response.status + " for " + path);
        }
        return response.json();
    }

    async function execute(command) {
        if (command.type === "evaluate") {
            const value = (0, eval)(command.expression);
            return await value;
        }
        if (command.type === "screenshot") {
            return new Promise(function (resolve, reject) {
                TestUtils.getFrameBufferData(function (data) {
                    try {
                        const width = canvas.width;
                        const height = canvas.height;
                        if (data.length !== width * height * 4) {
                            throw new Error("Parity readback dimensions do not match the canvas");
                        }
                        TestUtils.writePNG(data, width, height, command.path);
                        resolve({ width: width, height: height, bytes: data.length });
                    } catch (error) {
                        reject(error);
                    }
                });
            });
        }
        throw new Error("Unsupported parity command: " + command.type);
    }

    async function poll() {
        const command = await request("/poll", { events: events.splice(0) });
        if (command.type !== "idle") {
            let result;
            try {
                const value = await execute(command);
                result = { id: command.id, value: encode(value) };
            } catch (error) {
                result = { id: command.id, error: describe(error) };
            }
            await request("/result", result);
        }
        setTimeout(function () {
            poll().catch(fatal);
        }, 10);
    }

    function fatal(error) {
        console.error("LITE_PARITY_BRIDGE_FAIL: " + (error.stack || error));
        TestUtils.exit(1);
    }

    async function initialize() {
        if (typeof BABYLON !== "undefined" || typeof _playgroundWebGPUEngine !== "undefined") {
            throw new Error("Lite parity must run without Babylon.js bootstrap");
        }
        const response = await nativeFetch("app:///Scripts/lite-parity-config.json");
        if (!response.ok) {
            throw new Error("Cannot load lite-parity-config.json: HTTP " + response.status);
        }
        const config = await response.json();
        endpoint = config.endpoint;
        TestUtils.updateSize(config.width, config.height);
        const url = new URL(config.url);
        globalThis.location = {};
        for (const key of ["href", "origin", "protocol", "host", "hostname", "port", "pathname", "search", "hash"]) {
            globalThis.location[key] = url[key];
        }
        window.location = globalThis.location;
        globalThis.__liteParityModuleUrl = config.moduleUrl;
        if (typeof URLSearchParams === "undefined") {
            globalThis.URLSearchParams = function (search) {
                return new URL("http://localhost/?" + String(search || "").replace(/^\?/, "")).searchParams;
            };
        }
        globalThis.innerWidth = config.width;
        globalThis.innerHeight = config.height;
        if (globalThis.devicePixelRatio !== 1) {
            throw new Error("Parity requires devicePixelRatio=1; native host reports " + globalThis.devicePixelRatio);
        }
        window.innerWidth = config.width;
        window.innerHeight = config.height;
        globalThis.fetch = function (input, options) {
            const target = typeof input === "string" && !/^[a-z][a-z0-9+.-]*:/i.test(input)
                ? new URL(input, config.assetBase).href : input;
            return nativeFetch.call(this, target, options).catch(function (error) {
                throw new Error("LITE_PARITY_ASSET_FAIL: " + String(target) + ": " + String(error));
            });
        };
        for (const level of ["log", "warn", "error"]) {
            const original = console[level];
            console[level] = function () {
                const args = Array.prototype.slice.call(arguments);
                events.push({ type: level, text: args.map(String).join(" "), sequence: sequence++ });
                original.apply(console, args);
            };
        }
        for (const script of config.initScripts || []) {
            (0, eval)(script);
        }
        await request("/ready", { dataset: Object.assign({}, canvas.dataset) });
        poll().catch(fatal);
        const bundle = await nativeFetch(config.bundle);
        if (!bundle.ok) {
            throw new Error("Cannot load parity bundle: HTTP " + bundle.status);
        }
        (0, eval)(await bundle.text());
    }

    initialize().catch(fatal);
})();
