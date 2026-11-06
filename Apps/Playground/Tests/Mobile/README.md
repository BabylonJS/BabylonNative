# Native mobile Playwright tests

Android and iOS run Babylon.js's shared `visualization.native.test.ts` through its existing `NativePage` adapter. Playwright owns each scene, test selection, steps, snapshot assertions, retries and worker replacement. This runner supplies only the BrowserStack App Automate lifecycle and native evaluation/framebuffer transport. It does not invoke `validation_native.js`, copy the scene catalog, or modify reference images or comparison tolerances.

```text
Shared Playwright scene test -> NativePage -> mobile transport
                                              |
                                    BrowserStack Local tunnel
                                              |
                                   Android/iOS Playground
                                   evaluate / captured RGBA
                                              |
Shared web-reference snapshot <- lossless PNG <-+
```

## Build

Android requires the normal Playground toolchain. From `Apps/Playground/Android`, build the opt-in ARM64 package:

```powershell
.\gradlew.bat 'assembleRelease' '-PjsEngine=V8' '-PARM64Only' '-PNDK_VERSION=28.2.13676358' '-PmobileVisualTests' '-PBABYLON_NATIVE_PLUGIN_NATIVEDRACO=ON' '-PBABYLON_NATIVE_PLUGIN_NATIVEMESHOPT=ON' '-PSANITIZERS=OFF'
```

iOS requires macOS and the repository's supported Xcode toolchain. Configure with `BABYLON_NATIVE_PLAYGROUND_MOBILE_TESTS=ON` and `BABYLON_NATIVE_PLUGIN_TESTUTILS=ON`, build Playground for `iphoneos` with `CODE_SIGNING_ALLOWED=NO`, and zip `Payload/Playground.app` into an IPA. The manual workflow contains the build/archive steps. BrowserStack re-signs this test package; it is not an App Store package.

Without a mobile endpoint, the app retains ordinary Playground startup. Default builds omit the command transport. Never distribute a test-enabled app as a production app: it accepts JavaScript from its private controller. The opt-in iOS policy permits HTTP only for the exact `bs-local.com` domain, without changing ordinary App Transport Security settings.

## Run the shared suite

Prepare a Babylon.js checkout containing the shared native suite and `NativePage`'s external transport support, using its documented package-manager version and `npm ci`. From this directory, restore host dependencies with `npm ci`, then run `npm test` and `npm run test:shared`.

| Variable | Meaning |
| --- | --- |
| `BABYLON_JS_ROOT` | Absolute path to that prepared Babylon.js checkout. Its Playwright installation, shared native spec and web reference images are used directly. |
| `BROWSERSTACK_USERNAME`, `BROWSERSTACK_ACCESS_KEY` | Host-only credentials from the approved account owner/secret store. Not needed for discovery or portable controls. |
| `MOBILE_APP` | Absolute path to this checkout's opt-in APK or unsigned IPA, containing protocol 2. Older catalog-wrapper packages are rejected. |
| `MOBILE_PLATFORM` | `android` or `ios`. Supplies the shared platform/backend exclusion tags. |
| `MOBILE_DEVICE`, `MOBILE_OS_VERSION` | Exact pair in the live App Automate inventory. No nearest-device substitution. |
| `BROWSERSTACK_LOCAL_BINARY` | Installed BrowserStack Local executable; defaults to the user's `.browserstack` directory. |
| `MOBILE_OUTPUT` | Optional result directory; default `mobile-results` beside this file. |
| `MOBILE_TEST_GREP`, `MOBILE_SHARD` | Optional equivalents of Playwright's `--grep` and `--shard`. Legacy `MOBILE_TEST_INDICES` is rejected. |
| `TIMEOUT` | Optional shared per-scene timeout in milliseconds; mobile default 300000 includes startup for the worker's first test. |

Examples after setting the checkout and device variables:

```powershell
npm run visual -- --list
npm run visual -- --grep 'Particle system'
npm run visual -- --workers 2 --retries 1
npm run visual -- --shard 1/2
```

The run starts one cloud device and tunnel per worker slot before launching Playwright. Successful scenes reuse the engine according to the shared suite; native cleanup restores hardware scale 1 so a scene's scaling cannot affect later screenshots. Failed assertions retire the Playwright worker, not its device: the replacement worker acquires the same slot after scene/engine cleanup. An acknowledged scene error is recoverable; a command timeout or failed cleanup poisons that device and fails the run without silently replaying commands or allocating a replacement. Default is one worker with no test retries. Worker counts must be positive integers, not percentages. Increase workers only within available paid capacity, leaving room for other users. Sharding divides the shared Playwright tests, not native-catalog indices. Snapshot updates and replacement projects/configurations are forbidden.

The runner uploads the package once per invocation and shares that exact hash-checked upload across workers and retries. There is no persistent upload cache. Discovery does not upload or allocate a device.

The runtime Babylon.js version must match `Apps/package-lock.json`. The shared suite fetches snippets/assets and compares exported native frames with its existing web references. Differences from the old native catalog's selection and outcomes are expected; historical native-catalog runs do not qualify this shared-suite integration.

## Results and cleanup

`playwright-results.json` contains one named result per selected scene, step timings, retries, worker indices and normal Playwright failures. Snapshot mismatches retain actual/expected/difference images under `test-results/`; passing screenshots are not separately saved. Playwright clears only that child directory, preserving run-owned upload and session evidence created before test startup. Each session's `sessions/<id>/session.json` records package hash, device/runtime, startup timing, worker index, cloud occupancy and cleanup. Startup, transport and cleanup errors are explicit; unexecuted scenes are not passes.

`app-upload.json` records the single shared upload's duration and package hash. `run.json` records total time, Playwright time, device startup times, worker lease history and run/cleanup failures. Session summaries retain cloud occupancy for every run. The run owner releases all devices/tunnels when Playwright exits, including partial startup, nonzero exit and graceful interruption. Native device logs are fetched after startup, command or cleanup failures; host-only snapshot mismatches retain Playwright's image evidence without waiting for unrelated device logs.

Screenshot export consumes the frame captured by the shared render step, preserves RGBA bytes and validates PNG dimensions/checksums. Missing captures fail rather than reusing a previous scene's image. Commands are correlated and acknowledged; timeouts poison that session instead of replaying ambiguous work.

The controller binds to loopback with a random endpoint. BrowserStack Local must register the exact cloud identifier before allocation. Credentials never enter app arguments or scripts. Traces, video, network capture and Appium debug screenshots are disabled. Private tunnel logs remain outside result artifacts and are deleted on cleanup. Device diagnostics and saved metadata are credential-redacted.

## Manual continuous integration

`.github/workflows/mobile-visual-tests.yml` is manual and defaults to building APK/IPA artifacts without paid sessions. Existing pull-request checks are unchanged. Optional cloud runs select a full trusted Babylon.js commit SHA containing the shared adapter, then prepare and execute that exact suite. The paired Babylon.js change must be published before such a revision can be selected.

Before enabling `run-cloud`, the account owner must confirm approved credentials, rotation status, plan/cost and permitted devices. Configure required reviewers and allowed branches on the `browserstack-mobile` environment, its two credential secrets, and `MOBILE_TESTING_APPROVED=true`. Both repository selections must be trusted: their code executes while credentials are available. Do not put credentials in inputs, repository files, recovery scripts, URLs or artifacts.

Cloud jobs use one device per selected platform, at most two simultaneously. Extra device/operating-system combinations require measured qualification; one device does not establish platform-wide coverage. This workflow does not authorize production rollout or a purchase.
