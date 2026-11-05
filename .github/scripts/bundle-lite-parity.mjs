import { createRequire } from "node:module";
import { existsSync } from "node:fs";
import { mkdir, readFile, writeFile } from "node:fs/promises";
import { dirname, resolve } from "node:path";
import { pathToFileURL } from "node:url";

const clone = resolve(process.argv[2] ?? "build\\lite-parity");
const output = resolve(process.argv[3] ?? "build\\dawn\\Apps\\Playground\\Scripts\\lite-parity");
const require = createRequire(resolve(clone, "package.json"));
const { build } = await import(pathToFileURL(require.resolve("esbuild")));
const catalog = JSON.parse(await readFile(resolve(clone, "scene-config.json"), "utf8"));
if (!Array.isArray(catalog) || new Set(catalog.map(scene => scene.id)).size !== catalog.length) {
    throw new Error("Expected a unique scene array in scene-config.json");
}
const library = resolve(clone, "packages\\babylon-lite\\build\\lib");
if (!existsSync(resolve(library, "index.js"))) {
    throw new Error("Run Babylon-Lite build:lib before bundling parity scenes");
}
await mkdir(output, { recursive: true });
const manifest = [];
for (const scene of catalog) {
    const entry = resolve(clone, `lab\\lite\\src\\lite\\scene${scene.id}.ts`);
    const file = resolve(output, `scene${scene.id}.js`);
    try {
        const result = await build({
            entryPoints: [entry],
            bundle: true,
            splitting: false,
            format: "iife",
            platform: "browser",
            target: "esnext",
            minify: false,
            keepNames: true,
            legalComments: "none",
            sourcemap: "inline",
            outfile: file,
            metafile: true,
            alias: { "babylon-lite": library },
            define: { "import.meta.url": "globalThis.__liteParityModuleUrl" },
            loader: { ".wgsl": "text", ".glsl": "text", ".wasm": "file", ".ttf": "file" },
            assetNames: "assets/[name]-[hash]",
            plugins: [{
                name: "vite-asset-queries",
                setup(builder) {
                    builder.onResolve({ filter: /\?(raw|url)$/ }, args => ({
                        path: resolve(args.resolveDir, args.path.replace(/\?(raw|url)$/, "")),
                        namespace: args.path.endsWith("?raw") ? "raw-asset" : "file",
                    }));
                    builder.onLoad({ filter: /.*/, namespace: "raw-asset" }, async args => ({
                        contents: await readFile(args.path, "utf8"),
                        loader: "text",
                    }));
                },
            }],
            logLevel: "silent",
        });
        await writeFile(resolve(output, `scene${scene.id}.meta.json`), JSON.stringify(result.metafile));
        manifest.push({ ...scene, entry, bundle: file, build: "PASS" });
    } catch (error) {
        const message = error.errors?.map(item => `${item.location?.file ?? entry}: ${item.text}`).join("\n") ?? String(error);
        await writeFile(resolve(output, `scene${scene.id}.build.log`), message);
        manifest.push({ ...scene, entry, bundle: file, build: "FAIL", buildError: message });
    }
    await writeFile(resolve(output, "manifest.json"), JSON.stringify(manifest, null, 2));
    console.log(`scene${scene.id}: ${manifest.at(-1).build}`);
}
console.log(`Bundled ${manifest.filter(scene => scene.build === "PASS").length}/${catalog.length} scenes into ${dirname(resolve(output, "manifest.json"))}`);
