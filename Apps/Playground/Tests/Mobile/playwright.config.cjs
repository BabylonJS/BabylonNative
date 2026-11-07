const path = require("node:path");
const { createRequire } = require("node:module");

if (!process.env.BABYLON_JS_ROOT) {
    throw new Error("Set BABYLON_JS_ROOT to the Babylon.js checkout containing the shared native Playwright suite");
}
const root = path.resolve(process.env.BABYLON_JS_ROOT);
const sharedRequire = createRequire(path.join(root, "package.json"));
const { defineConfig } = sharedRequire("@playwright/test");
if (!["android", "ios"].includes(process.env.MOBILE_PLATFORM)) {
    throw new Error("MOBILE_PLATFORM must be android or ios");
}
process.env.BABYLON_NATIVE_TRANSPORT = path.join(__dirname, "device-broker.cjs");
process.env.BABYLON_NATIVE_STARTUP_TIMEOUT_MS = "180000";
process.env.TIMEOUT ||= "300000";
process.env.BABYLON_NATIVE_PLATFORM = process.env.MOBILE_PLATFORM;
process.env.BABYLON_NATIVE_GRAPHICS_API = process.env.MOBILE_PLATFORM === "ios" ? "metal" : "gles";
const outputDir = process.env.MOBILE_OUTPUT || path.join(__dirname, "mobile-results");

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
    outputDir: path.join(outputDir, "test-results"),
    use: { trace: "off", video: "off", screenshot: "off" },
    reporter: [["line"], ["json", { outputFile: path.join(outputDir, "playwright-results.json") }]],
});
