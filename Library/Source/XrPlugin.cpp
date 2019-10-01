#include "XrPlugin.h"

#include "NativeEngineImpl.h"

#include <XR.h>

namespace
{
    bgfx::TextureFormat::Enum XrTextureFormatToBgfxFormat(xr::TextureFormat format)
    {
        switch (format)
        {
        case xr::TextureFormat::RGBA8:
            return bgfx::TextureFormat::RGBA8;
        case xr::TextureFormat::D24S8:
            return bgfx::TextureFormat::D24S8;
        default:
            throw std::exception{ /* Unsupported texture format */ };
        }
    }
}

namespace babylon
{
    struct XRSessionType
    {
        static inline const std::string IMMERSIVE_VR{ "immersive-vr" };
        static inline const std::string IMMERSIVE_AR{ "immersive-vr" };
        static inline const std::string IMMERSIVE_INLINE{ "inline" };
    };

    struct XRReferenceSpaceType
    {
        static inline const std::string VIEWER{ "viewer" };
        static inline const std::string LOCAL{ "local" };
        static inline const std::string LOCAL_FLOOR{ "local-floor" };
        static inline const std::string BOUNDED_FLOOR{ "bounded-floor" };
        static inline const std::string UNBOUNDED{ "unbounded" };
    };

    struct XREye
    {
        static inline const std::string NONE{ "none" };
        static inline const std::string LEFT{ "left" };
        static inline const std::string RIGHT{ "right" };
    };

    // Implementation of the XRReferenceSpace interface: https://immersive-web.github.io/webxr/#xrreferencespace-interface
    class XRReferenceSpace : public Napi::ObjectWrap<XRReferenceSpace>
    {
        static constexpr auto JS_CLASS_NAME = "XRReferenceSpace";

    public:
        static void Initialize(Napi::Env& env)
        {
            Napi::HandleScope scope{ env };

            Napi::Function func = DefineClass(
                env,
                JS_CLASS_NAME,
                {

                });

            constructor = Napi::Persistent(func);
            constructor.SuppressDestruct();

            env.Global().Set(JS_CLASS_NAME, func);
        }

        static Napi::Object New(const Napi::CallbackInfo& info)
        {
            return constructor.New({ info[0] });
        }

        XRReferenceSpace(const Napi::CallbackInfo& info)
            : Napi::ObjectWrap<XRReferenceSpace>{ info }
        {
            // TODO: Figure out what in the world THIS is supposed to do.
            assert(info[0].As<Napi::String>().Utf8Value() == XRReferenceSpaceType::UNBOUNDED);
        }

    private:
        static inline Napi::FunctionReference constructor{};
    };

    // Implementation of the XRSession interface: https://immersive-web.github.io/webxr/#xrsession-interface
    class XRSession : public Napi::ObjectWrap<XRSession>
    {
        static constexpr auto JS_CLASS_NAME = "XRSession";
        static constexpr auto JS_EVENT_NAME_END = "end";

    public:
        static void Initialize(Napi::Env& env)
        {
            Napi::HandleScope scope{ env };

            Napi::Function func = DefineClass(
                env,
                JS_CLASS_NAME,
                {
                    InstanceMethod("addEventListener", &XRSession::AddEventListener),
                    InstanceMethod("requestReferenceSpace", &XRSession::RequestReferenceSpace),
                    InstanceMethod("updateRenderState", &XRSession::UpdateRenderState),
                    InstanceMethod("requestAnimationFrame", &XRSession::RequestAnimationFrame),
                    InstanceMethod("cancelAnimationFrame", &XRSession::CancelAnimationFrame),
                    InstanceMethod("end", &XRSession::End)
                });

            constructor = Napi::Persistent(func);
            constructor.SuppressDestruct();

            env.Global().Set(JS_CLASS_NAME, func);
        }

        static Napi::Object New(const Napi::CallbackInfo& info)
        {
            return constructor.New({ info[0] });
        }

        XRSession(const Napi::CallbackInfo& info)
            : Napi::ObjectWrap<XRSession>{ info }
        {
            // Currently only immersive VR is supported.
            assert(info[0].As<Napi::String>().Utf8Value() == XRSessionType::IMMERSIVE_VR);
        }

