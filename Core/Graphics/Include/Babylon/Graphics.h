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
        void UpdateSize(size_t width, size_t height);

        void AddToJavaScript(Napi::Env);
        static Graphics& GetFromJavaScript(Napi::Env);

        void EnableRendering();
        void DisableRendering();

        void StartRenderingCurrentFrame();
        void FinishRenderingCurrentFrame();
        
        void RenderCurrentFrame()
        {
            StartRenderingCurrentFrame();
            FinishRenderingCurrentFrame();
        }

        typedef uint32_t CallbackHandle;
        CallbackHandle RegisterOnBeginFrame(std::function<void()> callback);
        void UnregisterOnBeginFrame(CallbackHandle callbackHandle);

        CallbackHandle RegisterOnEndFrame(std::function<void()> callback);
        void UnregisterOnEndFrame(CallbackHandle callbackHandle);

        void SetDiagnosticOutput(std::function<void(const char* output)> outputFunction);

    private:
        Graphics();

        Graphics(const Graphics&) = delete;
        Graphics(Graphics&&) = delete;

        std::unique_ptr<Impl> m_impl{};
    };
}
