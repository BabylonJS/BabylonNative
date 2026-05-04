# Babylon Native — Simplified Integration API Plan

## 1. Problem

Integrating Babylon Native into a host app today requires understanding and
hand-wiring a large number of internal components. The canonical setup
(see `Apps/Playground/Shared/AppContext.cpp`) requires the consumer to:

- Create and configure `Babylon::Graphics::Device` + `DeviceUpdate`, drive
  `StartRenderingCurrentFrame` / `FinishRenderingCurrentFrame` from the right
  thread.
- Create a `Babylon::AppRuntime`, then `Dispatch` a lambda onto its JS thread
  to call **~10+** `Initialize` functions (`Console`, `Window`,
  `XMLHttpRequest`, `Canvas`, `Performance`, `Blob`, `TextDecoder`,
  `NativeEngine`, `NativeInput`, `NativeXr`, `NativeCamera`,
  `NativeCapture`, `NativeEncoding`, `NativeOptimizations`,
  `NativeTracing`, `ShaderCache`, `TestUtils`, …) in the correct order.
- Use `ScriptLoader` to load `babylon.max.js`, loaders, materials, GUI,
  serializers, ammo, recast, etc., in a specific order.
- Plumb window/surface handles, resize events, and input events from each
  platform's native windowing system into `Graphics::Configuration` and
  `NativeInput` respectively.
- Repeat all of this in per-platform glue: `Apps/Playground/Win32/App.cpp`,
  `Apps/Playground/iOS/LibNativeBridge.mm`,
  `Apps/Playground/Android/BabylonNative/...`, `macOS/`, `X11/`, `UWP/`,
  `visionOS/`.

There is no "single-call" path for the common case: *show a Babylon scene
in this view*. Even trivial integrations require ~150 lines of C++.

## 2. Goal

Provide a small, stable, opinionated API that lets a host app embed Babylon
Native in a handful of calls, while keeping the existing low-level API
intact for advanced users.

The deliverable is two layers shipped in the repo:

1. **A shared C++ Integrations layer** (`Babylon::Integrations`) — a thin facade
   over the existing components that exposes a `Runtime` + `View` C++
   API.
2. **Per-platform interop layers** that bridge each platform's native
   inter-language ABI (JNI on Android, Objective-C(++) on Apple, plain
   C++ on Windows / Linux) to the shared Integrations layer. They are built
   in C++ but expose entry points that are **directly callable from the
   platform's UI language without a generic FFI generator**, exactly
   the way `Apps/Playground/Android/BabylonNative/src/main/cpp/BabylonNativeJNI.cpp`
   exposes JNI entry points to Kotlin today.

Non-goals:

- Replacing or deprecating the existing component-level API.
- Rewriting any rendering, scripting, or platform code. The simplified API
  is a *facade* over current components.
- Changing the JavaScript-facing contract (Babylon.js code keeps working).
- **Shipping precompiled "everything" binaries.** Babylon Native stays
  source-distributed via CMake. Hosts must be able to disable any plugin
  they don't use (`NativeXr`, `NativeCamera`, `NativeCapture`, …) at
  configure time so unused features add zero binary size.
- **No language-specific high-level wrappers.** We do not ship a Kotlin
  class library, a Swift package with idiomatic Swift wrapper types, a
  managed .NET assembly, or a Rust safe-wrapper crate. The per-platform
  interop layer's job ends at "Kotlin/Swift/etc. can call into native
  Babylon code"; designing idiomatic high-level wrappers in each host
  language is left to the consumer.
- **No generic C ABI / FFI surface.** With per-platform interop layers,
  each platform talks to the Integrations layer via its own native mechanism
  (JNI, Objective-C runtime, direct C++). A separate flat C ABI would
  duplicate the surface for no consumer.

## 3. Design Principles

1. **One library, two layers.** Keep `Babylon::*` (low level) untouched.
   Add `Babylon::Integrations::*` as a thin C++ facade.
2. **Sensible defaults, escape hatches.** Default config matches the
   Playground's setup. Every default is overridable.
3. **No N-API in the public surface of the Integrations layer.** If a
   consumer wants JS interop, they get an opaque escape hatch
   (`RunOnJsThread`) but they don't have to learn Napi to embed a scene.
4. **Per-platform interop layers, not a generic FFI.** Each platform
   ships a tiny C++ interop module that uses the platform's native
   inter-language ABI (JNI, Objective-C, COM/WinRT where appropriate)
   to expose `Runtime` + `View` operations to the host UI language. The
   shape of the entry points is adapted to the platform — JNI methods
   on Android, Objective-C classes on Apple, plain C++ on Windows — so
   the host writes the minimum amount of glue code in their UI
   language.
5. **Two distinct lifetimes: Runtime vs. View.** The Babylon Native
   runtime (JS engine, loaded scripts, scene state, GPU device) is
   long-lived and typically scoped to the host *application* or
   *process*. Views/surfaces are transient — they come and go as the
   host navigates between screens, backgrounds/foregrounds the app on
   mobile, or detaches a rendering surface (e.g., Android
   `surfaceDestroyed`/`surfaceCreated`, iOS background/foreground,
   Win32 window recreation). The API must let a single `Runtime`
   outlive any number of `View` attachments without tearing down JS
   state, reloading scripts, or losing the scene. This is already
   supported by the underlying `Graphics::Device` via `UpdateWindow`,
   `EnableRendering`/`DisableRendering`, and
   `StartRenderingCurrentFrame`/`FinishRenderingCurrentFrame`
   (`Core/Graphics/Include/Shared/Babylon/Graphics/Device.h:114-134`);
   the simplified facade exposes that capability cleanly.
6. **Lifecycle is explicit and symmetric.** Runtime: `create → … →
   destroy`. View: `attach → resize / input → detach`. Either side may
   be repeated: one runtime, many sequential view attachments.
7. **Host owns the frame source.** The library does not subscribe to
   any vsync / display-link / choreographer source. The host calls
   `View::RenderFrame()` from whatever draw callback its UI framework
   already provides for the view/control that hosts the rendering
   surface. See §4.1 *How frames actually get rendered*.
8. **C++ OO at the shared layer; per-platform handle-ification at the
   interop layers.** the Integrations layer is C++-object-oriented (`Runtime`
   and `View` classes with RAII, `std::function`, `std::string_view`).
   We do **not** flatten it to a C ABI:
   - Every interop layer's host language speaks C++ natively (JNI
     files compile as C++, Obj-C uses `.mm`, C++/WinRT is C++), so the
     interop modules call C++ methods directly.
   - Win32 / Linux hosts consume the C++ API directly with no interop
     layer in between — RAII is the natural shape for those.
   - A flat C ABI would only buy something if we had a generic FFI
     consumer or shipped a precompiled binary, both of which are
     explicit non-goals.
   Each interop layer handles its own conversion to opaque handles
   where the host language requires them (JNI uses `jlong`; Obj-C
   stores the C++ object in an Obj-C instance ivar).
9. **Conditional API surface mirrors plugin flags.** When a plugin or
   polyfill is disabled at configure time, the corresponding methods
   are removed from the public header — not silently no-opped. See
   §4.4.

## 4. Proposed Public Surface

### 4.1 Shared C++ facade — `Babylon::Integrations`

The facade splits along the lifetime boundary: a long-lived `Runtime`
that owns JS state and the GPU device, and a transient `View` that
binds a platform surface to the runtime for as long as that surface
exists.

