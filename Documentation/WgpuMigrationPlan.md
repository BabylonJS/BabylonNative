# WGPU Migration Plan (BGFX Replacement)

## Scope
- Replace `bgfx` with a Rust `wgpu` backend across Babylon Native.
- Keep current feature parity for NativeEngine + NativeXR.
- Enable BabylonJS WebGPU execution path on native targets.
- Preserve platform coverage: Android 10+, iOS, macOS, and Windows 10.
- Rebase long-term backend implementation onto upstream `wgpu-native` to avoid
  maintaining a local fork-like Rust implementation.

## Module Ownership Map (for reviewers)
- `Core/GraphicsWgpu`:
  owns backend lifetime (instance/surface/device/queue/present path) and exports
  the shared Rust C ABI (`babylon_graphics_backend`).
- `Plugins/NativeWebGPU`:
  owns JS-facing `navigator.gpu` module surface and forwards WebGPU/interop calls
  to the shared GraphicsWgpu ABI.
- `Polyfills/CanvasWgpu`:
  owns JS-facing Canvas subset (`Canvas`, `Context`, `Path2D`, etc.) and forwards
  Canvas draw operations to Rust/femtovg; `getCanvasTexture()` returns a native
  texture handle consumed by `GPUQueue.copyExternalImageToTexture(...)`.
- Relationship:
  `NativeWebGPU` and `CanvasWgpu` are peer JS modules; both use
  `Core/GraphicsWgpu` as the single backend target so the process links one
  wgpu runtime graph.

## Progress Update (current branch)
- Added a root Rust workspace (`/Cargo.toml`) centered on
  `Core/GraphicsWgpu/Rust`, with CanvasWgpu Rust source consumed as an in-crate
  module from GraphicsWgpu so there is a single dependency graph and lockfile
  (`/Cargo.lock`) for the linked backend.
- Unified Rust build artifacts under top-level build output (`${CMAKE_BINARY_DIR}/cargo`)
  and removed inline Rust-target lockfiles from source subdirectories.
- Removed separate CanvasWgpu Rust crate build wiring; CanvasWgpu Rust exports
  are compiled into `babylon_graphics_backend` so only one Rust static backend
  target is linked into native binaries.
- Implemented `CanvasWgpu` filter blur execution (blur-only CSS filter path)
  and exposed native canvas interop handle export through `getCanvasTexture()`.
- Wired Playground WebGPU smoke script to render Canvas gradient+blur+text and
  push the native canvas handle into the WebGPU canvas-texture path.
- Added native bridge function
  `babylon_wgpu_import_canvas_texture_from_native(...)`
  (with `babylon_wgpu_set_debug_texture_from_native(...)` alias retained) to
  import CanvasWgpu output into GraphicsWgpu cube sampling (used by
  `GPUQueue.copyExternalImageToTexture(...)` internals).
- Added optional upstream `wgpu-native` source integration (`FetchContent`) behind
  `BABYLON_NATIVE_WGPU_USE_UPSTREAM_NATIVE`, with Rust link wiring so the
  GraphicsWgpu backend can consume upstream staticlib artifacts.
- Added explicit `webgpu-headers` FetchContent wiring (pinned to the commit used
  by upstream `wgpu-native`) and switched shim bindgen include resolution to use
  that source-of-truth header path instead of relying on nested vendored copies.
- Added a shared internal C ABI declaration header
  (`Core/GraphicsWgpu/InternalInclude/Babylon/Graphics/WgpuInterop.h`) so
  `Core/GraphicsWgpu` and `Plugins/NativeWebGPU` consume one declaration source
  for Rust-exported `babylon_wgpu_*` entry points instead of duplicating local
  extern declarations.
- Added canvas-prefixed texture-import/stats C ABI entry points
  (`babylon_wgpu_import_canvas_texture_from_native` and
  `babylon_wgpu_get_canvas_texture_*`) and switched NativeWebGPU to those names
  while retaining debug-prefixed aliases for compatibility.
