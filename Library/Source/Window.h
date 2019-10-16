#pragma once

#include <napi/napi.h>
#include <chrono>

#include <gsl/gsl>

#include <functional>
#include <vector>

namespace babylon
{
    class RuntimeImpl;

    class Window final
    {
    public:
        Window(RuntimeImpl& runtime);

        auto AddOnResizeCallback(std::function<void(size_t, size_t)>&& handler)
        {
            m_onResizeCallbacks.push_back(handler);

            return gsl::finally([]() {

            });
        }

    private:
        static void SetTimeout(const Napi::CallbackInfo& info);
        static Napi::Value DecodeBase64(const Napi::CallbackInfo& info);

        void RecursiveWaitOrCall(std::shared_ptr<Napi::FunctionReference>, std::chrono::system_clock::time_point);

        RuntimeImpl& m_runtimeImpl;
        std::vector<std::function<void(size_t, size_t)>> m_onResizeCallbacks{};
    };
}
