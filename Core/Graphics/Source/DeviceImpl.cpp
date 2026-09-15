#include "DeviceImpl.h"

#include <Babylon/Graphics/DeviceQueries.h>
#include <Babylon/Graphics/Platform.h>
#include <Babylon/Graphics/RendererType.h>
#include <Babylon/JsRuntime.h>
#include <arcana/tracing/trace_region.h>
#include <gsl/util>
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <cstring>

#ifdef GRAPHICS_BACK_BUFFER_SUPPORT
#include "ExternalBackBufferD3D11.h"
#endif

#if defined(__APPLE__)
#include <TargetConditionals.h>
#endif

#ifdef BABYLON_NATIVE_CHECK_THREAD_AFFINITY
#define ASSERT_THREAD_AFFINITY(affinity) assert(affinity.check())
#else
#define ASSERT_THREAD_AFFINITY(affinity)
#endif

namespace
{
    constexpr auto JS_GRAPHICS_NAME = "_Graphics";

    bool FuzzyEqual(float a, float b, float epsilon = std::numeric_limits<float>::epsilon())
    {
        return std::abs(a - b) < epsilon;
    }

    // An environment variable counts as set unless it is absent, empty, or exactly "0".
    // Compared in full rather than by first character so that values like "01" or "0x1"
    // enable rather than disable.
    bool IsEnvironmentFlagSet(const char* value)
    {
        return value != nullptr && value[0] != '\0' && std::strcmp(value, "0") != 0;
    }
}

namespace Babylon::Graphics
{
    DeviceImpl::DeviceImpl(const Configuration& config)
        : m_bgfxCallback{[this](const auto& data) { CaptureCallback(data); }}
        , m_context{*this}
        , m_bgfxId{0}
    {
        std::scoped_lock lock{m_state.Mutex};
        m_state.Bgfx.Initialized = false;

        auto& init = m_state.Bgfx.InitState;
        init.callback = &m_bgfxCallback;

        // Opt-in debug device: bgfx only loads RenderDoc (enabling the
        // BGFX_FRAME_DEBUG_CAPTURE trigger used by --capture) when init.debug
        // is set. Release builds default this to false, so gate it behind an
        // env var that is only set when a GPU capture is requested.
        {
#if defined(_WIN32)
            char* renderDocEnv = nullptr;
            size_t renderDocEnvLen = 0;
            if (_dupenv_s(&renderDocEnv, &renderDocEnvLen, "BABYLON_NATIVE_RENDERDOC") == 0 &&
                IsEnvironmentFlagSet(renderDocEnv))
            {
                init.debug = true;
            }
            free(renderDocEnv);
#else
            if (IsEnvironmentFlagSet(std::getenv("BABYLON_NATIVE_RENDERDOC")))
            {
                init.debug = true;
            }
#endif
        }

        // Use the noop renderer if the configuration has no window and no size.
        if (config.Window == WindowT{} && config.Width == 0 && config.Height == 0)
        {
            init.type = bgfx::RendererType::Noop;
        }
        else
        {
            init.type = s_bgfxRenderType;
        }

        //
        // init.platformData
        //
        // Configure the window/device BEFORE the resolution below. The
        // first UpdateSize -> UpdateBgfxResolution syncs the render
        // resolution onto the surface via ResizeRenderSurface, which reads
        // m_state.Window — so the window must already be set. This also
        // matches the re-attach ordering in ViewImpl::InitializeIfReady
        // (UpdateWindow then UpdateSize).
        //

        UpdateWindow(config.Window);
        UpdateDevice(config.Device);

#ifdef GRAPHICS_BACK_BUFFER_SUPPORT
        UpdateBackBuffer(config.BackBufferColor, config.BackBufferDepthStencil);
#endif

        //
        // init.swapChain
        //

        init.reset = BGFX_RESET_VSYNC | BGFX_RESET_MAXANISOTROPY | BGFX_RESET_FLIP_AFTER_RENDER;
        init.swapChain.maxFrameLatency = 1;

        UpdateSize(config.Width, config.Height);
        UpdateMSAA(config.MSAASamples);
        UpdateAlphaPremultiplied(config.AlphaPremultiplied);

        switch (config.BackBufferDepthStencilFormat)
        {
            case DepthStencilFormat::None:
                init.swapChain.formatDepthStencil = bgfx::TextureFormat::Count;
                break;
            case DepthStencilFormat::Depth32:
                // D32 has no DSV mapping on D3D11/D12 in current bgfx; D32F does.
                init.swapChain.formatDepthStencil = bgfx::TextureFormat::D32F;
                break;
            case DepthStencilFormat::Depth24Stencil8:
            default:
                init.swapChain.formatDepthStencil = bgfx::TextureFormat::D24S8;
                break;
        }
    }

