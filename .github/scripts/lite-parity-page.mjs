import { EventEmitter } from "node:events";
import { spawn } from "node:child_process";
import { randomBytes } from "node:crypto";
import { createReadStream, createWriteStream } from "node:fs";
import * as fs from "node:fs/promises";
import http from "node:http";
import { createRequire } from "node:module";
import { dirname, extname, isAbsolute, relative, resolve, sep } from "node:path";
import { performance } from "node:perf_hooks";
import { pipeline } from "node:stream/promises";
import { StringDecoder } from "node:string_decoder";

const BODY_LIMIT = 10 * 1024 * 1024;
const TAIL_LIMIT = 64 * 1024;
const activePages = new Map();
const mimeTypes = {
    ".js": "text/javascript; charset=utf-8",
    ".mjs": "text/javascript; charset=utf-8",
    ".wasm": "application/wasm",
    ".json": "application/json; charset=utf-8",
    ".png": "image/png",
    ".jpg": "image/jpeg",
    ".jpeg": "image/jpeg",
    ".webp": "image/webp",
    ".svg": "image/svg+xml",
    ".bin": "application/octet-stream",
};

function deferred() {
    let resolvePromise;
    let rejectPromise;
    const promise = new Promise((resolveValue, rejectValue) => {
        resolvePromise = resolveValue;
        rejectPromise = rejectValue;
    });
    // A process can fail before a caller starts awaiting its ready/exit signals.
    promise.catch(() => {});
    return { promise, resolve: resolvePromise, reject: rejectPromise };
}

function positiveTimeout(value, fallback, name = "timeout") {
    const result = value === undefined ? fallback : value;
    if (!Number.isFinite(result) || result <= 0 || result > 2_147_483_647) {
        throw new TypeError(`${name} must be a positive, bounded number of milliseconds`);
    }
    return result;
}

function viewport(value = { width: 1280, height: 720 }) {
    if (!value || !Number.isInteger(value.width) || !Number.isInteger(value.height)
        || value.width <= 0 || value.height <= 0) {
        throw new TypeError("viewport must contain positive integer width and height");
    }
    return { width: value.width, height: value.height };
}

function jsonArgument(value, ancestors = new Set()) {
    if (value === null || typeof value === "string" || typeof value === "boolean") {
        return JSON.stringify(value);
    }
    if (typeof value === "number" && Number.isFinite(value)) {
        return JSON.stringify(value);
    }
    if (typeof value !== "object" || ancestors.has(value)) {
        throw new TypeError("NativePage arguments must be JSON-safe (no undefined, cycles, or non-finite values)");
    }
    const prototype = Object.getPrototypeOf(value);
    if (!Array.isArray(value) && prototype !== Object.prototype && prototype !== null) {
        throw new TypeError("NativePage arguments must contain only plain JSON objects and arrays");
    }
    ancestors.add(value);
    try {
        if (Array.isArray(value)) {
            return `[${Array.from(value, item => jsonArgument(item, ancestors)).join(",")}]`;
        }
        return `{${Object.keys(value).map(key =>
            `${JSON.stringify(key)}:${jsonArgument(value[key], ancestors)}`).join(",")}}`;
    } finally {
        ancestors.delete(value);
    }
}

function functionSource(fn) {
    const source = Function.prototype.toString.call(fn);
    if (source.includes("[native code]")) {
        throw new TypeError("NativePage cannot serialize native or bound functions");
    }
    return source;
}

function argumentExpression(arg) {
    if (arg === undefined) {
        return "undefined";
    }
    const json = JSON.stringify(JSON.stringify(encodeNativeValue(arg)))
        .replace(/\u2028/g, "\\u2028").replace(/\u2029/g, "\\u2029");
    return `globalThis.__liteParityDecode(JSON.parse(${json}))`;
}

export function encodeNativeValue(value, ancestors = new Set(), depth = 0) {
    if (depth > 512) {
        throw new TypeError("NativePage argument exceeds the supported nesting depth");
    }
    if (value === undefined) {
        return { kind: "undefined" };
    }
    if (typeof value === "number" && (!Number.isFinite(value) || Object.is(value, -0))) {
        return { kind: "number", value: Object.is(value, -0) ? "-0" : String(value) };
    }
    if (value === null || ["string", "boolean", "number"].includes(typeof value)) {
        return { kind: "primitive", value };
    }
    if (typeof value !== "object" || ancestors.has(value)) {
        throw new TypeError("NativePage arguments cannot contain cycles, functions, symbols, or bigint");
    }
    const prototype = Object.getPrototypeOf(value);
    if (!Array.isArray(value) && prototype !== Object.prototype && prototype !== null) {
        throw new TypeError("NativePage arguments must contain only plain objects and arrays");
    }
    ancestors.add(value);
    try {
        return Array.isArray(value)
            ? { kind: "array", value: Array.from(value, item => encodeNativeValue(item, ancestors, depth + 1)) }
            : { kind: "object", value: Object.keys(value).map(key =>
                [key, encodeNativeValue(value[key], ancestors, depth + 1)]) };
    } finally {
        ancestors.delete(value);
    }
}

export function serializeEvaluation(fnOrString, arg) {
    if (typeof fnOrString === "function") {
        const argument = arg === undefined ? "" : argumentExpression(arg);
        return `(${functionSource(fnOrString)})(${argument})`;
    }
    if (typeof fnOrString === "string" && fnOrString.trim()) {
        if (arg !== undefined) {
            throw new TypeError("A string evaluation cannot receive an argument; use a function");
        }
        return fnOrString;
    }
    throw new TypeError("evaluate expects a function or a non-empty expression string");
}

