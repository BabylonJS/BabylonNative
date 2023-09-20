#include "DeviceImpl.h"

#include <Babylon/Graphics/Platform.h>
#include <Babylon/Graphics/RendererType.h>

#include <Babylon/JsRuntime.h>
#include <arcana/tracing/trace_region.h>

#if defined(__APPLE__)
#include <TargetConditionals.h>
#endif

namespace
{
    constexpr auto JS_GRAPHICS_NAME = "_Graphics";
}

namespace Babylon::Graphics
{
    DeviceImpl::DeviceImpl(const Configuration& config)
        : m_bgfxCallback{[this](const auto& data) { CaptureCallback(data); }}
        , m_context{*this}
    {
        std::scoped_lock lock{m_state.Mutex};
        m_state.Bgfx.Initialized = false;

        auto& init = m_state.Bgfx.InitState;
        init.type = s_bgfxRenderType;
        init.resolution.reset = BGFX_RESET_VSYNC | BGFX_RESET_MAXANISOTROPY;
        init.resolution.maxFrameLatency = 1;
        if (s_bgfxFlipAfterRender)
        {
            init.resolution.reset |= BGFX_RESET_FLIP_AFTER_RENDER;
        }

        init.callback = &m_bgfxCallback;

        init.platformData.context = config.Device;
        UpdateWindow(config.Window);
        UpdateSize(config.Width, config.Height);
        UpdateMSAA(config.MSAASamples);
        UpdateAlphaPremultiplied(config.AlphaPremultiplied);
    }

    DeviceImpl::~DeviceImpl()
    {
        DisableRendering();
    }

    void DeviceImpl::UpdateWindow(WindowT window)
    {
        std::scoped_lock lock{m_state.Mutex};
        m_state.Bgfx.Dirty = true;
        ConfigureBgfxPlatformData(m_state.Bgfx.InitState.platformData, window);
        ConfigureBgfxRenderType(m_state.Bgfx.InitState.platformData, m_state.Bgfx.InitState.type);
        m_state.Resolution.DevicePixelRatio = GetDevicePixelRatio(window);
    }

    void DeviceImpl::UpdateSize(size_t width, size_t height)
    {
        std::scoped_lock lock{m_state.Mutex};
        m_state.Resolution.Width = width;
        m_state.Resolution.Height = height;
        UpdateBgfxResolution();
    }

    void DeviceImpl::UpdateMSAA(uint8_t value)
    {
        std::scoped_lock lock{m_state.Mutex};
        m_state.Bgfx.Dirty = true;
        auto& init = m_state.Bgfx.InitState;
        init.resolution.reset &= ~BGFX_RESET_MSAA_MASK;
        switch (value)
        {
            case 0:
            case 1:
                // disable MSAA
                break;
            case 2:
                init.resolution.reset |= BGFX_RESET_MSAA_X2;
                break;
            case 4:
                init.resolution.reset |= BGFX_RESET_MSAA_X4;
                break;
            case 8:
                init.resolution.reset |= BGFX_RESET_MSAA_X8;
                break;
            case 16:
                init.resolution.reset |= BGFX_RESET_MSAA_X16;
                break;
            default:
                m_bgfxCallback.trace(__FILE__, __LINE__, "WARNING: Setting an incorrect value for SetMSAA (%d). Correct values are 0, 1 (disable MSAA) or 2, 4, 8, 16.", int(value));
                break;
        }
    }

    void DeviceImpl::UpdateAlphaPremultiplied(bool enabled)
    {
        std::scoped_lock lock{m_state.Mutex};
        m_state.Bgfx.Dirty = true;
        auto& init = m_state.Bgfx.InitState;
        init.resolution.reset &= ~BGFX_RESET_TRANSPARENT_BACKBUFFER;
        init.resolution.reset |= enabled ? BGFX_RESET_TRANSPARENT_BACKBUFFER : 0;
    }

