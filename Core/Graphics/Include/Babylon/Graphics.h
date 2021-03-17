#pragma once

#include <Babylon/JsRuntime.h>

#include <memory>

namespace Babylon
{
    class Graphics
    {
    public:
        class Impl;

        ~Graphics();

        template<typename... Ts>
        static std::unique_ptr<Graphics> CreateGraphics(Ts...);

        template<typename... Ts>
        void UpdateWindow(Ts...);

        void UpdateSize(size_t width, size_t height);

        void AddToJavaScript(Napi::Env);
        static Graphics& GetFromJavaScript(Napi::Env);

        void EnableRendering();
        void DisableRendering();

        void StartRenderingCurrentFrame();
        void FinishRenderingCurrentFrame();

        void SetDiagnosticOutput(std::function<void(const char* output)> outputFunction);

        float GetHardwareScalingLevel();
        void SetHardwareScalingLevel(float level);

    private:
        Graphics();

        Graphics(const Graphics&) = delete;
        Graphics(Graphics&&) = delete;

        std::unique_ptr<Impl> m_impl{};
    };
}