export function decodeNativeValue(value, depth = 0) {
    if (depth > 512 || !value || typeof value !== "object" || Array.isArray(value)) {
        throw new TypeError("Invalid encoded native value");
    }
    switch (value.kind) {
        case "undefined":
            return undefined;
        case "number":
            if (!["NaN", "Infinity", "-Infinity", "-0"].includes(value.value)) {
                throw new TypeError("Invalid encoded native number");
            }
            return Number(value.value);
        case "primitive":
            if (!Object.hasOwn(value, "value") || (value.value !== null
                && !["string", "boolean", "number"].includes(typeof value.value))
                || (typeof value.value === "number" && !Number.isFinite(value.value))) {
                throw new TypeError("Invalid encoded native primitive");
            }
            return value.value;
        case "array":
            if (!Array.isArray(value.value)) {
                throw new TypeError("Invalid encoded native array");
            }
            return value.value.map(item => decodeNativeValue(item, depth + 1));
        case "object": {
            if (!Array.isArray(value.value)) {
                throw new TypeError("Invalid encoded native object");
            }
            const object = {};
            for (const entry of value.value) {
                if (!Array.isArray(entry) || entry.length !== 2 || typeof entry[0] !== "string"
                    || Object.hasOwn(object, entry[0])) {
                    throw new TypeError("Invalid encoded native object entry");
                }
                Object.defineProperty(object, entry[0], {
                    value: decodeNativeValue(entry[1], depth + 1), enumerable: true, writable: true, configurable: true,
                });
            }
            return object;
        }
        default:
            throw new TypeError("Unknown encoded native value kind");
    }
}

async function initExpression(script, arg) {
    if (script && typeof script === "object") {
        if (arg !== undefined || (typeof script.path === "string") === (typeof script.content === "string")) {
            throw new TypeError("addInitScript expects exactly one of { path } or { content }, without an argument");
        }
        return typeof script.path === "string" ? fs.readFile(resolve(script.path), "utf8") : script.content;
    }
    return serializeEvaluation(script, arg);
}

function unsupported(api, reason = "the native bridge has no real DOM input-event injection") {
    throw new Error(`${api} is unsupported: ${reason}`);
}

function httpError(status, message) {
    const error = new Error(message);
    error.statusCode = status;
    return error;
}

function sendJson(response, status, body) {
    if (response.destroyed || response.writableEnded) {
        return;
    }
    response.writeHead(status, { "Content-Type": "application/json; charset=utf-8", "Cache-Control": "no-store" });
    response.end(JSON.stringify(body));
}

async function readJson(request) {
    if (Number(request.headers["content-length"]) > BODY_LIMIT) {
        request.resume();
        throw httpError(413, "Parity bridge request exceeds 10 MiB");
    }
    return new Promise((resolveBody, rejectBody) => {
        const chunks = [];
        let length = 0;
        let settled = false;
        const fail = error => {
            if (!settled) {
                settled = true;
                rejectBody(error);
            }
        };
        request.on("data", chunk => {
            length += chunk.length;
            if (length > BODY_LIMIT) {
                chunks.length = 0;
                fail(httpError(413, "Parity bridge request exceeds 10 MiB"));
            } else if (!settled) {
                chunks.push(chunk);
            }
        });
        request.on("error", fail);
        request.on("aborted", () => fail(httpError(400, "Parity bridge request was aborted")));
        request.on("end", () => {
            if (settled) {
                return;
            }
            settled = true;
            try {
                const value = JSON.parse(Buffer.concat(chunks).toString("utf8"));
                if (!value || typeof value !== "object" || Array.isArray(value)) {
                    throw new Error("Expected a JSON object");
                }
                resolveBody(value);
            } catch (error) {
                rejectBody(httpError(400, `Invalid bridge JSON: ${error.message}`));
            }
        });
    });
}

function withinDirectory(directory, file) {
    const child = relative(directory, file);
    return child !== "" && child !== ".." && !child.startsWith(`..${sep}`) && !isAbsolute(child);
}

function nativeError(error, prefix) {
    const result = new Error(`${prefix}: ${error.message}`);
    if (error.stack) {
        result.stack += `\nNative stack:\n${error.stack}`;
    }
    return result;
}

class NativeSession {
    constructor(page, url, scene, initScripts) {
        this.page = page;
        this.host = page._host;
        this.url = url;
        this.scene = scene;
        this.initScripts = initScripts;
        this.token = randomBytes(24).toString("hex");
        this.endpoint = `${this.host.origin}/session/${this.token}`;
        this.ready = deferred();
        this.responsive = deferred();
        this.exited = deferred();
        this.queue = [];
        this.current = null;
        this.nextId = 1;
        this.tail = "";
        this.error = null;
        this.stopping = false;
        this.readyReceived = false;
        this.shutdownPromise = null;
        this.commandRecords = new Map();
        this.output = new Map();
    }

    launch(bundlePath) {
        this.launchPromise = this._launch(bundlePath);
        return this.launchPromise;
    }

    _assertLaunching() {
        if (this.stopping || this.error) {
            throw this.error || new Error("Native session was closed during launch");
        }
    }

