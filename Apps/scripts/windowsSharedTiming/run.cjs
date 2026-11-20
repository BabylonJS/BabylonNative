const fs = require("node:fs");
const path = require("node:path");
const crypto = require("node:crypto");
const { spawn, execFileSync } = require("node:child_process");

function hash(file) {
    const bytes = fs.readFileSync(file);
    const content = file.endsWith(".json") ? bytes.toString("utf8").replace(/\r\n/g, "\n") : bytes;
    return crypto.createHash("sha256").update(content).digest("hex");
}

function summarizePhase(report, titles) {
    const specs = [];
    function visit(suite) {
        specs.push(...(suite.specs || []));
        for (const child of suite.suites || []) {
            visit(child);
        }
    }
    visit(report);
    if (new Set(titles).size !== titles.length || specs.length !== titles.length ||
        new Set(specs.map(spec => spec.title)).size !== titles.length ||
        specs.some(spec => !titles.includes(spec.title))) {
        throw new Error("Executed scene names differ from the fixed workload");
    }
    const stagesMs = {};
    const failures = [];
    let testsMs = 0;
    let passed = 0;
    let capturedFrames = 0;
    for (const spec of specs) {
        if (spec.tests.length !== 1 || spec.tests[0].results.length !== 1) {
            throw new Error(`Expected exactly one attempt for ${spec.title}`);
        }
        const result = spec.tests[0].results[0];
        if (!["passed", "failed", "timedOut"].includes(result.status)) {
            throw new Error(`Scene did not complete: ${spec.title} (${result.status})`);
        }
        testsMs += result.duration;
        for (const step of result.steps || []) {
            stagesMs[step.title] = (stagesMs[step.title] || 0) + step.duration;
            if (step.title === "Compare screenshot") {
                capturedFrames++;
            }
        }
        if (result.status === "passed") {
            passed++;
        } else {
            failures.push({ title: spec.title, status: result.status, errors: result.errors });
        }
    }
    return {
        selected: titles.length, executed: specs.length, passed, failed: failures.length,
        completed: true, playwrightMs: report.stats.duration, testsMs, capturedFrames,
        stagesMs, failures,
    };
}

async function main() {
    for (const name of ["TIMING_SUITE_ROOT", "TIMING_ARTIFACTS", "BABYLON_NATIVE_PLAYGROUND"]) {
        if (!process.env[name]) {
            throw new Error(`Set ${name} before running the experiment`);
        }
    }
    const root = path.resolve(process.env.TIMING_SUITE_ROOT);
    const artifacts = path.resolve(process.env.TIMING_ARTIFACTS);
    const executable = path.resolve(process.env.BABYLON_NATIVE_PLAYGROUND);
    const workload = JSON.parse(fs.readFileSync(path.join(__dirname, "workload.json"), "utf8"));
    const suiteHead = execFileSync("git", ["-C", root, "rev-parse", "HEAD"], { encoding: "utf8" }).trim();
    if (suiteHead !== workload.suiteHead) {
        throw new Error("Shared suite revision differs from the measured mobile revision");
    }
    for (const file of workload.files) {
        if (hash(path.join(root, file.path)) !== file.sha256) {
            throw new Error(`Shared scene metadata or original reference changed: ${file.path}`);
        }
    }
    fs.mkdirSync(artifacts, { recursive: true });
    if (fs.existsSync(path.join(artifacts, "summary.json"))) {
        throw new Error(`Refusing to overwrite an earlier experiment: ${artifacts}`);
    }
    const summary = { suiteHead, executableSha256: hash(executable), workers: 1, retries: 0, phases: [] };
    const persist = () => fs.writeFileSync(path.join(artifacts, "summary.json"), JSON.stringify(summary, null, 2));
    const cli = require.resolve("@playwright/test/cli", { paths: [root] });
    for (const phase of workload.phases) {
        const output = path.join(artifacts, phase.name);
        if (fs.existsSync(output)) {
            throw new Error(`Refusing to overwrite an earlier phase: ${output}`);
        }
        fs.mkdirSync(output);
        const escape = title => title.replace(/[.*+?^${}()|[\]\\]/g, "\\$&");
        const grep = `visualization\\.native\\.test\\.ts (?:${phase.titles.map(escape).join("|")})$`;
        const result = { mode: phase.name, selected: phase.titles.length, completed: false, startedAtMs: Date.now() };
        summary.phases.push(result);
        persist();
        const started = performance.now();
        const log = fs.openSync(path.join(output, "runner.log"), "wx");
        try {
            result.exitCode = await new Promise((resolve, reject) => {
                const child = spawn(process.execPath, [cli, "test", "--config", path.join(__dirname, "playwright.config.cjs"),
                    "--grep", grep, "--workers", "1", "--retries", "0"], {
                    cwd: root,
                    env: { ...process.env, TIMING_SUITE_ROOT: root, TIMING_ARTIFACTS: artifacts,
                        TIMING_PHASE: phase.name, BABYLON_NATIVE_PLAYGROUND: executable },
                    stdio: ["ignore", log, log],
                });
                child.once("error", reject);
                child.once("exit", (code, signal) => {
                    if (signal) {
                        reject(new Error(`Playwright terminated by ${signal}`));
                    } else {
                        resolve(code ?? 1);
                    }
                });
            });
            result.processWallMs = performance.now() - started;
            const report = JSON.parse(fs.readFileSync(path.join(output, "playwright.json"), "utf8"));
            Object.assign(result, summarizePhase(report, phase.titles));
            console.log(JSON.stringify({ ...result, failures: undefined }));
        } catch (error) {
            result.error = error.stack || String(error);
            throw error;
        } finally {
            fs.closeSync(log);
            persist();
        }
    }
    if (summary.phases.some(phase => phase.exitCode !== 0 || phase.failed !== 0 || !phase.completed)) {
        throw new Error("The visual experiment retained failures; inspect the per-scene reports and image differences");
    }
}

module.exports = { summarizePhase };
if (require.main === module) {
    main().catch(error => {
        console.error(error);
        process.exitCode = 1;
    });
}
