import { cp, mkdir, readFile, readdir, rm, writeFile } from "node:fs/promises";
import { existsSync } from "node:fs";
import { createHash } from "node:crypto";
import { dirname, join, resolve, relative } from "node:path";
import { fileURLToPath, pathToFileURL } from "node:url";

const clone = resolve(process.argv[2] ?? "build\\lite-parity");
const playground = resolve(process.argv[3] ?? "build\\dawn\\Apps\\Playground\\Playground.exe");
const artifacts = resolve(process.argv[4] ?? "build\\lite-parity-results");
const timeout = Number(process.argv[5] ?? 60000);
if (!Number.isFinite(timeout) || timeout <= 0 || timeout > 2147483647) {
    throw new Error("Parity timeout must be a positive bounded number of milliseconds");
}
const generated = resolve(process.argv[6] ?? join(clone, ".native-parity"));
const references = join(artifacts, "references");
const scripts = dirname(fileURLToPath(import.meta.url));
const options = {
    playground, liteDirectory: clone, assetBase: "http://127.0.0.1:5179",
    bundleDirectory: join(dirname(playground), "Scripts", "lite-parity"),
    artifactsDirectory: join(artifacts, "native"), historyFile: join(artifacts, "history.jsonl"),
    testTimeout: timeout, timeoutMs: timeout,
};
await mkdir(generated, { recursive: true });
await mkdir(artifacts, { recursive: true });
await rm(join(generated, "tests", "lite", "parity", "scenes", "native-catalog-demos.spec.ts"), { force: true });
await cp(join(clone, "tests", "lite", "parity"), join(generated, "tests", "lite", "parity"), { recursive: true });
await cp(join(clone, "tests", "shared"), join(generated, "tests", "shared"), { recursive: true });
await cp(join(clone, "reference", "lite"), references, { recursive: true });
await writeFile(join(generated, "package.json"), JSON.stringify({ type: "module" }));
await writeFile(join(generated, "fixture.ts"), `
import { test as base, expect as browserExpect } from '@playwright/test';
import { createParityFixture } from ${JSON.stringify(pathToFileURL(join(scripts, "lite-parity-fixture.mjs")).href)};
const fixture = createParityFixture(base, browserExpect, ${JSON.stringify(options)});
export const {test, expect, REUSE_BROWSER, acquireContext, acquireReferencePage, reusedContext} = fixture;
`);
await writeFile(join(generated, "browser-fixture.ts"), "export {test, expect} from '@playwright/test';\n");
await writeFile(join(generated, "tests", "lite", "parity", "parity-fixtures.ts"),
    "export * from '../../../fixture';\n");
await writeFile(join(generated, "tests", "shared", "reuse-fixtures.ts"),
    "export * from '../../fixture';\n");