    async _launch(bundlePath) {
        const host = this.host;
        const nativeBundle = resolve(host._exeDirectory, "Scripts", "lite-parity", `${this.scene}.js`);
        this._assertLaunching();
        await fs.mkdir(dirname(nativeBundle), { recursive: true });
        this._assertLaunching();
        if (resolve(bundlePath) !== nativeBundle) {
            await fs.copyFile(bundlePath, nativeBundle);
            this._assertLaunching();
        }
        await fs.mkdir(dirname(host._configPath), { recursive: true });
        this._assertLaunching();
        const size = this.page.viewportSize();
        await fs.writeFile(host._configPath, JSON.stringify({
            endpoint: this.endpoint,
            width: size.width,
            height: size.height,
            url: this.url,
            moduleUrl: `${host.origin}/bundles/${this.scene}.js`,
            assetBase: host._assetBase,
            bundle: `app:///Scripts/lite-parity/${this.scene}.js`,
            initScripts: this.initScripts,
        }), "utf8");
        this._assertLaunching();
        this.logPath = resolve(host._artifactsDirectory, `${this.scene}-${++host._navigationCount}-${this.token}.log`);
        this.history = { url: this.url, logPath: this.logPath, backend: "native" };
        this.page.history.push(this.history);
        this.log = createWriteStream(this.logPath, { flags: "wx" });
        this.logFailure = null;
        this.log.on("error", error => {
            this.logFailure = error;
            this.fail(new Error(`Cannot preserve native navigation log ${this.logPath}: ${error.message}`));
        });
        await new Promise((resolveLog, rejectLog) => {
            this.log.once("open", resolveLog);
            this.log.once("error", rejectLog);
        });
        this._assertLaunching();
        this.log.write(`URL: ${this.url}\nExecutable: ${host._playground}\n\n`);
        host._sessions.set(this.token, this);
        this.child = spawn(host._playground,
            ["--headless", "--once", "app:///Scripts/lite_parity_native.js"],
            { cwd: host._exeDirectory, windowsHide: true, stdio: ["ignore", "pipe", "pipe"] });
        for (const [name, stream] of [["stdout", this.child.stdout], ["stderr", this.child.stderr]]) {
            const output = { decoder: new StringDecoder("utf8"), pending: "" };
            this.output.set(name, output);
            stream.on("data", chunk => {
                if (!this.log.destroyed) {
                    this.log.write(chunk);
                }
                this.tail = (this.tail + `[${name}] ${chunk.toString("utf8")}`).slice(-TAIL_LIMIT);
                output.pending += output.decoder.write(chunk);
                const lines = output.pending.split(/\r?\n/);
                output.pending = lines.pop();
                for (const line of lines) {
                    this.emitEarlyOutput(name, line);
                }
                if (output.pending.length > TAIL_LIMIT) {
                    this.emitEarlyOutput(name, output.pending);
                    output.pending = "";
                }
            });
            stream.on("end", () => {
                this.emitEarlyOutput(name, output.pending + output.decoder.end());
                output.pending = "";
            });
            stream.on("error", error => this.fail(new Error(`Native ${name} stream failed: ${error.message}`)));
        }
        this.child.on("error", error => {
            this.fail(new Error(`Cannot run Playground: ${error.message}\nLog: ${this.logPath}`), true);
            if (!this.child.pid) {
                this.exited.resolve();
            }
        });
        const processFailure = (code, signal) => new Error(
            `Playground exited before page close (code ${code}, signal ${signal || "none"}).\n`
            + `Log: ${this.logPath}\n${this.tail || "(no native output)"}`);
        this.child.on("exit", (code, signal) => {
            if (!this.stopping) {
                this.fail(processFailure(code, signal), true);
            }
        });
        this.child.on("close", (code, signal) => {
            if (!this.stopping) {
                this.fail(processFailure(code, signal), true);
            }
            this.exited.resolve();
        });
    }

    emitEarlyOutput(name, line) {
        if (this.readyReceived || !line.trim()) {
            return;
        }
        if (/\[(?:error|fatal)\]|JS CONSOLE ERROR|UNCAUGHT JS ERROR|LITE_PARITY_BRIDGE_FAIL|--- (?:CRASH|ASSERT|ABORT) ---/i.test(line)) {
            this.page._emitConsole("error", line);
        } else if (/\[(?:warn|warning)\]/i.test(line)) {
            this.page._emitConsole("warning", line);
        }
    }

    fail(error, crash = false) {
        if (this.error) {
            return;
        }
        this.error = error;
        this.ready.reject(error);
        this.responsive.reject(error);
        for (const command of [...this.queue, ...(this.current ? [this.current] : [])]) {
            clearTimeout(command.timer);
            const record = this.commandRecords.get(command.id);
            if (record) {
                record.status = "failed";
                record.error = error.message;
            }
            command.reject(error);
        }
        this.queue = [];
        this.current = null;
        for (const [timer, rejectDelay] of this.page._timers) {
            clearTimeout(timer);
            rejectDelay(error);
        }
        this.page._timers.clear();
        if (!this.stopping) {
            try {
                this.page.emit("pageerror", error);
                if (crash) {
                    this.page.emit("crash");
                }
            } finally {
                this.kill();
            }
        }
    }

    kill() {
        if (this.child?.pid && this.child.exitCode === null && this.child.signalCode === null) {
            try {
                if (!this.child.kill("SIGKILL")) {
                    this.killFailure = new Error(`Could not terminate Playground PID ${this.child.pid}`);
                }
            } catch (error) {
                this.killFailure = new Error(`Could not terminate Playground PID ${this.child.pid}: ${error.message}`);
            }
        }
    }

    alive() {
        return this.child?.pid && this.child.exitCode === null && this.child.signalCode === null && !this.error;
    }

    command(type, fields, timeout) {
        if (this.error) {
            return Promise.reject(this.error);
        }
        if (this.stopping || !this.launchPromise || (this.child && !this.alive())) {
            return Promise.reject(new Error("The native page has no live Playground process"));
        }
        const result = deferred();
        const command = { id: this.nextId++, type, ...fields, resolve: result.resolve, reject: result.reject };
        const record = {
            id: command.id, type, ...fields, url: this.url, logPath: this.logPath,
            backend: "native", status: "queued",
        };
        this.commandRecords.set(command.id, record);
        this.host.commands.push(record);
        command.timer = setTimeout(() => {
            const error = new Error(`Native ${type} command ${command.id} timed out after ${timeout} ms.\n`
                + `Log: ${this.logPath}\n${this.tail}`);
            this.fail(error);
        }, timeout);
        this.queue.push(command);
        return result.promise;
    }

    receiveReady(body) {
        if (this.error || this.stopping) {
            throw httpError(410, this.error?.message || "Native session is closing");
        }
        if (this.readyReceived) {
            throw httpError(409, "Native session sent /ready more than once");
        }
        if (body.dataset !== undefined) {
            if (!body.dataset || typeof body.dataset !== "object" || Array.isArray(body.dataset)) {
                throw httpError(400, "Native /ready dataset must be an object");
            }
            try {
                jsonArgument(body.dataset);
            } catch (error) {
                throw httpError(400, error.message);
            }
            this.history.dataset = body.dataset;
        }
        for (const [name, output] of this.output) {
            if (output.pending) {
                this.emitEarlyOutput(name, output.pending);
                output.pending = "";
            }
        }
        this.readyReceived = true;
        this.ready.resolve();
        return {};
    }