- Replaced NativeWebGPU `GPUQueue.copyExternalImageToTexture` no-op with a
  standards-aligned bridge that accepts Canvas-like external image sources and
  routes them through the shared canvas texture import path.
- Gated non-standard `navigator.gpu` hooks behind Chromium/WebKit-style
  developer flags:
  `BABYLON_NATIVE_ENABLE_WEBGPU_DEVELOPER_FEATURES` and
  `BABYLON_NATIVE_ENABLE_UNSAFE_WEBGPU`.
- Removed in-build patching of upstream `wgpu-native/Cargo.toml`; upstream is now
  consumed read-only and built as staticlib via Cargo/rustc flags from CMake.
- Converted the `upstream_wgpu_native` feature seam from no-op to active probe:
  backend init now records upstream `wgpu-native` version via `wgpuGetVersion()`
  and includes that metadata in adapter diagnostics.
- Expanded the upstream probe to use `wgpu-native` C ABI request flows
  (`wgpuInstanceRequestAdapter` + `wgpuAdapterRequestDevice` + `wgpuInstanceProcessEvents`)
  so adapter/device bootstrap viability is validated through upstream primitives
  before local fallback path execution.
- Updated async wiring assumptions for current upstream behavior: target builds
  treat `WGPUFuture` IDs from adapter/device/map/error-scope/pipeline async calls
  as optional metadata (`NULL_FUTURE` is expected today), and rely on callback
  completion + `wgpuInstanceProcessEvents` instead of `wgpuInstanceWaitAny`.
- Removed production `std::async` usage from NativeWebGPU Promise APIs (kept only
  under test hooks) and switched to JS-runtime deferred dispatch to cut thread
  churn and hot-path heap pressure.
- Optimized debug canvas texture import path to reuse GPU texture resources when
  dimensions are stable, avoiding per-frame texture/view/bind-group rebuilds.
- Added native readback staging-buffer reuse and CPU upload-buffer recycling in
  the debug canvas-texture import path, eliminating per-frame GPU/heap buffer
  allocations while preserving texture import behavior.
- Eliminated a CanvasWgpu hot-loop allocation leak by avoiding render-target
  recreation when width/height/DPI are unchanged across `nvgBeginFrame`.
- Added native `destroy` aliases for CanvasWgpu `Canvas` and `Context` objects,
  and wired disposal to release retained JS context references.
- Reduced NativeWebGPU per-frame JS wrapper churn by caching shared no-op and
  draw-marker callbacks instead of creating new function objects repeatedly.
- Reworked local fallback compute dispatch to reuse a persistent device/queue
  and cached compute pipeline, removing per-dispatch adapter/device setup.
- Reworked upstream `wgpu-native` compute dispatch path to reuse a persistent
  runtime (instance/adapter/device/queue) plus cached pipeline, removing
  per-dispatch bootstrap and reducing hot-path allocation churn.
- Inlined upstream `wgpu-native` C ABI bindings/dispatch logic into the
  primary GraphicsWgpu crate (`Core/GraphicsWgpu/Rust/src/lib.rs`) and removed
  the separate `upstream-shim` crate boundary from the workspace/dependency
  graph to avoid split ownership and duplicate crate wiring.
- Moved local `wgpu` runtime bootstrap ownership (instance + adapter/device
  selection/retry) into the same shim crate and switched `create_context` to
  consume shim-managed bootstrap results, reducing duplicate runtime ownership
  logic in `Core/GraphicsWgpu/Rust/src/lib.rs`.
- Moved depth/offscreen target creation and dimension clamping into shim-owned
  `LocalRuntimeState` helpers so local backend context code no longer duplicates
  those lifecycle/guard utilities.
- Moved local surface creation/configuration helpers into the shim crate
  (`create_local_surface` + `configure_local_surface`) so `create_context`
  remains focused on Babylon-facing context assembly while preserving behavior.