```cpp
namespace Babylon::Integrations
{
    // Platform-surface handle. Populated by the platform interop layer
    // from whatever native object the host's UI framework provides.
    // The interop layer is also responsible for any unit conversion
    // (see "Pixel units" below).
    //
    // `nativeWindow` is `Babylon::Graphics::WindowT`, the same per-platform
    // typedef the Graphics layer already uses (HWND on Win32,
    // ANativeWindow* on Android, CA::MetalLayer* on Apple,
    // X11 `Window` on Linux, winrt::IInspectable on UWP). Using the
    // typed handle avoids a round-trip through `void*` and gives
    // hosts compile-time safety.
    struct ViewDescriptor {
        Babylon::Graphics::WindowT nativeWindow{};
        uint32_t width;            // logical pixels (see "Pixel units")
        uint32_t height;           // logical pixels (see "Pixel units")
        float    devicePixelRatio = 1.0f;
    };

    struct RuntimeOptions {
        uint32_t msaaSamples = 4;
        bool     enableDebugger = false;
        bool     enableShaderCache = true;

        // Which feature bundles to wire up. Defaults match Playground.
        struct Features {
            bool input    = true;
            bool xr       = false;
            bool camera   = false;
            bool capture  = false;
        } features;

        std::function<void(LogLevel, std::string_view)> log;
        std::function<void(std::string_view)>           onUnhandledError;
    };

    // Long-lived: typically created once per app/process. Sets up the
    // AppRuntime (JS thread + Napi env), JsRuntime, and non-GPU
    // polyfills/plugins. Construction is cheap and synchronous —
    // no GPU device exists yet. Device construction and GPU plugin
    // initialization (NativeEngine, etc.) are deferred to the first
    // `View::Attach` call.
    class Runtime {
    public:
        static std::unique_ptr<Runtime> Create(RuntimeOptions = {});

        // // Future construction mode — adopt a host-owned Babylon::JsRuntime
        // // instead of letting Runtime construct its own AppRuntime+JsRuntime.
        // // Intended for hosts that already own a JS engine and want
        // // Babylon Native plugins to live inside it: React Native (the
        // // host already owns Hermes/JSC + a CallInvoker dispatcher),
        // // future custom V8/QuickJS embedders, etc. The Integrations
        // // layer never sees JSI directly — only Babylon::JsRuntime,
        // // which the host wires up against whatever JS engine they have.
        // //
        // // In Attach mode `~Runtime` does NOT tear down the JS engine
        // // (the host owns it); Suspend/Resume only DisableRendering on
        // // the Device since the JS thread isn't ours to pause. Same
        // // instance API as Create-mode otherwise — same LoadScript,
        // // RunOnJsThread, View::Attach semantics. See "Construction
        // // modes" below.
        // static std::unique_ptr<Runtime> Attach(Babylon::JsRuntime& jsRuntime,
        //                                         RuntimeOptions = {});

        // JS interaction — safe to call regardless of view/suspend state.
        // LoadScript: calls made before the first `View::Attach` are
        // queued internally and dispatched onto the JS thread after
        // engine initialization completes during that first Attach.
        // Calls made after the first Attach are dispatched immediately.
        // (Most existing integrations — Playground's `AppContext`, both
        // bridges — already gate their own LoadScript on engine init by
        // hand; this just formalizes the same ordering.)
        void LoadScript(std::string_view url);   // file://, http(s)://, app://
        void Eval(std::string_view source, std::string_view sourceUrl = {});
        void RunOnJsThread(std::function<void(Napi::Env)>);  // escape hatch

        // Suspend/Resume — orthogonal to view attachment. Use when the host
        // app is backgrounded, throttled, or otherwise should not be doing
        // work (iOS applicationWillResignActive, Android onPause, power
        // saving, modal dialogs). While suspended:
        //   - JS timers (setTimeout/setInterval) pause.
        //   - In-flight microtasks complete; no new tasks are dispatched.
        //   - Any attached View becomes a no-op for RenderFrame() — the
        //     host can keep calling it from its draw callback; nothing
        //     happens until Resume().
        // Calls are reference-counted: nesting is safe.
        void Suspend();
        void Resume();
        bool IsSuspended() const;

        ~Runtime();   // detaches any current view implicitly

    private:
        friend class View;
    };

    // Transient: created when a host surface appears, destroyed when it
    // goes away. Multiple sequential Views may be attached to the same
    // Runtime over its lifetime. At most one View may be attached at a
    // time.
    class View {
    public:
        // Attaches `handle` to `runtime`.
        //
        // First Attach on a given Runtime is the heavy step: it
        // constructs `Graphics::Device` against `handle`, dispatches
        // GPU plugin initialization (`Device::AddToJavaScript`,
        // `NativeEngine::Initialize`, `NativeInput::CreateForJavaScript`,
        // …), and flushes any scripts queued via `Runtime::LoadScript`
        // before this point. Opens the first frame.
        //
        // Subsequent Attach calls on the same Runtime are cheap: the
        // Device is already constructed, plugins are initialized, the
        // JS engine is running. They just call `Device::UpdateWindow` +
        // `Device::EnableRendering` to bind the new surface, then open
        // the first frame for the new attachment.
        //
        // Detach (~View) closes the in-flight frame and calls
        // `Device::DisableRendering`. The Device persists on the
        // Runtime, so the next Attach is fast.
        static std::unique_ptr<View> Attach(Runtime& runtime, const ViewDescriptor& handle);

        // Render exactly one frame. Must be called from the same thread
        // each time (the "frame thread"). No-op if the runtime is
        // suspended. The host calls this from the platform view/control's
        // existing draw callback — see "How frames actually get rendered".
        void RenderFrame();

        // Resize the bound surface. Width/height are in **logical
        // pixels**; `dpr` is the physical-to-logical ratio (e.g. 2.0
        // for a Retina display). The platform interop layer is
        // responsible for converting whatever its UI framework
        // provides (Android `View.onSizeChanged` is in physical
        // pixels; iOS `MTKViewDelegate.drawableSizeWillChange:` is in
        // physical pixels; SwiftUI / AppKit hand you points; etc.)
        // into this convention — see §4.2 "Pixel units".
        void Resize(uint32_t width, uint32_t height, float dpr = 1.0f);

        // Input — host calls these from its event loop while the view
        // exists. Safe to call from any thread; routed to the JS thread
        // via NativeInput.
        void OnPointerDown(int32_t pointerId, float x, float y);
        void OnPointerMove(int32_t pointerId, float x, float y);
        void OnPointerUp  (int32_t pointerId, float x, float y);
        void OnKey(int32_t keyCode, bool down);

        ~View();
    };
}
```

#### Construction modes (forward-compatibility note)

The `Runtime::Attach` factory above is reserved for a future addition;
it is shown commented-out in the header so the API shape leaves room
for it without breaking changes when added.

The split is along a single axis — *who owns the JS runtime*:

- **`Create` (standalone)** — the Integrations layer builds its own
  `Babylon::AppRuntime` (JS thread + Napi env). Used by every host
  that doesn't already have a JS engine: Win32, Android Activities
  hosting Babylon directly, iOS apps with `BNRuntime`, etc.
- **`Attach` (embedded, future)** — the host has already wired up a
  `Babylon::JsRuntime` (the existing public class at
  `Core/JsRuntime/Include/Babylon/JsRuntime.h`) against their JS
  engine and dispatcher and hands it in. Used by hosts whose
  framework already owns the JS runtime: React Native (Hermes/JSC +
  `CallInvoker` dispatcher) is the concrete consumer; this is
  exactly what `BabylonReactNative`'s `BabylonNative.cpp:50` does
  today via `Babylon::JsRuntime::CreateForJavaScript(env, dispatcher)`.

Both factories produce a `Runtime` in the same "JS engine ready,
GPU not yet constructed" state. Past construction, the instance API
(`LoadScript`, `Eval`, `RunOnJsThread`, `View::Attach`, `RenderFrame`,
…) is identical — `LoadScript` and `Eval` work in Attach mode because
the Integrations layer's `ScriptLoader` accepts any object with a
`Dispatch(std::function<void(Napi::Env)>)` method (see
`Core/ScriptLoader/Include/Babylon/ScriptLoader.h:19-23`), and both
`AppRuntime` (Create mode) and `JsRuntime` (Attach mode) qualify.

A few semantic differences worth flagging at impl time:

- **`LoadScript` / `Eval` are usually unused in Attach mode.** RN-style
  hosts already own script loading via their framework's bundler
  (Metro / Webpack imports `@babylonjs/core` and the experience code
  as ES modules). The methods are still callable — they'd fetch via
  XHR and execute via `env.RunScript` — but only make sense if the
  host wants to load a UMD bundle outside its own module system. The
  primary integration point in Attach mode is `RunOnJsThread`, which
  is exactly the `JsRuntime::Dispatch` pattern BRN already uses
  (`BabylonReactNative` `BabylonNative.cpp:50,107`).