    receivePoll(body) {
        if (this.error || this.stopping) {
            throw httpError(410, this.error?.message || "Native session is closing");
        }
        if (!this.readyReceived) {
            throw httpError(409, "Native session must send /ready before /poll");
        }
        if (!Array.isArray(body.events) || body.events.length > 10_000) {
            throw httpError(400, "/poll requires a bounded events array");
        }
        for (const event of body.events) {
            if (!event || typeof event.type !== "string" || typeof event.text !== "string") {
                throw httpError(400, "Malformed console event");
            }
        }
        for (const event of body.events) {
            this.page._emitConsole(event.type, event.text);
        }
        this.responsive.resolve();
        if (this.current || !this.queue.length) {
            return { type: "idle" };
        }
        this.current = this.queue.shift();
        this.commandRecords.get(this.current.id).status = "running";
        const { id, type, expression, path } = this.current;
        return type === "evaluate" ? { id, type, expression } : { id, type, path };
    }

    receiveResult(body) {
        if (this.error || this.stopping) {
            throw httpError(410, this.error?.message || "Native session is closing");
        }
        if (!Number.isSafeInteger(body.id) || !this.current || body.id !== this.current.id) {
            throw httpError(409, "Result ID does not match the active native command");
        }
        let decoded;
        if (body.error !== undefined) {
            if (!body.error || typeof body.error.message !== "string"
                || (body.error.stack !== undefined && typeof body.error.stack !== "string")) {
                throw httpError(400, "Malformed native command error");
            }
        } else if (!Object.hasOwn(body, "value")) {
            throw httpError(400, "Native result must contain value or error");
        } else {
            try {
                jsonArgument(body.value);
                decoded = body.value && typeof body.value === "object"
                    && Object.hasOwn(body.value, "kind") ? decodeNativeValue(body.value) : body.value;
            } catch (error) {
                throw httpError(400, error.message);
            }
        }
        const command = this.current;
        this.current = null;
        clearTimeout(command.timer);
        const record = this.commandRecords.get(command.id);
        record.status = body.error ? "failed" : "completed";
        if (body.error) {
            record.error = body.error.message;
            command.reject(nativeError(body.error, `Native ${command.type} failed`));
        } else {
            command.resolve(decoded);
        }
        return {};
    }

    async shutdown(reason = new Error("Native page was closed")) {
        if (this.shutdownPromise) {
            return this.shutdownPromise;
        }
        this.shutdownPromise = this._shutdown(reason);
        return this.shutdownPromise;
    }

    async _shutdown(reason) {
        this.stopping = true;
        this.fail(reason);
        const errors = [];
        if (this.launchPromise) {
            try {
                await this.launchPromise;
            } catch (error) {
                if (error !== this.error) {
                    errors.push(error);
                }
            }
        }
        this.kill();
        this.host._sessions.delete(this.token);
        if (this.child) {
            let timer;
            try {
                await Promise.race([
                    this.exited.promise,
                    new Promise((_, rejectExit) => {
                        timer = setTimeout(() => rejectExit(this.killFailure
                            || new Error(`Playground PID ${this.child.pid} did not terminate within 10 seconds`)), 10_000);
                    }),
                ]);
            } catch (error) {
                errors.push(error);
            } finally {
                clearTimeout(timer);
            }
        }
        if (this.log && !this.log.destroyed) {
            try {
                await new Promise((resolveLog, rejectLog) => {
                    this.log.once("error", rejectLog);
                    this.log.end(resolveLog);
                });
            } catch (error) {
                errors.push(error);
            }
        }
        if (this.logFailure) {
            errors.push(this.logFailure);
        }
        try {
            const config = JSON.parse(await fs.readFile(this.host._configPath, "utf8"));
            if (config.endpoint === this.endpoint) {
                await fs.unlink(this.host._configPath);
            }
        } catch (error) {
            if (error.code !== "ENOENT") {
                errors.push(new Error(`Cannot clean native session config: ${error.message}`));
            }
        }
        if (errors.length) {
            throw new AggregateError(errors, "Native session cleanup failed");
        }
    }
}

function pngDimensions(buffer) {
    if (buffer.length < 45 || !buffer.subarray(0, 8).equals(Buffer.from([137, 80, 78, 71, 13, 10, 26, 10]))) {
        throw new Error("Native screenshot is not a PNG");
    }
    let offset = 8;
    let dimensions;
    let imageData = false;
    while (offset + 12 <= buffer.length) {
        const length = buffer.readUInt32BE(offset);
        const end = offset + 12 + length;
        if (end > buffer.length) {
            throw new Error("Native screenshot contains a truncated PNG chunk");
        }
        const type = buffer.toString("ascii", offset + 4, offset + 8);
        let crc = 0xffffffff;
        for (let index = offset + 4; index < end - 4; index++) {
            crc ^= buffer[index];
            for (let bit = 0; bit < 8; bit++) {
                crc = (crc >>> 1) ^ ((crc & 1) ? 0xedb88320 : 0);
            }
        }
        if (((crc ^ 0xffffffff) >>> 0) !== buffer.readUInt32BE(end - 4)) {
            throw new Error(`Native screenshot has an invalid ${type} PNG checksum`);
        }
        if (offset === 8) {
            if (type !== "IHDR" || length !== 13) {
                throw new Error("Native screenshot has no valid PNG IHDR");
            }
            dimensions = { width: buffer.readUInt32BE(offset + 8), height: buffer.readUInt32BE(offset + 12) };
            if (!dimensions.width || !dimensions.height) {
                throw new Error("Native screenshot has zero dimensions");
            }
        } else if (type === "IHDR") {
            throw new Error("Native screenshot contains multiple PNG headers");
        }
        if (type === "IDAT") {
            imageData = true;
        }
        if (type === "IEND") {
            if (length !== 0 || end !== buffer.length || !imageData) {
                throw new Error("Native screenshot has an invalid PNG end");
            }
            return dimensions;
        }
        offset = end;
    }
    throw new Error("Native screenshot is missing the PNG end");
}

