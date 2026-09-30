# AppRuntime

[JsRuntime](JsRuntime.md) is the most fundamental of all Babylon Native 
components, but it is not in any way self-contained. While the `JsRuntime`
abstraction provides a mechanism for getting onto the JavaScript thread and 
interacting with the JavaScript engine instance, it does not actually 
provide either of the things it gives access to. It is merely an integration
point, not an implementation.

The AppRuntime component is the "canonical" implementation that underlies
a JsRuntime. `AppRuntime` is designed to make it easy to integrate, 
initialize, and use everything needed for a Babylon Native app including
a dedicated thread for the JavaScript engine instance to run on, the 
JavaScript engine instance itself, and a `JsRuntime` to provide access to 
both of the prior resources. For most dedicated Babylon Native scenarios, 
creating an `AppRuntime` is the fastest and safest way to create and 
control JavaScript in a way that can be easily consumed by Babylon Native
components.

## Synchronously dispatching rendering work

Do not block the graphics/frame thread on a future or condition variable waiting
for JavaScript that can render or read pixels. JS may need the frame thread to
flush accumulated GPU commands before continuing. Holding a frame-completion
scope protects the encoder but does not make that thread available.

For hosts using the Embedding facade, use `Runtime::RunOnJsThreadAndWait` from
the frame thread after attaching and resizing a View:

```cpp
runtime.RunOnJsThreadAndWait([](Napi::Env env) {
    auto render = env.Global().Get("renderScene").As<Napi::Function>();
    render.Call({});
}, true); // Order this callback after previously queued script loads/evaluations.
view.RenderFrame();
```

The synchronous call services non-presenting mid-frame flushes on the **calling
frame thread** while JS runs. JS resumes with a fresh encoder and view IDs after
each flush, including for synchronous Canvas pixel readback. It does not present
or close the logical frame; the host still drives `View::RenderFrame` normally.
No additional graphics thread is created.

Low-level hosts using `Graphics::Device` and `AppRuntime` can use the same pump:

```cpp
device.StartRenderingCurrentFrame();
device.DispatchAndWait(
    [&appRuntime](auto callback) { appRuntime.Dispatch(std::move(callback)); },
    [](Napi::Env env) {
        // Synchronous JS/native rendering work; JS bindings are already initialized.
        env.Global().Get("renderScene").As<Napi::Function>().Call({});
    });
device.FinishRenderingCurrentFrame();
```

`DispatchAndWait` holds a frame-completion scope before queuing the callback.
Its dispatcher must enqueue the callback exactly once on the JS thread, or throw
without queuing it. The runtime must be running, not suspended. Neither API
waits for a JavaScript Promise started by the callback; they wait for the
callback's synchronous execution. Callback exceptions propagate to the caller;
JS exceptions are converted to `std::runtime_error` on the JS thread so
thread-affine JS references are not transferred to the host thread. The error
includes the message even when the engine's stack omits it. Throwing diagnostic
accessors are reported without replacing the readable parts of the original error.

Do not hold application locks needed by JS or rendering across a synchronous
call, and do not invoke it recursively from a render/flush callback. Embedding
calls require an attached, resized, unsuspended View. Calls from the JS thread,
outside an open frame, or recursively inside the flush pump are rejected.

Asynchronous hosts can continue dispatching work and entering
`FinishRenderingCurrentFrame`, which services the same flush requests while
waiting for frame-completion scopes. Existing asynchronous dispatch APIs and
view/flush budgets are unchanged. An application-owned plain blocking wait
cannot service GPU flushes and must be replaced with the pump-aware call.

## AppRuntime Configuration

For the most part, AppRuntime usage should be quite straightforward, and
examples of how to use it can be found in every implementation of the 
provided Playground example app. However, AppRuntime allows for an unusual
amount of customization at CMake configuration time, which is worth 
a slightly more specific analysis.

AppRuntime is designed to be able to support multiple host platforms 
-- Win32, iOS, etc. -- and multiple JavaScript engines -- JavaScriptCore, 
Chakra, V8 (_not_ JSI, which is exclusive to React Native) -- by 
swapping out platform-specific parts of its implementation using the build 
system at configuration time. This implementation-swapping is controlled
by two CMake variables.

- `NAPI_JAVASCRIPT_ENGINE`: Though this is technically a N-API 
    configuration variable, AppRuntime also reads and reacts to this 
    variable in order to select the implementation that will instantiate
    the desired JavaScript engine. At present, this configuration variable,
    which is defined in the 
    [N-API CMakeLists.txt](../Dependencies/napi/CMakeLists.txt), can take 
    any of the following values:
    - `Chakra`, the default value for Windows targets.
    - `V8`, the default value for Android targets.
    - `JavaScriptCore`, the default value for Apple targets.
    - Note that, while `JSI` is also an allowed value for
        `NAPI_JAVASCRIPT_ENGINE`, AppRuntime does not have an 
        implementation that can instantiate and own a JSI JavaScript engine
        instance.
- `BABYLON_NATIVE_PLATFORM`: This configuration variable is set internally
    by the Babylon Native build system depending on what target platform is
    specified and/or detected. For example, when configuring on Windows 
    without specifying a target platform, the default Win32 target will be 
    used and `BABYLON_NATIVE_PLATFORM` will be set to `Win32`. However, if
    `CMAKE_SYSTEM_NAME` is set to `WindowsStore` and `CMAKE_SYSTEM_VERSION` 
    is set to `10.0`, a Universal Windows target will be specified and
    `BABYLON_NATIVE_PLATFORM` will be set to `UWP`. At present, this 
    configuration variable will be set by the 
    [root-level CMakeLists.txt](../CMakeLists.txt) and can take any of the
    following values:
    - `Android`
    - `Apple` (for all Apple target platforms including iOS and macOS)
    - `UWP`
    - `Win32`
    - `Unix` (for Linux and other non-Apple Unix-like targets)

As mentioned above, these configuration-time variables will cause CMake to
select the platform- and engine-specific behaviors required to allow 
the AppRuntime component to function correctly in the desired scenario. 
None of these configuration variables change the contract of `AppRuntime`
at all; the exact same calling code should work with Chakra on Win32 as 
works with V8 on Android. It is not, however, possible to use every 
possible combination of platforms and engines as some JavaScript engines 
are only available on certain platforms.

As a final note, AppRuntime is not designed to be able to target more than
one platform/engine configuration within the same build. It is not, for 
example, possible to build a Win32 AppRuntime with the ability to target 
both Chakra _and_ V8, nor is it possible to build a version of AppRuntime
that can serve for both Win32 and UWP. Differently-configured `AppRuntime`s
are fundamentally divergent and mutually exclusive types, and to change 
which platform or engine is being used, AppRuntime must be reconfigured and
built again.
