'use strict';

// Used by WPT to prove terminate() can interrupt top-level script evaluation.
// Chromium's later termination fix is a reminder that callbacks must re-check
// their execution context rather than dereference it after shutdown begins.
// https://github.com/web-platform-tests/wpt/commit/d1c32457e97d9147803705abc4ffac424733dd8d
// https://github.com/chromium/chromium/commit/3115ace01f79a4a1181a82d70f7777a9ddafd8c2
postMessage('start');
while (true) {}
