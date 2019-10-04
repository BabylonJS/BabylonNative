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

    constexpr std::array<float, 16> IDENTITY_MATRIX
    {
        1.f, 0.f, 0.f, 0.f,
        0.f, 1.f, 0.f, 0.f,
        0.f, 0.f, 1.f, 0.f,
        0.f, 0.f, 0.f, 1.f
    };

    // From https://github.com/microsoft/OpenXR-SDK-VisualStudio/blob/master/samples/XrUtility/XrMath.h#L332-L335
    // 2 * n / (r - l)        0                     0                    0
    // 0                      2 * n / (t - b)       0                    0
    // (r + l) / (r - l)      (t + b) / (t - b)     f / (n - f)         -1
    // 0                      0                     n*f / (n - f)        0
    // Except we're row-major, so we transpose.
    inline std::array<float, 16> createProjectionMatrix(const xr::System::Session::Frame::View & view)
    {
        constexpr float n{ std::min(xr::System::DEPTH_NEAR_Z, xr::System::DEPTH_FAR_Z) };
        constexpr float f{ std::max(xr::System::DEPTH_NEAR_Z, xr::System::DEPTH_FAR_Z) };
        float r{ view.FieldOfView.AngleRight };
        float l{ view.FieldOfView.AngleLeft };
        float t{ view.FieldOfView.AngleUp };
        float b{ view.FieldOfView.AngleDown };

        return{
            2.f * n / (r - l),  0.f,                (r + l) / (r - l),  0.f,
            0.f,                2.f * n / (t - b),  (t + b) / (t - b),  0.f,
            0.f,                0.f,                f / (n - f),        n* f / (n - f),
            0.f,                0.f,                -1.f,               0.f
        };
    }

    // From https://github.com/BabylonJS/Babylon.js/blob/c620b2730fdc42d00d485b9cd43a993fad331254/src/Maths/math.vector.ts#L5216-L5248
    inline std::array<float, 16> createTransformMatrix(const xr::System::Session::Frame::View& view)
    {
        auto& quat = view.Orientation;
        auto& pos = view.Position;

        float xx = quat.X * quat.X;
        float yy = quat.Y * quat.Y;
        float zz = quat.Z * quat.Z;
        float xy = quat.X * quat.Y;
        float zw = quat.Z * quat.W;
        float zx = quat.Z * quat.X;
        float yw = quat.Y * quat.W;
        float yz = quat.Y * quat.Z;
        float xw = quat.X * quat.W;

        auto result{ IDENTITY_MATRIX };

        result[0] = 1.f - (2.f * (yy + zz));
        result[1] = 2.f * (xy + zw);
        result[2] = 2.f * (zx - yw);
        result[3] = pos.X;

        result[4] = 2.f * (xy - zw);
        result[5] = 1.f - (2.f * (zz + xx));
        result[6] = 2.f * (yz + xw);
        result[7] = pos.Y;

        result[8] = 2.f * (zx + yw);
        result[9] = 2.f * (yz - xw);
        result[10] = 1.f - (2.f * (yy + xx));
        result[11] = pos.Z;

        return result;
    }
}

// XrPlugin implementation proper.
namespace babylon
{
    class XrPlugin
    {
    public:
        XrPlugin::XrPlugin();
        ~XrPlugin();

        void BeginSession(); // TODO: Make this asynchronous.
        void EndSession(); // TODO: Make this asynchronous.
        void BeginFrame();
        void EndFrame();

        const auto& Frame() const
        {
            return *m_frame;
        }

        auto ActiveFrameBuffers() const
        {
            return gsl::make_span(m_activeFrameBuffers);
        }

        void SetEngine(Napi::Object& jsEngine)
        {
            auto nativeEngine = jsEngine.Get("_native").As<Napi::Object>();
            auto getEngine = nativeEngine.Get("getEngine").As<Napi::Function>();
            m_engineImpl = getEngine.Call(nativeEngine, {}).As<Napi::External<NativeEngine::Impl>>().Data();
        }

        template<typename CallableT>
        void Dispatch(CallableT&& callable)
        {
            m_engineImpl->Dispatch(callable);
        }

    private:
        xr::System m_system{};
        std::unique_ptr<xr::System::Session> m_session{};
        std::unique_ptr<xr::System::Session::Frame> m_frame{};
        NativeEngine::Impl* m_engineImpl{};
        std::vector<FrameBufferData*> m_activeFrameBuffers{};

        std::map<uintptr_t, std::unique_ptr<FrameBufferData>> m_texturesToFrameBuffers{};
    };

    XrPlugin::XrPlugin()
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
    void XrPlugin::BeginSession()
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

