# Copilot Mobile Migration to `Babylon::Integrations`

> Plan for replacing the Copilot Mobile codebase's bespoke Babylon Native
> JNI bridge with the new `Babylon::Integrations` interop layer
> (`Integrations/Android/src/main/cpp/BabylonNativeIntegrations.cpp` etc.
> in this repo).
>
> This document covers the **Android** migration. iOS will follow the
> same shape using `Integrations/Apple/`.

---

## 1. What the Copilot Mobile stack looks like today

Three repositories cooperate:

```
copilot-appearance ─►  copilot-appearance.js                  (the JS bundle)
                                  ▲ loaded via "file:///..." app:/// URL
babylon-native-bridge ─►  libBabylonNativeJNI.so + Java glue   (per-app JNI)
                                  ▲ depends on
        BabylonNative ─►  AppRuntime, Device, plugins, polyfills (this repo)
                                  ▲ consumed by
app-android ─►  Compose / Kotlin host                          (the app)
```

### Native side (`babylon-native-bridge/android/src/main/cpp/babylon.cpp`)

Single 850-line cpp file. Owns these process-globals (under one
`engineMutex`):

```cpp
std::optional<Babylon::Graphics::Device> device;
std::optional<Babylon::Graphics::DeviceUpdate> deviceUpdate;
std::optional<Babylon::AppRuntime> runtime;
Babylon::Plugins::NativeInput* nativeInput;
std::optional<Babylon::Polyfills::Canvas> nativeCanvas;
std::optional<Babylon::ScriptLoader> scriptLoader;
std::atomic<bool> engineReady, waitingOnNewView, activityPaused, needsDisableRendering;
```

JNI symbols all on `com.microsoft.babylonnative.BabylonNativeBridge`:

| JNI method | What it does |
|---|---|
| `preload(hiddenSurface, context, cacheDir, scriptPath)` | One-shot init: `android::global::Initialize`, construct Device against the hidden 16×16 surface, load persistent shader cache from `cacheDir`, construct `AppRuntime`, dispatch a JS-thread lambda that wires up Console / NativeEngine / NativeOptimizations / NativeInput / Polyfills / Canvas, registers `lumiInterop` (custom JS bridge), then `LoadScript(scriptPath)`. |
| `surfaceChanged(w, h, surface)` | Switch to a real on-screen surface. Tears down bgfx if `needsDisableRendering`, then `UpdateWindow / UpdateSize`, pumps a clear frame, sets `engineReady = true`. |
| `onSurfaceTeardown()` | Sets `engineReady = false; waitingOnNewView = true; needsDisableRendering = true`. Defers actual `DisableRendering` to the next `renderFrame` (lazy teardown). |
| `renderFrame()` | Tries the engineMutex (non-blocking); honors `needsDisableRendering` (tears bgfx down) and `activityPaused`; otherwise the standard `Finish/Start` pair. Tracks FPS over 100-frame windows. Skips entirely until `scriptNotifiedReady` is set by JS via `lumiInterop.notifyReady()`. |
| `readyToRender()` | Returns `engineReady`. |
| `setCurrentActivity / activityOnPause / activityOnResume / activityOnRequestPermissionsResult` | Forwards to `android::global::*`; `Pause` / `Resume` also `runtime->Suspend / Resume` and toggle the `activityPaused` flag. |
| `setTouchInfo(id, x, y, buttonAction, buttonValue)` | One method for down / move / up; `try_to_lock`. |
| `loadScript / eval / runScript` | Forwards to `ScriptLoader` / `AppRuntime::Dispatch`. |
| `getStats()` | Returns `"FPS <n>"` from the renderFrame counter. |
| `finishEngine()` | Tears everything down in order. |

Custom JS-visible global `lumiInterop` (implemented as a `Napi::ObjectWrap`):

- `lumiInterop.notifyReady()` — flips `scriptNotifiedReady = true`. Until
  this fires, `renderFrame` is a no-op (gate against rendering before
  the JS side has finished its first `start`).
