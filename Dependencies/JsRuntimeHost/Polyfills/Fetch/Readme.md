# Fetch
Implementation of the [WHATWG `fetch()`](https://fetch.spec.whatwg.org/) API.
Like `XMLHttpRequest`, it uses the platform-specific transport from `UrlLib`, so
network behavior is shared between the two polyfills.

```js
const response = await fetch("https://example.com/data.json");
if (response.ok) {
    const data = await response.json();
}
```

## Headers and Response
The module installs `Headers` and `Response` when the host does not already
provide them. Response bodies use a `ReadableStream` and follow the browser's
single-consumption behavior through `bodyUsed`. The supported body readers are
`arrayBuffer()`, `blob()`, `bytes()`, `json()`, and `text()`.

Initialize the Streams, Blob, TextEncoder, TextDecoder, and URL polyfills before
using body-bearing responses on engines that do not provide those globals.

A completed native response is copied once into JavaScript-owned memory and
exposed as a byte stream. Body readers retain chunk references while consuming
a stream and allocate a contiguous result at most once when that result requires
one. Repeated header iteration reuses a mutation-versioned normalized view, and
header deletion and replacement compact the field list in place.

The focused conformance tests are adapted from WPT `fetch/api/headers` and
`fetch/api/response`, plus WebKit, Firefox, and Chromium response-body
regressions.

## Local files
Like `XMLHttpRequest`, `fetch()` supports loading local resources:
* `file:///` loads from an absolute path
* `app:///` loads from a path relative to the current program or package depending on platform

## Other things to be aware of
* Only `GET` and `POST` methods are supported (a `UrlLib` limitation shared with `XMLHttpRequest`).
* Only string request bodies are supported.
* Consistent with the fetch spec, the promise rejects only on transport-level failures. A completed request with a non-`2xx` status (e.g. `404`) still resolves, with `response.ok === false`.

## Transport-failure rejections
On a transport-level failure (DNS failure, connection refused, TLS failure, missing local
asset, ...) the promise rejects with a **`TypeError`** whose `message` is the stable string
`"fetch failed"`. The message is intentionally constant so crash-report grouping stays intact;
the variable detail is carried on `error.cause` (the Node/undici shape), never spread across the
message:

```js
try {
    await fetch("https://does-not-resolve.invalid/");
} catch (error) {
    error.message;        // "fetch failed" (stable)
    error.cause.code;     // stable token, e.g. "CURLE_COULDNT_RESOLVE_HOST" (where available)
    error.cause.detail;   // full normalized UrlLib string (where available)
    error.cause.url;      // the requested URL
    error.cause.status;   // 0 for a transport failure
}
```

`error.cause.code` / `error.cause.detail` come from `UrlLib`'s normalized transport-error
accessors and are present on the backends that populate them (Apple, Linux); on backends that do
not yet (Windows, Android) they are omitted while the standard observable shape (a `TypeError`
with the stable message, plus `cause.url` / `cause.status`) is preserved. This is a deliberate,
strictly-additive superset of the spec: spec-conformant code only sees a `TypeError`, exactly as
in a browser, while BN-aware diagnostic code can read `cause` to distinguish a DNS failure from a
refused connection or a missing local asset.

The rejection's `stack` is captured synchronously at the `fetch()` call site (before the request
is handed to a worker thread), so crash reports can attribute the failing call rather than an
empty scheduler tick. (Engines that only materialize `.stack` when an error is thrown may omit
the frames.)
