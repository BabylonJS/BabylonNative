# Node-API (N-API) Conformance & Version Roadmap

_Last updated: 2026-06-04 · Tracks PR #116 (`napi-tests`) and the staged path to higher N-API levels._

## Scope & PR discipline

This is a multi-PR effort. Keep the boundaries strict:

- **PR #116 (this PR): land the conformance test suite on the N-API v5 surface that upstream already supports.** That is the whole goal — a regression safety net at the current capability level.
- **Out of scope here — each becomes its own follow-up PR:**
  - **Bug fixes** against the current N-API implementation that the tests surface → *quarantine* the failing test via the allow-list and open a separate fix PR. Do not fix impl bugs in the test-suite PR.
  - **Any `NAPI_VERSION` bump** (6 → 7 → 8 → …) and the per-engine native work it requires.
  - **jsc-android engine bump** (v6 enabler — see below).

> **Android packaging note:** to run the suite in-process the addons are `dlopen`'d as standalone
> `.node` modules, which requires `napi` to ship as a shared library (`libnapi.so`) on Android (see
> *Platform status*). That is a deliberate packaging change for all Android consumers; if upstream
> prefers it isolated, it can land as a small precursor commit/PR that this suite depends on.

## Current state (2026-06-04)

- `NAPI_VERSION` is pinned at **5** via `[BABYLON-NATIVE-ADDITION]` `#define` blocks in
  `Core/Node-API/Include/Shared/napi/{js_native_api.h, js_native_api_types.h, napi.h}`, and `NAPI_HAS_THREADS` is forced to **0**. Upstream `main` is also still 5.
- Recommended early refactor (a later PR): replace the three hardcoded defines with a single
  build-system knob (`target_compile_definitions(... NAPI_VERSION=${JSR_NAPI_VERSION})`), per-engine overridable.

### Per-engine implementation completeness

| Engine | Source (fns) | v5 | v6 bigint / instance-data | v7 detach AB | v8 type-tag / freeze-seal | Notes |
|---|---|:--:|:--:|:--:|:--:|---|
| **V8** | `js_native_api_v8.cc` (109) | ✅ | ✅ | ✅ | ✅ | Upstream Node impl; ready to ~v8/v9 once the version is bumped. |
| **JavaScriptCore** | `js_native_api_javascriptcore.cc` (97) | ✅* | ❌ | ❌ | ❌ | Hand-port; no v6+ surface. Bump jsc-android + crib from Bun to implement. |
| **Chakra** | `js_native_api_chakra.cc` (97) | ✅* | ❌ (hard wall: BigInt) | ❌ | partial (soft) | Frozen OS engine on post-EOL Win10. Decouple; see ceiling. |

`*` v5 surface to be confirmed green by this PR's suite.

### Platform status (this PR)

| Platform | Engine | v5 suite | Runner | Notes |
|---|---|---|---|---|
| **macOS** | JavaScriptCore | ✅ **12/12** (plain + ASan/UBSan + TSan) | child-process | Full v5 reference. |
| **Android** | V8 (`libv8android.so`) | ✅ **4/4 js-native-api** (dynamic `.node` + `libnapi.so`, in-process) | in-process | App sandbox can't `fork`/`exec`; addons `dlopen`'d — see below. |

**Android in-process addon loading (dynamic `.node` + shared `libnapi.so`).** The app sandbox can't `fork`/`exec`, so the conformance suite runs the addons *in-process*. The addons are built as standalone SHARED `lib<name>.so` modules (matching nodejs/node-api-cts's `add_node_api_cts_addon`), packaged by AGP into `nativeLibraryDir`, and `dlopen`'d by `node_lite_android` by soname. For their `napi_*` imports to bind at load, **`napi` is built as a shared library (`libnapi.so`) on Android** — a real `DT_NEEDED` of both the host and every addon, so there is a single napi instance. This is the IoC / dynamic-self-registration model: `dlopen` the addon → its `DT_NEEDED libnapi.so` resolves the `napi_*` → `dlsym("napi_register_module_v1")` → call it. Verified on device: `lib2_function_arguments.so` carries `DT_NEEDED [libnapi.so]`, its 8 `napi_*` are imports (`U`), `libnapi.so` exports all 106 `napi_*` (`T`), and the 4 v5 tests pass. The harness also needs the `noexcept`-removal fix (`38864e4`) so a failing test surfaces as a `ProcessResult` rather than `std::terminate`, and stdout is routed to logcat via AndroidExtensions' `StdoutLogger` (tag `StdoutLogger`) for visible gtest output.

> Why shared `napi` rather than static-linking the addons into the host: bionic will not surface a `System.loadLibrary`-loaded (RTLD_LOCAL) host's `napi_*` to a `dlopen`'d module, and post-hoc `RTLD_GLOBAL` host promotion is a no-op on bionic — so a `dlopen`'d addon can only resolve `napi` via a real shared-library `DT_NEEDED`. (An earlier iteration statically linked the addons into the host and passed too; the shared-lib model was adopted to align with node-api-cts's dynamic `.node` addons, easing a future migration.)

