const assert = require("node:assert/strict");
const { readFileSync } = require("node:fs");
const { join } = require("node:path");
const { test } = require("node:test");
const { runInNewContext } = require("node:vm");

const source = readFileSync(join(__dirname, "..", "Scripts", "validation_native.js"), "utf8");

test("restores forward depth between scenes on the reused engine", () => {
    const start = source.indexOf("    function cleanupAfterTest(");
    const end = source.indexOf("    function makeTestDone(", start);
    assert.ok(start >= 0 && end > start);
    let disposed = 0;
    const engine = {
        scenes: [],
        useReverseDepthBuffer: true,
        setHardwareScalingLevel() {},
        setStencilBuffer() {},
        disableScissor() {},
        releaseEffects() { assert.equal(this.useReverseDepthBuffer, false); },
        getLoadedTexturesCache() { return []; },
        clearInternalTexturesCache() {},
    };
    const context = {
        engine,
        currentScene: { dispose() { disposed++; } },
        console,
        BABYLON: { SceneLoader: { OnPluginActivatedObservable: { clear() {} } } },
    };
    runInNewContext(source.slice(start, end), context);
    context.cleanupAfterTest();
    context.cleanupAfterTest();
    assert.equal(disposed, 1);
    assert.equal(context.currentScene, null);
    assert.equal(engine.useReverseDepthBuffer, false);
});
