# Web Streams

Installs the standard `ReadableStream`, `WritableStream`, `TransformStream`,
reader, writer, controller, and queuing-strategy globals when the selected
JavaScript engine does not provide them.

The implementation is the ES5 ponyfill bundle from
[`web-streams-polyfill` 4.3.0](https://github.com/MattiasBuelens/web-streams-polyfill/tree/add69059ed08eae6a18559aba49575a280d1529e),
which is based on the WHATWG reference implementation. The vendored project
tests this release against the Streams portion of WPT at revision
[`c05b4473`](https://github.com/web-platform-tests/wpt/tree/c05b447326585237713013c66341eab2cdf967b6/streams).

`Initialize` preserves any Streams constructors already supplied by the host
engine and fills only missing globals.