    void XrPlugin::BeginFrame()
    {
        assert(m_engineImpl != nullptr);
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

                auto fbPtr = m_engineImpl->GetFrameBufferManager().CreateNew(
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

    void XrPlugin::EndFrame()
    {
        assert(m_session != nullptr);
        assert(m_frame != nullptr);

        m_activeFrameBuffers.clear();

        m_frame.reset();
    }
}

namespace babylon
{
    namespace
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

            template<typename IdxT>
            static inline const auto& IndexToEye(IdxT idx)
            {
                switch (idx)
                {
                case 0:
                    return LEFT;
                case 1:
                    return RIGHT;
                default:
                    throw std::exception{ /* Unsupported idx */ };
                }
            }

            static inline const auto EyeToIndex(const std::string& eye)
            {
                if (eye == LEFT)
                {
                    return 0;
                }
                else if (eye == RIGHT)
                {
                    return 1;
                }
                else
                {
                    throw std::exception{ /* Unsupported eye */ };
                }
            }
        };

        class XRWebGLLayer : public Napi::ObjectWrap<XRWebGLLayer>
        {
            static constexpr auto JS_CLASS_NAME = "XRWebGLLayer";

        public:
            static void Initialize(Napi::Env& env)
            {
                Napi::HandleScope scope{ env };

                Napi::Function func = DefineClass(
                    env,
                    JS_CLASS_NAME,
                    {
                        InstanceMethod("getViewport", &XRWebGLLayer::GetViewport)
                    });

                constructor = Napi::Persistent(func);
                constructor.SuppressDestruct();

                env.Global().Set(JS_CLASS_NAME, func);
            }

            static Napi::Object New()
            {
                return constructor.New({});
            }

            XRWebGLLayer(const Napi::CallbackInfo& info)
                : Napi::ObjectWrap<XRWebGLLayer>{ info }
            {}

        private:
            static inline Napi::FunctionReference constructor{};

            Napi::Value GetViewport(const Napi::CallbackInfo& info)
            {
                return info.This().As<Napi::Object>().Get("viewport");
            }
        };

        class XRRigidTransform : public Napi::ObjectWrap<XRRigidTransform>
        {
            static constexpr auto JS_CLASS_NAME = "XRRigidTransform";

        public:
            static void Initialize(Napi::Env& env)
            {
                Napi::HandleScope scope{ env };

                Napi::Function func = DefineClass(
                    env,
                    JS_CLASS_NAME,
                    {
                        InstanceAccessor("matrix", &XRRigidTransform::Matrix, nullptr)
                    });

                constructor = Napi::Persistent(func);
                constructor.SuppressDestruct();

                env.Global().Set(JS_CLASS_NAME, func);
            }

            static Napi::Object New(Napi::Env env, gsl::span<const float, 16> matrix)
            {
                auto jsMatrix = Napi::Float32Array::New(env, matrix.size());
                std::memcpy(jsMatrix.Data(), matrix.data(), jsMatrix.ByteLength());
                return constructor.New({ jsMatrix });
            }

            XRRigidTransform(const Napi::CallbackInfo& info)
                : Napi::ObjectWrap<XRRigidTransform>{ info }
            {
                m_matrix = info[0].As<Napi::Float32Array>();
            }

        private:
            static inline Napi::FunctionReference constructor{};

            Napi::Float32Array m_matrix{};

            Napi::Value Matrix(const Napi::CallbackInfo& info)
            {
                return m_matrix;
            }
        };

        class XRView : public Napi::ObjectWrap<XRView>
        {
            static constexpr auto JS_CLASS_NAME = "XRView";

        public:
            static void Initialize(Napi::Env& env)
            {
                Napi::HandleScope scope{ env };

                Napi::Function func = DefineClass(env, JS_CLASS_NAME, {});

                constructor = Napi::Persistent(func);
                constructor.SuppressDestruct();

                env.Global().Set(JS_CLASS_NAME, func);
            }

            static Napi::Object New(Napi::Env env, const std::string& eye, gsl::span<const float, 16> projectionMatrix, gsl::span<const float, 16> transformMatrix)
            {
                auto object = constructor.New({});

                object.Set("eye", Napi::Value::From(env, eye.c_str()));

                auto jsMatrix = Napi::Float32Array::New(env, projectionMatrix.size());
                std::memcpy(jsMatrix.Data(), projectionMatrix.data(), jsMatrix.ByteLength());
                object.Set("projectionMatrix", jsMatrix);

                object.Set("transform", XRRigidTransform::New(env, transformMatrix));

                return object;
            }

            XRView(const Napi::CallbackInfo& info)
                : Napi::ObjectWrap<XRView>{ info }
            {}

        private:
            static inline Napi::FunctionReference constructor{};
        };

        class XRViewerPose : public Napi::ObjectWrap<XRViewerPose>
        {
            static constexpr auto JS_CLASS_NAME = "XRViewerPose";

        public:
            static void Initialize(Napi::Env& env)
            {
                Napi::HandleScope scope{ env };

                Napi::Function func = DefineClass(env, JS_CLASS_NAME, {});

                constructor = Napi::Persistent(func);
                constructor.SuppressDestruct();

                env.Global().Set(JS_CLASS_NAME, func);
            }

            static Napi::Object New(Napi::Env env, gsl::span<const float, 16> matrix, gsl::span<const xr::System::Session::Frame::View> views)
            {
                auto object = constructor.New({});

                object.Set("transform", XRRigidTransform::New(env, matrix));

                auto jsViews = Napi::Array::New(env, views.size());
                for (int idx = 0; idx < views.size(); ++idx)
                {
                    auto& view = views[idx];

                    auto projectionMatrix = createProjectionMatrix(view);

                    auto xrView = XRView::New(
                        env, 
                        XREye::IndexToEye(idx), 
                        projectionMatrix, 
                        createTransformMatrix(view));
                    jsViews.Set(idx, xrView);
                }
                object.Set("views", jsViews);

                return object;
            }

            XRViewerPose(const Napi::CallbackInfo& info)
                : Napi::ObjectWrap<XRViewerPose>{ info }
            {}

        private:
            static inline Napi::FunctionReference constructor{};
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

        class XRFrame : public Napi::ObjectWrap<XRFrame>
        {
            static constexpr auto JS_CLASS_NAME = "XRFrame";

        public:
            static void Initialize(Napi::Env& env)
            {
                Napi::HandleScope scope{ env };

                Napi::Function func = DefineClass(
                    env,
                    JS_CLASS_NAME,
                    {
                        InstanceMethod("getViewerPose", &XRFrame::GetViewerPose)
                    });

                constructor = Napi::Persistent(func);
                constructor.SuppressDestruct();

                env.Global().Set(JS_CLASS_NAME, func);
            }

            static Napi::Object New(const xr::System::Session::Frame& frame)
            {
                auto object = constructor.New({});
                XRFrame::Unwrap(object)->m_frame = &frame;
                return object;
            }

            XRFrame(const Napi::CallbackInfo& info)
                : Napi::ObjectWrap<XRFrame>{ info }
            {}

        private:
            static inline Napi::FunctionReference constructor{};

            const xr::System::Session::Frame* m_frame{};

            Napi::Value GetViewerPose(const Napi::CallbackInfo& info)
            {
                auto& space = *XRReferenceSpace::Unwrap(info[0].As<Napi::Object>());

                // Tracking the location of the space is currently not supported, so XRViewerPose transform
                // is always the identity matrix.
                return XRViewerPose::New(info.Env(), IDENTITY_MATRIX, m_frame->Views);
            }
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
                        InstanceMethod("requestAnimationFrame", &XRSession::RequestAnimationFrame)
                    });

                constructor = Napi::Persistent(func);
                constructor.SuppressDestruct();

                env.Global().Set(JS_CLASS_NAME, func);
            }

