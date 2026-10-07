import { readFile, writeFile } from "node:fs/promises";
import { dirname, resolve, join } from "node:path";
import { fileURLToPath } from "node:url";
import { spawn } from "node:child_process";

const clone = resolve(process.argv[2] ?? "build\\lite-parity");
const nativeArtifacts = resolve(process.argv[3] ?? "build\\lite-parity-results");
const browserArtifacts = resolve(process.argv[4] ?? "build\\lite-parity-browser-results");
const playground = resolve(process.argv[5] ?? "build\\dawn\\Apps\\Playground\\Playground.exe");
const scripts = dirname(fileURLToPath(import.meta.url));
function run(file, args, options = {}) {
    return new Promise((resolveRun, reject) => {
        const child = spawn(process.execPath, [file, ...args], { stdio: "inherit", ...options });
        child.once("error", reject);
        child.once("exit", (code, signal) => {
            if (signal) {
                reject(new Error(`Diagnostic process terminated by ${signal}`));
            } else {
                resolveRun(code);
            }
        });
    });
}
const records = (await readFile(join(nativeArtifacts, "cases.jsonl"), "utf8"))
    .trim().split("\n").filter(Boolean).map(line => JSON.parse(line));
const files = Array.from(new Set(records
    .filter(test => test.status !== "passed" && /[\\/]scenes[\\/]/.test(test.location.file)
        && test.history.some(item => item.backend === "native"))
    .map(test => test.location.file)));
if (!files.length) {
    console.log("No failed native scene cases require browser-Lite diagnostic replay.");
    process.exit(0);
}
const preparation = await run(join(scripts, "prepare-lite-parity.mjs"),
    [clone, playground, browserArtifacts, "60000"]);
if (preparation !== 0) {
    throw new Error("Cannot prepare browser diagnostic suite: exit " + preparation);
}
await writeFile(join(browserArtifacts, "diagnostic-files.json"), JSON.stringify(files, null, 2));
const filter = files.map(file => file.split(/[\\/]/).at(-1).replace(/[.*+?^${}()|[\]\\]/g, "\\$&")).join("|");
const result = await run(join(clone, "node_modules", "@playwright", "test", "cli.js"),
    ["test", "--config", ".native-parity\\playwright.config.mjs", filter], {
        cwd: clone,
        env: {
            ...process.env, NATIVE_PARITY_BROWSER: "1", NATIVE_PARITY_REPORT: "0",
            REUSE_BROWSER: "false",
        },
    });
const report = await run(join(scripts, "report-lite-parity.mjs"), [
    clone, nativeArtifacts, resolve("Dawn-Lite-Parity.md"), browserArtifacts,
], { cwd: resolve(".") });
if (report !== 0) {
    throw new Error("Cannot refresh the parity report: exit " + report);
}
process.exitCode = result;
