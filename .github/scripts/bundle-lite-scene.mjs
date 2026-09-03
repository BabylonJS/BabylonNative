// Bundle a single Babylon-Lite lab scene into one self-contained IIFE "classic"
// script that a standalone V8 JS engine (the BabylonNative Playground on the
// NativeDawn/WebGPU backend) can load with a plain XHR + eval -- no bundler, no
// HTTP server, no ESM/code-splitting.
//
// The official Babylon-Lite build (`pnpm build:bundle-scenes`) emits ESM with
// code-splitting, which the Playground's classic loader can't consume. This
// script re-bundles the same scene entry (e.g. lab/lite/src/lite/scene1.ts)
// against the prebuilt library (packages/babylon-lite/build/lib) as a single
// minified IIFE, inlining every dynamic import.
//
// Prerequisites (run in the Babylon-Lite clone first):
//   pnpm install
//   pnpm build:lib          # produces packages/babylon-lite/build/lib
//   pnpm add -D -w esbuild   # make esbuild resolvable
//
// Usage (cwd = Babylon-Lite clone, so `esbuild` resolves from its node_modules):
//   pnpm exec node <path>/bundle-lite-scene.mjs <cloneDir> <scene> <outFile>
//   e.g. pnpm exec node .../bundle-lite-scene.mjs . scene1 ./scene1.playground.js
import { resolve } from "node:path";
import { existsSync } from "node:fs";
import { createRequire } from "node:module";
import { pathToFileURL } from "node:url";

const cloneDir = resolve(process.argv[2] || process.cwd());
const scene = process.argv[3] || "scene1";
const outFile = resolve(process.argv[4] || resolve(cloneDir, `${scene}.playground.js`));

// Resolve esbuild from the Babylon-Lite clone's node_modules (this script lives
// in the BabylonNative repo, so a bare `import "esbuild"` would resolve against
// the wrong tree). `pnpm add -D -w esbuild` in the clone makes it a direct dep.
const requireFromClone = createRequire(resolve(cloneDir, "package.json"));
const esbuildEntry = requireFromClone.resolve("esbuild");
const { build } = await import(pathToFileURL(esbuildEntry));

const entryCandidates = [
    resolve(cloneDir, `lab/lite/src/lite/${scene}.ts`),
    resolve(cloneDir, `lab/src/lite/${scene}.ts`),
];
const entry = entryCandidates.find(existsSync);
if (!entry) {
    console.error("[bundle-lite-scene] scene entry not found. Tried:\n  " + entryCandidates.join("\n  "));
    process.exit(1);
}

const libDir = resolve(cloneDir, "packages/babylon-lite/build/lib");
if (!existsSync(libDir)) {
    console.error("[bundle-lite-scene] prebuilt lib missing: " + libDir + "\n  Run `pnpm build:lib` first.");
    process.exit(1);
}

console.log("[bundle-lite-scene] entry : " + entry);
console.log("[bundle-lite-scene] lib   : " + libDir);
console.log("[bundle-lite-scene] out   : " + outFile);

try {
    await build({
        entryPoints: [entry],
        bundle: true,
        splitting: false,        // single file: inline all dynamic imports
        format: "iife",          // classic script (no import/export)
        platform: "browser",
        target: "esnext",
        minify: true,
        keepNames: true,         // Babylon-Lite relies on Function.name
        legalComments: "none",
        sourcemap: false,
        outfile: outFile,
        alias: { "babylon-lite": libDir },
        loader: { ".wgsl": "text", ".glsl": "text" },
        logLevel: "info",
    });
    console.log("[bundle-lite-scene] wrote " + outFile);
} catch (err) {
    console.error("[bundle-lite-scene] build failed:", err);
    process.exit(1);
}