- Moved `create_context` bootstrap wiring (`instance/surface/probe/runtime/
  surface-config/format resolution`) into shim-managed
  `bootstrap_local_context`, leaving local `lib.rs` focused on Babylon pipeline
  assembly and per-frame behavior.
- Moved adapter identity resolution (backend/vendor/device/name fallback) into
  shim bootstrap output, so `create_context` no longer duplicates local-vs-
  upstream adapter mapping logic.
- Moved surface-frame acquisition and queue submit/present handoff into the
  shim (`acquire_surface_frame_view` + `submit_and_present`) so the local
  render loop uses shim-managed present semantics with less duplicated
  surface-error handling.
- Moved runtime+renderer ownership into a shim-managed
  `InteropBackendContext` (bootstrap/resize/render/install-debug-texture),
  leaving `Core/GraphicsWgpu/Rust/src/lib.rs` focused on Babylon FFI, error
  propagation, and telemetry state.
- Moved surface reconfigure path into shim (`reconfigure_local_surface`) so
  surface lifecycle operations (configure/reconfigure/acquire/present) are now
  consistently shim-owned.
- Introduced shim-owned `LocalRuntimeState` and migrated local backend context
  ownership to that runtime struct (device/queue/surface/surface-config/
  adapter metadata), reducing duplicated bootstrap/lifecycle fields in
  `Core/GraphicsWgpu/Rust/src/lib.rs`.
- Added persistent upstream bootstrap runtime initialization (`instance` +
  `adapter` + `device` + `queue`) in the shim and switched feature-enabled
  `create_context` probe path to consume that runtime.
- Extended upstream shim coverage with real surface-backed adapter probing
  (`wgpuInstanceCreateSurface` for Metal/Android/Win32 + adapter request with
  `compatibleSurface`) and switched `create_context` probe path to use it when
  a platform surface handle is available.
- Added upstream surface lifecycle probe (`wgpuSurfaceGetCapabilities` +
  `wgpuSurfaceConfigure` + `wgpuSurfaceGetCurrentTexture` + `wgpuSurfacePresent`
  + `wgpuSurfaceUnconfigure`) to validate queue/present-path viability through
  upstream C ABI before local fallback rendering path.
- Removed the remaining inline fallback shim definitions from
  `Core/GraphicsWgpu/Rust/src/lib.rs`; shim dependency is now always present,
  with upstream behavior selected via crate features.
- Switched root default to `BABYLON_NATIVE_WGPU_USE_UPSTREAM_NATIVE=ON` for this
  branch and removed the large non-upstream compute fallback in the shim,
  reducing local duplicate runtime logic while keeping explicit compile-time
  disabled stubs for non-upstream builds.
- Removed shim-only upstream surface probe/present validation helpers and
  switched bootstrap probing to `ensure_bootstrap_runtime(...)` to keep the
  interop layer smaller and avoid duplicate surface lifecycle validation paths
  that are already exercised by live render execution.
- Added serialized backend-call gating around Rust FFI `render`/`resize`/`destroy`
  to prevent resize-vs-present races observed on Android API 31 emulator
  (`Surface is not configured for presentation` panic loop).
- Hardened Playground startup against script ordering races by waiting for
  `createScene` initialization in the runner instead of exiting early, which
  removes intermittent gray-screen launches on Android API 31 simulator.
- Removed non-standard `__nativeWebGpuReady` and `__nativeCanvasReady` globals.
  The AppRuntime FIFO WorkQueue guarantees `navigator.gpu` and `_native.Canvas`
  are synchronously available before any script runs, matching the W3C WebGPU
  spec where `navigator.gpu` is a synchronous `[SameObject]` attribute.
- Hardened platform launcher reset scripts (macOS/iOS/Android Playground) to
  clear stale WebGPU smoke globals (`__babylonPlaygroundWebGpuSmokeReady`,
  `__webgpuSmokeDispose`) alongside `createScene` factory state, reducing
  intermittent gray starts caused by cross-run script-state carryover.
