const assert = require("node:assert/strict");
const { readFileSync } = require("node:fs");
const path = require("node:path");
const { test } = require("node:test");
const vm = require("node:vm");

const source = readFileSync(path.join(__dirname, "..", "Scripts", "validation_native.js"), "utf8");

function makeScene(engine) {
    const scene = {
        textures: [],
        meshes: [],
        ready: true,
        renderId: 0,
        rendered: 0,
        disposed: 0,
        getEngine: () => engine,
        isReady() { return this.ready; },
        incrementRenderId() { ++this.renderId; },
        render() { ++this.rendered; ++engine.rendered; },
        dispose() {
            ++this.disposed;
            engine.scenes = engine.scenes.filter(value => value !== this);
        },
        onReadyTimeoutObservable: {
            addOnce(callback) { scene.readyTimeout = callback; },
        },
        executeWhenReady(callback) {
            this.readyCallback = callback;
            if (!this.deferReady) {
                callback();
            }
        },
    };
    scene.activeCamera = { renderPassId: 7, getScene: () => scene };
    return scene;
}

function createRunner(options = {}) {
    const state = { scenes: [], callbacks: [], timers: [], errors: [], logs: [], exits: [], reads: [], captures: [] };
    let math;
    const definitions = options.tests || [{ title: "test", renderCount: options.renderCount || 1 }];
    class Engine {
        constructor() {
            this.scenes = [];
            this._virtualScenes = [];
            this.currentRenderPassId = 91;
            this.rendered = 0;
        }
        getCaps() { return {}; }
        runRenderLoop(callback) { this.loop = callback; state.callbacks.push(callback); }
        stopRenderLoop() { this.loop = null; }
        getHardwareScalingLevel() { return 1; }
        setHardwareScalingLevel() {}
        setStencilBuffer() {}
        disableScissor() {}
        releaseEffects() {}
        getLoadedTexturesCache() { return []; }
        clearInternalTexturesCache() {}
        dispose() { this.disposed = true; }
    }
    const context = vm.createContext({
        _playgroundOptions: { saveResults: false, captureFrame: options.captureFrame || 0 },
        window: {},
        console: {
            log: (...args) => state.logs.push(args.join(" ")),
            error: (...args) => state.errors.push(args.map(String).join(" ")),
        },
        setTimeout: callback => state.timers.push(callback),
        _native: { Canvas: { loadTTFAsync: () => ({ then: callback => callback() }) } },
        TestUtils: {
            setTitle() {},
            updateSize() {},
            getGraphicsApiName: () => "D3D11",
            getOutputDirectory: () => "unused",
            decodeImage: () => new Uint8Array([0, 0, 0, 255]),
            getImageData: image => image,
            getFrameBufferData(callback) {
                state.reads.push(context.engine.rendered);
                callback(new Uint8Array([0, 0, 0, 255]));
            },
            captureNextFrame: () => state.captures.push(context.engine.rendered + 1),
            writePNG() { throw new Error("Unexpected image write"); },
            exit: code => state.exits.push(code),
        },
        BABYLON: {
            NativeEngine: Engine,
            Tools: { LoadFile: (url, onload) => onload(new ArrayBuffer(4)) },
            SceneLoader: {
                OnPluginActivatedObservable: { clear() {} },
                Load(root, filename, engine, onload) {
                    const scene = options.createScene
                        ? options.createScene(engine, state.scenes.length, math)
                        : makeScene(engine);
                    state.scenes.push(scene);
                    engine.scenes.push(scene);
                    onload(scene);
                },
            },
        },
        XMLHttpRequest: class {
            open() {}
            addEventListener(name, callback) { this.ready = callback; }
            send() {
                this.status = 200;
                this.responseText = JSON.stringify({
                    root: "",
                    tests: definitions.map(value => ({
                        sceneFolder: "synthetic/", sceneFilename: "scene.babylon",
                        referenceImage: "reference.png", ...value,
                    })),
                });
                this.ready();
            }
        },
    });
    math = vm.runInContext("Math", context);
    vm.runInContext(source, context, { filename: "validation_native.js" });
    return {
        ...state,
        engine: context.engine,
        tick() {
            if (context.engine.loop) {
                context.engine.loop();
            }
        },
        flushTimers() {
            while (state.timers.length) {
                state.timers.shift()();
            }
        },
    };
}

