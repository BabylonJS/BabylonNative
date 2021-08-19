# Babylon Native Polyfills
Babylon Native Polyfills add support for certain JavaScript browser APIs, such as [`canvas`](https://developer.mozilla.org/en-US/docs/Web/API/Canvas_API) and [`window`](https://developer.mozilla.org/en-US/docs/Web/API/console). They do not attempt to fulfill the web specification exactly, nor do they mirror the behavior of browsers. The polyfills only provide functionality that is either required by Babylon.js, or would be frequently used by Babylon developers.

The polyfills are still in an early stage and are subject to change.

At the moment, we are using the following polyfills:
* [Canvas](../Polyfills/Canvas/readme.md)
* [Console](../Polyfills/Console/readme.md)
* [Window](../Polyfills/Window/readme.md)
* [XMLHttpRequest](../Polyfills/XMLHttpRequest/readme.md)