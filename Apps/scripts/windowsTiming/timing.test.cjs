const fs = require("fs");
const path = require("path");
const crypto = require("crypto");
const { test, expect } = require("@playwright/test");
const { PNG } = require("pngjs");
const shared = require("./shared.cjs");
const workload = require("./workload.json");
const engineIsolation = "fresh NativeEngine per scene in one persistent app process per worker";

test("Time the fixed 305-scene workload with original references", async ({}, testInfo) => {
    const selected = workload.cases.filter(item => testInfo.project.name === "single" ||
        item.partition === testInfo.project.name);
    const results = {
        platform: "windows", graphicsApi: "D3D11", engine: "V8", engineIsolation,
        nativeSource: workload.nativeSource, referenceSource: workload.referenceSource,
        babylonVersion: "9.15.0", threshold: 0.035, timeoutPerSceneMs: 100_000,
        updateSnapshots: "none", worker: testInfo.project.name, selectedCount: selected.length,
        cases: [], sessions: 0, completed: false,
    };
    const filename = path.join(process.env.TIMING_ARTIFACTS, process.env.TIMING_MODE, `${testInfo.project.name}.json`);
    fs.mkdirSync(path.dirname(filename), { recursive: true });
    const started = performance.now();
    let page;
    let consoleLines = [];
    let currentStage;
    function persist() {
        results.elapsedMs = performance.now() - started;
        results.passed = results.cases.filter(item => item.status === "passed").length;
        results.failed = results.cases.length - results.passed;
        fs.writeFileSync(filename, JSON.stringify(results, null, 2));
    }
    async function open() {
        page = await shared.NativePage.launch();
        results.sessions++;
        page.on("console", message => {
            consoleLines.push({ type: message.type(), text: message.text() });
            if (consoleLines.length > 40) {
                consoleLines.shift();
            }
        });
        page.setDefaultTimeout(100_000);
        await page.setViewportSize({ width: 600, height: 400 });
        expect(await page.evaluate(() => BABYLON.Engine.Version)).toBe("9.15.0");
    }
    async function close() {
        await page?.close();
        page = undefined;
    }
    try {
        for (const item of selected) {
            if (!page) {
                await open();
            }
            const scene = shared.config.tests[item.index];
            expect(scene).toEqual(item.scene);
            consoleLines = [];
            const result = { index: item.index, title: scene.title, playgroundId: scene.playgroundId, stages: {}, status: "running" };
            const sceneStarted = performance.now();
            async function stage(name, action) {
                currentStage = name;
                const remaining = results.timeoutPerSceneMs - (performance.now() - sceneStarted);
                if (remaining <= 0) {
                    throw new Error("Scene exceeded the shared 100000ms timeout");
                }
                page.setDefaultTimeout(Math.max(1, Math.floor(remaining)));
                const begin = performance.now();
                try {
                    return await action();
                } finally {
                    result.stages[name] = performance.now() - begin;
                }
            }
            let engineFlags;
            let recover = false;
            try {
                currentStage = "reference";
                const name = (scene.referenceImage || scene.title).replace(".png", "") + ".png";
                result.referenceName = name;
                expect(path.basename(testInfo.snapshotPath(name))).toBe(item.referenceFile);
                const reference = fs.readFileSync(testInfo.snapshotPath(name));
                expect(crypto.createHash("sha256").update(reference).digest("hex")).toBe(item.referenceSha256);
                engineFlags = await stage("initialize", () => page.evaluate(shared.evaluateInitEngineForVisualization, {
                    engineName: "native", useLargeWorldRendering: scene.useLargeWorldRendering ?? false,
                    useReverseDepthBuffer: scene.useReverseDepthBuffer ?? "false",
                    useNonCompatibilityMode: scene.useNonCompatibilityMode ?? "false", baseUrl: shared.config.root,
                }));
                let preloaded;
                if (scene.playgroundId) {
                    try {
                        preloaded = await stage("fetchSnippet", () => shared.preloadSnippetCode(scene.playgroundId, "https://snippet.babylonjs.com"));
                    } catch (error) {
                        result.preloadError = String(error);
                        console.warn(`Snippet preload failed for ${scene.title}; using the shared fallback: ${error.message}`);
                    }
                }
                await stage("prepare", () => page.evaluate(shared.evaluatePrepareScene, {
                    sceneMetadata: { ...scene, snippetCode: preloaded?.code, snippetSceneCall: preloaded?.sceneCall },
                    globalConfig: { root: shared.config.root, snippetUrl: "https://snippet.babylonjs.com", pgRoot: "https://playground.babylonjs.com" },
                }));
                expect(await stage("render", () => page.evaluate(shared.evaluateRenderSceneForVisualization, {
                    renderCount: scene.renderCount || 1, continueRenderingOnDone: !!scene.continueRenderingOnDone,
                }))).toBeTruthy();
                const png = await stage("export", () => page.screenshot());
                result.pngBytes = png.length;
                await stage("compare", async () => {
                    const image = PNG.sync.read(png);
                    expect({ width: image.width, height: image.height }).toEqual({ width: 600, height: 400 });
                    const errorsBefore = testInfo.errors.length;
                    expect(png).toMatchSnapshot(name, {
                        threshold: results.threshold, maxDiffPixelRatio: (scene.errorRatio || 1.1) / 100,
                    });
                    expect(testInfo.errors.length).toBe(errorsBefore);
                });
                result.status = "passed";
            } catch (error) {
                result.status = currentStage === "reference" ? "missing-reference" :
                    currentStage === "compare" ? "image-mismatch" :
                    /timed out|timeout/i.test(String(error)) ? "timeout" : "scene-error";
                result.failedStage = currentStage;
                result.error = String(error);
                result.console = consoleLines;
                recover = result.status === "timeout" || /exited|closing/i.test(String(error));
                console.log(`FAILED ${item.index} ${scene.title}: ${result.status} at ${currentStage}: ${error.message}`);
            } finally {
                result.executionMs = performance.now() - sceneStarted;
                const cleanupStarted = performance.now();
                if (!recover) {
                    try {
                        page.setDefaultTimeout(15_000);
                        if (engineFlags) {
                            await page.evaluate(shared.evaluateDisposeSceneForVisualization, engineFlags);
                        }
                        await page.evaluate(() => {
                            if (window.engine) {
                                window.engine.stopRenderLoop();
                                window.engine.dispose();
                            }
                            window.engine = null;
                            window.scene = null;
                            delete globalThis.__babylonNativeScreenshot;
                        });
                    } catch (error) {
                        result.cleanupError = String(error);
                        result.status = "cleanup-error";
                        recover = true;
                    }
                }
                result.cleanupMs = performance.now() - cleanupStarted;
                result.totalMs = performance.now() - sceneStarted;
                results.cases.push(result);
                if (recover) {
                    page.abort(new Error(`Recovering after scene ${item.index}: ${result.status}`));
                    await close();
                    result.recoveryMs = performance.now() - sceneStarted - result.totalMs;
                }
                persist();
                if (results.cases.length % 10 === 0) {
                    console.log(`${testInfo.project.name}: ${results.cases.length}/${selected.length}, ${results.passed} passed, ${results.failed} failed, ${(results.elapsedMs / 1000).toFixed(1)}s`);
                }
            }
        }
        results.completed = true;
    } catch (error) {
        results.runError = String(error);
        throw error;
    } finally {
        try {
            await close();
        } finally {
            persist();
        }
    }
    expect(results.cases).toHaveLength(selected.length);
    expect(results.failed, `Full outcomes: ${filename}`).toBe(0);
});
