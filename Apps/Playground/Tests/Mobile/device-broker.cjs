const http = require("node:http");
const { randomUUID } = require("node:crypto");

const resetSource = Buffer.from(`(async () => {
    if (globalThis.scene) { scene.dispose(); }
    globalThis.scene = null;
    if (globalThis.engine) {
        engine.setHardwareScalingLevel(1);
        engine.dispose();
    }
    globalThis.engine = null;
    delete globalThis.__babylonNativeScreenshot;
    return JSON.stringify({});
})()`).toString("base64");

async function startBroker(transport, consoleMessages = []) {
    let lease;
    let pending = false;
    let failure;
    const history = [];
    const token = randomUUID();
    const server = http.createServer(async (request, response) => {
        response.setHeader("Content-Type", "application/json");
        response.setHeader("Connection", "close");
        if (request.method !== "POST" || request.url !== `/${token}`) {
            response.writeHead(404);
            response.end(JSON.stringify({ error: "Unknown device broker endpoint" }));
            return;
        }
        try {
            const chunks = [];
            let length = 0;
            for await (const chunk of request) {
                length += chunk.length;
                if (length > 16 * 1024 * 1024) {
                    throw new Error("Device broker payload exceeds 16 MiB");
                }
                chunks.push(chunk);
            }
            const input = JSON.parse(Buffer.concat(chunks).toString("utf8"));
            let value;
            if (input.operation === "acquire") {
                if (failure) { throw failure; }
                if (lease || pending) { throw new Error("Only one worker may own this device"); }
                lease = randomUUID();
                value = lease;
                history.push({ event: "acquire", worker: input.worker, atMs: Date.now() });
            } else {
                if (!lease || input.lease !== lease) { throw new Error("Invalid device lease"); }
                if (input.operation === "abort") {
                    history.push({ event: "abort", message: input.message, atMs: Date.now() });
                    if (pending) {
                        failure = new Error("Worker aborted with a native command still in flight");
                        transport.abort(failure);
                    }
                } else if (input.operation === "close") {
                    if (pending) { throw new Error("Cannot release an in-flight native command"); }
                    if (failure) { throw failure; }
                    pending = true;
                    try {
                        await transport.request("EVALUATE", [resetSource], 100_000);
                        history.push({ event: "release", atMs: Date.now() });
                        lease = undefined;
                    } catch (error) {
                        failure = error;
                        throw error;
                    } finally {
                        pending = false;
                    }
                } else if (input.operation === "request") {
                    if (failure) { throw failure; }
                    if (pending) { throw new Error("Concurrent native commands are not allowed"); }
                    if (!["EVALUATE", "RESIZE", "SCREENSHOT"].includes(input.method) ||
                        !Array.isArray(input.fields) || input.fields.some(field => typeof field !== "string") ||
                        !Number.isSafeInteger(input.timeoutMs) || input.timeoutMs <= 0) {
                        throw new Error("Invalid native command");
                    }
                    pending = true;
                    try {
                        value = (await transport.request(input.method, input.fields, input.timeoutMs)).toString("base64");
                    } catch (error) {
                        history.push({ event: "command-error", method: input.method, message: error.message, atMs: Date.now() });
                        try {
                            await transport.request("EVALUATE", [resetSource], 100_000);
                            history.push({ event: "recovered", atMs: Date.now() });
                        } catch (cleanupError) {
                            failure = new AggregateError([error, cleanupError], "Native command and recovery failed");
                        }
                        throw error;
                    } finally {
                        pending = false;
                    }
                } else {
                    throw new Error("Unknown device broker operation");
                }
            }
            response.end(JSON.stringify({ value, console: consoleMessages.splice(0) }));
        } catch (error) {
            response.writeHead(409);
            response.end(JSON.stringify({ error: error.message, console: consoleMessages.splice(0) }));
        }
    });
    await new Promise((resolve, reject) => {
        server.once("error", reject);
        server.listen(0, "127.0.0.1", resolve);
    });
    return {
        url: `http://127.0.0.1:${server.address().port}/${token}`,
        history,
        get failure() { return failure?.message; },
        async close() {
            server.closeAllConnections();
            await new Promise((resolve, reject) => server.close(error => error ? reject(error) : resolve()));
        }
    };
}

async function createTransport(onConsole, signal) {
    const urls = JSON.parse(process.env.MOBILE_DEVICE_BROKERS || "[]");
    const slot = Number(process.env.TEST_PARALLEL_INDEX);
    const url = urls[slot];
    if (!Number.isSafeInteger(slot) || slot < 0 || !/^http:\/\/127\.0\.0\.1:[0-9]+\/[a-f0-9-]{36}$/.test(url || "")) {
        throw new Error("Missing run-owned device broker for this worker slot; launch with npm run visual");
    }
    let lease;
    let abortPromise;
    let closePromise;
    async function rpc(operation, args = {}, startup = false) {
        const response = await fetch(url, {
            method: "POST", headers: { "Content-Type": "application/json" },
            body: JSON.stringify({ operation, lease, ...args }),
            signal: startup ? signal : AbortSignal.timeout((args.timeoutMs || 100_000) + 10_000)
        });
        const body = await response.json();
        for (const item of body.console || []) { onConsole(item.level, item.text); }
        if (!response.ok || body.error) { throw new Error(body.error || `Broker HTTP ${response.status}`); }
        return body.value;
    }
    lease = await rpc("acquire", { worker: process.env.TEST_WORKER_INDEX }, true);
    return {
        async request(method, fields, timeoutMs) {
            return Buffer.from(await rpc("request", { method, fields, timeoutMs }), "base64");
        },
        abort(error) {
            abortPromise = rpc("abort", { message: error.message });
            // NativePage's abort is synchronous; close observes this failure.
            abortPromise.catch(() => {});
        },
        close() {
            closePromise ??= (async () => {
                await abortPromise;
                await rpc("close");
            })();
            return closePromise;
        }
    };
}

module.exports = { startBroker, createTransport };
