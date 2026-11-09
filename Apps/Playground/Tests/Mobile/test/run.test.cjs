const { test } = require("node:test");
const assert = require("node:assert/strict");
const { EventEmitter } = require("node:events");
const fs = require("node:fs");
const path = require("node:path");
const { workerCount, main } = require("../run.cjs");

test("persistent device count accepts only one positive integer worker option", () => {
    assert.equal(workerCount([]), 1);
    assert.equal(workerCount(["--workers=2"]), 2);
    assert.equal(workerCount(["-j", "3"]), 3);
    assert.equal(workerCount(["-j2"]), 2);
    for (const args of [["--workers", "0"], ["--workers", "50%"], ["--workers"], ["--workers", "2", "-j", "1"]]) {
        assert.throws(() => workerCount(args), /positive integer|only once/);
    }
});

test("run-owned devices and partial startup are always cleaned up", async t => {
    const original = process.env.MOBILE_OUTPUT;
    const originalRoot = process.env.BABYLON_JS_ROOT;
    const output = fs.mkdtempSync(path.join(__dirname, "run-results-"));
    process.env.MOBILE_OUTPUT = output;
    process.env.BABYLON_JS_ROOT = output;
    t.after(() => {
        if (original === undefined) { delete process.env.MOBILE_OUTPUT; }
        else { process.env.MOBILE_OUTPUT = original; }
        if (originalRoot === undefined) { delete process.env.BABYLON_JS_ROOT; }
        else { process.env.BABYLON_JS_ROOT = originalRoot; }
        fs.rmSync(output, { recursive: true });
    });
    let allocated = 0;
    let closed = 0;
    let uploaded = 0;
    const dependencies = {
        cli: __filename,
        auth: { redact: text => text },
        uploadApp: async () => {
            uploaded++;
            return { appUrl: "bs://fixture", sha256: "hash", bytes: 1, uploadMs: 2 };
        },
        createTransport: async () => {
            allocated++;
            return { request: async () => Buffer.from("{}"), close: async () => { closed++; } };
        },
        spawn: (_command, _args, options) => {
            assert.equal(JSON.parse(options.env.MOBILE_DEVICE_BROKERS).length, 2);
            const child = new EventEmitter();
            setImmediate(() => child.emit("exit", 1, null));
            return child;
        }
    };
    assert.equal(await main(["--workers", "2"], dependencies), 1);
    assert.equal(uploaded, 1);
    assert.equal(allocated, 2);
    assert.equal(closed, 2);
    assert.equal(JSON.parse(fs.readFileSync(path.join(output, "run.json"))).playwrightExitCode, 1);
    allocated = closed = 0;
    await assert.rejects(main(["--workers", "2"], {
        ...dependencies,
        createTransport: async () => {
            if (++allocated === 2) { throw new Error("second allocation failed"); }
            return { close: async () => { closed++; } };
        }
    }), /second allocation failed/);
    assert.equal(closed, 1);
    assert.match(JSON.parse(fs.readFileSync(path.join(output, "run.json"))).error, /second allocation failed/);
    allocated = closed = 0;
    await assert.rejects(main([], {
        ...dependencies,
        spawn: () => { throw new Error("child startup failed"); }
    }), /child startup failed/);
    assert.equal(allocated, 1);
    assert.equal(closed, 1);
    allocated = closed = 0;
    await assert.rejects(main([], {
        ...dependencies,
        spawn: () => {
            const child = new EventEmitter();
            child.kill = signal => { setImmediate(() => child.emit("exit", null, signal)); };
            setImmediate(() => process.emit("SIGINT"));
            return child;
        }
    }), /terminated by SIGINT/);
    assert.equal(allocated, 1);
    assert.equal(closed, 1);
    allocated = closed = 0;
    await assert.rejects(main([], {
        ...dependencies,
        uploadApp: ({ signal }) => new Promise((_resolve, reject) => {
            signal.addEventListener("abort", () => reject(signal.reason), { once: true });
            setImmediate(() => process.emit("SIGTERM"));
        })
    }), /interrupted by SIGTERM/);
    assert.equal(allocated, 0);
    allocated = closed = 0;
    await assert.rejects(main([], {
        ...dependencies,
        createTransport: async () => {
            allocated++;
            return { close: async () => { closed++; throw new Error("device cleanup failed"); } };
        },
        spawn: () => { throw new Error("primary startup failure"); }
    }), error => {
        assert.match(error.message, /primary startup failure/);
        assert.deepEqual(error.errors.map(item => item.message), ["primary startup failure", "device cleanup failed"]);
        return true;
    });
    assert.equal(closed, 1);
    assert.match(JSON.parse(fs.readFileSync(path.join(output, "run.json"))).cleanupError, /device cleanup failed/);
});
