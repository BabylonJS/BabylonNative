#pragma once

#include <Babylon/AppRuntime.h>
#include <Babylon/Graphics/Device.h>
#include <Babylon/Polyfills/Canvas.h>
#include <Babylon/Polyfills/Console.h>
#include <Babylon/Plugins/NativeInput.h>
#include <Babylon/ScriptLoader.h>

#include <functional>
#include <optional>
#include <vector>
#include <string>

class AppContext
{
public:
    using AdditionalInitCallback = std::function<void(Napi::Env)>;
    using DebugLogCallback = std::function<void(const char*)>;

    AppContext(
        Babylon::Graphics::WindowT window,
        size_t width,
        size_t height,
        DebugLogCallback debugLog,
        AdditionalInitCallback additionalInit = {});

    ~AppContext();

    // Copy semantics
    AppContext(const AppContext&) = delete;
    AppContext& operator=(const AppContext&) noexcept = delete;

    // Move semantics
    AppContext(AppContext&&) = delete;
    AppContext& operator=(AppContext&&) noexcept = delete;

    Babylon::Graphics::Device& Device() { return *m_device; }
    Babylon::Graphics::DeviceUpdate& DeviceUpdate() { return *m_deviceUpdate; }
    Babylon::AppRuntime& Runtime() { return *m_runtime; }
    Babylon::Polyfills::Canvas& Canvas() { return *m_canvas; }
    Babylon::Plugins::NativeInput* Input() { return m_input; }
    Babylon::ScriptLoader& ScriptLoader() { return *m_scriptLoader; }

private:
    std::optional<Babylon::Graphics::Device> m_device;
    std::optional<Babylon::Graphics::DeviceUpdate> m_deviceUpdate;
    std::optional<Babylon::AppRuntime> m_runtime;
    std::optional<Babylon::Polyfills::Canvas> m_canvas;
    Babylon::Plugins::NativeInput* m_input{};
    std::optional<Babylon::ScriptLoader> m_scriptLoader;
};