export class NativePage extends EventEmitter {
    constructor(host, options) {
        super();
        this._host = host;
        this._browser = options.browser ?? null;
        this._testInfo = options.testInfo;
        this._context = {
            ...host._context,
            browser: () => this._browser,
            newPage: options => host.createPage({ browser: this._browser, testInfo: this._testInfo, ...options }),
        };
        this.history = [];
        this._viewport = viewport(options.viewport);
        this._timeout = host._timeout;
        this._navigationTimeout = host._timeout;
        this._initScripts = [];
        this._closed = false;
        this._url = "about:blank";
        this._timers = new Map();
        this.keyboard = Object.fromEntries(["press", "type", "insertText", "down", "up"]
            .map(method => [method, async () => unsupported(`NativePage.keyboard.${method}`)]));
        this.mouse = Object.fromEntries(["move", "click", "dblclick", "down", "up", "wheel"]
            .map(method => [method, async () => unsupported(`NativePage.mouse.${method}`)]));
    }

    _assertOpen() {
        if (this._closed || this._host._closed) {
            throw new Error("NativePage is closed");
        }
    }

    _emitConsole(type, text) {
        const level = type === "warn" ? "warning" : type;
        return this.emit("console", { type: () => level, text: () => text, location: () => ({}) });
    }

    _command(type, fields, timeout = this._timeout) {
        this._assertOpen();
        if (!this._session) {
            throw new Error("NativePage must navigate to a scene before issuing native commands");
        }
        return this._session.command(type, fields, timeout);
    }

    async goto(target, options = {}) {
        this._assertOpen();
        if (this._navigating) {
            throw new Error("Concurrent NativePage navigations are unsupported");
        }
        if (typeof target !== "string") {
            throw new TypeError("goto expects a URL string");
        }
        const url = new URL(target, this._host._assetBase);
        if (!["http:", "https:"].includes(url.protocol)) {
            throw new Error("NativePage scene URLs must use HTTP or HTTPS");
        }
        const match = /\/(scene\d+)\.html$/i.exec(url.pathname);
        if (!match) {
            throw new Error(`NativePage URL does not identify /sceneN.html: ${url.href}`);
        }
        const scene = match[1];
        const bundle = resolve(this._host._bundleDirectory, `${scene}.js`);
        const stat = await fs.stat(bundle).catch(error => {
            throw new Error(`Missing native scene bundle ${bundle}: ${error.message}`);
        });
        if (!stat.isFile()) {
            throw new Error(`Native scene bundle is not a file: ${bundle}`);
        }
        const timeout = positiveTimeout(options.timeout, this._navigationTimeout, "navigation timeout");
        if (this._navigating) {
            throw new Error("Concurrent NativePage navigations are unsupported");
        }
        this._assertOpen();
        this._navigating = true;
        let timer;
        try {
            if (this._session) {
                await this._session.shutdown(new Error("Native navigation replaced the previous Playground process"));
            }
            this._assertOpen();
            const session = new NativeSession(this, url.href, scene,
                [...this._host._contextInitScripts, ...this._initScripts]);
            this._session = session;
            const navigation = async () => {
                await session.launch(bundle);
                if (this._closed || session.stopping) {
                    throw new Error("NativePage was closed during navigation");
                }
                await session.ready.promise;
                await session.responsive.promise;
                if (!session.alive()) {
                    throw session.error || new Error("Playground exited during navigation");
                }
            };
            await Promise.race([
                navigation(),
                new Promise((_, rejectNavigation) => {
                    timer = setTimeout(() => {
                        const error = new Error(`Native navigation timed out after ${timeout} ms waiting for /ready and /poll.\n`
                            + `Log: ${session.logPath || "(not opened)"}\n${session.tail}`);
                        session.fail(error);
                        rejectNavigation(error);
                    }, timeout);
                }),
            ]);
            this._url = url.href;
            return { ok: () => true, status: () => 200, url: () => url.href };
        } catch (error) {
            if (this._session) {
                try {
                    await this._session.shutdown(error);
                } catch (cleanupError) {
                    throw new AggregateError([error, cleanupError], "Native navigation and cleanup failed");
                }
            }
            throw error;
        } finally {
            clearTimeout(timer);
            this._navigating = false;
        }
    }

    url() { return this._url; }
    isClosed() { return this._closed; }
    context() { return this._context; }
    viewportSize() { return { ...this._viewport }; }

    setDefaultTimeout(timeout) {
        this._timeout = positiveTimeout(timeout, this._timeout);
    }

    setDefaultNavigationTimeout(timeout) {
        this._navigationTimeout = positiveTimeout(timeout, this._navigationTimeout, "navigation timeout");
    }

    async addInitScript(script, arg) {
        this._assertOpen();
        this._initScripts.push(await initExpression(script, arg));
        this._assertOpen();
    }

    async evaluate(fnOrString, arg) {
        return this._command("evaluate", { expression: serializeEvaluation(fnOrString, arg) });
    }

    async waitForFunction(fnOrString, arg, options) {
        this._assertOpen();
        if (options === undefined && typeof fnOrString === "function" && fnOrString.length === 0
            && arg && typeof arg === "object" && !Array.isArray(arg)
            && Object.keys(arg).length && Object.keys(arg).every(key => ["timeout", "polling"].includes(key))) {
            options = arg;
            arg = undefined;
        }
        options ??= {};
        const timeout = positiveTimeout(options.timeout, this._timeout);
        const polling = options.polling === undefined || options.polling === "raf"
            ? 50 : positiveTimeout(options.polling, 50, "polling interval");
        const expression = serializeEvaluation(fnOrString, arg);
        const deadline = performance.now() + timeout;
        while (true) {
            const remaining = deadline - performance.now();
            if (remaining <= 0) {
                throw new Error(`NativePage.waitForFunction timed out after ${timeout} ms`);
            }
            const value = await this._command("evaluate", { expression }, Math.min(remaining, this._timeout));
            if (value) {
                let disposed = false;
                return {
                    jsonValue: async () => {
                        if (disposed) {
                            throw new Error("Native JSON handle has been disposed");
                        }
                        return value;
                    },
                    dispose: async () => { disposed = true; },
                };
            }
            const delay = Math.min(polling, deadline - performance.now());
            if (delay > 0) {
                await this.waitForTimeout(delay);
            }
        }
    }

