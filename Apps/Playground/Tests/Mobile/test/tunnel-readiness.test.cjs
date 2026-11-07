const { test } = require("node:test");
const assert = require("node:assert/strict");
const { waitForCloudTunnel } = require("../tunnel-readiness.cjs");

test("cloud readiness requires the exact live identifier, not just a running process or another tunnel", async () => {
    let calls = 0;
    const result = await waitForCloudTunnel({
        key: "fixture-secret", identifier: "owned",
        sleep: async () => {},
        request: async () => ({
            ok: true,
            json: async () => ({ instances: ++calls === 1
                ? [{ localIdentifier: "other", endTime: null }, { localIdentifier: "owned", endTime: "ended" }]
                : [{ localIdentifier: "owned", endTime: null }] })
        })
    });
    assert.equal(result.polls, 2);
});

test("unregistered cloud tunnel fails rather than allowing allocation", async () => {
    await assert.rejects(waitForCloudTunnel({
        key: "fixture-secret", identifier: "owned", timeoutMs: 10,
        request: async () => ({ ok: true, json: async () => ({ instances: [] }) })
    }), /did not register/);
});

test("credential-bearing request failures are not propagated", async () => {
    await assert.rejects(waitForCloudTunnel({
        key: "fixture-secret", identifier: "owned",
        request: async url => { throw new Error(url); }
    }), error => {
        assert.match(error.message, /request failed/);
        assert.equal(error.message.includes("fixture-secret"), false);
        return true;
    });
    await assert.rejects(waitForCloudTunnel({
        key: "fixture-secret", identifier: "owned",
        request: async () => ({ ok: true, json: async () => ({ wrong: [] }) })
    }), /instances collection/);
});