        void SetEngine(NativeEngine::Impl& engine)
        {
            m_engine = &engine;
        }

    private:
        static inline Napi::FunctionReference constructor{};

        std::map<const std::string, Napi::FunctionReference> m_eventNamesToCallbacks{};
        NativeEngine::Impl* m_engine{};

        void AddEventListener(const Napi::CallbackInfo& info)
        {
            auto [pair, success] = m_eventNamesToCallbacks.insert({
                info[0].As<Napi::String>().Utf8Value(),
                Napi::Persistent(info[1].As<Napi::Function>()) });
            assert(success);
        }

        Napi::Value RequestReferenceSpace(const Napi::CallbackInfo& info)
        {
            auto deferred = Napi::Promise::Deferred::New(info.Env());
            deferred.Resolve(XRReferenceSpace::New(info));
            return deferred.Promise();
        }

        Napi::Value UpdateRenderState(const Napi::CallbackInfo& info)
        {
            auto renderState = info[0].As<Napi::Object>();

            auto deferred = Napi::Promise::Deferred::New(info.Env());
            deferred.Resolve(Napi::Value::From(info.Env(), 0));
            return deferred.Promise();
        }

        Napi::Value RequestAnimationFrame(const Napi::CallbackInfo& info)
        {
            m_engine->Dispatch([func = std::make_shared<Napi::FunctionReference>(std::move(Napi::Persistent(info[0].As<Napi::Function>()))), env = info.Env()]()
            {
                func->Call({ Napi::Value::From(env, 12345), Napi::Object::New(env) });
            });
            
            // TODO: Timestamp, I think? Or frame handle? Look up what this return value is and return the right thing.
            return Napi::Value::From(info.Env(), 0);
        }

        void CancelAnimationFrame(const Napi::CallbackInfo& info)
        {
            throw;
        }

        Napi::Value End(const Napi::CallbackInfo& info)
        {
            throw;
        }
    };

    class NativeWebXROutputTarget : public Napi::ObjectWrap<NativeWebXROutputTarget>
    {
        static constexpr auto JS_CLASS_NAME = "NativeWebXROutputTarget";

        // TODO: Remove this once the correct (non-singleton) design is in place for the NativeEngine.
        static auto& GetNativeEngineImplRef(const Napi::CallbackInfo& info)
        {
            auto jsEngine = info[0].As<Napi::Object>();
            auto nativeEngine = jsEngine.Get("_native").As<Napi::Object>();
            auto getEngine = nativeEngine.Get("getEngine").As<Napi::Function>();
            return *getEngine.Call(nativeEngine, {}).As<Napi::External<NativeEngine::Impl>>().Data();
        }

    public:
        static void Initialize(Napi::Env& env)
        {
            Napi::HandleScope scope{ env };

            Napi::Function func = DefineClass(
                env,
                JS_CLASS_NAME,
                {
                    InstanceMethod("initializeXRLayerAsync", &NativeWebXROutputTarget::InitializeXRLayerAsync)
                });

            constructor = Napi::Persistent(func);
            constructor.SuppressDestruct();

            env.Global().Set(JS_CLASS_NAME, func);
        }

        static Napi::Object New(const Napi::CallbackInfo& info)
        {
            return constructor.New({ info[0] });
        }

        NativeWebXROutputTarget(const Napi::CallbackInfo& info)
            : Napi::ObjectWrap<NativeWebXROutputTarget>{ info }
            , m_jsEngineReference{ Napi::Persistent(info[0].As<Napi::Object>()) }
            , m_engine{ GetNativeEngineImplRef(info) }
        {}

    private:
        static inline Napi::FunctionReference constructor{};

        // Lifetime control to prevent the cleanup of the NativeEngine while XR is still alive.
        Napi::ObjectReference m_jsEngineReference{};
        // TODO: Keep a less dramatic impression of the native engine than this.
        NativeEngine::Impl& m_engine;

