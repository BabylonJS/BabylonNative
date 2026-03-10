#include <Babylon/Graphics/Device.h>
#include "DeviceImpl.h"

namespace Babylon::Graphics
{
    Device::Device(const Configuration& config)
        : m_impl{new DeviceImpl{config}}
    {
    }

    Device::~Device() = default;

    // Move semantics
    Device::Device(Device&& other) noexcept
        : m_impl{std::move(other.m_impl)}
        , m_jsRuntime{other.m_jsRuntime}
        , m_frameLoopRunning{other.m_frameLoopRunning.load()}
        , m_stopRequested{other.m_stopRequested.load()}
    {
        other.m_jsRuntime = nullptr;
    }

    Device& Device::operator=(Device&& other) noexcept
    {
        m_impl = std::move(other.m_impl);
        m_jsRuntime = other.m_jsRuntime;
        m_frameLoopRunning.store(other.m_frameLoopRunning.load());
        m_stopRequested.store(other.m_stopRequested.load());
        other.m_jsRuntime = nullptr;
        return *this;
    }

    void Device::UpdateDevice(DeviceT device) 
    {
       m_impl->UpdateDevice(device);
    }

    void Device::UpdateWindow(WindowT window)
    {
        m_impl->UpdateWindow(window);
    }

    void Device::UpdateSize(size_t width, size_t height)
    {
        m_impl->UpdateSize(width, height);
    }

    void Device::UpdateMSAA(uint8_t value)
    {
        m_impl->UpdateMSAA(value);
    }

    void Device::UpdateAlphaPremultiplied(bool enabled)
    {
        m_impl->UpdateAlphaPremultiplied(enabled);
    }

#ifdef GRAPHICS_BACK_BUFFER_SUPPORT
    void Device::UpdateBackBuffer(BackBufferColorT backBufferColor, BackBufferDepthStencilT backBufferDepthStencil)
    {
        m_impl->UpdateBackBuffer(backBufferColor, backBufferDepthStencil);
    }
#endif

    void Device::AddToJavaScript(Napi::Env env)
    {
        m_impl->AddToJavaScript(env);
        StartFrameLoop("update", env);
    }

    Napi::Value Device::CreateContext(Napi::Env env)
    {
        return m_impl->CreateContext(env);
    }

    void Device::EnableRendering()
    {
        m_impl->EnableRendering();
    }

    void Device::DisableRendering()
    {
        m_impl->DisableRendering();
    }

    void Device::StartRenderingCurrentFrame()
    {
        m_impl->StartRenderingCurrentFrame();
    }

    void Device::FinishRenderingCurrentFrame()
    {
        m_impl->FinishRenderingCurrentFrame();
    }

    void Device::RenderFrame(int32_t _timeoutInMs)
    {
        m_impl->RenderFrame(_timeoutInMs);
    }

    void Device::StartFrameLoop(const char*, Napi::Env env)
    {
        // First frame: init bgfx and open the guarantor.
        m_impl->StartRenderingCurrentFrame();

        m_stopRequested.store(false);
        m_frameLoopRunning.store(true);

        auto& jsRuntime = JsRuntime::GetFromJavaScript(env);
        m_jsRuntime = &jsRuntime;
        auto framePump = std::make_shared<std::function<void(Napi::Env)>>();
        auto* stopFlag = &m_stopRequested;
        auto* runningFlag = &m_frameLoopRunning;
        *framePump = [impl = m_impl.get(), &jsRuntime, framePump, stopFlag, runningFlag](Napi::Env) {
            if (stopFlag->load())
            {
                // Signal that the frame loop is done BEFORE calling
                // FinishRenderingCurrentFrame.  bgfx::frame() inside Finish
                // wakes the main thread's bgfx::renderFrame(); if runningFlag
                // were still true at that point the main thread would re-enter
                // bgfx::renderFrame(-1) and block forever (nobody left to call
                // bgfx::frame).
                runningFlag->store(false);
                impl->FinishRenderingCurrentFrame();
                return;
            }
            impl->FinishRenderingCurrentFrame();
            impl->StartRenderingCurrentFrame();
            jsRuntime.Dispatch(*framePump);
        };
        jsRuntime.Dispatch(*framePump);
    }

    void Device::StopFrameLoop()
    {
        m_stopRequested.store(true);
    }

    bool Device::IsFrameLoopRunning() const
    {
        return m_frameLoopRunning.load();
    }

    void Device::Shutdown(std::function<void()> beforeDisableRendering)
    {
        // Phase 1: Stop the frame loop.  Pump RenderFrame() on this (main)
        // thread so the in-flight bgfx::frame() on the JS thread can complete.
        StopFrameLoop();
        while (IsFrameLoopRunning())
        {
            RenderFrame();
        }

        // Phase 2: Dispatch cleanup + DisableRendering to the JS thread
        // (the thread that owns the bgfx API).
        std::atomic<bool> done{false};
        m_jsRuntime->Dispatch([this, &done, beforeDisableRendering = std::move(beforeDisableRendering)](Napi::Env) {
            if (beforeDisableRendering)
            {
                beforeDisableRendering();
            }
            DisableRendering();
            done.store(true);
        });

        // Pump RenderFrame() so bgfx::frame() / bgfx::shutdown() inside
        // DisableRendering can complete.
        while (!done.load())
        {
            RenderFrame();
        }
    }

    void Device::SetDiagnosticOutput(std::function<void(const char* output)> outputFunction)
    {
        m_impl->SetDiagnosticOutput(std::move(outputFunction));
    }

    void Device::SetHardwareScalingLevel(float level)
    {
        m_impl->SetHardwareScalingLevel(level);
    }

    float Device::GetHardwareScalingLevel()
    {
        return m_impl->GetHardwareScalingLevel();
    }

    float Device::GetDevicePixelRatio() const
    {
        return m_impl->GetDevicePixelRatio();
    }

    PlatformInfo Device::GetPlatformInfo() const
    {
        return m_impl->GetPlatformInfo();
    }
}