    void DeviceImpl::UpdateDevicePixelRatio(float value)
    {
        std::scoped_lock lock{m_state.Mutex};
        m_state.Bgfx.Dirty = true;
        m_state.Resolution.DevicePixelRatio = value;
    }

    void DeviceImpl::AddToJavaScript(Napi::Env env)
    {
        JsRuntime::NativeObject::GetFromJavaScript(env)
            .Set(JS_GRAPHICS_NAME, Napi::External<DeviceImpl>::New(env, this));
    }

    DeviceImpl& DeviceImpl::GetFromJavaScript(Napi::Env env)
    {
        return *JsRuntime::NativeObject::GetFromJavaScript(env)
                    .Get(JS_GRAPHICS_NAME)
                    .As<Napi::External<DeviceImpl>>()
                    .Data();
    }

    Napi::Value DeviceImpl::CreateContext(Napi::Env env)
    {
        return DeviceContext::Create(env, *this);
    }

    void DeviceImpl::EnableRendering()
    {
        std::scoped_lock lock{m_state.Mutex};

        if (!m_state.Bgfx.Initialized)
        {
            // Set the thread affinity (all other rendering operations must happen on this thread).
            m_renderThreadAffinity = std::this_thread::get_id();

            // This tells bgfx to not create its own render thread.
            bgfx::renderFrame();

            // Initialize bgfx.
            auto& init{m_state.Bgfx.InitState};
            bgfx::setPlatformData(init.platformData);
            bgfx::init(init);

            m_state.Bgfx.Initialized = true;
            m_state.Bgfx.Dirty = false;

            m_cancellationSource.emplace();
        }
    }

    void DeviceImpl::DisableRendering()
    {
        assert(m_renderThreadAffinity.check());

        std::scoped_lock lock{m_state.Mutex};

        if (m_state.Bgfx.Initialized)
        {
            // Drain readTextures queue, completing them in an error state.
            while (!m_readTextureRequests.empty())
            {
                auto error = arcana::make_unexpected(std::make_exception_ptr(std::system_error(std::make_error_code(std::errc::operation_canceled))));
                m_readTextureRequests.front().second.complete(error);
                m_readTextureRequests.pop();
            }

            // HACK: Render one more frame to drain the before/after render work queues.
            StartRenderingCurrentFrame();
            FinishRenderingCurrentFrame();

            m_cancellationSource->cancel();

            bgfx::shutdown();
            m_state.Bgfx.Initialized = false;

            m_renderThreadAffinity = {};
        }
    }

    SafeTimespanGuarantor& DeviceImpl::GetSafeTimespanGuarantor(const char* updateName)
    {
        std::scoped_lock lock{m_updateSafeTimespansMutex};
        std::string updateNameStr{updateName};
        auto found = m_updateSafeTimespans.find(updateNameStr);
        if (found == m_updateSafeTimespans.end())
        {
            m_updateSafeTimespans.emplace(std::piecewise_construct, std::forward_as_tuple(updateNameStr), std::forward_as_tuple(m_cancellationSource));
            found = m_updateSafeTimespans.find(updateNameStr);
        }
        return found->second;
    }

    void DeviceImpl::SetDiagnosticOutput(std::function<void(const char* output)> diagnosticOutput)
    {
        assert(m_renderThreadAffinity.check());
        m_bgfxCallback.SetDiagnosticOutput(std::move(diagnosticOutput));
    }

    void DeviceImpl::StartRenderingCurrentFrame()
    {
        arcana::trace_region startRenderingRegion{"DeviceImpl::StartRenderingCurrentFrame"};

        assert(m_renderThreadAffinity.check());

        if (m_rendering)
        {
            throw std::runtime_error{"Current frame cannot be started before prior frame has been finished."};
        }

        m_rendering = true;

        // Ensure rendering is enabled.
        EnableRendering();

        // Update bgfx state if necessary.
        UpdateBgfxState();

        // Unlock the update safe timespans.
        {
            std::scoped_lock lock{m_updateSafeTimespansMutex};
            for (auto& [key, value] : m_updateSafeTimespans)
            {
                value.Unlock();
            }
        }
    }