        Napi::Value InitializeXRLayerAsync(const Napi::CallbackInfo& info)
        {
            // TODO BAD DESIGN: Having this here introduces something of a circular dependency with the session 
            // and the output target having to know about each other. There are several possible solutions to 
            // this, including rearchitecting the JavaScript side slightly (probably the safest, but will be
            // a bit invasive) and combining the session and the output target to be the same thing. For now,
            // keep the circular dependency ONLY TO SEE HOW THE REST OF THE LOGIC FALLS AROUND IT. Once all the
            // logic is in place and working, hopefully it will be clear what's the best way to refactor to 
            // remove the circular dependency.
            auto& session = *XRSession::Unwrap(info[0].As<Napi::Object>());
            session.SetEngine(m_engine);

            // throw std::exception{ /* What the hell are we even supposed to do here? */ };

            auto deferred = Napi::Promise::Deferred::New(info.Env());
            deferred.Resolve(Napi::Value::From(info.Env(), 0)); // TODO: Why is it REQUIRED that I pass a valid value here?
            return deferred.Promise();
        }
    };

    class NativeRenderTargetProvider : public Napi::ObjectWrap<NativeRenderTargetProvider>
    {
        static constexpr auto JS_CLASS_NAME = "NativeRenderTargetProvider";

    public:
        static void Initialize(Napi::Env& env)
        {
            Napi::HandleScope scope{ env };

            Napi::Function func = DefineClass(
                env,
                JS_CLASS_NAME,
                {
                    InstanceMethod("getRenderTargetForEye", &NativeRenderTargetProvider::GetRenderTargetForEye)
                });

            constructor = Napi::Persistent(func);
            constructor.SuppressDestruct();

            env.Global().Set(JS_CLASS_NAME, func);
        }

        static Napi::Object New(const Napi::CallbackInfo& info)
        {
            return constructor.New({ info[0], info[1] });
        }

        NativeRenderTargetProvider(const Napi::CallbackInfo& info)
            : Napi::ObjectWrap<NativeRenderTargetProvider>{ info }
        {
            auto& session = *XRSession::Unwrap(info[0].As<Napi::Object>());
            auto createRenderTextureCallback = info[1].As<Napi::Function>();

            // TODO: Do stuff.
        }

    private:
        static inline Napi::FunctionReference constructor{};

        Napi::Value GetRenderTargetForEye(const Napi::CallbackInfo& info)
        {
            const std::string eye{ info[0].As<Napi::String>().Utf8Value() };

            if (eye == XREye::LEFT)
            {
                throw;
            }
            else if (eye == XREye::RIGHT)
            {
                throw;
            }
            else if (eye == XREye::NONE)
            {
                throw;
            }
            else
            {
                throw std::exception{ /* Unrecognized eye. */ };
            }
        }
    };

    // Implementation of the XR interface: https://immersive-web.github.io/webxr/#xr-interface
    class XR : public Napi::ObjectWrap<XR>
    {
        static constexpr auto JS_CLASS_NAME = "NativeXR";
        static constexpr auto JS_NAVIGATOR_NAME = "navigator";
        static constexpr auto JS_XR_NAME = "xr";
        static constexpr auto JS_NATIVE_NAME = "native";

    public:
        static void Initialize(Napi::Env& env)
        {
            Napi::HandleScope scope{ env };

            Napi::Function func = DefineClass(
                env,
                JS_CLASS_NAME,
                {
                    InstanceMethod("isSessionSupported", &XR::IsSessionSupported),
                    InstanceMethod("requestSession", &XR::RequestSession),
                    InstanceMethod("getWebXROutputTarget", &XR::GetWebXROutputTarget),
                    InstanceMethod("getNativeRenderTargetProvider", &XR::GetNativeRenderTargetProvider),
                    InstanceValue(JS_NATIVE_NAME, Napi::Value::From(env, true))
                });

            Napi::Object global = env.Global();
            Napi::Object navigator;
            if (global.Has(JS_NAVIGATOR_NAME))
            {
                navigator = global.Get(JS_NAVIGATOR_NAME).As<Napi::Object>();
            }
            else
            {
                navigator = Napi::Object::New(env);
                global.Set(JS_NAVIGATOR_NAME, navigator);
            }

            auto xr = func.New({});
            navigator.Set(JS_XR_NAME, xr);
        }

        XR(const Napi::CallbackInfo& info)
            : Napi::ObjectWrap<XR>{ info }
        {}

    private:

        Napi::Value IsSessionSupported(const Napi::CallbackInfo& info)
        {
            auto sessionType = info[0].As<Napi::String>().Utf8Value();
            bool isSupported = false;

            if (sessionType == XRSessionType::IMMERSIVE_VR)
            {
                isSupported = true;
            }

            auto deferred = Napi::Promise::Deferred::New(info.Env());
            deferred.Resolve(Napi::Boolean::New(info.Env(), isSupported));
            return deferred.Promise();
        }

        Napi::Value RequestSession(const Napi::CallbackInfo& info)
        {
            auto deferred = Napi::Promise::Deferred::New(info.Env());
            deferred.Resolve(XRSession::New(info));
            return deferred.Promise();
        }

        Napi::Value GetWebXROutputTarget(const Napi::CallbackInfo& info)
        {
            return NativeWebXROutputTarget::New(info);
        }

        Napi::Value GetNativeRenderTargetProvider(const Napi::CallbackInfo& info)
        {
            return NativeRenderTargetProvider::New(info);
        }
    };


















    class XrPlugin : public Napi::ObjectWrap<XrPlugin>
    {
    public:
        static void Initialize(Napi::Env&);

        XrPlugin::XrPlugin(const Napi::CallbackInfo& info);
        ~XrPlugin();

    private:
        static inline Napi::FunctionReference constructor{};

        void BeginSession(const Napi::CallbackInfo&); // TODO: Make this asynchronous.
        void EndSession(const Napi::CallbackInfo&); // TODO: Make this asynchronous.
        void EndSession();
        void BeginFrame(const Napi::CallbackInfo&);
        void EndFrame(const Napi::CallbackInfo&);
        void EndFrame();
        Napi::Value GetActiveFrameBuffers(const Napi::CallbackInfo& info);

        xr::System m_system{};
        std::unique_ptr<xr::System::Session> m_session{};
        std::unique_ptr<xr::System::Session::Frame> m_frame{};
        FrameBufferManager& m_frameBufferManager;
        std::vector<FrameBufferData*> m_activeFrameBuffers{};

        std::map<uintptr_t, std::unique_ptr<FrameBufferData>> m_texturesToFrameBuffers{};
    };

    void XrPlugin::Initialize(Napi::Env& env)
    {
        Napi::HandleScope scope{ env };

        Napi::Function func = DefineClass(
            env,
            "XrPlugin",
            {
                InstanceMethod("beginSession", &XrPlugin::BeginSession),
                InstanceMethod("endSession", &XrPlugin::EndSession),
                InstanceMethod("beginFrame", &XrPlugin::BeginFrame),
                InstanceMethod("endFrame", &XrPlugin::EndFrame),
                InstanceMethod("getActiveFrameBuffers", &XrPlugin::GetActiveFrameBuffers)
            });

        constructor = Napi::Persistent(func);
        constructor.SuppressDestruct();

        env.Global().Set("XrPlugin", func);
    }

    XrPlugin::XrPlugin(const Napi::CallbackInfo& info)
        : Napi::ObjectWrap<XrPlugin>{ info }
        , m_frameBufferManager{ info[0].As<Napi::External<NativeEngine::Impl>>().Data()->GetFrameBufferManager() }
    {}

    XrPlugin::~XrPlugin()
    {
        if (m_session != nullptr)
        {
            if (m_frame != nullptr)
            {
                EndFrame();
            }

            EndSession();
        }
    }

    // TODO: Make this asynchronous.
    void XrPlugin::BeginSession(const Napi::CallbackInfo&)
    {
        assert(m_session == nullptr);
        assert(m_frame == nullptr);

        if (!m_system.IsInitialized())
        {
            while (!m_system.TryInitialize());
        }

        m_session = std::make_unique<xr::System::Session>(m_system, bgfx::getInternalData()->context);
    }

    // TODO: Make this asynchronous.
    void XrPlugin::EndSession(const Napi::CallbackInfo&)
    {
        EndSession();
    }

    void XrPlugin::EndSession()
    {
        assert(m_session != nullptr);
        assert(m_frame == nullptr);

        m_session->RequestEndSession();

        bool shouldEndSession{};
        bool shouldRestartSession{};
        do
        {
            // Block and burn frames until XR successfully shuts down.
            m_frame = m_session->GetNextFrame(shouldEndSession, shouldRestartSession);
            m_frame.reset();
        } while (!shouldEndSession);
        m_session.reset();
    }

