'use strict';

// Adapted from the current WPT close() coverage. In particular, close() must
// discard later tasks without suppressing messages or errors produced by the
// task that called close().
// https://github.com/web-platform-tests/wpt/blob/57e48fbf38927e10e86e049b9b03c0e7a1686878/workers/support/WorkerGlobalScope-close.js
onmessage = event => {
  switch (event.data) {
    case 'close':
      close();
      postMessage('Should be delivered');
      setTimeout(() => postMessage('Should not be delivered'), 0);
      break;

    case 'closeWithPendingEvents':
      setTimeout(() => postMessage('Pending event should be discarded'), 0);
      close();
      break;

    case 'closeWithError':
      close();
      throw new Error('Error after close should be delivered');
  }
};
