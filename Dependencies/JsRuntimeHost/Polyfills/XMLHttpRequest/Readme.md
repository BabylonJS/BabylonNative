# XMLHttpRequest
Minimal implementation of XMLHttpRequest required to support the Babylon.js RequestFile method. Under the hood, XMLHttpRequest is implemented using various platform-specific APIs in the UrlLib dependency.

## Event listening
We do not support `onload`-style event listeners. Instead, you should listen to events using `addEventListener`. At the moment, we only support the following events:
* `loadend`
* `readystatechange`
* `error` (fired on a transport failure or a non-`2xx` HTTP response, before `loadend`)

## Local files
Unlike the web, XMLHttpRequest supports loading local files using two schemes:
* `file:///` allows you to load from an absolute path
* `app:///` allows you to load from a relative path, either the current program or package depending on platform

## Other things to be aware of:
* Only `GET` requests are currently supported
* For `readyState`, we only support `UNSENT`, `OPENED`, and `DONE`

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
