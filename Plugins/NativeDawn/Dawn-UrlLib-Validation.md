# NativeDawn validation

**Latest results:** [October 28 monorepo validation](#monorepo-integration-and-validation-2026-10-28).
The original sections below describe earlier override-based 9.21.2 runs, not
the current monorepo consumer. Older dependency pins and external transport
requirements below describe historical runs only: transport fixes are now
included in-tree and require no separate checkout or pull request.

**Run date: 2026-10-07.** Both complete Babylon.js WebGPU visualization catalogs
and the complete Babylon-Lite parity schedule were rerun locally on Windows.
These are fresh runs, not totals assembled from earlier focused retries.

## Results

| Suite / consumer | Executed | Passed | Failed | Timed out | Skipped | Duration | Exit |
|---|---:|---:|---:|---:|---:|---|---:|
| Babylon-Lite full parity schedule | 537 | 477 | 55 | 5 | 0 | 35m 48.392s | 1 |
| Babylon.js WebGPU, unchanged 9.21.2 bundle | 714 | 709 | 5 | 0 | 148 | 28m 44.817s | -1 |
| Babylon.js WebGPU, isolated canonical font-metrics backport | 714 | 714 | 0 | 0 | 148 | 13m 20.779s | 0 |
| NativeDawn standalone runtime checks | 111 | 111 | 0 | 0 | 0 | 515ms | 0 |
| NativeDawn Babylon.js host checks | 20 | 20 | 0 | 0 | 0 | 968ms | 0 |

The Babylon.js counts include readiness failures as failures, not as a separate
Playwright timeout category. Both visualization runs report **zero missing
references** and retain the original 148 configured exclusions.

**The isolated canonical Babylon.js consumer passes the full visualization
catalog; the unchanged bundle and Babylon-Lite do not fully pass.** Dependency
pins and the default Babylon.js bundle remain unchanged. The validation and
diagnostic runs themselves made no commits or pushes.

## Historical PR publication scope (before monorepo integration)

This report accompanies the BabylonNative-only update to
[#1781](https://github.com/BabylonJS/BabylonNative/pull/1781).
**This describes the earlier publication, not the current monorepo setup.**
The passing integration results below are not passing results for a clean
checkout of the PR:

- JsRuntimeHost remains pinned to `33e4a233dea178712352ec90a647683084bfbf1d`.
  Its local Blob/File, Streams/Compression, microtask, clone and URL fixes
  are not included.
- UrlLib remains transitively pinned by that host to
  `0c991337a1160ba7a2d062bf8e342d0a66f48dc9`.
  The transport fixes were not included in that publication.
- No local dependency source override or canonical Babylon.js font-backport
  bundle is installed by the published source. Conditional Compression
  initialization consumes the host target only if it exists; it does not
  supply the missing implementation.

The pending master merge includes the upstream bgfx.cmake update at
`86975512f5945d8e3f6b2898dc2bfa4ed58051c8`; this is separate from the excluded
UrlLib/JsRuntimeHost fixes. Known NativeDawn and harness gaps below remain
unresolved even with the tested overrides. Build-dir logs, screenshots and
diagnostic runner copies remain local, ignored artifacts, not files shipped
by the PR. The report and BabylonNative parity tooling are included.

## Tested sources and environment

The consumer was rebuilt with the local UrlLib source override at the clean,
published revision containing the transport fixes,
commit `4dabd8dbd65d7406bf80d9c5b3f8d2108b0e17ef`.
This includes missing Windows response-MIME handling, data-URL GET decoding,
case-insensitive request content-header routing, and byte-preserving POST bodies.
These are consumer integration tests, not a rerun of UrlLib's separate platform
unit-test suites.

| Component | Tested identity |
|---|---|
| BabylonNative HEAD at validation | `cbb2f4eee370e33694f8f7dc294410d7b37fbbdc`, plus the then-uncommitted changes |
| Pending master merge | `86975512f5945d8e3f6b2898dc2bfa4ed58051c8` |
| BabylonNative source-patch SHA256 | `7da2acd6de2ac9e4fc229e67e90bd4da3965cbe799fb89e7ecd4011cdfa9e778` |
| UrlLib | `4dabd8dbd65d7406bf80d9c5b3f8d2108b0e17ef`, clean source override |
| JsRuntimeHost HEAD | `33e4a233dea178712352ec90a647683084bfbf1d`, plus local capability fixes |
| JsRuntimeHost source-patch SHA256 | `675da9a5ab3c5c465ffbe609f5dc9c49948dd2a55789bfcd1a3dcf4d57a4bc2c` |
| Babylon-Lite HEAD | `206de862a935c4ceace9aaf0107062f1effbd5fa` |
| Native Playground executable SHA256 | `90af372a652e8f64bf1978ed3bb642021e8bfd3c27252866b4ed5ceac971d792` |
| Unchanged Babylon.js 9.21.2 bundle SHA256 | `d43d29f8b0bb6ad3c0184afbd4c9c22e63a9ebc68c827b0b5f5832e8067a20f6` |
| Canonical font-backport bundle SHA256 | `4f8a3a9ac2ca2b51fe444ac1be118ba0bc5cd39c61c11408a74d1d18b82d9018` |

Windows x64, V8, Dawn `v20260209.194954` / D3D12, NVIDIA GeForce RTX 4060
hardware adapter; MSVC 19.51, Ninja, RelWithDebInfo. Lite used Node **22.22.0**,
pnpm **11.23.0**, and actual Chrome **155.0.8059.39** at 1280x720, DPR 1, sRGB.
Its browser Babylon.js oracle uses the locked **9.28.0** packages; the native
Babylon.js visualization consumer is **9.21.2**.

The GPU suites ran sequentially, using separate executable and result
directories. The Lite library and all **262/262 native scene bundles** were
rebuilt before execution. The local lab server was stopped afterward.

Exact source patches, new-file hashes, CMake override paths, and package
manifests are recorded in
[run provenance](../../build/urllib-validation-20261007/provenance.json).
The BabylonNative fingerprint excludes Markdown files to avoid self-referential
report hashes.

## Babylon-Lite: full results and remaining gaps

**537/537 scheduled cases completed across 537 attempts; all 262 catalog scenes
were attempted natively.** The overall 477 passes are not 477 native rendering
passes:

| Coverage group | Executed | Passed | Failed | Timed out |
|---|---:|---:|---:|---:|
| Scene cases reaching NativeDawn | 272 | 214 | 53 | 5 |
| Scene cases running only Chrome-oracle substeps | 3 | 3 | 0 | 0 |
| Non-scene checks | 262 | 260 | 2 | 0 |
| Total | 537 | 477 | 55 | 5 |

The 260 passing non-scene checks are browser bundle-size checks, not native
rendering coverage. The two other non-scene tests, Ocean and PBR gamma rebuild,
are blocked by the adapter's unsupported non-catalog URLs before native
navigation. They are not valid native feature verdicts.

Distinct catalog-scene verdicts are **202 visual passes, 3 behavioral passes,
21 pixel mismatches, 18 runtime failures, 8 readiness/deadline failures,
5 harness limitations, 2 dimension mismatches, and 3 other assertion failures**.
These are scene counts, not test-case counts: multiple cases can belong to one
scene, and a failed readiness predicate can be recorded as `failed` rather than
Playwright's `timedOut`.

| Remaining group | Scenes / evidence |
|---|---|
| Pixel mismatches, 21 scenes | 8, 47, 48, 50, 55, 57, 92, 93, 95, 96, 97, 100, 101, 104, 105, 106, 113, 116, 213, 265, 290 |
| Missing Worker support | Gaussian-splatting scenes 120-129 and 226 |
| Missing indirect-draw entry point | 16 and 165: `pass.drawIndexedIndirect is not a function` |
| Other runtime errors | 91: runner observes WASM fallback error output; 153: no completed framebuffer; 227/228: additional canvas lookup fails; 306: missing `captureStream` |
| Readiness/deadline failures | 30, 36, 112, 164, 180, 181, 211, 315; device recovery in 164 is not verified |
| Unsupported adapter interactions | CSS in 4/22; pointer injection in 221/222; click injection in 304 |
| Incompatible image dimensions | 102/103: 1280x720 native output versus 1359x765 reference; no overlap-only comparison |
| Other original assertions | 115: picking face ID is -1; 187: SMAA readiness; 300: Sprite2D bridge readiness |

Representative original assertions pass for BoomBox, ChibiRex, Shader Balls,
scene 40 physics, scene 51 premultiplied sprites, and scene 283's live query/
animation checks. Scene 40 measures full-image MAD **0.3218 <= 0.5**, and
scene 51 **0.0964 <= 0.1**. Scene 50 still fails at **0.0656 > 0.02**; scene 100
still fails at **0.7207 > 0.5**.

No native crash/assert or tagged data-URL failure markers were found in this
run's native logs. That does not establish complete API support or prove that
every remaining difference is caused by NativeDawn rather than the oracle,
host, or adapter.

The original assertions, thresholds, queries, and readiness requirements were
retained. Copied references are isolated; original force/live Chrome captures
remain browser-generated. The native adapter uses fresh Playground processes,
explicitly rejects unavailable interactions, and caps per-test deadlines at
60 seconds. Quarantined scenes are attempted rather than silently skipped.
No new browser-Lite diagnostic replay was performed; earlier replay annotations
are deliberately excluded from the fresh report.

Full per-scene reasons, measurements, screenshots, and native-log links:
[Babylon-Lite detailed report](../../build/urllib-validation-20261007/lite/report.md).
Structured results:
[summary](../../build/urllib-validation-20261007/lite/summary.json),
[case records](../../build/urllib-validation-20261007/lite/cases.jsonl),
[metrics](../../build/urllib-validation-20261007/lite/metrics.jsonl),
[complete log](../../build/urllib-validation-20261007/lite/playwright.log).

### Readiness/deadline investigation

The eight readiness/deadline cases are not eight slow downloads. Separate,
observational native runs identified decoder-loading, device-lifecycle and
missing HTML-control failures. These diagnostic runs **do not replace the
original 537-case results** and are not new pixel-parity passes.

The original limits differ:

| Scene | Original failing stage | Effective limit |
|---|---|---|
| 30, volume testing | Wait for `canvas.dataset.ready` | 30s predicate |
| 36, Basis | Initialization/readiness; no pixel comparison | 60s overall cap; source spec allows 120s overall / 90s readiness |
| 112, FlightHelmet KTX | Initialization/readiness; no pixel comparison | 60s overall cap; source spec allows 180s |
| 164, device recovery | Pre-loss screenshot succeeds; then waits for `dataset.deviceLost` | 30s loss predicate |
| 180/181, text demos | Execution-only demo initialization; no upstream pixel spec | 60s overall cap |
| 211, BrainStem meshopt | Wait for `dataset.ready`; never reaches animation-frozen assertion | 30s predicate |
| 315, coastal cliff | `waitForCanvasReady`, before screenshot | 60s overall cap; source spec allows 120s overall / 90s readiness |

**Decoder script loading:** scene 30 receives the complete 4,415,856-byte GLB
with HTTP 200 at 1.817s, then appends `/draco_decoder.js` at 1.819s.
Scene 36 receives all 52,496 Basis bytes at 0.922s, but the earlier
`basis_transcoder.js` append never installs `BASIS`. Scene 112 receives the
glTF, binary and every requested KTX2 body by 0.553s, but the decoder script
appended at 0.197s never installs `KTX2DECODER`. Scene 211 receives its glTF
and binary by 0.082s, then appends `meshopt_decoder.js` at 0.083s without
installing `MeshoptDecoder`. Scene 315 receives its complete 4,890,384-byte
coastal-cliff GLB at 0.583s and appends `/draco_decoder.js` at 0.585s;
its environment and BRDF bodies also completed successfully. It is blocked
before task registration/rendering, not in the final 15-frame settling loop.
All five still have zero scheduled/completed animation-frame callbacks and
undefined corresponding decoder globals at their diagnostic deadlines.

This matches the actual loader implementations: each awaits a dynamically
appended script's `onload`/`onerror`, but NativeDawn's document-head
`appendChild` is a no-op. The initialized native runtime does not execute those
scripts, so neither callback settles the loader promise. Increasing the timeout
does not provide the missing loading behavior. The fix belongs in explicit,
host-independent decoder initialization or the shared host's real script-loading
capability, not a decoder-specific NativeDawn global injection. Decoder/WASM
execution, decoded content and pixel output still need verification after that
first gate is implemented; these observations do not prove the later stages work.

**Device recovery, scene 164:** the diagnostic run loads the scene, freezes the
animation, saves a real pre-loss screenshot and sets `captured=true`.
`GPUDevice.destroy()` is actually called at 2.591s, but `device.lost` never
resolves and no `deviceLost`, `deviceRecovered` or replacement-resource flags
appear. The old scene continues rendering, reaching 1,796 frames by the
33.332s observation endpoint. This is an implementation gap, not a stalled RAF
or shader: NativeDawn currently implements `destroy()` as a no-op and creates
a `lost` promise with no resolving path. Genuine device ownership, destruction,
loss notification and replacement/recovery need implementation. No post-loss
image invariance or resource reconstruction is established. The diagnostic
[pre-loss screenshot](../../build/urllib-validation-20261007/readiness/scene164-before-loss.png)
is retained separately from the original full-run screenshot.

**Text demos, scenes 180/181:** both successfully load all 876,576 bytes of
`Inter.ttf` with HTTP 200, then reject with
`TypeError: Cannot read properties of null (reading 'value')`.
The missing element is `textInput`; scene 180's sliders and labels are also
absent. The native bridge evaluates the scene bundle without materializing
these pages' HTML controls. Their terminal `void run()` discards the rejected
promise, so the original records show a deadline rather than the underlying
stack. Adding rejection reporting only to separate diagnostic bundle copies
exposes the error within approximately 1.4s. This needs an honest host/adapter
UI capability or a supported explicit non-DOM text entry point, not a longer
font timeout or dummy controls reported as parity. Text rendering itself was
not reached.

Evidence, including per-second datasets, completed body transfers, script
appends, device lifecycle observations and the terminal stacks:
[Lite readiness diagnostics](../../build/urllib-validation-20261007/readiness/lite-readiness.json),
[diagnostic bundle identities](../../build/urllib-validation-20261007/readiness/bundle-identities.json),
[driver](../../build/urllib-validation-20261007/readiness/probe-lite.mjs),
[telemetry](../../build/urllib-validation-20261007/readiness/telemetry.js).
Only diagnostic copies of scenes 180/181 add terminal rejection reporting;
the other six diagnostic scene bundles are byte-identical to the tested
originals. The bridge temporarily copies these into its runnable consumer;
the two modified text bundles were restored afterward. All **1,674 recorded
files across the three original consumers** match their pre-run hashes.
Scene sources, executable, references and assertions remain unchanged.

## Babylon.js WebGPU visualization

The unchanged 9.21.2 consumer records these five failures:

| Test | Failure | Divergent pixels / original allowance |
|---|---|---|
| GUI Near Menu | Did not converge within 240 render-loop ticks | No completed pixel comparison |
| GUI Force Resize Width | Pixel mismatch | 8044/240000, 3.352% / 2.5% |
| FrameGraph nrg gui bloom | Pixel mismatch | 6120/240000, 2.550% / 2.5% |
| FrameGraph gui bloom | Pixel mismatch | 6104/240000, 2.543% / 2.5% |
| Iridescence NME | Pixel mismatch | 29506/240000, 12.294% / 2.5% |

The separately packaged canonical font-line-box backport then passes the
complete **714/714** catalog. It changes only the canvas font-offset fallback
in the exact 9.21.2 bundle; it does not replace the whole package, change
Engine.Version, or install a runtime plugin workaround.

Both full runs use the same executable, fixed UrlLib, JsRuntimeHost source,
runner, catalog, references, and comparison settings. The runner SHA256 is
`cc74003f969793d87464b3d2b8651a0648d7f31356bcd7e0c7009ec71afb3214`;
the catalog SHA256 is
`2887256e1d6404219ca7248fb7020cda5181bbfa3a3e5011eac1a02ce5f48500`.
The canonical consumer is a labelled local integration experiment, not a
verdict for the unchanged default dependency pins.

The font correction addresses the known headless GUI line-layout issue.
Sequential asset/loading timing also differs between runs: the passing
canonical Near Menu and Iridescence results do **not** prove that font metrics
fixed those intermittent failures. These runs are likewise not a controlled
UrlLib-only comparison; existing NativeDawn and shared-host fixes are included.

Logs, summaries, native result/diff images, and tested packages:
[unchanged-bundle summary](../../build/urllib-validation-20261007/standard/summary.json),
[unchanged-bundle log](../../build/urllib-validation-20261007/standard/validation.log),
[canonical summary](../../build/urllib-validation-20261007/canonical/summary.json),
[canonical log](../../build/urllib-validation-20261007/canonical/validation.log).
The canonical directory also contains the upstream patch, regeneration script,
and bundle provenance.

### GUI Near Menu: intermittent convergence, not a pixel failure

The original failure is the **240 render-loop-tick convergence guard**, not the
separate **600s initial resource-readiness deadline**. The guard checks the main
scene and associated utility scenes for `scene.isReady()`, GUI texture readiness,
dirty material defines and unfinished effects. On a blocked tick, it increments
scene render IDs but does not render the scene or advance the test's frame index.
Consequently the original failed attempt never reaches its 60 comparison frames
or saves a rendered screenshot. Repeated error text in the log is banner/log
mirroring, not three independent attempts.

Three fresh isolated repetitions with the unchanged 9.21.2 bundle and original
runner produced **one convergence failure (8.244s), then two passes (2.499s and
2.439s)**. Both successful pixel comparisons measured **552/240000 = 0.230%**,
well below the unchanged 2.5% allowance. The passing screenshot is preserved
[separately](../../build/urllib-validation-20261007/readiness/gui-near-menu-original-attempts-2-3.png);
it is not evidence that the original full-catalog failed attempt captured a frame.

An additional five isolated repetitions used a separately named, reason-only
diagnostic runner. It retains the original 240-tick/600s limits, frame progression,
consumer bundle and comparison settings. **All five passed**, each at the same
0.230% pixel difference; none entered a blocked convergence guard. Therefore the
exact blocking predicate/object of the intermittent failure is **not yet
identified**. Later quick passes are consistent with loading/compilation timing,
but no cold-cache or controlled-network experiment was performed, so cache
warm-up is not a proven cause. The scene constructs a GUI3DManager, NearMenu and
three TouchHolographicButtons; no evidence ties its failure to UrlLib's POST fix
or proves the font backport resolves it.

The original fresh full-catalog verdict remains failed. The next meaningful
investigation is to reproduce that failure with the guard diagnostics enabled,
not simply raise the deadline or count retries as a pass. No RenderDoc capture
was attempted because the failing attempt never reached pixel comparison.
Evidence:
[original failed isolated run](../../build/urllib-validation-20261007/readiness/gui-near-menu-original-1.log),
[original passing run](../../build/urllib-validation-20261007/readiness/gui-near-menu-original-2.log),
[instrumented run](../../build/urllib-validation-20261007/readiness/gui-near-menu-diagnostic-1.log),
[diagnostic runner generator](../../build/urllib-validation-20261007/readiness/prepare-gui-probe.mjs),
[derived runner](../../build/urllib-validation-20261007/readiness/gui-derived-runner.js),
[runner hashes](../../build/urllib-validation-20261007/readiness/gui-runner-provenance.json).
All three original and five instrumented logs are in the same `readiness` directory.
The temporary GUI runner was removed from the original consumer afterward;
the generator recreates it for a labelled diagnostic repetition.

### Iridescence NME: reproduced material-assignment readiness race

The exact scene is [#2FDQT5#1507](https://playground.babylonjs.com/#2FDQT5#1507).
Its failed image contains a **black sphere**, not a subtly incorrect iridescence
color. The reference and canonical passing frame show the environment-reflecting
iridescent sphere. At `(300,200)`, the failed RGBA is `(0,0,0,255)` versus
`(94,98,57,255)` in both the reference and passing frame.

The scene returns synchronously while this asynchronous work is outstanding:
`NodeMaterial.ParseFromSnippetAsync("Q9NP6V#0", scene).then(...)` assigns
`sphere.material` only when the promise finishes. The catalog has no
`renderCount`, so comparison occurs on **frame 1**. Scene/material readiness
can succeed while the sphere still has no explicitly assigned material; the
runner cannot infer that an unawaited application promise will replace it.
The scene creates no lights, so that temporary default-material sphere is black.

Separate native runs used an isolated copy of the unchanged 9.21.2 consumer,
with identical executable, catalog, reference, first comparison frame,
600s/240-tick readiness limits, channel threshold 25 and 2.5% allowance:

| Experiment | Attempts | Result | Divergent pixels |
|---|---:|---|---:|
| Original scene and original runner | 3 | 3 passed | 600/240000, 0.250% |
| Original scene, material-timing observations only | 3 | 3 passed; NodeMaterial assigned before capture | 600/240000, 0.250% |
| Original scene, real material-promise fulfillment held for 2s | 3 | 3 failed; material still null at frame-1 capture | 29506/240000, 12.294% |
| Await assignment before returning, preserve setup order, same 2s delay | 3 | 3 passed; NodeMaterial assigned at capture | 600/240000, 0.250% |
| Same order-preserving await, without added delay | 3 | 3 passed | 600/240000, 0.250% |
| Initial inline-await controls that moved skybox setup after material construction | 2 | 2 failed; ordering-confounded controls, not the proposed fix | 29506/240000, 12.294% |

**All three delayed/unawaited frames are RGBA pixel-for-pixel identical to the
original full-catalog failure.** At their capture times (0.505s, 0.670s and
0.520s from runner start), diagnostics show `materialAssigned=false`,
`resolved=0`, `pendingResources=0` and `compareFrame=1`. The delay holds the
actual successful parser's promise delivery; it does not substitute a fake
material, alter the network response, or increase the rendering frame count.
These controlled runs establish a reproducible application-readiness failure
mechanism matching the original image. The original full-catalog log did not
record material state or request latency, so its exact late-request timing
cannot be reconstructed.

**Every passing frame, including the unchanged 9.21.2 repetitions, is RGBA
pixel-for-pixel identical to the earlier canonical consumer's passing frame.**
The original 29,506 divergent pixels consist of 28,906 sphere-region pixels
within `x=204..395, y=103..296`, plus 600 pixels on the final image row.
The passing frames retain only those 600 final-row differences (0.250%, below
the original allowance). This investigation did not diagnose or modify that
separate last-row discrepancy. It found no persistent iridescence-color
difference once the real material was installed, and provides no reason to
attribute this failure to the font-metrics correction.

The appropriate correction is **in the playground scene's initialization
contract**, not a NativeDawn shader change, a timeout increase, a larger
`renderCount`, or a renderer-wide node-material interception. Make its existing
`createScene` asynchronous and await the actual material-assignment promise
**after the original skybox/environment setup and before returning the scene**:

```javascript
const materialReady = BABYLON.NodeMaterial.ParseFromSnippetAsync("Q9NP6V#0", scene)
    .then(nodeMaterial => { sphere.material = nodeMaterial; });
scene.createDefaultSkybox(environmentTexture, true, undefined, 0.3, true);
await materialReady;
return scene;
```

Setup order matters: simply adding an inline `await` before
`createDefaultSkybox` moved environment setup until after node-material
construction and also produced black frames despite an assigned NodeMaterial.
The graph's `ReflectionBlock` falls back to `scene.environmentTexture`.
Those two unsuccessful controls are retained, rather than omitted or counted
as validation of the correction. The order-preserving experiment changes only
when the scene is returned; all original setup remains in its original order.

No production scene/catalog, NativeDawn implementation, default bundle,
dependency pin, reference or threshold was changed for this investigation.
The proposed scene correction was exercised only in separately labelled
diagnostic runners. The original full-catalog failure remains recorded as failed.
No RenderDoc capture was needed to identify this readiness mechanism.

Evidence:
[structured image/timing analysis](../../build/urllib-validation-20261007/iridescence/analysis.json),
[delayed original-scene log](../../build/urllib-validation-20261007/iridescence/delayed-unawaited-1.log),
[delayed black-sphere reproduction](../../build/urllib-validation-20261007/iridescence/delayed-unawaited-1.png),
[order-preserving delayed log](../../build/urllib-validation-20261007/iridescence/delayed-awaited-preserving-order-1.log),
[order-preserving passing frame](../../build/urllib-validation-20261007/iridescence/delayed-awaited-preserving-order-1.png),
[unchanged isolated log](../../build/urllib-validation-20261007/iridescence/original-1.log),
[runner generator](../../build/urllib-validation-20261007/iridescence/prepare-probe.mjs),
[runner provenance](../../build/urllib-validation-20261007/iridescence/runner-provenance.json),
[initial-control source provenance](../../build/urllib-validation-20261007/iridescence/first-runner-provenance.json),
[copied-consumer identities](../../build/urllib-validation-20261007/iridescence/consumer-identities.json).
All 17 logs, frames, original/corrected diagnostic runner sources, and public
scene/node-material JSON payloads are retained under
`build\urllib-validation-20261007\iridescence`.

## Reproduction and integrity

The retained runnable consumers are under
`build\urllib-validation-20261007\standard\consumer`,
`build\urllib-validation-20261007\canonical\consumer`, and
`build\urllib-validation-20261007\lite\consumer`.
From either Babylon.js consumer directory:

```powershell
.\Playground.exe --headless app:///Scripts/validation_native.js
```

For Lite, start the lab in one terminal:

```powershell
Set-Location build\lite-parity\lab
npx --offline --package node@22.22.0 --package pnpm@11.23.0 -- pnpm exec vite --host 127.0.0.1 --port 5179 --strictPort
```

From the repository root in another terminal:

```powershell
Remove-Item Env:NATIVE_PARITY_BROWSER,Env:NATIVE_PARITY_REPORT,Env:RECAPTURE_GOLDEN,Env:CI -ErrorAction SilentlyContinue
$env:REUSE_BROWSER = '0'
Set-Location build\lite-parity
npx --offline --package node@22.22.0 --package pnpm@11.23.0 -- pnpm exec playwright test --config .native-parity-urllib-20261007\playwright.config.mjs
```

Use a new artifact directory when preparing another independent run so these
logs and first-attempt outcomes remain preserved. To regenerate this run's
detailed report from the repository root without importing old browser replay
results:

```powershell
node .github\scripts\report-lite-parity.mjs build\lite-parity build\urllib-validation-20261007\lite build\urllib-validation-20261007\lite\report.md build\urllib-validation-20261007\browser-lite-not-run
```

Post-run checks confirm **all 893 original Babylon.js reference assets, all
108 original Lite reference files, and every pre-recorded tested-package file
remain unchanged**. Reference generation was not used. No UrlLib/JsRuntimeHost
dependency pin, catalog exclusion, reference, or threshold was edited for these
runs. Subsequent publication does not convert override-based results into
validation of the unchanged pins. This report establishes Windows/V8/D3D12 results only,
not Apple/Linux or other-engine validation.

## Master refresh: clean repository pins (2026-10-07)

Master `3428526f143f4eec86c4cf62d5bc493c1afca159` is integrated locally on
previously published HEAD `bcc059023d722e23f640f8e0a0c8c01440d54a68`. The resolved
master merge and subsequent integration fixes are included in this PR refresh.
The tested snapshots were uncommitted when executed; their recorded source
fingerprints remain the exact reproduction identities after publication.
Incoming changes include Babylon.js 9.29.0,
native vertex-layout reuse, bgfx.cmake/minz updates, Canvas/Image lifecycle
improvements and upstream validation re-enablings.

Both local dependency overrides were cleared before configuring and rebuilding.
This pass uses clean fetched **JsRuntimeHost
`2f8b9be3a4d691d4a76c85c9c856319eb25f744c`** and **UrlLib
`0c991337a1160ba7a2d062bf8e342d0a66f48dc9`**. Master's host update does not
include the separate uncommitted capability/URL fixes. The transport fixes
were excluded from that historical run.
The app lockfile was restored with `npm ci`; the tested Babylon.js bundle is
the normal repository-downleveled **9.29.0** package, with no isolated font
backport or diagnostic source replacement.

### Complete fresh results

| Suite / consumer | Executed | Passed | Failed | Timed out | Skipped | Duration | Exit |
|---|---:|---:|---:|---:|---:|---|---:|
| Babylon-Lite full original schedule | 537 | 439 | 92 | 6 | 0 | 36m 15.240s | 1 |
| Babylon.js WebGPU, initial merged runner | 727 | 477 | 250 | 0 | 135 | 16m 59.681s | -1 |
| Babylon.js WebGPU, corrected Dawn input canvas | 727 | 478 | 249 | 0 | 135 | 14m 17.116s | -1 |
| Shared Canvas focused native regressions | 15 | 15 | 0 | 0 | 0 | 1.832s | 0 |
| NativeDawn Babylon.js host checks | 20 | 20 | 0 | 0 | 0 | 1.108s | 0 |

Neither visualization invocation reached its external safety deadline; both
completed all selected tests and reported **zero missing references**. The
final 249 failures are all pixel-comparison failures. These are actual
failures, not a passing renderer result. The 15 shared Canvas tests run through
BGFX/D3D11 and must not be described as 15 independent Dawn tests.

The standalone NativeDawn runtime invocation fails at
`dawn_runtime_native.js:548` with
`Empty-valued URL query flags retain presence and search`. It exits -1 before
the remaining checks complete; the earlier override-based **111/111** result
does not apply to this pinned invocation.

The merged catalog has 862 entries: **727 run, 135 are excluded** on WebGPU.
Compared with the earlier 714-scene run, the 13 newly runnable scenes are:
SOGS with SH; Nested BBG; Camera rig; ShadowOnlyMaterial; Draco Mesh Compression
(fallback); Instances manual update + motion blur; Thin instances + motion
blur; Picking Visual Test; GUI Input Text Area Inside ScrollViewer; GUI Images
in Grid; Sprites Pixel Perfect; water-material; FrameGraph nrge frozen meshes.
The separate master-only catalog/backend denominator is not used here.

Lite completed **537/537 scheduled cases in 537 attempts**, without retries or
skips, and attempted all 262 catalog scenes. Native-reaching scene cases are
**176/272 passing**; three additional scene cases are browser-oracle-only
passes. Of 262 non-scene checks, 260 pass; the two failures are explicitly
unsupported adapter routes for `demo-ocean.html` and `/`, not demonstrated
renderer failures.

Scene verdicts are 170 PASS, one BEHAVIOR PASS, four HARNESS LIMIT, 13 PIXEL
MISMATCH, three CRASH, seven FAIL, nine TIMEOUT, 22 ASSET / FETCH FAIL,
31 RUNTIME FAIL and two DIMENSION MISMATCH. These scene classifications are
not the same denominator as the six Playwright timed-out cases.
The three crashes (scenes 12, 186 and 304) retain native stacks in
`HttpMediaTypeHeaderValue::ToString` / `UrlLib::UrlRequest::Impl::LoadHttpAsync`,
matching the absent-response-MIME defect excluded from this pinned consumer.
Readiness/deadline failures remain visible rather than being converted to
synthetic successful frames.

### Integration corrections and remaining blockers

The first build exposed duplicate Windows output rules for the byte-identical
`DroidSans.ttf` and Dawn-only `droidsans.ttf`. Removed redundant Dawn packaging
and use the common upstream asset, with canonical URL capitalization. No
font bytes or reference images were substituted. Provenance capture now
resolves fetched dependency sources when no override is configured.

The initial full run also exposed a merge-introduced input regression:
upstream BGFX's `getRenderingCanvas() => window` /
`getInputElement() => 0` overrides had replaced Dawn's real canvas methods.
Restricting those overrides to non-Dawn engines fixes the
`getBoundingClientRect` TypeError. **Picking Visual Test passes** in a focused
native run and the second complete catalog run. First-run evidence is retained.
Lite does not load `validation_native.js`, so that visualization-only guard
does not invalidate or require repeating its completed run.

The large new visualization failure cluster includes explicit Cube/2D shader
binding-dimension errors, invalid render pipelines and invalid command buffers.
A diagnostic-only observer forwards device descriptors unchanged and records
the actual WGSL, layouts and pipelines. It found invalid JavaScript
`texture.viewDimension` values containing `{groupIndex, bindingIndex}` rather
than WebGPU dimension strings, **before NativeDawn translates the descriptors**.

An isolated, GPU/network-free reproduction localizes a concrete packaging
defect to the upstream ES5 downlevel step: TypeScript 5.9.3 emits the same
`_40` name for the WGSL processor's dimension value and its destructuring
temporary. The later assignment overwrites the dimension before
`_addTextureBindingDescription` is called. The authoritative 9.29.0 source
returns `"2d"`, `"cube"` and `"2d"` for the three tested texture kinds; the
tested packaged copy returns binding-coordinate objects for all three.
Full-program emission and a diagnostic TypeScript 6.0.3 emission also retain
the collision, so neither was adopted as a fix. The original package and
reproducible extraction test are retained separately; tested consumers were
not modified.

This is a proven pre-GPU descriptor-construction fault, **not proof that every
one of the 249 pixel failures has that sole cause**. The canonical downlevel
pipeline needs correction while preserving legacy-engine support; inferring
dimensions or rewriting shaders in NativeDawn would conceal the defect.
NativeDawn's existing non-string enum defaulting also masks this invalid input
and needs proper API validation separately. No speculative renderer workaround,
bundle swap, threshold relaxation or new exclusion was applied. The historical
9.21.2 Iridescence material-readiness diagnosis is not evidence that its new
9.29.0 failure has the same cause.

### Retained evidence and reproducibility

Artifact root:

```text
C:\Users\cedric\dev\copilot-worktrees\BabylonNative\cedricguillemet-supreme-waffle\build\master-validation-20261007
```

- [Corrected full summary](../../build/master-validation-20261007/run-summary-corrected.json),
  [initial full summary](../../build/master-validation-20261007/run-summary.json) and
  [complete Lite scene report](../../build/master-validation-20261007/lite/report.md).
- [Corrected visualization log](../../build/master-validation-20261007/standard-corrected/validation.log),
  [Lite full-suite log](../../build/master-validation-20261007/lite/full-suite.log) and
  [focused input result](../../build/master-validation-20261007/standard-corrected/picking-smoke.log).
- [Recorded binding descriptors](../../build/master-validation-20261007/binding-probe/probe.log),
  [descriptor analysis](../../build/master-validation-20261007/binding-probe/analysis.json),
  [isolated emitter reproduction](../../build/master-validation-20261007/binding-probe/emitter-reproduction.json)
  and [reproduction script](../../build/master-validation-20261007/binding-probe/reproduce-downlevel-collision.mjs).
- [Corrected provenance](../../build/master-validation-20261007/standard-corrected/provenance.json)
  and [Lite provenance](../../build/master-validation-20261007/lite/provenance.json).

Final visualization result PNGs and red diff overlays are respectively in
`standard-corrected\consumer\Apps\Playground\Results` and
`standard-corrected\consumer\Apps\Playground\Errors` under that root.
There are 704 result files and 242 diff files; multiple catalog tests share
reference-image filenames, so these are not 727 and 249 unique screenshots.
The complete log identifies every failed title. Lite screenshots are under
`lite\references\<scene-slug>\`; its detailed report links individual evidence.

Both native consumers use executable SHA256
`e365fe841a3ca87e1ad2261eee55d77d3bf584eb75d104a02ce0eda73f23866c`.
The tested downleveled 9.29.0 bundle is
`dadfff66301f4bd3ad3137a5528be398ab1a164fd57878eea72930eb102a24f5`.
The initial/Lite BabylonNative source fingerprint is
`752dfe9ca215cf3b58a81ef64dc033a1ad2c3003b2a854dfce75bd26228feeb7`;
the corrected visualization fingerprint is
`0faa2449fbd3c82371687c564b9d78f8665433d9327b34a4c3092932894c91da`.
Lite provenance retains its actual earlier source snapshot and records the
actual Lite executable path after byte-identity verification.

Post-run integrity verification covers 935 corrected visualization consumer
files, 1,460 Lite consumer files, all 893 original visualization reference
assets and all 108 original Lite reference files. Both fetched dependencies
remain clean at their recorded pins. Original assertions, thresholds and
reference policies were retained. GPU suites ran sequentially; all owned
test processes finished and the owned Lite server was stopped.

These results are not a controlled single-variable comparison with the
earlier fixed-UrlLib/capability/font-backport runs: master, Babylon.js and
dependency source identities changed. They establish the current
Windows/V8/D3D12 pinned-consumer result, which remains failing.

## Master refresh (2026-10-19)

Integrated master `af8d8aa1c6f3749b1eabd7eca959b47976b9f351` into the same
NativeDawn branch, starting from `2222ce8390826aa59d7e39bb4fbaaa6e4e9b915a`.
Resolved the CI, catalog and build-documentation conflicts. The merged CI
retains master's V8-only Windows and JavaScriptCore-only Linux visualization
rules without accidentally running NativeEngine validation in plugin-only
Dawn jobs. The new `ci-complete` aggregate also depends on all five Dawn jobs.
The reviewed catalog resolutions retain incoming master metadata and
workarounds, alongside non-conflicting branch-specific WebGPU coverage.

The rebuilt Windows/V8 consumer uses clean fetched JsRuntimeHost
`980cfb0197f7fda661d362df4b275cff42d1e7f2` and UrlLib
`0c991337a1160ba7a2d062bf8e342d0a66f48dc9`. No local dependency override,
font-backport bundle, reference generation or test retry is included.
Master's host-controlled script-loading update is not the separate
unmerged URL/capability patch. Existing transport/capability fixes remain
excluded from publication.

| Local suite | Results | Duration | Exit |
|---|---|---|---:|
| Babylon.js 9.29.0 / WebGPU full catalog | 744 executed: 497 passed, 247 failed; 118 skipped; zero missing references | 21m 41.554s application time | -1 |
| Babylon-Lite original full schedule | 537 executed: 435 passed, 95 failed, 7 timed out; zero skipped | 35m 03.217s invocation wall time | 1 |
| Dawn Babylon.js host regression | 20/20 checks passed | 1.023s | 0 |
| Shared BGFX/D3D11 Canvas regressions | 17/17 tests passed | 2.184s | 0 |
| Picking Visual Test smoke | 1/1 passed | 5.684s | 0 |

The parity-tooling regression tests also pass 8/8. Both complete GPU suites
ran sequentially. Lite's library and all 262 native scene bundles were rebuilt
from the same recorded Lite checkout, `206de862a935c4ceace9aaf0107062f1effbd5fa`.
Its actual Chrome version is 155.0.8059.80, with the locked Babylon.js 9.28.0
oracle, 1280x720, DPR one and sRGB; its 60-second adapter deadlines are unchanged.

**Neither full suite passes.** The WebGPU catalog has 246 pixel failures,
including the existing shader/layout binding-dimension error cluster. Its one
non-pixel failure is the newly enabled **Material Plugin** example
`#22HT5Z#10`: Babylon.js rejects its `BlackAndWhite` plugin as incompatible with
the material's shader language. This GLSL-only example is not evidence of a
new native crash; it remains explicitly failed, not silently skipped or
rewritten by the host.

The standalone runtime still fails the assertion
`Empty-valued URL query flags retain presence and search`. It has no 111/111
passing verdict with the published pins. The known pre-GPU ES5 downlevel
descriptor-corruption problem, excluded URL/MIME dependency fixes, and
unsupported native/harness capabilities remain blockers; merging master is
not represented as solving those pre-existing issues.

Lite completes 537/537 scheduled cases in 537 attempts, but only **261/262**
catalog scenes reach native navigation: scene 27 times out while creating its
browser reference before native navigation, so it is an oracle/harness-blocked
case, not a native failure. Native-reaching scene cases are **172/271 passing**;
three other scene cases are browser-oracle-only passes. The two non-scene
adapter route failures remain separate from the 260 passing size checks.
Scene verdicts are 166 PASS, one BEHAVIOR PASS, four HARNESS LIMIT,
13 PIXEL MISMATCH, three CRASH, seven FAIL, one ORACLE / HARNESS BLOCKED,
nine TIMEOUT, 22 ASSET / FETCH FAIL, 34 RUNTIME FAIL and two DIMENSION MISMATCH.
The crashes are again scenes 12, 186 and 304, with the pinned UrlLib
response-MIME stack. Diagnostic retries have not replaced any original result.

Retained artifacts:

- [Complete summary and integrity counts](../../build/master-validation-20261019/summary.json)
  and [source/dependency provenance](../../build/master-validation-20261019/provenance.json).
- [Full WebGPU log](../../build/master-validation-20261019/standard/validation.log)
  and [per-scene Lite report](../../build/master-validation-20261019/lite/report.md).
- [Lite full-suite log](../../build/master-validation-20261019/lite/full-suite.log),
  [runtime failure](../../build/master-validation-20261019/dawn_runtime_native.log)
  and [shared Canvas results](../../build/master-validation-20261019/canvas-tests.log).

The artifact root is
`C:\Users\cedric\dev\copilot-worktrees\BabylonNative\cedricguillemet-supreme-waffle\build\master-validation-20261019`.
WebGPU screenshots and diff overlays are in
`standard\consumer\Apps\Playground\Results` and
`standard\consumer\Apps\Playground\Errors`; Lite images are under
`lite\references\<scene-slug>\`.
Post-run verification confirms all 934 standard-consumer files,
1,459 Lite-consumer files, 893 original visualization reference assets and
108 original Lite reference files match their pre-run hashes, with both
dependencies still clean at their recorded pins.
The executable SHA256 is
`a6b0eef1d3fcb575badcc876824551fa61b0d399de932d395e9454ff7271a953`;
the tested BabylonNative source fingerprint, excluding Markdown, is
`f2f70b10b5e4db62f785c37117960ca52b3af1f227f1f6e4e8d037a6c121505d`.

## CI runtime step disabled and full validation (2026-10-27)

Fetched master again: `a224a31700f673f3cbb22d48741c00be2739104f`
was already integrated, so there were no additional merge conflicts.
Removed the Windows `NativeDawn Runtime Regression Tests` CI step as requested.
The standalone and Babylon.js host scripts remain available locally; the
Lite smoke test and full Dawn visualization CI step remain enabled.
The latter generates images on CI's software adapter rather than claiming
pixel parity with hardware references.

The local full-catalog run exposed a merge-integration regression:
master's validation runner now queries `TestUtils.getJavaScriptEngineName()`
when applying engine-specific exclusions, but Dawn's replacement TestUtils
did not expose that method. Unfiltered execution stopped during initial
catalog filtering, at Gaussian Splatting SPZ SH, before any scene ran.
The method now returns the configured engine name, and a host regression
assertion covers it. The complete authoritative run below was made with
fresh consumers after rebuilding this fix. Aborted startup investigations
are not included in its totals; transient instrumentation was removed.

All configured Windows targets were rebuilt with V8, RelWithDebInfo and no
JsRuntimeHost/UrlLib source overrides. The fetched dependency trees remain
clean at JsRuntimeHost `980cfb0197f7fda661d362df4b275cff42d1e7f2`
and UrlLib `0c991337a1160ba7a2d062bf8e342d0a66f48dc9`.
No separate dependency fix or canonical font backport was imported.

| Local suite | Results | Duration | Exit |
|---|---|---|---:|
| Babylon.js 9.31.3 / WebGPU full catalog | 769 executed: 509 passed, 260 failed; 93 skipped; zero missing references | 24m 43.397s application time | -1 |
| Babylon-Lite original full schedule | 537 executed: 432 passed, 98 failed, 7 timed out; zero skipped | 37m 29.776s invocation wall time | 1 |
| All native unit tests, including shared Canvas | 174/174 passed | 110.115s test time | 0 |
| Parity tooling regressions | 8/8 passed | 98ms | 0 |
| Dawn Babylon.js host regression | 21/21 checks passed | 1.104s | 0 |
| Freshly rebuilt Lite scene1 smoke | Passed; 1.623% differing pixels against the unchanged 5% ceiling | 1.603s | 0 |
| Standalone runtime regression | Failed at empty-valued URL query presence/search | 695ms | -1 |
| ModuleLoadTest | Failed: unexpected `bcrypt.dll`, `ole32.dll`, `shlwapi.dll` | See log | 1 |

**Neither full visualization/parity suite passes.** The WebGPU failures are
259 pixel mismatches and the existing Material Plugin shader-language
incompatibility. The unchanged postinstall/downlevel pipeline still corrupts
texture binding descriptors before NativeDawn receives them. A CPU-only
reproduction against the exact tested 9.31.3 bundle shows that `texture_2d`,
`texture_cube` and `texture_depth_2d` all pass a
`{groupIndex: 1, bindingIndex: 0}` object as the view dimension, instead of
`"2d"`/`"cube"`: TypeScript 5.9.3 ES5 output reuses `_41` for both the
dimension and a loop-local binding. This is not corrected by guessing
dimensions in the native API. It remains a packaging/compiler blocker;
this reproduction does not establish that it causes every pixel failure.

Lite's library and all 262 native bundles were rebuilt from
`206de862a935c4ceace9aaf0107062f1effbd5fa`. The unchanged 537-case schedule
ran once, with one worker, no retries and 60-second deadlines, after WebGPU
completed. Chrome was 155.0.8059.86; Node was 22.22.0; the locked Babylon.js
oracle was 9.28.0, at 1280x720, DPR one and sRGB.
There were 271 native-reaching scene cases, of which 169 passed.
Only 261/262 scenes reached native navigation: scene 27 timed out while
producing its browser oracle. Browser-only passes are not native passes.
Scene verdicts were 163 PASS, one BEHAVIOR PASS, four HARNESS LIMIT,
11 PIXEL MISMATCH, three CRASH, eight FAIL, one ORACLE / HARNESS BLOCKED,
eight TIMEOUT, 39 RUNTIME FAIL, 22 ASSET / FETCH FAIL and two
DIMENSION MISMATCH. These scene-level verdicts are distinct from the
537 case outcomes. The crashes in scenes 12, 186 and 304 still identify
the pinned UrlLib response-MIME path. No thresholds, references, assertions
or exclusions were weakened to change these results.

The runtime URL assertion remains an explicitly recorded failure, not a
pass obtained by disabling its CI step. ModuleLoadTest exercises the shared
BGFX/NativeEngine boot, not Dawn initialization; its three unexpected
Windows modules remain unresolved, and its allowlist was not expanded
without attribution. The already excluded dependency fixes, remaining
native/adapter limitations and packaging problem are not resolved by the
new engine-identity method.

Artifacts are retained locally under
`C:\Users\cedric\dev\copilot-worktrees\BabylonNative\cedricguillemet-supreme-waffle\build\full-validation-20261027-fixed`:

- [Full summary](../../build/full-validation-20261027-fixed/summary.json),
  [source/dependency provenance](../../build/full-validation-20261027-fixed/provenance.json)
  and [descriptor reproduction](../../build/full-validation-20261027-fixed/binding-reproduction.json).
- [Full WebGPU log](../../build/full-validation-20261027-fixed/validation.log)
  and [262-scene Lite report](../../build/full-validation-20261027-fixed/lite/report.md).
- [Lite full-suite log](../../build/full-validation-20261027-fixed/lite/full-suite.log),
  [runtime failure](../../build/full-validation-20261027-fixed/runtime.log)
  and [module-load failure](../../build/full-validation-20261027-fixed/module.log).

WebGPU screenshots and diff overlays are in
`standard\consumer\Apps\Playground\Results` and
`standard\consumer\Apps\Playground\Errors`; Lite images are under
`lite\references\<scene-slug>\`. Post-run verification matched all
934 standard-consumer files, 1,459 Lite-consumer files, 893 original
visualization reference assets and 108 original Lite reference files to
their pre-run hashes. The owned Lite server was stopped after testing.
The tested executable SHA256 is
`b20dad00d6eb4ea7fa86be71e4381210a96caeadbe3c6d7cb9c4f10b5dd281b1`;
the BabylonNative source fingerprint, excluding Markdown, is
`af2bffe5ed160476ec0b35e979c1c3fc4f74458670b081ef553593dde34e4331`.

## Monorepo integration and validation (2026-10-28)

Integrated master `e79a4e30b6b6edc5b3fd58afbb1476d211b98f22` into the
working tree. Resolved Android/iOS/macOS workflow, aggregate-CI and catalog
conflicts, preserving Dawn build routes alongside the new component tests.
Accepted master's newly enabled catalog coverage and retained the branch's
non-conflicting WebGPU coverage and JavaScriptCore exclusions.
The merge and follow-up changes remain uncommitted.

The shared transport fixes are now applied directly to
`Dependencies\UrlLib`: data-URL GET decoding, optional Windows response-MIME
handling, byte-preserving POST bodies on Windows/Apple/Android, and associated
loopback/cancellation/reuse regressions. The imported source revision is
`81ea0404f6d719e2bc306c20d99594c30c434b19`. Monorepo sibling paths,
desktop-only test configuration and newer Android test skips were retained;
the existing root notice already contains the required base-n license.
**No external transport checkout, source override or separate pull request
is required to use these fixes.**

NativeDawn's V8 internal-header path now uses the vendored host rather than
a FetchContent source directory. The Windows Dawn CI target list includes
both component test executables, matching master's new test steps.
Provenance capture now records the in-tree component sources. Build
instructions and the historical parity report no longer direct consumers
to external transport prerequisites.

The Windows x64/V8/RelWithDebInfo build uses the tracked monorepo sources,
Babylon.js 9.32.1 and Dawn/D3D12 on NVIDIA RTX 4060. Playground, native unit
tests, transport tests and ModuleLoadTest built successfully. The aggregate
build was blocked by the host test's symbolic-link fixture creation:
Windows reports that the process lacks the required privilege. Ordinary
host assets were staged separately for diagnosis; no symlinks were replaced
with fake files and no tests were skipped to produce a passing verdict.

| Local suite | Results | Duration | Exit |
|---|---|---|---:|
| Babylon.js 9.32.1 / WebGPU full catalog | 784 executed: 520 passed, 264 failed; 78 skipped; zero missing references | 24m 32.945s application time | -1 |
| Babylon-Lite original full schedule | 537 executed: 451 passed, 81 failed, 5 timed out; zero skipped | 35m 19.658s invocation wall time | 1 |
| Vendored transport tests | 54 passed, 6 existing error-detail skips, zero failures; CTest also passed 1/1 | 7.146s direct test time | 0 |
| Native unit tests, including shared Canvas | 174/174 passed | 83.131s | 0 |
| Parity tooling regressions | 8/8 passed | 92ms | 0 |
| Dawn Babylon.js host regression | 21/21 checks passed | 1.015s | 0 |
| Fresh Lite scene1 smoke | Passed; 1.623% differing pixels against unchanged 5% ceiling | 1.563s | 0 |
| Standalone runtime regression | Failed at empty-valued URL query presence/search | 797ms | -1 |
| Vendored host tests, after ordinary asset staging | 35 passed, 1 skipped, 1 failed aggregate test; embedded JS: 250 passed, 4 pending, 2 missing-symlink failures | 10.697s | 1 |
| ModuleLoadTest | Same three unexpected Windows modules: `bcrypt.dll`, `ole32.dll`, `shlwapi.dll` | See log | 1 |

**The full suites remain failing.** WebGPU has 263 pixel mismatches plus
the Material Plugin shader-language incompatibility. Lite completes all
537 scheduled cases in 537 attempts, using the same fixed Lite checkout
`206de862a935c4ceace9aaf0107062f1effbd5fa` and previously generated 262
native bundles. It runs with one worker, no retries and unchanged
60-second deadlines after WebGPU finishes. Chrome is 156.0.8078.48; Node is
22.22.0; the locked Babylon.js oracle remains 9.28.0 at 1280x720, DPR one,
sRGB. Master, Babylon.js and Chrome changed, so comparisons with October 27
are not a controlled transport-only experiment.

There are 188 passing native-reaching scene cases out of 271.
Native navigation reaches 261/262 scenes; scene 27 remains blocked by
its browser-oracle timeout. Scene verdicts are 180 visual PASS,
2 BEHAVIOR PASS, 5 HARNESS LIMIT, 21 PIXEL MISMATCH, 7 FAIL,
1 ORACLE / HARNESS BLOCKED, 7 TIMEOUT, 37 RUNTIME FAIL and
2 DIMENSION MISMATCH. There are **no native crashes** in this run.
The three prior response-MIME crash cases now reach their actual outcomes:
scene 12 passes its pixel assertions (MAD 0.0247 against 0.027),
scene 186 passes its behavioral assertions, and scene 304 explicitly fails
at unsupported DOM click injection rather than crashing.
Browser-only checks and behavior-only passes are not pixel-parity passes.

The shared-host `?live` URL assertion and remaining native/browser-adapter
limitations are separate from transport. The runtime CI step stays disabled
as previously requested. ModuleLoadTest's allowlist and all test assertions,
thresholds and reference policies remain unchanged.

Retained local artifacts:

- [Full summary](../../build/monorepo-validation-20261028/summary.json),
  [source provenance](../../build/monorepo-validation-20261028/provenance.json)
  and [reviewed import adaptations](../../build/monorepo-validation-20261028/transport-import-reviewed.json).
- [Final provenance-helper check](../../build/monorepo-validation-20261028/final-provenance/provenance.json)
  records the subsequent path-portability cleanup in the capture script;
  the tested executable and consumer inputs are unchanged.
- [WebGPU log](../../build/monorepo-validation-20261028/validation.log),
  [262-scene Lite report](../../build/monorepo-validation-20261028/lite/report.md)
  and [full Lite log](../../build/monorepo-validation-20261028/lite/full-suite.log).
- [Transport CTest](../../build/monorepo-validation-20261028/transport-ctest.log),
  [host fixture failures](../../build/monorepo-validation-20261028/host-unit-tests-assets.log),
  [runtime failure](../../build/monorepo-validation-20261028/runtime.log)
  and [module-load failure](../../build/monorepo-validation-20261028/module.log).

Artifact root:
`C:\Users\cedric\dev\copilot-worktrees\BabylonNative\cedricguillemet-supreme-waffle\build\monorepo-validation-20261028`.
WebGPU screenshots/diffs are in
`standard\consumer\Apps\Playground\Results` and
`standard\consumer\Apps\Playground\Errors`; Lite images are in
`lite\references\<scene-slug>\`.
Post-run verification matched all 934 standard-consumer files, 1,459
Lite-consumer files, 893 original visualization references and 108 original
Lite references against their pre-run hashes, and both vendored component
patches against their captured identities. The owned Lite server was stopped.
Executable SHA256:
`c9eb767211d27c69a9d237357f48bb4444df7bf8457af8f259e69d0664803a7c`.
Tested BabylonNative source fingerprint, excluding Markdown:
`c6ca475e9d19eeafd4fbc9defb2e5c31060cd89d56922e33e77a59671b313dc7`.
