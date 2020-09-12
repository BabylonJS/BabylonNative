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

        template<typename NativeWindowT>
        static std::unique_ptr<Graphics> InitializeFromWindow(NativeWindowT window, size_t width, size_t height);

        template<typename NativeWindowT>
        void ReinitializeFromWindow(NativeWindowT window, size_t width, size_t height);

        void AddToJavaScript(Napi::Env);

        void StartRenderingCurrentFrame();
        void FinishRenderingCurrentFrame();
        
        void RenderCurrentFrame()
        {
            StartRenderingCurrentFrame();
            FinishRenderingCurrentFrame();
        }

        void UpdateSize(size_t width, size_t height);

    private:
        Graphics();
        Graphics(const Graphics&) = delete;
        Graphics(Graphics&&) = delete;

        std::unique_ptr<Impl> m_impl{};
    };
}