    async waitForTimeout(milliseconds) {
        this._assertOpen();
        if (!Number.isFinite(milliseconds) || milliseconds < 0 || milliseconds > 2_147_483_647) {
            throw new TypeError("waitForTimeout requires a bounded, non-negative duration");
        }
        if (this._session?.error) {
            throw this._session.error;
        }
        await new Promise((resolveDelay, rejectDelay) => {
            const timer = setTimeout(() => {
                this._timers.delete(timer);
                resolveDelay();
            }, milliseconds);
            this._timers.set(timer, rejectDelay);
        });
        this._assertOpen();
        if (this._session?.error) {
            throw this._session.error;
        }
    }

    async setViewportSize(size) {
        this._assertOpen();
        const next = viewport(size);
        if (this._session) {
            const actual = await this.evaluate(({ width, height }) => {
                TestUtils.updateSize(width, height);
                globalThis.innerWidth = width;
                globalThis.innerHeight = height;
                window.innerWidth = width;
                window.innerHeight = height;
                const canvas = document.getElementById("renderCanvas");
                return { width: canvas.width, height: canvas.height };
            }, next);
            if (!actual || actual.width !== next.width || actual.height !== next.height) {
                throw new Error("Native canvas did not adopt the requested viewport size");
            }
        }
        this._viewport = next;
    }

    async screenshot(options = {}) {
        this._assertOpen();
        if (options.type !== undefined && options.type !== "png") {
            unsupported("NativePage.screenshot", "only PNG capture is implemented");
        }
        if (options.fullPage) {
            unsupported("NativePage.screenshot", "fullPage capture requires a browser layout engine");
        }
        if (options.path !== undefined && typeof options.path !== "string") {
            throw new TypeError("screenshot path must be a string");
        }
        if (options.path && [".jpg", ".jpeg", ".webp"].includes(extname(options.path).toLowerCase())) {
            unsupported("NativePage.screenshot", "the requested file extension is not PNG");
        }
        const clip = options.clip;
        if (clip !== undefined && (!clip || !["x", "y", "width", "height"].every(key =>
            Number.isInteger(clip[key])) || clip.x < 0 || clip.y < 0 || clip.width <= 0 || clip.height <= 0)) {
            throw new TypeError("Native PNG clipping requires non-negative integer coordinates and positive integer dimensions");
        }
        const file = options.path ? resolve(options.path)
            : resolve(this._host._artifactsDirectory, `screenshot-${randomBytes(12).toString("hex")}.png`);
        await fs.mkdir(dirname(file), { recursive: true });
        // Remove stale output so an unsuccessful native write cannot reuse an earlier screenshot.
        await fs.rm(file, { force: true });
        const metadata = await this._command("screenshot", { path: file },
            positiveTimeout(options.timeout, this._timeout));
        let buffer = await fs.readFile(file);
        const dimensions = pngDimensions(buffer);
        if (!metadata || metadata.width !== dimensions.width || metadata.height !== dimensions.height
            || metadata.bytes !== dimensions.width * dimensions.height * 4
            || dimensions.width !== this._viewport.width || dimensions.height !== this._viewport.height) {
            throw new Error(`Native PNG dimensions disagree with capture metadata or viewport: ${JSON.stringify({
                dimensions, metadata, viewport: this._viewport,
            })}`);
        }
        if (clip !== undefined) {
            if (clip.x + clip.width > dimensions.width || clip.y + clip.height > dimensions.height) {
                throw new Error("Native PNG clip lies outside the captured framebuffer");
            }
            const { PNG } = this._host._pngjs();
            const source = PNG.sync.read(buffer);
            const cropped = new PNG({ width: clip.width, height: clip.height });
            PNG.bitblt(source, cropped, clip.x, clip.y, clip.width, clip.height, 0, 0);
            buffer = PNG.sync.write(cropped);
            pngDimensions(buffer);
            await fs.writeFile(file, buffer);
        }
        const capture = {
            path: file, url: this._session.url, logPath: this._session.logPath,
            width: clip?.width ?? dimensions.width, height: clip?.height ?? dimensions.height,
            backend: "native",
        };
        const mark = `LITE_PARITY_CAPTURE ${JSON.stringify(capture)}`;
        this._session.history.captures ??= [];
        this._session.history.captures.push(capture);
        this._session.log.write(`${mark}\n`);
        this._emitConsole("log", mark);
        return buffer;
    }

    async addStyleTag() {
        unsupported("NativePage.addStyleTag", "the native canvas host has no CSS stylesheet engine");
    }

    async setContent() {
        unsupported("NativePage.setContent", "the native canvas host cannot parse and render an HTML document");
    }

    locator(selector) {
        this._assertOpen();
        return new NativeLocator(this, selector);
    }

    async $eval(selector, fn, arg) {
        return this.locator(selector).first().evaluate(fn, arg);
    }

    async $$eval(selector, fn, arg) {
        if (typeof selector !== "string" || typeof fn !== "function") {
            throw new TypeError("$$eval requires a CSS selector and a function");
        }
        const argument = argumentExpression(arg);
        return this.evaluate(`(${functionSource(fn)})(Array.from(document.querySelectorAll(${JSON.stringify(selector)})), ${argument})`);
    }

    async close() {
        if (this._closePromise) {
            return this._closePromise;
        }
        this._closed = true;
        this._closePromise = (async () => {
            for (const [timer, rejectDelay] of this._timers) {
                clearTimeout(timer);
                rejectDelay(new Error("NativePage was closed during waitForTimeout"));
            }
            this._timers.clear();
            try {
                if (this._session) {
                    await this._session.shutdown();
                }
            } finally {
                const child = this._session?.child;
                if ((!child?.pid || child.exitCode !== null || child.signalCode !== null)
                    && activePages.get(this._host._pageKey) === this) {
                    activePages.delete(this._host._pageKey);
                }
                this.emit("close");
            }
        })();
        return this._closePromise;
    }
}

export class NativeLocator {
    constructor(page, selector, index = null) {
        if (typeof selector !== "string" || !selector.trim()) {
            throw new TypeError("NativeLocator requires a non-empty CSS selector");
        }
        this._page = page;
        this._selector = selector;
        this._index = index;
    }

    first() { return this.nth(0); }

    nth(index) {
        if (!Number.isInteger(index) || index < 0) {
            throw new TypeError("NativeLocator.nth requires a non-negative integer");
        }
        return new NativeLocator(this._page, this._selector, index);
    }