test("already-ready scenes keep their exact render count", () => {
    const runner = createRunner({ renderCount: 3 });
    runner.tick();
    runner.tick();
    assert.deepEqual(runner.reads, []);
    runner.tick();
    runner.flushTimers();
    assert.equal(runner.scenes[0].rendered, 3);
    assert.equal(runner.scenes[0].renderId, 0);
    assert.deepEqual(runner.reads, [3]);
    assert.deepEqual(runner.exits, [0]);
});

test("GUI images, dirty defines, and pending effects do not consume rendered frames", () => {
    let guiReady = false;
    let dirty = true;
    let effectReady = false;
    const runner = createRunner({
        renderCount: 2,
        createScene(engine) {
            const scene = makeScene(engine);
            scene.textures.push({ guiIsReady: () => guiReady });
            scene.meshes.push({
                isEnabled: () => true,
                subMeshes: [{
                    get materialDefines() { return { isDirty: dirty }; },
                    effect: { isReady: () => effectReady },
                }],
            });
            return scene;
        },
    });
    runner.tick();
    guiReady = true;
    runner.tick();
    dirty = false;
    runner.tick();
    assert.equal(runner.scenes[0].rendered, 0);
    assert.equal(runner.scenes[0].renderId, 3);
    assert.equal(runner.engine.currentRenderPassId, 91);
    effectReady = true;
    runner.tick();
    runner.tick();
    runner.flushTimers();
    assert.deepEqual(runner.reads, [2]);
    assert.deepEqual(runner.exits, [0]);
});

test("scene readiness is rechecked after executeWhenReady", () => {
    const runner = createRunner();
    runner.scenes[0].ready = false;
    runner.tick();
    assert.equal(runner.scenes[0].rendered, 0);
    runner.scenes[0].ready = true;
    runner.tick();
    runner.flushTimers();
    assert.deepEqual(runner.reads, [1]);
    assert.deepEqual(runner.exits, [0]);
});

for (const cameraPass of [7, null]) {
    test(`material inspection selects and restores the render pass (camera=${cameraPass})`, () => {
        const inspected = [];
        const runner = createRunner({
            createScene(engine) {
                const scene = makeScene(engine);
                if (cameraPass === null) {
                    scene.activeCamera = null;
                }
                scene.meshes.push({
                    isEnabled: () => true,
                    subMeshes: [{
                        get materialDefines() {
                            inspected.push(engine.currentRenderPassId);
                            return { isDirty: engine.currentRenderPassId !== (cameraPass || 91) };
                        },
                    }],
                });
                return scene;
            },
        });
        runner.tick();
        runner.flushTimers();
        assert.deepEqual(inspected, [cameraPass || 91]);
        assert.equal(runner.engine.currentRenderPassId, 91);
        assert.deepEqual(runner.exits, [0]);
    });
}

test("readiness exceptions restore the render pass and stop stale callbacks", () => {
    const runner = createRunner({
        createScene(engine) {
            const scene = makeScene(engine);
            scene.meshes.push({
                isEnabled: () => true,
                subMeshes: [{
                    get materialDefines() { throw new Error("readiness failure"); },
                }],
            });
            return scene;
        },
    });
    runner.tick();
    runner.flushTimers();
    runner.callbacks[0]();
    assert.equal(runner.engine.currentRenderPassId, 91);
    assert.equal(runner.errors.length, 1);
    assert.match(runner.errors[0], /readiness failure/);
    assert.deepEqual(runner.exits, [-1]);
    assert.deepEqual(runner.reads, []);
});

