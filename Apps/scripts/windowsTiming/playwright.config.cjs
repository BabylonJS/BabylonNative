const path = require("path");
const parallel = process.env.TIMING_MODE === "parallel";
if (!["single", "parallel"].includes(process.env.TIMING_MODE)) {
    throw new Error("TIMING_MODE must be single or parallel");
}
module.exports = {
    testDir: __dirname,
    testMatch: "timing.test.cjs",
    workers: parallel ? 2 : 1,
    projects: parallel ? [{ name: "first" }, { name: "second" }] : [{ name: "single" }],
    retries: 0,
    timeout: 305 * 100_000 + 600_000,
    globalTimeout: 305 * 100_000 + 600_000,
    updateSnapshots: "none",
    outputDir: path.join(process.env.TIMING_ARTIFACTS, process.env.TIMING_MODE, "images"),
    snapshotPathTemplate: path.join(process.env.TIMING_SUITE_ROOT,
        "packages", "tools", "tests", "test", "visualization", "ReferenceImages", "{arg}{ext}"),
    reporter: [["line"], ["json", {
        outputFile: path.join(process.env.TIMING_ARTIFACTS, process.env.TIMING_MODE, "playwright.json"),
    }]],
};
