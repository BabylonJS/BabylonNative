const path = require("node:path");
const { createRequire } = require("node:module");

for (const name of ["TIMING_SUITE_ROOT", "TIMING_ARTIFACTS", "TIMING_PHASE", "BABYLON_NATIVE_PLAYGROUND"]) {
    if (!process.env[name]) {
        throw new Error(`Set ${name} before running the experiment`);
    }
}
const root = path.resolve(process.env.TIMING_SUITE_ROOT);
const { defineConfig } = createRequire(path.join(root, "package.json"))("@playwright/test");
delete process.env.BABYLON_NATIVE_TRANSPORT;
process.env.BABYLON_NATIVE_PLATFORM = "win32";
process.env.BABYLON_NATIVE_GRAPHICS_API = "d3d11";
process.env.TIMEOUT = "300000";
const output = path.join(process.env.TIMING_ARTIFACTS, process.env.TIMING_PHASE);

module.exports = defineConfig({
    testDir: path.join(root, "packages", "tools", "tests", "test", "playwright"),
    testMatch: "visualization.native.test.ts",
    projects: [{ name: "native", use: {} }],
    workers: 1,
    fullyParallel: true,
    retries: 0,
    timeout: 300_000,
    globalTimeout: 95 * 60_000,
    updateSnapshots: "none",
    snapshotPathTemplate: path.join(root, "packages", "tools", "tests", "test", "visualization", "ReferenceImages", "{arg}{ext}"),
    outputDir: path.join(output, "test-results"),
    use: { trace: "off", video: "off", screenshot: "off" },
    reporter: [["line"], ["json", { outputFile: path.join(output, "playwright.json") }]],
});