test("only associated utility scenes participate in convergence", () => {
    let guiReady = false;
    let utility;
    let unrelated;
    const runner = createRunner({
        createScene(engine) {
            const scene = makeScene(engine);
            utility = makeScene(engine);
            utility.activeCamera = scene.activeCamera;
            utility.textures.push({ guiIsReady: () => guiReady });
            unrelated = makeScene(engine);
            unrelated.ready = false;
            engine._virtualScenes.push(scene, utility, unrelated);
            return scene;
        },
    });
    runner.tick();
    assert.equal(runner.scenes[0].rendered, 0);
    assert.equal(runner.scenes[0].renderId, 1);
    assert.equal(utility.renderId, 1);
    assert.equal(unrelated.renderId, 0);
    guiReady = true;
    runner.tick();
    runner.flushTimers();
    assert.deepEqual(runner.exits, [0]);
});

test("convergence expires after 240 waiting ticks and the suite continues once", () => {
    const runner = createRunner({
        tests: [{ title: "never ready" }, { title: "ready" }],
        createScene(engine, index) {
            const scene = makeScene(engine);
            if (index === 0) {
                scene.textures.push({ guiIsReady: () => false });
            }
            return scene;
        },
    });
    for (let index = 0; index < 240; ++index) {
        runner.tick();
    }
    assert.equal(runner.scenes[0].rendered, 0);
    assert.equal(runner.scenes[0].renderId, 240);
    assert.equal(runner.errors.length, 0);
    runner.tick();
    runner.flushTimers();
    runner.callbacks[0]();
    assert.equal(runner.scenes[0].disposed, 1);
    assert.equal(runner.errors.length, 1);
    assert.match(runner.errors[0], /did not converge within 240/);
    assert.equal(runner.scenes.length, 2);
    runner.tick();
    runner.flushTimers();
    assert.deepEqual(runner.reads, [1]);
    assert.deepEqual(runner.exits, [-1]);
    assert.ok(runner.logs.some(line => /ran=2 passed=1 failed=1/.test(line)));
});

test("each test restores both the seeded random function and its sequence", () => {
    const sequences = [];
    const functions = [];
    const runner = createRunner({
        tests: [{ title: "replaces Math.random" }, { title: "next test" }],
        createScene(engine, index, math) {
            functions.push(math.random);
            sequences.push([math.random(), math.random()]);
            if (index === 0) {
                math.random = () => 0.123456;
            }
            return makeScene(engine);
        },
    });
    runner.tick();
    runner.flushTimers();
    runner.tick();
    runner.flushTimers();
    assert.deepEqual(sequences[1], sequences[0]);
    assert.equal(functions[1], functions[0]);
    assert.deepEqual(runner.exits, [0]);
});

test("convergence does not shift screenshot or RenderDoc capture frames", () => {
    let ready = false;
    const runner = createRunner({
        renderCount: 2,
        captureFrame: 5,
        createScene(engine) {
            const scene = makeScene(engine);
            scene.textures.push({ guiIsReady: () => ready });
            return scene;
        },
    });
    runner.tick();
    runner.tick();
    assert.equal(runner.scenes[0].rendered, 0);
    ready = true;
    for (let index = 0; index < 10; ++index) {
        runner.tick();
    }
    runner.flushTimers();
    assert.deepEqual(runner.reads, [2]);
    assert.deepEqual(runner.captures, [5]);
    assert.equal(runner.scenes[0].rendered, 10);
    assert.deepEqual(runner.exits, [0]);
});

test("an initial readiness timeout prevents a late callback from starting rendering", () => {
    const runner = createRunner({
        createScene(engine) {
            const scene = makeScene(engine);
            scene.deferReady = true;
            return scene;
        },
    });
    runner.scenes[0].readyTimeout();
    assert.doesNotThrow(() => runner.scenes[0].readyCallback());
    assert.equal(runner.callbacks.length, 0);
    assert.equal(runner.scenes[0].disposed, 1);
    assert.deepEqual(runner.exits, [-1]);
});