> Emulator note: the unrelated `JavaScript.All` UnitTest (XMLHttpRequest/WebSocket/HTTP mocha tests) can fail in an offline emulator (status 0 vs 200/404, socket timeouts); it runs before and is independent of the js-native-api suite.

## Chakra N-API ceiling

The Windows-OS Chakra (`chakra.dll`, JSRT/`jsrt.h`) is frozen ~ES2017 and will never gain new VM
primitives; Windows 10 reached EOL 2025-10-14. OSS ChakraCore has more, but is itself archived (≈2021)
and shipping it would mean bundling an unmaintained, security-frozen engine. So Chakra's capability is set
by the frozen JSRT surface you link.

- **Hard walls (a VM primitive is missing — cannot be coded around):** **BigInt** (v6
  `napi_create_bigint_words` / `napi_get_value_bigint_*`) → no faithful v6 on Chakra, ever; **ArrayBuffer
  detach** (v7) unless `JsDetachArrayBuffer` exists (verify against the actual `ChakraCore.h`).
- **Soft (more native work on existing primitives):** type tags (private symbol / external data),
  `object_freeze`/`seal` (call ES `Object.freeze`), instance data, `get_all_property_names`,
  references/finalizers (already work via `JsSetObjectBeforeCollectCallback`).

**Decision: do not let Chakra set a global ceiling.** N-API is per-engine by design — `napi_get_version`
reports the level *this* engine supports and addons feature-detect. Chakra reports the honest version it can
reach, returns `napi_generic_failure` for the walled functions, and the conformance allow-list is gated per
engine. Pragmatic Chakra target: keep v5 green, optionally cherry-pick the cheap v8 wins (freeze/seal,
type-tags), hard-stop at BigInt. Do not pour native effort into a frozen engine on a sunset OS.

## jsc-android bump (v6 enabler — separate PR)

Currently pinned at JSC `250231.0.0`; `294992.0.0` is available. Bumping brings a modern JSC with real
BigInt + ES2020 primitives (what v6/v7 need) and makes **Bun's mature N-API-on-JSC layer**
(`src/bun.js/bindings/napi.cpp` et al.) directly referenceable. Caveats: Bun rides its own WebKit fork
(API mostly matches, build differs), jsc-android-buildscripts is semi-stale, binary size grows. Sequence it
*after* this PR, as the first step of the JSC v6→v8 work.

## Staged version roadmap (post-PR1)

Each tier: bump the knob → recompile (V8 exposes the surface for free) → enable the matching test dirs →
run the suite per engine → implement the JSC/(Chakra) gaps → green.

| Step | Target | Unlocks (test dirs) | V8 | JSC / Chakra work |
|---|---|---|:--:|---|
| B1 | **v6** | `test_bigint`, `test_instance_data`, `get_all_property_names` | free | bigint create/get + instance-data (Chakra: bigint = hard wall) |
| B2 | **v7** | detached-ArrayBuffer cases | free | `napi_detach_arraybuffer` / `is_detached` |
| B3 | **v8** | type-tag + freeze/seal in `test_object`/`test_general` | free | type tags + freeze/seal → **parity with hermes-windows** |
| B4* | **v9** | `symbol_for`, syntax-error, `module_file_name` | free | implement on JSC |
| B5* | **v10** | external strings, property keys (matches `facebook/hermes API/napi`) | mostly | implement on JSC |

`*` stretch. Separately: the worklets/worker goal needs **threadsafe functions** — a runtime-layer
(`node_api.h`, `NAPI_HAS_THREADS 1`) axis not covered by the engine-only suite; track independently.

**Reference/finalizer test staging (measured at v5).** Of the vendored reference/finalizer/wrap dirs, only
`test_reference_double_free` is v5-clean and is enabled now (green on macOS/JSC incl. ASan, and Android/V8); its
`test_wrap.js` is quarantined (JSC `napi_remove_wrap` on an unwrapped object returns `napi_invalid_arg` — it
does not crash; separate fix). The rest are gated by symbols our v5 pin doesn't export and enable with the
bump: `test_reference` → `node_api_symbol_for` (v9, B4); `test_finalizer/` & `6_object_wrap` →
`napi_get_instance_data` (v6, B1) + `node_api_basic_env`/`node_api_post_finalizer` (v9, B4). `test_finalizer`
also surfaced a JSC finalizer-delivery timing case (`mustCall(1)`→0) to confirm at B1.

