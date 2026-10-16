# Fetch
Minimal implementation of the [WHATWG `fetch()`](https://fetch.spec.whatwg.org/) API. Like `XMLHttpRequest`, it is implemented on top of the platform-specific transports in the `UrlLib` dependency, so network behavior (libcurl / WinHTTP / etc.) is identical between the two polyfills.

```js
const response = await fetch("https://example.com/data.json");
if (response.ok) {
    const data = await response.json();
}
```

## Response
`fetch()` returns a `Promise` that resolves to a `Response`-like object exposing:
* `ok`, `status`, `statusText`, `url`, `redirected`, `type`, `bodyUsed`
* `headers` with `get(name)`, `has(name)`, and `forEach(callback)` (header names are matched case-insensitively)
* `text()`, `arrayBuffer()`, `json()`, `blob()` (each returns a `Promise`)
* `clone()`

The response body is fully buffered before the promise resolves. The body accessors may therefore be called more than once (`bodyUsed` is always reported as `false`), which is a deliberate, lenient deviation from the spec's single-use semantics.

`blob()` requires the `Blob` polyfill to be initialized; otherwise the returned promise rejects.

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

