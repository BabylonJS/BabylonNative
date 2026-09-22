#pragma once

#include <napi/env.h>
#include <Babylon/Api.h>

#include <string>

namespace Babylon::Polyfills::Console
{
    /**
     * Importance level of messages sent via logging callbacks.
     */
    enum class LogLevel
    {
        Log,
        Warn,
        Error,
    };

    /**
     * Callback invoked for each `console.log` / `console.warn` / `console.error` call.
     *
     * The `const char*` message is the formatted message produced by joining the call arguments
     * (like browsers do). Its storage is only valid for the duration of the callback -- copy if
     * you need to retain it. The `LogLevel` indicates which `console.*` method was invoked.
     */
    using CallbackT = std::function<void BABYLON_API (const char*, LogLevel)>;

    void BABYLON_API Initialize(Napi::Env env, CallbackT callback);

    /**
     * Capture the JavaScript callstack at the current JS execution point.
     *
     * Only meaningful when called from within a `CallbackT` invocation (or any other Napi callback
     * the polyfill itself registered) -- captures the JS frames that produced the originating
     * `console.*` call. The polyfill's own shim frame(s) are skipped, so the top frame is the
     * user's call site.
     *
     * Format is engine-defined opaque text -- treat as diagnostic-only, do not parse. Capture is
     * **best-effort**: returns an empty string if no JS context is active, the engine doesn't
     * expose a `stack` property on `Error`, or any internal operation fails. Hosts must check
     * for an empty result before using the value.
     *
     * Cost is non-trivial (currently implemented via `new Error().stack` under the hood); the
     * caller decides per-message whether to invoke. Hosts that want stacks only on error
     * messages can branch on `LogLevel`; hosts that want them everywhere are free to do so;
     * hosts that don't care pay nothing.
     */
    std::string BABYLON_API CaptureCurrentJsStack(Napi::Env env);
}