- **`~Runtime`** — Create mode tears down the owned `AppRuntime`,
  joining its JS thread. Attach mode does *not* tear down the host's
  JS engine; it cancels any pending `LoadScript`/`Eval`/`RunOnJsThread`
  lambdas, then `Napi::Detach`es. Hosts that re-create the runtime
  across a JS-engine reload (e.g. RN dev-mode bridge invalidation —
  `BabylonReactNative` listens for `RCTBridgeWillInvalidateModulesNotification`
  and calls `Deinitialize` to release its `weak_ptr`) get a clean
  destroy-then-recreate path.
- **`Suspend`/`Resume`** — Create mode pauses the JS thread via
  `AppRuntime::Suspend`. Attach mode can only `DisableRendering` on
  the `Device` since the JS thread isn't owned by us; the host's
  framework controls JS-thread pause/resume.

Adding `Attach` later is purely additive — one new static factory
taking an existing public Babylon Native type. No breaking change,
no surface duplication.

#### Switching surfaces

Whenever the host wants to render against a different platform
surface — including the swap from a hidden pre-warm window to the
user-visible one, the swap from one Activity's `SurfaceView` to
another after configuration changes, or any other surface change —
the pattern is the same:

1. Destroy the current `View` (`view.reset()` / `~View()`).
2. Construct a new `View::Attach(runtime, newWindow)`.

The Runtime's underlying `Graphics::Device` (constructed during the
*first* Attach and persisted on the Runtime thereafter) calls
`UpdateWindow` internally on subsequent Attaches. JS state, plugins,
loaded scripts, and scene state are all preserved across the swap.

This is the only mechanism the Integrations layer provides for
changing surfaces. There is no `View::SwapTo`, no `Runtime::SetWindow`,
and no implicit "current surface" the host needs to track — there's
just construct a `View` to bind a surface and destroy it to unbind.

#### Starting the engine before the user-visible UI exists (host pattern)

Some hosts want the scene to be ready the moment the user navigates
to the rendering screen — they want script load, plugin init, scene
construction, texture/mesh upload, and shader compilation to have
already happened *before* the user-visible UI attaches. This is what
`babylon-native-bridge`'s `BabylonNativeBridge::start:` (iOS) and
`BabylonNativeBridge.preload(...)` (Android) do today.

The Integrations layer does not bake this in as a feature — it falls
out naturally from the *Switching surfaces* model:

1. At app start, host calls `Runtime::Create()`. This is cheap; no
   GPU device exists yet.
2. Host calls `Runtime::LoadScript("app:///bundle.js")` etc. The
   scripts are queued internally pending the first `View::Attach`.
3. When the host wants to start scene construction (typically still
   before the user-visible UI is ready), host allocates a small
   hidden window — iOS `[CAMetalLayer layer]` with `isHidden = YES`;
   Android off-screen `SurfaceView`; Win32 `WS_EX_TOOLWINDOW` HWND —
   and calls `View::Attach(runtime, hiddenWindow)`. This is the
   first Attach, so it constructs the `Graphics::Device`, dispatches
   GPU plugin init, flushes the queued scripts. The JS thread is now
   running scene construction against the hidden surface.
4. Eventually the user-visible UI's surface becomes ready. Host
   destroys the hidden-surface View, constructs a new
   `View::Attach(runtime, realWindow)`. `Device::UpdateWindow` swaps
   the surface; scene state is preserved.

A host that doesn't care about pre-loading just calls `Runtime::Create()`
and then `View::Attach(runtime, realWindow)` whenever the real
surface is ready, with `LoadScript` calls anywhere in between. The
Integrations layer doesn't know which path the host took.

#### Loading Babylon.js: two supported routes

the Integrations layer is agnostic about how Babylon.js gets into the JS
runtime — `LoadScript` is the only mechanism. Two patterns are
first-class:

1. **Pre-bundled (recommended for new integrations).** The host
   bundles `@babylonjs/core` (and any of `@babylonjs/loaders`,
   `materials`, `gui`, `serializers`, `havok`, etc.) into a single
   ES-module-or-IIFE bundle using webpack / vite / esbuild / rollup,
   then loads it with one call:

   ```cpp
   runtime->LoadScript("app:///bundle.js");
   runtime->LoadScript("app:///experience.js");  // user's scene code
   ```

2. **Multi-UMD (Playground-style).** The host loads each `babylon.*.js`
   UMD bundle individually, in dependency order. This matches what
   `Apps/Playground/Shared/AppContext.cpp` does today and is useful
   for projects that drop in stock UMD builds without a bundler:

   ```cpp
   runtime->LoadScript("app:///babylon.max.js");
   runtime->LoadScript("app:///babylonjs.loaders.js");
   runtime->LoadScript("app:///babylonjs.materials.js");
   runtime->LoadScript("app:///babylonjs.gui.js");
   runtime->LoadScript("app:///babylonjs.serializers.js");
   runtime->LoadScript("app:///experience.js");
   ```

`LoadScript` calls are queued onto the JS thread in submission order,
so this is sufficient — no separate `bootstrapScripts` option is
needed. The library does not preload anything by default.

#### How frames actually get rendered

Today the host is responsible for the per-frame
`FinishRenderingCurrentFrame()` / `StartRenderingCurrentFrame()` pair,
driven from a platform-specific source. The simplified API keeps that
responsibility on the host but collapses the per-frame work to a
single call: `view->RenderFrame()`.

The library deliberately does **not** subscribe to a vsync source,
`CADisplayLink`, `Choreographer`, `CompositionTarget::Rendering`, or
anything similar. Every UI framework already gives the view/control
that hosts the rendering surface a natural draw callback — the host
wires `RenderFrame()` to that and is done.

Examples of the "natural draw callback" per platform:

| Platform        | Where the host calls `RenderFrame()`                                       |
|-----------------|----------------------------------------------------------------------------|
| Win32           | `WM_PAINT` handler on the rendering window                                 |
| UWP             | `SwapChainPanel`'s composition / rendering callback                        |
| macOS           | `NSView::drawRect:` or an `MTKViewDelegate::drawInMTKView:`                |
| iOS / visionOS  | `UIView::drawRect:` or `MTKViewDelegate::drawInMTKView:`                   |
| Android         | A custom `View`'s `onDraw(Canvas)` or `SurfaceView`'s render thread loop   |
| X11             | `Expose` event handler                                                     |
| Wayland         | `wl_surface::frame` callback (set up by the host's UI framework)           |

In all cases this is glue the host already writes to integrate any
custom rendering with its UI framework — Babylon Native does not add
new requirements.

##### What `RenderFrame()` does

```cpp
void View::RenderFrame()
{
    if (m_runtime.IsSuspended()) return;     // pause cleanly, no GPU work

    m_device.FinishRenderingCurrentFrame();  // submit the in-flight frame
    m_device.StartRenderingCurrentFrame();   // open the next one
    // Babylon's JS render loop (requestAnimationFrame / scene.render)
    // runs between Start and Finish, scheduled via DeviceUpdate onto
    // the JS thread. RenderFrame does not call into JS directly —
    // DeviceUpdate already coordinates Napi dispatch, bgfx command
    // recording, and present.
}
```

##### Threading

Two threads are in play:

| Thread           | Owner / lifetime                                  | What runs there                                                 |
|------------------|---------------------------------------------------|-----------------------------------------------------------------|
| **JS thread**    | `AppRuntime`'s `WorkQueue`, lives with `Runtime` | All Napi/JS execution (`scene.render()`, timers, XHR callbacks) |
| **Frame thread** | The thread the host calls `RenderFrame()` from   | `Device::Start/FinishRenderingCurrentFrame` and bgfx submission |

bgfx is configured to *not* create its own render thread
(`Core/Graphics/Source/DeviceImpl.cpp:205`), so the "render thread"
is simply whichever thread the host calls `RenderFrame()` from —
consistently. The contract is:

- **Call `View::Attach`, `View::RenderFrame`, `View::Resize`, and
  `View::~View` from the same thread** (the frame thread — usually
  the UI thread, since draw callbacks fire on the UI thread).
- **Input methods (`OnPointerDown`, etc.) may be called from any
  thread**; they post to the JS thread via `NativeInput`.
- The JS thread is invisible to the host; `RunOnJsThread` is the
  only way to reach it.

Cross-thread coordination between the frame thread and the JS thread
is handled entirely by the existing `DeviceUpdate` +
`SafeTimespanGuarantor` machinery
(`Core/Graphics/InternalInclude/Babylon/Graphics/SafeTimespanGuarantor.h`).
The simplified API does not reinvent it.

#### Lifetime and state relationship

```
Runtime           ────────────────────────────────────────────────► destroy
                         │                │       │            │
View attachments         ├── attach1 ─────┤       ├── attach2  ┤
                         (window 1)       │       (window 2)   │
                                          │                    │
Suspend state    running ─┬─ suspended ──┴── running ──┬─ suspended ─ running
                          │                            │
                          (app backgrounded)           (modal dialog)
```

Three independent axes:
- **Runtime lifetime** — JS engine, scene, device. One per app/process.
- **View attachment** — *where* to render. Zero or one at a time, may
  cycle freely.
- **Suspend state** — *whether* to do work. Independent of view; an
  app can be suspended with or without a view attached.

The `Runtime` keeps the JS engine, scene graph, loaded assets, and
`Graphics::Device` alive across detach/attach cycles. Re-attaching a
new `View` calls `Device::UpdateWindow` + `EnableRendering` and
resumes rendering without re-running any JS.

Internally `Runtime` owns the `Graphics::Device`, `DeviceUpdate`,
`AppRuntime`, `ScriptLoader`, `Canvas`, and `NativeInput*`.
`View::Attach` performs the equivalent of `Device::UpdateWindow` +
`Device::EnableRendering` + `DeviceUpdate::Start` +
`Device::StartRenderingCurrentFrame`; `~View` performs the symmetric
`FinishRenderingCurrentFrame` + `DeviceUpdate::Finish` +
`DisableRendering` so rendering cleanly pauses while no surface
exists.

### 4.2 Per-platform interop layers

Each platform ships a tiny **C++ interop module** that uses the
platform's native inter-language ABI to expose `Runtime` + `View` to
the host UI language. These modules are the analog of today's
`Apps/Playground/Android/BabylonNative/src/main/cpp/BabylonNativeJNI.cpp`,
generalized into reusable building blocks under `Integrations/<platform>/`.

The interop module **always includes** whatever small native-side
shim the platform's interop ABI fundamentally requires (e.g., a Java
class declaring `external fun` declarations on Android, an Objective-C
`@interface` header on Apple). It **does not** include idiomatic
high-level wrappers in the host language (no Kotlin "BabylonView"
class with Compose helpers, no Swift `BabylonScene` value type).
Designing those is the consumer's job.

