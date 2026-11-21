# Compression streams

This optional polyfill installs the browser `CompressionStream` and
`DecompressionStream` interfaces when the JavaScript host does not already
provide them. It supports the interoperable `deflate`, `deflate-raw`, and
`gzip` formats using zlib and the WHATWG Streams API.

The built-in format list is the same on Apple, Android, Windows, and Linux.
Apple links SDK libz. Other builds select an existing zlib target, a system
package, or the pinned source fallback. Brotli, Zstandard, and XZ are not
implemented by this polyfill. ZIP is an archive container, not a stream
format: gzip-compressed SPZ data can be decoded here, but unpacking SOG ZIP
entries and loading the SPZ ES module are separate operations. The transform
also works on the main runtime; synchronous native codec work on a large chunk
can still occupy the calling JS thread.

Input `BufferSource` memory is borrowed only for the duration of a synchronous
codec call. Output is accumulated before invoking JavaScript so an enqueue
callback cannot invalidate an input buffer still in use. A native 64 KiB
scratch buffer is reused for the lifetime of each active stream; each emitted
`Uint8Array` receives one exact-size copy into JavaScript-owned memory.

The module uses an existing CMake zlib target or the platform zlib package when
available. It fetches pinned zlib 1.3.1 only when no zlib package is present.
The zlib license is included under `ThirdParty/zlib`.
