#pragma once

#include <Babylon/JsRuntime.h>
#include <Babylon/GraphicsPlatform.h>

#include <memory>

namespace Babylon
{
    class Graphics
    {
        class Impl;

    public:
        ~Graphics();

        // Note: This API contract is subject to change in coming versions.
        // Features and functionnalities will be added and
        // method and structure might change.

        static std::unique_ptr<Graphics> CreateGraphics(const WindowConfiguration& config);
        static std::unique_ptr<Graphics> CreateGraphics(const ContextConfiguration& config);

        void UpdateWindow(const WindowConfiguration& config);
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

        float GetDevicePixelRatio() const;

    private:
        Graphics();

        Graphics(const Graphics&) = delete;
        Graphics(Graphics&&) = delete;

        void UpdateContext(const ContextConfiguration& config);

        float UpdateDevicePixelRatio();

        std::unique_ptr<Impl> m_impl{};
    };
}
