(() => {
  const failures = [];
  const cases = [
    "/workers/focused-api.js"
  ];

  let index = 0;
  let timer = 0;

  function progress(name) {
    if (typeof __jsrhWptProgress === "function") __jsrhWptProgress(name);
  }

  function fail(name, detail) {
    failures.push(name + ": " + detail);
  }

  function finish() {
    progress("finish");
    clearTimeout(timer);
    __jsrhWptDone(failures.length === 0, failures.join("\n"));
  }

  function runStructuredMessageCase() {
    const name = "/workers/support/Worker-structure-message.js";
    progress(name);
    const worker = new Worker(name);
    const input = new ArrayBuffer(20);
    let sawPass = false;

    timer = setTimeout(() => {
      worker.terminate();
      fail(name, "timed out");
      finish();
    }, 10000);

    worker.onerror = event => {
      clearTimeout(timer);
      worker.terminate();
      fail(name, event.message || "worker error");
      finish();
    };

    worker.onmessage = event => {
      if (typeof event.data === "string") {
        sawPass = event.data.indexOf("PASS:") === 0;
        if (!sawPass) fail(name, event.data);
        return;
      }

      clearTimeout(timer);
      const value = event.data;
      if (!sawPass || !value || value.operation !== "find-edges" ||
          !(value.input instanceof ArrayBuffer) || value.input.byteLength !== 20 ||
          value.threshold !== 0.6) {
        fail(name, "structured clone did not preserve the WPT payload");
      }
      worker.terminate();
      runCloseCases();
    };

    worker.postMessage({ operation: "find-edges", input, threshold: 0.6 }, [input]);
    // Current WebKitGTK exposes the standards-track detached getter before
    // its plain ArrayBuffer byteLength reflection catches up. Either signal
    // confirms the sender backing store is detached; the Node-API adapter
    // independently validates detached state before and after transfer.
    if (input.detached !== true && input.byteLength !== 0) {
      fail(name, "transfer did not detach the sender ArrayBuffer");
    }
  }

  function runThrowingGetterCase() {
    const name = "structured clone preserves a throwing getter exception";
    progress(name);
    const worker = new Worker("/workers/support/Worker-early-message.js");
    const expected = new Error("getter sentinel");
    const transfer = new ArrayBuffer(8);
    let thrown;

    // Servo used to clear the pending JS exception raised while reading an
    // enumerable property and replace it with DataCloneError. Serialization
    // must propagate the original exception and must not detach transferables
    // after serialization has already failed.
    // https://github.com/servo/servo/commit/26f4da824907946569fb673a249a2c9035c1d1e4
    try {
      worker.postMessage({
        get value() {
          throw expected;
        }
      }, [transfer]);
    } catch (error) {
      thrown = error;
    }

    worker.terminate();
    if (thrown !== expected) {
      fail(name, "postMessage replaced or swallowed the getter exception");
    }
    if (transfer.detached === true || transfer.byteLength === 0) {
      fail(name, "postMessage detached a transfer after serialization failed");
    }
    runStructuredMessageCase();
  }

  function runCloseCases() {
    const name = "/workers/support/WorkerGlobalScope-close.js";
    const modes = ['close', 'closeWithPendingEvents', 'closeWithError'];
    let closeIndex = 0;

    function runNextClose() {
      clearTimeout(timer);
      if (closeIndex === modes.length) {
        runEarlyMessageCase();
        return;
      }

      const mode = modes[closeIndex++];
      progress(name + " / " + mode);
      const worker = new Worker(name);
      let messages = 0;
      let settled = false;

      function complete() {
        if (settled) return;
        settled = true;
        clearTimeout(timer);
        worker.terminate();
        runNextClose();
      }

      worker.onmessage = event => {
        messages++;
        if (mode === 'close' && messages === 1 && event.data === 'Should be delivered') {
          // Parent-to-worker tasks posted after close() must be discarded too.
          worker.postMessage('Should not be observed');
          clearTimeout(timer);
          timer = setTimeout(() => {
            if (messages !== 1) fail(name + ' / ' + mode, 'a later task was delivered');
            complete();
          }, 100);
          return;
        }
        fail(name + ' / ' + mode, 'unexpected message: ' + String(event.data));
        complete();
      };

      worker.onerror = event => {
        if (mode === 'closeWithError') {
          if (String(event.message).indexOf('Error after close should be delivered') === -1) {
            fail(name + ' / ' + mode, event.message || 'wrong worker error');
          }
        } else {
          fail(name + ' / ' + mode, event.message || 'worker error');
        }
        complete();
      };

      timer = setTimeout(() => {
        if (mode !== 'closeWithPendingEvents') {
          fail(name + ' / ' + mode, 'timed out');
        }
        complete();
      }, mode === 'closeWithPendingEvents' ? 150 : 5000);
      worker.postMessage(mode);
    }

    runNextClose();
  }

  function runEarlyMessageCase() {
    const name = "/workers/support/Worker-early-message.js";
    progress(name);
    const worker = new Worker(name);
    const payload = { phase: 'queued-before-start' };
    let postMessageReturned = false;

    timer = setTimeout(() => {
      worker.terminate();
      fail(name, 'timed out');
      runEarlyTerminationCase();
    }, 5000);

    worker.onerror = event => {
      clearTimeout(timer);
      worker.terminate();
      fail(name, event.message || 'worker error');
      runEarlyTerminationCase();
    };
    worker.onmessage = event => {
      clearTimeout(timer);
      if (!postMessageReturned) fail(name, 'message delivery was synchronous');
      if (!event.data || event.data.phase !== payload.phase) {
        fail(name, 'message queued during startup was lost or corrupted');
      }
      worker.terminate();
      runEarlyTerminationCase();
    };

    worker.postMessage(payload);
    postMessageReturned = true;
  }

  function runEarlyTerminationCase() {
    const name = "/workers/support/Worker-early-message.js / terminate-before-start";
    progress(name);
    const worker = new Worker("/workers/support/Worker-early-message.js");
    let delivered = false;

    worker.onmessage = () => { delivered = true; };
    worker.onerror = () => { delivered = true; };
    worker.postMessage('must be discarded');
    worker.terminate();

    timer = setTimeout(() => {
      if (delivered) fail(name, 'a pre-start event ran after terminate()');
      runTerminationDuringEvaluationCase();
    }, 150);
  }

  function runTerminationDuringEvaluationCase() {
    const name = "/workers/support/Worker-run-forever.js";
    progress(name);
    if (globalThis.__jsrhCanInterruptWorker !== true) {
      runTerminationStressCase();
      return;
    }

    const worker = new Worker(name);
    timer = setTimeout(() => {
      worker.terminate();
      fail(name, 'worker did not begin top-level evaluation');
      runTerminationStressCase();
    }, 5000);

    worker.onerror = event => {
      clearTimeout(timer);
      worker.terminate();
      fail(name, event.message || 'worker error');
      runTerminationStressCase();
    };
    worker.onmessage = event => {
      if (event.data !== 'start') {
        fail(name, 'unexpected message before termination');
        return;
      }
      clearTimeout(timer);
      worker.terminate();
      timer = setTimeout(runTerminationStressCase, 150);
    };
  }

  function runTerminationStressCase() {
    const name = "/workers/support/Worker-termination-stress.js";
    let iteration = 0;

    function runIteration() {
      clearTimeout(timer);
      if (iteration === 20) {
        runTerminateCase();
        return;
      }

      const current = iteration;
      progress(name + " / iteration " + current);
      const worker = new Worker(name);
      let retired = false;
      timer = setTimeout(() => {
        retired = true;
        worker.terminate();
        fail(name + ' / iteration ' + current, 'timed out');
        iteration++;
        runIteration();
      }, 5000);

      worker.onerror = event => {
        if (retired) {
          fail(name + ' / iteration ' + current, 'error arrived after terminate()');
          return;
        }
        clearTimeout(timer);
        retired = true;
        worker.terminate();
        fail(name + ' / iteration ' + current, event.message || 'worker error');
        iteration++;
        runIteration();
      };
      worker.onmessage = event => {
        if (retired) {
          fail(name + ' / iteration ' + current, 'message arrived after terminate()');
          return;
        }
        if (event.data !== 'ready') {
          fail(name + ' / iteration ' + current, 'unexpected startup message');
          return;
        }

        clearTimeout(timer);
        // Leave an inbound task pending while shutdown starts. Repetition is
        // intentional: WebKit's 2026 regression needed a lifecycle stress
        // test to expose cross-thread destruction of worker-owned state.
        worker.postMessage({ iteration: current });
        retired = true;
        worker.terminate();
        iteration++;
        timer = setTimeout(runIteration, 0);
      };
    }

    runIteration();
  }

  function runTerminateCase() {
    const name = "/workers/constructors/Worker/terminate.js";
    progress(name);
    const worker = new Worker(name);
    let messages = 0;

    worker.onerror = event => {
      clearTimeout(timer);
      worker.terminate();
      fail(name, event.message || "worker error");
      finish();
    };
    worker.onmessage = () => { messages++; };

    timer = setTimeout(() => {
      const expected = messages;
      // Adapt the WPT document harness: hold the parent turn while the Worker
      // queues additional messages, then verify terminate() discards them.
      const start = Date.now();
      while (Date.now() - start < 50) {}
      worker.terminate();

      timer = setTimeout(() => {
        if (messages !== expected) {
          fail(name, "message events queued before terminate() were delivered");
        }
        runVisualizationStartupCase();
      }, 100);
    }, 100);
  }

  function runVisualizationStartupCase() {
    const name = "/workers/support/visualization-worker-smoke.js";
    progress(name);
    const workerUrl = new URL(name, "app:///");
    const worker = new Worker(workerUrl, {
      type: "module",
      name: "github-portfolio"
    });
    const config = { owner: "BabylonJS", repo: "Babylon.js" };
    const expectedKey = config.owner + "/" + config.repo;
    let sawBootstrap = false;
    let sawPipeline = false;
    let sawData = false;
    let sawPlaybackDate = false;
    const initialTypes = new Set();

    function completeIfReady() {
      if (!sawBootstrap || !sawPipeline || !sawData || !sawPlaybackDate ||
          initialTypes.size !== 5) {
        return;
      }
      clearTimeout(timer);
      worker.terminate();
      finish();
    }

    timer = setTimeout(() => {
      worker.terminate();
      fail(name, "timed out during app-derived startup");
      finish();
    }, 10000);

    worker.onerror = event => {
      clearTimeout(timer);
      worker.terminate();
      fail(name, event.message || "worker error");
      finish();
    };
    worker.onmessageerror = () => {
      clearTimeout(timer);
      worker.terminate();
      fail(name, "messageerror while cloning a visualization stream");
      finish();
    };
    worker.onmessage = event => {
      const value = event.data;
      if (!value || typeof value.type !== "string") return;
      switch (value.type) {
        case "integrationError":
          fail(name, "app-derived worker integration failed: " + value.message);
          clearTimeout(timer);
          worker.terminate();
          finish();
          return;
        case "bootstrap":
          if (value.workerName !== "github-portfolio" ||
              !value.location || value.location.protocol !== "app:" ||
              value.location.pathname !== name ||
              !value.globals || value.globals.indexedDB !== "object" ||
              value.globals.fetch !== "function" ||
              value.globals.AbortController !== true ||
              value.globals.AbortControllerType !== "function" ||
              value.globals.AbortControllerPrimitiveReason !== true ||
              value.globals.TextEncoder !== true ||
              value.globals.TextEncoderType !== "function" ||
              value.globals.BlobType !== "function" ||
              value.globals.URLType !== "function" ||
              value.globals.NativeConstructorStatics !== true ||
              value.globals.NativeConstructorInstanceof !== true ||
              value.globals.ReadableStream !== "function" ||
              value.globals.Response !== "function" ||
              value.globals.ResponseError !== true ||
              value.globals.DecompressionStream !== "function" ||
              value.globals.blobStream !== "function") {
            fail(name, "worker-global bootstrap surface is incomplete: " +
              JSON.stringify(value));
          }
          sawBootstrap = true;
          break;
        case "pipelineCreated":
          if (value.key !== expectedKey || value.cacheHydrated !== true) {
            fail(name, "relative/gzip cache hydration or IndexedDB pipeline setup failed");
          }
          sawPipeline = true;
          worker.postMessage({ type: "pipelinePlay", key: expectedKey });
          break;
        case "clone":
        case "currentDate":
        case "startFrom":
        case "playRate":
        case "configs":
          if (value.type === "currentDate" && value.date instanceof Date) {
            if (value.date.toISOString() !== "2026-07-21T00:00:00.000Z") {
              fail(name, "playback Date changed across postMessage");
            }
            sawPlaybackDate = true;
          } else {
            initialTypes.add(value.type);
          }
          break;
        case "data": {
          const streams = value.data;
          if (!Array.isArray(streams) || streams.length !== 3 ||
              !(value.updatedItems instanceof Map) ||
              value.updatedItems.size !== 384 ||
              !(value.metadata.activeTypes instanceof Set) ||
              value.metadata.activeTypes.size !== 3 ||
              !(streams[0].currentDate instanceof Date) ||
              streams[0].currentDate !== streams[1].currentDate ||
              streams[0].items[0].self !== streams[0].items[0]) {
            fail(name, "multi-stream structured clone lost Date/Map/Set/alias fidelity");
          }
          sawData = true;
          break;
        }
      }
      completeIfReady();
    };

    // Match the interop's startup protocol: messages may be queued while the
    // module-compatible bundle is still evaluating.
    worker.postMessage({ type: "pipelineCreate", config });
    worker.postMessage({ type: "subscribe" });
  }

  function next() {
    clearTimeout(timer);
    if (index === cases.length) {
      runThrowingGetterCase();
      return;
    }

    const name = cases[index++];
    progress(name);
    const worker = new Worker(name);
    timer = setTimeout(() => {
      worker.terminate();
      fail(name, "timed out");
      next();
    }, 10000);

    worker.onerror = event => {
      clearTimeout(timer);
      worker.terminate();
      fail(name, event.message || "worker error");
      next();
    };

    worker.onmessage = event => {
      const value = event.data;
      if (!value || value.type !== "focused-results") return;
      clearTimeout(timer);
      for (const test of value.failures || []) {
        fail(name + " / " + test.name, test.message || "failed");
      }
      worker.terminate();
      next();
    };
  }

  next();
})();