    _selection(required) {
        const selector = JSON.stringify(this._selector);
        const strict = this._index === null && required
            ? `if (matches.length > 1) throw new Error("NativeLocator strict-mode violation: selector matched multiple elements");` : "";
        return `const matches = document.querySelectorAll(${selector});
            ${strict}
            const element = ${this._index === null || this._index === 0
                ? `document.querySelector(${selector})` : `matches[${this._index}]`};
            ${required ? `if (!element) throw new Error("NativeLocator selector matched no element: " + ${selector});` : ""}`;
    }

    async evaluate(fn, arg) {
        if (typeof fn !== "function") {
            throw new TypeError("NativeLocator.evaluate requires a function");
        }
        return this._page.evaluate(`(function () {
            ${this._selection(true)}
            return (${functionSource(fn)})(element, ${argumentExpression(arg)});
        })()`);
    }

    async getAttribute(name) {
        if (typeof name !== "string") {
            throw new TypeError("getAttribute requires an attribute name");
        }
        return this.evaluate((element, attribute) => {
            if (typeof element.getAttribute === "function") {
                return element.getAttribute(attribute);
            }
            if (attribute.startsWith("data-") && element.dataset) {
                const key = attribute.slice(5).replace(/-([a-z])/g, (_, letter) => letter.toUpperCase());
                return Object.prototype.hasOwnProperty.call(element.dataset, key) ? String(element.dataset[key]) : null;
            }
            throw new Error("NativeLocator.getAttribute is unsupported by the native DOM for " + attribute);
        }, name);
    }

    async boundingBox() {
        return this._page.evaluate(`(function () {
            ${this._selection(false)}
            if (!element) return null;
            if (typeof element.getBoundingClientRect !== "function")
                throw new Error("NativeLocator.boundingBox is unsupported: native DOM has no getBoundingClientRect");
            const rect = element.getBoundingClientRect();
            if (!rect.width || !rect.height) return null;
            return { x: rect.x === undefined ? rect.left : rect.x,
                y: rect.y === undefined ? rect.top : rect.y, width: rect.width, height: rect.height };
        })()`);
    }

    async count() {
        return this._page.evaluate(`(function () {
            const count = document.querySelectorAll(${JSON.stringify(this._selector)}).length;
            return ${this._index === null ? "count" : `count > ${this._index} ? 1 : 0`};
        })()`);
    }

    async textContent() {
        return this.evaluate(element => {
            if (!("textContent" in element)) {
                throw new Error("NativeLocator.textContent is unsupported by the native DOM");
            }
            return element.textContent;
        });
    }

    async inputValue() {
        return this.evaluate(element => {
            if (!("value" in element)) {
                throw new Error("NativeLocator.inputValue is unsupported: native element has no value property");
            }
            return element.value;
        });
    }

    async waitFor(options = {}) {
        const state = options.state ?? "visible";
        if (!["attached", "detached", "visible", "hidden"].includes(state)) {
            throw new TypeError("NativeLocator.waitFor state must be attached, detached, visible, or hidden");
        }
        const expression = `(function () {
            ${this._selection(false)}
            if (${JSON.stringify(state)} === "attached") return !!element;
            if (${JSON.stringify(state)} === "detached") return !element;
            if (!element) return ${state === "hidden"};
            if (typeof element.getBoundingClientRect !== "function")
                throw new Error("NativeLocator visibility is unsupported: native DOM has no getBoundingClientRect");
            const rect = element.getBoundingClientRect();
            let visible = rect.width > 0 && rect.height > 0;
            if (typeof getComputedStyle === "function") {
                const style = getComputedStyle(element);
                visible = visible && style.display !== "none" && style.visibility !== "hidden" && style.visibility !== "collapse";
            }
            return ${state === "hidden" ? "!visible" : "visible"};
        })()`;
        const handle = await this._page.waitForFunction(expression, undefined, { timeout: options.timeout });
        await handle.dispose();
    }

    async screenshot(options = {}) {
        const isFramebuffer = await this.evaluate(element =>
            element === document.getElementById("renderCanvas"));
        if (!isFramebuffer) {
            unsupported("NativeLocator.screenshot", "only the real renderCanvas framebuffer can be captured");
        }
        return this._page.screenshot(options);
    }

    async click() { unsupported("NativeLocator.click"); }
    async fill() { unsupported("NativeLocator.fill"); }
    async check() { unsupported("NativeLocator.check", "native controls do not have functional DOM event dispatch"); }
    async uncheck() { unsupported("NativeLocator.uncheck", "native controls do not have functional DOM event dispatch"); }
}

