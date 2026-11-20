# Current shared-suite Windows timing

This experiment runs Babylon.js's actual `visualization.native.test.ts`, once for the 417-scene Android workload and then for the 397-scene iOS workload. Each phase uses one worker, zero retries, the original reference images and unchanged scene render counts and tolerances. Every scene remains a separate Playwright test.

The fixed workload records the measured mobile suite revision and hashes the shared metadata and original references. Metadata hashes normalize Windows line endings; image hashes use the original bytes. Set `TIMING_SUITE_ROOT`, `BABYLON_NATIVE_PLAYGROUND` and a fresh `TIMING_ARTIFACTS` directory, then run `node run.cjs`. The suite checkout needs its normal dependencies and prepared test tools. The executable must contain the native Win32 Playwright bridge and the pinned Babylon.js 9.29.0 runtime.

Reports retain failures and image differences. A nonzero exit remains a failed experiment; timings do not qualify failing scenes as passing. The second phase follows the first on the same machine, so report the order rather than interpreting both as cold-machine measurements. Build and dependency setup are outside the measured Playwright processes.

This is not a production migration. The earlier 305-scene batch experiment remains in `../windowsTiming`; its timings are not current per-scene suite measurements.