    void XrPlugin::BeginFrame(const Napi::CallbackInfo&)
    {
        assert(m_session != nullptr);
        assert(m_frame == nullptr);

        bool shouldEndSession{};
        bool shouldRestartSession{};
        m_frame = m_session->GetNextFrame(shouldEndSession, shouldRestartSession);

        // Ending a session outside of calls to EndSession() is currently not supported.
        assert(!shouldEndSession);
        assert(m_frame != nullptr);

        m_activeFrameBuffers.reserve(m_frame->Views.size());
        for (const auto& view : m_frame->Views)
        {
            auto colorTexPtr = reinterpret_cast<uintptr_t>(view.ColorTexturePointer);

            auto it = m_texturesToFrameBuffers.find(colorTexPtr);
            if (it == m_texturesToFrameBuffers.end())
            {
                assert(view.ColorTextureSize.Width == view.DepthTextureSize.Width);
                assert(view.ColorTextureSize.Height == view.DepthTextureSize.Height);

                auto colorTextureFormat = XrTextureFormatToBgfxFormat(view.ColorTextureFormat);
                auto depthTextureFormat = XrTextureFormatToBgfxFormat(view.DepthTextureFormat);

                assert(bgfx::isTextureValid(0, false, 1, colorTextureFormat, BGFX_TEXTURE_RT));
                assert(bgfx::isTextureValid(0, false, 1, depthTextureFormat, BGFX_TEXTURE_RT));

                auto colorTex = bgfx::createTexture2D(1, 1, false, 1, colorTextureFormat, BGFX_TEXTURE_RT);
                auto depthTex = bgfx::createTexture2D(1, 1, false, 1, depthTextureFormat, BGFX_TEXTURE_RT);

                // Force BGFX to create the texture now, which is necessary in order to use overrideInternal.
                bgfx::frame();

                bgfx::overrideInternal(colorTex, colorTexPtr);
                bgfx::overrideInternal(depthTex, reinterpret_cast<uintptr_t>(view.DepthTexturePointer));

                std::array<bgfx::Attachment, 2> attachments{};
                attachments[0].init(colorTex);
                attachments[1].init(depthTex);
                auto frameBuffer = bgfx::createFrameBuffer(static_cast<uint8_t>(attachments.size()), attachments.data(), false);

                auto fbPtr = m_frameBufferManager.CreateNew(
                    frameBuffer,
                    static_cast<uint16_t>(view.ColorTextureSize.Width),
                    static_cast<uint16_t>(view.ColorTextureSize.Height));
                m_texturesToFrameBuffers[colorTexPtr] = std::unique_ptr<FrameBufferData>{ fbPtr };

                m_activeFrameBuffers.push_back(fbPtr);
            }
            else
            {
                m_activeFrameBuffers.push_back(it->second.get());
            }
        }
    }

    void XrPlugin::EndFrame(const Napi::CallbackInfo&)
    {
        EndFrame();
    }

    void XrPlugin::EndFrame()
    {
        assert(m_session != nullptr);
        assert(m_frame != nullptr);

        m_activeFrameBuffers.clear();

        m_frame.reset();
    }

    Napi::Value XrPlugin::GetActiveFrameBuffers(const Napi::CallbackInfo& info)
    {
        auto activeFrameBuffers = Napi::Array::New(info.Env(), m_activeFrameBuffers.size());
        for (uint32_t idx = 0; idx < m_activeFrameBuffers.size(); ++idx)
        {
            activeFrameBuffers[idx] = Napi::External<FrameBufferData>::New(info.Env(), m_activeFrameBuffers[idx]);
        }

        return activeFrameBuffers;
    }

    void InitializeXrPlugin(babylon::Env& env)
    {
        XRReferenceSpace::Initialize(env);
        XRSession::Initialize(env);
        NativeWebXROutputTarget::Initialize(env);
        NativeRenderTargetProvider::Initialize(env);
        XR::Initialize(env);

        XrPlugin::Initialize(env);
    }
}
