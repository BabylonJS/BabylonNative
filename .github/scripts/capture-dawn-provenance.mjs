import { createHash } from "node:crypto";
import { execFileSync } from "node:child_process";
import { mkdir, readFile, writeFile } from "node:fs/promises";
import { dirname, resolve } from "node:path";

const build = resolve(process.argv[2] ?? "build\\dawn");
const output = resolve(process.argv[3] ?? "build\\lite-parity-postfix-results\\provenance.json");
const sha256 = bytes => createHash("sha256").update(bytes).digest("hex");
const cache = Object.fromEntries((await readFile(resolve(build, "CMakeCache.txt"), "utf8"))
    .split(/\r?\n/).flatMap(line => {
        const match = /^([^#/:]+):[^=]+=(.*)$/.exec(line);
        return match ? [[match[1], match[2]]] : [];
    }));

function dependencySource(override, source) {
    const directory = cache[override] || cache[source];
    if (!directory) {
        throw new Error(`Missing dependency source in CMake cache: ${override} / ${source}`);
    }
    return resolve(directory);
}

async function identity(name, directory, excludeReports = false) {
    const git = (...args) => execFileSync("git", ["-C", directory, ...args], { maxBuffer: 20 * 1024 * 1024 });
    const exclusions = excludeReports ? [":!*.md"] : [];
    const patch = git("diff", "--binary", "--no-ext-diff", "--no-textconv", "HEAD", "--", ".", ...exclusions);
    const files = git("ls-files", "--others", "--exclude-standard", "-z").toString("utf8")
        .split("\0").filter(file => file && (!excludeReports || !file.endsWith(".md"))).sort();
    const newFiles = await Promise.all(files.map(async file => ({
        file, sha256: sha256(await readFile(resolve(directory, file))),
    })));
    const manifest = Buffer.from(newFiles.map(({ file, sha256 }) => `${file}\t${sha256}\n`).join(""));
    const archive = resolve(dirname(output), "source-patches", name);
    await mkdir(archive, { recursive: true });
    await writeFile(resolve(archive, "tracked.patch"), patch);
    await writeFile(resolve(archive, "new-files.sha256"), manifest);
    for (const file of files) {
        const destination = resolve(archive, "new-files", file);
        await mkdir(dirname(destination), { recursive: true });
        await writeFile(destination, await readFile(resolve(directory, file)));
    }
    return {
        directory, head: git("rev-parse", "HEAD").toString("utf8").trim(),
        mergeHead: git("rev-parse", "--revs-only", "MERGE_HEAD").toString("utf8").trim() || null,
        patchSHA256: sha256(Buffer.concat([patch, manifest])),
        trackedPatchSHA256: sha256(patch), newFileManifestSHA256: sha256(manifest),
        newFiles, excludedReports: excludeReports, archive,
    };
}

const executablePath = resolve(process.argv[4] ?? resolve(build, "Apps", "Playground", "Playground.exe"));
const repositories = {
    BabylonNative: await identity("BabylonNative", resolve("."), true),
    JsRuntimeHost: await identity("JsRuntimeHost", dependencySource("FETCHCONTENT_SOURCE_DIR_JSRUNTIMEHOST", "JsRuntimeHost_SOURCE_DIR")),
    UrlLib: await identity("UrlLib", dependencySource("FETCHCONTENT_SOURCE_DIR_URLLIB", "UrlLib_SOURCE_DIR")),
    BabylonLite: await identity("BabylonLite", resolve("build\\lite-parity")),
};
await writeFile(output, JSON.stringify({
    capturedAt: new Date().toISOString(),
    repositories,
    executable: { path: executablePath, sha256: sha256(await readFile(executablePath)) },
    build,
    cache: Object.fromEntries(Object.entries(cache).filter(([name]) =>
        /^(CMAKE_(BUILD_TYPE|GENERATOR|CXX_COMPILER)|NAPI_JAVASCRIPT_ENGINE|BABYLON_NATIVE_PLUGIN_NATIVEDAWN|JSRUNTIMEHOST_POLYFILL_COMPRESSION|FETCHCONTENT_SOURCE_DIR_(URLLIB|JSRUNTIMEHOST))$/.test(name))),
    fingerprintDefinition: "SHA256(git diff --binary --no-ext-diff --no-textconv HEAD bytes || UTF-8 manifest with ordinal-sorted new file name, tab, lowercase SHA256, LF)",
}, null, 2));
console.log(`Recorded source and executable provenance at ${output}`);
