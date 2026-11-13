const assert = require("node:assert/strict");
const { PNG } = require("pngjs");
const shared = require("./shared.cjs");
const workload = require("./workload.json");

async function main() {
    const scene = workload.cases[0].scene;
    const page = await shared.NativePage.launch();
    page.setDefaultTimeout(100_000);
    try {
        await page.setViewportSize({ width: 600, height: 400 });
        assert.equal(await page.evaluate(() => BABYLON.Engine.Version), "9.15.0");
        const flags = await page.evaluate(shared.evaluateInitEngineForVisualization, {
            engineName: "native", useLargeWorldRendering: scene.useLargeWorldRendering ?? false,
            useReverseDepthBuffer: scene.useReverseDepthBuffer ?? "false",
            useNonCompatibilityMode: scene.useNonCompatibilityMode ?? "false", baseUrl: shared.config.root,
        });
        const preloaded = scene.playgroundId
            ? await shared.preloadSnippetCode(scene.playgroundId, "https://snippet.babylonjs.com")
            : undefined;
        await page.evaluate(shared.evaluatePrepareScene, {
            sceneMetadata: { ...scene, snippetCode: preloaded?.code, snippetSceneCall: preloaded?.sceneCall },
            globalConfig: { root: shared.config.root, snippetUrl: "https://snippet.babylonjs.com", pgRoot: "https://playground.babylonjs.com" },
        });
        assert.ok(await page.evaluate(shared.evaluateRenderSceneForVisualization, {
            renderCount: scene.renderCount || 1, continueRenderingOnDone: !!scene.continueRenderingOnDone,
        }));
        const image = PNG.sync.read(await page.screenshot());
        assert.equal(image.width, 600);
        assert.equal(image.height, 400);
        await page.evaluate(shared.evaluateDisposeSceneForVisualization, flags);
        await page.evaluate(() => {
            window.engine.stopRenderLoop();
            window.engine.dispose();
            window.engine = null;
            window.scene = null;
        });
        console.log(`Native protocol/scene lifecycle smoke check passed: ${scene.title}`);
    } finally {
        await page.close();
    }
}

main().catch(error => {
    console.error(error);
    process.exitCode = 1;
});
