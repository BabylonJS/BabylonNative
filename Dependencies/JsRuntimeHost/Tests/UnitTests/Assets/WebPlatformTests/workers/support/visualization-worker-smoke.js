'use strict';

// A small, non-proprietary reproduction of the Worker contract used by
// rebeckerspecialties/webapp's GithubPortfolio.worker.ts at commit
// 3765b131bca72057a4331ed7f466b9121cf24d1f. The deployed app constructs an
// ES-module Worker, creates an IndexedDB-backed pipeline, and moves multiple
// Date/Map-rich playback streams through postMessage.

const databaseOpenPromise = new Promise((resolve, reject) => {
  const request = indexedDB.open('visualization-worker-smoke');
  request.onupgradeneeded = () => {
    if (!request.result.objectStoreNames.contains('rebeckerLoaderCacheStore')) {
      request.result.createObjectStore('rebeckerLoaderCacheStore')
        .put({ schema: 1 }, '__schema__');
    }
  };
  request.onsuccess = () => resolve(request.result);
  request.onerror = () => reject(request.error);
});

const gzipBytes = new Uint8Array([
  31, 139, 8, 0, 0, 0, 0, 0, 0, 3, 171, 86, 42, 74, 77, 74,
  77, 206, 78, 45, 242, 201, 79, 76, 73, 45, 114, 78, 76, 206,
  72, 13, 46, 201, 47, 74, 85, 178, 170, 86, 74, 6, 241, 172,
  12, 64, 204, 148, 196, 146, 68, 37, 171, 104, 67, 29, 35, 29,
  227, 216, 90, 29, 168, 148, 33, 146, 148, 137, 142, 169, 142,
  89, 108, 109, 109, 45, 0, 105, 118, 117, 71, 84, 0, 0, 0
]);

const cacheHydrationPromise = (async () => {
  // Match the production cache path: its `${owner}.${repo}.gz` request is
  // relative to the module worker bundle, rather than an absolute app URL.
  const metadataResponse = await fetch('./visualization-worker-cache.json');
  const metadata = await metadataResponse.json();

  const compressedBlob = new Blob([gzipBytes]);
  const decompressedStream = compressedBlob.stream().pipeThrough(
    new DecompressionStream('gzip'));
  const decompressedBlob = await new Response(decompressedStream).blob();
  const prerecorded = JSON.parse(await decompressedBlob.text());

  // WPT requires truncated, checksum-corrupt, and trailing-junk inputs to
  // reject instead of returning a plausible prefix. Keep the native inflater
  // from adopting that historical implementation failure mode.
  // https://github.com/web-platform-tests/wpt/blob/57e48fbf38927e10e86e049b9b03c0e7a1686878/compression/decompression-corrupt-input.any.js
  for (const invalidBytes of [
    gzipBytes.slice(0, gzipBytes.length - 4),
    new Uint8Array([...gzipBytes, 0])
  ]) {
    let rejected = false;
    try {
      const invalid = new Blob([invalidBytes]);
      await new Response(invalid.stream().pipeThrough(
        new DecompressionStream('gzip'))).arrayBuffer();
    } catch (_) {
      rejected = true;
    }
    if (!rejected) {
      throw new Error('invalid gzip cache data was accepted');
    }
  }

  return { metadata, prerecorded };
})();

const databasePromise = Promise.all([
  databaseOpenPromise,
  cacheHydrationPromise
]).then(async ([database, cache]) => {
  // Match IndexedDBCache.setMany(): all hydrated repository records are
  // queued in one readwrite transaction and completion, not individual put
  // success events, resolves the batch.
  await new Promise((resolve, reject) => {
    const transaction = database.transaction(
      'rebeckerLoaderCacheStore', 'readwrite');
    const store = transaction.objectStore('rebeckerLoaderCacheStore');
    for (const [key, value] of Object.entries(
      cache.prerecorded.rebeckerLoaderCacheStore)) {
      store.put(value, key);
    }
    transaction.oncomplete = resolve;
    transaction.onerror = () => reject(transaction.error);
    transaction.onabort = () => reject(transaction.error);
  });

  return { database, cache };
});