    DeviceImpl::~DeviceImpl()
    {
        DisableRendering();
    }

    uintptr_t DeviceImpl::GetId() const
    {
        return m_bgfxId;
    }

    bgfx::FrameBufferHandle DeviceImpl::GetBackBufferHandle() const
    {
#ifdef GRAPHICS_BACK_BUFFER_SUPPORT
        if (m_externalBackBuffer && bgfx::isValid(m_externalBackBuffer->GetFrameBuffer()))
        {
            return m_externalBackBuffer->GetFrameBuffer();
        }
#endif
        return m_windowFrameBuffer;
    }

    void DeviceImpl::UpdateWindow(WindowT window)
    {
        std::scoped_lock lock{m_state.Mutex};
        m_state.Window = window;
        ConfigureBgfxSwapChain(m_state.Bgfx.InitState.swapChain, window);
        m_state.Resolution.DevicePixelRatio = Babylon::Graphics::GetDevicePixelRatio(window);
        m_state.Bgfx.Dirty = true;
    }

    void DeviceImpl::UpdateDevice(DeviceT device)
    {
        std::scoped_lock lock{m_state.Mutex};
        if (m_state.Bgfx.Initialized)
        {
            throw std::runtime_error{"UpdateDevice called while rendering is enabled."};
        }
        m_state.Bgfx.InitState.platformData.context = device;
        m_state.Bgfx.Dirty = true;
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
        auto& init = m_state.Bgfx.InitState;
        init.swapChain.flags &= ~BGFX_SWAP_CHAIN_MSAA_MASK;
        switch (value)
        {
            case 0:
            case 1:
                // disable MSAA
                break;
            case 2:
                init.swapChain.flags |= BGFX_SWAP_CHAIN_MSAA_X2;
                break;
            case 4:
                init.swapChain.flags |= BGFX_SWAP_CHAIN_MSAA_X4;
                break;
            case 8:
                init.swapChain.flags |= BGFX_SWAP_CHAIN_MSAA_X8;
                break;
            case 16:
                init.swapChain.flags |= BGFX_SWAP_CHAIN_MSAA_X16;
                break;
            default:
                m_bgfxCallback.trace(__FILE__, __LINE__, "WARNING: Setting an incorrect value for SetMSAA (%d). Correct values are 0, 1 (disable MSAA) or 2, 4, 8, 16.", static_cast<int>(value));
                break;
        }
        m_state.Bgfx.Dirty = true;
    }

    void DeviceImpl::UpdateAlphaPremultiplied(bool enabled)
    {
        std::scoped_lock lock{m_state.Mutex};
        auto& init = m_state.Bgfx.InitState;
        init.swapChain.flags &= ~BGFX_SWAP_CHAIN_TRANSPARENT_BACKBUFFER;
        init.swapChain.flags |= enabled ? BGFX_SWAP_CHAIN_TRANSPARENT_BACKBUFFER : 0;
        m_state.Bgfx.Dirty = true;
    }

#ifdef GRAPHICS_BACK_BUFFER_SUPPORT
    void DeviceImpl::UpdateBackBuffer(BackBufferColorT backBufferColor, BackBufferDepthStencilT backBufferDepthStencil)
    {
        std::scoped_lock lock{m_state.Mutex};
        m_state.BackBufferColor.copy_from(backBufferColor);
        m_state.BackBufferDepthStencil.copy_from(backBufferDepthStencil);
        m_state.Bgfx.Dirty = true;
    }
#endif

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

