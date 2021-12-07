#include "GraphicsImpl.h"
#include <Babylon/GraphicsPlatform.h>
#include <Babylon/GraphicsPlatformImpl.h>
#include <JsRuntimeInternalState.h>
#include <arcana/tracing/trace_region.h>

namespace
{
    constexpr auto JS_GRAPHICS_NAME = "_Graphics";
}

namespace Babylon
{
    GraphicsImpl::UpdateToken::UpdateToken(GraphicsImpl& graphicsImpl, SafeTimespanGuarantor& guarantor)
        : m_graphicsImpl(graphicsImpl)
        , m_guarantee{guarantor.GetSafetyGuarantee()}
    {
    }

    bgfx::Encoder* GraphicsImpl::UpdateToken::GetEncoder()
    {
        return m_graphicsImpl.GetEncoderForThread();
    }

    GraphicsImpl::GraphicsImpl()
        : m_bgfxCallback{[this](const auto& data) { CaptureCallback(data); }}
    {
        std::scoped_lock lock{m_state.Mutex};
        m_state.Bgfx.Initialized = false;

        auto& init = m_state.Bgfx.InitState;
        init.type = BgfxDefaultRendererType;
        init.resolution.reset = BGFX_RESET_FLAGS;
        init.callback = &m_bgfxCallback;
    }

    GraphicsImpl::~GraphicsImpl()
    {
        DisableRendering();
    }

    template<>
    WindowType GraphicsImpl::GetNativeWindow<WindowType>()
    {
        std::scoped_lock lock{m_state.Mutex};
        return static_cast<WindowType>(m_state.Bgfx.InitState.platformData.nwh);
    }

    void GraphicsImpl::UpdateWindow(const WindowConfiguration& config)
    {
        std::scoped_lock lock{m_state.Mutex};
        m_state.Bgfx.Dirty = true;
        ConfigureBgfxPlatformData(config, m_state.Bgfx.InitState.platformData);
        UpdateDevicePixelRatio();
    }

    void GraphicsImpl::UpdateContext(const ContextConfiguration& config)
    {
        std::scoped_lock lock{m_state.Mutex};
        m_state.Bgfx.Dirty = true;
        m_state.Resolution.DevicePixelRatio = config.DevicePixelRatio;
        ConfigureBgfxPlatformData(config, m_state.Bgfx.InitState.platformData);
    }

    void GraphicsImpl::Resize(size_t width, size_t height)
    {
        std::scoped_lock lock{m_state.Mutex};
        m_state.Resolution.Width = width;
        m_state.Resolution.Height = height;
        UpdateBgfxResolution();
    }

    size_t GraphicsImpl::GetWidth() const
    {
        return m_state.Resolution.Width;
    }

    size_t GraphicsImpl::GetHeight() const
    {
        return m_state.Resolution.Height;
    }

    void GraphicsImpl::AddToJavaScript(Napi::Env env)
    {
        JsRuntime::NativeObject::GetFromJavaScript(env)
            .Set(JS_GRAPHICS_NAME, Napi::External<GraphicsImpl>::New(env, this));
    }

    GraphicsImpl& GraphicsImpl::GetFromJavaScript(Napi::Env env)
    {
        return *JsRuntime::NativeObject::GetFromJavaScript(env)
                    .Get(JS_GRAPHICS_NAME)
                    .As<Napi::External<GraphicsImpl>>()
                    .Data();
    }

    continuation_scheduler<>& GraphicsImpl::BeforeRenderScheduler()
    {
        return m_beforeRenderDispatcher.scheduler();
    }

    continuation_scheduler<>& GraphicsImpl::AfterRenderScheduler()
    {
        return m_afterRenderDispatcher.scheduler();
    }

    void GraphicsImpl::EnableRendering()
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

    void GraphicsImpl::DisableRendering()
    {
        assert(m_renderThreadAffinity.check());

        std::scoped_lock lock{m_state.Mutex};

        if (m_state.Bgfx.Initialized)
        {
            // HACK: Render one more frame to drain the before/after render work queues.
            StartRenderingCurrentFrame();
            FinishRenderingCurrentFrame();

            m_cancellationSource->cancel();

            bgfx::shutdown();
            m_state.Bgfx.Initialized = false;

            m_renderThreadAffinity = {};
        }
    }