- `lumiInterop.callNative(jsonString)` — calls the Java static method
  `BabylonNativeBridge.onNativeFunctionCallInternal(String)`, which
  forwards to a single registered `NativeFunctionCallback`. This is
  Copilot's JS→native message pipe.

### Java side (`babylon-native-bridge/android/src/main/java`)

- **`BabylonNativeBridge`** — `static native` declarations + the
  `NativeFunctionCallback` registration glue.
- **`BabylonNativeManagerView`** — preload widget. The host puts an
  invisible 1×1 dp `SurfaceView` on screen; when `surfaceCreated` fires,
  the holder is stashed; `initializeWithScript(path)` (called once by
  the host once the script is downloaded) calls `BabylonNativeBridge.preload(...)`
  on the main thread. Pre-renders 5 frames (warming bgfx + shader compile)
  then stops invalidating to avoid touching a dying EGL surface.
- **`BabylonView`** — visible widget. Constructed against an `Activity`
  + an optional `useMediaOverlay` flag (Z-order). Calls
  `setCurrentActivity` / `surfaceChanged` / `onSurfaceTeardown` /
  `setTouchInfo`. Drives `renderFrame` from `onDraw` while `mViewReady`,
  re-`invalidate()`s while `mRenderLoopActive`. `onPause / onResume`
  forward to `BabylonNativeBridge.activityOnPause / Resume`.

### Kotlin host (`app-android/.../BabylonManagerView.kt`)

Compose component that mounts `BabylonNativeManagerView` (the preload
SurfaceView) inside an invisible 1×1 dp Box. Subscribes to a
`FilamentGlSignal` to wait until the parallel Filament engine has
initialized its GL context (avoids GL contention). Observes
`Lifecycle.Event.ON_PAUSE / ON_RESUME` to call
`BabylonNativeBridge.activityOnPause / Resume`. Calls
`BabylonNativeManagerView.initializeWithScript(scriptPath)` once the
script asset has been downloaded; retries after 2 seconds if it
silently fails. Adds an extra `SurfaceHolder.Callback` to the inner
SurfaceView to call `BabylonNativeBridge.onSurfaceTeardown()` on
`surfaceDestroyed`.

### JS side (`copilot-appearance/.../surfaces/mobile/src/main.ts`)

The downloaded bundle calls `lumiInterop.callNative(JSON.stringify({...}))`
to send messages to native (e.g. `"scriptLoaded"`,
`"initializationError"`), and `lumiInterop.notifyReady()` once
`window.start(...)` finishes synchronously.

---

## 2. What `Babylon::Integrations` already does for this stack

Comparing one to one:

| Concern | Copilot Mobile today | `Babylon::Integrations` today |
|---|---|---|
| AppRuntime + JsRuntime + ScriptLoader lifecycle | `babylon.cpp::preload` / `finishEngine` | `Runtime::Create` / `~Runtime` |
| Device + DeviceUpdate construction at first surface | `preload` constructs against a 16×16 hidden surface | `View::Attach` on first call constructs against the real surface (or any surface — see "prewarm" below) |
| Initialization queueing (LoadScript before Device exists) | Not separated; `preload` does both at once | Pre-init queue with `arcana::task_completion_source` |
| Hidden / dummy surface to start engine before the visible UI exists | The "preload SurfaceView" pattern in `BabylonNativeManagerView` | Same pattern, documented in `SimplifiedAPI.md §4.1 "Starting the engine before the user-visible UI exists"` |
| Surface switch | `surfaceChanged` → `Device::UpdateWindow / UpdateSize` | `~View` then a fresh `View::Attach` (which calls the same Device methods internally) |
| Render frame | `renderFrame` (`Finish/Start` cycle) | `View::RenderFrame` (same cycle, with `m_runtime.IsSuspended()` short-circuit) |
| Suspend/Resume from Activity lifecycle | `activityOnPause` / `activityOnResume` JNI methods | Auto-wired in `runtimeCreate` via `android::global::AddPauseCallback / AddResumeCallback` (each Runtime auto-suspends when the host calls `BabylonNative.pause/resume`). |
| `android::global::*` plumbing (NativeCamera, NativeXr) | `setCurrentActivity` / `activityOnRequestPermissionsResult` JNI | `BabylonNative.setContext / setCurrentActivity / requestPermissionsResult` JNI |
| Pointer input | `setTouchInfo(id, x, y, button, value)` (one method) | `viewPointerDown / Move / Up` (three methods); the View internally divides physical→logical via `ViewImpl::ToLogicalCoords` |
| Console / DebugTrace / uncaught exceptions | Hard-coded `__android_log_write` switch on level | `RuntimeOptions::log(LogLevel, std::string_view)`; the JNI `runtimeCreate` already routes everything to logcat under tag `"BabylonNative"` |
| Persistent shader cache | Hard-coded in `babylon.cpp` (`PlaygroundShaderCache.bin` in `cacheDir`) | **Not yet** in Integrations layer — see Open Questions |
| `lumiInterop.notifyReady()` gate | `scriptNotifiedReady` atomic; `renderFrame` no-ops until it fires | **Not** in Integrations layer — Copilot-specific. See migration plan below. |
| `lumiInterop.callNative()` | Custom `Napi::ObjectWrap` registering global `lumiInterop`; calls back into Java via cached `jclass` | **Not** in Integrations layer — Copilot-specific. Stays in app-specific JNI helper. |
| FPS stats | `getStats()` returns "FPS <n>" | **Not** in Integrations layer. Easy to keep app-side. |