**GC-safety (re upstream [hermes-windows#321](https://github.com/microsoft/hermes-windows/pull/321)).** That
weak-ref-over-Proxy moving-GC bug is **not present here**. V8 uses `v8::Persistent` (an immediate, auto-relocated
GC root). JSC keeps the to-be-referenced object in the `value` argument on the C stack across the reference's
finalizer-setup allocation, so JSC's **conservative stack scan pins it against collection and relocation** — the
creation-time window is closed regardless of whether the collector moves cells — and weak liveness is an
object-id check, not a bare-pointer deref. Chakra is non-moving with a nullptr-returning weak read. Empirically,
`test_reference_double_free` runs **clean under ASan on the modern system JSC** (no use-after-free). There is no
weak-ref-over-Proxy case in the suite yet; add one at the v9 tier.

## Test-suite sourcing strategy

- **Now (this PR):** vendored copy of vmoroz's hermes-windows `unittests/NodeApi/` (engine-layer, v8-capable
  harness, `node_lite`). Resync the v1–v5 test files from upstream hermes-windows so our copies are current (task 5).
- **Evaluated (task 6) — `nodejs/node-api-cts` as a `FetchContent` `GIT_REPOSITORY` dep: not yet; track for later.**
  Findings (HEAD `ea10da9`, 2026-06):
  - **Maturity blocker:** the README states it "is currently a work-in-progress and shouldn't yet be relied on by
    anyone" (v0.1.0, not on npm). Too early to take as an upstream dependency.
  - **Harness-model mismatch:** the runner is Node.js + TypeScript (`node --test implementors/node/run-tests.ts`,
    `amaro` for TS-strip); the only implementor is `node`. Adopting it means authoring an `implementors/jsruntimehost/`
    harness (JS modules: `load-addon`, `assert`, `must-call`, `gc`, `napi-version`, `features`, `skip-test`) and
    driving the test `.js` from our runtime — i.e. re-expressing what `node_lite` already does, in their contract.
  - **Android now aligns:** `add_node_api_cts_addon()` builds SHARED `.node` (dlopen) — the same model our Android
    suite now uses (dynamic `.node` + `libnapi.so`). Its CMake only handles Apple `-undefined dynamic_lookup` /
    MSVC import-libs, so an Android port would still need our `libnapi.so` + soname-load glue, but the addon model
    itself matches — a future migration is much smoother now.
  - **No v5 coverage gain now:** its `tests/js-native-api/` are ported from the same `nodejs/node` source as our
    vendored copies; the 4 v5 tests are identical content.
  - **Upside (why track it):** engine-agnostic, active (~24 js-native-api tests ported incl. `test_bigint`,
    `test_typedarray`, `test_string`, `test_date` — valuable once we bump NAPI_VERSION), CMake-based, and
    contributing a JsRuntimeHost implementor could upstream our Android in-process + static-link learnings.
  - **Recommendation:** keep the vendored suite for v5 (works; both platforms green). Revisit node-api-cts when we
    bump NAPI_VERSION (needing its broader tests) **and** it approaches 1.0 — migrating there rather than doing a
    hermes-windows resync at that point. That is the eventual answer to the "gross copying" this PR flags.

---

## Appendix: JS engine compatibility baseline

_Folded from the original `engine-compat-baseline.md` (captured 2025-10-02 on macOS V8)._

**Environment:** Node.js v24.2.0 · V8 13.6.233.10-node.17 · macOS (darwin arm64). All 15 smoke checks passed.

| Group | Checks |
|---|---|
| Engine detection | V8 detection; WebAssembly available |
| N-API compatibility | 1 MB strings < 100 ms; TypedArray aliasing/endianness; local + global Symbols |
| Unicode / encoding | UTF-16 surrogate pairs (emoji); NFC/NFD normalization; TextEncoder/TextDecoder UTF-8 |
| Memory | 10 MB array allocation; WeakMap/WeakSet |
| ES6+ | Proxy/Reflect; BigInt; async generators |
| Performance | 1000 timers < 100 ms; deep recursion to 8,907 frames |

**Engine-upgrade-sensitive features (may fail on older Android V8/JSC):** TextEncoder/TextDecoder, BigInt
(needs V8 6.7+ / JSC with BigInt), async generators, deep recursion (lower Android stack limits), global
`v8` object detection. Tests should **feature-detect and `skip()`** rather than assume availability.

**Android engine notes:** prebuilt Android V8 may lag Node's V8 (test against Android-XR system V8);
JavaScriptCore Android `250231.0.0` is older than current Safari JSC and may lack some ES2020+ — consider the
`294992.0.0` bump (see jsc-android section).

## References

- PR #116: https://github.com/BabylonJS/JsRuntimeHost/pull/116
- hermes-windows N-API suite: https://github.com/microsoft/hermes-windows/tree/main/unittests/NodeApi
- node-api-cts: https://github.com/nodejs/node-api-cts (umbrella issue #15; publishing issue #35)
- facebook/hermes native Node-API (v10): https://github.com/facebook/hermes/tree/main/API/napi (`COMPATIBILITY.md`)
- Node-API version gating reference: https://github.com/nodejs/node-api-headers
