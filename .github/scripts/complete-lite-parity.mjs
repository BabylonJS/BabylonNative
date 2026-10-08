import { mkdir, readFile, appendFile } from "node:fs/promises";
import { resolve, join, dirname } from "node:path";
import { createNativeHost } from "./lite-parity-page.mjs";

const clone = resolve(process.argv[2] ?? "build\\lite-parity");
const artifacts = resolve(process.argv[3] ?? "build\\lite-parity-results");
const playground = resolve(process.argv[4] ?? "build\\dawn\\Apps\\Playground\\Playground.exe");
const requested = process.argv[5]?.split(",").map(Number);
const catalog = JSON.parse(await readFile(join(clone, "scene-config.json"), "utf8"));
if (requested && (!/^\d+(,\d+)*$/.test(process.argv[5])
    || requested.some(id => !catalog.some(scene => scene.id === id)))) {
    throw new Error("Requested coverage IDs must be a comma-separated list of catalog scene numbers");
}
if (requested?.includes(164)) {
    console.log("Scene164requires the original pre-loss/capture/recovery spec; no generic readiness retry.");
}
const cases = (await readFile(join(artifacts, "cases.jsonl"), "utf8"))
    .trim().split("\n").filter(Boolean).map(line => JSON.parse(line));
const attempted = new Set(cases.flatMap(test => test.history ?? [])
    .filter(item => item.backend === "native")
    .map(item => Number(/\/scene(\d+)\.html/.exec(item.url)?.[1])));
const queries = {
    seekTime: {
        5: 2, 7: 2, 11: 1.91, 12: .5, 20: 0, 23: 0, 26: 3, 34: 0, 39: 5,
        58: .72, 59: .72, 115: 100 / 60, 150: 1, 151: .5, 152: 1.91, 153: 1,
        154: .75, 155: 1, 156: 1.25, 157: 1.2, 158: 1.2, 169: 1.25, 211: .5,
        218: 1, 219: 1, 231: .5, 240: .5, 241: 2, 242: 1, 243: 1, 244: 1,
        245: 1, 246: 1, 250: 5, 251: .5, 253: 1, 254: 2, 255: 1, 302: 2,
    },
    captureFrame: {
        40: 120, 41: 10, 42: 300, 43: 300, 46: 10, 47: 1, 100: 120, 101: 150,
        102: 5, 103: 5, 104: 35, 105: 55, 106: 20, 290: 180,
    },
    captureAfter: { 44: 5, 45: 3 },
    capture: { 48: 1, 49: 1 },
    freeze: { 64: 1, 66: 1, 140: 1, 171: 1, 172: 1, 173: 1, 174: 1, 175: 1 },
    msaa: { 51: 4 },
    nocam: { 224: 1 },
};
await mkdir(join(artifacts, "coverage"), { recursive: true });
const host = await createNativeHost({
    playground, liteDirectory: clone, assetBase: "http://127.0.0.1:5179",
    bundleDirectory: join(dirname(playground), "Scripts", "lite-parity"),
    artifactsDirectory: join(artifacts, "native-coverage"), timeoutMs: 180000,
});
try {
    for (const scene of catalog.filter(scene => scene.id !== 164
        && (requested ? requested.includes(scene.id) : !attempted.has(scene.id)))) {
        const page = await host.createPage();
        const record = { id: scene.id, slug: scene.slug, status: "FAIL", history: page.history };
        let sceneError;
        page.on("console", message => {
            if (message.type() === "error") {
                sceneError = new Error(message.text());
            }
        });
        page.on("pageerror", error => { sceneError = error; });
        try {
            const params = new URLSearchParams();
            for (const [key, values] of Object.entries(queries)) {
                if (scene.id in values) {
                    params.set(key, String(values[scene.id]));
                }
            }
            const flags = ["ready"];
            if (params.has("seekTime") || [283,284].includes(scene.id)) {
                flags.push("animationFrozen");
            }
            if (params.has("captureFrame") || params.has("captureAfter") || params.has("capture")) {
                flags.push("captureReady");
            }
            if (scene.id === 272) { flags.push("swapped"); }
            if (scene.id === 273) { flags.push("added"); }
            await page.goto(`http://127.0.0.1:5179/scene${scene.id}.html?${params}`);
            const deadline = Date.now() + 180000;
            while (Date.now() < deadline) {
                if (sceneError) {
                    throw sceneError;
                }
                const dataset = await page.evaluate(() => ({ ...document.querySelector("canvas").dataset }));
                record.dataset = dataset;
                if (dataset.error) {
                    throw new Error(dataset.error);
                }
                if (flags.every(flag => dataset[flag] === "true")) {
                    record.status = "EXECUTION PASS";
                    break;
                }
                await page.waitForTimeout(50);
            }
            if (record.status !== "EXECUTION PASS") {
                throw new Error("Native coverage readiness timeout after 180s; required " + flags.join(", "));
            }
            await page.waitForTimeout(500);
            record.screenshot = join(artifacts, "coverage", `scene${scene.id}.png`);
            await page.locator("canvas").screenshot({ path: record.screenshot });
        } catch (error) {
            record.status = /timeout/i.test(error.message) ? "TIMEOUT" : "FAIL";
            record.error = { message: error.message, stack: error.stack };
        } finally {
            await page.close();
            await appendFile(join(artifacts, "coverage.jsonl"), JSON.stringify(record) + "\n");
            console.log(`Coverage scene${scene.id}: ${record.status}${record.error ? " — " + record.error.message.split("\n")[0] : ""}`);
        }
    }
} finally {
    await host.close();
}