- Added explicit WebGPU-smoke readiness signaling
  (`__babylonPlaygroundWebGpuSmokeReady`) so the runner can await real
  async scene/canvas-texture readiness instead of relying on retry-only startup
  heuristics; this removed intermittent Android/iOS/macOS gray-start races
  without blocking the JS runtime thread.
- Removed native fail-open auto-enable draw fallback in
  `babylon_wgpu_render` (240-frame timeout path). Presentation is now driven by
  explicit JS/native draw signals (`_markWebGpuDrawRequested` and successful
  canvas-texture upload) instead of implicit frame-count heuristics.
- Fixed Playground scene bootstrap to bind Babylon scenes to the passed engine
  instance (`new BABYLON.Scene(engineArg)`), removing stale-global runtime
  coupling that could leave Android launches in a gray-screen state.
- Added render-loop exception guarding in the Playground runner so transient
  JS-side render errors are surfaced via status callbacks and trigger runtime
  recycle after repeated failures instead of leaving a silent gray frame.
- Removed extra panic/unwind wrapping from `babylon_wgpu_render` and
  `babylon_wgpu_resize` hot paths and switched to direct context casts by C ABI
  contract, reducing per-frame overhead on native render loops.
- Added a lock-free pending-upload gate for canvas texture handoff so the
  render loop avoids mutex acquisition when no Canvas texture upload is queued.
- Aligned local `wgpu` crate usage to the upstream `wgpu-native` major line
  (`wgpu` 27.x) and updated local API callsites (`FilterMode` sampler mipmap
  setting, `PipelineLayoutDescriptor.push_constant_ranges`, and
  `RenderPipelineDescriptor.multiview`) to keep compatibility while reducing
  drift during migration.
- Hardened Android Playground launch sequencing by explicitly clearing
  stale `createScene` / scene-factory globals before script load and adding
  bounded async startup retries in `playground_runner.js`, reducing intermittent
  gray-start races without blocking the runtime thread.
- Removed probe-only upstream bootstrap ownership from the GraphicsWgpu render
  bootstrap path (`ensure_bootstrap_runtime` usage in local context creation),
  keeping adapter identity sourced from the active local runtime and reducing
  duplicate C-ABI validation paths in `Core/GraphicsWgpu/Rust/src/lib.rs`.
- Simplified local adapter/device bootstrap error/retry flow in
  `Core/GraphicsWgpu/Rust/src/lib.rs` by collapsing duplicated retry branches
  into a single helper-driven selection sequence and one Android-specific
  low-power fallback retry path.
- Further reduced local bootstrap duplication by replacing platform-split
  adapter selection branches with one ordered attempt plan per platform
  (`ADAPTER_ATTEMPTS`) in `bootstrap_local_wgpu_runtime`, keeping behavior
  while shrinking local orchestration code.
- Aligned iOS/macOS Playground script bootstrap with Android by clearing stale
  global scene factory symbols before loading smoke/runner scripts, reducing
  intermittent gray-start races after repeated app relaunches.
- Added explicit post-submit device polling in CanvasWgpu and tightened iOS
  simulator poll mode in GraphicsWgpu submit path to improve command retirement
  and reduce sustained simulator memory growth during long-running smoke loops.

## Latest Validation Snapshot (this session)
- Visual checks (non-headless) confirmed textured cube + Canvas text on:
  - macOS Playground app,
  - iOS 16.4 iPhone 11 simulator (`A1EA0817-6BA0-4A8E-860F-03E3762C30F0`),
  - Android API 31 emulator (`emulator-5554`).
- New startup markers consistently appear in logs:
  - `runner:smoke-ready-await:<version>`
  - `webgpu-smoke:ready:scene-ready`
  - `runner:smoke-ready:<version>:scene-ready`
  - followed by `runner:renderloop-frame:first`
