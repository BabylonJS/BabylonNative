#pragma once

#include <Babylon/JsRuntime.h>
#include <Babylon/GraphicsPlatform.h>

#include <memory>

namespace Babylon
{
    class Graphics
    {
    public:
        class Impl;

        ~Graphics();

        static std::unique_ptr<Graphics> CreateGraphics(GraphicsConfiguration config);

        void UpdateWindow(GraphicsConfiguration config);
        void UpdateSize(size_t width, size_t height);

        void AddToJavaScript(Napi::Env);

        void EnableRendering();
        void DisableRendering();

        void StartRenderingCurrentFrame();
        void FinishRenderingCurrentFrame();

        void SetDiagnosticOutput(std::function<void(const char* output)> outputFunction);

        float GetHardwareScalingLevel();
        void SetHardwareScalingLevel(float level);

        float GetDevicePixelRatio() const;

    private:
        Graphics();

        Graphics(const Graphics&) = delete;
        Graphics(Graphics&&) = delete;

        void UpdateWindowInternal(GraphicsConfiguration config);

        float UpdateDevicePixelRatio();

        std::unique_ptr<Impl> m_impl{};
    };
}
