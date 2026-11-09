const fs = require("node:fs");
const path = require("node:path");
const vm = require("node:vm");
const { test } = require("node:test");
const assert = require("node:assert/strict");

function fixture(t, { rejectAllocation = false, rejectRegistration = false, rejectLogRemoval = false, rejectAllocationBody = false, nativeLogFailures } = {}) {
    const root = fs.mkdtempSync(path.join(__dirname, ".cloud-fixture-"));
    const output = path.join(root, "results");
    t.after(() => fs.rmSync(root, { recursive: true, force: true }));
    const calls = [];
    const sink = () => {};
    const console = { error: sink };
    let running = false;
    class FakeLocal {
        start(options, callback) {
            assert.equal(options["enable-logging-for-api"], true);
            assert.equal(options.onlyAutomate, true);
            fs.writeFileSync(options.logFile, "fixture private log");
            this.pid = 12345;
            running = true;
            callback();
        }
        isRunning() { return running; }
        stop(callback) {
            calls.push("stop-local");
            running = false;
            callback();
        }
    }
    const mockFetch = async (url, options = {}) => {
        const route = String(url);
        calls.push(`${options.method || "GET"} ${route}`);
        assert.equal(options.headers.Authorization, "Basic fixture");
        let data;
        if (route.endsWith("/plan.json")) {
            data = { parallel_sessions_max_allowed: 5, parallel_sessions_running: 0 };
        } else if (route.endsWith("/devices.json")) {
            data = [{ device: "fixture-device", os: "android", os_version: "12.0" }];
        } else if (route.endsWith("/upload")) {
            data = { app_url: "bs://fixture" };
        } else if (route.endsWith("/session") && options.method === "POST") {
            if (rejectAllocationBody) {
                return { ok: true, text: async () => { throw new Error("fixture-secret response body timed out"); } };
            }
            data = rejectAllocation ? { value: { error: "fixture-error", message: "fixture-secret allocation" } }
                : { value: { sessionId: "fixture-session" } };
        } else if (route.endsWith("/fixture-session") && options.method === "DELETE") {
            data = { value: null };
        } else if (route.endsWith("/fixture-session.json")) {
            data = { automation_session: { duration: 7, status: "done",
                ...(nativeLogFailures === undefined ? {} : { device_logs_url: "https://api.browserstack.com/logs/fixture.txt" }) } };
        } else if (route.endsWith("/logs/fixture.txt")) {
            const attempts = calls.filter(call => call.endsWith("/logs/fixture.txt")).length;
            return attempts <= nativeLogFailures ? { ok: false, status: 404 } :
                { ok: true, status: 200, text: async () =>
                    "unrelated log\nPlayground(CFNetwork) ATS -1022 fixture-secret\nBabylonNative native output" };
        } else {
            throw new Error("Unexpected fixture API route");
        }
        return { ok: true, text: async () => JSON.stringify(data) };
    };
    const module = { exports: {} };
    vm.runInNewContext(fs.readFileSync(path.join(__dirname, "..", "browserstack.cjs"), "utf8"), {
        require: name => {
            if (name === "node:os") {
                return { ...require(name), tmpdir: () => root };
            }
            if (name === "node:fs" && rejectLogRemoval) {
                return { ...fs, rmSync: () => { throw Object.assign(new Error("fixture removal denied"), { code: "EACCES" }); } };
            }
            if (name === "browserstack-local") {
                return { Local: FakeLocal };
            }
            if (name === "./tunnel-readiness.cjs") {
                return { waitForCloudTunnel: async () => {
                    if (rejectRegistration) {
                        throw new Error("fixture-secret registration");
                    }
                    return { polls: 1, elapsedMs: 2 };
                } };
            }
            return require(name);
        },
        module, console, fetch: mockFetch, performance, Blob, FormData, AbortSignal, URL,
        setInterval, clearInterval, setTimeout: callback => setImmediate(callback),
        process: { env: { BROWSERSTACK_LOCAL_BINARY: __filename } }
    }, { filename: "browserstack-fixture.cjs" });
    const options = {
        controller: { port: 1234, devicePath: "/device/fixture", ready: async () => ({ protocol: 2 }) },
        platform: "android", deviceName: "fixture-device", osVersion: "12.0",
        app: __filename, output,
        auth: { key: "fixture-secret", authorization: "Basic fixture",
            redact: value => String(value).split("fixture-secret").join("[REDACTED]") }
    };
    return { startSession: module.exports.startSession, uploadApp: module.exports.uploadApp, options, calls, root, output, console, sink,
        isRunning: () => running };
}

test("successful session cleanup deletes only its cloud session and owned tunnel, including the private log", async t => {
    const f = fixture(t);
    const session = await f.startSession(f.options);
    await session.stop();
    await session.diagnostics();
    await session.stop();
    assert.equal(f.isRunning(), false);
    assert.equal(f.calls.filter(call => call.startsWith("DELETE ")).length, 1);
    assert.equal(f.calls.filter(call => call === "stop-local").length, 1);
    assert.deepEqual(fs.readdirSync(f.output), []);
    assert.equal(f.console.error, f.sink);
    assert.equal(session.measurements.deviceSeconds, 7);
    assert.equal(session.measurements.sessionDeleted, true);
    assert.equal(session.measurements.localStopped, true);
});

