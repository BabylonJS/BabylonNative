'use strict';

// WebKit fixed a terminate-time UAF caused by destroying a worker-owned JS
// object on the main thread when a cross-thread postTask failed. The runner
// repeatedly terminates with an inbound task pending to exercise that shape.
// https://github.com/WebKit/WebKit/commit/4aaa3c1477e296e67b03e1461479b8caf57c37dd
postMessage('ready');
onmessage = event => postMessage(event.data);
