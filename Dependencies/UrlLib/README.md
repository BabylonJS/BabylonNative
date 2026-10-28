# UrlLib

UrlLib is a cross-platform C++ library that utilizes platform-specific implementations
for URL-related functionality. Although it was created as a subcomponent of
[JsRuntimeHost](../JsRuntimeHost) for its polyfills, it may also be used standalone.

## Error reporting

A request that fails at the transport layer (DNS failure, connection refused, TLS failure,
missing local file, ...) completes with a `StatusCode()` of 0. To make those failures
diagnosable from logs and crash reports, `UrlRequest` additionally exposes:

* `ErrorString()` — the full normalized error, shaped for log-pipeline filtering:
  `"<domain>:<symbol>(<code>): <detail>"`
* `ErrorSymbol()` — the stable symbolic token alone (e.g. `CURLE_COULDNT_RESOLVE_HOST`,
  `NSURLErrorTimedOut`)
* `ErrorCode()` — the raw numeric platform code (CURLcode, NSError code, ...)

`ErrorString()` and `ErrorSymbol()` are empty (and `ErrorCode()` is 0) when the request did
not fail at the transport layer; an HTTP error status such as 404 is **not** a transport
failure. To detect a transport failure, test whether `ErrorString()` (or `ErrorSymbol()`) is
non-empty rather than checking `ErrorCode() != 0`: a genuine failure can carry a zero code
(for example, a missing `app:///` resource reports `AppResourceNotFound` with code `0`).

Examples:

```
curl:CURLE_COULDNT_CONNECT(7): Failed to connect to 127.0.0.1 port 47651 after 0 ms: Couldn't connect to server
curl:CURLE_FILE_COULDNT_READ_FILE(37): Couldn't open file /tmp/missing.bin
nsurl:NSURLErrorCannotConnectToHost(-1004): Could not connect to the server.
nsurl:NSURLErrorServerCertificateUntrusted(-1202): The certificate for this server is invalid. ...
urllib:AppResourceNotFound(0): no bundled resource for 'app:///missing.js'
```

On Apple platforms, when the `NSError` carries an underlying-error chain with different
codes (e.g. a POSIX-level failure), each distinct level is appended as
`<- <domain>(<code>): <message>`.

The `<domain>` and `<symbol>` tokens are stable ASCII identifiers, so observability
queries can filter on exact substrings (e.g. Splunk `"curl:CURLE_COULDNT_RESOLVE_HOST"`
or `"nsurl:NSURLErrorTimedOut"`). The `<detail>` portion is the platform's human-readable
message — it may be OS-localized on Apple platforms and includes request specifics like
host, port, and path where the platform provides them.

Platform support: the Apple (`NSURLSession`) and Linux (`libcurl`) backends populate
these accessors today; the Windows and Android backends currently always report
empty/zero (contributions welcome — the plumbing in `UrlRequest_Base.h` is shared).

## In-memory data URLs

`UrlRequest` provides a built-in, platform-independent `data:` GET decoder through the
same shared scheme-resolution path as registered resolvers. No network transport or
consumer-specific image/fetch wrapper is involved. The supported contract is:

* `data:[type/subtype[;name=value...]][;base64],payload`, with a required comma.
  An omitted type defaults to `text/plain;charset=US-ASCII`; the `;charset=...`
  shorthand uses `text/plain`. Explicit MIME types use ASCII HTTP token syntax;
  parameter values are nonempty tokens or ASCII quoted strings with backslash escapes.
  Type/subtype and parameter names are lowercased; parameter values are retained.
  Invalid or unsupported MIME syntax is rejected, not replaced by a guessed MIME type.
* Scheme and the final `;base64` marker are ASCII case-insensitive. Surrounding ASCII
  whitespace in the metadata, and spaces between that semicolon and `base64`, are allowed.
  The payload is percent-decoded to **bytes** before optional base64 decoding. Literal
  `+` is preserved; incomplete or non-hex percent escapes remain literal, as in URL percent
  decoding. Raw non-ASCII payload bytes are preserved; callers should supply UTF-8 or
  percent-encoded bytes, not expect locale-dependent URI unescaping.
