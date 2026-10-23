const fs = require("fs");
const path = require("path");
const vm = require("vm");
const ts = require("typescript");

const suiteRoot = path.resolve(process.env.TIMING_SUITE_ROOT);
const sourceRoot = path.join(suiteRoot, "packages", "tools", "tests", "test");
const sourceFile = path.join(sourceRoot, "playwright", "visualizationPlaywright.utils.ts");
let text = fs.readFileSync(sourceFile, "utf8");
const disposal = "window.engine.applyStates();";
if (text.split(disposal).length !== 2) {
    throw new Error("Expected exactly one shared disposal applyStates call");
}
// Match the measured fresh-engine run: NativeEngine has no applyStates method.
text = text.replace(disposal, "if (!globalThis.__babylonNativePlaywright) { window.engine.applyStates(); }");
const source = ts.createSourceFile(sourceFile, text, ts.ScriptTarget.Latest, true);
const modules = new Map();

function loadTypeScript(filename) {
    filename = path.resolve(filename);
    if (modules.has(filename)) {
        return modules.get(filename).exports;
    }
    const module = { exports: {} };
    modules.set(filename, module);
    const code = ts.transpileModule(fs.readFileSync(filename, "utf8"), {
        compilerOptions: { target: ts.ScriptTarget.ES2017, module: ts.ModuleKind.CommonJS },
        fileName: filename,
    }).outputText;
    const localRequire = name => name.startsWith(".")
        ? loadTypeScript(path.resolve(path.dirname(filename), name + ".ts"))
        : require(name);
    const compiled = vm.runInThisContext(`(function(exports, require, module, fetch) { ${code}\n})`, { filename });
    compiled(module.exports, localRequire, module,
        (url, options = {}) => fetch(url, { ...options, signal: AbortSignal.timeout(100_000) }));
    return module.exports;
}

const loaderRoot = path.join(suiteRoot, "packages", "tools", "snippetLoader", "src");
const loader = loadTypeScript(path.join(loaderRoot, "snippetLoader.ts"));
const fetcher = loadTypeScript(path.join(loaderRoot, "fetchSnippet.ts"));
const transpiler = loader.CreateTypeScriptTranspiler(ts);
const context = {
    exports: {},
    FetchSnippet: fetcher.FetchSnippet,
    ParseSnippetResponse: loader.ParseSnippetResponse,
    transpileSnippet: async (code, filename) => /\.tsx?$/i.test(filename) ? transpiler(code, filename) : code,
    console,
};
vm.createContext(context);
for (const name of [
    "stripModuleSyntax", "detectSceneCall", "preloadSnippetCode",
    "evaluateInitEngineForVisualization", "evaluatePrepareScene",
    "evaluateRenderSceneForVisualization", "evaluateDisposeSceneForVisualization",
]) {
    const matches = source.statements.filter(statement =>
        (ts.isFunctionDeclaration(statement) && statement.name?.getText(source) === name) ||
        (ts.isVariableStatement(statement) &&
            statement.declarationList.declarations.some(declaration => declaration.name.getText(source) === name)));
    if (matches.length !== 1) {
        throw new Error(`Shared function ${name} has ${matches.length} definitions`);
    }
    const code = ts.transpileModule(matches[0].getText(source), {
        compilerOptions: { target: ts.ScriptTarget.ES2017, module: ts.ModuleKind.CommonJS },
    }).outputText;
    vm.runInContext(code + (ts.isFunctionDeclaration(matches[0]) ? `\nexports.${name} = ${name};` : ""), context);
}

module.exports = {
    ...context.exports,
    NativePage: loadTypeScript(path.join(sourceRoot, "playwright", "nativePage.ts")).NativePage,
    sourceRoot,
    config: JSON.parse(fs.readFileSync(path.join(sourceRoot, "visualization", "config.json"), "utf8").replace(/^\uFEFF/, "")),
};
