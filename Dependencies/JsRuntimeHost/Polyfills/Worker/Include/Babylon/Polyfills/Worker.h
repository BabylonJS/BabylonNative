#pragma once

#include <Babylon/Api.h>
#include <napi/env.h>

#include <functional>
#include <string>

namespace Babylon::Polyfills::Worker
{
    struct Options
    {
        // Base directory used for relative paths and app:/// URLs. Files are
        // read directly from this directory when ScriptResolver is not set.
        std::string ScriptRoot{};

        // Optional host asset resolver. The callback receives a normalized
        // worker URL and returns its JavaScript source. It may be called from
        // any Worker runtime thread and therefore must be thread-safe.
        std::function<std::string(const std::string&)> ScriptResolver{};

        // Optional worker-console sink. The callback is invoked on the Worker
        // runtime thread and must be thread-safe.
        std::function<void(const char*)> ConsoleCallback{};
    };

    // Installs Worker, EventTarget, Event, MessageEvent, ErrorEvent and
    // DOMException on the current global object.
    void BABYLON_API Initialize(Napi::Env env, Options options = {});
}