for (const failure of ["rejectAllocation", "rejectRegistration"]) {
    test(`${failure} cleans up an already started tunnel and retains redacted startup measurements`, async t => {
        const f = fixture(t, { [failure]: true });
        await assert.rejects(f.startSession(f.options), error => {
            assert.equal(error.message.includes("fixture-secret"), false);
            assert.equal(error.mobileMeasurements.localStopped, true);
            return true;
        });
        assert.equal(f.isRunning(), false);
        assert.deepEqual(fs.readdirSync(f.output), []);
        assert.equal(f.calls.some(call => call.startsWith("DELETE ")), false);
        assert.equal(f.console.error, f.sink);
    });
}

test("private logs cannot enter result artifacts even when their removal fails", async t => {
    const f = fixture(t, { rejectLogRemoval: true });
    const session = await f.startSession(f.options);
    await assert.rejects(session.stop(), /Mobile cloud cleanup failed/);
    assert.equal(f.isRunning(), false);
    assert.deepEqual(fs.readdirSync(f.output), []);
    const privateDirectory = fs.readdirSync(f.root).find(name => name.startsWith("babylon-native-mobile-"));
    assert.equal(fs.readFileSync(path.join(f.root, privateDirectory, "local.log"), "utf8"), "fixture private log");
    assert.match(session.measurements.cleanupErrors[0], /EACCES/);
});

test("allocation response-body failures name the request, redact credentials and release the tunnel", async t => {
    const f = fixture(t, { rejectAllocationBody: true });
    await assert.rejects(f.startSession(f.options), error => {
        assert.match(error.message, /POST \/wd\/hub\/session request failed/);
        assert.equal(error.message.includes("fixture-secret"), false);
        assert.equal(error.mobileMeasurements.localStopped, true);
        return true;
    });
    assert.equal(f.isRunning(), false);
});

test("combined startup and cleanup failure preserves measurements without publishing private logs", async t => {
    const f = fixture(t, { rejectAllocation: true, rejectLogRemoval: true });
    await assert.rejects(f.startSession(f.options), error => {
        assert.match(error.message, /Startup and cleanup failed/);
        assert.equal(error.mobileMeasurements.localStopped, true);
        assert.equal(error.errors[0].message.includes("fixture-secret"), false);
        return true;
    });
    assert.equal(f.isRunning(), false);
    assert.deepEqual(fs.readdirSync(f.output), []);
});

test("startup timeout retains native diagnostics after releasing its allocated session", async t => {
    const f = fixture(t);
    f.options.controller.ready = async () => { throw new Error("fixture startup timeout"); };
    await assert.rejects(f.startSession(f.options), error => {
        assert.match(error.message, /fixture startup timeout/);
        assert.equal(error.mobileMeasurements.deviceSeconds, 7);
        assert.equal(error.mobileMeasurements.sessionDeleted, true);
        assert.equal(error.mobileMeasurements.localStopped, true);
        return true;
    });

    assert.equal(f.isRunning(), false);
    assert.ok(f.calls.findIndex(call => call.startsWith("DELETE ")) <
        f.calls.findIndex(call => call.endsWith("/fixture-session.json")));
});

test("native diagnostics wait for delayed logs, retain network-policy errors and redact credentials", async t => {
    const f = fixture(t, { nativeLogFailures: 2 });
    const session = await f.startSession(f.options);
    await session.stop();
    await session.diagnostics();
    const text = fs.readFileSync(path.join(f.output, "fixture-session-native.log"), "utf8");
    assert.equal(f.calls.filter(call => call.endsWith("/logs/fixture.txt")).length, 3);
    assert.match(text, /Playground\(CFNetwork\) ATS -1022 \[REDACTED\]/);
    assert.equal(text.includes("fixture-secret"), false);
    assert.equal(text.includes("unrelated log"), false);
});

test("missing native diagnostics preserve the startup failure and report the diagnostics failure", async t => {
    const f = fixture(t, { nativeLogFailures: 5 });
    f.options.controller.ready = async () => { throw new Error("fixture startup timeout"); };
    await assert.rejects(f.startSession(f.options), error => {
        assert.match(error.message, /fixture startup timeout/);
        assert.match(error.mobileMeasurements.diagnosticsError, /native diagnostics HTTP 404/);
        assert.equal(error.mobileMeasurements.deviceSeconds, 7);
        return true;
    });
    assert.equal(f.calls.filter(call => call.endsWith("/logs/fixture.txt")).length, 5);
});

test("a successful run retains occupancy without waiting for optional native logs", async t => {
    const f = fixture(t, { nativeLogFailures: 5 });
    const session = await f.startSession(f.options);
    await session.stop();
    await session.diagnostics({ includeNativeLogs: false });
    assert.equal(session.measurements.deviceSeconds, 7);
    assert.equal(f.calls.filter(call => call.endsWith("/logs/fixture.txt")).length, 0);
});

test("replacement workers reuse one exact-package upload", async t => {
    const f = fixture(t);
    f.options.uploadedApp = await f.uploadApp(f.options);
    for (let worker = 0; worker < 2; worker++) {
        const session = await f.startSession(f.options);
        assert.equal(session.measurements.uploadShared, true);
        assert.equal(session.measurements.uploadMs, 0);
        await session.stop();
    }
    assert.equal(f.calls.filter(call => call.endsWith("/upload")).length, 1);
    assert.equal(f.calls.filter(call => call.startsWith("DELETE ")).length, 2);
});

test("a shared upload for different bytes is rejected before allocation", async t => {
    const f = fixture(t);
    f.options.uploadedApp = { ...await f.uploadApp(f.options), sha256: "not-the-package-hash" };
    await assert.rejects(f.startSession(f.options), /does not match the exact mobile package/);
    assert.equal(f.calls.some(call => call.endsWith("/session")), false);
    assert.equal(f.isRunning(), false);
});
