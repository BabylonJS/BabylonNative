const fs = require("node:fs");
const path = require("node:path");
const vm = require("node:vm");
const { test } = require("node:test");
const assert = require("node:assert/strict");
const { PNG } = require("pngjs");
const { MobileController } = require("../controller.cjs");
const { credentials } = require("../browserstack.cjs");

async function device(t, { onEvent } = {}) {
    const controller = await new MobileController(onEvent).listen();
    const endpoint = `http://127.0.0.1:${controller.port}${controller.devicePath}`;
    const cancellation = new AbortController();
    const errors = [];
    const nativeUtils = Object.freeze(Object.fromEntries([
        "setMSAASamples", "setTitle", "getGraphicsApiName", "decodeImage", "getImageData",
        "getFrameBufferData", "captureNextFrame"
    ].map(name => [name, function () {
        assert.equal(this, nativeUtils);
        return name === "getGraphicsApiName" ? "OpenGL" : undefined;
    }])));
    const context = vm.createContext({
        __mobilePlaywrightUrl: endpoint,
        BABYLON: {
            Engine: { Version: "9.29.0" },
            NativeEngine: class {
                constructor() { throw new Error("Premature Babylon.js engine would freeze matrix precision"); }
            }
        },
        _native: {
            Engine: class {
                getRenderWidth() { return 600; }
                getRenderHeight() { return 400; }
                dispose() {}
            }
        },
        TestUtils: nativeUtils,
        fetch: (url, options = {}) => {
            assert.ok(!url.startsWith("app:///"), "The transport must not load a test runner or catalog");
            return fetch(url, { ...options, signal: cancellation.signal });
        },
        console: { log() {}, warn() {}, error: text => errors.push(text) },
        performance, TextEncoder, TextDecoder
    });
    vm.runInContext(fs.readFileSync(path.join(__dirname, "..", "..", "..", "Scripts", "mobilePlaywright.js"), "utf8"), context);
    t.after(async () => {
        cancellation.abort();
        await controller.close();
    });
    await controller.ready(1000);
    return { controller, context, errors, nativeUtils };
}

test("packaged mobile client completes asynchronous evaluations and retains error text plus stack", async t => {
    const { controller } = await device(t);
    assert.equal(await controller.evaluate(async n => n * 2, 21), 42);
    await assert.rejects(controller.evaluate(() => { throw new Error("intentional-ios-message"); }), error => {
        assert.match(error.message, /intentional-ios-message/);
        assert.match(error.deviceStack, /intentional-ios-message/);
        return true;
    });
    assert.equal(await controller.evaluate(() => "after-error"), "after-error");
});

test("device PNG export preserves every RGBA byte including alpha", async t => {
    const { controller, context } = await device(t);
    const root = path.dirname(require.resolve("fflate/package.json"));
    vm.runInContext(fs.readFileSync(path.join(root, "umd", "index.js"), "utf8"), context);
    const image = await controller.evaluate(() => {
        const data = new Uint8Array(16 * 8 * 4);
        for (let i = 0; i < data.length; ++i) {
            data[i] = i % 251;
        }
        return __mobilePlaywright.encodePNG(data, 16, 8);
    });
    const encoded = Buffer.from(image.png, "base64");
    assert.equal(encoded.length, image.pngBytes);
    const decoded = PNG.sync.read(encoded, { checkCRC: true });
    assert.equal(decoded.width, 16);
    assert.equal(decoded.height, 8);
    for (let i = 0; i < decoded.data.length; ++i) {
        assert.equal(decoded.data[i], i % 251);
    }
    encoded[encoded.length - 1] ^= 1;
    assert.throws(() => PNG.sync.read(encoded, { checkCRC: true }));
});

test("invalid framebuffer metadata and missing compression are explicit failures", async t => {
    const { controller } = await device(t);
    await assert.rejects(controller.evaluate(() => __mobilePlaywright.encodePNG(new Uint8Array(4), 2, 1)), /dimensions or length/);
    await assert.rejects(controller.evaluate(() => __mobilePlaywright.encodePNG(new Uint8Array(4), 1, 1)), /compression was not installed/);
});

test("hardware-scaled export uses native integer dimensions without rejecting larger frames", async t => {
    const { controller, context } = await device(t);
    const root = path.dirname(require.resolve("fflate/package.json"));
    vm.runInContext(fs.readFileSync(path.join(root, "umd", "index.js"), "utf8"), context);
    const image = await controller.evaluate(() =>
        __mobilePlaywright.encodePNG(new Uint8Array(666 * 444 * 4), 600 / 0.9, 400 / 0.9));
    const decoded = PNG.sync.read(Buffer.from(image.png, "base64"), { checkCRC: true });
    assert.equal(decoded.width, 666);
    assert.equal(decoded.height, 444);
    assert.equal(decoded.data.length, 666 * 444 * 4);
});

test("transport forwards console output without replacing native bindings or loading a catalog", async t => {
    const events = [];
    const { controller, context, nativeUtils } = await device(t, { onEvent: async message => events.push(message) });
    await controller.evaluate(() => { console.error("fixture scene error"); TestUtils.setMSAASamples(0); });
    assert.deepEqual(events, [{ level: "error", text: "fixture scene error" }]);
    assert.equal(context.TestUtils, nativeUtils);
    assert.equal(context._playgroundOptions, undefined);
    assert.equal(context.__mobileValidation, undefined);
});

test("credential redaction covers literal, URL-encoded and basic-auth forms", () => {
    assert.throws(() => credentials({}), /approved credential source/);
    const auth = credentials({ BROWSERSTACK_USERNAME: "fixture-user", BROWSERSTACK_ACCESS_KEY: "fixture/key+" });
    const unsafe = [
        "fixture-user", "fixture/key+", encodeURIComponent("fixture/key+"),
        Buffer.from("fixture-user:fixture/key+").toString("base64")
    ].join(" ");
    assert.equal(auth.redact(unsafe), "[REDACTED] [REDACTED] [REDACTED] [REDACTED]");
});

test("density-scaled framebuffer fails before initializing a Playwright scene", async t => {
    const { controller, context } = await device(t);
    vm.runInContext("_native.Engine.prototype.getRenderWidth = function () { return 200; };", context);
    await assert.rejects(controller.evaluate(() => __mobilePlaywright.viewport({ width: 600, height: 400 })),
        /Mobile framebuffer is 200x400, expected 600x400/);
});