- Memory drift samples after startup settle show no unbounded leak trend in
  current smoke path:
  - macOS: ~`-48 KB` over 15s sample window,
  - iOS simulator: `+10,496 KB` over first 15s warmup window, then `0 KB`
    drift over a later 60s steady-state sample,
  - Android API 31 emulator: `+4,908 KB` over first 15s warmup window, then
    `+816 KB` over a later 60s steady-state sample.
- Fresh post-patch short-window samples (2s settle + 15s) are now effectively
  flat on active smoke runs:
  - macOS: `0 KB` delta (`227,936 KB` -> `227,936 KB`),
  - iOS 16.4 simulator: `-16 KB` delta (`377,152 KB` -> `377,136 KB`),
  - Android API 31 emulator: `0 KB` delta (`259,736 KB` -> `259,736 KB`).
- Current run after removing frame-timeout auto-enable fallback:
  - Android API 31 relaunch reliability: `5/5` launches reached both
    `runner:renderloop-frame:first` and `webgpu-smoke:canvas-texture-uploaded:1`
    without startup gray-stall.
  - 60s drift samples (after 2s settle) stayed bounded:
    - macOS: `0 KB` (`221,104 KB` -> `221,104 KB`),
    - iOS 16.4 simulator: `+432 KB` (`365,728 KB` -> `366,160 KB`),
    - Android API 31 emulator: `+240 KB` (`258,876 KB` -> `259,116 KB`).
- Current optimized artifact sizes:
  - macOS `build_release_lto` Playground app bundle: `21 MB` (`9.5 MB` executable),
  - iOS simulator `build_ios164_release_lto` Playground app bundle: `44 MB`
    (`9.6 MB` executable),
  - Android release APK (`app-release.apk`): `39 MB`.

## Current Spike Reality (as of this branch)
- `Core/GraphicsWgpu/Rust/src/lib.rs` is now the single GraphicsWgpu Rust
  runtime source (~2.9k LOC) and contains Babylon-facing FFI glue plus the
  upstream C-ABI interop/runtime management that was previously split across
  crates. The compute dispatch path lives in `src/compute.rs` as a submodule.
- `Polyfills/CanvasWgpu/Rust/src/lib.rs` remains the CanvasWgpu Rust runtime
  source (~1.2k LOC) and is included into the GraphicsWgpu crate via
  `#[path = ...]` so only one Rust staticlib is produced.
  - TODO: Convert CanvasWgpu to a proper workspace member crate once the
    code stabilizes, to improve IDE tooling and eliminate the fragile
    cross-directory `#[path]` include.
- There is no longer a separate `Core/GraphicsWgpu/Rust/upstream-shim` crate
  or Rust source file in-tree.
- This is not a patched dependency; `wgpu` resolves from crates.io.
- MSRV is `1.76` (set in `Cargo.toml`). `wgpu 27.0.1` requires this minimum.
- Android emulator validation indicates the practical floor is currently API 31
  for stable Vulkan behavior in this environment; API 29/30 emulator images
  expose adapter/device-loss issues that are likely emulator-stack specific.
- `Plugins/NativeWebGPU` still contains temporary stubbed draw-path bridging:
  WebGPU JS draw activity currently marks draw intent/counters, while actual
  frame rendering is still executed by `DebugCubeRenderer` in
  `Core/GraphicsWgpu/Rust/src/lib.rs`. Full BabylonJS command-stream execution
  over upstream `wgpu-native` C ABI remains a Phase 2 migration item.
- All FFI entry points now use `catch_unwind` to prevent panics from crossing
  the `extern "C"` boundary, including the render and resize hot paths.