const keyFor = config => `${config.owner}/${config.repo}`;
const itemsPerStream = 128;
const makeStream = (recordType, weight) => ({
  recordType,
  items: Array.from({ length: itemsPerStream }, (_, index) => ({
    key: `${recordType}-${index}`,
    weight
  }))
});

onmessage = event => {
  const message = event.data;
  switch (message.type) {
    case 'pipelineCreate':
      databasePromise.then(({ database, cache }) => {
        const transaction = database.transaction(
          'rebeckerLoaderCacheStore', 'readwrite');
        transaction.objectStore('rebeckerLoaderCacheStore').put({
          currentDate: new Date('2026-07-21T00:00:00.000Z'),
          streams: [
            makeStream('commits', 3),
            makeStream('pulls', 2),
            makeStream('issues', 1)
          ]
        }, keyFor(message.config));
        transaction.oncomplete = () => {
          postMessage({
            type: 'pipelineCreated',
            key: keyFor(message.config),
            cacheHydrated:
              cache.metadata.source === 'relative-worker-fetch' &&
              cache.prerecorded.rebeckerLoaderCacheStore['cache:1']
                .data.length === 3
          });
        };
      }).catch(error => {
        postMessage({
          type: 'integrationError',
          message: error && error.message ? error.message : String(error)
        });
      });
      return;

    case 'subscribe':
      postMessage({ type: 'clone', value: false });
      postMessage({ type: 'currentDate', date: null });
      postMessage({ type: 'startFrom', date: undefined });
      postMessage({ type: 'playRate', playRate: 24 });
      postMessage({ type: 'configs', configs: [] });
      return;

    case 'pipelinePlay':
      databasePromise.then(({ database }) => {
        const request = database.transaction('rebeckerLoaderCacheStore')
          .objectStore('rebeckerLoaderCacheStore').get(message.key);
        request.onsuccess = () => {
          const cached = request.result;
          const updatedItems = new Map();
          for (const stream of cached.streams) {
            for (const item of stream.items) updatedItems.set(item.key, item);
          }
          // Preserve an alias across the structured-clone graph, just as the
          // production diff envelope can reference an item from both data and
          // updatedItems.
          cached.streams[0].items[0].self = cached.streams[0].items[0];
          postMessage({
            type: 'data',
            data: cached.streams.map(stream => ({
              ...stream,
              currentDate: cached.currentDate
            })),
            updatedItems,
            metadata: {
              activeTypes: new Set(cached.streams.map(stream => stream.recordType))
            }
          });
          postMessage({ type: 'currentDate', date: cached.currentDate });
        };
      }).catch(error => {
        postMessage({
          type: 'integrationError',
          message: error && error.message ? error.message : String(error)
        });
      });
      return;
  }
};

const abortController = new AbortController();
abortController.abort('visualization-stop');

postMessage({
  type: 'bootstrap',
  workerName: name,
  location: {
    href: location.href,
    protocol: location.protocol,
    pathname: location.pathname
  },
  globals: {
    indexedDB: typeof indexedDB,
    fetch: typeof fetch,
    // The JavaScriptCore Node-API adapter reports some native constructors as
    // typeof "object"; exercise their browser behavior instead of asserting a
    // backend-specific typeof result.
    AbortController:
      new AbortController().signal.aborted === false,
    AbortControllerType: typeof AbortController,
    AbortControllerPrimitiveReason:
      abortController.signal.aborted === true &&
      abortController.signal.reason === 'visualization-stop',
    TextEncoder:
      new TextEncoder().encode('A')[0] === 65,
    TextEncoderType: typeof TextEncoder,
    BlobType: typeof Blob,
    URLType: typeof URL,
    NativeConstructorStatics:
      URL.canParse('app:///cache.gz') &&
      AbortSignal.abort().aborted === true,
    NativeConstructorInstanceof:
      new Blob([]) instanceof Blob &&
      new TextEncoder() instanceof TextEncoder,
    ReadableStream: typeof ReadableStream,
    Response: typeof Response,
    ResponseError:
      Response.error().status === 0 &&
      Response.error().type === 'error',
    DecompressionStream: typeof DecompressionStream,
    blobStream: typeof Blob.prototype.stream
  }
});
