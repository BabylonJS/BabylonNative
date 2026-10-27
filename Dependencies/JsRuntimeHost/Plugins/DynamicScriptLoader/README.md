# Dynamic script loader

The optional C++ `DynamicScriptLoader` plugin installs `_native.loadScript(name)`, which returns a promise that settles after evaluating source in the current JavaScript context. The host supplies a resolver that returns either a JavaScript string or a promise of a string. Missing resources (null or undefined), rejected lookups, and evaluation errors reject the returned promise. No file or network loader is installed by JsRuntimeHost.

```cpp
#include <Babylon/Plugins/DynamicScriptLoader.h>

Babylon::Plugins::DynamicScriptLoader::Initialize(env,
    [](Napi::Env env, const std::string& name) -> Napi::Value {
        auto source = FindPackagedScript(name);
        return source ? Napi::String::New(env, *source) : env.Null();
    });
```

Initialize the plugin after `JsRuntime` and before evaluating the entry bundle. Link the `DynamicScriptLoader` target; `JSRUNTIMEHOST_PLUGIN_DYNAMICSCRIPTLOADER` controls whether it is built. It uses the existing `_native` object and does not install global `loadScript`, `importScripts`, or native `import()` syntax. Names must be nonempty strings without NUL characters; source containing NUL characters is rejected because the evaluation API accepts null-terminated strings. Empty source is allowed.

The resolver runs on the JavaScript thread. For asynchronous resolvers, return `Napi::Promise::Deferred::Promise()` and resolve it with source on a later JavaScript dispatch turn, or reject it with the lookup error. The host must stop asynchronous producers before destroying the runtime; it must not access Node-API values or dispatch through a destroyed runtime. The loader does not cache scripts or cancel host operations.

## Bundled imports

The runtime plugin is bundler-independent: it evaluates classic scripts, not untransformed ECMAScript modules. A bundler transforms source-level `import()` and manages exports, dependencies, caching, and concurrent imports. Its chunk transport must call `_native.loadScript` instead of a browser or filesystem loader.

### Webpack 5 example

The following `webpack.config.cjs` is an application-side example, not a separate JsRuntimeHost build package. It keeps Webpack's chunk registration and adds the host evaluation promise to the promises each import already waits for. JsRuntimeHost supplies `window` as an alias for the script global; no document is required.

```js
module.exports = {
    entry: "./src/index.js",
    target: ["web", "es5"],
    mode: "production",
    devtool: false,
    output: {
        filename: "entry.js",
        chunkFilename: "[name].js",
        chunkFormat: "array-push",
        chunkLoading: "jsonp",
        globalObject: "window",
        publicPath: "",
    },
    plugins: [{
        apply(compiler) {
            const { RuntimeGlobals, RuntimeModule } = compiler.webpack;
            compiler.hooks.thisCompilation.tap("HostScriptLoading", compilation => {
                compilation.hooks.runtimeModule.tap("HostScriptLoading", module => {
                    if (module.name === "load script") {
                        module.generate = () => "";
                    }
                });
                compilation.hooks.runtimeRequirementInTree.for(RuntimeGlobals.loadScript)
                    .tap("HostScriptLoading", chunk => {
                        const runtime = new RuntimeModule("host script loading", RuntimeModule.STAGE_ATTACH + 1);
                        runtime.generate = () => `
var loads = Object.create(null);
${RuntimeGlobals.loadScript} = function(name, done) {
    var load = loads[name] = Promise.resolve().then(function() {
        return _native.loadScript(name);
    });
    load.then(function() {
        done({ type: "load", target: { src: name } });
    }, function() {
        done({ type: "error", target: { src: name } });
    });
};
var ensureChunk = ${RuntimeGlobals.ensureChunkHandlers}.j;
${RuntimeGlobals.ensureChunkHandlers}.j = function(chunkId, promises) {
    ensureChunk(chunkId, promises);
    var load = loads[${RuntimeGlobals.publicPath} + ${RuntimeGlobals.getChunkScriptFilename}(chunkId)];
    if (load) {
        promises.push(load);
    }
};`;
                        compilation.addRuntimeModule(chunk, runtime);
                    });
            });
        },
    }],
};
```

Initialize the runtime plugin before evaluating `entry.js`. Package every emitted JavaScript chunk under its emitted relative name; the host resolver decides which names are accepted. For browser builds, omit the example's plugin and use the normal browser output settings. Other bundlers need their own chunk-transport integration.

The import promise waits for both chunk registration and complete script evaluation. Lookup and evaluation failures reject it; successful evaluation without the expected registration rejects with Webpack's `ChunkLoadError`. Failed lookups can be retried. Evaluation failure after registration is not rolled back; discard the runtime if that failure leaves application state unusable.

This example uses classic JavaScript chunks only. Do not combine it with hot module replacement, a development server, browser prefetch/preload hints, CSS chunk loading, or native-module externals. The host must complete or reject each resource operation; no timeout is added. Select source transpilation appropriate for the engine: Webpack's `target` controls its generated runtime, not all syntax in application source.