## Current Coupling Snapshot
- `bgfx` API usage in tracked source: ~644 references (`Core/` + `Plugins/`).
- Direct `#include <bgfx/...>` usage: 28 files.
- Coupling is not only in rendering core:
  - `Core/Graphics`: device lifecycle, frame scheduling, view IDs, texture/framebuffer lifetime.
  - `Plugins/NativeEngine`: shader binaries, pipeline state mapping, texture formats, draw submission.
  - `Plugins/NativeXr`: swapchain texture wrapping via `bgfx::overrideInternal`.
  - `Plugins/ExternalTexture` and `Plugins/NativeCamera`: native texture interop currently assumes bgfx handles.

## Phase 2 Blockers (must resolve before PR merge)
- [ ] Replace `DebugCubeRenderer` with real BabylonJS WebGPU command-stream
  execution through upstream `wgpu-native` C ABI.
- [ ] Resolve dual GPU runtime duplication (local `wgpu` Rust API for render +
  raw C-API for compute) — unify on a single runtime.
- [ ] Implement `getImageData` GPU texture readback (currently returns zeros).
- [ ] Windows validation snapshot — no Win32 D3D12 data in current results.
- [ ] Vulkan-backend CI coverage on Windows (currently D3D12 only).
- [ ] Android runtime test automation (currently commented out in CI).

## Known API / Fidelity Gaps
| Area | Gap | Severity |
|------|-----|----------|
| Canvas `getImageData` | Returns zeroed pixel data (no GPU readback) | High |
| Canvas `putImageData` | Throws `not implemented` | High |
| Canvas `setLineDash` | Throws `not implemented` (femtovg lacks stroke dashing) | Medium |
| Canvas shadow properties | All throw `not implemented` | Medium |
| Canvas `drawImage` 9-arg | Source rect parameters ignored | Medium |
| Canvas RTL text | Byte-reversed (incorrect for multi-byte UTF-8) | Medium |
| Canvas `roundRect` elliptic | Averages x/y radius (approximation) | Low |
| NativeWebGPU error scopes | `pushErrorScope`/`popErrorScope` are no-ops | Medium |
| NativeWebGPU `device.lost` | Never-resolving promise | Low |
| NativeWebGPU adapter limits | Hardcoded, not queried from GPU | Medium |
| NativeWebGPU cached encoder | Object-identity diverges from W3C spec | Low |
| NativeWebGPU `GPUCanvasContext` | Non-standard `_createCanvasContext` only | Medium |
| NativeWebGPU features | `adapter.features` and `device.features` are empty Sets | Medium |
| Smoke test pixel validation | Counter-based only; no pixel comparison | Medium |
| Compute shader validation | No-op shader only; no output verification | Low |

## bgfx Removal Tracking
The following areas still contain bgfx coupling that must be addressed during
Phases 3-6. Search for `TODO(bgfx-removal)` comments in the codebase.
- `Core/Graphics/` — device lifecycle, frame scheduling, view IDs, texture/framebuffer lifetime (~644 bgfx references across `Core/` + `Plugins/`).
- `Plugins/NativeEngine/` — shader binaries, pipeline state mapping, texture formats, draw submission.
- `Plugins/NativeXr/` — swapchain texture wrapping via `bgfx::overrideInternal`.
- `Plugins/ExternalTexture/` and `Plugins/NativeCamera/` — native texture interop assumes bgfx handles.
- `CMakeLists.txt` root — `option()` toggles for NativeWebGPU/Canvas are hardwired ON with `FORCE` overrides.
- `WgpuInterop.h` — legacy `debug_texture` aliases should be removed.
- `validation_native.js` — still uses `BABYLON.NativeEngine()`, not WebGPU engine.
- CI templates — Linux still references `OpenGL_GL_PREFERENCE=GLVND` (bgfx-era flag).

## Migration Strategy

### Phase 0: Freeze behavior + baseline
- Lock current behavior with golden tests and capture:
  - `Apps/UnitTests` render tests.
  - XR startup/render/session teardown flow.
  - External texture and camera integration smoke tests.
- Add a feature coverage checklist (textures, MRT, readback, stencil, shader variants, XR multiview).

