# URL
Partial implementation for [`URL`](https://developer.mozilla.org/en-US/docs/Web/API/URL)

## Supported
- [`search`](https://developer.mozilla.org/en-US/docs/Web/API/URL/search)
- [`href`](https://developer.mozilla.org/en-US/docs/Web/API/URL/href)
- [`origin`](https://developer.mozilla.org/en-US/docs/Web/API/URL/origin)
- [`pathname`](https://developer.mozilla.org/en-US/docs/Web/API/URL/pathname)
- [`hostname`](https://developer.mozilla.org/en-US/docs/Web/API/URL/hostname)
- [`searchParams`](https://developer.mozilla.org/en-US/docs/Web/API/URL/searchParams)

## Not implemented
- URL constructor is partially implemented, it only supports one argument and the url parsing is limited. Parsing is planned to be forward to UrlLib and use platform specific functions to parse the URL and get the parts
- All other properties/methods not specified above

# URLSearchParams
Partial implementatioin for [`URLSearchParams`](https://developer.mozilla.org/en-US/docs/Web/API/URLSearchParams)  

## Supported

- [`get`](https://developer.mozilla.org/en-US/docs/Web/API/URLSearchParams/get)
- [`set`](https://developer.mozilla.org/en-US/docs/Web/API/URLSearchParams/set)
- [`has`](https://developer.mozilla.org/en-US/docs/Web/API/URLSearchParams/has)

## Not implemented
- URLSearchParams constructor is partially implemented, it does not support sequence of pairs and records
- All other properties/methods not specified above