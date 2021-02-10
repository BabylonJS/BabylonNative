#include "GraphicsImpl.h"

#include <JsRuntimeInternalState.h>

#include <cassert>

#include <Windows.h>

#if (ANDROID)
// MSAA is disabled on Android.
// See issue https://github.com/BabylonJS/BabylonNative/issues/494#issuecomment-731135918
// for explanation
#define BGFX_RESET_FLAGS (BGFX_RESET_VSYNC | BGFX_RESET_MAXANISOTROPY)
#elif __APPLE__
// On Apple devices we need to set BGFX_RESET_FLIP_AFTER_RENDER so that draw calls are immediately pushed
// to the Metal command buffer to avoid a one frame delay.
#define BGFX_RESET_FLAGS (BGFX_RESET_VSYNC | BGFX_RESET_MSAA_X4 | BGFX_RESET_MAXANISOTROPY | BGFX_RESET_FLIP_AFTER_RENDER)
#else
#define BGFX_RESET_FLAGS (BGFX_RESET_VSYNC | BGFX_RESET_MSAA_X4 | BGFX_RESET_MAXANISOTROPY)
#endif

namespace
{
    constexpr auto JS_GRAPHICS_NAME = "_Graphics";

    bool g_bgfxRenderFrameCalled{false};
}

namespace Babylon
{
    Graphics::Impl::UpdateToken::UpdateToken(Graphics::Impl& graphicsImpl)
        : m_graphicsImpl(graphicsImpl)
        , m_punchCard{m_graphicsImpl.m_shiftManager.PunchIn()}
    {
    }

    bgfx::Encoder* Graphics::Impl::UpdateToken::GetEncoder()
    {
        return m_graphicsImpl.GetEncoderForThread();
    }

    FrameBuffer& Graphics::Impl::UpdateToken::AddFrameBuffer(
        bgfx::FrameBufferHandle handle,
        uint16_t width,
        uint16_t height,
        bool backBuffer)
    {
        return m_graphicsImpl.AddFrameBuffer(handle, width, height, backBuffer);
    }

    void Graphics::Impl::UpdateToken::RemoveFrameBuffer(const FrameBuffer& frameBuffer)
    {
        m_graphicsImpl.RemoveFrameBuffer(frameBuffer);
    }

    FrameBuffer& Graphics::Impl::UpdateToken::DefaultFrameBuffer()
    {
        return m_graphicsImpl.DefaultFrameBuffer();
    }

    FrameBuffer& Graphics::Impl::UpdateToken::BoundFrameBuffer()
    {
        return m_graphicsImpl.BoundFrameBuffer();
    }

    Graphics::Impl::Impl()
    {
        // Set the thread affinity (all other rendering operations must happen on this thread).
        m_renderThreadAffinity = std::this_thread::get_id();

        if (!g_bgfxRenderFrameCalled)
        {
            bgfx::renderFrame();
        }

        std::scoped_lock lock{m_state.Mutex};
        m_state.Bgfx.Initialized = false;

        auto& init = m_state.Bgfx.InitState;
#if (ANDROID)
        init.type = bgfx::RendererType::OpenGLES;
#else
        init.type = bgfx::RendererType::Direct3D11;
#endif
        init.resolution.reset = BGFX_RESET_FLAGS;
        init.callback = &m_callback;
    }

    Graphics::Impl::~Impl()
    {
        DisableRendering();
    }

    void* Graphics::Impl::GetNativeWindow()
    {
        std::scoped_lock lock{m_state.Mutex};
        return m_state.Bgfx.InitState.platformData.nwh;
    }

    void Graphics::Impl::SetNativeWindow(void* nativeWindowPtr, void* windowTypePtr)
    {
        std::scoped_lock lock{m_state.Mutex};
        m_state.Bgfx.Dirty = true;

        auto& pd = m_state.Bgfx.InitState.platformData;
        pd.ndt = windowTypePtr;
        pd.nwh = nativeWindowPtr;
        pd.context = nullptr;
        pd.backBuffer = nullptr;
        pd.backBufferDS = nullptr;
    }

    void Graphics::Impl::Resize(size_t width, size_t height)
    {
        std::scoped_lock lock{m_state.Mutex};
        m_state.Resolution.Width = width;
        m_state.Resolution.Height = height;
        UpdateBgfxResolution();
    }

    void Graphics::Impl::AddToJavaScript(Napi::Env env)
    {
        m_jsThreadAffinity = std::this_thread::get_id();

        JsRuntime::NativeObject::GetFromJavaScript(env)
            .Set(JS_GRAPHICS_NAME, Napi::External<Impl>::New(env, this));
    }

    Graphics::Impl& Graphics::Impl::GetFromJavaScript(Napi::Env env)
    {
        return *JsRuntime::NativeObject::GetFromJavaScript(env)
                    .Get(JS_GRAPHICS_NAME)
                    .As<Napi::External<Graphics::Impl>>()
                    .Data();
    }

    Graphics::Impl::RenderScheduler& Graphics::Impl::BeforeRenderScheduler()
    {
        return m_beforeRenderScheduler;
    }

    Graphics::Impl::RenderScheduler& Graphics::Impl::AfterRenderScheduler()
    {
        return m_afterRenderScheduler;
    }

    void Graphics::Impl::EnableRendering()
    {
        assert(m_renderThreadAffinity.check());

        std::scoped_lock lock{m_state.Mutex};

        if (!m_state.Bgfx.Initialized)
        {
            // Initialize bgfx.
            auto& init{m_state.Bgfx.InitState};
            bgfx::setPlatformData(init.platformData);
            bgfx::init(init);

            m_state.Bgfx.Initialized = true;
            m_state.Bgfx.Dirty = false;

            m_frameBufferManager = std::make_unique<FrameBufferManager>();
        }
    }

