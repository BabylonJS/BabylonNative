#pragma once

#include <napi/env.h>

#include <functional>
#include <memory>
#include <string>

namespace Babylon
{
    class ScriptLoader
    {
    public:
        using DispatchFunctionT = std::function<void(std::function<void(Napi::Env)>)>;

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

        void LoadScript(std::string url);
        void Eval(std::string source, std::string url);
        void Dispatch(std::function<void(Napi::Env)> callback);

    private:
        class Impl;
        std::unique_ptr<Impl> m_impl{};
    };
}
