const fs = require("node:fs");
const path = require("node:path");
const vm = require("node:vm");
const { randomUUID } = require("node:crypto");

function sessionFactory(options = {}) {
    const state = { stopped: 0, diagnostics: 0, diagnosticRequests: [] };
    const factory = async ({ controller, signal }) => {
        signal.throwIfAborted();
        if (options.startupError) {
            throw new Error(options.startupError);
        }
        const cancellation = new AbortController();
        const id = randomUUID();
        const event = phase => {
            if (options.events) {
                fs.appendFileSync(options.events, JSON.stringify({ phase, id, worker: process.env.TEST_WORKER_INDEX }) + "\n");
            }
        };
        const context = vm.createContext({
            performance, TextEncoder, TextDecoder,
            setTimeout, clearTimeout, setImmediate, clearImmediate,
            __mobilePlaywrightUrl: `http://127.0.0.1:${controller.port}${controller.devicePath}`,
            console: { log() {}, warn() {}, error() {} },
            fetch: async (url, config = {}) => {
                if (url.startsWith("http://127.0.0.1:")) {
                    return fetch(url, { ...config, signal: cancellation.signal });
                }
                const title = url.split("/").at(-1);
                return new Response(`function createScene() { return __fixtureScene(${JSON.stringify(title)}); }`);
            }
        });
        vm.runInContext(`
            class Engine {
                static scaling = 1;
                getRenderWidth() { return Math.floor(600 / Engine.scaling); }
                getRenderHeight() { return Math.floor(400 / Engine.scaling); }
                setHardwareScalingLevel(value) { Engine.scaling = value; }
                getCaps() { return {}; }
                dispose() { this.stopRenderLoop(); }
                runRenderLoop(callback) {
                    this.running = true;
                    const tick = () => {
                        if (!this.running) { return; }
                        callback();
                        if (this.running) { this.next = setImmediate(tick); }
                    };
                    this.next = setImmediate(tick);
                }
                stopRenderLoop() { this.running = false; clearImmediate(this.next); }
            }
            globalThis._native = {Engine};
            globalThis.BABYLON = {
                Engine: {Version: ${JSON.stringify(options.version || require(path.resolve(__dirname, "..", "..", "..", "..", "..", "package-lock.json")).packages["node_modules/babylonjs"].version)}},
                NativeEngine: Engine, Tools: {}, SceneLoader: {OnPluginActivatedObservable: {clear() {}}},
                DracoDecoder: {}, MeshoptCompression: {Configuration: {}}, GLTFValidation: {},
                KhronosTextureContainer2: {}, BasisToolsOptions: {}, NodeMaterial: {}
            };
            globalThis.__fixtureScene = title => {
                if (title === "scaled") { engine.setHardwareScalingLevel(1.5); }
                return {
                    title, dispose() {}, render() {}, isReady() { return true; },
                    executeWhenReady(callback) { callback(); }
                };
            };
        `, context);
        context.TestUtils = {
            getGraphicsApiName: () => options.api || "OpenGL",
            getFrameBufferData: callback => {
                const fail = context.scene.title === "retry" && options.retryMarker && !fs.existsSync(options.retryMarker);
                if (fail) {
                    fs.writeFileSync(options.retryMarker, "failed first attempt");
                }
                const data = new Uint8Array(context.engine.getRenderWidth() * context.engine.getRenderHeight() * 4);
                for (let i = 0; i < data.length; i += 4) {
                    data.set(fail ? [255, 255, 255, 255] : [1, 2, 3, 255], i);
                }
                callback(data);
            }
        };
        vm.runInContext(fs.readFileSync(path.resolve(__dirname, "..", "..", "..", "..", "Scripts", "mobilePlaywright.js"), "utf8"), context);
        state.context = context;
        const ready = await controller.ready(1000);
        event("start");
        return {
            measurements: { sessionId: id, ready },
            async stop() {
                state.stopped++;
                cancellation.abort();
                context.engine?.stopRenderLoop();
                event("stop");
                if (options.cleanupError) {
                    throw new Error(options.cleanupError);
                }
            },
            async diagnostics(options) {
                state.diagnostics++;
                state.diagnosticRequests.push(options);
            }
        };
    };
    return { factory, state };
}

module.exports = { sessionFactory };