            static Napi::Promise CreateAsync(const Napi::CallbackInfo& info)
            {
                auto jsSession = constructor.New({ info[0] });
                auto& session = *XRSession::Unwrap(jsSession);

                session.m_xrPlugin.BeginSession();

                auto deferred = Napi::Promise::Deferred::New(info.Env());
                deferred.Resolve(jsSession);
                return deferred.Promise();
            }

            XRSession(const Napi::CallbackInfo& info)
                : Napi::ObjectWrap<XRSession>{ info }
            {
                // Currently only immersive VR is supported.
                assert(info[0].As<Napi::String>().Utf8Value() == XRSessionType::IMMERSIVE_VR);
            }

            void SetEngine(Napi::Object& jsEngine)
            {
                m_xrPlugin.SetEngine(jsEngine);
            }

            void InitializeXrLayer(Napi::Object layer)
            {
                constexpr size_t WIDTH = 1280;
                constexpr size_t HEIGHT = 1280;

                auto env = layer.Env();
                auto viewport = Napi::Object::New(env);
                viewport.Set("x", Napi::Value::From(env, 0));
                viewport.Set("y", Napi::Value::From(env, 0));
                viewport.Set("width", Napi::Value::From(env, WIDTH));
                viewport.Set("height", Napi::Value::From(env, HEIGHT));
                layer.Set("viewport", viewport);

                layer.Set("framebufferWidth", Napi::Value::From(env, WIDTH));
                layer.Set("framebufferHeight", Napi::Value::From(env, HEIGHT));
            }

            FrameBufferData* GetFrameBufferForEye(const std::string& eye) const
            {
                return m_xrPlugin.ActiveFrameBuffers()[XREye::EyeToIndex(eye)];
            }

        private:
            static inline Napi::FunctionReference constructor{};

            XrPlugin m_xrPlugin{};

