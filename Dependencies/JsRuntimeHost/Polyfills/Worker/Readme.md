# Worker

Provides a browser-compatible dedicated `Worker` backed by one `AppRuntime`
(and therefore one engine realm and native thread) per Worker.

Enable `JSRUNTIMEHOST_POLYFILL_WORKER`, link the host with `Worker`, and call
`Babylon::Polyfills::Worker::Initialize(env, options)` before loading application
scripts. Set `options.ScriptRoot` to the packaged bundle root, or supply a
thread-safe resolver. No renderer dependency or application-JS shim is needed.

Implemented surface:

- `new Worker(url, { name, type })`, `postMessage()`, and `terminate()`
- `WorkerGlobalScope` / `DedicatedWorkerGlobalScope`, `self`, `location`,
  `close()`, synchronous `importScripts()`, and worker `postMessage()`
- `EventTarget`, `Event`, `MessageEvent`, `ErrorEvent`, `DOMException`,
  `onmessage`, `onmessageerror`, and `onerror`
- worker-relative fetch URLs through the existing Fetch polyfill
- structured cloning for cyclic objects, arrays, dates, regular expressions,
  maps, sets, errors, ArrayBuffers, DataViews, typed arrays, BigInts, and
  special number values
- transferable ArrayBuffers using the N-API v7 detach operation

The default loader reads relative paths and `app:///` URLs below
`Options::ScriptRoot`. Native applications can instead supply a thread-safe
`ScriptResolver` for packaged assets.

WHATWG `URL` serializes a hostless `app:///worker.js` URL as
`app:/worker.js`; both spellings resolve through `ScriptRoot`. Worker
`location` exposes the URL fields application bundles normally inspect
(`protocol`, `origin`, `pathname`, and related fields), not only `href`.
Native JavaScriptCore class objects are normalized inside the Worker realm so
browser-style constructor feature checks such as
`typeof AbortController === "function"` behave as expected.

Embedders can install optional APIs (such as streams or compression) through
`Options::InitializeRealm`. This runs once on the worker thread before its
entry script. Worker does not link those libraries.

`type: "module"` accepts self-contained, script-compatible application bundles.
JavaScriptCore's public C API has no module-loader hook, so the application's
normal bundler must flatten top-level `import` and `export` declarations before
loading. Classic workers and bundled module workers share the same isolated
runtime.

Apple distribution builds use public JavaScriptCore and terminate cooperatively
between dispatches. They cannot interrupt a non-yielding loop. The opt-in
`JSRUNTIMEHOST_JSC_PRIVATE_WATCHDOG` diagnostic uses private SPI and must not
be enabled in distribution builds. WebKitGTK may provide its own watchdog.

`WorkerGlobalScope.close()` stops future worker tasks but preserves messages
and uncaught errors produced later in the task that called it, as required by
WPT. Worker-owned runtime state is torn down on the worker thread; the final
strong reference to the parent-realm `Worker` object is released by a FIFO
dispatch on the parent runtime after worker engine/environment teardown.
