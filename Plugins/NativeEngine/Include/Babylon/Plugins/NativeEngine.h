#pragma once

#include <napi/env.h>

#include <memory>

namespace Babylon::Plugins::NativeEngine
{
    class RenderToken
    {
    public:
        RenderToken(bool* isTokenStillAlive)
            : m_isTokenStillAlive{isTokenStillAlive}
        {
            *m_isTokenStillAlive = true;
        }

        ~RenderToken()
        {
            *m_isTokenStillAlive = false;
        }

        RenderToken(const RenderToken&) = delete;

    private:
        bool* m_isTokenStillAlive{};
    };

    void InitializeGraphics(void* windowPtr, size_t width, size_t height);

    void Initialize(Napi::Env env);

    std::unique_ptr<RenderToken> Render(Napi::Env);

    void Reinitialize(Napi::Env env, void* windowPtr, size_t width, size_t height);

    void DeinitializeGraphics();
}
