#include <Babylon/Graphics.h>
#include "GraphicsImpl.h"
#include <Babylon/Graphics.h>

namespace Babylon
{
    Graphics::Graphics()
        : m_impl{std::make_unique<Impl>()}
    {
    }

    Graphics::~Graphics() = default;

    void Graphics::UpdateWindow(GraphicsConfiguration config)
    {
        m_impl->SetNativeWindow(config);
    }

    std::unique_ptr<Graphics> Graphics::CreateGraphics(GraphicsConfiguration config)
    {
        std::unique_ptr<Graphics> graphics{new Graphics()};
        graphics->UpdateWindow(config);
        graphics->UpdateSize(config.width, config.height);
        return graphics;
    }

    void Graphics::UpdateSize(size_t width, size_t height)
    {
        m_impl->Resize(width, height);
    }

    void Graphics::AddToJavaScript(Napi::Env env)
    {
        m_impl->AddToJavaScript(env);
    }

    void Graphics::EnableRendering()
    {
        m_impl->EnableRendering();
    }

    void Graphics::DisableRendering()
    {
        m_impl->DisableRendering();
    }

    void Graphics::StartRenderingCurrentFrame()
    {
        m_impl->StartRenderingCurrentFrame();
    }

    void Graphics::FinishRenderingCurrentFrame()
    {
        m_impl->FinishRenderingCurrentFrame();
    }

    void Graphics::SetDiagnosticOutput(std::function<void(const char* output)> outputFunction)
    {
        m_impl->SetDiagnosticOutput(std::move(outputFunction));
    }

    void Graphics::SetHardwareScalingLevel(float level)
    {
        m_impl->SetHardwareScalingLevel(level);
    }

    float Graphics::GetHardwareScalingLevel()
    {
        return m_impl->GetHardwareScalingLevel();
    }

    float Graphics::GetDevicePixelRatio() const
    {
        return m_impl->GetDevicePixelRatio();
    }
}
