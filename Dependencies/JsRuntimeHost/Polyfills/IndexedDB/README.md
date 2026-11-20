# IndexedDB

`Babylon::Polyfills::IndexedDB::Initialize` installs the standard IndexedDB
globals when the selected JavaScript engine does not already provide them.
The implementation is backed by memory for the lifetime of the JavaScript
runtime; it does not persist data across runtime or process restarts.

The embedded implementation is
[`fake-indexeddb` 6.2.5](https://github.com/dumbmatter/fakeIndexedDB/tree/v6.2.5),
licensed under Apache-2.0. That release passes 82.8% of the applicable
IndexedDB Web Platform Tests and supplies object stores, indexes, cursors,
key ranges, key paths, generated keys, transaction rollback, blocked upgrades,
and version-change events.

JsRuntimeHost supplies a private storage-clone fallback to the embedded
implementation because bare JavaScript engines do not normally expose the
browser `structuredClone` global. It preserves cyclic graphs, shared
references, typed-array backing buffers, dates, regular expressions, maps,
sets, and errors. It is used only by IndexedDB and does not install a partial
public `structuredClone`. A standards-shaped `DOMException` constructor is
installed only when the engine does not already provide one, so IndexedDB
errors remain testable with normal browser code.

Applications that require durable storage should install a host-backed
IndexedDB implementation before calling `Initialize`; an existing
`globalThis.indexedDB` is preserved.

## Updating the embedded implementation

From this directory:

```sh
npm install --no-save esbuild@0.28.1 fake-indexeddb@6.2.5
npx esbuild ThirdParty/fake-indexeddb/BuildEntry.js \
  --bundle --format=iife --target=es2017 --minify --legal-comments=none \
  --inject:ThirdParty/fake-indexeddb/StorageClone.js \
  --banner:js='/* fake-indexeddb 6.2.5 | Apache-2.0 | JsRuntimeHost storage-clone adapter */' \
  --outfile=ThirdParty/fake-indexeddb/fake-indexeddb.js
```

Keep the upstream Apache-2.0 `LICENSE` beside the generated bundle.
