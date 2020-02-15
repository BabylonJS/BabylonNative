#pragma once

#include "JsRuntime.h"

#include <memory>
#include <string>

namespace Babylon
{
    class AppRuntime final : public JsRuntime
    {
    public:
        AppRuntime(const char* rootUrl = nullptr);

        const std::string& GetRootUrl() const;

        void Suspend();
        void Resume();

        void Dispatch(std::function<void(Napi::Env)> callback);

    private:
        class Impl;
        std::unique_ptr<Impl> m_impl{};
    };
}
