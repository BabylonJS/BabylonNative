// These are small ports of only the Worker WPT assertions that exposed gaps in
// JsRuntimeHost. The upstream files remain pinned by UPSTREAM_REVISION and are
// run from an external WPT checkout during broader conformance work.
//
// https://github.com/web-platform-tests/wpt/blob/4809b72f863e05ab1df710d3390547dd86694239/workers/interfaces/DedicatedWorkerGlobalScope/EventTarget.worker.js
// https://github.com/web-platform-tests/wpt/blob/4809b72f863e05ab1df710d3390547dd86694239/workers/interfaces/DedicatedWorkerGlobalScope/onmessage.worker.js
// https://github.com/web-platform-tests/wpt/blob/4809b72f863e05ab1df710d3390547dd86694239/workers/interfaces/DedicatedWorkerGlobalScope/postMessage/return-value.worker.js
// https://github.com/web-platform-tests/wpt/blob/4809b72f863e05ab1df710d3390547dd86694239/workers/interfaces/WorkerGlobalScope/self.any.js
// https://github.com/web-platform-tests/wpt/blob/4809b72f863e05ab1df710d3390547dd86694239/workers/interfaces/WorkerUtils/importScripts/001.worker.js

const failures = [];

function check(name, callback) {
  try {
    callback();
  } catch (error) {
    failures.push({
      name,
      message: error && error.message ? error.message : String(error)
    });
  }
}

function assert(condition, message) {
  if (!condition) throw new Error(message);
}

check('self is the worker global', () => {
  assert(self === globalThis, 'self and globalThis differ');
  assert(self instanceof WorkerGlobalScope, 'self is not a WorkerGlobalScope');
});

check('self is readonly', () => {
  const original = self;
  self = 1;
  assert(self === original, 'assigning self replaced the worker global');
});

check('removeEventListener removes a capturing listener', () => {
  let calls = 0;
  function listener() {
    calls++;
    removeEventListener('message', listener, true);
  }
  addEventListener('message', listener, true);
  dispatchEvent(new Event('message'));
  dispatchEvent(new Event('message'));
  assert(calls === 1, 'listener ran ' + calls + ' times');
});

check('dispatched event targets the worker global', () => {
  let target;
  function listener(event) {
    target = event.target;
  }
  addEventListener('message', listener, true);
  dispatchEvent(new Event('message'));
  removeEventListener('message', listener, true);
  assert(target === self, 'event.target was not self');
});

check('onmessage rejects primitive handlers', () => {
  self.onmessage = 1;
  assert(self.onmessage === null, 'primitive handler did not normalize to null');
});

check('onmessage invokes function handlers', () => {
  let called = false;
  self.onmessage = () => {
    called = true;
  };
  dispatchEvent(new Event('message'));
  assert(called, 'function handler was not invoked');
});

check('postMessage returns undefined', () => {
  assert(postMessage({ type: 'probe' }) === undefined,
    'postMessage returned a non-undefined value');
});

check('importScripts with no arguments is a no-op', () => {
  importScripts();
});

postMessage({ type: 'focused-results', failures });
