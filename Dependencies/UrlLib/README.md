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

## Response metadata

`UrlRequest` reports a response the way the Fetch standard defines it, as far as each platform
API allows:

* `StatusText()` is the reason phrase of the status line. HTTP/2 and HTTP/3 have none, so it is
  empty for a response that arrived over them. Over HTTP/1.x it is the phrase the server sent,
  including a non-standard one, where the platform reports it: libcurl and `Windows.Web.Http`
  do. `NSURLSession` and `HttpURLConnection` (through the wrapper used here) do not, and there it
  is the standard phrase for the status code (`OK`, `Not Found`, ...), as it is for `file://` and
  `app://` responses.
* `ResponseUrl()` is the URL of the final response after redirects, without a fragment, and
  `Redirected()` tells whether there was a redirect. Both are empty / false until there is a
  response and after a transport failure.
* `GetAllResponseHeaders()` is keyed by lower-cased name. The values of a header that the
  response repeats are combined with `, ` in the order they arrived.

| Platform | Reason phrase over HTTP/1.x | Protocol detection |
|---|---|---|
| macOS, iOS, visionOS | Standard phrase for the code | `NSURLSessionTaskMetrics` (macOS 12 / iOS 15 and later; earlier systems report the standard phrase for every protocol) |
| Linux | As sent | Status line written by libcurl |
| Windows (Win32, UWP) | As sent | `HttpResponseMessage.Version` |
| Android | Standard phrase for the code | `X-Android-Selected-Protocol` |

## HTTP versions

Each backend negotiates the newest HTTP version its platform stack offers and falls back
automatically. Plain `http://`, `file://` and `app://` requests are unaffected.

| Platform | Backend | Default |
|---|---|---|
| macOS, iOS, visionOS | `NSURLSession` | HTTP/2 through ALPN. HTTP/3 when the origin advertises it in an HTTPS DNS record or a previous `Alt-Svc` response. |
| Linux | libcurl | HTTP/2 over TLS when the system curl has it. HTTP/3 is not attempted. |
| Windows (Win32, UWP) | `Windows.Web.Http` | HTTP/2 through ALPN. The API has no HTTP/3 setting. |
| Android | `java.net.HttpURLConnection` | HTTP/1.1. |

`URLLIB_ATTEMPT_HTTP3=ON` (CMake, off by default) makes the Apple and libcurl backends attempt
HTTP/3 first on every `https` request: `assumesHTTP3Capable` on macOS 11.3 / iOS 14.5 and later,
and `CURL_HTTP_VERSION_3` when the run-time curl is 8.0 or later with a QUIC backend. It is a
trade, not a free upgrade. Measured over cold connections (macOS 27, `NSURLSession`; curl 8.23
behaves alike):

- An HTTP/3 origin that does not advertise in DNS (`quic.nginx.org`, `cdn.jsdelivr.net`) answers
  the first request over HTTP/3 instead of HTTP/2, about 60 to 180 ms sooner.
- An HTTP/2-only origin (`github.com`, `raw.githubusercontent.com`, `assets.babylonjs.com`) answers
  the first request 110 to 330 ms later.
- An origin that advertises HTTP/3 in DNS is unchanged; it already uses HTTP/3 on the first request.

Turn it on when the application's own origins are known to serve HTTP/3.

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
