# URL
Partial implementation for [`URL`](https://developer.mozilla.org/en-US/docs/Web/API/URL)

## Supported
- [`search`](https://developer.mozilla.org/en-US/docs/Web/API/URL/search)
- [`href`](https://developer.mozilla.org/en-US/docs/Web/API/URL/href)
- [`origin`](https://developer.mozilla.org/en-US/docs/Web/API/URL/origin)
- [`pathname`](https://developer.mozilla.org/en-US/docs/Web/API/URL/pathname)
- [`hostname`](https://developer.mozilla.org/en-US/docs/Web/API/URL/hostname)
- [`searchParams`](https://developer.mozilla.org/en-US/docs/Web/API/URL/searchParams)
- [`protocol`](https://developer.mozilla.org/en-US/docs/Web/API/URL/protocol)
- [`username`](https://developer.mozilla.org/en-US/docs/Web/API/URL/username)
- [`password`](https://developer.mozilla.org/en-US/docs/Web/API/URL/password)
- [`host`](https://developer.mozilla.org/en-US/docs/Web/API/URL/host)
- [`port`](https://developer.mozilla.org/en-US/docs/Web/API/URL/port)
- [`hash`](https://developer.mozilla.org/en-US/docs/Web/API/URL/hash)
- [`toString`](https://developer.mozilla.org/en-US/docs/Web/API/URL/toString)
- [`toJSON`](https://developer.mozilla.org/en-US/docs/Web/API/URL/toJSON)
- [`parse`](https://developer.mozilla.org/en-US/docs/Web/API/URL/parse_static)
- [`canParse`](https://developer.mozilla.org/en-US/docs/Web/API/URL/canParse_static)
- [`createObjectURL`](https://developer.mozilla.org/en-US/docs/Web/API/URL/createObjectURL_static) (Blob only; mints a `blob:` URL backed by an in-memory, process-global object-URL store)
- [`revokeObjectURL`](https://developer.mozilla.org/en-US/docs/Web/API/URL/revokeObjectURL_static) (releases the stored bytes for a `blob:` URL previously returned by `createObjectURL`; subsequent fetch/XMLHttpRequest against that URL fail as a network error)

# URLSearchParams
Partial implementatioin for [`URLSearchParams`](https://developer.mozilla.org/en-US/docs/Web/API/URLSearchParams)  

## Supported

- [`get`](https://developer.mozilla.org/en-US/docs/Web/API/URLSearchParams/get)
- [`set`](https://developer.mozilla.org/en-US/docs/Web/API/URLSearchParams/set)
- [`has`](https://developer.mozilla.org/en-US/docs/Web/API/URLSearchParams/has)

## Not implemented
- URLSearchParams constructor is partially implemented, it does not support sequence of pairs and records
- All other properties/methods not specified above