const rewrites = [];
const sources = [];
async function transformDirectory(directory) {
    for (const entry of await readdir(directory, { withFileTypes: true })) {
        const path = join(directory, entry.name);
        if (entry.isDirectory()) {
            await transformDirectory(path);
        } else if (entry.name.endsWith(".ts")) {
            const original = await readFile(join(clone, relative(generated, path)), "utf8");
            const input = await readFile(path, "utf8");
            let source = input.replace(/(['"])(\.\.\/)+(scene-config\.json|reference\/lite[^'"]*|lab\/[^'"]*|packages\/[^'"]*|scripts\/[^'"]*)\1/g,
                (whole, quote, ignored, suffix) => {
                    const target = suffix.startsWith("reference/lite")
                        ? resolve(references, suffix.slice("reference/lite".length).replace(/^[/\\]/, ""))
                        : resolve(clone, ...suffix.split("/"));
                    return JSON.stringify(target);
                });
            source = source.replace(/test\.describe\.serial/g, "test.describe");
            if (entry.name === "scene276-npe-animations.spec.ts") {
                source = source.replace(/from ['"]@playwright\/test['"]/g, "from '../parity-fixtures'");
            }
            if (entry.name === "pbr-gamma-rebuild.spec.ts") {
                source = source.replace(/from ['"]@playwright\/test['"]/g, "from '../parity-fixtures'");
            }
            if (entry.name === "bundle-size.spec.ts") {
                source = source.replace(/from ['"]\.\/parity-fixtures['"]/g, "from '../../../browser-fixture'");
            }
            source = source.replace(/from ("[A-Z]:[^"]+")/g, (whole, quoted) => {
                let target = JSON.parse(quoted);
                if (!existsSync(target) && existsSync(target + ".ts")) {
                    target += ".ts";
                }
                return "from " + JSON.stringify(pathToFileURL(target).href);
            });
            if (entry.name === "compare-utils.ts") {
                source = source.replace(/`\/babylon-ref-scene/g, "`http://127.0.0.1:5179/babylon-ref-scene");
            }
            if (entry.name === "compare-core.ts") {
                source = source.replace(/export function compareImages\(/, "function compareImagesUnchecked(")
                    .replace(/export function compareRegion\(/, "function compareRegionUnchecked(");
                source += `
function nativeParityDimensions(actual: string, reference: string) {
    const a = NativeParityPNG.sync.read(nativeParityReadFile(actual));
    const b = NativeParityPNG.sync.read(nativeParityReadFile(reference));
    if (a.width !== b.width || a.height !== b.height) {
        throw new Error('PARITY_DIMENSION_MISMATCH: ' + a.width + 'x' + a.height + ' vs ' + b.width + 'x' + b.height);
    }
}
export function compareImages(...args: Parameters<typeof compareImagesUnchecked>) {
    nativeParityDimensions(args[0], args[1]);
    const result = compareImagesUnchecked(...args);
    nativeParityAppend(${JSON.stringify(join(artifacts, "metrics.jsonl"))}, JSON.stringify({testId:nativeParityTest.info().testId, recordedAt:new Date().toISOString(), type:'full', actual:args[0], reference:args[1], result})+'\\n');
    return result;
}
export function compareRegion(...args: Parameters<typeof compareRegionUnchecked>) {
    nativeParityDimensions(args[0], args[1]);
    const result = compareRegionUnchecked(...args);
    nativeParityAppend(${JSON.stringify(join(artifacts, "metrics.jsonl"))}, JSON.stringify({testId:nativeParityTest.info().testId, recordedAt:new Date().toISOString(), type:'region', actual:args[0], reference:args[1], result})+'\\n');
    return result;
}
`;
                source = `import {test as nativeParityTest} from '@playwright/test';
import {PNG as NativeParityPNG} from 'pngjs';
import {readFileSync as nativeParityReadFile, appendFileSync as nativeParityAppend} from 'node:fs';
` + source;
            }
            if (source.includes("__dirname")) {
                source = `import {fileURLToPath as nativeParityFileURL} from 'node:url';
import {dirname as nativeParityDirectory} from 'node:path';
const __dirname = nativeParityDirectory(nativeParityFileURL(import.meta.url));
` + source;
            }
            if (source !== original) {
                rewrites.push({
                    file: relative(generated, path),
                    originalSHA256: createHash("sha256").update(original).digest("hex"),
                    generatedSHA256: createHash("sha256").update(source).digest("hex"),
                });
                await writeFile(path, source);
            }
            sources.push({
                file: relative(generated, path),
                originalSHA256: createHash("sha256").update(original).digest("hex"),
                generatedSHA256: createHash("sha256").update(source).digest("hex"),
                rewritten: source !== original,
            });
        }
    }
}
await transformDirectory(join(generated, "tests"));
await writeFile(join(generated, "tests", "lite", "parity", "scenes", "native-catalog-demos.spec.ts"), `
import {test, expect} from '../parity-fixtures';
for (const id of [180,181,227,228]) {
    test('scene'+id+' native execution (no upstream parity spec/oracle)', async ({page}) => {
        await page.goto('/scene'+id+'.html');
        await page.waitForFunction(() => document.getElementById('renderCanvas')?.dataset.ready === 'true');
        if (id === 227 || id === 228) {
            await page.waitForFunction(() => document.getElementById('canvasB')?.dataset.ready === 'true');
        }
        expect(await page.locator('#renderCanvas').getAttribute('data-error')).toBe(null);
        await page.locator('#renderCanvas').screenshot();
    });
}
`);
await writeFile(join(artifacts, "rewrites.json"), JSON.stringify(rewrites, null, 2));
await writeFile(join(artifacts, "test-sources.json"), JSON.stringify(sources, null, 2));
await writeFile(join(generated, "playwright.config.mjs"), `
import {defineConfig} from '@playwright/test';
export default defineConfig({
    testDir: './tests/lite/parity',
    outputDir: ${JSON.stringify(join(artifacts, "test-output"))},
    timeout: ${timeout},
    globalTimeout: 0,
    fullyParallel: false,
    workers: 1,
    retries: 0,
    maxFailures: 0,
    reporter: [
        ['list'],
        [${JSON.stringify(join(scripts, "lite-parity-reporter.mjs"))}, {output: ${JSON.stringify(artifacts)}}],
        ['json', {outputFile: ${JSON.stringify(join(artifacts, "playwright-results.json"))}}],
    ],
    use: {
        baseURL: 'http://127.0.0.1:5179',
        viewport: {width:1280,height:720},
        deviceScaleFactor: 1,
        browserName: 'chromium',
        channel: 'chrome',
        launchOptions: {args:['--force-color-profile=srgb','--enable-unsafe-webgpu']},
        trace: 'off',
        screenshot: 'off',
    },
});
`);
await writeFile(join(artifacts, "execution-options.json"), JSON.stringify({
    ...options, generated, references, timeout,
    deviations: [
        "Explicit repository/reference roots replace upstream off-by-one paths.",
        "Native Page replaces only Lite targets; BJS targets remain real Chrome.",
        "Copied goldens are isolated; force/live captures never overwrite tracked Lite references.",
        "Original skip gates are logged but not used to omit requested native attempts.",
        "Serial groups run sequentially without fail-fast sibling skips.",
        `Per-test deadlines are capped at ${timeout}ms; timeout means inconclusive, not renderer failure.`,
        "Pixel comparison rejects differing dimensions instead of comparing only overlap.",
        "Browser bundle-size checks remain browser-only; they do not measure native IIFEs.",
    ],
}, null, 2));
console.log("Prepared original parity specs at " + generated);