    void Graphics::Impl::DisableRendering()
    {
        assert(m_renderThreadAffinity.check());

        std::scoped_lock lock{m_state.Mutex};

        if (m_state.Bgfx.Initialized)
        {
            m_cancellationSource.cancel();

            m_frameBufferManager.reset();

            bgfx::shutdown();
            m_state.Bgfx.Initialized = false;

            m_renderThreadAffinity = {};
        }
    }

    void Graphics::Impl::StartRenderingCurrentFrame()
    {
        assert(m_renderThreadAffinity.check());

        // Ensure rendering is enabled.
        EnableRendering();

        // Update bgfx state if necessary.
        UpdateBgfxState();

        m_shiftManager.BeginShift();
        m_beforeRenderScheduler.m_dispatcher.tick(m_cancellationSource);
    }

    void Graphics::Impl::FinishRenderingCurrentFrame()
    {
        assert(m_renderThreadAffinity.check());
        m_shiftManager.EndShift();

        {
            std::scoped_lock lock{m_threadIdToEncoderMutex};
            for (auto [threadId, encoder] : m_threadIdToEncoder)
            {
                bgfx::end(encoder);
            }
            m_threadIdToEncoder.clear();
        }

        Frame();

        m_afterRenderScheduler.m_dispatcher.tick(m_cancellationSource);
    }

    Graphics::Impl::UpdateToken Graphics::Impl::GetUpdateToken()
    {
        return {*this};
    }

    void Graphics::Impl::SetDiagnosticOutput(std::function<void(const char* output)> diagnosticOutput)
    {
        assert(m_renderThreadAffinity.check());
        m_callback.SetDiagnosticOutput(std::move(diagnosticOutput));
    }

    float Graphics::Impl::GetHardwareScalingLevel()
    {
        std::scoped_lock lock{m_state.Mutex};
        return m_state.Resolution.HardwareScalingLevel;
    }

    void Graphics::Impl::SetHardwareScalingLevel(float level)
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

    FrameBuffer& Graphics::Impl::AddFrameBuffer(bgfx::FrameBufferHandle handle, uint16_t width, uint16_t height, bool backBuffer)
    {
        assert(m_jsThreadAffinity.check());
        return m_frameBufferManager->AddFrameBuffer(handle, width, height, backBuffer);
    }

    void Graphics::Impl::RemoveFrameBuffer(const FrameBuffer& frameBuffer)
    {
        assert(m_jsThreadAffinity.check());
        return m_frameBufferManager->RemoveFrameBuffer(frameBuffer);
    }

    FrameBuffer& Graphics::Impl::DefaultFrameBuffer()
    {
        assert(m_jsThreadAffinity.check());
        return m_frameBufferManager->DefaultFrameBuffer();
    }

    FrameBuffer& Graphics::Impl::BoundFrameBuffer()
    {
        assert(m_jsThreadAffinity.check());
        return m_frameBufferManager->BoundFrameBuffer();
    }

    BgfxCallback& Graphics::Impl::Callback()
    {
        return m_callback;
    }

    void Graphics::Impl::UpdateBgfxState()
    {
        std::scoped_lock lock{m_state.Mutex};
        if (m_state.Bgfx.Dirty)
        {
            bgfx::setPlatformData(m_state.Bgfx.InitState.platformData);

            // Ensure bgfx rebinds all texture information.
            bgfx::discard(BGFX_DISCARD_ALL);

            auto& res = m_state.Bgfx.InitState.resolution;
            bgfx::reset(res.width, res.height, BGFX_RESET_FLAGS);
            bgfx::setViewRect(0, 0, 0, static_cast<uint16_t>(res.width), static_cast<uint16_t>(res.height));

            m_state.Bgfx.Dirty = false;
        }
    }

    void Graphics::Impl::UpdateBgfxResolution()
    {
        std::scoped_lock lock{m_state.Mutex};
        m_state.Bgfx.Dirty = true;
        auto& res = m_state.Bgfx.InitState.resolution;
        auto level = m_state.Resolution.HardwareScalingLevel;
        res.width = static_cast<uint32_t>(m_state.Resolution.Width / level);
        res.height = static_cast<uint32_t>(m_state.Resolution.Height / level);
    }

    void Graphics::Impl::DiscardIfDirty()
    {
        std::scoped_lock lock{m_state.Mutex};
        if (m_state.Bgfx.Dirty)
        {
            bgfx::discard();
        }
    }

    void Graphics::Impl::Frame()
    {
        // Check for bgfx views that only called clear and not submit.
        m_frameBufferManager->Check();

        // Discard everything if the bgfx state is dirty.
        DiscardIfDirty();

        // Advance frame and render!
        bgfx::frame();

        // Reset the frame buffers.
        m_frameBufferManager->Reset();
    }

    bgfx::Encoder* Graphics::Impl::GetEncoderForThread()
    {
        assert(!m_renderThreadAffinity.check());
        std::scoped_lock lock{m_threadIdToEncoderMutex};

        const auto threadId = std::this_thread::get_id();
        auto found = m_threadIdToEncoder.find(threadId);
        if (found == m_threadIdToEncoder.end())
        {
            bgfx::Encoder* encoder = bgfx::begin(true);
            m_threadIdToEncoder.emplace(threadId, encoder);
            return encoder;
        }
        else
        {
            return found->second;
        }
    }
}
