#pragma once

#include <Babylon/JsRuntime.h>
#include <Babylon/GraphicsPlatform.h>
#include <Babylon/GraphicsRendererType.h>

#include <future>
#include <memory>

namespace Babylon
{
    class Graphics
    {
        class Impl;

    public:
        class Update
        {
        public:
            Update(const Update&) = default;
            Update(Update&&) = default;
            Update& operator=(const Update&) = default;
            Update& operator=(Update&&) = default;

            void Start()
            {
                m_start();
            }

            void RequestFinish(std::function<void()> onFinishCallback)
            {
                m_requestFinish(std::move(onFinishCallback));
            }

            void Finish()
            {
                std::promise<void> promise{};
                auto future = promise.get_future();
                RequestFinish([&promise] { promise.set_value(); });
                future.wait();
            }

        private:
            friend class Graphics;

            template<typename StartCallableT, typename RequestEndCallableT>
            Update(StartCallableT&& start, RequestEndCallableT&& requestEnd)
                : m_start{std::forward<StartCallableT>(start)}
                , m_requestFinish{std::forward<RequestEndCallableT>(requestEnd)}
            {
            }

            std::function<void()> m_start{};
            std::function<void(std::function<void()>)> m_requestFinish{};
        };

        ~Graphics();

        // Note: This API contract is subject to change in coming versions.
        // Features and functionalities will be added and
        // method and structure might change.

        static std::unique_ptr<Graphics> CreateGraphics(const WindowConfiguration& config);
        static std::unique_ptr<Graphics> CreateGraphics(const ContextConfiguration& config);

        void UpdateWindow(const WindowConfiguration& config);
        void UpdateSize(size_t width, size_t height);

        void AddToJavaScript(Napi::Env);

        void EnableRendering();
        void DisableRendering();

        Update GetUpdate(const char* updateName);

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