### Built-in features the migration brings for free

- Threaded `Suspend / Resume` reference counting.
- TCS-based pre-init queueing — host can call `runtimeLoadScript` before
  the Device exists.
- Lazy GPU init: `runtimeCreate` is cheap and allocation-free; first
  `viewAttach` triggers Device + plugin init. Means we no longer need
  the dummy 1×1 SurfaceView trick *for the engine init phase*. (We
  still want it for Copilot's "render in the background while the
  visible view isn't yet up" use case — but that's a different story;
  see §3.2.)
- Consistent logical-pixel pointer coordinates (`ViewImpl_Android.cpp`
  divides by `Device::GetDevicePixelRatio()`).
- Per-platform window-size query: `View::Attach(runtime, window)` calls
  `ANativeWindow_getWidth/Height` itself; no need for the
  `surfaceChanged(w, h, surface)` JNI signature. Resize is
  `view.viewResize(w, h)`.

---

## 3. Migration plan

### 3.1 Copilot-specific app surface to keep

Two things are app-specific and must remain in app-side glue (analogous
to `Apps/Playground/Android/BabylonNative/src/main/cpp/PlaygroundJNI.cpp`
in this repo, where the Playground app keeps a Playground-specific JNI
helper alongside the generic one):

1. **`lumiInterop` global** — `notifyReady()` and `callNative(json)`,
   plus the Java-side static method dispatch and the
   `NativeFunctionCallback` registration. This is Copilot's product
   contract with the JS bundle.
2. **Persistent shader cache load/save** — `cacheDir` plumbing,
   `Babylon::Plugins::ShaderCache::Load / Save` to `PlaygroundShaderCache.bin`.

Both live in a new file in babylon-native-bridge:
`android/src/main/cpp/copilot_jni.cpp` (or similar). Same pattern as
`PlaygroundJNI.cpp` — `target_sources` it onto the
`BabylonNativeIntegrations` shared library so there's exactly one copy
of `Babylon::Integrations` symbols in the process.

### 3.2 Engine-prewarm pattern — keep using the hidden SurfaceView

The Copilot product wants the engine warmed up before the user
navigates to the live view (so the avatar is ready as soon as the
visible surface appears). Two implementation options:

**Option A — Same hidden-SurfaceView trick.** Mount a 1×1 invisible
`SurfaceView`, call `BNManager.viewAttach(runtime, surface)` against
its `ANativeWindow`. That triggers Device construction + plugin init
+ first frame. When the visible `BabylonView` later does its own
`surfaceCreated`, we **detach** the prewarm view and **attach** the
visible one. `Babylon::Integrations` permits exactly this: subsequent
`Attach` calls just rebind via `Device::UpdateWindow` (cheap), and the
JS state and ShaderCache survive.