    void DeviceImpl::FinishRenderingCurrentFrame()
    {
        // Lock the update safe timespans.
        {
            std::scoped_lock lock{m_updateSafeTimespansMutex};
            for (auto& [key, value] : m_updateSafeTimespans)
            {
                value.Lock();
            }
        }

        arcana::trace_region finishRenderingRegion{"DeviceImpl::FinishRenderingCurrentFrame"};

        assert(m_renderThreadAffinity.check());

        if (!m_rendering)
        {
            throw std::runtime_error{"Current frame cannot be finished prior to having been started."};
        }

        m_beforeRenderDispatcher.tick(*m_cancellationSource);

        Frame();

        m_afterRenderDispatcher.tick(*m_cancellationSource);

        m_rendering = false;
    }

    float DeviceImpl::GetHardwareScalingLevel() const
    {
        std::scoped_lock lock{m_state.Mutex};
        return m_state.Resolution.HardwareScalingLevel;
    }

    void DeviceImpl::SetHardwareScalingLevel(float level)
    {
        if (level <= std::numeric_limits<float>::epsilon())
        {
            throw std::runtime_error{"HardwareScalingValue cannot be less than or equal to 0."};
        }

        {
            std::scoped_lock lock{m_state.Mutex};
            m_state.Resolution.HardwareScalingLevel = level;
        }

        UpdateBgfxResolution();
    }

    float DeviceImpl::GetDevicePixelRatio() const
    {
        std::scoped_lock lock{m_state.Mutex};
        return m_state.Resolution.DevicePixelRatio;
    }

    PlatformInfo DeviceImpl::GetPlatformInfo() const
    {
        return {static_cast<DeviceT>(bgfx::getInternalData()->context)};
    }

    continuation_scheduler<>& DeviceImpl::BeforeRenderScheduler()
    {
        return m_beforeRenderDispatcher.scheduler();
    }

    continuation_scheduler<>& DeviceImpl::AfterRenderScheduler()
    {
        return m_afterRenderDispatcher.scheduler();
    }

    void DeviceImpl::RequestScreenShot(std::function<void(std::vector<uint8_t>)> callback)
    {
        m_screenShotCallbacks.push(std::move(callback));
    }

    arcana::task<void, std::exception_ptr> DeviceImpl::ReadTextureAsync(bgfx::TextureHandle handle, gsl::span<uint8_t> data, uint8_t mipLevel)
    {
        arcana::task_completion_source<void, std::exception_ptr> completionSource{};
        m_readTextureRequests.emplace(bgfx::readTexture(handle, data.data(), mipLevel), completionSource);
        return completionSource.as_task();
    }

    DeviceImpl::CaptureCallbackTicketT DeviceImpl::AddCaptureCallback(std::function<void(const BgfxCallback::CaptureData&)> callback)
    {
        // If we're not already capturing, start.
        {
            std::scoped_lock lock{m_state.Mutex};
            if ((m_state.Bgfx.InitState.resolution.reset & BGFX_RESET_CAPTURE) == 0)
            {
                m_state.Bgfx.Dirty = true;
                m_state.Bgfx.InitState.resolution.reset |= BGFX_RESET_CAPTURE;
            }
        }

        return m_captureCallbacks.insert(std::move(callback), m_captureCallbacksMutex);
    }

    bgfx::ViewId DeviceImpl::AcquireNewViewId(bgfx::Encoder&)
    {
        bgfx::ViewId viewId = m_nextViewId.fetch_add(1);
        if (viewId >= bgfx::getCaps()->limits.maxViews)
        {
            throw std::runtime_error{"Too many views"};
        }
        return viewId;
    }

