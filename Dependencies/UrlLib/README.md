# UrlLib

UrlLib is a cross-platform C++ library that utilizes platform-specific implementations
for URL-related functionality. Although it was created as a subcomponent of
[JsRuntimeHost](https://github.com/BabylonJS/JsRuntimeHost) for its polyfills, it may also be used standalone.

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

## Contributing

Please read [CONTRIBUTING.md](./CONTRIBUTING.md) for details on our code of conduct, and 
the process for submitting pull requests.

## Reporting Security Issues

Security issues and bugs should be reported privately, via email, to the Microsoft 
Security Response Center (MSRC) at [secure@microsoft.com](mailto:secure@microsoft.com). 
You should receive a response within 24 hours. If for some reason you do not, please 
follow up via email to ensure we received your original message. Further information, 
including the [MSRC PGP](https://technet.microsoft.com/en-us/security/dn606155) key, can 
be found in the [Security TechCenter](https://technet.microsoft.com/en-us/security/default).
