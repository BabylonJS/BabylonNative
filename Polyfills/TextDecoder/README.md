# TextDecoder Polyfill

A C++ implementation of the [WHATWG Encoding API](https://encoding.spec.whatwg.org/) `TextDecoder` interface for use in Babylon Native JavaScript runtimes via [Napi](https://github.com/nodejs/node-addon-api).

## Current State

### Supported

- Decoding `Uint8Array`, `Int8Array`, and other typed array views from a UTF-8 encoded byte sequence.
- Decoding raw `ArrayBuffer` objects.
- Constructing `TextDecoder` with no argument (defaults to `utf-8`).
- Constructing `TextDecoder` with the explicit encoding label `"utf-8"` or `"UTF-8"`.
- Calling `decode()` with no argument or `undefined` returns an empty string (matches the Web API).

### Not Supported

- Encodings other than UTF-8 — passing any other label (e.g. `"utf-16"`, `"iso-8859-1"`) throws a JavaScript `Error`.
- `DataView` is not accepted by `decode()` — due to missing `Napi::DataView` support in the underlying JSI layer.
- Passing a non-BufferSource value (e.g. a string or number) to `decode()` throws a `TypeError`.
- The `fatal` option: decoding errors are not detected and do not throw a `TypeError`.
- The `ignoreBOM` option: the byte order mark is not stripped.
- Streaming decode (passing `{ stream: true }` to `decode()`) — each call is stateless.
- The `encoding` property on the `TextDecoder` instance is not exposed.

## Usage

```javascript
const decoder = new TextDecoder();              // utf-8
const decoder = new TextDecoder("utf-8");       // explicit, also fine

const bytes = new Uint8Array([72, 101, 108, 108, 111]);
decoder.decode(bytes); // "Hello"
```

Passing an unsupported encoding throws:

```javascript
new TextDecoder("utf-16"); // Error: TextDecoder: unsupported encoding 'utf-16', only 'utf-8' is supported
```
