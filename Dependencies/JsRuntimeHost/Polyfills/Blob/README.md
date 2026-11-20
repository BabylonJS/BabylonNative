# Blob

Provides the browser `Blob` constructor, byte/text readers, zero-copy Blob
composition and slicing, and a lazily pulled byte `ReadableStream`.

Call `Babylon::Polyfills::Streams::Initialize` before using `Blob.stream()` on
engines that do not provide Web Streams. Stream reads copy only the requested
chunk into JavaScript-owned memory; composing Blobs and slicing share immutable
native byte segments.

The focused tests are adapted from WPT `FileAPI/blob`, WebKit's Blob stream
chunk/crash regressions, and Firefox's large Blob `pipeTo` regression.