**Option B — Headless prewarm via `Runtime::Create` only.** No
SurfaceView at all; let `runtimeCreate` start the JS engine and queue
`runtimeLoadScript`; only when the visible `BabylonView` is created
does the first `viewAttach` happen. JS bundle initialization runs
serially after first attach (because pre-init queueing waits for the
Device).

**Recommendation: Option A.** Matches today's behavior, matches the
Playground host pattern, and shaves the most latency from the visible
view's first frame. Compose composable structure barely changes.

### 3.3 Concrete file-by-file changes

#### `babylon-native-bridge/android/src/main/cpp/babylon.cpp` → mostly delete

Replace with a new `copilot_jni.cpp` that is **very small**:

- `LumiInterop` Napi::ObjectWrap class (lift from current babylon.cpp,
  unchanged).
- One JNI method `Java_com_microsoft_babylonnative_CopilotBridge_initialize(runtimeHandle, cacheDir)`
  that:
  - Caches `g_javaVM` and the `BabylonNativeBridge` jclass global ref
    (currently in `preload`).
  - Loads shader cache from `cacheDir/PlaygroundShaderCache.bin` if
    present.
  - Calls `runtime->RunOnJsThread([](Napi::Env env){ LumiInterop::Initialize(env); })`.
- One JNI method `Java_com_microsoft_babylonnative_CopilotBridge_saveShaderCache()`
  for the explicit save path (called from `Activity.onPause` and
  `runtime` teardown).
- One JNI method `Java_com_microsoft_babylonnative_CopilotBridge_isReadyToRender(runtimeHandle)`
  that returns `scriptNotifiedReady && runtimeHandle != 0`. (See §3.4
  about why this is on the app side.)
- Resolve the runtime handle via `Babylon::Integrations::Android::RuntimeFromHandle`
  (already in `Integrations/Android/Include/...`).

Net: ~150 lines of app-specific code, vs. today's 850.

#### `BabylonNativeBridge.java` → delete (after step 5)

Becomes redundant. Its two responsibilities split as follows:

- **`static native` declarations for engine/view ops** → all callers
  shift to `com.babylonjs.integrations.BabylonNative` (the generic
  Integrations Java class, identical to what the Playground uses).
- **`NativeFunctionCallback` registration + `onNativeFunctionCallInternal(String)`**
  → moves to a new `CopilotBridge` class (next).

The intermediate `System.loadLibrary` static block stays alive on
`CopilotBridge` so the .so loads once.

#### `CopilotBridge.java` (new)

App-specific JNI surface. Wraps `copilot_jni.cpp`:

```java
public final class CopilotBridge {
    static { System.loadLibrary("BabylonNativeIntegrations"); }
    private CopilotBridge() {}

    /** Performs Copilot-specific runtime setup: caches the JavaVM +
     *  CopilotBridge jclass for `lumiInterop.callNative`, registers
     *  the `lumiInterop` global on the JS side, loads the persistent
     *  shader cache from `cacheDir`. Call once per Runtime, after
     *  `BabylonNative.runtimeCreate`. */
    public static native void initialize(long runtimeHandle, String cacheDir);

    /** Save the persistent shader cache (typically called from
     *  Activity.onPause). No-op if `initialize` hasn't been called. */
    public static native void saveShaderCache();

    /** Returns true once `lumiInterop.notifyReady()` has been called
     *  by the JS bundle. The Java widgets call this before each
     *  `viewRenderFrame` to avoid rendering empty frames. */
    public static native boolean isReadyToRender();

    // ----- JS → native message pipe -----

    public interface NativeFunctionCallback {
        void onNativeFunctionCall(String jsonData);
    }
    private static NativeFunctionCallback callback;
    public static void setNativeFunctionCallback(NativeFunctionCallback cb) { callback = cb; }
    /** Called from native by `lumiInterop.callNative(json)`. */
    public static void onNativeFunctionCallInternal(String jsonData) {
        if (callback != null) callback.onNativeFunctionCall(jsonData);
    }
}
```

