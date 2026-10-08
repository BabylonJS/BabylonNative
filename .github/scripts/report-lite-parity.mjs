import { readFile, writeFile } from "node:fs/promises";
import { dirname, resolve, relative } from "node:path";
import { execFileSync } from "node:child_process";
import { latestCases, metricsForCases, summarizeRun } from "./lite-parity-records.mjs";

const clone = resolve(process.argv[2] ?? "build\\lite-parity");
const artifacts = resolve(process.argv[3] ?? "build\\lite-parity-results");
const output = resolve(process.argv[4] ?? "Dawn-Lite-Parity.md");
const browserArtifacts = resolve(process.argv[5] ?? "build\\lite-parity-browser-results");
const catalog = JSON.parse(await readFile(resolve(clone, "scene-config.json"), "utf8"));
async function optionalJson(path) {
    try {
        return JSON.parse(await readFile(path, "utf8"));
    } catch (error) {
        if (error.code === "ENOENT") {
            return undefined;
        }
        throw error;
    }
}
const runtime = await optionalJson(resolve(artifacts, "test-runtime.json"))
    ?? await optionalJson(resolve(browserArtifacts, "test-runtime.json"));
const browserEnvironment = await optionalJson(resolve(artifacts, "browser-environment.json"));
const executionOptions = await optionalJson(resolve(artifacts, "execution-options.json"));
const provenance = await optionalJson(resolve(artifacts, "provenance.json"));
async function jsonLines(path) {
    try {
        return (await readFile(path, "utf8")).trim().split("\n").filter(Boolean).map(line => JSON.parse(line));
    } catch (error) {
        if (error.code === "ENOENT") {
            return [];
        }
        throw error;
    }
}
const attempts = await jsonLines(resolve(artifacts, "cases.jsonl"));
const cases = latestCases(attempts);
const scheduled = await optionalJson(resolve(artifacts, "full-scheduled-tests.json"))
    ?? await optionalJson(resolve(artifacts, "scheduled-tests.json"));