    void DeviceImpl::UpdateBgfxState()
    {
        std::scoped_lock lock{m_state.Mutex};
        if (m_state.Bgfx.Dirty)
        {
            bgfx::setPlatformData(m_state.Bgfx.InitState.platformData);

            // Ensure bgfx rebinds all texture information.
            bgfx::discard(BGFX_DISCARD_ALL);

            auto& res = m_state.Bgfx.InitState.resolution;
            bgfx::reset(res.width, res.height, res.reset);
            bgfx::setViewRect(0, 0, 0, static_cast<uint16_t>(res.width), static_cast<uint16_t>(res.height));
            bgfx::frame();

            m_state.Bgfx.Dirty = false;
        }
    }

    void DeviceImpl::UpdateBgfxResolution()
    {
        std::scoped_lock lock{m_state.Mutex};
        m_state.Bgfx.Dirty = true;
        auto& res = m_state.Bgfx.InitState.resolution;
        auto level = m_state.Resolution.HardwareScalingLevel;
        res.width = static_cast<uint32_t>(m_state.Resolution.Width / level);
        res.height = static_cast<uint32_t>(m_state.Resolution.Height / level);
    }

    void DeviceImpl::DiscardIfDirty()
    {
        std::scoped_lock lock{m_state.Mutex};
        if (m_state.Bgfx.Dirty)
        {
            bgfx::discard();
        }
    }

    void DeviceImpl::RequestScreenShots()
    {
        std::function<void(std::vector<uint8_t>)> callback;
        while (m_screenShotCallbacks.try_pop(callback, *m_cancellationSource))
        {
            m_bgfxCallback.AddScreenShotCallback(std::move(callback));
#if D3D12
            // D3D12 capture is immediate but needs an extra frame swap because back buffer is captured.
            // Because of previous swapchain flip, back buffer is not what's just been rendered.
            bgfx::frame();
#endif
            bgfx::requestScreenShot(BGFX_INVALID_HANDLE, "DeviceImpl::RequestScreenShot");
        }
    }

    void DeviceImpl::Frame()
    {
        arcana::trace_region frameRegion{"DeviceImpl::Frame"};

        // Automatically end bgfx encoders.
        EndEncoders();

        // Discard everything if the bgfx state is dirty.
        DiscardIfDirty();

        // Request screen shots before bgfx::frame.
        RequestScreenShots();

        // Advance frame and render!
        uint32_t frameNumber{bgfx::frame()};

        // Process read texture requests.
        while (!m_readTextureRequests.empty() && m_readTextureRequests.front().first <= frameNumber)
        {
            m_readTextureRequests.front().second.complete();
            m_readTextureRequests.pop();
        }

        m_nextViewId.store(0);
    }

    bgfx::Encoder* DeviceImpl::GetEncoderForThread()
    {
        assert(!m_renderThreadAffinity.check());
        std::scoped_lock lock{m_threadIdToEncoderMutex};

        const auto threadId{std::this_thread::get_id()};
        auto it{m_threadIdToEncoder.find(threadId)};
        if (it == m_threadIdToEncoder.end())
        {
            bgfx::Encoder* encoder{bgfx::begin(true)};
            it = m_threadIdToEncoder.emplace(threadId, encoder).first;
        }

        return it->second;
    }

    void DeviceImpl::EndEncoders()
    {
        std::scoped_lock lock{m_threadIdToEncoderMutex};

        for (auto [threadId, encoder] : m_threadIdToEncoder)
        {
            bgfx::end(encoder);
        }

        m_threadIdToEncoder.clear();
    }

    void DeviceImpl::CaptureCallback(const BgfxCallback::CaptureData& data)
    {
        std::scoped_lock callbackLock{m_captureCallbacksMutex};

        // If no one is listening anymore, stop capturing.
        if (m_captureCallbacks.empty())
        {
            std::scoped_lock stateLock{m_state.Mutex};
            m_state.Bgfx.Dirty = true;
            m_state.Bgfx.InitState.resolution.reset &= ~BGFX_RESET_CAPTURE;
            return;
        }

        for (const auto& callback : m_captureCallbacks)
        {
            callback(data);
        }
    }
}
