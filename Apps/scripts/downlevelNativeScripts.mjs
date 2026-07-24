#!/usr/bin/env node

import { readFile, writeFile } from "fs/promises";
import { resolve } from "path";
import ts from "typescript";

const targets = process.argv.slice(2);

if (targets.length === 0) {
    process.stderr.write("Usage: node scripts/downlevelNativeScripts.mjs <file> [...]\n");
    process.exit(1);
}

// Babylon.js emits its UMD bundles at an ES2015 target: the TC39 decorator migration relies on the
// `accessor` keyword, which cannot be expressed in ES5, and esbuild cannot emit ES5 classes. Babylon
// Native runs those bundles on Chakra, which consumes ES5-level script, so the core bundle must be
// down-leveled before it is executed. Babylon.js does the same thing to the artifacts this repo
// publishes (see `downlevel:native-scripts` in the Babylon.js monorepo pipeline); without it the
// bundle still parses and most of the engine works, so the breakage is silent - e.g. GlowLayer never
// reports ready and any scene using it hangs in Scene.executeWhenReady.
//
// TypeScript is used for the transform rather than Babel. Babel's ES5 class lowering emits
// `Reflect.construct` / `_wrapNativeSuper` machinery for classes that extend native built-ins such as
// `Error`, and that machinery runs at class-definition time and hard-crashes Chakra while the bundle
// loads. TypeScript lowers classes with its `__extends` helper - plain prototype assignment, no
// `Reflect.construct` - which is the emit Babylon Native consumed for years when the bundles were
// built at an ES5 target directly. `ts.transpileModule` is a purely syntactic single-file transform,
// so it handles the multi-megabyte bundle without type checking and inlines its own helpers.
const compilerOptions = {
    target: ts.ScriptTarget.ES5,
    // The bundles are UMD/IIFE scripts with no top-level module syntax; leave module output untouched.
    module: ts.ModuleKind.None,
    // Lower `for..of`, spread and other iterator protocols correctly for ES5.
    downlevelIteration: true,
    // Inline the emit helpers so the bundle stays self-contained on Chakra.
    importHelpers: false,
    newLine: ts.NewLineKind.LineFeed,
    sourceMap: false,
    ignoreDeprecations: "6.0",
};

let count = 0;

for (const target of targets) {
    const filePath = resolve(target);
    const source = await readFile(filePath, "utf8");
    const { outputText } = ts.transpileModule(source, { compilerOptions, fileName: filePath });
    await writeFile(filePath, outputText, "utf8");
    process.stdout.write(`Downleveled ${filePath}\n`);
    ++count;
}

process.stdout.write(`Downleveled ${count} Babylon Native script file(s).\n`);
