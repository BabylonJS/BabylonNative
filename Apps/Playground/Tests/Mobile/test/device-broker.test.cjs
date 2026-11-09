const { test } = require("node:test");
const assert = require("node:assert/strict");
const { startBroker, createTransport } = require("../device-broker.cjs");

async function fixture(t, request) {
    const original = { MOBILE_DEVICE_BROKERS: process.env.MOBILE_DEVICE_BROKERS, TEST_PARALLEL_INDEX: process.env.TEST_PARALLEL_INDEX };
    const state = { aborts: 0, calls: [] };
    const broker = await startBroker({
        request: async (...args) => {
            state.calls.push(args);
            return request ? request(...args) : Buffer.from("result");
        },
        abort() { state.aborts++; }
    });
    process.env.MOBILE_DEVICE_BROKERS = JSON.stringify([broker.url]);
    process.env.TEST_PARALLEL_INDEX = "0";
    t.after(async () => {
        await broker.close();
        for (const [key, value] of Object.entries(original)) {
            if (value === undefined) { delete process.env[key]; }
            else { process.env[key] = value; }
        }
    });
    return { broker, state, acquire: () => createTransport(() => {}, AbortSignal.timeout(1000)) };
}

test("replacement workers reuse the same device with an exclusive, reset lease", async t => {
    const { broker, state, acquire } = await fixture(t);
    const first = await acquire();
    await assert.rejects(acquire(), /Only one worker/);
    assert.equal((await first.request("SCREENSHOT", [], 1000)).toString(), "result");
    first.abort(new Error("snapshot failed"));
    await Promise.all([first.close(), first.close()]);
    const second = await acquire();
    await second.close();
    assert.equal(state.aborts, 0);
    assert.deepEqual(broker.history.map(item => item.event), ["acquire", "abort", "release", "acquire", "release"]);
    const source = Buffer.from(state.calls[1][1][0], "base64").toString();
    assert.match(source, /setHardwareScalingLevel\(1\)/);
    assert.match(source, /scene\.dispose/);
    assert.match(source, /engine\.dispose/);
    assert.match(source, /delete globalThis\.__babylonNativeScreenshot/);
});

test("ordinary scene errors remain failures but permit replacement after recovery", async t => {
    let calls = 0;
    const { broker, acquire } = await fixture(t, async () => {
        if (++calls === 1) { throw new Error("scene creation failed"); }
        return Buffer.from("{}");
    });
    const first = await acquire();
    await assert.rejects(first.request("EVALUATE", ["source"], 1000), /scene creation failed/);
    await first.close();
    const second = await acquire();
    await second.close();
    assert.equal(broker.failure, undefined);
    assert.ok(broker.history.some(item => item.event === "recovered"));
});

test("a poisoned device is never silently replaced or handed to another worker", async t => {
    const { broker, acquire } = await fixture(t, async () => { throw new Error("transport timed out"); });
    const first = await acquire();
    await assert.rejects(first.request("SCREENSHOT", [], 1000), /transport timed out/);
    await assert.rejects(first.close(), /recovery failed/);
    await assert.rejects(acquire(), /recovery failed/);
    assert.match(broker.failure, /recovery failed/);
});

test("an abort during a command poisons the device instead of replaying work", async t => {
    let finish;
    const { broker, state, acquire } = await fixture(t, () => new Promise(resolve => { finish = resolve; }));
    const first = await acquire();
    const pending = first.request("SCREENSHOT", [], 1000);
    while (!finish) { await new Promise(resolve => setImmediate(resolve)); }
    first.abort(new Error("test timed out"));
    await assert.rejects(first.close(), /in-flight|still in flight/);
    finish(Buffer.alloc(0));
    await pending;
    assert.equal(state.aborts, 1);
    assert.match(broker.failure, /still in flight/);
});

test("invalid native commands and stale leases cannot touch the device", async t => {
    const { broker, state, acquire } = await fixture(t);
    const first = await acquire();
    await assert.rejects(first.request("DELETE", [], 1000), /Invalid native command/);
    const response = await fetch(broker.url, {
        method: "POST", body: JSON.stringify({ operation: "request", lease: "stale", method: "SCREENSHOT", fields: [], timeoutMs: 1000 })
    });
    assert.equal(response.status, 409);
    assert.equal(state.calls.length, 0);
    await first.close();
});
