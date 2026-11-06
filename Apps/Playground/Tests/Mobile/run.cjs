const path = require("node:path");
const fs = require("node:fs");
const { spawn } = require("node:child_process");

const { startBroker } = require("./device-broker.cjs");

function workerCount(args) {
    let workers = 1;
    let specified = false;
    for (let i = 0; i < args.length; i++) {
        if (args[i] === "--workers" || args[i].startsWith("-j") || args[i].startsWith("--workers=")) {
            if (specified) { throw new Error("Specify the worker/device count only once"); }
            specified = true;
            const value = args[i].startsWith("--workers=") ? args[i].slice("--workers=".length)
                : args[i].startsWith("-j") && args[i].length > 2 ? args[i].slice(2) : args[++i];
            if (!/^[1-9][0-9]*$/.test(value || "") || !Number.isSafeInteger(Number(value))) {
                throw new Error("Workers must be a positive integer: one persistent device per worker slot");
            }
            workers = Number(value);
        }
    }
    return workers;
}

async function main(args = process.argv.slice(2), dependencies = {}) {
    if (!process.env.BABYLON_JS_ROOT) {
        throw new Error("Set BABYLON_JS_ROOT to the Babylon.js checkout containing the shared native Playwright suite");
    }
    if (process.env.MOBILE_TEST_INDICES) {
        throw new Error("MOBILE_TEST_INDICES selected the removed native catalog; use Playwright --grep instead");
    }
    if (args.some(arg => /^--(?:update-snapshots|config|project)(?:=|$)/.test(arg) || ["-c", "-p", "-u"].includes(arg))) {
        throw new Error("The shared native project, configuration and unchanged snapshots are fixed by this runner");
    }
    const workers = workerCount(args);
    const root = path.resolve(process.env.BABYLON_JS_ROOT);
    const cli = dependencies.cli || require.resolve("@playwright/test/cli", { paths: [root] });
    args = [...args];
    if (process.env.MOBILE_TEST_GREP) { args.push("--grep", process.env.MOBILE_TEST_GREP); }
    if (process.env.MOBILE_SHARD) { args.push("--shard", process.env.MOBILE_SHARD); }
    const env = { ...process.env };
    const discovery = args.some(arg => ["--list", "--help", "-h", "--version", "-V"].includes(arg));
    const devices = [];
    const cancellation = new AbortController();
    const started = performance.now();
    const report = { startedAtMs: Date.now(), workers, devices: [] };
    const output = env.MOBILE_OUTPUT || path.join(__dirname, "mobile-results");
    let child;
    let auth;
    let primaryError;
    const cancel = signal => {
        cancellation.abort(new Error(`Mobile visual run interrupted by ${signal}`));
        child?.kill(signal);
    };
    const interrupt = () => cancel("SIGINT");
    const terminate = () => cancel("SIGTERM");
    process.once("SIGINT", interrupt);
    process.once("SIGTERM", terminate);
    try {
        if (!discovery) {
            const cloud = require("./browserstack.cjs");
            auth = dependencies.auth || cloud.credentials();
            const upload = dependencies.uploadApp || cloud.uploadApp;
            const create = dependencies.createTransport || require("./transport.cjs").createTransport;
            fs.mkdirSync(output, { recursive: true });
            const uploadedApp = await upload({ app: env.MOBILE_APP, auth, signal: cancellation.signal });
            env.MOBILE_UPLOADED_APP = JSON.stringify(uploadedApp);
            const { bytes, sha256, uploadMs } = uploadedApp;
            fs.writeFileSync(path.join(output, "app-upload.json"), JSON.stringify({ bytes, sha256, uploadMs }, null, 2));
            for (let slot = 0; slot < workers; slot++) {
                cancellation.signal.throwIfAborted();
                const messages = [];
                const ready = performance.now();
                const transport = await create((level, text) => messages.push({ level, text }),
                    AbortSignal.any([cancellation.signal, AbortSignal.timeout(180_000)]),
                    { auth, uploadedApp, workerIndex: `run-owner-${slot}` });
                const device = { transport };
                devices.push(device);
                device.broker = await startBroker(transport, messages);
                report.devices.push({ slot, readyMs: performance.now() - ready, history: device.broker.history });
            }
            env.MOBILE_DEVICE_BROKERS = JSON.stringify(devices.map(device => device.broker.url));
        } else {
            delete env.MOBILE_DEVICE_BROKERS;
        }
        cancellation.signal.throwIfAborted();
        const spawnChild = dependencies.spawn || spawn;
        const testStarted = performance.now();
        report.playwrightExitCode = await new Promise((resolve, reject) => {
            child = spawnChild(process.execPath, [cli, "test", "--config", path.join(__dirname, "playwright.config.cjs"), ...args], {
                cwd: root, stdio: "inherit", env
            });
            child.once("error", reject);
            child.once("exit", (code, signal) => {
                if (signal) { reject(new Error(`Native Playwright runner terminated by ${signal}`)); }
                else { resolve(code ?? 1); }
            });
        });
        report.playwrightMs = performance.now() - testStarted;
        cancellation.signal.throwIfAborted();
    } catch (error) {
        primaryError = error;
        report.error = auth ? auth.redact(error.message) : error.message;
        throw error;
    } finally {
        process.removeListener("SIGINT", interrupt);
        process.removeListener("SIGTERM", terminate);
        const errors = [];
        for (const device of devices) {
            try { await device.broker?.close(); } catch (error) { errors.push(error); }
            try { await device.transport.close(); } catch (error) { errors.push(error); }
            if (device.broker?.failure) { errors.push(new Error(device.broker.failure)); }
        }
        report.elapsedMs = performance.now() - started;
        report.finishedAtMs = Date.now();
        if (errors.length) { report.cleanupError = errors.map(error => error.message).join("; "); }
        if (!discovery && auth) {
            fs.mkdirSync(output, { recursive: true });
            fs.writeFileSync(path.join(output, "run.json"), auth.redact(JSON.stringify(report, null, 2)));
        }
        if (errors.length) {
            throw new AggregateError(primaryError ? [primaryError, ...errors] : errors,
                `Mobile visual run cleanup failed: ${report.error || report.cleanupError}`);
        }
    }
    return report.playwrightExitCode;
}

module.exports = { main, workerCount };
if (require.main === module) {
    main().then(code => { process.exitCode = code; }).catch(error => {
        const { credentials } = require("./browserstack.cjs");
        const redact = process.env.BROWSERSTACK_USERNAME && process.env.BROWSERSTACK_ACCESS_KEY
            ? credentials().redact : text => text;
        console.error(redact(error.message));
        process.exitCode = 1;
    });
}