export async function createNativeHost({
    playground, assetBase, bundleDirectory, artifactsDirectory, timeoutMs = 180_000, liteDirectory,
}) {
    for (const [name, value] of Object.entries({ playground, assetBase, bundleDirectory, artifactsDirectory })) {
        if (typeof value !== "string" || !value) {
            throw new TypeError(`createNativeHost requires ${name}`);
        }
    }
    const executable = await fs.realpath(resolve(playground));
    if (!(await fs.stat(executable)).isFile()) {
        throw new Error(`Playground executable is not a file: ${executable}`);
    }
    const bundles = await fs.realpath(resolve(bundleDirectory));
    if (!(await fs.stat(bundles)).isDirectory()) {
        throw new Error(`Native bundle directory is not a directory: ${bundles}`);
    }
    const base = new URL(assetBase);
    if (!["http:", "https:"].includes(base.protocol)) {
        throw new Error("assetBase must be an absolute HTTP or HTTPS URL");
    }
    const artifacts = resolve(artifactsDirectory);
    await fs.mkdir(artifacts, { recursive: true });
    const host = {
        origin: null,
        commands: [],
        _playground: executable,
        _exeDirectory: dirname(executable),
        _configPath: resolve(dirname(executable), "Scripts", "lite-parity-config.json"),
        _assetBase: base.href,
        _bundleDirectory: bundles,
        _artifactsDirectory: artifacts,
        _timeout: positiveTimeout(timeoutMs, 180_000, "timeoutMs"),
        _sessions: new Map(),
        _contextInitScripts: [],
        _navigationCount: 0,
        _closed: false,
        _pages: new Set(),
        _pngjs() {
            try {
                const require = liteDirectory ? createRequire(resolve(liteDirectory, "package.json")) : createRequire(import.meta.url);
                return require("pngjs");
            } catch (error) {
                throw new Error(`PNG clipping requires pngjs${liteDirectory ? ` in ${liteDirectory}` : ""}: ${error.message}`);
            }
        },
        async createPage(options = {}) {
            if (host._closed) {
                throw new Error("Native host is closed");
            }
            if (activePages.has(host._pageKey)) {
                throw new Error("Only one NativePage may be open for this Playground executable; close it before creating another");
            }
            if (options.deviceScaleFactor !== undefined && options.deviceScaleFactor !== 1) {
                throw new Error("NativePage supports deviceScaleFactor 1 only");
            }
            const page = new NativePage(host, options);
            activePages.set(host._pageKey, page);
            host._pages.add(page);
            page.once("close", () => host._pages.delete(page));
            return page;
        },
    };
    host._pageKey = process.platform === "win32" ? host._configPath.toLowerCase() : host._configPath;
    host._context = {
        async addInitScript(script, arg) {
            if (host._closed) {
                throw new Error("Native context is closed");
            }
            const expression = await initExpression(script, arg);
            if (host._closed) {
                throw new Error("Native context was closed while loading its init script");
            }
            host._contextInitScripts.push(expression);
        },
        pages: () => [...host._pages].filter(page => !page.isClosed()),
        browser: () => [...host._pages].find(page => !page.isClosed())?._browser ?? null,
        newPage: options => host.createPage(options),
        close: () => host.close(),
    };
    const sockets = new Set();
    const server = http.createServer(async (request, response) => {
        let session;
        try {
            if (!["127.0.0.1", "::1", "::ffff:127.0.0.1"].includes(request.socket.remoteAddress)) {
                throw httpError(403, "Parity bridge accepts loopback requests only");
            }
            const url = new URL(request.url, host.origin);
            if (url.pathname.startsWith("/bundles/")) {
                if (!["GET", "HEAD"].includes(request.method)) {
                    throw httpError(405, "Bundle assets require GET or HEAD");
                }
                let asset;
                try {
                    asset = decodeURIComponent(url.pathname.slice("/bundles/".length));
                } catch {
                    throw httpError(400, "Invalid bundle asset encoding");
                }
                if (!asset || asset.includes("\\") || asset.includes("\0")
                    || asset.split("/").some(part => !part || part === "." || part === "..")) {
                    throw httpError(400, "Invalid bundle asset path");
                }
                const file = await fs.realpath(resolve(bundles, ...asset.split("/"))).catch(error => {
                    if (error.code === "ENOENT" || error.code === "ENOTDIR") {
                        throw httpError(404, "Bundle asset not found");
                    }
                    throw error;
                });
                if (!withinDirectory(bundles, file)) {
                    throw httpError(403, "Bundle asset lies outside bundleDirectory");
                }
                const stat = await fs.stat(file);
                if (!stat.isFile()) {
                    throw httpError(404, "Bundle asset is not a file");
                }
                response.writeHead(200, {
                    "Content-Type": mimeTypes[extname(file).toLowerCase()] || "application/octet-stream",
                    "Content-Length": stat.size,
                    "Cache-Control": "no-store",
                    "Access-Control-Allow-Origin": "*",
                });
                if (request.method === "HEAD") {
                    response.end();
                } else {
                    await pipeline(createReadStream(file), response);
                }
                return;
            }
            const match = /^\/session\/([a-f0-9]{48})\/(ready|poll|result)$/.exec(url.pathname);
            if (!match || url.search) {
                throw httpError(404, "Unknown parity bridge route");
            }
            if (request.method !== "POST") {
                throw httpError(405, "Native session endpoints require POST");
            }
            session = host._sessions.get(match[1]);
            if (!session) {
                throw httpError(410, "Unknown or expired native session");
            }
            const body = await readJson(request);
            const result = match[2] === "ready" ? session.receiveReady(body)
                : match[2] === "poll" ? session.receivePoll(body) : session.receiveResult(body);
            sendJson(response, 200, result);
        } catch (error) {
            if (!error.statusCode) {
                const failure = new Error(`Parity HTTP server failed: ${error.stack || error}`);
                if (session) {
                    session.fail(failure);
                } else {
                    host._serverError = failure;
                    for (const page of host._pages) {
                        page.emit("pageerror", failure);
                    }
                }
            }
            if (response.headersSent) {
                response.destroy(error);
            } else {
                sendJson(response, error.statusCode || 500, { error: error.message });
            }
        }
    });
    server.requestTimeout = Math.min(host._timeout, 30_000);
    server.headersTimeout = Math.min(host._timeout, 30_000);
    server.on("connection", socket => {
        sockets.add(socket);
        socket.once("close", () => sockets.delete(socket));
    });
    await new Promise((resolveListen, rejectListen) => {
        server.once("error", rejectListen);
        server.listen(0, "127.0.0.1", () => {
            server.removeListener("error", rejectListen);
            resolveListen();
        });
    });
    host.origin = `http://127.0.0.1:${server.address().port}`;
    server.on("error", error => {
        host._serverError = error;
        for (const session of host._sessions.values()) {
            session.fail(new Error(`Parity HTTP server failed: ${error.message}`));
        }
    });
    host.close = async () => {
        if (host._closePromise) {
            return host._closePromise;
        }
        host._closed = true;
        host._closePromise = (async () => {
            const results = await Promise.allSettled([...host._pages].map(page => page.close()));
            await new Promise((resolveClose, rejectClose) => {
                server.close(error => error ? rejectClose(error) : resolveClose());
                server.closeAllConnections();
                for (const socket of sockets) {
                    socket.destroy();
                }
            });
            const errors = results.filter(result => result.status === "rejected").map(result => result.reason);
            if (host._serverError) {
                errors.push(host._serverError);
            }
            if (errors.length) {
                throw new AggregateError(errors, "Native host cleanup failed");
            }
        })();
        return host._closePromise;
    };
    return host;
}