    void DeviceImpl::SetRenderResetCallback(std::function<void()> callback)
    {
        m_renderResetCallback = std::move(callback);
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

            bool ready = false;
            const auto rollback = gsl::finally([&] {
                if (!ready)
                {
                    if (m_cancellationSource)
                    {
                        m_cancellationSource->cancel();
                    }
                    if (m_state.Bgfx.Initialized)
                    {
                        DestroyBackBuffer();
                        bgfx::shutdown();
                        m_state.Bgfx.Initialized = false;
                        ++m_bgfxId;
                    }
                    m_rendering = false;
                    m_renderThreadAffinity = {};
                }
            });

            // Select from the host surface; the headless bootstrap must not force Android to Noop.
            ConfigureBgfxRenderType(m_state.Bgfx.InitState);
            auto init{m_state.Bgfx.InitState};
            // Own the window framebuffer explicitly so reattaching a window does not
            // recreate the device or leave bgfx presenting to the previous surface.
            // Retain ndt so the rendering context uses the window's native display.
            init.swapChain.nwh = nullptr;
            init.swapChain.width = 0;
            init.swapChain.height = 0;
            init.swapChain.depth = BGFX_INVALID_HANDLE;
            if (!bgfx::init(init))
            {
                throw std::runtime_error{"Failed to initialize bgfx."};
            }

            m_state.Bgfx.Initialized = true;
            UpdateBackBufferState();

            m_cancellationSource.emplace();

            if (m_bgfxId != 0)
            {
                if (m_renderResetCallback)
                {
                    m_renderResetCallback();
                }
            }
            m_state.Bgfx.Dirty = false;
            ready = true;
        }
    }

    void DeviceImpl::DisableRendering()
    {
        ASSERT_THREAD_AFFINITY(m_renderThreadAffinity);

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

            DestroyBackBuffer();
            bgfx::shutdown();
            m_state.Bgfx.Initialized = false;
            m_bgfxId++;

            m_renderThreadAffinity = {};
        }
    }

    void DeviceImpl::SetDiagnosticOutput(std::function<void(const char* output)> diagnosticOutput)
    {
        ASSERT_THREAD_AFFINITY(m_renderThreadAffinity);
        m_bgfxCallback.SetDiagnosticOutput(std::move(diagnosticOutput));
    }

    void DeviceImpl::StartRenderingCurrentFrame()
    {
        arcana::trace_region startRenderingRegion{"DeviceImpl::StartRenderingCurrentFrame"};

        ASSERT_THREAD_AFFINITY(m_renderThreadAffinity);

        if (m_rendering)
        {
            throw std::runtime_error{"Current frame cannot be started before prior frame has been finished."};
        }

        m_rendering = true;
        m_firstFrameStarted = true;

        // Ensure rendering is enabled.
        EnableRendering();

        // Acquire the frame encoder BEFORE opening the gate. This guarantees
        // that when JS code unblocks from AcquireFrameCompletionScope, the
        // encoder is already available in DeviceContext.
        m_frameEncoder = bgfx::begin(true);

        // Open the gate: allow JS thread to acquire FrameCompletionScopes and use the encoder.
        {
            std::lock_guard lock{m_frameSyncMutex};
            m_frameBlocked = false;
        }
        m_frameSyncCV.notify_all();

        // Tick the frame start dispatcher. This fires requestAnimationFrame tasks that
        // were scheduled by NativeEngine/NativeXr. Those tasks acquire FrameCompletionScopes
        // (keeping the gate reference count > 0) and dispatch JS callbacks to the JS thread.
        m_frameStartDispatcher.tick(*m_cancellationSource);
    }

    void DeviceImpl::FinishRenderingCurrentFrame()
    {
        arcana::trace_region finishRenderingRegion{"DeviceImpl::FinishRenderingCurrentFrame"};

        ASSERT_THREAD_AFFINITY(m_renderThreadAffinity);

        if (!m_rendering)
        {
            if (!m_firstFrameStarted)
            {
                // First call at startup - no frame in progress yet, nothing to finish.
                return;
            }

            throw std::runtime_error{"Current frame cannot be finished prior to having been started."};
        }

        // Close the gate: wait until JS thread has released all FrameCompletionScopes
        // (meaning all encoder work for this frame is done), then block new acquisitions.
        // After this point, no bgfx encoder calls can be in flight on the JS thread.
        //
        // While waiting, also service any mid-frame view-flush requests from the JS
        // thread (see FlushViewsIfNeeded): the JS thread parks itself and we advance a
        // non-presenting bgfx frame here on the render thread to reset the view counter,
        // then hand a fresh encoder back so rendering continues within the same logical
        // frame.
        {
            std::unique_lock lock{m_frameSyncMutex};
            while (true)
            {
                m_frameSyncCV.wait(lock, [this] { return m_pendingFrameScopes == 0 || m_flushRequested; });

                if (m_flushRequested)
                {
                    PerformMidFrameViewFlush();
                    m_flushRequested = false;
                    m_flushCompleteCV.notify_all();
                    continue;
                }

                break;
            }
            m_frameBlocked = true;
        }

        m_beforeRenderDispatcher.tick(*m_cancellationSource);

        // End the frame encoder before calling bgfx::frame(). frame() waits for
        // all encoders to be returned via encoderApiWait(), so the encoder must
        // be ended first to avoid a deadlock on the same thread.
        if (m_frameEncoder)
        {
            bgfx::end(m_frameEncoder);
            m_frameEncoder = nullptr;
        }

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

        std::scoped_lock lock{m_state.Mutex};
        if (!FuzzyEqual(m_state.Resolution.HardwareScalingLevel, level))
        {
            m_state.Resolution.HardwareScalingLevel = level;
            UpdateBgfxResolution();
        }
    }

    float DeviceImpl::GetDevicePixelRatio() const
    {
        std::scoped_lock lock{m_state.Mutex};
        return m_state.Resolution.DevicePixelRatio;
    }

    continuation_scheduler<>& DeviceImpl::BeforeRenderScheduler()
    {
        return m_beforeRenderDispatcher.scheduler();
    }

    continuation_scheduler<>& DeviceImpl::AfterRenderScheduler()
    {
        return m_afterRenderDispatcher.scheduler();
    }

    continuation_scheduler<>& DeviceImpl::FrameStartScheduler()
    {
        return m_frameStartDispatcher.scheduler();
    }

    // Called by FrameCompletionScope constructor (on any thread).
    // Blocks if the gate is closed (m_frameBlocked), meaning bgfx::frame() is running
    // or no frame has started yet. Once unblocked, increments the scope counter.
    void DeviceImpl::IncrementPendingFrameScopes()
    {
        std::unique_lock lock{m_frameSyncMutex};
        m_frameSyncCV.wait(lock, [this] { return !m_frameBlocked; });
        m_pendingFrameScopes++;
    }

    // Called by FrameCompletionScope destructor (on any thread).
    // Decrements the scope counter and wakes the main thread, which may be waiting
    // in FinishRenderingCurrentFrame for all scopes to be released.
    void DeviceImpl::DecrementPendingFrameScopes()
    {
        {
            std::lock_guard lock{m_frameSyncMutex};
            m_pendingFrameScopes--;
        }
        m_frameSyncCV.notify_all();
    }

    void DeviceImpl::SetActiveEncoder(bgfx::Encoder* encoder)
    {
        m_frameEncoder = encoder;
    }

    bgfx::Encoder* DeviceImpl::GetActiveEncoder() const
    {
        return m_frameEncoder;
    }

    void DeviceImpl::RequestScreenShot(std::function<void(std::vector<uint8_t>)> callback)
    {
        m_screenShotCallbacks.push(std::move(callback));
    }

    void DeviceImpl::RequestCaptureNextFrame()
    {
        m_captureNextFrame.store(true);
    }

    arcana::task<void, std::exception_ptr> DeviceImpl::ReadTextureAsync(bgfx::TextureHandle handle, gsl::span<uint8_t> data, uint8_t mipLevel)
    {
        arcana::task_completion_source<void, std::exception_ptr> completionSource{};
        bgfx::TextureRegion region{};
        region.init(handle);
        region.mip = mipLevel;
        m_readTextureRequests.emplace(bgfx::read(region, data.data()), completionSource);
        return completionSource.as_task();
    }

    DeviceImpl::CaptureCallbackTicketT DeviceImpl::AddCaptureCallback(std::function<void(const BgfxCallback::CaptureData&)> callback)
    {
        return m_captureCallbacks.insert(std::move(callback), m_captureCallbacksMutex);
    }

    bgfx::ViewId DeviceImpl::AcquireNewViewId()
    {
        // Saturating increment. A plain fetch_add that is undone on the throw path would be
        // two separate atomic operations, so a concurrent PeekNextViewId could observe an
        // out-of-range value in between, and repeated failed acquisitions could drive the
        // counter past the cap. This loop fuses the "check the cap" and "take the id" steps,
        // clamping at maxViews, so the counter is never observable above the cap no matter
        // how many acquisitions fail.
        const uint32_t maxViews = bgfx::getCaps()->limits.maxViews;

        uint32_t viewId = m_nextViewId.load(std::memory_order_relaxed);
        while (viewId < maxViews && !m_nextViewId.compare_exchange_weak(viewId, viewId + 1))
        {
        }

        if (viewId >= maxViews)
        {
            throw std::runtime_error{"Too many views"};
        }

        return static_cast<bgfx::ViewId>(viewId);
    }

    bgfx::ViewId DeviceImpl::PeekNextViewId() const
    {
        // Saturated at maxViews by AcquireNewViewId, so this always narrows safely.
        return static_cast<bgfx::ViewId>(m_nextViewId.load());
    }

    uint32_t DeviceImpl::ViewIdGeneration() const
    {
        return m_viewIdGeneration.load();
    }

    void DeviceImpl::FlushViewsIfNeeded()
    {
        // Reserve headroom below the hard cap: a single draw/clear operation can
        // acquire a couple of views before the next flush check, and one view
        // (maxViews - 1) is reserved for readback blits.
        constexpr bgfx::ViewId kViewFlushMargin = 16;

        // Maximum mid-frame flushes allowed in one logical frame. Measured: no test in the
        // validation suite needs any at the real 256-view budget, and the heaviest content
        // found so far (the excluded "Nested BBG", which renders in a setInterval) peaks at
        // 5. 64 leaves generous headroom for legitimately heavy content while bounding a
        // mechanism that is otherwise unlimited: each flush is a blocking round-trip to the
        // render thread, so an unbounded number of them would degrade into an apparent hang
        // rather than an error.
        constexpr uint32_t kMaxMidFrameViewFlushes = 64;

        const bgfx::ViewId maxViews = static_cast<bgfx::ViewId>(bgfx::getCaps()->limits.maxViews);
        if (maxViews <= kViewFlushMargin)
        {
            return;
        }

        if (m_nextViewId.load() < static_cast<uint32_t>(maxViews - kViewFlushMargin))
        {
            return;
        }

        // Bound the rescue. Each flush is a blocking round-trip to the render thread, so
        // content that needs an unbounded number of them (e.g. a snippet that renders in a
        // setInterval without ever letting the frame present) would appear to hang rather
        // than fail. Past the budget, stop flushing and let AcquireNewViewId throw
        // "Too many views" — the pre-existing behaviour, and a far better diagnostic than a
        // process that makes progress too slowly to ever finish.
        if (m_midFrameFlushCount.load() >= kMaxMidFrameViewFlushes)
        {
            return;
        }

        // The flush advances a bgfx frame, which must happen on the render (bgfx API)
        // thread. This method is only expected to be called from the JS thread while
        // the render thread is parked in FinishRenderingCurrentFrame. If we're on the
        // render thread (or affinity is unset), there is nothing safe to do here.
        if (m_renderThreadAffinity.check())
        {
            return;
        }

        std::unique_lock lock{m_frameSyncMutex};

        // The mid-frame flush advances a bgfx frame on the render thread, which
        // can only be serviced while the render thread is parked in
        // FinishRenderingCurrentFrame waiting for frame scopes to drain. That is
        // only guaranteed while at least one FrameCompletionScope is active (the
        // normal requestAnimationFrame render path). When a snippet drives frames
        // manually (e.g. setInterval + engine.beginFrame/scene.render/
        // engine.endFrame) there is no frame scope, the render thread is not
        // parked to service the request, and parking the JS thread on
        // m_flushCompleteCV would deadlock. Skip the flush in that case; the hard
        // cap in AcquireNewViewId remains as a backstop. Likewise skip if the gate
        // is currently closed (bgfx::frame() in progress).
        if (m_frameBlocked || m_pendingFrameScopes == 0)
        {
            return;
        }

        m_flushRequested = true;
        m_frameSyncCV.notify_all();
        m_flushCompleteCV.wait(lock, [this] { return !m_flushRequested; });
    }

    // Called on the render thread from FinishRenderingCurrentFrame while holding
    // m_frameSyncMutex, with the requesting JS thread parked in FlushViewsIfNeeded
    // (so the frame encoder is idle). End the current encoder, advance a non-presenting
    // bgfx frame to submit the accumulated views and reset the view counter, then begin
    // a fresh encoder for the remainder of the logical frame.
    void DeviceImpl::PerformMidFrameViewFlush()
    {
        ASSERT_THREAD_AFFINITY(m_renderThreadAffinity);

        if (m_frameEncoder)
        {
            bgfx::end(m_frameEncoder);
            m_frameEncoder = nullptr;
        }

        // BGFX_FRAME_FLUSH executes all queued rendering commands and resets bgfx's per-frame
        // state (including the view counter) without presenting the backbuffer. A plain
        // bgfx::frame() would flip a half-drawn backbuffer to the screen partway through the
        // logical frame; bgfx remembers the flush in m_flushPrevFrame so the next real frame
        // still flips exactly once.
        bgfx::frame(BGFX_FRAME_FLUSH);
        m_nextViewId.store(0);
        m_midFrameFlushCount.fetch_add(1);

        // Publish a new generation so holders of cached view ids (FrameBuffer's m_viewId, the
        // Canvas blit reservation) can detect that their id predates the reset and re-acquire.
        // Without this a cached high id would sort *after* every id handed out from the reset
        // counter, inverting submission order relative to the JS-side draw order.
        m_viewIdGeneration.fetch_add(1);

        m_frameEncoder = bgfx::begin(true);
    }

    void DeviceImpl::UpdateBgfxState()
    {
        std::scoped_lock lock{m_state.Mutex};
        if (m_state.Bgfx.Dirty)
        {
            // Discard the whole frame.
            bgfx::frame(BGFX_FRAME_DISCARD);

            bgfx::reset(m_state.Bgfx.InitState.reset);
            UpdateBackBufferState();
            const auto& res = m_state.Bgfx.InitState.swapChain;
            bgfx::setViewRect(0, 0, 0, static_cast<uint16_t>(res.width), static_cast<uint16_t>(res.height));

            m_state.Bgfx.Dirty = false;
        }
    }

    void DeviceImpl::UpdateBgfxResolution()
    {
        std::scoped_lock lock{m_state.Mutex};
        auto& res = m_state.Bgfx.InitState.swapChain;
        auto level = m_state.Resolution.HardwareScalingLevel;
        res.width = static_cast<uint32_t>(m_state.Resolution.Width / level);
        res.height = static_cast<uint32_t>(m_state.Resolution.Height / level);
        m_state.Bgfx.Dirty = true;

        // Keep the native rendering surface in sync with the render resolution
        // we just computed (platform/graphics-API specific; no-op where the
        // surface size is driven elsewhere).
        ResizeRenderSurface(m_state.Window, res.width, res.height);
    }

    void DeviceImpl::DestroyBackBuffer()
    {
        if (bgfx::isValid(m_windowFrameBuffer))
        {
            bgfx::destroy(m_windowFrameBuffer);
            m_windowFrameBuffer = BGFX_INVALID_HANDLE;
        }
        m_windowHandle = nullptr;
        m_displayHandle = nullptr;
#ifdef GRAPHICS_BACK_BUFFER_SUPPORT
        m_externalBackBuffer.reset();
#endif
    }

    void DeviceImpl::UpdateBackBufferState()
    {
        auto swapChain = m_state.Bgfx.InitState.swapChain;
        swapChain.width = std::max(1u, swapChain.width);
        swapChain.height = std::max(1u, swapChain.height);

#ifdef GRAPHICS_BACK_BUFFER_SUPPORT
        if (m_externalBackBuffer || m_state.BackBufferColor || m_state.BackBufferDepthStencil)
        {
            DestroyBackBuffer();
            // Release the old native swap chain before another one can bind its window.
            bgfx::frame(BGFX_FRAME_DISCARD);
            if (m_state.BackBufferColor || m_state.BackBufferDepthStencil)
            {
                m_externalBackBuffer = std::make_unique<ExternalBackBufferD3D11>(
                    m_state.BackBufferColor.get(), m_state.BackBufferDepthStencil.get(), swapChain);
                if (bgfx::isValid(m_externalBackBuffer->GetFrameBuffer()))
                {
                    return;
                }
                swapChain.depth = m_externalBackBuffer->GetDepthTexture();
            }
        }
#endif

        if (bgfx::isValid(m_windowFrameBuffer) &&
            (m_windowHandle != swapChain.nwh || m_displayHandle != swapChain.ndt))
        {
            DestroyBackBuffer();
            bgfx::frame(BGFX_FRAME_DISCARD);
        }

        if (swapChain.nwh != nullptr)
        {
            if (bgfx::isValid(m_windowFrameBuffer))
            {
                bgfx::updateSwapChain(m_windowFrameBuffer, swapChain);
            }
            else
            {
                m_windowFrameBuffer = bgfx::createFrameBuffer(swapChain);
                if (!bgfx::isValid(m_windowFrameBuffer))
                {
                    throw std::runtime_error{"Failed to create the window frame buffer."};
                }
                m_windowHandle = swapChain.nwh;
                m_displayHandle = swapChain.ndt;
            }
        }
    }

    bool DeviceImpl::RequestScreenShots()
    {
        bool requested = false;
        std::function<void(std::vector<uint8_t>)> callback;
        while (m_screenShotCallbacks.try_pop(callback, *m_cancellationSource))
        {
            m_bgfxCallback.AddScreenShotCallback(std::move(callback));
            requested = true;
        }
        {
            std::scoped_lock lock{m_captureCallbacksMutex};
            if (!m_captureCallbacks.empty())
            {
                m_bgfxCallback.CaptureNextScreenShot();
                requested = true;
            }
        }
        if (!requested)
        {
            return false;
        }
        if (!bgfx::isValid(GetBackBufferHandle()))
        {
            throw std::runtime_error{"Cannot capture without a window or an external back buffer."};
        }
#ifdef GRAPHICS_BACK_BUFFER_SUPPORT
        if (m_externalBackBuffer && bgfx::isValid(m_externalBackBuffer->GetFrameBuffer()))
        {
            return true;
        }
#endif
        // bgfx accepts only one screenshot per framebuffer in a frame.
        bgfx::requestScreenShot(m_windowFrameBuffer, "DeviceImpl::RequestScreenShot");
        return false;
    }

    void DeviceImpl::Frame()
    {
        arcana::trace_region frameRegion{"DeviceImpl::Frame"};

        // Update bgfx state if necessary.
        UpdateBgfxState();

        // Request screen shots before bgfx::frame.
        [[maybe_unused]] const bool externalScreenShot = RequestScreenShots();

        // Advance frame and render!
        const uint8_t frameFlags = m_captureNextFrame.exchange(false) ? BGFX_FRAME_DEBUG_CAPTURE : 0;
        uint32_t frameNumber{bgfx::frame(frameFlags)};

#ifdef GRAPHICS_BACK_BUFFER_SUPPORT
        if (externalScreenShot)
        {
            m_externalBackBuffer->ReadPixels([this](const auto& data) {
                m_bgfxCallback.CompleteScreenShot(data);
            });
        }
#endif

        // Process read texture requests.
        while (!m_readTextureRequests.empty() && m_readTextureRequests.front().first <= frameNumber)
        {
            m_readTextureRequests.front().second.complete();
            m_readTextureRequests.pop();
        }

        m_nextViewId.store(0);
        m_midFrameFlushCount.store(0);
    }

    void DeviceImpl::CaptureCallback(const BgfxCallback::CaptureData& data)
    {
        std::scoped_lock callbackLock{m_captureCallbacksMutex};

        for (const auto& callback : m_captureCallbacks)
        {
            callback(data);
        }
    }
}
