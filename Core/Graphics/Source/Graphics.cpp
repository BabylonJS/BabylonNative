#include <Babylon/Graphics.h>
#include "GraphicsImpl.h"

namespace Babylon
{
    class Graphics::Impl : public GraphicsImpl { };

    Graphics::Graphics()
        : m_impl{std::make_unique<Impl>()}
    {
    }

    Graphics::~Graphics() = default;

    void Graphics::UpdateWindow(const WindowConfiguration& config)
    {
        m_impl->UpdateWindow(config);
    }

    void Graphics::UpdateContext(const ContextConfiguration& config)
    {
        m_impl->UpdateContext(config);
    }

    std::unique_ptr<Graphics> Graphics::CreateGraphics(const WindowConfiguration& config)
    {
        std::unique_ptr<Graphics> graphics{new Graphics()};
        graphics->UpdateWindow(config);
        graphics->UpdateSize(config.Width, config.Height);
        return graphics;
    }

    std::unique_ptr<Graphics> Graphics::CreateGraphics(const ContextConfiguration& config)
    {
        std::unique_ptr<Graphics> graphics{new Graphics()};
        graphics->UpdateContext(config);
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

    Graphics::Update Graphics::GetUpdate(const char* updateName)
    {
        auto& update = m_impl->GetUpdate(updateName);
        return {
            [&update]
            { 
                update.Start();
            },
            [&update](std::function<void()> callback)
            {
                update.EndScheduler()(std::move(callback));
                update.Stop();
            }
        };
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
