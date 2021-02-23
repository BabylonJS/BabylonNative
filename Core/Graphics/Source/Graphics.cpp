#include "GraphicsImpl.h"

#include <cassert>
#include <limits>

namespace Babylon
{
    Graphics::Graphics()
        : m_impl{std::make_unique<Impl>()}
    {
    }

    Graphics::~Graphics() = default;

    template<>
    void Graphics::UpdateWindow<WindowType>(WindowType windowPtr)
    {
        m_impl->SetNativeWindow(windowPtr, nullptr);
    }

    template<>
    void Graphics::UpdateWindow<WindowType, void*>(WindowType windowPtr, void* windowTypePtr)
    {
        m_impl->SetNativeWindow(windowPtr, windowTypePtr);
    }

    template<>
    std::unique_ptr<Graphics> Graphics::CreateGraphics<WindowType, size_t, size_t>(WindowType nativeWindowPtr, size_t width, size_t height)
    {
        std::unique_ptr<Graphics> graphics{new Graphics()};
        graphics->UpdateWindow<WindowType>(nativeWindowPtr);
        graphics->UpdateSize(width, height);
        return graphics;
    }

    template<>
    std::unique_ptr<Graphics> Graphics::CreateGraphics<WindowType, void*, size_t, size_t>(WindowType nativeWindowPtr, void* nativeWindowTypePtr, size_t width, size_t height)
    {
        std::unique_ptr<Graphics> graphics{new Graphics()};
        graphics->UpdateWindow<WindowType, void*>(nativeWindowPtr, nativeWindowTypePtr);
        graphics->UpdateSize(width, height);
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
