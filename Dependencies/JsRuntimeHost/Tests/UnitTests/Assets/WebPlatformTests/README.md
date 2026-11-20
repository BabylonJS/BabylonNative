# Focused Worker conformance regressions

`UPSTREAM_REVISION` records the exact
[web-platform-tests/wpt](https://github.com/web-platform-tests/wpt) commit used
when the broader suite is run outside this repository. The WPT checkout and
`testharness.js` are deliberately not vendored.

`workers/focused-api.js` is a small host-native port of eight assertions that
found real gaps in the initial implementation: worker-global identity and
readonly behavior, EventTarget removal/targeting, `onmessage` normalization and
dispatch, `postMessage()`'s return value, and zero-argument `importScripts()`.
The source links are kept in that file. `workers/support/Worker-structure-message.js`
and `workers/constructors/Worker/terminate.js` are two additional focused WPT
ports for structured transfer and termination.

The remaining cases are regressions adapted from browser-engine fixes; each
test carries the exact upstream link:

- `workers/support/WorkerGlobalScope-close.js` checks that `close()` preserves
  same-task messages/errors while discarding later tasks.
- `workers/support/Worker-early-message.js` checks the startup queue and the
  complementary terminate-before-start cleanup path.
- `workers/support/Worker-run-forever.js` checks JSC interruption during
  top-level evaluation, based on WPT's terminate-during-evaluation regression.
- `workers/support/Worker-termination-stress.js` repeats shutdown with an
  inbound task pending, guarding the cross-thread destruction pattern fixed by
  WebKit in July 2026.
- `runner.js` checks that a throwing structured-clone getter propagates the
  original exception and leaves transferables attached, guarding Servo's 2025
  exception-clearing regression.
- `workers/support/visualization-worker-smoke.js` is a small, non-proprietary
  reproduction of the deployed rebeckerspecialties visualization worker's
  startup contract. It constructs a named module-compatible worker from a
  WHATWG `URL`, opens the IndexedDB cache used by `pipelineCreate`, queues the
  interop startup messages, and sends multiple `Date`/`Map`/`Set`-rich
  playback streams through structured clone.

`runner.js` launches these focused tests directly, without WPT infrastructure,
and adapts the document-side structured-clone, transfer, startup, close, and
termination checks to the JsRuntimeHost unit-test host. The infinite-evaluation
case runs only when the selected engine has a native execution-interrupt hook.
The visualization smoke case runs last so it exercises a fresh worker after
the lifecycle stress cases.