| Platform         | Interop ABI                | Interop module shape                                                  | Native-side host shim                                            |
|------------------|----------------------------|-----------------------------------------------------------------------|------------------------------------------------------------------|
| Android          | JNI                        | `extern "C" JNIEXPORT` functions on a single JNI class                | Minimal Java/Kotlin class declaring `external fun` entry points  |
| iOS / macOS / visionOS | Objective-C runtime  | Obj-C++ (`.mm`) files exposing `@interface BNRuntime/BNView`          | Obj-C `@interface` headers (auto-imported from Swift)            |
| UWP              | WinRT                      | C++/WinRT `runtimeclass`es                                            | `.idl` for projection generation                                 |
| Win32            | Plain C++                  | Direct `Babylon::Integrations::*` (no interop layer needed)                 | None                                                             |
| Linux (X11/Wayland) | Plain C++               | Direct `Babylon::Integrations::*` (no interop layer needed)                 | None                                                             |

The interop module's surface mirrors `Runtime` and `View` 1:1, with
one entry point per public method. It is intentionally as thin as the
JNI file we have today.

#### Interop layer responsibilities

Beyond the 1:1 mirroring of `Runtime` and `View`, each interop layer
owns two platform adaptations on the host's behalf so the host's
UI-language code stays as simple as possible:

1. **Translate platform-natural units to the C++ contract.** The
   shared C++ `View::Resize(width, height, dpr)` and `ViewDescriptor`
   take **logical pixels + DPR**. Each interop layer converts
   whatever its UI framework hands the host:
   - **Android.** `View.onSizeChanged(int w, int h, ...)` provides
     **physical pixels**. The interop layer divides by
     `Resources.getSystem().getDisplayMetrics().density` and passes
     the result + the density itself to the native side. The host's
     Kotlin code passes the raw `w/h` it received.
   - **Apple.** `MTKViewDelegate.mtkView(_:drawableSizeWillChange:)`
     provides **physical pixels**; `view.contentsScale` is the DPR.
     The interop layer divides and passes through. The host's Swift
     code passes the `CGSize` it received (or hands over the layer
     directly).
   - **UWP.** `SwapChainPanel.SizeChanged` provides logical pixels +
     `RasterizationScale` for DPR. The interop layer passes both
     through unchanged.
   - **Win32 / Linux.** No interop layer; the host C++ does the
     conversion if its windowing system cares.
2. **Expose platform-specific lifecycle entries that don't belong on
   the cross-platform API.** Examples from `babylon-native-bridge`:
   - Android: `setCurrentActivity(Activity)` →
     `android::global::SetCurrentActivity(...)`,
     `activityOnRequestPermissionsResult(...)` →
     `android::global::RequestPermissionsResult(...)`. These hook
     into `AndroidExtensions/Globals.h` and are required by plugins
     like `NativeCamera` that need to call back into the Java side.
   - iOS: hooks for `applicationWillTerminate` and similar that map
     to interop-layer cleanup.

   These live on the interop layer's own surface, *not* on
   `Babylon::Integrations::Runtime` or `View`. The cross-platform layer
   stays free of `#ifdef`s; per-platform concerns live where they
   belong.

The interop layer does **not** auto-allocate a hidden initial
surface, set rendering policy, or otherwise opt the host into a
particular lifecycle pattern. The host always provides the surface
it wants the runtime to use; the interop layer just forwards it.

#### Distribution model: source + opt-in CMake subprojects

**Babylon Native is not distributed as a precompiled binary.** Source-based
CMake remains the only distribution model — that's what allows hosts to
exclude unused plugins (`NativeXr`, `NativeCamera`, `NativeCapture`,
`NativeEncoding`, `ShaderCache`, etc.) and keep their final binary small.
A precompiled `babylon_native.dll/.so/.dylib` would force every host to
link every plugin.

The Integrations layer is just **additional CMake targets that the host
opts into**, layered on top of the existing component targets:

```
  Babylon::Graphics, Babylon::AppRuntime, Babylon::ScriptLoader,
  Babylon::Polyfills::*, Babylon::Plugins::*  (existing — unchanged)
                              │
                              ▼
                       Babylon::Integrations          (new — shared C++ facade)
                              │
              ┌───────────────┼───────────────┬───────────────┐
              ▼               ▼               ▼               ▼
       Babylon::Integrations::Android   ::Apple   ::Uwp   …
        (new, optional)                 (new, optional)  (new, optional)
```

Each new target is its own CMake subdirectory under `Integrations/`,
gated by a CMake option that defaults to OFF (except the cross-platform
facade itself, which defaults to ON because it's lightweight and useful
for any host). Platforms that don't need an interop layer (Win32,
Linux) consume `Babylon::Integrations` directly.

| CMake option                          | Default | Builds                                          |
|---------------------------------------|---------|-------------------------------------------------|
| `BABYLON_NATIVE_INTEGRATIONS`         | ON      | `Babylon::Integrations` (shared C++ facade)     |
| `BABYLON_NATIVE_INTEGRATIONS_ANDROID` | OFF     | JNI interop `.so` + companion Java sources      |
| `BABYLON_NATIVE_INTEGRATIONS_APPLE`   | OFF     | Obj-C++ interop static lib + Obj-C headers      |
| `BABYLON_NATIVE_INTEGRATIONS_UWP`     | OFF     | C++/WinRT runtimeclass DLL + `.winmd`           |

Plugin selection remains opt-in/out via CMake options that the
Integrations layer respects:

| CMake option                              | Default | Effect                                          |
|-------------------------------------------|---------|-------------------------------------------------|
| `BABYLON_NATIVE_PLUGIN_NATIVEENGINE`      | ON      | Required for rendering                          |
| `BABYLON_NATIVE_PLUGIN_NATIVEINPUT`       | ON      | Required for `View::OnPointer*` / `OnKey`       |
| `BABYLON_NATIVE_PLUGIN_NATIVEXR`          | OFF     | XR session support                              |
| `BABYLON_NATIVE_PLUGIN_NATIVECAMERA`      | OFF     | Webcam capture                                  |
| `BABYLON_NATIVE_PLUGIN_NATIVECAPTURE`     | OFF     | Frame/region capture                            |
| `BABYLON_NATIVE_PLUGIN_NATIVEENCODING`    | OFF     | Image encoding                                  |
| `BABYLON_NATIVE_PLUGIN_NATIVEOPTIMIZATIONS` | OFF   | Babylon.js perf hooks                           |
| `BABYLON_NATIVE_PLUGIN_NATIVETRACING`     | OFF     | ETW / signpost tracing                          |
| `BABYLON_NATIVE_PLUGIN_SHADERCACHE`       | ON      | Disk shader cache                               |
| `BABYLON_NATIVE_POLYFILL_XHR`             | ON      | `XMLHttpRequest`                                |
| `BABYLON_NATIVE_POLYFILL_CANVAS`          | ON      | 2D canvas                                       |

`Babylon::Integrations` reads these flags (via `target_compile_definitions`
or `if(TARGET ...)` checks) and only wires up the corresponding plugin
in its setup function — so disabling `BABYLON_NATIVE_PLUGIN_NATIVEXR`
removes both the `NativeXr` library from the link line *and* the
`Babylon::Plugins::NativeXr::Initialize(env)` call from the runtime
boot, with no runtime overhead.

The interop layers depend on `Babylon::Integrations` (which depends on
whichever plugins are enabled), so each interop artifact contains only
the code paths that were actually compiled in.

##### Conditional API surface

Plugin/polyfill flags don't just remove implementations — they remove
the **public API surface** that depends on them, so misuse is a
compile error in the host's language rather than a silent runtime
no-op.

Each plugin-gated method in `Babylon::Integrations` is wrapped in
`#if BABYLON_NATIVE_PLUGIN_<NAME>` (defined by the build via
`target_compile_definitions` when the corresponding CMake option is
ON). The interop layers mirror the same gating on their own entry
points: when a plugin is disabled the JNI export, the Obj-C method,
and the C++/WinRT method all disappear together.

| CMake option                              | Public surface gated by it                                                                  |
|-------------------------------------------|---------------------------------------------------------------------------------------------|
| `BABYLON_NATIVE_PLUGIN_NATIVEINPUT`       | `View::OnPointerDown` / `OnPointerMove` / `OnPointerUp` / `OnKey` and their interop entries |
| `BABYLON_NATIVE_PLUGIN_NATIVEXR`          | XR-specific extension methods (e.g. `View::SetXrSessionStateChangedCallback`)               |
| `BABYLON_NATIVE_PLUGIN_NATIVECAMERA`      | Camera permission helpers, if any are added at the Integrations layer                             |
| `BABYLON_NATIVE_PLUGIN_NATIVECAPTURE`     | `Runtime::CaptureFrame` (if exposed at the Integrations layer)                                    |

Method-level subset of the C++ header, illustrating the pattern:

```cpp
class View {
public:
    static std::unique_ptr<View> Attach(Runtime&, const ViewDescriptor&);
    void RenderFrame();
    void Resize(uint32_t w, uint32_t h, float dpr = 1.0f);

#if BABYLON_NATIVE_PLUGIN_NATIVEINPUT
    void OnPointerDown(int32_t pointerId, float x, float y);
    void OnPointerMove(int32_t pointerId, float x, float y);
    void OnPointerUp  (int32_t pointerId, float x, float y);
    void OnKey(int32_t keyCode, bool down);
#endif

    ~View();
};
```

And the matching Android JNI gating:

```cpp
#if BABYLON_NATIVE_PLUGIN_NATIVEINPUT
extern "C" JNIEXPORT void JNICALL
Java_com_babylonjs_native_BabylonNative_viewPointerDown(JNIEnv*, jclass, jlong h, jint id, jfloat x, jfloat y) {
    reinterpret_cast<Babylon::Integrations::View*>(h)->OnPointerDown(id, x, y);
}
// ...PointerMove, PointerUp, Key likewise.
#endif
```

The companion Kotlin shim under `Integrations/Android/src/main/java/` is
generated at configure time (or hand-maintained in matched halves) so
that the `external fun` declarations only appear when the
corresponding native entries do. Same approach for the Apple `.h` and
the UWP `.idl`.

`RunOnJsThread` is gated by `BABYLON_NATIVE_EXPOSE_NAPI` (default ON,
since the escape hatch is harmless if unused) so a host that wants a
strictly N-API-free header surface can opt out.

##### How the host consumes each interop layer

- **Android.** `Integrations/Android/` contains a CMakeLists.txt that
  builds the JNI interop `.so` and a `src/main/java/...` directory
  with the companion Java/Kotlin shim class declaring `external fun`
  entry points. The host's existing `app/build.gradle` references the
  CMakeLists via `externalNativeBuild { cmake { path "..." } }` (the
  standard Android NDK + gradle integration) and adds the Java sources
  to its `sourceSets`. No AAR is produced by us; the host's existing
  gradle build emits whatever artifact it already emits.
