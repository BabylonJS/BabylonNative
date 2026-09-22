# File

Implements the `File` and `FileReader` web APIs on top of the native `Blob`
polyfill provided by JsRuntimeHost. Babylon.js GLTF/OBJ serializer
round-trip codepaths construct `new File([blob], 'scene.glb')` and read it
back via `FileReader.readAsArrayBuffer(...)`, so the runtime needs both
constructors for those tests (and any consumer code that wraps serializer
output) to work.

## Behaviour

* No-op when the runtime already exposes a global `File` / `FileReader`
  (e.g. V8 in some embeddings).
* `File` is self-contained: the constructor delegates to the global
  `Blob` constructor to build the underlying byte storage, then decorates
  the instance with `name` and `lastModified`. Methods (`size`, `type`,
  `arrayBuffer`, `text`, `bytes`) forward to the inner `Blob`.
* `FileReader` supports `readAsArrayBuffer`, `readAsText`, and
  `readAsDataURL`, plus `abort`, `addEventListener` /
  `removeEventListener` / `dispatchEvent`, and the standard `onload` /
  `onerror` / `onloadstart` / `onloadend` / `onprogress` / `onabort`
  handler slots. `abort()` invalidates in-flight reads via a monotonic
  token so late-resolving `arrayBuffer()` promises cannot dispatch a
  phantom `load` event after a user-initiated abort.
* `File` extends `Blob`: the JS-visible prototype chain is wired so
  `new File(...) instanceof Blob === true`. Babylon.js core branches on
  `instanceof Blob` in several places (fileTools, Offline/database,
  abstractEngine, thinNativeEngine).

## Prerequisites

`Babylon::Polyfills::Blob::Initialize(env)` (from JsRuntimeHost) must be
called before `Babylon::Polyfills::File::Initialize(env)`; if `Blob` is
missing from the global object when `File::Initialize` runs, the `File`
constructor will not be registered.