### Phase 1: Introduce backend boundary (no behavior change)
- Add backend-agnostic interfaces in `Core/Graphics`:
  - `IRenderDevice`, `IRenderQueue`, `ITexture`, `IFrameBuffer`, `IPipeline`, `IShaderModule`.
- Convert `DeviceContext`, `Texture`, and `FrameBuffer` wrappers to depend on these interfaces, not raw `bgfx` handles.
- Keep `bgfx` implementation behind the new boundary as the initial backend.
- Do not change JS-facing API yet.

### Phase 2: Rebase core on `wgpu-native`
- Adopt upstream `wgpu-native` as the implementation base for WebGPU-native ABI.
- Keep Babylon-specific code as a thin shim layer only:
  - platform surface wiring and host window handles,
  - async bridge glue (Rust task -> C++ future -> JS Promise),
  - diagnostics and feature gating.
  - preserve actionable JS callsite stack fidelity for rejected async APIs so
    crash/telemetry systems (e.g., Sentry) keep useful JavaScript frames.
- Replace large local Rust render/device logic with calls into `wgpu-native` C ABI.
- Preserve existing C++ boundary while transitioning:
  - keep `WgpuNative` host object,
  - progressively route internals through upstream ABI.
- Version alignment requirement:
  - keep Babylon crate `wgpu*` versions aligned with `wgpu-native` major/minor
    during migration to avoid API drift and duplicate backend logic.
- Integration note:
  - upstream `wgpu-native` currently builds as `cdylib/staticlib` (not `rlib`),
    so Babylon should consume its C ABI (or build artifact) instead of depending
    on it as a direct Rust crate.
- Remaining work for Phase 2:
  - replace local `Core/GraphicsWgpu/Rust/src/lib.rs` device/pipeline logic
    with upstream `wgpu-native` ABI-backed calls while preserving current C++ APIs,
  - move CanvasWgpu interop path from local raw-handle bridge to upstream-safe
    interop abstractions where available,
  - eliminate temporary dual-runtime duplication (`wgpu` local runtime +
    `wgpu-native` staticlib runtime) once bootstrap/present paths are fully
    switched to upstream ABI.

### Phase 3: NativeEngine port
- Replace direct `bgfx` calls in `Plugins/NativeEngine` with backend interface calls.
- Rework shader path:
  - move from bgfx binary shader expectations to WGSL/SPIR-V path consumed by `wgpu`.
  - preserve existing BabylonJS shader defines/variants contract.
- Port render state translation (blend/depth/stencil/cull/sampler) to explicit `wgpu` pipeline descriptors.

### Phase 4: XR swapchain interop (critical)
- Replace `bgfx::overrideInternal` flow in `Plugins/NativeXr/Source/NativeXrImpl.cpp`.
- Import OpenXR swapchain images into the `wgpu` backend through a controlled unsafe interop path:
  - per-API import adapter (Vulkan/Metal/D3D12) in Rust side,
  - explicit ownership/lifetime rules (no implicit handle reuse),
  - one render target abstraction for mono/stereo array layers.
- Keep the existing JS render target creation callbacks unchanged.

### Phase 5: ExternalTexture + NativeCamera
- Port native texture wrapping to the new backend.
- Preserve existing behavior contracts for:
  - render-thread scheduling (`BeforeRenderScheduler`, `AfterRenderScheduler`),
  - async texture updates and teardown safety.

### Phase 6: Remove bgfx
- Delete `bgfx` dependencies from `Core`, `Plugins`, and `Dependencies` wiring.
- Remove bgfx-specific shader compiler code paths and constants.
- Keep a temporary compile-time rollback flag only during stabilization; remove after rollout.

## Windows 10 DX12 Plan (special handling)

### Why this needs special care
- Windows 10 has more driver/compiler fragmentation than Metal/Vulkan targets.
- Shader compiler choice and present model have material stability/perf impact.