* Base64 uses the standard alphabet and forgiving-base64 rules: ASCII whitespace is ignored,
  padding may be omitted, and unused trailing bits are ignored. Bad alphabet characters,
  misplaced/excess padding, and lengths congruent to one modulo four are errors. Base64url
  (`-`/`_`) is not supported.
* A literal `?` belongs to the payload (or to the metadata if before the comma), not a
  separate query to discard. A literal `#` starts a fragment, which is excluded from decoding
  and `ResponseUrl()`. Encode these bytes as `%3F`/`%23` to include them literally.
* Success is status 200 / `"OK"`, with a `content-type` header and the decoded body, including
  empty bodies. `Buffer` returns exact bytes; `String` also preserves those bytes (including
  NUL), without charset transcoding, matching the shared resolver contract.
* Decoding is synchronous at `SendAsync()`, once per `Open()`. Malformed/unsupported input
  completes normally with status 0, empty body/headers, and a diagnostic `DataUrlInvalid`
  error. Non-GET requests similarly report `DataUrlUnsupportedMethod`; other decoder
  exceptions report `DataUrlFailed`. No invalid payload is passed to the platform transport.
  `Abort()` before sending prevents shared resolution and returns an operation-cancelled task,
  as for cancellation-guarded Windows continuations. There is no asynchronous decoding to
  interrupt; aborting an already-completed request does not erase its response. As with the
  existing transport cancellation source, reopening an aborted request does not reset cancellation.

This is a bounded byte/MIME transport, not an implementation of all Fetch behavior: it
does not implement browser URL canonicalization, permissive MIME-parser recovery,
charset conversion, streaming, CORS, or a payload-size policy. Supply a serialized URL
without surrounding control characters. A resolver explicitly registered for `"data"`
overrides the built-in decoder; unregistering it restores the built-in behavior.
Base64 decoding reuses the pinned `base-n` helper, with validation before invoking its
otherwise permissive decoder.

On Windows, a successful HTTP response without a Content-Type header leaves that header
absent rather than dereferencing the optional WinRT value or inventing a MIME type.
Existing Windows HTTP response behavior is otherwise unchanged: successful 2xx responses normalize
to 200 after body reading; non-success responses retain their wire status and do not read
their body or publish response headers.

Windows POST matches Content-Type case-insensitively and sets it on the content headers,
not the request headers. Bodies are sent byte-for-byte without UTF-8 transcoding or an
automatically added charset. Omitted Content-Type stays absent; supplied MIME parameters
are preserved, and an invalid supplied MIME type reports a transport error rather than success.

Apple POST also preserves body bytes, using length-aware NSData rather than a
NUL-terminated UTF-8 string. The shared POST wire tests run on Windows and Apple;
Linux explicitly skips them because its backend does not implement POST yet.

Android POST writes raw byte-array chunks to the connection's output stream instead of
converting the body to a Java string. This preserves embedded NULs and UTF-8 bytes while
keeping Content-Length consistent with the supplied body. Android coverage comes from
JsRuntimeHost's Fetch/XHR HTTP transport tests.

## Contributing

Please read [CONTRIBUTING.md](../../CONTRIBUTING.md) for details on our code of conduct, and
the process for submitting pull requests.

## Reporting Security Issues

Security issues and bugs should be reported privately, via email, to the Microsoft 
Security Response Center (MSRC) at [secure@microsoft.com](mailto:secure@microsoft.com). 
You should receive a response within 24 hours. If for some reason you do not, please 
follow up via email to ensure we received your original message. Further information, 
including the [MSRC PGP](https://technet.microsoft.com/en-us/security/dn606155) key, can 
be found in the [Security TechCenter](https://technet.microsoft.com/en-us/security/default).
