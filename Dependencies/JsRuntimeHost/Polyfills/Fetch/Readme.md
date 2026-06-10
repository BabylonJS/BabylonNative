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
