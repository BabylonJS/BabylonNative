# WGPU Migration Plan (BGFX Replacement)

## Scope
- Replace `bgfx` with a Rust `wgpu` backend across Babylon Native.
- Keep current feature parity for NativeEngine + NativeXR.
- Enable BabylonJS WebGPU execution path on native targets.
- Preserve platform coverage: Android 10+, iOS, macOS, and Windows 10.
- Rebase long-term backend implementation onto upstream `wgpu-native` to avoid
  maintaining a local fork-like Rust implementation.

## Progress Update (current branch)
- Added a root Rust workspace (`/Cargo.toml`) so `Core/GraphicsWgpu/Rust` and
  `Polyfills/CanvasWgpu/Rust` resolve through a single dependency graph and a
  single workspace lockfile (`/Cargo.lock`).
- Unified Rust build artifacts under top-level build output (`${CMAKE_BINARY_DIR}/cargo`)
  and removed inline Rust-target lockfiles from source subdirectories.
- Added CMake dependency serialization between `babylon_graphics_backend_rust`
  and `babylon_canvas_wgpu_backend_rust` to avoid concurrent cargo writes in
  the shared workspace target dir.
- Implemented `CanvasWgpu` filter blur execution (blur-only CSS filter path)
  and exposed native canvas interop handle export through `getCanvasTexture()`.
- Wired Playground WebGPU smoke script to render Canvas gradient+blur+text and
  push the native canvas handle into the WebGPU debug texture path.
- Added native bridge function `babylon_wgpu_set_debug_texture_from_native(...)`
  to import CanvasWgpu output into GraphicsWgpu debug cube sampling.
- Added optional upstream `wgpu-native` source integration (`FetchContent`) behind
  `BABYLON_NATIVE_WGPU_USE_UPSTREAM_NATIVE`, with Rust link wiring so the
  GraphicsWgpu backend can consume upstream staticlib artifacts.
- Converted the `upstream_wgpu_native` feature seam from no-op to active probe:
  backend init now records upstream `wgpu-native` version via `wgpuGetVersion()`
  and includes that metadata in adapter diagnostics.
- Expanded the upstream probe to use `wgpu-native` C ABI request flows
  (`wgpuInstanceRequestAdapter` + `wgpuAdapterRequestDevice` + `wgpuInstanceProcessEvents`)
  so adapter/device bootstrap viability is validated through upstream primitives
  before local fallback path execution.
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
- Extracted upstream `wgpu-native` C ABI bindings/dispatch logic into a separate
  thin shim crate (`Core/GraphicsWgpu/Rust/upstream-shim`) to reduce
  `lib.rs` coupling and isolate upstream interop concerns.
- Added persistent upstream bootstrap runtime initialization (`instance` +
  `adapter` + `device` + `queue`) in the shim and switched feature-enabled
  `create_context` probe path to consume that runtime.
- Extended upstream shim coverage with real surface-backed adapter probing
  (`wgpuInstanceCreateSurface` for Metal/Android/Win32 + adapter request with
  `compatibleSurface`) and switched `create_context` probe path to use it when
  a platform surface handle is available.
- Removed the remaining inline fallback shim definitions from
  `Core/GraphicsWgpu/Rust/src/lib.rs`; shim dependency is now always present,
  with upstream behavior selected via crate features.
- Aligned local `wgpu` crate usage to the upstream `wgpu-native` major line
  (`wgpu` 27.x) and updated local API callsites (`FilterMode` sampler mipmap
  setting, `PipelineLayoutDescriptor.push_constant_ranges`, and
  `RenderPipelineDescriptor.multiview`) to keep compatibility while reducing
  drift during migration.

## Current Spike Reality (as of this branch)
- `Core/GraphicsWgpu/Rust/src/lib.rs` currently includes a large local backend
  implementation (~1200 LOC) built directly on the `wgpu` Rust crate.
- This is not a patched dependency; `wgpu` resolves from crates.io.
- Android emulator validation indicates the practical floor is currently API 31
  for stable Vulkan behavior in this environment; API 29/30 emulator images
  expose adapter/device-loss issues that are likely emulator-stack specific.

## Current Coupling Snapshot
- `bgfx` API usage in tracked source: ~644 references (`Core/` + `Plugins/`).
- Direct `#include <bgfx/...>` usage: 28 files.
- Coupling is not only in rendering core:
  - `Core/Graphics`: device lifecycle, frame scheduling, view IDs, texture/framebuffer lifetime.
  - `Plugins/NativeEngine`: shader binaries, pipeline state mapping, texture formats, draw submission.
  - `Plugins/NativeXr`: swapchain texture wrapping via `bgfx::overrideInternal`.
  - `Plugins/ExternalTexture` and `Plugins/NativeCamera`: native texture interop currently assumes bgfx handles.

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
  - eliminate temporary dual-runtime duplication (`wgpu` 28 local backend +
    `wgpu-native` 27 staticlib probe) once bootstrap/present paths are fully
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
- [x] Add a compile-time backend selector (`local` vs `wgpu-native`) with `local` as temporary default.
- [x] Wire upstream `wgpu-native` staticlib build + link path into Babylon CMake/Rust build.
- [x] Introduce a thin Rust shim crate that only wraps upstream `wgpu-native` handles and callbacks.
- [ ] Port adapter/device/surface bootstrap to upstream ABI.
  - In progress: adapter/device request bootstrap now initializes and reuses a
    persistent upstream runtime via the shim.
- [ ] Port queue submit + present path to upstream ABI.
- [ ] Port async callback and error propagation tests.
- [ ] Remove local duplicate render/pipeline management code once parity is achieved.

## Risk Register
- XR external image import lifetime mismatches.
- Shader translation drift (bgfx shader model vs `wgpu` pipeline model).
- D3D12 compiler/runtime differences on older Windows 10 installs.
- Performance regressions from excessive command encoder churn.

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
