const fs = require("node:fs");
const path = require("node:path");
const { spawn, spawnSync } = require("node:child_process");
const { test } = require("node:test");
const assert = require("node:assert/strict");
const { PNG } = require("pngjs");
const { main } = require("../run.cjs");
const { createTransport } = require("../transport.cjs");
const { sessionFactory } = require("./fixtures/session.cjs");

if (!process.env.BABYLON_JS_ROOT) {
    throw new Error("BABYLON_JS_ROOT is required for the shared Playwright integration controls");
}
const root = path.resolve(process.env.BABYLON_JS_ROOT);
const cli = require.resolve("@playwright/test/cli", { paths: [root] });

test("the normal runner keeps devices through actual shared-suite assertions and worker replacement", async t => {
    const directory = fs.mkdtempSync(path.join(__dirname, "shared-suite-results-"));
    t.after(() => fs.rmSync(directory, { recursive: true }));
    const events = path.join(directory, "sessions.jsonl");
    const marker = path.join(directory, "first-failure");
    const config = path.join(directory, "playwright.config.cjs");
    const catalog = path.join(directory, "catalog.json");
    fs.writeFileSync(config, `
        const config = require(${JSON.stringify(path.resolve(__dirname, "..", "playwright.config.cjs"))});
        module.exports = {
            ...config,
            snapshotPathTemplate: ${JSON.stringify(path.join(directory, "{arg}{ext}"))},
            reporter: [["json", {outputFile: ${JSON.stringify(path.join(directory, "report.json"))}}]]
        };
    `);
    fs.writeFileSync(catalog, JSON.stringify({ root: "https://fixture.invalid/", tests: [
        { title: "Native transport passing scene", scriptToRun: "pass", functionToCall: "createScene", referenceImage: "fixture.png", renderCount: 3 },
        { title: "Native transport retry scene", scriptToRun: "retry", functionToCall: "createScene", referenceImage: "fixture.png", renderCount: 2 },
        { title: "Native transport scaled scene", scriptToRun: "scaled", functionToCall: "createScene", referenceImage: "scaled.png", renderCount: 2 },
        { title: "Native transport after scale", scriptToRun: "pass", functionToCall: "createScene", referenceImage: "fixture.png", renderCount: 2 }
    ] }));
    const data = Buffer.alloc(600 * 400 * 4);
    for (let i = 0; i < data.length; i += 4) {
        data.set([1, 2, 3, 255], i);
    }
    fs.writeFileSync(path.join(directory, "fixture.png"), PNG.sync.write({ width: 600, height: 400, data }));
    fs.writeFileSync(path.join(directory, "scaled.png"), PNG.sync.write({ width: 400, height: 266, data: data.subarray(0, 400 * 266 * 4) }));
    const env = {
        ...process.env, MOBILE_PLATFORM: "android", MOBILE_OUTPUT: path.join(directory, "output"),
        CONFIG_PATH: catalog, CDN_BASE_URL: "https://fixture.invalid/", TIMEOUT: "10000"
    };
    const original = Object.fromEntries(Object.keys(env).map(key => [key, process.env[key]]));
    Object.assign(process.env, env);
    t.after(() => {
        for (const [key, value] of Object.entries(original)) {
            if (value === undefined) { delete process.env[key]; }
            else { process.env[key] = value; }
        }
    });
    async function run(args, expectedCode = 0) {
        const session = sessionFactory({ events, retryMarker: marker });
        const startupEvidence = path.join(env.MOBILE_OUTPUT, "startup-evidence.json");
        let log = "";
        const code = await main(args, {
            auth: { redact: text => text },
            uploadApp: async () => ({ appUrl: "bs://fixture", sha256: "fixture", bytes: 1, uploadMs: 0 }),
            createTransport: async (onConsole, signal, options) => {
                const transport = await createTransport(onConsole, signal, {
                    ...options, sessionFactory: session.factory, auth: { redact: text => text }
                });
                fs.writeFileSync(startupEvidence, "Created before Playwright startup");
                return transport;
            },
            spawn: (command, args, options) => {
                args[3] = config;
                const child = spawn(command, args, { ...options, stdio: ["ignore", "pipe", "pipe"] });
                child.stdout.on("data", bytes => { log += bytes; });
                child.stderr.on("data", bytes => { log += bytes; });
                return child;
            }
        });
        assert.equal(code, expectedCode, log);
        assert.equal(fs.readFileSync(startupEvidence, "utf8"), "Created before Playwright startup");
        assert.equal(JSON.parse(fs.readFileSync(path.join(env.MOBILE_OUTPUT, "app-upload.json"))).sha256, "fixture");
        return JSON.parse(fs.readFileSync(path.join(directory, "report.json"), "utf8"));
    }
    const selected = await run(["--grep", "passing scene", "--workers", "1"]);
    assert.equal(selected.stats.expected, 1);
    assert.equal(selected.stats.unexpected, 0);
    let lifecycle = fs.readFileSync(events, "utf8").trim().split("\n").map(line => JSON.parse(line));
    assert.deepEqual(lifecycle.map(item => item.phase), ["start", "stop"]);
    fs.writeFileSync(events, "");
    const scaled = await run(["--grep", "scaled scene|after scale", "--workers", "1"]);
    assert.equal(scaled.stats.expected, 2);
    assert.equal(scaled.stats.unexpected, 0);
    lifecycle = fs.readFileSync(events, "utf8").trim().split("\n").map(line => JSON.parse(line));
    assert.deepEqual(lifecycle.map(item => item.phase), ["start", "stop"]);
    fs.writeFileSync(events, "");
    const all = await run(["--workers", "1", "--retries", "1"]);
    assert.equal(all.stats.expected, 3);
    assert.equal(all.stats.flaky, 1);
    assert.equal(all.stats.unexpected, 0);
    const specs = all.suites.flatMap(suite => suite.specs);
    assert.equal(specs.length, 4);
    const retry = specs.find(spec => spec.title === "Native transport retry scene").tests[0];
    assert.deepEqual(retry.results.map(result => result.status), ["failed", "passed"]);
    assert.notEqual(retry.results[0].workerIndex, retry.results[1].workerIndex);
    assert.ok(retry.results[0].errors.some(error => /Screenshot comparison failed|pixels.*different/.test(error.message)));
    lifecycle = fs.readFileSync(events, "utf8").trim().split("\n").map(line => JSON.parse(line));
    const started = lifecycle.filter(item => item.phase === "start").map(item => item.id).sort();
    const stopped = lifecycle.filter(item => item.phase === "stop").map(item => item.id).sort();
    assert.equal(started.length, 1);
    assert.deepEqual(stopped, started);
    const owner = JSON.parse(fs.readFileSync(path.join(env.MOBILE_OUTPUT, "run.json")));
    assert.equal(owner.devices.length, 1);
    assert.equal(owner.devices[0].history.filter(event => event.event === "acquire").length, 2);
    fs.writeFileSync(events, "");
    const parallel = await run(["--workers", "2"]);
    assert.equal(parallel.stats.expected, 4);
    lifecycle = fs.readFileSync(events, "utf8").trim().split("\n").map(line => JSON.parse(line));
    assert.equal(lifecycle.filter(item => item.phase === "start").length, 2);
    assert.equal(lifecycle.filter(item => item.phase === "stop").length, 2);
    const unsupported = JSON.parse(fs.readFileSync(catalog));
    unsupported.tests.push({
        title: "Native transport CSS background", scriptToRun: "pass", functionToCall: "createScene",
        referenceImage: "fixture.png", renderCount: 2, canvasBackgroundColor: "white"
    });
    fs.writeFileSync(catalog, JSON.stringify(unsupported));
    const background = await run(["--grep", "CSS background", "--workers", "1"], 1);
    assert.equal(background.stats.unexpected, 1);
    assert.match(background.suites[0].specs[0].tests[0].results[0].errors[0].message,
        /Native framebuffer screenshots do not support CSS canvasBackgroundColor/);
});

test("the mobile entry point refuses replacement projects, configs and reference updates", () => {
    for (const args of [["--update-snapshots"], ["-u"], ["--project=webgl2"], ["--config", "other.cjs"], ["-c", "other.cjs"]]) {
        const result = spawnSync(process.execPath, [path.resolve(__dirname, "..", "run.cjs"), ...args], { encoding: "utf8" });
        assert.equal(result.status, 1);
        assert.match(result.stderr, /unchanged snapshots are fixed/);
    }
});