const browserRecords = await jsonLines(resolve(browserArtifacts, "cases.jsonl"));
const browserCases = Array.from(new Map(browserRecords.map(record => [
    record.location.file.split(/[\\/]/).at(-1) + ":" + record.title.join("/"), record,
])).values());
const metrics = metricsForCases(await jsonLines(resolve(artifacts, "metrics.jsonl")), cases);
const coverage = await jsonLines(resolve(artifacts, "coverage.jsonl"));
const strip = text => String(text ?? "").replace(/\x1b\[[0-9;]*m/g, "");
const cell = text => strip(text).replace(/\|/g, "\\|").replace(/\r?\n/g, " ").replace(/\s+/g, " ").trim();
const sourceId = test => {
    if (!/[\\/]scenes[\\/]/.test(test.location.file)) {
        return undefined;
    }
    const match = /scene(\d+)[-.]/.exec(test.location.file) ?? /scene(\d+)/.exec(test.title.at(-1));
    return match ? Number(match[1]) : undefined;
};
const links = path => relative(dirname(output), path).replace(/\\/g, "/");
const counts = new Map();
const rows = [];
for (const scene of catalog) {
    const tests = cases.filter(test => sourceId(test) === scene.id);
    const native = tests.filter(test => test.history.some(item => item.backend === "native"));
    const failed = tests.filter(test => test.status !== "passed");
    const diagnostic = browserCases.filter(test => sourceId(test) === scene.id);
    const testIds = new Set(tests.map(test => test.id));
    const comparisons = metrics.filter(metric => metric.testId ?
        testIds.has(metric.testId) : metric.actual.includes(scene.slug));
    const full = comparisons.filter(metric => metric.type === "full");
    const fallback = scene.id === 164 ? undefined : coverage.findLast(record => record.id === scene.id);
    const screenshots = native.flatMap(test => test.history).flatMap(item => item.captures ?? []);
    const logs = [];
    for (const navigation of native.flatMap(test => test.history)) {
        if (navigation.logPath) {
            logs.push(await readFile(navigation.logPath, "utf8"));
        }
    }
    const nativeLog = logs.join("\n");
    const nativeExecution = screenshots.length ? "Rendered/captured"
        : fallback?.status === "EXECUTION PASS" ? "Rendered (coverage)"
        : native.length || fallback ? "Attempted; see reason" : "Not reached";
    let verdict = "NOT RUN";
    let reason = "No completed case was recorded.";
    if (tests.length) {
        if (failed.length) {
            verdict = failed.some(test => test.status === "timedOut") ? "TIMEOUT" : "FAIL";
            reason = failed.flatMap(test => test.errors).map(error => strip(error.message)).find(Boolean)
                ?? "Original test assertions did not complete successfully.";
            reason = reason.split("\n").map(line => line.trim()).filter(Boolean).slice(0, 6).join(" ").slice(0, 280);
            if (/exited before page close/.test(reason)) {
                const loggedError = nativeLog.split("\n").find(line =>
                    /LITE_PARITY_BRIDGE_FAIL:|\[Uncaught Error\]|TypeError:|ReferenceError:/.test(line));
                const crash = /--- (?:BN: )?CRASH ---/.test(nativeLog);
                const crashFrames = nativeLog.split("\n").filter(line =>
                    /^\s*\d+:\s/.test(line) && /Playground\.exe/.test(line)).slice(0, 2).map(line =>
                    line.trim().replace(/^.*?Playground\.exe\+\S+\s+/, "").slice(0, 180));
                reason = crash ? "Native crash (exit 3): " + (crashFrames.join("; ") || "see native callstack")
                    : loggedError?.trim() ?? reason;
                verdict = crash ? "CRASH" : "RUNTIME FAIL";
            } else if (/NATIVE_SCENE_ERROR:/.test(reason)) {
                verdict = /fetch failed|ASSET_FAIL/i.test(reason) ? "ASSET / FETCH FAIL" : "RUNTIME FAIL";
            } else if (/waitForFunction timed out/.test(reason)) {
                verdict = "TIMEOUT";
            } else if (full.length && /MAD|pixel|within|Exact/i.test(reason)) {
                verdict = "PIXEL MISMATCH";
            }
            if (verdict === "TIMEOUT") {
                const predicate = failed.flatMap(test => test.errors)
                    .map(error => error.message).find(message => /waitForFunction timed out/.test(message));
                const flags = predicate ? Array.from(new Set(
                    Array.from(predicate.matchAll(/dataset\.([A-Za-z0-9_]+)/g), match => match[1]))) : [];
                if (flags.length) {
                    reason = "Readiness timeout: " + flags.join(", ") + " did not reach their required states within 60s.";
                }
            }
            if (!native.length) {
                verdict = "ORACLE / HARNESS BLOCKED";
                reason = "Before native execution / oracle or harness failure: " + reason;
            } else if (/unsupported:|Unsupported parity|used before navigation|no CSS stylesheet engine|no real DOM input-event injection/i.test(reason)) {
                verdict = "HARNESS LIMIT";
            } else if (/PARITY_DIMENSION_MISMATCH/.test(reason)) {
                verdict = "DIMENSION MISMATCH";
            }
        } else if (!native.length) {
            verdict = "NO NATIVE ATTEMPT";
            reason = "Only browser or Node assertions were recorded; no NativeDawn navigation.";
        } else if ([164,180,181,186,187,227,228,300,301,303,304].includes(scene.id)) {
            verdict = "BEHAVIOR PASS";
            reason = "Original behavioral/invariance checks passed; no Babylon.js pixel oracle. Not a visual-parity PASS.";
        } else {
            verdict = "PASS";
            reason = "All recorded original scene assertions passed with NativeDawn.";
        }
        if (fallback?.error) {
            const detail = fallback.error.message.split("\n")[0];
            reason += " Independent native coverage: " + detail.slice(0, 220) + ".";
            if (detail.includes("LITE_PARITY_ASSET_FAIL: data:")) {
                verdict = "DATA-URL FETCH FAIL";
                reason = "Native fetch rejects an inline data:image URI; no external HTTP request is involved. " +
                    "Confirmed by URL-tagged retry. " + reason;
            }
        } else if (fallback?.status === "EXECUTION PASS") {
            reason += " Independent coverage confirms native readiness/readback; oracle parity remains unproven.";
        }
    }
    if (scene.skipParity || scene.skipParityOnCI) {
        reason += " Upstream quarantine: " + (scene.skipParityReason ?? scene.skipNotes ?? "catalog skip flag") + ".";
    }
    counts.set(verdict, (counts.get(verdict) ?? 0) + 1);
    const browserStatus = !diagnostic.length ? "Not replayed"
        : diagnostic.every(test => test.status === "passed") ? `${diagnostic.length}/${diagnostic.length} PASS`
        : `${diagnostic.filter(test => test.status === "passed").length}/${diagnostic.length} PASS`;
    if (failed.length && diagnostic.length && diagnostic.every(test => test.status === "passed")) {
        reason += " The identical browser-Lite assertions pass: native/host-specific failure.";
    } else if (failed.length && diagnostic.some(test => test.status !== "passed")) {
        reason += " Browser-Lite replay also fails; not established as NativeDawn-only.";
    }
    const maxMad = full.length ? Math.max(...full.map(metric => metric.result.mad)) : undefined;
    const mad = maxMad === undefined ? "N/A" : `${maxMad.toFixed(4)} / ${scene.maxMad ?? "spec-specific"}`;
    const log = native.flatMap(test => test.history).find(item => item.logPath)?.logPath
        ?? fallback?.history.find(item => item.logPath)?.logPath;
    const evidence = log ? ` [log](${links(log)})` : "";
    rows.push(`| ${scene.id}: ${cell(scene.name)} | ${tests.filter(test => test.status === "passed").length}/${tests.length} | ${nativeExecution} | ${verdict} | ${mad} | ${browserStatus} | ${cell(reason)}${evidence} |`);
}
let run;
try {
    run = JSON.parse(await readFile(resolve(artifacts, "run-result.json"), "utf8"));
} catch (error) {
    if (error.code !== "ENOENT") {
        throw error;
    }
}
const { status: runStatus, pending } = summarizeRun(cases, scheduled, run?.status);
const extras = cases.filter(test => sourceId(test) === undefined);
const sceneCases = cases.filter(test => sourceId(test) !== undefined);
const nativeCases = sceneCases.filter(test => test.history.some(item => item.backend === "native"));
const oracleOnlyCases = sceneCases.filter(test =>
    !test.history.some(item => item.backend === "native") &&
    test.history.some(item => item.backend === "browser-oracle"));
const sha = execFileSync("git", ["-C", clone, "rev-parse", "HEAD"], { encoding: "utf8" }).trim();
const nativeSha = execFileSync("git", ["rev-parse", "HEAD"], { encoding: "utf8" }).trim();
const masterSha = provenance?.repositories?.BabylonNative?.mergeHead ??
    execFileSync("git", ["rev-parse", "--revs-only", "MERGE_HEAD"], { encoding: "utf8" }).trim();
const sourceChanges = masterSha ? `the uncommitted merge of master \`${masterSha}\` and ` : "uncommitted changes and ";
let text = `# Babylon-Lite parity on NativeDawn\n\n`;
text += `**Run status: ${runStatus}.** Generated ${new Date().toISOString()}.\n\n`;
if (scheduled) {
    text += `Scheduled coverage: **${scheduled.length - pending}/${scheduled.length} distinct cases completed**. ` +
        `Invocation-level results are not a full-suite verdict after a resume or focused retry.\n\n`;
}
text += `Babylon-Lite \`${sha}\`; BabylonNative \`${nativeSha}\` plus ${sourceChanges}this test adapter. Windows, V8, Dawn \`v20260209.194954\` / D3D12, NVIDIA GeForce RTX 4060. Recorded test Node: ${runtime?.node ?? "not recorded"}; browser: ${runtime?.browser ?? browserEnvironment?.version ?? "not recorded"} (${browserEnvironment?.vendor ?? "unrecorded vendor"}/${browserEnvironment?.architecture ?? "unrecorded architecture"}). Installation used the clone's declared pnpm 11.23.0. The Babylon.js oracle uses the clone's locked dependencies (9.28.0). Framebuffer 1280x720, DPR 1, sRGB browser profile.\n\n`;
text += `## Scope and method\n\n`;
text += `All **${catalog.length} catalog scenes** were bundled. The generated suite preserves original assertions, queries, readiness flags, interactions and comparison ceilings; four demos without specs get execution-only attempts. Real Chrome runs Babylon.js oracle substeps. No native-generated image is used as a Babylon.js reference. Copied goldens and fresh captures stay in the build artifacts, not the Lite checkout. MAD is mean absolute RGB byte error (0-255), not a percentage. Region and behavioral assertions remain enforced by the original specs, even though the table shows only the largest full-image MAD.\n\n`;
text += `PASS requires all completed scene cases to pass and a recorded native navigation. BEHAVIOR PASS has no Babylon.js pixel oracle. TIMEOUT is inconclusive at the recorded deadline, not proof of an unsupported renderer feature. Oracle/build/adapter failures are explicitly distinguished from native failures. Browser-Lite diagnostic replays use the same assertions rather than an approximate screenshot.\n\n`;
text += `Generated-only adaptations: corrected upstream repository/reference paths, loopback native Page transport, independent scene processes, quarantines attempted instead of skipped, serial groups without fail-fast sibling skips, test deadlines capped at ${(executionOptions?.timeout ?? 60000) / 1000} seconds, and strict dimension equality before image comparisons. IIFE execution does not provide browser module-chunk network semantics. Unsupported DOM/input/media/control operations fail explicitly; they are not replaced with successful no-ops.\n\n`;
text += `## Results\n\n`;
text += `Latest outcomes cover **${cases.length} distinct cases** (${attempts.length} recorded attempts): ` +
    `**${cases.filter(test => test.status === "passed").length} passed**, ` +
    `**${cases.filter(test => test.status === "failed").length} failed**, ` +
    `**${cases.filter(test => test.status === "timedOut").length} timed out**. ` +
    `Of these, ${sceneCases.length} are scene cases (${sceneCases.filter(test => test.status === "passed").length} passed); ` +
    `${extras.length} are other parity checks (${extras.filter(test => test.status === "passed").length} passed). ` +
    `Browser-only size/catalog passes must not be counted as NativeDawn scene passes.\n\n`;
text += `**Native scene cases: ${nativeCases.filter(test => test.status === "passed").length} passed / ${nativeCases.length} reached native execution.** ` +
    `${oracleOnlyCases.length} scene cases ran only browser-oracle substeps; ` +
    `${sceneCases.length - nativeCases.length - oracleOnlyCases.length} other scene cases did not reach a recorded native or oracle navigation. ` +
    `These are not native passes, even when their assertions pass.\n\n`;
const nativeIds = new Set(cases.flatMap(test => test.history ?? [])
    .filter(item => item.backend === "native")
    .map(item => Number(/\/scene(\d+)\.html/.exec(item.url)?.[1])));
coverage.forEach(record => nativeIds.add(record.id));
text += `**Native catalog coverage: ${catalog.filter(scene => nativeIds.has(scene.id)).length}/${catalog.length} scenes attempted.** ` +
    `Readiness-only fallback attempts do not replace the original parity assertions.\n\n`;
text += Array.from(counts, ([status, count]) => `**${status}: ${count}**`).join("; ") + `. Total: ${catalog.length} scenes; ${cases.length} completed test cases (${extras.length} non-scene cases).\n\n`;
text += `| Scene | Cases passed/recorded | Native execution | Verdict | Full MAD / catalog ceiling | Browser-Lite diagnostic | Reason / evidence |\n|---|---:|---|---|---:|---|---|\n`;
text += rows.join("\n") + "\n\n";
text += `## Failure interpretation\n\n`;
text += `The scene table keeps the original native assertion results. Supplemental 180-second readiness attempts and URL-tagged fetch retries add evidence without turning an unverified image into a parity pass. Device-recovery scene 164 requires its original pre-loss/capture/recovery handshake; a generic readiness-only retry is not a valid recovery test.\n\n`;
text += `The table describes this artifact set, not assumed failures from an earlier build. A browser-Lite failure shared with NativeDawn is not evidence of a native-only defect. Inline data:image failures are separate from external HTTP failures. Differing image dimensions are invalid comparisons, not measured pixel mismatches. Real device loss/recovery, pointer input, multiple presentation canvases, DOM/CSS/media and browser module-network semantics are not supplied by successful WebGPU draw calls and must be judged by their original tests.\n\n`;
text += `Browser-Lite replay completed ${browserCases.length} distinct diagnostic cases: ` +
    `${browserCases.filter(test => test.status === "passed").length} passed, ` +
    `${browserCases.filter(test => test.status !== "passed").length} did not. ` +
    `Per-scene replay outcomes are shown in the table; a shared failure does not establish NativeDawn as its sole cause.\n\n`;
text += `## Additional parity checks\n\n| Test | Status | Reason |\n|---|---|---|\n`;
text += `| Browser-only bundle-size and Node/catalog checks | ${extras.filter(test => test.status === "passed").length} passed | Not native rendering coverage. |\n`;
for (const test of extras.filter(test => test.status !== "passed")) {
    text += `| ${cell(test.title.at(-1))} | ${test.status} | ${cell(test.errors[0]?.message ?? "Passed; browser-only network-size or Node/catalog checks are not native scene coverage.").slice(0, 300)} |\n`;
}
text += `\n## Reproduction and artifacts\n\n\`\`\`powershell\n`;
const reproductionExecutable = executionOptions?.playground ?? provenance?.executable.path
    ?? resolve("build\\dawn\\Apps\\Playground\\Playground.exe");
const reproductionBundles = executionOptions?.bundleDirectory
    ?? resolve(dirname(reproductionExecutable), "Scripts", "lite-parity");
const reproductionGenerated = executionOptions?.generated ?? resolve(clone, ".native-parity");
text += `node .github\\scripts\\bundle-lite-parity.mjs "${relative(resolve("."), clone)}" "${relative(resolve("."), reproductionBundles)}"\n`;
text += `node .github\\scripts\\prepare-lite-parity.mjs "${relative(resolve("."), clone)}" "${relative(resolve("."), reproductionExecutable)}" "${relative(resolve("."), artifacts)}" ${executionOptions?.timeout ?? 60000} "${relative(resolve("."), reproductionGenerated)}"\n`;
text += `# Serve the clone's lab on 127.0.0.1:5179, then run from the clone:\n`;
text += `pnpm exec playwright test --config "${relative(clone, reproductionGenerated)}\\playwright.config.mjs"\n`;
text += `# Focused runs additionally require their preserved test-list or original spec selectors.\n`;
text += `# From BabylonNative:\nnode .github\\scripts\\report-lite-parity.mjs build\\lite-parity "${relative(resolve("."), artifacts)}" "${relative(resolve("."), output)}"\n\`\`\`\n\n`;
if (provenance) {
    text += `### Source identity\n\n`;
    for (const [name, identity] of Object.entries(provenance.repositories)) {
        text += `- **${name}**: HEAD \`${identity.head}\`; source-patch SHA256 \`${identity.patchSHA256}\`; ` +
            `${identity.newFiles.length} nonignored new files.\n`;
    }
    text += `\nPlayground executable SHA256: \`${provenance.executable.sha256}\`. ` +
        `The provenance file records the exact local dependency source overrides, changed/new-file hashes, and build cache settings. ` +
        `The BabylonNative source fingerprint excludes Markdown report/documentation files to avoid a self-referential report hash. ` +
        `Local-source results do not establish that the unchanged default dependency pins provide these fixes.\n\n`;
}
text += `The browser diagnostic rerun uses \`node .github\\scripts\\diagnose-lite-parity.mjs\`. ` +
    `For selected readiness/asset evidence, use \`node .github\\scripts\\complete-lite-parity.mjs build\\lite-parity build\\lite-parity-results build\\dawn\\Apps\\Playground\\Playground.exe <comma-separated-IDs>\`; ` +
    `these captures are execution diagnostics, not replacements for the original parity assertions. ` +
    `The two additional ocean/PBR-gamma tests are blocked by unsupported non-catalog navigation/content handling in this adapter and are not valid native feature verdicts.\n\n`;
text += `Results: [case records](${links(resolve(artifacts, "cases.jsonl"))}), [metrics](${links(resolve(artifacts, "metrics.jsonl"))}), [Playwright report](${links(resolve(artifacts, "playwright-results.json"))}), [adapter rewrites](${links(resolve(artifacts, "rewrites.json"))}), [all copied test-source hashes](${links(resolve(artifacts, "test-sources.json"))}), [source/build provenance](${links(resolve(artifacts, "provenance.json"))}), [execution options](${links(resolve(artifacts, "execution-options.json"))}). Native logs/screenshots and isolated reference images live alongside these files. Build artifacts are local and ignored by Git.\n`;
await writeFile(output, text);
console.log(`Wrote ${output}: ${catalog.length} scene rows, ${cases.length} completed cases, run ${runStatus}`);