    void GraphicsImpl::StartRenderingCurrentFrame()
    {
        arcana::trace_region startRenderingRegion{"GraphicsImpl::StartRenderingCurrentFrame"};

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

    void GraphicsImpl::FinishRenderingCurrentFrame()
    {
        // Lock the update safe timespans.
        {
            std::scoped_lock lock{m_updateSafeTimespansMutex};
            for (auto& [key, value] : m_updateSafeTimespans)
            {
                value.Lock();
            }
        }
        
        arcana::trace_region finishRenderingRegion{"GraphicsImpl::FinishRenderingCurrentFrame"};

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

    GraphicsImpl::Update GraphicsImpl::GetUpdate(const char* updateName)
    {
        return {GetSafeTimespanGuarantor(updateName), *this};
    }

    SafeTimespanGuarantor& GraphicsImpl::GetSafeTimespanGuarantor(const char* updateName)
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

    void GraphicsImpl::AddTexture(bgfx::TextureHandle handle, uint16_t width, uint16_t height, bool hasMips, uint16_t numLayers, bgfx::TextureFormat::Enum format)
    {
        auto lock{std::unique_lock(m_textureHandleToInfoMutex)};
        TextureInfo textureInfo{width, height, hasMips, numLayers, format};
        m_textureHandleToInfo.emplace(handle.idx, textureInfo);
    }

    void GraphicsImpl::RemoveTexture(bgfx::TextureHandle handle)
    {
        auto lock{std::unique_lock(m_textureHandleToInfoMutex)};
        m_textureHandleToInfo.erase(handle.idx);
    }

    GraphicsImpl::TextureInfo GraphicsImpl::GetTextureInfo(bgfx::TextureHandle handle)
    {
        auto lock{std::unique_lock(m_textureHandleToInfoMutex)};
        return m_textureHandleToInfo[handle.idx];
    }

    void GraphicsImpl::SetDiagnosticOutput(std::function<void(const char* output)> diagnosticOutput)
    {
        assert(m_renderThreadAffinity.check());
        m_bgfxCallback.SetDiagnosticOutput(std::move(diagnosticOutput));
    }

    void GraphicsImpl::RequestScreenShot(std::function<void(std::vector<uint8_t>)> callback)
    {
        m_screenShotCallbacks.push(std::move(callback));
    }

    arcana::task<void, std::exception_ptr> GraphicsImpl::ReadTextureAsync(bgfx::TextureHandle handle, gsl::span<uint8_t> data)
    {
        arcana::task_completion_source<void, std::exception_ptr> completionSource{};
        m_readTextureRequests.emplace(bgfx::readTexture(handle, data.data()), completionSource);
        return completionSource.as_task();
    }

    float GraphicsImpl::GetHardwareScalingLevel()
    {
        std::scoped_lock lock{m_state.Mutex};
        return m_state.Resolution.HardwareScalingLevel;
    }

    void GraphicsImpl::SetHardwareScalingLevel(float level)
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

    GraphicsImpl::CaptureCallbackTicketT GraphicsImpl::AddCaptureCallback(std::function<void(const BgfxCallback::CaptureData&)> callback)
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

    bgfx::ViewId GraphicsImpl::AcquireNewViewId(bgfx::Encoder&)
    {
        return m_nextViewId.fetch_add(1);
    }

    void GraphicsImpl::UpdateBgfxState()
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

            m_state.Bgfx.Dirty = false;
        }
    }

    void GraphicsImpl::UpdateBgfxResolution()
    {
        std::scoped_lock lock{m_state.Mutex};
        m_state.Bgfx.Dirty = true;
        auto& res = m_state.Bgfx.InitState.resolution;
        auto level = m_state.Resolution.HardwareScalingLevel;
        res.width = static_cast<uint32_t>(m_state.Resolution.Width / level);
        res.height = static_cast<uint32_t>(m_state.Resolution.Height / level);
    }

    void GraphicsImpl::DiscardIfDirty()
    {
        std::scoped_lock lock{m_state.Mutex};
        if (m_state.Bgfx.Dirty)
        {
            bgfx::discard();
        }
    }

    void GraphicsImpl::RequestScreenShots()
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
            bgfx::requestScreenShot(BGFX_INVALID_HANDLE, "GraphicsImpl::RequestScreenShot");
        }
    }

    void GraphicsImpl::Frame()
    {
        arcana::trace_region frameRegion{"GraphicsImpl::Frame"};

        // Automatically end bgfx encoders.
        EndEncoders();

        // Discard everything if the bgfx state is dirty.
        DiscardIfDirty();

        // Request screen shots before bgfx::frame.
        RequestScreenShots();

        // Advance frame and render!
        uint32_t frameNumber{bgfx::frame()};

        // Process read texture requests.
        assert(m_readTextureRequests.empty() || m_readTextureRequests.front().first >= frameNumber);
        while (!m_readTextureRequests.empty() && m_readTextureRequests.front().first == frameNumber)
        {
            m_readTextureRequests.front().second.complete();
            m_readTextureRequests.pop();
        }

        m_nextViewId.store(0);
    }

    bgfx::Encoder* GraphicsImpl::GetEncoderForThread()
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

    void GraphicsImpl::EndEncoders()
    {
        std::scoped_lock lock{m_threadIdToEncoderMutex};

        for (auto [threadId, encoder] : m_threadIdToEncoder)
        {
            bgfx::end(encoder);
        }

        m_threadIdToEncoder.clear();
    }

    void GraphicsImpl::CaptureCallback(const BgfxCallback::CaptureData& data)
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

    float GraphicsImpl::GetDevicePixelRatio()
    {
        std::scoped_lock lock{m_state.Mutex};
        return m_state.Resolution.DevicePixelRatio;
    }
}
