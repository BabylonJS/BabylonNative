#pragma once

#include <napi/env.h>
#include <Babylon/Api.h>

#include <functional>
#include <memory>
#include <string>

namespace Babylon
{
    class ScriptLoader
    {
    public:
        using DispatchFunctionT = std::function<void BABYLON_API (std::function<void BABYLON_API (Napi::Env)>)>;

        ScriptLoader(DispatchFunctionT dispatchFunction);

        template<typename T>
        ScriptLoader(T& dispatcher)
            : ScriptLoader([&dispatcher](auto func) { dispatcher.Dispatch(std::move(func)); })
        {
        }

        ~ScriptLoader();

        // Move semantics
        ScriptLoader(ScriptLoader&&) noexcept;
        ScriptLoader& operator=(ScriptLoader&&) noexcept;

        // Source containing NUL characters is rejected through the runtime's unhandled exception handler.
        void LoadScript(std::string url);
        void Eval(std::string source, std::string url);
        void Dispatch(std::function<void BABYLON_API (Napi::Env)> callback);

    private:
        class Impl;
        std::unique_ptr<Impl> m_impl{};
    };
}
