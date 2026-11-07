const fs = require("node:fs");
const path = require("node:path");
const { test } = require("node:test");
const assert = require("node:assert/strict");
const { PNG } = require("pngjs");
const { createTransport } = require("../transport.cjs");
const { decodeFrame } = require("../framebuffer.cjs");
const { sessionFactory } = require("./fixtures/session.cjs");

const auth = { redact: text => text.replaceAll("fixture-secret", "[REDACTED]") };

async function fixture(t, options = {}, signal = new AbortController().signal) {
    const output = fs.mkdtempSync(path.join(__dirname, "transport-results-"));
    const original = { MOBILE_OUTPUT: process.env.MOBILE_OUTPUT, MOBILE_PLATFORM: process.env.MOBILE_PLATFORM };
    process.env.MOBILE_OUTPUT = output;
    process.env.MOBILE_PLATFORM = "android";
    const session = sessionFactory(options);
    t.after(() => {
        for (const [name, value] of Object.entries(original)) {
            if (value === undefined) {
                delete process.env[name];
            } else {
                process.env[name] = value;
            }
        }
        fs.rmSync(output, { recursive: true });
    });
    const logs = [];
    return {
        output, session, logs,
        launch: () => createTransport((level, text) => logs.push({ level, text }), signal, { sessionFactory: session.factory, auth })
    };
}

function evaluate(transport, source, timeout = 1000) {
    const envelope = `(async () => JSON.stringify({value: await (0, eval)(${JSON.stringify(source)})}))()`;
    return transport.request("EVALUATE", [Buffer.from(envelope).toString("base64")], timeout)
        .then(bytes => JSON.parse(bytes.toString()).value);
}

test("mobile transport implements the shared NativePage envelope and consumes captured frames", async t => {
    const { launch, session, logs } = await fixture(t);
    const transport = await launch();
    t.after(() => transport.close());
    assert.equal(await evaluate(transport, "21 * 2"), 42);
    await evaluate(transport, "console.warn('fixture-secret')");
    assert.deepEqual(logs, [{ level: "warn", text: "[REDACTED]" }]);
    await transport.request("RESIZE", ["600", "400"], 1000);
    await assert.rejects(transport.request("RESIZE", ["200", "133"], 1000), /expected 200x133/);
    await assert.rejects(transport.request("SCREENSHOT", [], 1000), /did not capture/);
    await evaluate(transport, "__babylonNativeScreenshot = {data: new Uint8Array([1,2,3,4]), width:1, height:1}; 0");
    const png = await transport.request("SCREENSHOT", [], 1000);
    assert.deepEqual([...PNG.sync.read(png).data], [1, 2, 3, 4]);
    await assert.rejects(transport.request("SCREENSHOT", [], 1000), /did not capture/);
    await Promise.all([transport.close(), transport.close()]);
    assert.equal(session.state.stopped, 1);
    assert.equal(session.state.diagnostics, 1);
});

for (const [options, expected] of [
    [{ version: "wrong" }, /version differs/],
    [{ api: "Metal" }, /Unexpected native graphics API/],
    [{ startupError: "allocation failed" }, /allocation failed/]
]) {
    test(`failed startup cleans up and persists its reason: ${JSON.stringify(options)}`, async t => {
        const { launch, session, output } = await fixture(t, options);
        await assert.rejects(launch(), expected);
        assert.equal(session.state.stopped, options.startupError ? 0 : 1);
        const directories = fs.readdirSync(path.join(output, "sessions"));
        assert.equal(directories.length, 1);
        const report = JSON.parse(fs.readFileSync(path.join(output, "sessions", directories[0], "session.json")));
        assert.match(report.startupError, expected);
        assert.ok(report.finishedAtMs >= report.startedAtMs);
    });
}

test("an aborted startup never allocates a device", async t => {
    const cancellation = new AbortController();
    cancellation.abort(new Error("cancelled startup"));
    const { launch, session } = await fixture(t, {}, cancellation.signal);
    await assert.rejects(launch(), /cancelled startup/);
    assert.equal(session.state.stopped, 0);
});

test("a command timeout poisons the transport and still permits cloud cleanup", async t => {
    const { launch, session } = await fixture(t);
    const transport = await launch();
    await assert.rejects(evaluate(transport, "new Promise(() => {})", 20), /timed out/);
    await assert.rejects(evaluate(transport, "42"), /timed out/);
    await transport.close();
    assert.equal(session.state.stopped, 1);
    assert.equal(session.state.diagnosticRequests[0].includeNativeLogs, true);
});

test("a host assertion abort retains occupancy without waiting for native device logs", async t => {
    const { launch, session } = await fixture(t);
    const transport = await launch();
    assert.equal(await evaluate(transport, "42"), 42);
    transport.abort(new Error("Native test failed: snapshot mismatch"));
    await transport.close();
    assert.equal(session.state.stopped, 1);
    assert.equal(session.state.diagnostics, 1);
    assert.equal(session.state.diagnosticRequests[0].includeNativeLogs, false);
});

test("cloud cleanup failure is reported rather than a successful close", async t => {
    const { launch, session } = await fixture(t, { cleanupError: "delete session failed" });
    const transport = await launch();
    await assert.rejects(transport.close(), error => {
        assert.match(error.message, /cleanup failed/);
        assert.match(error.errors[0].message, /delete session failed/);
        return true;
    });
    await assert.rejects(transport.close(), /cleanup failed/);
    assert.equal(session.state.stopped, 1);
    assert.equal(session.state.diagnosticRequests[0].includeNativeLogs, true);
});

test("frame decoder rejects mismatched metadata and corrupted PNG checksums", () => {
    const bytes = PNG.sync.write({ width: 1, height: 1, data: Buffer.from([1, 2, 3, 4]) });
    const image = { png: bytes.toString("base64"), pngBytes: bytes.length, width: 1, height: 1 };
    assert.deepEqual(decodeFrame(image), bytes);
    assert.throws(() => decodeFrame({ ...image, width: 2 }), /differ from metadata/);
    bytes[bytes.length - 1] ^= 1;
    assert.throws(() => decodeFrame({ ...image, png: bytes.toString("base64") }), /crc/i);
});