### Required handling
- Use `wgpu` DX12 backend options explicitly:
  - shader compiler selection (`Fxc`, `DynamicDxc`, `StaticDxc`),
  - swapchain present model (`Discard`, `Sequential`, `FlipSequential`),
  - frame latency controls (`present_waitable`, `max_frame_latency`).
- Build/runtime policy:
  - Default to `DynamicDxc` when available.
  - Fall back to `Fxc` on systems missing compatible DXC.
  - Provide environment/config override hooks for support triage.
- Validation matrix on Windows 10:
  - base 19041 and later,
  - Intel + AMD + NVIDIA representative GPUs,
  - D3D12 fallback behavior when DXIL path is unavailable.

## BabylonJS WebGPU Enablement
- Keep the JS `NativeEngine` contract stable while adding a backend capability flag for WebGPU mode.
- Ensure BabylonJS can select its WebGPU path when native backend reports required capabilities.
- Validate with representative scenes:
  - PBR + postprocess,
  - MRT/depth/stencil heavy scenes,
  - compute + readback where supported.

## `wgpu-native` Rebase Checklist
- [x] Add a compile-time backend selector (`local` vs `wgpu-native`) and switch branch default to upstream while keeping rollback path.
- [x] Wire upstream `wgpu-native` staticlib build + link path into Babylon CMake/Rust build.
- [x] Introduce and then inline a thin Rust shim layer that wraps upstream
  `wgpu-native` handles/callbacks without maintaining a separate crate boundary.
- [ ] Port adapter/device/surface bootstrap to upstream ABI.
  - In progress: adapter/device request bootstrap now initializes and reuses a
    persistent upstream runtime via the shim; legacy probe-only surface checks
    were removed in favor of runtime-backed probe + live render validation.
- [ ] Migrate remaining local `create_context` render/surface pipeline setup
  into shim-managed upstream handles in reversible slices.
- [ ] Replace temporary DebugCubeRenderer submit path with real
  NativeWebGPU -> `wgpu-native` command ownership.
- [x] Continue collapsing local render-resource ownership into shim-managed
  runtime primitives (depth/offscreen target lifecycle + size/format guards).
- [ ] Port queue submit + present path to upstream ABI.
- [ ] Port async callback and error propagation tests.
- [ ] Remove local duplicate render/pipeline management code once parity is achieved.

## Risk Register
- XR external image import lifetime mismatches.
- Shader translation drift (bgfx shader model vs `wgpu` pipeline model).
- D3D12 compiler/runtime differences on older Windows 10 installs.
- Performance regressions from excessive command encoder churn.
- Cross-device texture copy overhead (Canvas renders on isolated wgpu device).
- `bindgen` build dependency requires `libclang` on all build hosts — not yet
  documented in contributor setup guides.
- Canvas blur approximation (up to 289 draw calls per blurred operation) may be
  prohibitive for complex scenes with multiple blurred elements.
- CanvasWgpu font data may be double-stored (once in `font_blobs`, once in femtovg).
- Stale pointer risk in `import_native_texture_rgba_inner` if Canvas Rust objects
  are dropped while C++ still holds the `CanvasNativeTextureHandle`.

## Suggested Deliverables
1. PR A: backend interfaces + bgfx adapter (no behavior change).
2. PR B: Rust `wgpu` bootstrap + device/surface init + clear-screen sample.
3. PR C: NativeEngine core draw path on `wgpu` (desktop first).
4. PR D: NativeXR swapchain import + stereo render targets.
5. PR E: ExternalTexture/NativeCamera port.
6. PR F: bgfx removal + cleanup.

## Definition of Done
- Unit tests and representative app scenarios pass on Android, iOS, macOS, Win10.
- BabylonJS WebGPU path runs in Playground scenario without feature regression.
- XR session lifecycle works in Android XR simulator + physical device with stable frame pacing.
- No remaining `bgfx` link or include dependency in Babylon Native core/plugins.
- No large local Rust backend implementation duplicates `wgpu-native` internals.
