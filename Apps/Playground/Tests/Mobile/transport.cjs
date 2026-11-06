const fs = require("node:fs");
const path = require("node:path");
const { randomUUID } = require("node:crypto");
const { MobileController } = require("./controller.cjs");
const { credentials, startSession } = require("./browserstack.cjs");
const { decodeFrame } = require("./framebuffer.cjs");

async function createTransport(onConsole, signal, {
    sessionFactory = startSession, auth = credentials(),
    uploadedApp = process.env.MOBILE_UPLOADED_APP ? JSON.parse(process.env.MOBILE_UPLOADED_APP) : undefined,
    workerIndex = process.env.TEST_WORKER_INDEX
} = {}) {
    const output = path.join(process.env.MOBILE_OUTPUT || path.join(__dirname, "mobile-results"), "sessions", randomUUID());
    fs.mkdirSync(output, { recursive: true });
    const controller = await new MobileController(item => onConsole(item.level, auth.redact(item.text))).listen();
    const cancelStartup = () => controller.fail(signal.reason);
    signal.addEventListener("abort", cancelStartup, { once: true });
    if (signal.aborted) {
        cancelStartup();
    }
    const report = { startedAtMs: Date.now(), workerIndex, commands: 0 };
    let mobile;
    let closePromise;
    function persist() {
        fs.writeFileSync(path.join(output, "session.json"), auth.redact(JSON.stringify(report, null, 2)));
    }
    async function close() {
        closePromise ??= (async () => {
            report.closeStartedAtMs = Date.now();
            signal.removeEventListener("abort", cancelStartup);
            const errors = [];
            try {
                await controller.close();
            } catch (error) {
                errors.push(error);
            }
            try {
                await mobile?.stop();
            } catch (error) {
                errors.push(error);
            }
            if (mobile) {
                const diagnosticsStarted = performance.now();
                try {
                    await mobile.diagnostics({
                        includeNativeLogs: Boolean(report.lastRequestError || report.startupError || errors.length)
                    });
                } catch (error) {
                    report.diagnosticsError = auth.redact(error.message);
                } finally {
                    report.diagnosticsMs = performance.now() - diagnosticsStarted;
                }
            }
            report.finishedAtMs = Date.now();
            if (errors.length) {
                report.cleanupError = auth.redact(errors.map(error => error.message).join("; "));
            }
            persist();
            if (errors.length) {
                throw new AggregateError(errors, "Native mobile transport cleanup failed");
            }
        })();
        return closePromise;
    }
    try {
        mobile = await sessionFactory({
            controller, platform: process.env.MOBILE_PLATFORM, deviceName: process.env.MOBILE_DEVICE,
            osVersion: process.env.MOBILE_OS_VERSION, app: process.env.MOBILE_APP, output, auth, signal,
            uploadedApp
        });
        report.session = mobile.measurements;
        const version = require(path.join(__dirname, "..", "..", "..", "package-lock.json")).packages["node_modules/babylonjs"].version;
        if (mobile.measurements.ready.babylonVersion !== version) {
            throw new Error("Native runtime Babylon.js version differs from the packaged lockfile");
        }
        const expectedApi = process.env.MOBILE_PLATFORM === "ios" ? "Metal" : "OpenGL";
        if (mobile.measurements.ready.graphicsApi !== expectedApi) {
            throw new Error(`Unexpected native graphics API: ${mobile.measurements.ready.graphicsApi}`);
        }
        await controller.evaluate(fs.readFileSync(path.join(__dirname, "..", "..", "Scripts", "playwright.js"), "utf8"));
        const fflateRoot = path.dirname(require.resolve("fflate/package.json"));
        await controller.evaluate(fs.readFileSync(path.join(fflateRoot, "umd", "index.js"), "utf8"));
        signal.removeEventListener("abort", cancelStartup);
        persist();
        return {
            async request(method, fields, timeoutMs) {
                report.commands++;
                try {
                    if (method === "EVALUATE") {
                        const source = Buffer.from(fields[0], "base64").toString("utf8");
                        const value = await controller.evaluate(source, undefined, { timeoutMs });
                        if (typeof value !== "string") {
                            throw new Error("Native evaluation did not return the shared adapter's JSON envelope");
                        }
                        return Buffer.from(value);
                    }
                    if (method === "RESIZE") {
                        await controller.evaluate(size => __mobilePlaywright.viewport(size),
                            { width: Number(fields[0]), height: Number(fields[1]) }, { timeoutMs });
                        return Buffer.alloc(0);
                    }
                    if (method === "SCREENSHOT") {
                        return decodeFrame(await controller.evaluate(() => __mobilePlaywright.screenshot(), undefined, { timeoutMs }));
                    }
                    throw new Error(`Unsupported native mobile request: ${method}`);
                } catch (error) {
                    report.lastRequestError = auth.redact(error.message);
                    throw error;
                } finally {
                    persist();
                }
            },
            abort(error) {
                report.abortError = auth.redact(error.message);
                controller.fail(error);
                persist();
            },
            close
        };
    } catch (error) {
        report.startupError = auth.redact(error.message);
        if (error.mobileMeasurements) {
            report.session = error.mobileMeasurements;
        }
        try {
            await close();
        } catch (cleanupError) {
            throw new AggregateError([error, cleanupError], "Native mobile startup and cleanup failed");
        }
        throw error;
    }
}

module.exports = { createTransport };
