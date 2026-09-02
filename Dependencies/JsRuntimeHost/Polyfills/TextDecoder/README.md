# TextDecoder Polyfill

A C++ implementation of the [WHATWG Encoding API](https://encoding.spec.whatwg.org/) `TextDecoder` interface for use in Babylon Native JavaScript runtimes via [Napi](https://github.com/nodejs/node-addon-api).

## Current State

### Supported

- Decoding `Uint8Array`, `Int8Array`, and other typed array views from a UTF-8 or UTF-16 encoded byte sequence.
- Decoding raw `ArrayBuffer` objects.
- Constructing `TextDecoder` with no argument (defaults to `utf-8`).
- Constructing `TextDecoder` with any WHATWG label for UTF-8 (`"utf-8"`, `"utf8"`, `"unicode-1-1-utf-8"`, `"unicode11utf8"`, `"unicode20utf8"`, `"x-unicode20utf8"`), UTF-16LE (`"utf-16"`, `"utf-16le"`, `"ucs-2"`, `"unicode"`, `"unicodeFEFF"`, `"csunicode"`, `"iso-10646-ucs-2"`) or UTF-16BE (`"utf-16be"`, `"unicodeFFFE"`). Labels are matched case-insensitively and ignore surrounding whitespace.
- Stripping a leading byte order mark when decoding UTF-16.
- Calling `decode()` with no argument or `undefined` returns an empty string (matches the Web API).

### Not Supported

- Encodings other than UTF-8 and UTF-16 — passing any other label (e.g. `"iso-8859-1"`) throws a JavaScript `Error`.
- `DataView` is not accepted by `decode()` — due to missing `Napi::DataView` support in the underlying JSI layer.
- Passing a non-BufferSource value (e.g. a string or number) to `decode()` throws a `TypeError`.
- The `fatal` option: decoding errors are not detected and do not throw a `TypeError`. A trailing odd byte in a UTF-16 sequence is dropped rather than decoded as U+FFFD.
- The `ignoreBOM` option: a leading UTF-16 byte order mark is always stripped and cannot be retained. A UTF-8 byte order mark is never stripped.
- Streaming decode (passing `{ stream: true }` to `decode()`) — each call is stateless.
- The `encoding` property on the `TextDecoder` instance is not exposed.

## Usage

```javascript
const decoder = new TextDecoder();              // utf-8
const decoder = new TextDecoder("utf-8");       // explicit, also fine

const bytes = new Uint8Array([72, 101, 108, 108, 111]);
decoder.decode(bytes); // "Hello"

new TextDecoder("utf-16le").decode(new Uint8Array([0x48, 0x00, 0x69, 0x00])); // "Hi"
```

Passing an unsupported encoding throws:

```javascript
new TextDecoder("iso-8859-1"); // Error: TextDecoder: unsupported encoding 'iso-8859-1', only UTF-8 and UTF-16 are supported
```