#### `BabylonNativeBridge.java` → mostly delete

#### `BabylonNativeManagerView.java` → simplify

Conceptually unchanged — still hosts an invisible 1×1 SurfaceView for
prewarm — but its native interactions become:

- Constructor: `BabylonNative.setContext(context.getApplicationContext())`
  (idempotent inside the JNI). Caller still creates the Runtime; this
  view borrows the handle.
- `surfaceCreated`: stash holder + activity (today's behavior).
- `initializeWithScript(scriptPath)` (its public API, called from
  Compose host once the asset is downloaded):
  - Resolve to a real ANativeWindow on the main thread via the holder.
  - `viewHandle = BabylonNative.viewAttach(runtimeHandle, surface)` —
    triggers Device construction + plugin init + queued-script flush.
    (Bootstrap = `runtimeLoadScript(scriptPath)` happened earlier on
    the Runtime; the Integrations TCS queues it until first Attach.)
  - `CopilotBridge.initialize(runtimeHandle, activity.cacheDir)` —
    caches JavaVM + bridge jclass, loads shader cache, registers
    `lumiInterop`.
- `onDraw` prewarm loop (5 frames): drives `BabylonNative.viewRenderFrame(viewHandle)`,
  guarded by `CopilotBridge.isReadyToRender(runtimeHandle)`. Still
  stops invalidating after 5 frames as today.
- `notifyTearDownCurrentSurface()`: `BabylonNative.viewDetach(viewHandle)`
  (replaces `BabylonNativeBridge.onSurfaceTeardown()`). Caller is
  `BabylonManagerView.kt` from `surfaceDestroyed`.

**One ownership decision required:** today the Runtime is implicitly
created inside `preload`; nobody owns it explicitly. For the migration,
the Compose host (`BabylonManagerView.kt`) should own the
`runtimeHandle` (created on first composition, destroyed in
`onRelease`). `BabylonNativeManagerView` and `BabylonView` borrow it.

#### `BabylonView.java` → simplify

Becomes very close to the Playground's `BabylonView.java`. Borrows the
runtime handle from the host. `surfaceCreated → viewAttach`,
`surfaceDestroyed → viewDetach`, `surfaceChanged → viewResize`,
`onTouch → viewPointerDown / Move / Up`. Keeps the `useMediaOverlay`
parameter (product-specific Z-order behavior).

Drops:
- Manual `pixelDensityScale` math (Integrations does it).
- `mViewReady` / `mRenderLoopActive` flags — replaced by the
  attached/detached state of `viewHandle`.
- `setCurrentActivity` / `onPause / onResume / onRequestPermissionsResult`
  forwarding — those move to the Activity / Compose host (matching
  the design we settled on for the Playground).

#### `BabylonManagerView.kt` (Compose host) → small changes

- Owns `runtimeHandle: Long`; creates with
  `BabylonNative.runtimeCreate(enableDebugger = false)` once Filament
  signals ready, destroys in `onDispose`.
- Calls `BabylonNative.setContext(application)` and
  `BabylonNative.setCurrentActivity(activity)` (instead of going
  through `BabylonNativeBridge`).
- `Lifecycle.Event.ON_PAUSE / ON_RESUME` → `BabylonNative.pause / resume`
  (no per-view forwarding).
- `LaunchedEffect` triggers `runtimeLoadScript(runtimeHandle, scriptPath)`
  once the path is known — this can fire even before the SurfaceView
  exists, because Integrations queues it. Then the `surfaceCreated`
  path of `BabylonNativeManagerView` does its prewarm-attach and
  `CopilotBridge.initialize`.
- Rest of Compose plumbing (`FilamentGlSignal` wait, ViewModel script
  path resolution, retry-after-delay) is unchanged.

#### `copilot-appearance/.../mobile/src/main.ts` → no change

`lumiInterop.notifyReady` and `lumiInterop.callNative` are preserved
verbatim.

#### `babylon-native-bridge/android/CMakeLists.txt`

- Add `set(BABYLON_NATIVE_INTEGRATIONS ON CACHE BOOL "" FORCE)`.
- Add `set(BABYLON_NATIVE_INTEGRATIONS_ANDROID ON CACHE BOOL "" FORCE)`.
- Drop the `BabylonNativeJNI` `add_library` and its plugin link list.
- After `add_subdirectory(${BN_REPO_ROOT_DIR})`, add
  `target_sources(BabylonNativeIntegrations PRIVATE
   src/main/cpp/copilot_jni.cpp)` plus `target_link_libraries(...
   PRIVATE NativeOptimizations URL Performance Window)` (the few
   Copilot uses that aren't unconditionally linked by the
   Integrations target — to verify against the current Integrations
   `CMakeLists.txt`).

The published `.so` becomes `libBabylonNativeIntegrations.so` instead
of `libBabylonNativeJNI.so`. Update gradle / Java
`System.loadLibrary("BabylonNativeIntegrations")` accordingly.

### 3.4 Things that need extra design discussion

1. **`scriptNotifiedReady` gating of `renderFrame`.** Today, `renderFrame`
   is a no-op until JS calls `lumiInterop.notifyReady()`. The
   Integrations `View::RenderFrame` doesn't have this gate. Two ways
   to add it:
   - **Cheap:** keep a static `std::atomic<bool>` in `copilot_jni.cpp`
     that `lumiInterop.notifyReady` flips, and have
     `BabylonNativeManagerView.onDraw` / `BabylonView.onDraw` consult
     `CopilotBridge.isReadyToRender(runtimeHandle)` before calling
     `viewRenderFrame`. Same behavior as today; isolated to app code.
   - **Generic:** add an opt-in "render gate" callback to
     `RuntimeOptions` so any host can express "don't render until JS
     signals ready". Probably overkill for one app.
   I recommend the cheap path.

2. **Persistent shader cache.** `Babylon::Plugins::ShaderCache` is
   already linked from Integrations (gated by
   `BABYLON_NATIVE_PLUGIN_SHADERCACHE`); `View::Attach` enables it.
   But Integrations doesn't expose `ShaderCache::Load / Save`. For
   Copilot, `copilot_jni.cpp` calls `ShaderCache::Load(stream)` during
   `CopilotBridge.initialize`, and `ShaderCache::Save(stream)` from a
   `CopilotBridge.saveShaderCache()` JNI method (called from Compose
   host's `ON_PAUSE`).

3. **Existing crash-prevention bandaids in `babylon.cpp`** —
   `engineMutex try_to_lock`, `needsDisableRendering` lazy bgfx
   teardown, the "pump a clear frame" trick after `surfaceChanged`,
   the `waitingOnNewView` / `activityPaused` interleavings. Some are
   workarounds for races we've since fixed (e.g. the
   `~ScriptLoader / ~AppRuntime` ordering is enforced in
   `RuntimeImpl::~RuntimeImpl`); others may still be necessary on
   certain devices (the lazy bgfx teardown comment cites SIGABRT in
   `GlContext::resize`). Plan: move forward with the Integrations API
   as-is; if the crashes reappear in QA, add corresponding hooks to
   Integrations rather than retain the bandaids on the Copilot side.

4. **`useMediaOverlay` Z-ordering.** Compose-side concern; preserved on
   the Java widget. No native-API impact.

### 3.5 Sequencing

Suggested order (each step independently builds + runs):

1. **Add `CopilotBridge.java` + `copilot_jni.cpp`** — `lumiInterop` +
   shader-cache load/save + the `isReadyToRender` gate. Keep the
   existing `babylon.cpp` working in parallel (no changes there yet).
2. **Wire up `Babylon::Integrations` build.** Update
   `babylon-native-bridge/android/CMakeLists.txt` to enable the
   Integrations Android target; produce `libBabylonNativeIntegrations.so`
   *alongside* `libBabylonNativeJNI.so`. Add `BabylonNative.java`
   (the generic JNI declarations) verbatim from the Playground,
   adjusted for the Copilot package.
3. **Migrate `BabylonView.java`** to use the Integrations API (drop
   `surfaceChanged / setTouchInfo` etc.). Keep
   `BabylonNativeManagerView.java` and the Compose host calling
   `BabylonNativeBridge.preload` for now — the visible view's
   migration is independent.
4. **Migrate `BabylonNativeManagerView.java`** + Compose host: own the
   runtime handle, prewarm via `viewAttach`. Delete
   `BabylonNativeBridge.preload / surfaceChanged / activityOnPause /
   activityOnResume / setCurrentActivity / setTouchInfo / loadScript
   / eval / renderFrame / onSurfaceTeardown / readyToRender /
   getStats / runScript / finishEngine` + their JNI implementations.
5. **Delete `babylon.cpp`** — at this point only `copilot_jni.cpp`
   remains in `babylon-native-bridge/android/src/main/cpp/`.
6. **Delete `libBabylonNativeJNI.so` build target** from CMakeLists.

After step 4 the app works end-to-end on the new API; steps 5–6 are
cleanup.

### 3.6 Testing checklist

- [ ] Cold-launch the app — `BabylonManagerView` mounts, prewarm
      surface created, scene loads behind the scenes.
- [ ] Navigate to live view — `BabylonView` attaches to its visible
      surface; first frame appears immediately (shader cache primed).
- [ ] Background / foreground the app multiple times — no SIGSEGV /
      SIGABRT in `GlContext::resize` or `bgfx::reset`.
- [ ] Touch input on hi-DPI device — coordinates reach Babylon at the
      correct logical-pixel scale (regression target: tap detection
      is no different from today).
- [ ] `lumiInterop.callNative` round-trip — JS sends a payload,
      `NativeFunctionCallback.onNativeFunctionCall(jsonData)` fires.
- [ ] Shader cache file is created in `cacheDir` after first session
      and primes subsequent launches.
- [ ] Rotating the device → `surfaceChanged` → smooth resize, no
      teardown/reattach unless Android genuinely recreates the
      Surface (which the current `surfaceCreated → viewAttach` /
      `surfaceChanged → viewResize` split handles).

---

## 4. iOS migration (sketch — not in scope for this plan)

`Apps/Playground/iOS` already uses the Apple interop layer
(`BNRuntime`, `BNView`). The `babylon-native-bridge/ios` folder will
follow the same shape: drop the bespoke Obj-C bridge, replace with
calls into `BNRuntime / BNView`, keep a small `CopilotBridge.mm` that
exposes `lumiInterop` + shader cache.

---

## Open questions (resolved)

1. ~~Is "render-gate via `lumiInterop.notifyReady`" still load-bearing,
   or is the bug it was working around fixable / already fixed?~~
   **Confirmed: keep the gate. It's not a bug bandaid — it's a
   legitimate "wait until JS has a scene" signal. The JS bundle's
   `window.start(...)` does the engine/scene/asset setup synchronously,
   then calls `lumiInterop.notifyReady()` at the end. Until that fires,
   `viewRenderFrame` would just blit empty buffers. The Java widgets
   will consult `CopilotBridge.isReadyToRender(runtimeHandle)` before
   each render call.**
2. ~~Is the prewarm pattern (Option A) the desired ongoing behavior, or
   is the team open to dropping it once the Integrations layer's lazy
   GPU init is in place (Option B)?~~ **Confirmed: keep the prewarm
   pattern (Option A). `BabylonNativeManagerView` retains its hidden
   1×1 SurfaceView; it just calls `viewAttach` on the Integrations
   layer instead of `BabylonNativeBridge.preload`.**
3. ~~Naming — should the new app-specific class be `CopilotBridge` (as
   in this plan) or stay `BabylonNativeBridge`?~~ **Confirmed:
   `CopilotBridge` for the app-specific surface (`lumiInterop` glue,
   shader-cache load/save, render-ready gate). The legacy
   `BabylonNativeBridge` class disappears at end of step 5 (its only
   surviving responsibility — the `NativeFunctionCallback` registration
   — moves to `CopilotBridge`).**
