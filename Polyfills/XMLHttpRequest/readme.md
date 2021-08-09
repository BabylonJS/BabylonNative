# XMLHttpRequest
Minimal implementation of XMLHttpRequest required to support the Babylon.js SceneLoader. Under the hood, XMLHttpRequest is implemented using various platform-specific APIs in the UrlLib dependency.

## Event listening
We do not support `onload`-style event listeners. Instead, you should listen to events using `addEventListener`. At the moment, we only support the following events:
* `loadend`
* `readystatechange`

## Local files
Unlike the web, XMLHttpRequest supports loading local files using two schemes:
* `file://` allows you to load from an absolute path
*  `app://` allows you to load from a relative path, either the current program or package depending on platform



## Other things to be aware of:
* Only `GET` requests are currently supported
* For `readyState`, we only support `UNSENT`, `OPENED`, and `DONE`