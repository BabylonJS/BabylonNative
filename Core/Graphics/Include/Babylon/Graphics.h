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

        template<typename NativeWindowT>
        void UpdateWindow(NativeWindowT window);
        void UpdateWindowType(void* type);
        void UpdateSize(size_t width, size_t height);

        void AddToJavaScript(Napi::Env);

        void StartRenderingCurrentFrame();
        void FinishRenderingCurrentFrame();
        
        void RenderCurrentFrame()
        {
            StartRenderingCurrentFrame();
            FinishRenderingCurrentFrame();
        }

    private:
        Graphics();

        Graphics(const Graphics&) = delete;
        Graphics(Graphics&&) = delete;

        std::unique_ptr<Impl> m_impl{};
    };
}