- **Apple.** `Integrations/Apple/` is consumed via CMake by the host's
  Xcode project (or an `add_subdirectory` from the host's CMakeLists).
  It produces a static library and a set of public Objective-C
  headers that Swift code imports via the standard Swift–Obj-C bridge.
- **UWP.** `Integrations/Uwp/` produces a C++/WinRT runtime component DLL
  and `.winmd`. The host's C# / C++/WinRT project references it
  directly.
- **Win32 / Linux.** No interop layer; the host C++ uses
  `Babylon::Integrations` directly.

This keeps the source-build, opt-in-plugins model intact end-to-end:
no precompiled "everything" artifacts, and the host's build still
fully controls what code ends up in the final binary.

## 5. Examples

### Win32 (was ~250 LOC, becomes ~35) — direct C++

```cpp
#include <Babylon/Integrations/Runtime.h>
#include <Babylon/Integrations/View.h>

// Process-scoped: created once at startup, destroyed at shutdown.
static std::unique_ptr<Babylon::Integrations::Runtime> g_runtime;
// Window-scoped: created on WM_CREATE, destroyed on WM_DESTROY.
static std::unique_ptr<Babylon::Integrations::View> g_view;

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM w, LPARAM l) {
    switch (msg) {
    case WM_CREATE: {
        RECT r; GetClientRect(hwnd, &r);
        // First Attach: constructs Device, runs GPU plugin init, flushes
        // queued LoadScript calls. Subsequent re-Attaches against this
        // runtime are cheap.
        g_view = Babylon::Integrations::View::Attach(*g_runtime,
            { hwnd, (uint32_t)r.right, (uint32_t)r.bottom });
        return 0;
    }
    case WM_SIZE:    if (g_view) g_view->Resize(LOWORD(l), HIWORD(l)); return 0;
    case WM_PAINT:   if (g_view) g_view->RenderFrame();  // natural draw callback
                     ValidateRect(hwnd, nullptr); return 0;
    case WM_DESTROY: g_view.reset(); return 0;          // runtime stays alive
    }
    return DefWindowProc(hwnd, msg, w, l);
}

int WINAPI wWinMain(...) {
    g_runtime = Babylon::Integrations::Runtime::Create();
    g_runtime->LoadScript("app:///experience.js");      // queued; flushed
                                                         // on first View::Attach

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
        if (g_view) InvalidateRect(msg.hwnd, nullptr, FALSE);  // request next paint
    }
    g_runtime.reset();
}
```

> The host wires `RenderFrame()` to `WM_PAINT` and triggers it via
> `InvalidateRect` from its message loop. Babylon Native does not
> subscribe to any DXGI / DWM source itself.

**Pre-warm variant** — host wants the engine warm against a hidden
surface before the user-visible window opens. Allocate a hidden HWND
at app start, attach a View to it (this is what triggers Device
construction + GPU plugin init + script execution), and later destroy
it and re-attach against the real HWND when its `WM_CREATE` fires.

```cpp
static std::unique_ptr<Babylon::Integrations::View> g_prewarmView;
static HWND s_prewarmHwnd = nullptr;

int WINAPI wWinMain(...) {
    g_runtime = Babylon::Integrations::Runtime::Create();
    g_runtime->LoadScript("app:///experience.js");

    s_prewarmHwnd = CreateWindowEx(WS_EX_TOOLWINDOW, ..., HWND_MESSAGE, ...);
    g_prewarmView = Babylon::Integrations::View::Attach(*g_runtime,
        { s_prewarmHwnd, 16, 16, 1.0f });
    // Engine is now initializing + scene is building against hidden surface.

    // ...real window's WM_CREATE will g_prewarmView.reset() then
    //    View::Attach to its HWND. Device::UpdateWindow swaps to the
    //    real HWND under the hood; scene state preserved.

    MSG msg; while (GetMessage(&msg, nullptr, 0, 0) > 0) { /* same as above */ }
    g_runtime.reset();
    DestroyWindow(s_prewarmHwnd);
}
```

### Android — JNI interop layer + minimal Kotlin shim

The library ships the JNI `.cpp` and a companion Kotlin class. The
host writes a custom `View` (or `SurfaceView`) subclass and calls
`renderFrame()` from its draw callback.

The interop layer translates Android's physical-pixel +
`displayMetrics.density` pair into the C++ logical-pixel + DPR
convention (see §4.2). It does not allocate or own surfaces — the
host always supplies them.

**Library-supplied JNI interop** (lives in `Integrations/Android/src/main/cpp/`):

```cpp
extern "C" {

JNIEXPORT jlong JNICALL
Java_com_babylonjs_native_BabylonNative_runtimeCreate(JNIEnv* env, jclass) {
    // unique_ptr::release() returns the raw pointer and gives up
    // ownership *without* deleting; the JVM side now owns it via the
    // returned jlong handle and must call runtimeDestroy() to free it.
    auto* rt = Babylon::Integrations::Runtime::Create().release();
    return reinterpret_cast<jlong>(rt);
}

JNIEXPORT void JNICALL
Java_com_babylonjs_native_BabylonNative_runtimeDestroy(JNIEnv*, jclass, jlong h) {
    delete reinterpret_cast<Babylon::Integrations::Runtime*>(h);
}

JNIEXPORT void JNICALL
Java_com_babylonjs_native_BabylonNative_runtimeLoadScript(JNIEnv* env, jclass, jlong h, jstring url) {
    const char* s = env->GetStringUTFChars(url, nullptr);
    reinterpret_cast<Babylon::Integrations::Runtime*>(h)->LoadScript(s);
    env->ReleaseStringUTFChars(url, s);
}

JNIEXPORT void JNICALL Java_com_babylonjs_native_BabylonNative_runtimeSuspend(JNIEnv*, jclass, jlong h)
{ reinterpret_cast<Babylon::Integrations::Runtime*>(h)->Suspend(); }
JNIEXPORT void JNICALL Java_com_babylonjs_native_BabylonNative_runtimeResume(JNIEnv*, jclass, jlong h)
{ reinterpret_cast<Babylon::Integrations::Runtime*>(h)->Resume(); }

// viewAttach: physical pixels in, logical pixels through to C++.
JNIEXPORT jlong JNICALL
Java_com_babylonjs_native_BabylonNative_viewAttach(JNIEnv* env, jclass, jlong rt,
                                                    jobject surface,
                                                    jint physicalW, jint physicalH,
                                                    jfloat density) {
    ANativeWindow* win = ANativeWindow_fromSurface(env, surface);
    Babylon::Integrations::ViewDescriptor descriptor{
        win,
        (uint32_t)(physicalW / density),    // logical pixels for C++
        (uint32_t)(physicalH / density),
        density
    };
    auto* v = Babylon::Integrations::View::Attach(
        *reinterpret_cast<Babylon::Integrations::Runtime*>(rt), descriptor).release();
    return reinterpret_cast<jlong>(v);
}

JNIEXPORT void JNICALL Java_com_babylonjs_native_BabylonNative_viewDetach(JNIEnv*, jclass, jlong h)
{ delete reinterpret_cast<Babylon::Integrations::View*>(h); }
JNIEXPORT void JNICALL Java_com_babylonjs_native_BabylonNative_viewRenderFrame(JNIEnv*, jclass, jlong h)
{ reinterpret_cast<Babylon::Integrations::View*>(h)->RenderFrame(); }

// viewResize: same conversion as viewAttach.
JNIEXPORT void JNICALL
Java_com_babylonjs_native_BabylonNative_viewResize(JNIEnv*, jclass, jlong h,
                                                    jint physicalW, jint physicalH,
                                                    jfloat density) {
    reinterpret_cast<Babylon::Integrations::View*>(h)->Resize(
        (uint32_t)(physicalW / density),
        (uint32_t)(physicalH / density),
        density);
}

}
```

**Library-supplied Kotlin shim** (lives in `Integrations/Android/src/main/java/com/babylonjs/native/`):

```kotlin
package com.babylonjs.native

class BabylonNativeRuntime {
    private val handle: Long = nativeCreate()

    init { System.loadLibrary("babylon-native-interop") }

    fun loadScript(url: String) = nativeLoadScript(handle, url)
    fun suspend()                = nativeSuspend(handle)
    fun resume()                 = nativeResume(handle)
    fun close()                  = nativeDestroy(handle)

    internal fun nativeHandle(): Long = handle

    private external fun nativeCreate(): Long
    private external fun nativeDestroy(handle: Long)
    private external fun nativeLoadScript(handle: Long, url: String)
    private external fun nativeSuspend(handle: Long)
    private external fun nativeResume(handle: Long)
}

// view descriptor — opaque from the host's perspective. The interop layer
// converts physical-pixel dimensions to logical pixels internally.
class BabylonNativeView(runtime: BabylonNativeRuntime, surface: Surface,
                        physicalW: Int, physicalH: Int, density: Float) {
    private val handle = nativeAttach(runtime.nativeHandle(), surface, physicalW, physicalH, density)

    fun renderFrame() = nativeRenderFrame(handle)
    fun resize(physicalW: Int, physicalH: Int, density: Float) =
        nativeResize(handle, physicalW, physicalH, density)
    fun detach() = nativeDetach(handle)

    private external fun nativeAttach(rt: Long, s: Surface, w: Int, h: Int, d: Float): Long
    private external fun nativeDetach(handle: Long)
    private external fun nativeRenderFrame(handle: Long)
    private external fun nativeResize(handle: Long, w: Int, h: Int, d: Float)
}
```

**Host code — simple integration** (consumer's app — *not* shipped by the library):

The simplest host creates the runtime at app start and attaches a
View when its real `SurfaceView`'s surface is ready.

```kotlin
class MyApp : Application() {
    val runtime by lazy {
        BabylonNativeRuntime().apply { loadScript("app:///experience.js") }
        // LoadScript queues; will execute after the first View::Attach.
    }
}

class BabylonView(context: Context) : SurfaceView(context), SurfaceHolder.Callback {
    private val runtime get() = (context.applicationContext as MyApp).runtime
    private var view: BabylonNativeView? = null
    private val density = resources.displayMetrics.density

    init { holder.addCallback(this) }

    override fun surfaceCreated(h: SurfaceHolder) {
        // First Attach: triggers Device construction + GPU plugin init +
        // queued LoadScript flush. Subsequent Attaches are cheap.
        view = BabylonNativeView(runtime, h.surface, width, height, density)
    }
    override fun surfaceChanged(h: SurfaceHolder, f: Int, w: Int, hh: Int) {
        view?.resize(w, hh, density)            // physical pixels in
    }
    override fun surfaceDestroyed(h: SurfaceHolder) {
        view?.detach(); view = null
    }

    // Natural draw callback — host wires RenderFrame() here.
    override fun onDraw(canvas: Canvas) {
        view?.renderFrame()
        invalidate()    // request next draw
    }
}
```

**Host code — pre-loading the engine before the user-visible UI exists:**

A host that wants the engine warm before the user navigates to the
rendering screen creates the runtime + an off-screen `SurfaceView`
(the pattern from `babylon-native-bridge`'s
`BabylonNativeManagerView.java:51`) at app start, and attaches a
View to the off-screen surface so the first Attach kicks off Device
construction + plugin init + script execution. When the user-visible
surface arrives, the host destroys that View and attaches a new one
to the real surface (Device::UpdateWindow under the hood).

```kotlin
class MyApp : Application() {
    val runtime by lazy {
        BabylonNativeRuntime().apply { loadScript("app:///experience.js") }
    }

    private var hiddenSurface: Surface? = null
    private var prewarmView: BabylonNativeView? = null

    override fun onCreate() {
        super.onCreate()
        // Off-screen SurfaceView attached to a window that is never shown.
        // (Detail elided; same approach as babylon-native-bridge.)
        hiddenSurface = createHiddenSurface()
        prewarmView = BabylonNativeView(runtime, hiddenSurface!!, 1, 1, 1.0f)
        // First Attach now: engine boots, scripts run, scene builds.
    }

    fun releasePrewarm() {
        prewarmView?.detach(); prewarmView = null
    }
}
// ...later, when the user-visible BabylonView's surface is ready, host
//    calls (myApp).releasePrewarm() and constructs a new BabylonNativeView
//    against the real surface.

class MainActivity : AppCompatActivity() {
    private val runtime get() = (application as MyApp).runtime
    override fun onPause()  { super.onPause();  runtime.suspend() }
    override fun onResume() { super.onResume(); runtime.resume()  }
}
```
### iOS / macOS — Obj-C++ interop layer + minimal Obj-C headers

**Library-supplied Obj-C++ interop** (lives in `Integrations/Apple/`):

The interop layer reads each `CAMetalLayer`'s `drawableSize`
(physical pixels) and `contentsScale` (DPR) directly so the Swift
host doesn't have to think about units — it just hands the layer
over (see §4.2). Surface allocation is the host's responsibility.

```objc
// BNRuntime.h  — public Obj-C header (Swift sees this via the bridge)
@interface BNRuntime : NSObject
- (instancetype)init;
- (void)loadScript:(NSString*)url;
- (void)suspend;
- (void)resume;
@end

@interface BNView : NSObject
// `layer` is the user-visible CAMetalLayer (typically MTKView.layer).
// The interop layer reads drawableSize and contentsScale from it.
- (instancetype)initWithRuntime:(BNRuntime*)rt layer:(CAMetalLayer*)layer;
- (void)renderFrame;
- (void)resizeForLayer:(CAMetalLayer*)layer;    // re-reads size/scale
@end
```

```objc++
// BNRuntime.mm  — implementation
@implementation BNRuntime {
    std::unique_ptr<Babylon::Integrations::Runtime> _rt;
}
- (instancetype)init {
    if ((self = [super init])) {
        _rt = Babylon::Integrations::Runtime::Create();
    }
    return self;
}
- (void)loadScript:(NSString*)url { _rt->LoadScript(url.UTF8String); }
- (void)suspend { _rt->Suspend(); }
- (void)resume  { _rt->Resume();  }
- (Babylon::Integrations::Runtime*)native { return _rt.get(); }
@end

@implementation BNView {
    std::unique_ptr<Babylon::Integrations::View> _v;
}
// Helper: read physical-pixel dims + DPR from the layer, convert to
// the C++ logical-pixel + DPR convention.
static Babylon::Integrations::ViewDescriptor MakeViewDescriptor(CAMetalLayer* layer) {
    CGFloat scale = layer.contentsScale;
    return {
        (__bridge void*)layer,
        (uint32_t)(layer.drawableSize.width  / scale),    // logical
        (uint32_t)(layer.drawableSize.height / scale),
        (float)scale
    };
}
- (instancetype)initWithRuntime:(BNRuntime*)rt layer:(CAMetalLayer*)layer {
    if ((self = [super init])) {
        // First Attach on this runtime triggers Device construction +
        // GPU plugin init + queued LoadScript flush.
        _v = Babylon::Integrations::View::Attach(*[rt native], MakeViewDescriptor(layer));
    }
    return self;
}
- (void)renderFrame { _v->RenderFrame(); }
- (void)resizeForLayer:(CAMetalLayer*)layer {
    auto h = MakeViewDescriptor(layer);
    _v->Resize(h.width, h.height, h.devicePixelRatio);
}
@end
```

**Host code — simple integration** (consumer's app — *not* shipped by the library):

The simplest host creates the runtime at app start, loads scripts
(queued), then constructs a `BNView` against the user-visible MTKView
layer in `viewDidLoad`.

```swift
// AppDelegate — runtime created once at app start.
class AppDelegate: NSObject, UIApplicationDelegate {
    let runtime: BNRuntime = {
        let r = BNRuntime()
        r.loadScript(Bundle.main.url(forResource: "experience",
                                       withExtension: "js")!.absoluteString)
        // LoadScript queues; flushes on first BNView attach.
        return r
    }()

    func applicationWillResignActive(_ app: UIApplication) { runtime.suspend() }
    func applicationDidBecomeActive (_ app: UIApplication) { runtime.resume()  }
}

class BabylonViewController: UIViewController, MTKViewDelegate {
    var babylonView: BNView?

    override func viewDidLoad() {
        let mtkView = view as! MTKView
        mtkView.delegate = self
        let runtime = (UIApplication.shared.delegate as! AppDelegate).runtime
        // First Attach: triggers Device + plugin init + queued script flush.
        babylonView = BNView(runtime: runtime, layer: mtkView.layer as! CAMetalLayer)
    }

    // Natural draw callback — no size arithmetic in Swift.
    func draw(in view: MTKView) { babylonView?.renderFrame() }
    func mtkView(_ v: MTKView, drawableSizeWillChange size: CGSize) {
        babylonView?.resizeForLayer(v.layer as! CAMetalLayer)
    }
}
```

**Host code — pre-loading the engine before the user-visible UI exists:**

A host that wants the engine warm at app start creates a `BNView`
against an off-screen `CAMetalLayer` in the `AppDelegate` so the first
Attach fires immediately and starts initialization + scene
construction. Later, when a view controller appears, the host destroys
that View and constructs a new `BNView` against the user-visible
layer.

```swift
class AppDelegate: NSObject, UIApplicationDelegate {
    let runtime = BNRuntime()
    var prewarmLayer: CAMetalLayer!
    var prewarmView: BNView!

    func application(_ app: UIApplication, didFinishLaunchingWithOptions
                      options: [UIApplication.LaunchOptionsKey : Any]?) -> Bool {
        runtime.loadScript(Bundle.main.url(forResource: "experience",
                                            withExtension: "js")!.absoluteString)

        prewarmLayer = CAMetalLayer()
        prewarmLayer.drawableSize = CGSize(width: 16, height: 16)
        prewarmLayer.isHidden = true
        prewarmView = BNView(runtime: runtime, layer: prewarmLayer)
        // First Attach fires here; engine is now booting up + scripts running.
        return true
    }

    func releasePrewarm() { prewarmView = nil }    // call before binding real layer

    func applicationWillResignActive(_ app: UIApplication) { runtime.suspend() }
    func applicationDidBecomeActive (_ app: UIApplication) { runtime.resume()  }
}
// ... later, in the view controller:
//      delegate.releasePrewarm()
//      babylonView = BNView(runtime: delegate.runtime,
//                           layer: mtkView.layer as! CAMetalLayer)
//      // Device::UpdateWindow swaps to the user-visible layer; scene state
//      // and JS state are preserved.
```

### Suspend/Resume is reference-counted

Multiple subsystems can request suspension independently — the runtime
only resumes when all requests have been released. This makes it safe
to combine app-lifecycle suspension with ad-hoc pauses (e.g., a modal
dialog, a power-saving mode, a long-running native operation):

```cpp
runtime->Suspend();   // app backgrounded         -> count = 1, suspended
runtime->Suspend();   // modal dialog also pauses -> count = 2, suspended
runtime->Resume();    // dialog closed            -> count = 1, still suspended
runtime->Resume();    // app foregrounded         -> count = 0, running
```

## 6. Implementation Phases

### Phase 1 — Shared C++ facade (no new functionality)
- Extract the canonical setup from `Apps/Playground/Shared/AppContext.cpp`
  into a new `Integrations` component, split along the lifetime boundary:
  `Babylon::Integrations::Runtime` (long-lived; owns `AppRuntime`,
  `ScriptLoader`, non-GPU polyfills/plugins; lazily constructs the
  `Graphics::Device` on first View::Attach) and
  `Babylon::Integrations::View` (transient; binds a platform surface
  via `Device::UpdateWindow` + `EnableRendering` and drives the
  per-frame `Start/FinishRenderingCurrentFrame` pair).
- Implement `Runtime::Suspend/Resume` on top of `AppRuntime::Suspend/Resume`
  with reference-counted nesting.
- Implement `Runtime::LoadScript` queueing for calls made before the
  first `View::Attach`; flush the queue inside that first Attach
  after engine initialization completes.
- Add CMake option `BABYLON_NATIVE_INTEGRATIONS` (default ON).
- Plumb `BABYLON_NATIVE_PLUGIN_*` and `BABYLON_NATIVE_POLYFILL_*` flags
  through `Babylon::Integrations`'s setup function so disabling a plugin
  removes the link dep, the `Initialize` call, **and** the public
  header surface that depends on it (see §4.4 *Conditional API
  surface*).
- As a transitional step, refactor `AppContext` to be a thin wrapper
  over `Babylon::Integrations` for parity validation; Playground keeps
  working unchanged. `AppContext` is **deleted** at the end of Phase 5
  once every Playground host has been migrated to `Babylon::Integrations`
  directly (see Phase 5.5 below).

### Phase 2 — Win32 / Linux validation (no interop layer)
- Convert `Apps/Playground/Win32/App.cpp` and the Linux variants to use
  `Babylon::Integrations` directly (no interop layer needed on these
  platforms).
- Verify the `WM_PAINT` / Expose-event frame model works end-to-end
  with the existing playground content.

### Phase 3 — Android interop layer (`Integrations/Android/`)
- Create `Integrations/Android/CMakeLists.txt` that builds a JNI `.so`
  exposing the entry points listed in §5.
- Add the companion `BabylonNative.kt` (or `.java`) shim under
  `Integrations/Android/src/main/java/`.
- Convert `Apps/Playground/Android/BabylonNative/src/main/cpp/BabylonNativeJNI.cpp`
  to use the new interop layer (or simply delete it and have Playground
  consume `Integrations/Android/` directly).
- Document the gradle integration: `externalNativeBuild` referencing
  the `Integrations/Android` CMakeLists, plus adding the Java sources to
  the host's `sourceSets`.
- Gate the build on `BABYLON_NATIVE_INTEGRATIONS_ANDROID` (default OFF).

### Phase 4 — Apple interop layer (`Integrations/Apple/`)
- Create `Integrations/Apple/` with Obj-C++ implementations of `BNRuntime`
  and `BNView` and their public Obj-C headers.
- Convert `Apps/Playground/iOS/LibNativeBridge.mm` and the macOS /
  visionOS bridges to use the interop layer.
- Gate on `BABYLON_NATIVE_INTEGRATIONS_APPLE` (default OFF).

### Phase 5 — UWP interop layer (`Integrations/Uwp/`)
- Create `Integrations/Uwp/` exposing C++/WinRT runtime classes (`.idl` +
  generated projection).
- Convert `Apps/Playground/UWP/` to consume it.
- Gate on `BABYLON_NATIVE_INTEGRATIONS_UWP` (default OFF).

### Phase 5.5 — Delete `AppContext`
- After all Playground hosts (Win32, Linux, Android, iOS, macOS,
  visionOS, UWP) have been migrated to consume `Babylon::Integrations`
  directly (Win32/Linux) or through their respective interop layers
  (Android/Apple/UWP), delete `Apps/Playground/Shared/AppContext.{h,cpp}`
  and any remaining `#include` references.
- The Playground apps then double as the simplest-possible canonical
  examples of integrating `Babylon::Integrations` per platform — exactly
  what we want users to copy from.

### Phase 6 — Lifecycle and threading polish
- Audit Suspend/Resume across all platform interop layers; make sure
  every platform's app-lifecycle hook is wired in the Playground app
  but *not* assumed by the library.
- Document the frame-thread / JS-thread contract in
  `Documentation/Components.md`.
- Add an integration test (`Apps/UnitTests/Source/Tests.Integrations.*`)
  that exercises attach/detach/suspend/resume cycles without leaking
  the device or the JS engine.

### Phase 7 — Documentation
- Rewrite `Documentation/Components.md` "Getting started" to point at
  `Babylon::Integrations`.
- Add per-platform integration guides under `Documentation/Integration/`:
  Win32, Android, iOS, macOS, UWP, Linux. Each shows the smallest
  possible host integration using the shared facade and (where
  applicable) the platform's interop layer.

## 7. Risks & Open Questions

- **Surface swap discipline.** Constructing a `View` against a
  different surface than the runtime is currently bound to triggers
  `Device::UpdateWindow` (the same call `babylon-native-bridge`
  makes from `surfaceChanged` —
  `android/src/main/cpp/babylon.cpp:497-511`). Verify that this
  cleanly handles: detach-while-no-frame-in-flight,
  detach-mid-frame, and swap to a different surface mid-app.
  Reference: existing start/finish discipline in
  `Tests.ExternalTexture.D3D11.cpp:24-69`.
- **Shader cache directory.** `Babylon::Plugins::ShaderCache::Load/Save`
  needs an OS-mandated cache path that varies per platform (Android
  `Context.getCacheDir()`, iOS `NSCachesDirectory`, etc.). Add
  `RuntimeOptions::shaderCachePath: std::optional<std::string>` and
  let the Integrations layer auto-load on `Create` and auto-save on
  `Suspend` and `~Runtime`. Reference: bridge plumbing in
  `BabylonNativeBridge.mm:88-106` and `babylon.cpp:242-260,378-398`.
- **JS ↔ native messaging.** Both bridges add a custom Napi
  `ObjectWrap` (`LumiInterop`) exposing `callNative(jsonString)` and
  `notifyReady()` to JS, plus a way to push results back to the host
  (cached `jclass` + `JNIEnv` attach on Android; stored Swift block
  on iOS). This is the second-most common integration need after
  scene rendering. The plan currently only offers `RunOnJsThread` as
  an N-API escape hatch. **Decide:** ship a typed message channel
  (`Runtime::SetMessageHandler(std::function<void(std::string_view)>)`
  + JS global `babylonNative.postMessage(string)`, JSON-string in /
  out) so 90% of consumers don't have to write Napi.
- **Plugin granularity.** Some plugins have implicit JS dependencies
  (e.g., `NativeXr` expects WebXR shims to exist). The setup function
  needs to auto-skip dependent setup steps when their plugin is
  disabled.
- **Headless / external-texture mode.** Hosts that render to an
  external texture (no swap chain) currently use `BackBufferColor` /
  `UpdateBackBuffer` directly on `Graphics::Device`. The simplified
  facade may need a `View::AttachExternalTexture(...)` variant, or a
  separate `Runtime::SetExternalTexture(...)` path with no `View`
  attached.
- **Android-specific lifecycle entries on the interop layer.** The
  Android JNI surface needs `setCurrentActivity(Activity)` and
  `activityOnRequestPermissionsResult(...)` to feed
  `AndroidExtensions/Globals.h` (consumed by `NativeCamera` etc.).
  These don't belong on cross-platform `Runtime`/`View`; document
  them as part of the Android interop layer's surface, alongside
  the cross-platform mirror.
- **`RenderFrame` from a non-paint draw callback.** On Android,
  `View.onDraw` is intended for `Canvas`-based 2D drawing; using it
  as a generic frame tick may not be the idiomatic choice for
  Vulkan/GLES rendering against a `SurfaceView`. Document
  `SurfaceView` + a host-owned render thread as a supported pattern
  alongside `onDraw`. The `babylon-native-bridge` Android demo uses
  the `onDraw` + `invalidate()` pattern successfully
  (`BabylonView.java:162-169`).
- **macOS without an interop layer.** A pure-C++ macOS host (rare,
  but the unit-test app is one) should still be able to use
  `Babylon::Integrations` directly with a `CAMetalLayer*`. Confirm this
  works without pulling in the Obj-C interop layer.

## 8. Out of Scope

- Idiomatic high-level wrappers in any host language (no Kotlin
  `BabylonView` `@Composable`, no Swift `BabylonScene` value type, no
  managed .NET `BabylonControl`, no Rust `Scene` safe wrapper).
- A flat C ABI / FFI surface. Each platform talks to `Babylon::Integrations`
  through its own native interop ABI.
- Precompiled "everything" artifacts published to package registries
  (Maven Central, CocoaPods, NuGet, crates.io). Source-build via
  CMake remains the only distribution channel.
- Replacing or deprecating the existing component-level API.
- Changes to the JavaScript-facing Babylon.js contract.
