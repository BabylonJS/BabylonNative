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
            this.isDisposed = true;
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
    const state = { scenes: [], callbacks: [], timers: new Map(), errors: [], logs: [], exits: [], reads: [], readbacks: [], captures: [], sceneLoads: [] };
    let nextTimerId = 0;
    let now = 0;
    let math;
    class UtilityLayerRenderer {
        constructor(originalScene) {
            this.originalScene = originalScene;
            this.utilityLayerScene = makeScene(originalScene.getEngine());
            originalScene.getEngine()._virtualScenes.push(this.utilityLayerScene);
            this._updateCamera();
        }
        _updateCamera() {
            this.utilityLayerScene.activeCamera = this.originalScene.activeCamera;
        }
    }
    const definitions = options.tests || [{ title: "test", renderCount: options.renderCount || 1 }];
    function createScene(engine) {
        const scene = options.createScene
            ? options.createScene(engine, state.scenes.length, math, UtilityLayerRenderer)
            : makeScene(engine);
        state.scenes.push(scene);
        engine.scenes.push(scene);
        return scene;
    }
    class Engine {
        constructor() {
            this.scenes = [];
            this._virtualScenes = [];
            this.currentRenderPassId = 91;
            this.rendered = 0;
            this.effectObservers = new Set();
            this.onEffectErrorObservable = {
                add: callback => { this.effectObservers.add(callback); return callback; },
                remove: callback => this.effectObservers.delete(callback),
            };
        }
        emitEffectError({ terminal = true, ready = false, disposed = false } = {}) {
            for (const callback of [...this.effectObservers]) {
                callback({
                    effect: { allFallbacksProcessed: () => terminal, isReady: () => ready, isDisposed: disposed },
                    errors: "test shader compilation error",
                });
            }
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
        setTimeout(callback, delay = 0) {
            const id = ++nextTimerId;
            state.timers.set(id, { callback, due: now + delay });
            return id;
        },
        clearTimeout: id => state.timers.delete(id),
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
                if (options.deferReadback) {
                    state.readbacks.push(callback);
                } else {
                    callback(new Uint8Array([0, 0, 0, 255]));
                }
            },
            captureNextFrame: () => state.captures.push(context.engine.rendered + 1),
            writePNG() { throw new Error("Unexpected image write"); },
            exit: code => state.exits.push(code),
        },
        BABYLON: {
            NativeEngine: Engine,
            UtilityLayerRenderer,
            Tools: {
                LoadFile(url, onload, progress, database, binary, onerror) {
                    if (options.snippetLoadError && url.startsWith("https://snippet.babylonjs.com/")) {
                        onerror({ status: 404, statusText: "Not Found" });
                        return;
                    }
                    onload(url.startsWith("https://snippet.babylonjs.com/")
                        ? JSON.stringify({ jsonPayload: JSON.stringify({ code: options.playgroundCode }) })
                        : new ArrayBuffer(4));
                },
            },
            Scene: function (engine) { return createScene(engine); },
            SceneLoader: {
                OnPluginActivatedObservable: { clear() {} },
                Load(root, filename, engine, onload) {
                    if (options.deferSceneLoad) {
                        state.sceneLoads.push(() => onload(createScene(engine)));
                    } else {
                        onload(createScene(engine));
                    }
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
                        ...(value.playgroundId ? {} : { sceneFolder: "synthetic/", sceneFilename: "scene.babylon" }),
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
            while (true) {
                const next = [...state.timers].filter(([, timer]) => timer.due <= now)
                    .sort((left, right) => left[1].due - right[1].due)[0];
                if (!next) {
                    return;
                }
                state.timers.delete(next[0]);
                next[1].callback();
            }
        },
        advanceTimers(milliseconds) {
            now += milliseconds;
            this.flushTimers();
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

for (const synchronous of [false, true]) {
    test(`terminal shader errors reject pending creation and cancel its timer (synchronous=${synchronous})`, async () => {
        const runner = createRunner({
            tests: [{ title: "failed shader", playgroundId: "#TEST#0" }],
            playgroundCode: `function createScene(engine) {
                new BABYLON.Scene(engine);
                ${synchronous ? "engine.emitEffectError();" : ""}
                return new Promise(() => {});
            }`,
        });
        runner.flushTimers();
        if (!synchronous) {
            runner.engine.emitEffectError();
            assert.equal(runner.scenes[0].disposed, 0, "must not dispose inside the compiler notification");
            runner.flushTimers();
        }
        await new Promise(setImmediate);
        runner.flushTimers();
        assert.deepEqual(runner.exits, [-1]);
        assert.equal(runner.scenes[0].disposed, 1);
        assert.equal(runner.engine.effectObservers.size, 0);
        assert.equal(runner.timers.size, 0);
        assert.ok(runner.errors.some(value => value.includes("test shader compilation error")));
        assert.deepEqual(runner.reads, []);
    });
}

test("shader fallbacks, retained ready pipelines, and disposed effects do not fail validation", () => {
    const runner = createRunner();
    runner.engine.emitEffectError({ terminal: false });
    runner.engine.emitEffectError({ ready: true });
    runner.engine.emitEffectError({ disposed: true });
    runner.flushTimers();
    assert.deepEqual(runner.errors, []);
    assert.deepEqual(runner.exits, []);
    runner.tick();
    runner.flushTimers();
    assert.deepEqual(runner.exits, [0]);
    assert.equal(runner.engine.effectObservers.size, 0);
    assert.equal(runner.timers.size, 0);
});

for (const phase of ["initial readiness", "convergence", "screenshot"]) {
    test(`shader failures cancel ${phase} callbacks and continue exactly once`, () => {
        const runner = createRunner({
            tests: [{ title: "failed shader" }, { title: "next scene" }],
            deferReadback: true,
            createScene(engine, index) {
                const scene = makeScene(engine);
                if (index === 0) {
                    scene.deferReady = phase === "initial readiness";
                    scene.ready = phase !== "convergence";
                }
                return scene;
            },
        });
        const scene = runner.scenes[0];
        const renderCallback = runner.engine.loop;
        runner.tick();
        runner.engine.emitEffectError();
        runner.engine.emitEffectError();
        assert.equal(scene.disposed, 0);
        runner.flushTimers();
        assert.equal(scene.disposed, 1);
        assert.equal(runner.scenes.length, 2);
        const logs = runner.logs.slice();
        const errors = runner.errors.slice();
        scene.readyCallback();
        scene.readyTimeout();
        if (renderCallback) {
            renderCallback();
        }
        if (phase === "screenshot") {
            runner.readbacks[0](new Uint8Array([255, 0, 0, 255]));
        }
        assert.deepEqual(runner.logs, logs);
        assert.deepEqual(runner.errors, errors);
        runner.tick();
        runner.readbacks[runner.readbacks.length - 1](new Uint8Array([0, 0, 0, 255]));
        runner.flushTimers();
        assert.deepEqual(runner.exits, [-1]);
        assert.ok(runner.logs.some(line => /ran=2 passed=1 failed=1/.test(line)));
        assert.equal(runner.engine.effectObservers.size, 0);
        assert.equal(runner.timers.size, 0);
    });
}

test("a shader failure cannot become a pass when a screenshot completes before the deferred failure", () => {
    const runner = createRunner({ deferReadback: true });
    runner.tick();
    runner.engine.emitEffectError();
    runner.readbacks[0](new Uint8Array([0, 0, 0, 255]));
    runner.flushTimers();
    assert.deepEqual(runner.exits, [-1]);
    assert.equal(runner.engine.effectObservers.size, 0);
    assert.equal(runner.timers.size, 0);
});

test("a scene promise resolving after shader failure cannot replace the next scene", async () => {
    const runner = createRunner({
        tests: [{ title: "failed shader", playgroundId: "#TEST#0" }, { title: "next scene" }],
        playgroundCode: `function createScene(engine) {
            return new Promise(resolve => { engine.resolveLateScene = resolve; });
        }`,
    });
    runner.flushTimers();
    runner.engine.emitEffectError();
    runner.flushTimers();
    await new Promise(setImmediate);
    runner.flushTimers();
    const lateScene = makeScene(runner.engine);
    runner.engine.resolveLateScene(lateScene);
    await new Promise(setImmediate);
    assert.equal(lateScene.disposed, 1);
    runner.tick();
    runner.flushTimers();
    assert.equal(runner.scenes[0].rendered, 1);
    assert.ok(runner.logs.some(line => /ran=2 passed=1 failed=1/.test(line)));
    assert.deepEqual(runner.exits, [-1]);
    assert.equal(runner.timers.size, 0);
});

test("failed snippet loading removes its effect observer", () => {
    const runner = createRunner({
        tests: [{ title: "missing snippet", playgroundId: "#TEST#0" }],
        snippetLoadError: true,
    });
    runner.flushTimers();
    runner.engine.emitEffectError();
    assert.equal(runner.engine.effectObservers.size, 0);
    assert.equal(runner.errors.length, 1);
    assert.deepEqual(runner.exits, [-1]);
});

test("a scene-file load delivered after shader failure is disposed without rendering", () => {
    const runner = createRunner({ deferSceneLoad: true });
    runner.engine.emitEffectError();
    runner.flushTimers();
    runner.sceneLoads[0]();
    assert.equal(runner.scenes[0].disposed, 1);
    assert.equal(runner.scenes[0].rendered, 0);
    assert.deepEqual(runner.exits, [-1]);
    assert.equal(runner.engine.effectObservers.size, 0);
    assert.equal(runner.timers.size, 0);
});

for (const deferred of [false, true]) {
    test(`playground scene promises cancel their timeout (deferred=${deferred})`, async () => {
        const runner = createRunner({
            tests: [{ title: "promised scene", playgroundId: "#TEST#0", renderCount: 2 }],
            playgroundCode: `function createScene(engine) {
                const scene = new BABYLON.Scene(engine);
                return ${deferred ? "new Promise(resolve => setTimeout(() => resolve(scene), 50))" : "Promise.resolve(scene)"};
            }`,
        });
        runner.flushTimers();
        if (deferred) {
            runner.advanceTimers(49);
            await new Promise(resolve => setImmediate(resolve));
            assert.equal(runner.callbacks.length, 0);
            runner.advanceTimers(1);
        }
        await new Promise(resolve => setImmediate(resolve));
        assert.deepEqual(runner.errors, []);
        assert.equal(runner.callbacks.length, 1);
        assert.equal(runner.timers.size, 0);
        runner.tick();
        runner.tick();
        runner.flushTimers();
        assert.deepEqual(runner.reads, [2]);
        assert.deepEqual(runner.exits, [0]);
        runner.advanceTimers(10 * 60 * 1000);
        await new Promise(resolve => setImmediate(resolve));
        assert.deepEqual(runner.errors, []);
        assert.deepEqual(runner.exits, [0]);
    });
}

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

for (const { name, hasCamera = true, cameraPass, expectedPass } of [
    { name: "active camera", cameraPass: 7, expectedPass: 7 },
    { name: "main render pass", cameraPass: 0, expectedPass: 0 },
    { name: "no active camera", hasCamera: false, expectedPass: 91 },
    { name: "undefined camera pass", cameraPass: undefined, expectedPass: 91 },
    { name: "null camera pass", cameraPass: null, expectedPass: 91 },
]) {
    test(`material inspection selects and restores the render pass (${name})`, () => {
        const inspected = [];
        const runner = createRunner({
            createScene(engine) {
                const scene = makeScene(engine);
                if (!hasCamera) {
                    scene.activeCamera = null;
                } else {
                    scene.activeCamera.renderPassId = cameraPass;
                }
                scene.meshes.push({
                    isEnabled: () => true,
                    subMeshes: [{
                        get materialDefines() {
                            inspected.push(engine.currentRenderPassId);
                            return { isDirty: engine.currentRenderPassId !== expectedPass };
                        },
                    }],
                });
                return scene;
            },
        });
        runner.tick();
        runner.flushTimers();
        assert.deepEqual(inspected, [expectedPass]);
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

for (const hook of ["scene", "GUI"]) {
    test(`${hook} readiness exceptions restore the render pass before cleanup`, () => {
        const runner = createRunner({
            createScene(engine) {
                const scene = makeScene(engine);
                const throwFromReadiness = () => {
                    engine.currentRenderPassId = scene.activeCamera.renderPassId;
                    throw new Error(`${hook} readiness failure`);
                };
                if (hook === "scene") {
                    scene.isReady = throwFromReadiness;
                } else {
                    scene.textures.push({ guiIsReady: throwFromReadiness });
                }
                return scene;
            },
        });
        runner.tick();
        assert.equal(runner.engine.currentRenderPassId, 91);
        runner.flushTimers();
        runner.callbacks[0]();
        assert.equal(runner.errors.length, 1);
        assert.match(runner.errors[0], /readiness failure/);
        assert.equal(runner.scenes[0].disposed, 1);
        assert.deepEqual(runner.exits, [-1]);
        assert.deepEqual(runner.reads, []);
    });
}

for (const failure of ["render", "readiness", "convergence"]) {
    test(`a pending screenshot cannot evaluate after ${failure} failure`, () => {
        const runner = createRunner({
            tests: [{ title: "fails after compare frame" }, { title: "next scene" }],
            captureFrame: 3,
            deferReadback: true,
        });
        runner.tick();
        assert.equal(runner.readbacks.length, 1);
        const scene = runner.scenes[0];
        if (failure === "convergence") {
            scene.ready = false;
            for (let index = 0; index <= 240; ++index) {
                runner.tick();
            }
        } else {
            scene[failure === "render" ? "render" : "isReady"] = () => {
                throw new Error(`${failure} failure`);
            };
            runner.tick();
        }
        runner.flushTimers();
        assert.equal(scene.disposed, 1);
        assert.equal(runner.scenes.length, 2);
        assert.equal(runner.errors.length, 1);
        const logsBeforeReadback = runner.logs.slice();
        assert.doesNotThrow(() => runner.readbacks[0](new Uint8Array([255, 0, 0, 255])));
        assert.deepEqual(runner.logs, logsBeforeReadback);
        for (let index = 0; index < 8; ++index) {
            runner.tick();
        }
        runner.readbacks[1](new Uint8Array([0, 0, 0, 255]));
        runner.flushTimers();
        assert.deepEqual(runner.exits, [-1]);
        assert.ok(runner.logs.some(line => /ran=2 passed=1 failed=1/.test(line)));
    });
}

test("a stale initial readiness timeout cannot invalidate a pending screenshot", () => {
    const runner = createRunner({ deferReadback: true, captureFrame: 3 });
    runner.tick();
    assert.equal(runner.readbacks.length, 1);
    runner.scenes[0].readyTimeout();
    assert.deepEqual(runner.errors, []);
    for (let index = 0; index < 7; ++index) {
        runner.tick();
    }
    assert.equal(runner.engine.loop, null);
    runner.readbacks[0](new Uint8Array([0, 0, 0, 255]));
    runner.flushTimers();
    assert.deepEqual(runner.exits, [0]);
});

test("a delayed screenshot still evaluates after normal rendering completion", () => {
    const runner = createRunner({ deferReadback: true, captureFrame: 3 });
    for (let index = 0; index < 8; ++index) {
        runner.tick();
    }
    assert.equal(runner.engine.loop, null);
    assert.equal(runner.scenes[0].disposed, 0);
    runner.readbacks[0](new Uint8Array([0, 0, 0, 255]));
    runner.flushTimers();
    assert.deepEqual(runner.reads, [1]);
    assert.deepEqual(runner.captures, [3]);
    assert.equal(runner.scenes[0].rendered, 8);
    assert.equal(runner.scenes[0].disposed, 1);
    assert.deepEqual(runner.exits, [0]);
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

test("a utility layer created before the main camera participates in readiness", () => {
    let utility;
    let unrelated;
    const runner = createRunner({
        createScene(engine, index, math, UtilityLayerRenderer) {
            const scene = makeScene(engine);
            scene.activeCamera = null;
            utility = new UtilityLayerRenderer(scene).utilityLayerScene;
            utility.deferReady = true;
            unrelated = makeScene(engine);
            unrelated.activeCamera = null;
            unrelated.deferReady = true;
            engine._virtualScenes.push(unrelated);
            scene.activeCamera = { renderPassId: 7, getScene: () => scene };
            return scene;
        },
    });
    assert.equal(runner.callbacks.length, 0);
    assert.equal(typeof utility.readyCallback, "function");
    assert.equal(unrelated.readyCallback, undefined);
    utility.readyCallback();
    runner.tick();
    runner.flushTimers();
    assert.deepEqual(runner.reads, [1]);
    assert.deepEqual(runner.exits, [0]);
});

test("utility scenes attached during convergence participate in later ticks", () => {
    let mainReady = false;
    let utilityReady = false;
    let utility;
    const runner = createRunner({
        createScene(engine) {
            const scene = makeScene(engine);
            scene.textures.push({ guiIsReady: () => mainReady });
            return scene;
        },
    });
    const main = runner.scenes[0];
    runner.tick();
    utility = makeScene(runner.engine);
    utility.activeCamera = main.activeCamera;
    utility.textures.push({ guiIsReady: () => utilityReady });
    runner.engine._virtualScenes.push(utility);
    mainReady = true;
    runner.tick();
    assert.equal(main.rendered, 0);
    assert.equal(main.renderId, 2);
    assert.equal(utility.renderId, 1);
    utilityReady = true;
    runner.tick();
    runner.flushTimers();
    assert.deepEqual(runner.reads, [1]);
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

for (const firstReady of ["main", "utility"]) {
    test(`initial readiness waits for main and utility resources (${firstReady} first)`, () => {
        let utility;
        const runner = createRunner({
            createScene(engine) {
                const scene = makeScene(engine);
                scene.deferReady = true;
                utility = makeScene(engine);
                utility.deferReady = true;
                utility.activeCamera = scene.activeCamera;
                engine._virtualScenes.push(utility);
                return scene;
            },
        });
        const main = runner.scenes[0];
        const first = firstReady === "main" ? main : utility;
        const second = firstReady === "main" ? utility : main;
        assert.equal(runner.callbacks.length, 0);
        first.readyCallback();
        runner.tick();
        assert.equal(runner.callbacks.length, 0);
        assert.equal(main.rendered, 0);
        assert.equal(main.renderId, 0);
        assert.equal(utility.renderId, 0);
        second.readyCallback();
        runner.tick();
        runner.flushTimers();
        assert.equal(runner.callbacks.length, 1);
        assert.equal(main.rendered, 1);
        assert.deepEqual(runner.reads, [1]);
        assert.deepEqual(runner.exits, [0]);
    });
}

for (const removal of ["disposed", "detached"]) {
    test(`initial readiness drops a ${removal} utility scene`, () => {
        let utility;
        const runner = createRunner({
            createScene(engine) {
                const scene = makeScene(engine);
                utility = makeScene(engine);
                utility.deferReady = true;
                utility.activeCamera = scene.activeCamera;
                engine._virtualScenes.push(utility);
                return scene;
            },
        });
        const staleReadyCallback = utility.readyCallback;
        runner.engine._virtualScenes = runner.engine._virtualScenes.filter(scene => scene !== utility);
        if (removal === "disposed") {
            utility.disposed++;
            utility.readyCallback = null;
            utility.readyTimeout = null;
        }
        runner.advanceTimers(100);
        assert.equal(runner.callbacks.length, 1);
        runner.tick();
        runner.flushTimers();
        assert.deepEqual(runner.reads, [1]);
        assert.deepEqual(runner.exits, [0]);
        assert.doesNotThrow(() => staleReadyCallback());
        assert.equal(runner.callbacks.length, 1);
        assert.deepEqual(runner.exits, [0]);
    });
}

test("initial readiness enrolls a utility scene attached while waiting", () => {
    const runner = createRunner({
        createScene(engine) {
            const scene = makeScene(engine);
            scene.deferReady = true;
            return scene;
        },
    });
    const utility = makeScene(runner.engine);
    utility.deferReady = true;
    utility.activeCamera = runner.scenes[0].activeCamera;
    runner.engine._virtualScenes.push(utility);
    assert.equal(utility.readyCallback, undefined);
    runner.advanceTimers(100);
    assert.equal(typeof utility.readyCallback, "function");
    runner.scenes[0].readyCallback();
    assert.equal(runner.callbacks.length, 0);
    utility.readyCallback();
    assert.equal(runner.callbacks.length, 1);
    runner.tick();
    runner.flushTimers();
    assert.deepEqual(runner.reads, [1]);
    assert.deepEqual(runner.exits, [0]);
});

test("the runner owns the initial readiness timeout", () => {
    const runner = createRunner({
        createScene(engine) {
            const scene = makeScene(engine);
            scene.deferReady = true;
            return scene;
        },
    });
    runner.scenes[0].readyCallback = null;
    runner.scenes[0].readyTimeout = null;
    runner.advanceTimers(10 * 60 * 1000);
    assert.equal(runner.errors.length, 1);
    assert.match(runner.errors[0], /did not become ready within 600s/);
    assert.equal(runner.scenes[0].disposed, 1);
    assert.deepEqual(runner.exits, [-1]);
});

test("an initial utility readiness timeout stops all late callbacks", () => {
    let utility;
    const runner = createRunner({
        createScene(engine) {
            const scene = makeScene(engine);
            scene.deferReady = true;
            utility = makeScene(engine);
            utility.deferReady = true;
            utility.activeCamera = scene.activeCamera;
            engine._virtualScenes.push(utility);
            return scene;
        },
    });
    assert.equal(utility.onReadyTimeoutDuration, 10 * 60 * 1000);
    utility.readyTimeout();
    runner.scenes[0].readyCallback();
    utility.readyCallback();
    assert.equal(runner.callbacks.length, 0);
    assert.equal(runner.scenes[0].disposed, 1);
    assert.equal(runner.errors.length, 1);
    assert.deepEqual(runner.reads, []);
    assert.deepEqual(runner.exits, [-1]);
});