            std::map<const std::string, Napi::FunctionReference> m_eventNamesToCallbacks{};

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
                info.This().As<Napi::Object>().Set("renderState", renderState);

                auto deferred = Napi::Promise::Deferred::New(info.Env());
                deferred.Resolve(Napi::Value::From(info.Env(), 0));
                return deferred.Promise();
            }

            Napi::Value RequestAnimationFrame(const Napi::CallbackInfo& info)
            {
                m_xrPlugin.Dispatch([this, func = std::make_shared<Napi::FunctionReference>(std::move(Napi::Persistent(info[0].As<Napi::Function>()))), env = info.Env()]()
                {
                    m_xrPlugin.BeginFrame();

                    auto jsFrame = XRFrame::New(m_xrPlugin.Frame());
                    auto& frame = *XRFrame::Unwrap(jsFrame);

                    // TODO: initialize the frame.

                    func->Call({ Napi::Value::From(env, -1), jsFrame });

                    m_xrPlugin.EndFrame();

                    bgfx::frame();
                });

                // TODO: Timestamp, I think? Or frame handle? Look up what this return value is and return the right thing.
                return Napi::Value::From(info.Env(), 0);
            }
        };

        class NativeWebXRRenderTarget : public Napi::ObjectWrap<NativeWebXRRenderTarget>
        {
            static constexpr auto JS_CLASS_NAME = "NativeWebXRRenderTarget";

        public:
            static void Initialize(Napi::Env& env)
            {
                Napi::HandleScope scope{ env };

                Napi::Function func = DefineClass(
                    env,
                    JS_CLASS_NAME,
                    {
                        InstanceMethod("initializeXRLayerAsync", &NativeWebXRRenderTarget::InitializeXRLayerAsync)
                    });

                constructor = Napi::Persistent(func);
                constructor.SuppressDestruct();

                env.Global().Set(JS_CLASS_NAME, func);
            }

            static Napi::Object New(const Napi::CallbackInfo& info)
            {
                return constructor.New({ info[0] });
            }

            NativeWebXRRenderTarget(const Napi::CallbackInfo& info)
                : Napi::ObjectWrap<NativeWebXRRenderTarget>{ info }
                , m_jsEngineReference{ Napi::Persistent(info[0].As<Napi::Object>()) }
            {}

        private:
            static inline Napi::FunctionReference constructor{};

            // Lifetime control to prevent the cleanup of the NativeEngine while XR is still alive.
            Napi::ObjectReference m_jsEngineReference{};

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
                session.SetEngine(m_jsEngineReference.Value());

                auto xrLayer = XRWebGLLayer::New();
                session.InitializeXrLayer(xrLayer);
                info.This().As<Napi::Object>().Set("xrLayer", xrLayer);

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
                m_session = XRSession::Unwrap(info[0].As<Napi::Object>());

                auto createRenderTextureCallback = info[1].As<Napi::Function>();
                auto dimension = Napi::Value::From(info.Env(), 1280);

                for (size_t idx = 0; idx < m_renderTargetTextures.size(); ++idx)
                {
                    m_renderTargetTextures[idx] = Napi::Persistent(createRenderTextureCallback.Call({ dimension, dimension }).As<Napi::Object>());
                }
            }

        private:
            static inline Napi::FunctionReference constructor{};

            XRSession* m_session{};

            std::array<Napi::ObjectReference, 2> m_renderTargetTextures;

            Napi::Value GetRenderTargetForEye(const Napi::CallbackInfo& info)
            {
                const std::string eye{ info[0].As<Napi::String>().Utf8Value() };

                auto rtt = m_renderTargetTextures[XREye::EyeToIndex(eye)].Value();
                rtt.Set("_framebuffer", Napi::External<FrameBufferData>::New(info.Env(), m_session->GetFrameBufferForEye(eye)));
                return rtt;
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
                        InstanceMethod("getWebXRRenderTarget", &XR::GetWebXRRenderTarget),
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
                return XRSession::CreateAsync(info);
            }

            Napi::Value GetWebXRRenderTarget(const Napi::CallbackInfo& info)
            {
                return NativeWebXRRenderTarget::New(info);
            }

            Napi::Value GetNativeRenderTargetProvider(const Napi::CallbackInfo& info)
            {
                return NativeRenderTargetProvider::New(info);
            }
        };
    }

    void InitializeXrPlugin(babylon::Env& env)
    {
        XRWebGLLayer::Initialize(env);
        XRRigidTransform::Initialize(env);
        XRView::Initialize(env);
        XRViewerPose::Initialize(env);
        XRReferenceSpace::Initialize(env);
        XRFrame::Initialize(env);
        XRSession::Initialize(env);
        NativeWebXRRenderTarget::Initialize(env);
        NativeRenderTargetProvider::Initialize(env);
        XR::Initialize(env);
    }
}
