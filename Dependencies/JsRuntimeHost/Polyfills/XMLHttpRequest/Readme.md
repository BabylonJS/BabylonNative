# XMLHttpRequest
Minimal implementation of XMLHttpRequest required to support the Babylon.js RequestFile method. Under the hood, XMLHttpRequest is implemented using various platform-specific APIs in the UrlLib dependency.

## Events
Events can be listened to with `addEventListener` or with the `on<event>` handler properties
(`onload`, `onerror`, ...). A handler receives an event object with `type`, `target`, and the
`ProgressEvent` members `loaded`, `total` and `lengthComputable`.

A request raises the events of the [XMLHttpRequest standard](https://xhr.spec.whatwg.org/) in its order:
* `readystatechange` when `open()` moves the request to `OPENED`
* `loadstart` from `send()`
* for a response: `readystatechange` (`HEADERS_RECEIVED`), `readystatechange` (`LOADING`, when the
  response has a body), `progress`, `readystatechange` (`DONE`), `load`, `loadend`
* for a transport failure: `readystatechange` (`DONE`), `error`, `loadend`
* for `abort()` while a request is under way: `readystatechange` (`DONE`), `abort`, `loadend`,
  raised before `abort()` returns; the request is then `UNSENT`
* for an expired `timeout`: `readystatechange` (`DONE`), `timeout`, `loadend`

A response with an HTTP error status such as `404` or `500` is still a response: it ends with
`load`, and `status` tells the outcome. `error` is raised only when there is no response (DNS
failure, refused connection, TLS failure, missing local file, ...), and `status` is then `0`.

The response is delivered in one piece, so `HEADERS_RECEIVED`, `LOADING` and a single `progress`
event are raised back to back when it has arrived, not while it downloads.

Listeners and `on<event>` handlers are released when a request has raised `loadend`. Add them
again before reusing an `XMLHttpRequest` for another request.

## Response
* `status` and `statusText` are `0` and `""` until there is a response, and after a transport
  failure or `abort()`. `statusText` is the reason phrase the server sent over HTTP/1.x where the
  platform reports it (Linux, Windows), `""` for a response over HTTP/2 or HTTP/3 (which have no
  reason phrase), and otherwise the standard phrase for the status code (Apple and Android over
  HTTP/1.x, and local files).
* `responseURL` is the URL of the response after redirects, without a fragment.
* `getAllResponseHeaders()` returns a string with one `name: value\r\n` line per header: names
  lower-cased, sorted, and the values of a repeated header combined with `, `.
  `getResponseHeader(name)` matches the name case-insensitively and returns `null` for a header
  that is not there.

## Local files
Unlike the web, XMLHttpRequest supports loading local files using two schemes:
* `file:///` allows you to load from an absolute path
* `app:///` allows you to load from a relative path, either the current program or package depending on platform

## Other things to be aware of:
* Only `GET` requests are currently supported
* Requests are always asynchronous
* `responseType` supports `text` and `arraybuffer`
* `timeout` relies on the host providing `setTimeout`
* There is no `upload` object, `responseXML`, or `overrideMimeType()`

## Transport-error diagnostics (non-standard)
A transport-level failure surfaces the standard way -- an `error` event followed by `loadend`,
with `status === 0` -- exactly as on the web. In addition, two **non-standard, additive**
read-only properties expose the normalized `UrlLib` transport-error detail so BN-aware code can
tell a DNS failure from a refused connection or a missing local asset:
* `errorCode` -- the stable symbolic token (e.g. `"CURLE_COULDNT_CONNECT"`, `"NSURLErrorTimedOut"`,
  `"AppResourceNotFound"`)
* `errorDetail` -- the full normalized `"<domain>:<symbol>(<code>): <detail>"` string

Both are empty strings unless the request failed at the transport layer, and are populated only
on backends that expose the detail (Apple, Linux) -- empty on Windows/Android until those
backends populate `UrlLib`'s accessors. Browsers do not expose these properties, so
spec-conformant code is unaffected.
