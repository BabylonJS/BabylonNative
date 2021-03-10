#include "NativeXr.h"

#include "NativeEngine.h"
#include <Babylon/JsRuntimeScheduler.h>

#include <XR.h>

#include <bx/bx.h>
#include <bx/math.h>

#include <set>
#include <memory>
#include <napi/napi.h>
#include <arcana/threading/task.h>

namespace
{
    bgfx::TextureFormat::Enum XrTextureFormatToBgfxFormat(xr::TextureFormat format)
    {
        switch (format)
        {
            // Color Formats
            // NOTE: Use linear formats even though XR requests sRGB to match what happens on the web.
            //       WebGL shaders expect sRGB output while native shaders expect linear output.
            case xr::TextureFormat::BGRA8_SRGB:
                return bgfx::TextureFormat::BGRA8;
            case xr::TextureFormat::RGBA8_SRGB:
                return bgfx::TextureFormat::RGBA8;

            // Depth Formats
            case xr::TextureFormat::D24S8:
                return bgfx::TextureFormat::D24S8;

            default:
                throw std::runtime_error{"Unsupported texture format"};
        }
    }

    // clang-format off
    constexpr std::array<float, 16> IDENTITY_MATRIX{
        1.f, 0.f, 0.f, 0.f,
        0.f, 1.f, 0.f, 0.f,
        0.f, 0.f, 1.f, 0.f,
        0.f, 0.f, 0.f, 1.f
    };
    // clang-format on

    std::array<float, 16> CreateTransformMatrix(const xr::System::Session::Frame::Space& space, bool viewSpace = true)
    {
        auto& quat = space.Pose.Orientation;
        auto& pos = space.Pose.Position;

        // Quaternion to matrix from https://github.com/BabylonJS/Babylon.js/blob/v4.0.0/src/Maths/math.ts#L6245-L6283
        const float xx{quat.X * quat.X};
        const float yy{quat.Y * quat.Y};
        const float zz{quat.Z * quat.Z};
        const float xy{quat.X * quat.Y};
        const float zw{quat.Z * quat.W};
        const float zx{quat.Z * quat.X};
        const float yw{quat.Y * quat.W};
        const float yz{quat.Y * quat.Z};
        const float xw{quat.X * quat.W};

        auto worldSpaceTransform{IDENTITY_MATRIX};

        worldSpaceTransform[0] = 1.f - (2.f * (yy + zz));
        worldSpaceTransform[1] = 2.f * (xy + zw);
        worldSpaceTransform[2] = 2.f * (zx - yw);
        worldSpaceTransform[3] = 0.f;

        worldSpaceTransform[4] = 2.f * (xy - zw);
        worldSpaceTransform[5] = 1.f - (2.f * (zz + xx));
        worldSpaceTransform[6] = 2.f * (yz + xw);
        worldSpaceTransform[7] = 0.f;

        worldSpaceTransform[8] = 2.f * (zx + yw);
        worldSpaceTransform[9] = 2.f * (yz - xw);
        worldSpaceTransform[10] = 1.f - (2.f * (yy + xx));
        worldSpaceTransform[11] = 0.f;

        // Insert position into rotation matrix.
        worldSpaceTransform[12] = pos.X;
        worldSpaceTransform[13] = pos.Y;
        worldSpaceTransform[14] = pos.Z;
        worldSpaceTransform[15] = 1.f;

        if (viewSpace)
        {
            // Invert to get the view space transform.
            std::array<float, 16> viewSpaceTransform{};
            bx::mtxInverse(viewSpaceTransform.data(), worldSpaceTransform.data());

            return viewSpaceTransform;
        }
        else
        {
            return worldSpaceTransform;
        }
    }

    constexpr std::array<const char*, 25> HAND_JOINT_NAMES
    {
        "wrist",

        "thumb-metacarpal",
        "thumb-phalanx-proximal",
        "thumb-phalanx-distal",
        "thumb-tip",

        "index-finger-metacarpal",
        "index-finger-phalanx-proximal",
        "index-finger-phalanx-intermediate",
        "index-finger-phalanx-distal",
        "index-finger-tip",

        "middle-finger-metacarpal",
        "middle-finger-phalanx-proximal",
        "middle-finger-phalanx-intermediate",
        "middle-finger-phalanx-distal",
        "middle-finger-tip",

        "ring-finger-metacarpal",
        "ring-finger-phalanx-proximal",
        "ring-finger-phalanx-intermediate",
        "ring-finger-phalanx-distal",
        "ring-finger-tip",

        "pinky-finger-metacarpal",
        "pinky-finger-phalanx-proximal",
        "pinky-finger-phalanx-intermediate",
        "pinky-finger-phalanx-distal",
        "pinky-finger-tip"
    };

    void SetXRInputSourceData(Napi::Object& jsInputSource, xr::System::Session::Frame::InputSource& inputSource)
    {
        auto env = jsInputSource.Env();
        jsInputSource.Set("targetRaySpace", Napi::External<decltype(inputSource.AimSpace)>::New(env, &inputSource.AimSpace));
        jsInputSource.Set("gripSpace", Napi::External<decltype(inputSource.GripSpace)>::New(env, &inputSource.GripSpace));

        // Don't set hands up unless hand data is supported/available
        if (inputSource.JointsTrackedThisFrame)
        {
            auto handJointCollection = Napi::Array::New(env, HAND_JOINT_NAMES.size());

            for (size_t i = 0; i < HAND_JOINT_NAMES.size(); i++)
            {
                auto napiJoint = Napi::External<std::decay_t<decltype(*inputSource.HandJoints.begin())>>::New(env, &inputSource.HandJoints[i]);
                handJointCollection.Set(HAND_JOINT_NAMES[i], napiJoint);
            }
            
            auto jointGetter = [handJointCollection](const Napi::CallbackInfo& info) -> Napi::Value {
                return handJointCollection.Get(info[0].As<Napi::String>());
            };

            handJointCollection.Set("get", Napi::Function::New(env, jointGetter, "get"));
            handJointCollection.Set("size", static_cast<int>(HAND_JOINT_NAMES.size()));

            jsInputSource.Set("hand", handJointCollection);
        }
    }

    void SetXRGamepadObjectData(Napi::Object& jsInputSource, Napi::Object& jsGamepadObject, xr::System::Session::Frame::InputSource& inputSource)
    {
        auto env = jsInputSource.Env();
        //Set Gamepad Object
        auto gamepadButtons = Napi::Array::New(env, inputSource.GamepadObject.Buttons.size());
        for (size_t i = 0; i < inputSource.GamepadObject.Buttons.size(); i++)
        {
            auto gamepadButton = Napi::Object::New(env);
            auto napiGamepadPressed = Napi::Boolean::New(env, inputSource.GamepadObject.Buttons[i].Pressed);
            auto napiGamepadTouched = Napi::Boolean::New(env, inputSource.GamepadObject.Buttons[i].Touched);
            auto napiGamepadValue = Napi::Number::New(env, inputSource.GamepadObject.Buttons[i].Value);
            gamepadButton.Set("pressed", napiGamepadPressed);
            gamepadButton.Set("touched", napiGamepadTouched);
            gamepadButton.Set("value", napiGamepadValue);
            gamepadButtons.Set(static_cast<int>(i), gamepadButton);
        }
        jsGamepadObject.Set("buttons", gamepadButtons);

        auto gamepadAxes = Napi::Array::New(env, inputSource.GamepadObject.Axes.size());
        for (size_t i = 0; i < inputSource.GamepadObject.Axes.size(); i++)
        {
            auto napiGamepadAxesValue = Napi::Number::New(env, inputSource.GamepadObject.Axes[i]);
            gamepadAxes.Set(static_cast<int>(i), napiGamepadAxesValue);
        }
        jsGamepadObject.Set("axes", gamepadAxes);
        jsInputSource.Set("gamepad", jsGamepadObject);
    }

    Napi::ObjectReference CreateXRInputSource(xr::System::Session::Frame::InputSource& inputSource, Napi::Env& env)
    {
        constexpr std::array<const char*, 2> HANDEDNESS_STRINGS{
            "left",
            "right"};
        constexpr const char* TARGET_RAY_MODE{"tracked-pointer"};

        auto jsInputSource = Napi::Object::New(env);

        jsInputSource.Set("handedness", Napi::String::New(env, HANDEDNESS_STRINGS[static_cast<size_t>(inputSource.Handedness)]));
        jsInputSource.Set("targetRayMode", TARGET_RAY_MODE);
        SetXRInputSourceData(jsInputSource, inputSource);

        auto profiles = Napi::Array::New(env, 1);
        Napi::Value string = Napi::String::New(env, "generic-trigger-squeeze-touchpad-thumbstick");
        profiles.Set(uint32_t{0}, string);
        jsInputSource.Set("profiles", profiles);

        return Napi::Persistent(jsInputSource);
    }

    void PopulateDetectionBoundary(const Napi::Object& object, xr::DetectionBoundary& detectionBoundary)
    {
        if (object.Has("type"))
        {
            const std::map<std::string, xr::DetectionBoundaryType> detectionBoundaryTypeMap
            {
                {"box", xr::DetectionBoundaryType::Box},
                {"frustum", xr::DetectionBoundaryType::Frustum},
                {"sphere", xr::DetectionBoundaryType::Sphere}
            };
            detectionBoundary.Type = detectionBoundaryTypeMap.at(object.Get("type").As<Napi::String>());
        }

        switch (detectionBoundary.Type)
        {
        case xr::DetectionBoundaryType::Box:
            if (object.Has("extent"))
            {
                const auto& vector = object.Get("extent").As<Napi::Object>();
                xr::Vector3f boxDimensions{};
                boxDimensions.X = vector.Get("x").As<Napi::Number>();
                boxDimensions.Y = vector.Get("y").As<Napi::Number>();
                boxDimensions.Z = vector.Get("z").As<Napi::Number>();
                detectionBoundary.Data = boxDimensions;
            }
            break;
        case xr::DetectionBoundaryType::Frustum:
            if (object.Has("frustum"))
            {
                const auto& frustum = object.Get("frustum").As<Napi::Object>();
                xr::Frustum frustumData{};
                frustumData.FarDistance = frustum.Get("farDistance").As<Napi::Number>();

                const auto& vector = frustum.Get("position").As<Napi::Object>();
                frustumData.Pose.Position.X = vector.Get("x").As<Napi::Number>();
                frustumData.Pose.Position.Y = vector.Get("y").As<Napi::Number>();
                frustumData.Pose.Position.Z = vector.Get("z").As<Napi::Number>();

                const auto& quaternion = frustum.Get("orientation").As<Napi::Object>();
                frustumData.Pose.Orientation.X = quaternion.Get("x").As<Napi::Number>();
                frustumData.Pose.Orientation.Y = quaternion.Get("y").As<Napi::Number>();
                frustumData.Pose.Orientation.Z = quaternion.Get("z").As<Napi::Number>();
                frustumData.Pose.Orientation.W = quaternion.Get("w").As<Napi::Number>();

                const auto& fov = frustum.Get("fieldOfView").As<Napi::Object>();
                frustumData.FOV.AngleLeft = fov.Get("angleLeft").As<Napi::Number>();
                frustumData.FOV.AngleRight = fov.Get("angleRight").As<Napi::Number>();
                frustumData.FOV.AngleUp = fov.Get("angleUp").As<Napi::Number>();
                frustumData.FOV.AngleDown = fov.Get("angleDown").As<Napi::Number>();

                frustumData.FarDistance = frustum.Get("farDistance").As<Napi::Number>();

                detectionBoundary.Data = frustumData;
            }
            break;
        case xr::DetectionBoundaryType::Sphere:
            if (object.Has("radius"))
            {
                detectionBoundary.Data = object.Get("radius").As<Napi::Number>();
            }
            break;
        }
    }

    xr::GeometryDetectorOptions CreateDetectorOptions(const Napi::Object& object)
    {
        xr::GeometryDetectorOptions options{};
        if (object.Has("updateInterval"))
        {
            options.UpdateInterval = object.Get("updateInterval").As<Napi::Number>();
        }

        if (object.Has("detectionBoundary"))
        {
            const auto& detectionBoundary = object.Get("detectionBoundary").As<Napi::Object>();
            PopulateDetectionBoundary(detectionBoundary, options.DetectionBoundary);
        }

        return options;
    }

    void CreateXRGamepadObject(Napi::Object& jsInputSource, xr::System::Session::Frame::InputSource& inputSource)
    {
        auto env = jsInputSource.Env();
        auto jsGamepadObject = Napi::Object::New(env);
        SetXRGamepadObjectData(jsInputSource, jsGamepadObject, inputSource);
    }
}

// NativeXr implementation proper.
namespace Babylon
{
    namespace Plugins
    {
        class NativeXr::Impl : public std::enable_shared_from_this<NativeXr::Impl>
        {
        public:
            Impl(Napi::Env, JsRuntimeScheduler& runtimeScheduler, std::function<void(bool)> sessionStateChangedCallback, std::function<void*()> windowProvider);

            arcana::task<void, std::exception_ptr> BeginSessionAsync();
            arcana::task<void, std::exception_ptr> EndSessionAsync();

            void ScheduleFrame(std::function<void(const xr::System::Session::Frame&)>&& callback);

            void SetRenderTextureFunctions(const Napi::Function& createFunction, const Napi::Function& destroyFunction)
            {
                m_createRenderTexture = Napi::Persistent(createFunction);
                m_destroyRenderTexture = Napi::Persistent(destroyFunction);
            }

            Napi::Value GetRenderTargetForViewIndex(int viewIndex) const
            {
                auto itTextureToFrameBuffer{m_textureToFrameBufferMap.find(m_activeTextures[viewIndex])};
                if (itTextureToFrameBuffer == m_textureToFrameBufferMap.end())
                {
                    return m_env.Null();
                }

                auto itFrameBufferToJsTexture{m_frameBufferToJsTextureMap.find(itTextureToFrameBuffer->second)};
                return itFrameBufferToJsTexture->second.Value();
            }

            void SetDepthsNarFar(float depthNear, float depthFar)
            {
                m_session->SetDepthsNearFar(depthNear, depthFar);
            }

            void SetPlaneDetectionEnabled(bool enabled)
            {
                m_session->SetPlaneDetectionEnabled(enabled);
            }

            bool TrySetFeaturePointCloudEnabled(bool enabled)
            {
                return m_session->TrySetFeaturePointCloudEnabled(enabled);
            }

            bool TrySetPreferredPlaneDetectorOptions(const xr::GeometryDetectorOptions& options)
            {
                return m_session->TrySetPreferredPlaneDetectorOptions(options);
            }

            bool TrySetMeshDetectorEnabled(const bool enabled)
            {
                return m_session->TrySetMeshDetectorEnabled(enabled);
            }

            bool TrySetPreferredMeshDetectorOptions(const xr::GeometryDetectorOptions& options)
            {
                return m_session->TrySetPreferredMeshDetectorOptions(options);
            }

            uintptr_t GetNativeXrContext()
            {
                return m_session->GetNativeXrContext();
            }

            std::string GetNativeXrContextType()
            {
                return m_session->GetNativeXrContextType();
            }

        private:
            Napi::Env m_env;
            JsRuntimeScheduler& m_runtimeScheduler;
            Graphics::Impl& m_graphicsImpl;
            Napi::FunctionReference m_createRenderTexture{};
            Napi::FunctionReference m_destroyRenderTexture{};
            std::map<void*, FrameBuffer*> m_textureToFrameBufferMap{};
            std::map<FrameBuffer*, Napi::ObjectReference> m_frameBufferToJsTextureMap{};
            std::vector<void*> m_activeTextures{};
            std::unique_ptr<xr::System> m_system{};
            std::shared_ptr<xr::System::Session> m_session{};
            std::unique_ptr<xr::System::Session::Frame> m_frame{};
            arcana::cancellation_source m_cancellationSource{};
            bool m_frameScheduled{false};
            std::vector<std::function<void(const xr::System::Session::Frame&)>> m_scheduleFrameCallbacks{};
            arcana::task<void, std::exception_ptr> m_frameTask{};
            std::function<void(bool)> m_sessionStateChangedCallback{};
            std::function<void*()> m_windowProvider{};

            void BeginFrame();
            void BeginUpdate();
            void EndUpdate();
            void EndFrame();
        };

        NativeXr::Impl::Impl(Napi::Env env, JsRuntimeScheduler& runtimeScheduler, std::function<void(bool)> sessionStateChangedCallback, std::function<void*()> windowProvider)
            : m_env{env}
            , m_runtimeScheduler{runtimeScheduler}
            , m_graphicsImpl{Graphics::Impl::GetFromJavaScript(env)}
            , m_sessionStateChangedCallback{std::move(sessionStateChangedCallback)}
            , m_windowProvider{std::move(windowProvider)}
        {
        }

        arcana::task<void, std::exception_ptr> NativeXr::Impl::BeginSessionAsync()
        {
            return arcana::make_task(m_graphicsImpl.AfterRenderScheduler(), arcana::cancellation::none(),
                [this, thisRef{shared_from_this()}]() {
                    assert(m_system == nullptr);
                    assert(m_session == nullptr);
                    assert(m_frame == nullptr);

                    m_system = std::make_unique<xr::System>();

                    if (!m_system->IsInitialized())
                    {
                        while (!m_system->TryInitialize())
                        {
                            // do nothing
                        }
                    }

                    return xr::System::Session::CreateAsync(*m_system, bgfx::getInternalData()->context, m_windowProvider)
                        .then(m_graphicsImpl.AfterRenderScheduler(), arcana::cancellation::none(), [this, thisRef{shared_from_this()}](std::shared_ptr<xr::System::Session> session) {
                            m_session = std::move(session);

                            if (m_sessionStateChangedCallback)
                            {
                                m_sessionStateChangedCallback(true);
                            }
                        });
                });
        }

        arcana::task<void, std::exception_ptr> NativeXr::Impl::EndSessionAsync()
        {
            m_cancellationSource.cancel();

            m_frameBufferToJsTextureMap.clear();
            m_textureToFrameBufferMap.clear();
            m_activeTextures.clear();

            return m_frameTask.then(m_graphicsImpl.AfterRenderScheduler(), arcana::cancellation::none(), [this, thisRef{shared_from_this()}](const arcana::expected<void, std::exception_ptr>&) {
                assert(m_system != nullptr);
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
                m_system.reset();

                if (m_sessionStateChangedCallback)
                {
                    m_sessionStateChangedCallback(false);
                }
            });
        }

        void NativeXr::Impl::ScheduleFrame(std::function<void(const xr::System::Session::Frame&)>&& callback)
        {
            if (m_frameScheduled)
            {
                return;
            }

            m_frameScheduled = true;

            // REVIEW: This should technically be before the check for m_frameScheduled, but for some
            // reason requestAnimationFrame is being called twice when starting XR.
            m_scheduleFrameCallbacks.emplace_back(callback);

            m_frameTask = arcana::make_task(m_graphicsImpl.BeforeRenderScheduler(), m_cancellationSource, [this, thisRef{shared_from_this()}] {
                BeginFrame();

                return arcana::make_task(m_runtimeScheduler, m_cancellationSource, [this, updateToken{m_graphicsImpl.GetUpdateToken()}, thisRef{shared_from_this()}]() {
                    m_frameScheduled = false;

                    BeginUpdate();

                    auto callbacks{std::move(m_scheduleFrameCallbacks)};
                    for (auto& callback : callbacks)
                    {
                        callback(*m_frame);
                    }

                    EndUpdate();
                }).then(arcana::inline_scheduler, m_cancellationSource, [this, thisRef{shared_from_this()}](const arcana::expected<void, std::exception_ptr>& result) {
                    if (!m_cancellationSource.cancelled() && result.has_error())
                    {
                        Napi::Error::New(m_env, result.error()).ThrowAsJavaScriptException();
                    }
                }).then(m_graphicsImpl.AfterRenderScheduler(), arcana::cancellation::none(), [this, thisRef{shared_from_this()}](const arcana::expected<void, std::exception_ptr>&) {
                    EndFrame();
                });
            });
        }

        void NativeXr::Impl::BeginFrame()
        {
            assert(m_session != nullptr);
            assert(m_frame == nullptr);

            bool shouldEndSession{};
            bool shouldRestartSession{};
            m_frame = m_session->GetNextFrame(shouldEndSession, shouldRestartSession, [this](void* texturePointer) {
                m_runtimeScheduler([this, texturePointer]() {
                    auto itTextureToFrameBuffer{m_textureToFrameBufferMap.find(texturePointer)};
                    if (itTextureToFrameBuffer != m_textureToFrameBufferMap.end())
                    {
                        auto itFrameBufferToJsTexture{m_frameBufferToJsTextureMap.find(itTextureToFrameBuffer->second)};
                        if (itFrameBufferToJsTexture != m_frameBufferToJsTextureMap.end())
                        {
                            m_destroyRenderTexture.Call({itFrameBufferToJsTexture->second.Value()});
                            m_frameBufferToJsTextureMap.erase(itFrameBufferToJsTexture);
                        }

                        m_textureToFrameBufferMap.erase(itTextureToFrameBuffer);
                    }
                });
            });

            // Ending a session outside of calls to EndSessionAsync() is currently not supported.
            assert(!shouldEndSession);
            assert(m_frame != nullptr);
        }

        void NativeXr::Impl::BeginUpdate()
        {
            m_activeTextures.reserve(m_frame->Views.size());
            for (const auto& view : m_frame->Views)
            {
                m_activeTextures.push_back(view.ColorTexturePointer);

                auto it{m_textureToFrameBufferMap.find(view.ColorTexturePointer)};
                if (it == m_textureToFrameBufferMap.end() || it->second->Width() != view.ColorTextureSize.Width || it->second->Height() != view.ColorTextureSize.Height)
                {
                    // If a texture width or height is 0, bgfx will assert (can't create 0 sized texture). Asserting here instead of deeper in bgfx rendering.
                    assert(view.ColorTextureSize.Width != 0);
                    assert(view.ColorTextureSize.Height != 0);
                    assert(view.ColorTextureSize.Width == view.DepthTextureSize.Width);
                    assert(view.ColorTextureSize.Height == view.DepthTextureSize.Height);

                    // Create textures with the desired size. It will be freed and replaced with overrideInternal call
                    // This is mandatory as overrideInternal do not update texture size.
                    // And size is used for determining viewport when rendering to texture.
                    auto colorTextureFormat = XrTextureFormatToBgfxFormat(view.ColorTextureFormat);
                    auto colorTexture = bgfx::createTexture2D(static_cast<uint16_t>(view.ColorTextureSize.Width), static_cast<uint16_t>(view.ColorTextureSize.Height), false, 1, colorTextureFormat, BGFX_TEXTURE_RT);

                    auto depthTextureFormat = XrTextureFormatToBgfxFormat(view.DepthTextureFormat);
                    auto depthTexture = bgfx::createTexture2D(static_cast<uint16_t>(view.DepthTextureSize.Width), static_cast<uint16_t>(view.DepthTextureSize.Height), false, 1, depthTextureFormat, BGFX_TEXTURE_RT);

                    arcana::make_task(m_graphicsImpl.AfterRenderScheduler(), arcana::cancellation::none(), [colorTexture, depthTexture, &view]() {
                        bgfx::overrideInternal(colorTexture, reinterpret_cast<uintptr_t>(view.ColorTexturePointer));
                        bgfx::overrideInternal(depthTexture, reinterpret_cast<uintptr_t>(view.DepthTexturePointer));
                    }).then(m_runtimeScheduler, m_cancellationSource, [this, thisRef{shared_from_this()}, colorTexture, depthTexture, &view]() {
                        std::array<bgfx::Attachment, 2> attachments{};
                        attachments[0].init(colorTexture);
                        attachments[1].init(depthTexture);
                        auto frameBufferHandle = bgfx::createFrameBuffer(static_cast<uint8_t>(attachments.size()), attachments.data(), false);

                        auto& frameBuffer{m_graphicsImpl.AddFrameBuffer(frameBufferHandle,
                            static_cast<uint16_t>(view.ColorTextureSize.Width),
                            static_cast<uint16_t>(view.ColorTextureSize.Height),
                            true)};

                        // WebXR, at least in its current implementation, specifies an implicit default clear to black.
                        // https://immersive-web.github.io/webxr/#xrwebgllayer-interface
                        frameBuffer.Clear(m_graphicsImpl.GetUpdateToken().GetEncoder(), BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH | BGFX_CLEAR_STENCIL, 0, 1.0f, 0);

                        m_textureToFrameBufferMap[view.ColorTexturePointer] = &frameBuffer;

                        auto jsWidth{Napi::Value::From(m_env, view.ColorTextureSize.Width)};
                        auto jsHeight{Napi::Value::From(m_env, view.ColorTextureSize.Height)};
                        auto jsFrameBuffer{Napi::External<FrameBuffer>::New(m_env, &frameBuffer)};
                        m_frameBufferToJsTextureMap[&frameBuffer] = Napi::Persistent(m_createRenderTexture.Call({jsWidth, jsHeight, jsFrameBuffer}).As<Napi::Object>());
                    }).then(arcana::inline_scheduler, m_cancellationSource, [env{m_env}](const arcana::expected<void, std::exception_ptr>& result) {
                        if (result.has_error())
                        {
                            Napi::Error::New(env, result.error()).ThrowAsJavaScriptException();
                        }
                    });
                }
            }
        }

        void NativeXr::Impl::EndUpdate()
        {
            m_activeTextures.clear();
        }

        void NativeXr::Impl::EndFrame()
        {
            assert(m_session != nullptr);
            assert(m_frame != nullptr);

            m_frame.reset();
        }
    }

    namespace
    {
        class XRFrame;

        struct XRSessionType
        {
            static constexpr auto IMMERSIVE_VR{"immersive-vr"};
            static constexpr auto IMMERSIVE_AR{"immersive-ar"};
            static constexpr auto INLINE{"inline"};
        };

        struct XRReferenceSpaceType
        {
            static constexpr auto VIEWER{"viewer"};
            // static constexpr auto LOCAL{"local"};
            // static constexpr auto LOCAL_FLOOR{"local-floor"};
            // static constexpr auto BOUNDED_FLOOR{"bounded-floor"};
            static constexpr auto UNBOUNDED{"unbounded"};
        };

        struct XRHitTestTrackableType
        {
            static constexpr auto POINT{"point"};
            static constexpr auto PLANE{"plane"};
            static constexpr auto MESH{"mesh"};
        };

        struct XREye
        {
            static constexpr auto NONE{"none"};
            static constexpr auto LEFT{"left"};
            static constexpr auto RIGHT{"right"};

            static auto IndexToEye(size_t idx)
            {
                switch (idx)
                {
                    case 0:
                        return LEFT;
                    case 1:
                        return RIGHT;
                    case 2:
                        return NONE;
                    default:
                        throw std::runtime_error{"Unsupported index"};
                }
            }

            static auto EyeToIndex(const std::string& eye)
            {
                if (eye == LEFT)
                {
                    return 0;
                }
                else if (eye == RIGHT)
                {
                    return 1;
                }
                else if (eye == NONE)
                {
                    return 2;
                }
                else
                {
                    throw std::runtime_error{"Unsupported eye"};
                }
            }
        };

        class PointerEvent : public Napi::ObjectWrap<PointerEvent>
        {
            static constexpr auto JS_CLASS_NAME = "PointerEvent";

        public:
            static void Initialize(Napi::Env& env)
            {
                Napi::Function func = DefineClass(
                    env,
                    JS_CLASS_NAME,
                    {});

                env.Global().Set(JS_CLASS_NAME, func);
            }

            static Napi::Object New(const Napi::CallbackInfo& info)
            {
                return info.Env().Global().Get(JS_CLASS_NAME).As<Napi::Function>().New({});
            }

            PointerEvent(const Napi::CallbackInfo& info)
                : Napi::ObjectWrap<PointerEvent>{info}
            {
            }
        };

        class XRWebGLLayer : public Napi::ObjectWrap<XRWebGLLayer>
        {
            static constexpr auto JS_CLASS_NAME = "XRWebGLLayer";

        public:
            static void Initialize(Napi::Env env)
            {
                Napi::HandleScope scope{env};

                Napi::Function func = DefineClass(
                    env,
                    JS_CLASS_NAME,
                    {
                        InstanceMethod("getViewport", &XRWebGLLayer::GetViewport),
                    });

                env.Global().Set(JS_CLASS_NAME, func);
            }

            static Napi::Object New(const Napi::CallbackInfo& info)
            {
                return info.Env().Global().Get(JS_CLASS_NAME).As<Napi::Function>().New({});
            }

            XRWebGLLayer(const Napi::CallbackInfo& info)
                : Napi::ObjectWrap<XRWebGLLayer>{info}
            {
            }

        private:
            Napi::Value GetViewport(const Napi::CallbackInfo& info)
            {
                return info.This().As<Napi::Object>().Get("viewport");
            }
        };

        class XRRigidTransform : public Napi::ObjectWrap<XRRigidTransform>
        {
            static constexpr auto JS_CLASS_NAME = "XRRigidTransform";
            // static constexpr size_t VECTOR_SIZE = 4;
            static constexpr size_t MATRIX_SIZE = 16;

        public:
            static void Initialize(Napi::Env env)
            {
                Napi::HandleScope scope{env};

                Napi::Function func = DefineClass(
                    env,
                    JS_CLASS_NAME,
                    {
                        InstanceAccessor("position", &XRRigidTransform::Position, nullptr),
                        InstanceAccessor("orientation", &XRRigidTransform::Orientation, nullptr),
                        InstanceAccessor("matrix", &XRRigidTransform::Matrix, nullptr),
                    });

                env.Global().Set(JS_CLASS_NAME, func);
            }

            static Napi::Object New(const Napi::CallbackInfo& info)
            {
                return info.Env().Global().Get(JS_CLASS_NAME).As<Napi::Function>().New({});
            }

            XRRigidTransform(const Napi::CallbackInfo& info)
                : Napi::ObjectWrap<XRRigidTransform>{info}
                , m_matrix{Napi::Persistent(Napi::Float32Array::New(info.Env(), MATRIX_SIZE))}
            {
                if (info.Length() == 2)
                {
                    m_position = Napi::Persistent(info[0].As<Napi::Object>());
                    m_orientation = Napi::Persistent(info[1].As<Napi::Object>());
                }
                else
                {
                    m_position = Napi::Persistent(Napi::Object::New(info.Env()));
                    m_orientation = Napi::Persistent(Napi::Object::New(info.Env()));
                }
            }

            void Update(XRRigidTransform* transform)
            {
                Update({transform->GetNativePose()}, false);
            }

            void Update(const xr::System::Session::Frame::Space& space, bool isViewSpace)
            {
                auto position = m_position.Value();
                position.Set("x", space.Pose.Position.X);
                position.Set("y", space.Pose.Position.Y);
                position.Set("z", space.Pose.Position.Z);
                position.Set("w", 1.f);

                auto orientation = m_orientation.Value();
                orientation.Set("x", space.Pose.Orientation.X);
                orientation.Set("y", space.Pose.Orientation.Y);
                orientation.Set("z", space.Pose.Orientation.Z);
                orientation.Set("w", space.Pose.Orientation.W);

                std::memcpy(m_matrix.Value().Data(), CreateTransformMatrix(space, isViewSpace).data(), m_matrix.Value().ByteLength());
            }

            void Update(const xr::Pose& pose)
            {
                xr::System::Session::Frame::Space space{{pose}};
                Update(space, true);
            }

            xr::Pose GetNativePose()
            {
                auto position = m_position.Value();
                auto orientation = m_orientation.Value();
                return {
                    {position.Get("x").ToNumber().FloatValue(), position.Get("y").ToNumber().FloatValue(), position.Get("z").ToNumber().FloatValue()},
                    {orientation.Get("x").ToNumber().FloatValue(), orientation.Get("y").ToNumber().FloatValue(), orientation.Get("z").ToNumber().FloatValue(), orientation.Get("w").ToNumber().FloatValue()}};
            }

        private:
            Napi::ObjectReference m_position{};
            Napi::ObjectReference m_orientation{};
            Napi::Reference<Napi::Float32Array> m_matrix{};

            Napi::Value Position(const Napi::CallbackInfo&)
            {
                return m_position.Value();
            }

            Napi::Value Orientation(const Napi::CallbackInfo&)
            {
                return m_orientation.Value();
            }

            Napi::Value Matrix(const Napi::CallbackInfo&)
            {
                return m_matrix.Value();
            }
        };

        class XRView : public Napi::ObjectWrap<XRView>
        {
            static constexpr auto JS_CLASS_NAME = "XRView";
            static constexpr size_t MATRIX_SIZE = 16;

        public:
            static void Initialize(Napi::Env env)
            {
                Napi::HandleScope scope{env};

                Napi::Function func = DefineClass(
                    env,
                    JS_CLASS_NAME,
                    {
                        InstanceAccessor("eye", &XRView::GetEye, nullptr),
                        InstanceAccessor("projectionMatrix", &XRView::GetProjectionMatrix, nullptr),
                        InstanceAccessor("transform", &XRView::GetTransform, nullptr),
                        InstanceAccessor("isFirstPersonObserver", &XRView::IsFirstPersonObserver, nullptr)
                    });

                env.Global().Set(JS_CLASS_NAME, func);
            }

            static Napi::Object New(const Napi::CallbackInfo& info)
            {
                return info.Env().Global().Get(JS_CLASS_NAME).As<Napi::Function>().New({});
            }

            XRView(const Napi::CallbackInfo& info)
                : Napi::ObjectWrap<XRView>{info}
                , m_eyeIdx{0}
                , m_eye{XREye::IndexToEye(m_eyeIdx)}
                , m_projectionMatrix{Napi::Persistent(Napi::Float32Array::New(info.Env(), MATRIX_SIZE))}
                , m_rigidTransform{Napi::Persistent(XRRigidTransform::New(info))}
                , m_isFirstPersonObserver{ false }
            {
            }

            void Update(size_t eyeIdx, gsl::span<const float, 16> projectionMatrix, const xr::System::Session::Frame::Space& space, bool isFirstPersonObserver)
            {
                if (eyeIdx != m_eyeIdx)
                {
                    m_eyeIdx = eyeIdx;
                    m_eye = XREye::IndexToEye(m_eyeIdx);
                }

                std::memcpy(m_projectionMatrix.Value().Data(), projectionMatrix.data(), m_projectionMatrix.Value().ByteLength());

                XRRigidTransform::Unwrap(m_rigidTransform.Value())->Update(space, false);

                m_isFirstPersonObserver = isFirstPersonObserver;
            }

        private:
            size_t m_eyeIdx{};
            gsl::czstring<> m_eye{};
            Napi::Reference<Napi::Float32Array> m_projectionMatrix{};
            Napi::ObjectReference m_rigidTransform{};
            bool m_isFirstPersonObserver{};

            Napi::Value GetEye(const Napi::CallbackInfo& info)
            {
                return Napi::String::From(info.Env(), m_eye);
            }

            Napi::Value GetProjectionMatrix(const Napi::CallbackInfo&)
            {
                return m_projectionMatrix.Value();
            }

            Napi::Value GetTransform(const Napi::CallbackInfo&)
            {
                return m_rigidTransform.Value();
            }

            Napi::Value IsFirstPersonObserver(const Napi::CallbackInfo& info)
            {
                return Napi::Boolean::From(info.Env(), m_isFirstPersonObserver);
            }
        };

        class XRViewerPose : public Napi::ObjectWrap<XRViewerPose>
        {
            static constexpr auto JS_CLASS_NAME = "XRViewerPose";

        public:
            static void Initialize(Napi::Env env)
            {
                Napi::HandleScope scope{env};

                Napi::Function func = DefineClass(
                    env,
                    JS_CLASS_NAME,
                    {
                        InstanceAccessor("transform", &XRViewerPose::GetTransform, nullptr),
                        InstanceAccessor("views", &XRViewerPose::GetViews, nullptr),
                        InstanceAccessor("emulatedPosition", &XRViewerPose::GetEmulatedPosition, nullptr),
                    });

                env.Global().Set(JS_CLASS_NAME, func);
            }

            static Napi::Object New(const Napi::CallbackInfo& info)
            {
                return info.Env().Global().Get(JS_CLASS_NAME).As<Napi::Function>().New({});
            }

            XRViewerPose(const Napi::CallbackInfo& info)
                : Napi::ObjectWrap<XRViewerPose>{info}
                , m_jsTransform{Napi::Persistent(XRRigidTransform::New(info))}
                , m_jsViews{Napi::Persistent(Napi::Array::New(info.Env(), 0))}
                , m_transform{*XRRigidTransform::Unwrap(m_jsTransform.Value())}
                , m_isEmulatedPosition{true}
            {
            }

            void Update(const Napi::CallbackInfo& info, const xr::System::Session::Frame& frame)
            {
                // Update the transform, for now assume that the pose of the first view if it exists represents the viewer transform.
                // This is correct for devices with a single view, but is likely incorrect for devices with multiple views (eg. VR/AR headsets with binocular views).
                if (frame.Views.size() > 0)
                {
                    m_transform.Update(frame.Views[0].Space, true);
                }

                // Update the views array if necessary.
                const auto oldSize = static_cast<uint32_t>(m_views.size());
                const auto newSize = static_cast<uint32_t>(frame.Views.size());
                if (oldSize != newSize)
                {
                    auto newViews = Napi::Array::New(m_jsViews.Env(), newSize);
                    m_views.resize(newSize);

                    for (uint32_t idx = 0; idx < newSize; ++idx)
                    {
                        if (idx < oldSize)
                        {
                            newViews.Set(idx, m_jsViews.Value().Get(idx));
                        }
                        else
                        {
                            newViews.Set(idx, XRView::New(info));
                        }

                        m_views[idx] = XRView::Unwrap(newViews.Get(idx).As<Napi::Object>());
                    }

                    m_jsViews = Napi::Persistent(newViews);
                }

                // Update the individual views.
                for (uint32_t idx = 0; idx < static_cast<uint32_t>(frame.Views.size()); ++idx)
                {
                    const auto& view = frame.Views[idx];
                    m_views[idx]->Update(idx, view.ProjectionMatrix, view.Space, view.IsFirstPersonObserver);
                }

                // Check the frame to see if it has valid tracking, if it does not then the position should
                // be flagged as being emulated.
                m_isEmulatedPosition = !frame.IsTracking;
            }

        private:
            Napi::ObjectReference m_jsTransform{};
            Napi::Reference<Napi::Array> m_jsViews{};

            XRRigidTransform& m_transform;
            std::vector<XRView*> m_views{};
            bool m_isEmulatedPosition;

            Napi::Value GetTransform(const Napi::CallbackInfo& /*info*/)
            {
                return m_jsTransform.Value();
            }

            Napi::Value GetViews(const Napi::CallbackInfo& /*info*/)
            {
                return m_jsViews.Value();
            }

            Napi::Value GetEmulatedPosition(const Napi::CallbackInfo& info)
            {
                return Napi::Boolean::New(info.Env(), m_isEmulatedPosition);
            }
        };

        // Implementation of vanilla XRPose: https://immersive-web.github.io/webxr/#xrpose-interface
        class XRPose : public Napi::ObjectWrap<XRPose>
        {
            static constexpr auto JS_CLASS_NAME = "XRPose";

        public:
            static void Initialize(Napi::Env env)
            {
                Napi::HandleScope scope{env};

                Napi::Function func = DefineClass(
                    env,
                    JS_CLASS_NAME,
                    {
                        InstanceAccessor("transform", &XRPose::GetTransform, nullptr),
                    });

                env.Global().Set(JS_CLASS_NAME, func);
            }

            static Napi::Object New(const Napi::CallbackInfo& info)
            {
                return info.Env().Global().Get(JS_CLASS_NAME).As<Napi::Function>().New({});
            }

            XRPose(const Napi::CallbackInfo& info)
                : Napi::ObjectWrap<XRPose>{info}
                , m_jsTransform{Napi::Persistent(XRRigidTransform::New(info))}
                , m_transform{*XRRigidTransform::Unwrap(m_jsTransform.Value())}
            {
            }

            void Update(const Napi::CallbackInfo& /*info*/, const xr::Pose& pose)
            {
                // Update the transform.
                m_transform.Update(pose);
            }

            void Update(XRRigidTransform* transform)
            {
                // Update the transform.
                m_transform.Update(transform);
            }

        private:
            Napi::ObjectReference m_jsTransform{};
            XRRigidTransform& m_transform;

            Napi::Value GetTransform(const Napi::CallbackInfo& /*info*/)
            {
                return m_jsTransform.Value();
            }
        };

        // Implementation of XRRay: https://immersive-web.github.io/hit-test/#xrray-interface
        class XRRay : public Napi::ObjectWrap<XRRay>
        {
            static constexpr auto JS_CLASS_NAME = "XRRay";
            static constexpr size_t MATRIX_SIZE = 16;

        public:
            static void Initialize(Napi::Env env)
            {
                Napi::HandleScope scope{env};

                Napi::Function func = DefineClass(
                    env,
                    JS_CLASS_NAME,
                    {
                        InstanceAccessor("origin", &XRRay::Origin, nullptr),
                        InstanceAccessor("direction", &XRRay::Direction, nullptr),
                        InstanceAccessor("matrix", &XRRay::Matrix, nullptr),
                    });

                env.Global().Set(JS_CLASS_NAME, func);
            }

            static Napi::Object New(const Napi::CallbackInfo& info)
            {
                return info.Env().Global().Get(JS_CLASS_NAME).As<Napi::Function>().New({info[0]});
            }

            XRRay(const Napi::CallbackInfo& info)
                : Napi::ObjectWrap<XRRay>{info}
            {
                bool originSet = false;
                bool directionSet = false;
                bool matrixSet = false;
                if (info[0].IsObject())
                {
                    auto argumentObject = info[0].As<Napi::Object>();
                    auto originValue = argumentObject.Get("origin");
                    if (originValue.IsObject())
                    {
                        originSet = true;
                        m_origin = Napi::Persistent(originValue.As<Napi::Object>());
                    }

                    auto directionValue = argumentObject.Get("direction");
                    if (directionValue.IsObject())
                    {
                        directionSet = true;
                        m_direction = Napi::Persistent(directionValue.As<Napi::Object>());
                    }

                    auto matrixValue = argumentObject.Get("matrix");
                    if (matrixValue.IsArray())
                    {
                        matrixSet = true;
                        m_matrix = Napi::Persistent(matrixValue.As<Napi::Float32Array>());
                    }
                }

                if (!originSet)
                {
                    m_origin = Napi::Persistent(Napi::Object::New(info.Env()));
                }

                if (!directionSet)
                {
                    m_direction = Napi::Persistent(Napi::Object::New(info.Env()));
                }

                if (!matrixSet)
                {
                    m_matrix = Napi::Persistent(Napi::Float32Array::New(info.Env(), MATRIX_SIZE));
                }
            }

            xr::Ray GetNativeRay()
            {
                xr::Ray nativeRay{{0, 0, 0}, {0, 0, -1}};
                auto originObject = m_origin.Value();
                nativeRay.Origin.X = originObject.Get("x").ToNumber().FloatValue();
                nativeRay.Origin.Y = originObject.Get("y").ToNumber().FloatValue();
                nativeRay.Origin.Z = originObject.Get("z").ToNumber().FloatValue();

                auto directionObject = m_direction.Value();
                nativeRay.Direction.X = directionObject.Get("x").ToNumber().FloatValue();
                nativeRay.Direction.Y = directionObject.Get("y").ToNumber().FloatValue();
                nativeRay.Direction.Z = directionObject.Get("z").ToNumber().FloatValue();

                return nativeRay;
            }

        private:
            Napi::ObjectReference m_origin{};
            Napi::ObjectReference m_direction{};
            Napi::Reference<Napi::Float32Array> m_matrix{};

            Napi::Value Origin(const Napi::CallbackInfo&)
            {
                return m_origin.Value();
            }

            Napi::Value Direction(const Napi::CallbackInfo&)
            {
                return m_direction.Value();
            }

            Napi::Value Matrix(const Napi::CallbackInfo&)
            {
                return m_matrix.Value();
            }
        };

        // Implementation of the XRReferenceSpace interface: https://immersive-web.github.io/webxr/#xrreferencespace-interface
        class XRReferenceSpace : public Napi::ObjectWrap<XRReferenceSpace>
        {
            static constexpr auto JS_CLASS_NAME = "XRReferenceSpace";

        public:
            static void Initialize(Napi::Env env)
            {
                Napi::HandleScope scope{env};

                Napi::Function func = DefineClass(
                    env,
                    JS_CLASS_NAME,
                    {
                        InstanceMethod("getOffsetReferenceSpace", &XRReferenceSpace::GetOffsetReferenceSpace),
                    });

                env.Global().Set(JS_CLASS_NAME, func);
            }

            static Napi::Object New(const Napi::CallbackInfo& info)
            {
                return info.Env().Global().Get(JS_CLASS_NAME).As<Napi::Function>().New({info[0]});
            }

            static Napi::Object New(const Napi::Env env, Napi::Object napiTransform)
            {
                return env.Global().Get(JS_CLASS_NAME).As<Napi::Function>().New({napiTransform});
            }

            XRReferenceSpace(const Napi::CallbackInfo& info)
                : Napi::ObjectWrap<XRReferenceSpace>{info}
            {
                if (info.Length() > 0)
                {
                    if (info[0].IsString())
                    {
                        // TODO: Actually support the different types of reference spaces.
                        const auto referenceSpaceType = info[0].As<Napi::String>().Utf8Value();
                        assert(referenceSpaceType == XRReferenceSpaceType::UNBOUNDED ||
                            referenceSpaceType == XRReferenceSpaceType::VIEWER);
                        (void)XRReferenceSpaceType::UNBOUNDED;
                        (void)XRReferenceSpaceType::VIEWER;
                    }
                    else
                    {
                        m_jsTransform = Napi::Persistent(info[0].As<Napi::Object>());
                    }
                }
            }

            void SetTransform(Napi::Object transformObj)
            {
                m_jsTransform = Napi::Persistent(transformObj);
            }

            XRRigidTransform* GetTransform()
            {
                return XRRigidTransform::Unwrap(m_jsTransform.Value());
            }

        private:
            Napi::Value GetOffsetReferenceSpace(const Napi::CallbackInfo& info)
            {
                // TODO: Handle XRBoundedReferenceSpace case
                // https://immersive-web.github.io/webxr/#dom-xrreferencespace-getoffsetreferencespace

                return XRReferenceSpace::New(info);
            }

            Napi::ObjectReference m_jsTransform{};
        };

        // Implementation of the XRAnchor interface: https://immersive-web.github.io/anchors/#xr-anchor
        class XRAnchor : public Napi::ObjectWrap<XRAnchor>
        {
            static constexpr auto JS_CLASS_NAME = "XRAnchor";

        public:
            static void Initialize(Napi::Env env)
            {
                Napi::HandleScope scope{env};

                Napi::Function func = DefineClass(
                    env,
                    JS_CLASS_NAME,
                    {
                        InstanceAccessor("anchorSpace", &XRAnchor::GetAnchorSpace, nullptr),
                        InstanceMethod("delete", &XRAnchor::Delete),
                    });

                env.Global().Set(JS_CLASS_NAME, func);
            }

            static Napi::Object New(const Napi::CallbackInfo& info)
            {
                return info.Env().Global().Get(JS_CLASS_NAME).As<Napi::Function>().New({});
            }

            XRAnchor(const Napi::CallbackInfo& info)
                : Napi::ObjectWrap<XRAnchor>{info}
            {
            }

            xr::Anchor& GetNativeAnchor()
            {
                return m_nativeAnchor;
            }

            void SetAnchor(xr::Anchor& nativeAnchor)
            {
                m_nativeAnchor = nativeAnchor;
            }

        private:
            Napi::Value GetAnchorSpace(const Napi::CallbackInfo& info)
            {
                Napi::Object napiTransform = XRRigidTransform::New(info);
                XRRigidTransform* rigidTransform = XRRigidTransform::Unwrap(napiTransform);
                rigidTransform->Update(m_nativeAnchor.Pose);

                Napi::Object napiSpace = XRReferenceSpace::New(info.Env(), napiTransform);
                return std::move(napiSpace);
            }

            // Marks the anchor as no longer valid, and should be deleted on the next pass.
            void Delete(const Napi::CallbackInfo&)
            {
                m_nativeAnchor.IsValid = false;
            }

            // The native anchor which holds the current position of the anchor, and the native ref to the anchor.
            xr::Anchor m_nativeAnchor{};
        };

        // Implementation of the XRHitTestSource interface: https://immersive-web.github.io/hit-test/#hit-test-source-interface
        class XRHitTestSource : public Napi::ObjectWrap<XRHitTestSource>
        {
            static constexpr auto JS_CLASS_NAME = "XRHitTestSource";

        public:
            static void Initialize(Napi::Env env)
            {
                Napi::HandleScope scope{env};

                Napi::Function func = DefineClass(
                    env,
                    JS_CLASS_NAME,
                    {
                        InstanceMethod("cancel", &XRHitTestSource::Cancel),
                    });

                env.Global().Set(JS_CLASS_NAME, func);
            }

            static Napi::Object New(const Napi::CallbackInfo& info)
            {
                return info.Env().Global().Get(JS_CLASS_NAME).As<Napi::Function>().New({info[0]});
            }

            XRHitTestSource(const Napi::CallbackInfo& info)
                : Napi::ObjectWrap<XRHitTestSource>{info}
            {
                auto options = info[0].As<Napi::Object>();

                if (options.Has("space"))
                {
                    auto spaceValue = options.Get("space");

                    if (spaceValue.IsObject())
                    {
                        m_space = Napi::Persistent(spaceValue.As<Napi::Object>());
                        hasSpace = true;
                    }
                }

                if (options.Has("offsetRay"))
                {
                    m_offsetRay = Napi::Persistent(options.Get("offsetRay").As<Napi::Object>());
                    hasOffsetRay = true;
                }

                if (options.Has("entityTypes"))
                {
                    const auto entityTypeArray = options.Get("entityTypes").As<Napi::Array>();
                    for (uint32_t i = 0; i < entityTypeArray.Length(); i++)
                    {
                        const auto entityType = entityTypeArray.Get(i).As<Napi::String>().Utf8Value();
                        if (entityType == XRHitTestTrackableType::POINT)
                        {
                            m_entityTypes |= xr::HitTestTrackableType::POINT;
                        }
                        else if (entityType == XRHitTestTrackableType::PLANE)
                        {
                            m_entityTypes |= xr::HitTestTrackableType::PLANE;
                        }
                        else if (entityType == XRHitTestTrackableType::MESH)
                        {
                            m_entityTypes |= xr::HitTestTrackableType::MESH;
                        }
                    }
                }

                // Default to MESH if unspecified.
                if (m_entityTypes == xr::HitTestTrackableType::NONE)
                {
                    m_entityTypes = xr::HitTestTrackableType::MESH;
                }
            }

            XRRay* OffsetRay()
            {
                return hasOffsetRay ? XRRay::Unwrap(m_offsetRay.Value()) : nullptr;
            }

            XRReferenceSpace* Space()
            {
                return hasSpace ? XRReferenceSpace::Unwrap(m_space.Value()) : nullptr;
            }

            xr::HitTestTrackableType GetEntityTypes()
            {
                return m_entityTypes;
            }

        private:
            void Cancel(const Napi::CallbackInfo& /*info*/)
            {
                // no-op we don't keep a persistent list of active XRHitTestSources..
            }

            bool hasSpace = false;
            Napi::ObjectReference m_space;

            bool hasOffsetRay = false;
            Napi::ObjectReference m_offsetRay;

            xr::HitTestTrackableType m_entityTypes{xr::HitTestTrackableType::NONE};
        };

        // Implementation of the XRHitTestResult interface: https://immersive-web.github.io/hit-test/#xr-hit-test-result-interface
        class XRHitTestResult : public Napi::ObjectWrap<XRHitTestResult>
        {
            static constexpr auto JS_CLASS_NAME = "XRHitTestResult";

        public:
            static void Initialize(Napi::Env env)
            {
                Napi::HandleScope scope{env};

                Napi::Function func = DefineClass(
                    env,
                    JS_CLASS_NAME,
                    {
                        InstanceMethod("getPose", &XRHitTestResult::GetPose),
                        InstanceMethod("createAnchor", &XRHitTestResult::CreateAnchor),
                    });

                env.Global().Set(JS_CLASS_NAME, func);
            }

            static Napi::Object New(const Napi::CallbackInfo& info)
            {
                return info.Env().Global().Get(JS_CLASS_NAME).As<Napi::Function>().New({});
            }

            XRHitTestResult(const Napi::CallbackInfo& info)
                : Napi::ObjectWrap<XRHitTestResult>{info}
            {
            }

            // Sets the value of the hit pose and native entity via struct copy.
            void SetHitResult(const xr::HitResult& hitResult)
            {
                m_hitResult = hitResult;
            }

            void SetXRFrame(XRFrame* frame)
            {
                m_frame = frame;
            }

        private:
            // The hit hit result, which contains the pose in default AR Space, as well as the native entity.
            xr::HitResult m_hitResult{};
            XRFrame* m_frame{};

            Napi::Value GetPose(const Napi::CallbackInfo& info)
            {
                // TODO: Once multiple reference views are supported, we need to convert the values into the passed in reference space.
                Napi::Object napiPose = XRPose::New(info);
                XRPose* pose = XRPose::Unwrap(napiPose);
                pose->Update(info, m_hitResult.Pose);

                return std::move(napiPose);
            }

            Napi::Value CreateAnchor(const Napi::CallbackInfo& info);
        };

        // Implementation of the XRPlane interface: https://github.com/immersive-web/real-world-geometry/blob/master/plane-detection-explainer.md
        class XRPlane : public Napi::ObjectWrap<XRPlane>
        {
            static constexpr auto JS_CLASS_NAME = "XRPlane";

        public:
            static void Initialize(Napi::Env env)
            {
                Napi::HandleScope scope{env};

                Napi::Function func = DefineClass(
                    env,
                    JS_CLASS_NAME,
                    {
                        InstanceAccessor("planeSpace", &XRPlane::GetPlaneSpace, nullptr),
                        InstanceAccessor("polygon", &XRPlane::GetPolygon, nullptr),
                        InstanceAccessor("lastChangedTime", &XRPlane::GetLastChangedTime, nullptr),
                        InstanceAccessor("parentSceneObject", &XRPlane::GetParentSceneObject, nullptr)
                    });

                env.Global().Set(JS_CLASS_NAME, func);
            }

            static Napi::Object New(const Napi::Env& env)
            {
                return env.Global().Get(JS_CLASS_NAME).As<Napi::Function>().New({});
            }

            XRPlane(const Napi::CallbackInfo& info)
                : Napi::ObjectWrap<XRPlane>{info}
            {
            }

            void SetLastUpdatedTime(uint32_t timestamp)
            {
                m_lastUpdatedTimestamp = timestamp;
            }

            void SetNativePlaneId(xr::System::Session::Frame::Plane::Identifier planeID)
            {
                m_nativePlaneID = planeID;
            }

            void SetXRFrame(XRFrame* frame)
            {
                m_frame = frame;
            }

        private:
            xr::System::Session::Frame::Plane& GetPlane();

            Napi::Value GetPlaneSpace(const Napi::CallbackInfo& info)
            {
                Napi::Object napiTransform = XRRigidTransform::New(info);
                XRRigidTransform* rigidTransform = XRRigidTransform::Unwrap(napiTransform);
                rigidTransform->Update(GetPlane().Center);

                Napi::Object napiSpace = XRReferenceSpace::New(info.Env(), napiTransform);
                return std::move(napiSpace);
            }

            Napi::Value GetPolygon(const Napi::CallbackInfo& info)
            {
                // Translate the polygon from a native array to a JS array.
                auto& nativePlane = GetPlane();
                auto polygonArray = Napi::Array::New(info.Env(), nativePlane.PolygonSize);
                for (size_t i = 0; i < nativePlane.PolygonSize; i++)
                {
                    auto polygonPoint = Napi::Object::New(info.Env());
                    if (nativePlane.PolygonFormat == xr::PolygonFormat::XZ)
                    {
                        size_t polygonIndex = 2 * i;
                        polygonPoint.Set("x", nativePlane.Polygon[polygonIndex]);
                        polygonPoint.Set("y", 0);
                        polygonPoint.Set("z", nativePlane.Polygon[polygonIndex + 1]);
                    }
                    else
                    {
                        size_t polygonIndex = 3 * i;
                        polygonPoint.Set("x", nativePlane.Polygon[polygonIndex]);
                        polygonPoint.Set("y", nativePlane.Polygon[polygonIndex + 1]);
                        polygonPoint.Set("z", nativePlane.Polygon[polygonIndex + 2]);
                    }

                    polygonArray.Set((int)i, polygonPoint);
                }

                return std::move(polygonArray);
            }

            Napi::Value GetLastChangedTime(const Napi::CallbackInfo& info)
            {
                return Napi::Value::From(info.Env(), m_lastUpdatedTimestamp);
            }

            Napi::Value GetParentSceneObject(const Napi::CallbackInfo& info);

            // The last timestamp when this frame was updated (Pulled in from RequestAnimationFrame).
            uint32_t m_lastUpdatedTimestamp{0};

            // The underlying native plane.
            xr::System::Session::Frame::Plane::Identifier m_nativePlaneID{};

            // Pointer to the XRFrame object.
            XRFrame* m_frame{};
        };

        class XRMesh : public Napi::ObjectWrap<XRMesh>
        {
            static constexpr auto JS_CLASS_NAME = "XRMesh";

        public:
            static void Initialize(Napi::Env env)
            {
                Napi::HandleScope scope{env};

                Napi::Function func = DefineClass(
                    env,
                    JS_CLASS_NAME,
                    {
                        InstanceAccessor("meshSpace", &XRMesh::GetMeshSpace, nullptr),
                        InstanceAccessor("positions", &XRMesh::GetPositions, nullptr),
                        InstanceAccessor("indices", &XRMesh::GetIndices, nullptr),
                        InstanceAccessor("normals", &XRMesh::GetNormals, nullptr),
                        InstanceAccessor("lastChangedTime", &XRMesh::GetLastChangedTime, nullptr),
                        InstanceAccessor("parentSceneObject", &XRMesh::GetParentSceneObject, nullptr)
                    });

                env.Global().Set(JS_CLASS_NAME, func);
            }

            static Napi::Object New(const Napi::Env& env)
            {
                return env.Global().Get(JS_CLASS_NAME).As<Napi::Function>().New({});
            }

            XRMesh(const Napi::CallbackInfo& info)
                : Napi::ObjectWrap<XRMesh>{info}
            {
            }

            ~XRMesh()
            {
                m_jsPositions.Reset();
                m_jsIndices.Reset();
                m_jsNormals.Reset();
            }

            void SetLastUpdatedTime(uint32_t timestamp)
            {
                m_lastUpdatedTimestamp = timestamp;
            }

            void SetNativeMeshId(xr::System::Session::Frame::Mesh::Identifier meshID)
            {
                m_nativeMeshID = meshID;
            }

            void SetXRFrame(XRFrame* frame)
            {
                m_frame = frame;
            }

        private:
            xr::System::Session::Frame::Mesh& GetMesh();

            Napi::Value GetMeshSpace(const Napi::CallbackInfo& info)
            {
                Napi::Object napiTransform = XRRigidTransform::New(info);
                XRRigidTransform* rigidTransform = XRRigidTransform::Unwrap(napiTransform);

                // TODO: update to not use identity pose as needed
                rigidTransform->Update(xr::Pose{});

                Napi::Object napiSpace = XRReferenceSpace::New(info.Env(), napiTransform);
                return std::move(napiSpace);
            }

            Napi::Value GetPositions(const Napi::CallbackInfo& info)
            {
                // NOTE: WebXR reports positions as right-handed coordinates
                const auto& mesh = GetMesh();
                constexpr uint8_t VECTOR3_NUM_FLOATS = 3;
                bool updateValues = false;
                if (!m_jsPositions ||
                    m_numJsPositions != VECTOR3_NUM_FLOATS * mesh.Positions.size())
                {
                    m_numJsPositions = VECTOR3_NUM_FLOATS * mesh.Positions.size();
                    m_jsPositions.Reset();
                    m_jsPositions = Napi::Persistent(Napi::Float32Array::New(info.Env(), m_numJsPositions));
                    updateValues = true;
                }
                else if (m_lastPositionsUpdatedTimestamp != m_lastUpdatedTimestamp)
                {
                    m_lastPositionsUpdatedTimestamp = m_lastUpdatedTimestamp;
                    updateValues = true;
                }

                if (updateValues)
                {
                    for (size_t n = 0; n < mesh.Positions.size(); n++)
                    {
                        m_jsPositions.Value()[VECTOR3_NUM_FLOATS * n] = mesh.Positions.at(n).X;
                        m_jsPositions.Value()[VECTOR3_NUM_FLOATS * n + 1] = mesh.Positions.at(n).Y;
                        m_jsPositions.Value()[VECTOR3_NUM_FLOATS * n + 2] = mesh.Positions.at(n).Z;
                    }
                }

                return m_jsPositions.Value();
            }

            Napi::Value GetIndices(const Napi::CallbackInfo& info)
            {
                // NOTE: WebXR reports indices in a counterclockwise winding order
                assert(sizeof(xr::System::Session::Frame::Mesh::IndexType) == sizeof(uint32_t));
                const auto& mesh = GetMesh();
                if (!m_jsIndices ||
                    m_numJsIndices != mesh.Indices.size())
                {
                    m_numJsIndices = mesh.Indices.size();
                    m_jsIndices.Reset();
                    m_jsIndices = Napi::Persistent(Napi::Uint32Array::New(info.Env(), m_numJsIndices));
                    memcpy(m_jsIndices.Value().Data(), mesh.Indices.data(), mesh.Indices.size() * sizeof(xr::System::Session::Frame::Mesh::IndexType));
                    m_lastIndicesUpdatedTimestamp = m_lastUpdatedTimestamp;
                }
                else if (m_lastIndicesUpdatedTimestamp != m_lastUpdatedTimestamp)
                {
                    memcpy(m_jsIndices.Value().Data(), mesh.Indices.data(), mesh.Indices.size() * sizeof(xr::System::Session::Frame::Mesh::IndexType));
                    m_lastIndicesUpdatedTimestamp = m_lastUpdatedTimestamp;
                }

                return m_jsIndices.Value();
            }

            Napi::Value GetNormals(const Napi::CallbackInfo& info)
            {
                // NOTE: WebXR reports normals as right-handed vectors
                const auto& mesh = GetMesh();
                if (!mesh.HasNormals)
                {
                    return info.Env().Undefined();
                }

                constexpr uint8_t VECTOR3_NUM_FLOATS = 3;
                if (!m_jsNormals ||
                    m_numJsNormals != VECTOR3_NUM_FLOATS * mesh.Normals.size())
                {
                    m_numJsNormals = VECTOR3_NUM_FLOATS * mesh.Normals.size();
                    m_jsNormals.Reset();
                    m_jsNormals = Napi::Persistent(Napi::Float32Array::New(info.Env(), m_numJsNormals));
                    for (size_t n = 0; n < mesh.Normals.size(); n++)
                    {
                        m_jsNormals.Value()[VECTOR3_NUM_FLOATS * n] = mesh.Normals.at(n).X;
                        m_jsNormals.Value()[VECTOR3_NUM_FLOATS * n + 1] = mesh.Normals.at(n).Y;
                        m_jsNormals.Value()[VECTOR3_NUM_FLOATS * n + 2] = mesh.Normals.at(n).Z;
                    }
                    m_lastNormalsUpdatedTimestamp = m_lastUpdatedTimestamp;
                }
                else if (m_lastNormalsUpdatedTimestamp != m_lastUpdatedTimestamp)
                {
                    for (size_t n = 0; n < mesh.Normals.size(); n++)
                    {
                        m_jsNormals.Value()[VECTOR3_NUM_FLOATS * n] = mesh.Normals.at(n).X;
                        m_jsNormals.Value()[VECTOR3_NUM_FLOATS * n + 1] = mesh.Normals.at(n).Y;
                        m_jsNormals.Value()[VECTOR3_NUM_FLOATS * n + 2] = mesh.Normals.at(n).Z;
                    }
                    m_lastNormalsUpdatedTimestamp = m_lastUpdatedTimestamp;
                }

                return m_jsNormals.Value();
            }

            Napi::Value GetLastChangedTime(const Napi::CallbackInfo& info)
            {
                return Napi::Value::From(info.Env(), m_lastUpdatedTimestamp);
            }

            Napi::Value GetParentSceneObject(const Napi::CallbackInfo& info);

            // The last timestamp when this frame was updated (Pulled in from RequestAnimationFrame).
            uint32_t m_lastUpdatedTimestamp{ 0 };
            uint32_t m_lastPositionsUpdatedTimestamp{ 0 };
            uint32_t m_lastIndicesUpdatedTimestamp{ 0 };
            uint32_t m_lastNormalsUpdatedTimestamp{ 0 };

            size_t m_numJsPositions{ 0 };
            Napi::Reference<Napi::Float32Array> m_jsPositions{};
            size_t m_numJsIndices{ 0 };
            Napi::Reference<Napi::Uint32Array> m_jsIndices{};
            size_t m_numJsNormals{ 0 };
            Napi::Reference<Napi::Float32Array> m_jsNormals{};

            // The underlying native mesh.
            xr::System::Session::Frame::Mesh::Identifier m_nativeMeshID{};

            // Pointer to the XRFrame object.
            XRFrame* m_frame{};
        };

        class XRHand : public Napi::ObjectWrap<XRHand>
        {
            static constexpr auto JS_CLASS_NAME = "XRHand";

        public:
            static void Initialize(Napi::Env env)
            {
                Napi::HandleScope scope{env};

                std::vector<XRHand::PropertyDescriptor> initList{};
                initList.reserve(HAND_JOINT_NAMES.size() + 1);

                for (size_t idx = 0; idx < HAND_JOINT_NAMES.size(); idx++)
                {
                    initList.push_back(StaticValue(HAND_JOINT_NAMES[idx], Napi::Value::From(env, idx)));
                }

                initList.push_back(StaticAccessor("size", &XRHand::GetSize, nullptr));

                Napi::Function func = DefineClass(
                    env,
                    JS_CLASS_NAME,
                    initList
                    );

                env.Global().Set(JS_CLASS_NAME, func);
            }

            static Napi::Object New(const Napi::CallbackInfo& info)
            {
                return info.Env().Global().Get(JS_CLASS_NAME).As<Napi::Function>().New({});
            }

            XRHand(const Napi::CallbackInfo& info)
                : Napi::ObjectWrap<XRHand>{info}
            {
            }

        private:
            static Napi::Value GetSize(const Napi::CallbackInfo& info)
            {
                return Napi::Value::From(info.Env(), HAND_JOINT_NAMES.size());
            }
        };

        class XRFrame : public Napi::ObjectWrap<XRFrame>
        {
            static constexpr auto JS_CLASS_NAME = "XRFrame";

        public:
            static void Initialize(Napi::Env env)
            {
                Napi::HandleScope scope{env};

                Napi::Function func = DefineClass(
                    env,
                    JS_CLASS_NAME,
                    {
                        InstanceMethod("getViewerPose", &XRFrame::GetViewerPose),
                        InstanceMethod("getPose", &XRFrame::GetPose),
                        InstanceMethod("getHitTestResults", &XRFrame::GetHitTestResults),
                        InstanceMethod("createAnchor", &XRFrame::CreateAnchor),
                        InstanceMethod("getJointPose", &XRFrame::GetJointPose),
                        InstanceAccessor("trackedAnchors", &XRFrame::GetTrackedAnchors, nullptr),
                        InstanceAccessor("worldInformation", &XRFrame::GetWorldInformation, nullptr),
                        InstanceAccessor("featurePointCloud", &XRFrame::GetFeaturePointCloud, nullptr),
                    });

                env.Global().Set(JS_CLASS_NAME, func);
            }

            static Napi::Object New(const Napi::CallbackInfo& info)
            {
                return info.Env().Global().Get(JS_CLASS_NAME).As<Napi::Function>().New({});
            }

            XRFrame(const Napi::CallbackInfo& info)
                : Napi::ObjectWrap<XRFrame>{info}
                , m_jsXRViewerPose{Napi::Persistent(XRViewerPose::New(info))}
                , m_xrViewerPose{*XRViewerPose::Unwrap(m_jsXRViewerPose.Value())}
                , m_jsTransform{Napi::Persistent(XRRigidTransform::New(info))}
                , m_transform{*XRRigidTransform::Unwrap(m_jsTransform.Value())}
                , m_jsPose{Napi::Persistent(Napi::Object::New(info.Env()))}
                , m_jsJointPose{Napi::Persistent(Napi::Object::New(info.Env()))}
            {
                m_jsPose.Set("transform", m_jsTransform.Value());
                m_jsJointPose.Set("transform", m_jsTransform.Value());
            }

            void Update(const Napi::Env& env, const xr::System::Session::Frame& frame, uint32_t timestamp)
            {
                // Store off a pointer to the frame so that the viewer pose can be updated later. We cannot
                // update the viewer pose here because we don't yet know the desired reference space.
                m_frame = &frame;

                // Update anchor positions.
                UpdateAnchors();

                // Update scene objects.
                UpdateSceneObjects(env);

                // Update planes.
                UpdatePlanes(env, timestamp);

                // Update meshes.
                UpdateMeshes(env, timestamp);
            }

            Napi::Promise CreateNativeAnchor(const Napi::CallbackInfo& info, xr::Pose pose, xr::NativeTrackablePtr nativeTrackable)
            {
                // Create the native anchor.
                auto nativeAnchor = m_frame->CreateAnchor(pose, nativeTrackable);

                // Create the XRAnchor object, and initialize its members.
                auto napiAnchor = Napi::Persistent(XRAnchor::New(info));
                auto* xrAnchor = XRAnchor::Unwrap(napiAnchor.Value());
                xrAnchor->SetAnchor(nativeAnchor);

                // Add the anchor to the list of tracked anchors.
                m_trackedAnchors.emplace_back(std::move(napiAnchor));

                // Resolve the promise with the newly created anchor.
                auto deferred = Napi::Promise::Deferred::New(info.Env());
                deferred.Resolve(m_trackedAnchors.back().Value());
                return deferred.Promise();
            }

            xr::System::Session::Frame::Plane& GetPlaneFromID(xr::System::Session::Frame::Plane::Identifier planeID)
            {
                return m_frame->GetPlaneByID(planeID);
            }

            xr::System::Session::Frame::Mesh& GetMeshFromID(xr::System::Session::Frame::Mesh::Identifier meshID)
            {
                return m_frame->GetMeshByID(meshID);
            }

            Napi::Value GetJSSceneObjectFromID(const Napi::CallbackInfo& info, const xr::System::Session::Frame::SceneObject::Identifier objectID)
            {
                if (objectID == xr::System::Session::Frame::SceneObject::INVALID_ID)
                {
                    return info.Env().Undefined();
                }

                assert(m_sceneObjects.count(objectID) > 0);
                return m_sceneObjects.at(objectID).Value();
            }

        private:
            const xr::System::Session::Frame* m_frame{};
            Napi::ObjectReference m_jsXRViewerPose{};
            XRViewerPose& m_xrViewerPose;
            std::vector<Napi::ObjectReference> m_trackedAnchors{};
            std::unordered_map<xr::System::Session::Frame::Plane::Identifier, Napi::ObjectReference> m_trackedPlanes{};
            std::unordered_map<xr::System::Session::Frame::Mesh::Identifier, Napi::ObjectReference> m_trackedMeshes{};
            Napi::ObjectReference m_meshSet{};
            std::unordered_map<xr::System::Session::Frame::SceneObject::Identifier, Napi::ObjectReference> m_sceneObjects{};

            Napi::ObjectReference m_jsTransform{};
            XRRigidTransform& m_transform;
            Napi::ObjectReference m_jsPose{};
            Napi::ObjectReference m_jsJointPose{};

            bool m_hasBegunTracking{false};

            Napi::Value GetViewerPose(const Napi::CallbackInfo& info)
            {
                // To match the WebXR implementation we should return undefined here until we have gotten
                // initial tracking. After that point we can just continue returning the position
                // as it will be marked as estimated if tracking is lost.
                if (!m_hasBegunTracking && !m_frame->IsTracking)
                {
                    return info.Env().Undefined();
                }
                else
                {
                    // We've received initial tracking, update the flag
                    m_hasBegunTracking = true;
                }

                // TODO: Support reference spaces.
                // auto& space = *XRReferenceSpace::Unwrap(info[0].As<Napi::Object>());

                // Updating the reference space is currently not supported. Until it is, we assume the
                // reference space is unmoving at identity (which is usually true).
                m_xrViewerPose.Update(info, *m_frame);

                return m_jsXRViewerPose.Value();
            }

            Napi::Value GetPose(const Napi::CallbackInfo& info)
            {
                if (info[0].IsExternal())
                {
                    const auto& space = *info[0].As<Napi::External<xr::System::Session::Frame::Space>>().Data();
                    m_transform.Update(space, false);
                    return m_jsPose.Value();
                }
                else
                {
                    auto* xrSpace = XRReferenceSpace::Unwrap(info[0].As<Napi::Object>());
                    assert(xrSpace != nullptr);
                    Napi::Object napiPose = XRPose::New(info);
                    XRPose* pose = XRPose::Unwrap(napiPose);
                    pose->Update(xrSpace->GetTransform());
                    return std::move(napiPose);
                }
            }

            Napi::Value GetJointPose(const Napi::CallbackInfo& info)
            {
                assert(info[0].IsExternal());

                const auto& jointSpace = *info[0].As<Napi::External<xr::System::Session::Frame::JointSpace>>().Data();

                if (jointSpace.PoseTracked)
                {
                    m_transform.Update(jointSpace, false);
                    m_jsJointPose.Set("radius", jointSpace.PoseRadius);
                    return m_jsJointPose.Value();
                }
                else
                {
                    return info.Env().Undefined();
                }
            }

            Napi::Value GetHitTestResults(const Napi::CallbackInfo& info)
            {
                XRHitTestSource* hitTestSource = XRHitTestSource::Unwrap(info[0].As<Napi::Object>());
                XRRay* offsetRay = hitTestSource->OffsetRay();
                xr::Ray nativeRay{};
                if (offsetRay != nullptr)
                {
                    nativeRay = offsetRay->GetNativeRay();
                }
                else
                {
                    nativeRay = {{0, 0, 0}, {0, 0, -1}};
                }

                // Get the native results
                std::vector<xr::HitResult> nativeHitResults{};
                m_frame->GetHitTestResults(nativeHitResults, nativeRay, hitTestSource->GetEntityTypes());

                // Translate those results into a napi array.
                auto results = Napi::Array::New(info.Env(), nativeHitResults.size());
                uint32_t i{0};
                for (std::vector<xr::HitResult>::iterator it = nativeHitResults.begin(); it != nativeHitResults.end(); ++it)
                {
                    Napi::Object currentResult = XRHitTestResult::New(info);
                    XRHitTestResult* xrResult = XRHitTestResult::Unwrap(currentResult);
                    xrResult->SetHitResult(*it);
                    xrResult->SetXRFrame(this);

                    results[i++] = currentResult;
                }

                return std::move(results);
            }

            Napi::Value CreateAnchor(const Napi::CallbackInfo& info)
            {
                XRRigidTransform* transform = XRRigidTransform::Unwrap(info[0].As<Napi::Object>());
                return CreateNativeAnchor(info, transform->GetNativePose(), nullptr);
            }

            Napi::Value GetTrackedAnchors(const Napi::CallbackInfo& info)
            {
                // Create a JavaScript set to store all currently tracked anchors.
                Napi::Object anchorSet = info.Env().Global().Get("Set").As<Napi::Function>().New({});

                // Loop over the list of tracked anchors, and add them to the set.
                for (const Napi::ObjectReference& napiAnchorRef : m_trackedAnchors)
                {
                    anchorSet.Get("add").As<Napi::Function>().Call(anchorSet, {napiAnchorRef.Value()});
                }

                return std::move(anchorSet);
            }

            void UpdateAnchors()
            {
                // Loop over all anchors and update their state.
                std::vector<Napi::ObjectReference>::iterator anchorIter = m_trackedAnchors.begin();
                while (anchorIter != m_trackedAnchors.end())
                {
                    XRAnchor* xrAnchor = XRAnchor::Unwrap((*anchorIter).Value());
                    xr::Anchor& nativeAnchor = xrAnchor->GetNativeAnchor();

                    // Update the anchor if it has not been marked for deletion.
                    if (nativeAnchor.IsValid)
                    {
                        m_frame->UpdateAnchor(nativeAnchor);
                    }

                    // If the anchor has been marked for deletion, delete the anchor from the session
                    // and remove it from the list of tracked anchors.
                    if (!nativeAnchor.IsValid)
                    {
                        m_frame->DeleteAnchor(nativeAnchor);
                        anchorIter = m_trackedAnchors.erase(anchorIter);
                    }
                    else
                    {
                        ++anchorIter;
                    }
                }
            }

            Napi::Value GetWorldInformation(const Napi::CallbackInfo& info)
            {
                // Create a JavaScript object that stores all world information.
                Napi::Object worldInformationObj = Napi::Object::New(info.Env());

                // Create a set to contain all of the currently tracked planes.
                Napi::Object planeSet = info.Env().Global().Get("Set").As<Napi::Function>().New({});

                // Loop over the list of tracked planes, and add them to the set.
                for (const auto& [plane, planeNapiValue] : m_trackedPlanes)
                {
                    planeSet.Get("add").As<Napi::Function>().Call(planeSet, {planeNapiValue.Value()});
                }

                // Pass the world information object back to the caller.
                worldInformationObj.Set("detectedPlanes", planeSet);

                if (m_meshSet)
                {
                    worldInformationObj.Set("detectedMeshes", m_meshSet.Value());
                }

                return std::move(worldInformationObj);
            }

            Napi::Value GetFeaturePointCloud(const Napi::CallbackInfo& info)
            {
                // Get feature points from native.
                std::vector<xr::FeaturePoint>& pointCloud = m_frame->FeaturePointCloud;
                auto featurePointArray = Napi::Array::New(info.Env(), pointCloud.size() * 5);
                for (size_t i = 0; i < pointCloud.size(); i++)
                {
                    int pointIndex = (int) i * 5;
                    auto& featurePoint = pointCloud[i];
                    featurePointArray.Set(pointIndex, Napi::Value::From(info.Env(), featurePoint.X));
                    featurePointArray.Set(pointIndex + 1, Napi::Value::From(info.Env(), featurePoint.Y));
                    featurePointArray.Set(pointIndex + 2, Napi::Value::From(info.Env(), featurePoint.Z));
                    featurePointArray.Set(pointIndex + 3, Napi::Value::From(info.Env(), featurePoint.ConfidenceValue));
                    featurePointArray.Set(pointIndex + 4, Napi::Value::From(info.Env(), featurePoint.ID));
                }

                return std::move(featurePointArray);
            }

            void UpdateSceneObjects(const Napi::Env& env)
            {
                for (const auto& sceneObjectID : m_frame->UpdatedSceneObjects)
                {
                    if (m_sceneObjects.count(sceneObjectID) == 0)
                    {
                        m_sceneObjects[sceneObjectID] = Napi::Persistent(Napi::Object::New(env));
                    }

                    const auto& sceneObject = m_frame->GetSceneObjectByID(sceneObjectID);
                    m_sceneObjects.at(sceneObjectID).Value().Set("type", xr::SceneObjectTypeNames.at(sceneObject.Type));
                }

                for (const auto& removedObjectID : m_frame->RemovedSceneObjects)
                {
                    m_sceneObjects.erase(removedObjectID);
                }
            }

            void UpdatePlanes(const Napi::Env& env, uint32_t timestamp)
            {
                // First loop over the list of updated planes, check if they exist in our map if not create them otherwise update them.
                for (auto planeID : m_frame->UpdatedPlanes)
                {
                    XRPlane* xrPlane{};
                    auto trackedPlaneIterator = m_trackedPlanes.find(planeID);

                    // Plane does not yet exist create the JS object and insert it into the map.
                    if (trackedPlaneIterator == m_trackedPlanes.end())
                    {
                        auto napiPlane = Napi::Persistent(XRPlane::New(env));
                        xrPlane = XRPlane::Unwrap(napiPlane.Value());
                        xrPlane->SetNativePlaneId(planeID);
                        xrPlane->SetXRFrame(this);
                        m_trackedPlanes.insert({planeID, std::move(napiPlane)});
                    }
                    else
                    {
                        xrPlane = XRPlane::Unwrap(trackedPlaneIterator->second.Value());
                    }

                    xrPlane->SetLastUpdatedTime(timestamp);
                }

                // Next go over removed planes and remove them from our mapping.
                for (auto planeID : m_frame->RemovedPlanes)
                {
                    auto trackedPlaneIterator = m_trackedPlanes.find(planeID);
                    assert(trackedPlaneIterator != m_trackedPlanes.end());
                    m_trackedPlanes.erase(trackedPlaneIterator);
                }
            }

            void UpdateMeshes(const Napi::Env& env, uint32_t timestamp)
            {
                for (auto meshID : m_frame->UpdatedMeshes)
                {
                    XRMesh* xrMesh{};
                    auto trackedMeshIterator = m_trackedMeshes.find(meshID);

                    if (trackedMeshIterator == m_trackedMeshes.end())
                    {
                        auto napiMesh = Napi::Persistent(XRMesh::New(env));
                        xrMesh = XRMesh::Unwrap(napiMesh.Value());
                        xrMesh->SetNativeMeshId(meshID);
                        xrMesh->SetXRFrame(this);
                        m_trackedMeshes.insert({meshID, std::move(napiMesh)});
                    }
                    else
                    {
                        xrMesh = XRMesh::Unwrap(trackedMeshIterator->second.Value());
                    }

                    xrMesh->SetLastUpdatedTime(timestamp);
                }

                for (auto meshID : m_frame->RemovedMeshes)
                {
                    m_trackedMeshes.erase(meshID);
                }

                // Create a new mesh set every frame, detected meshes are assumed immutable
                m_meshSet = Napi::Persistent(env.Global().Get("Set").As<Napi::Function>().New({}));
                for (const auto& [meshID, meshNapiValue] : m_trackedMeshes)
                {
                    m_meshSet.Value().Get("add").As<Napi::Function>().Call(m_meshSet.Value(), { meshNapiValue.Value() });
                }
            }
        };

        // Creates an anchor from a hit result.
        Napi::Value XRHitTestResult::CreateAnchor(const Napi::CallbackInfo& info)
        {
            return m_frame->CreateNativeAnchor(info, m_hitResult.Pose, m_hitResult.NativeTrackable);
        }

        xr::System::Session::Frame::Plane& XRPlane::GetPlane()
        {
            return m_frame->GetPlaneFromID(m_nativePlaneID);
        }

        Napi::Value XRPlane::GetParentSceneObject(const Napi::CallbackInfo& info)
        {
            const auto& plane = GetPlane();
            return m_frame->GetJSSceneObjectFromID(info, plane.ParentSceneObjectID);
        }

        xr::System::Session::Frame::Mesh& XRMesh::GetMesh()
        {
            return m_frame->GetMeshFromID(m_nativeMeshID);
        }

        Napi::Value XRMesh::GetParentSceneObject(const Napi::CallbackInfo& info)
        {
            const auto& mesh = GetMesh();
            return m_frame->GetJSSceneObjectFromID(info, mesh.ParentSceneObjectID);
        }

        // Implementation of the XRSession interface: https://immersive-web.github.io/webxr/#xrsession-interface
        class XRSession : public Napi::ObjectWrap<XRSession>
        {
            static constexpr auto JS_CLASS_NAME = "XRSession";
            static constexpr auto JS_EVENT_NAME_END = "end";
            static constexpr auto JS_EVENT_NAME_INPUT_SOURCES_CHANGE = "inputsourceschange";
            static constexpr auto JS_SESSION_STATE_CALLBACK_NAME = "_sessionStateCallback";
            static constexpr auto JS_WINDOW_PROVIDER_NAME = "_windowProvider";

        public:
            static void Initialize(Napi::Env env, std::function<void(bool)> sessionStateChangedCallback, std::function<void*()> windowProvider)
            {
                Napi::Function func = DefineClass(
                    env,
                    JS_CLASS_NAME,
                    {
                        InstanceAccessor("inputSources", &XRSession::GetInputSources, nullptr),
                        InstanceAccessor("nativeXrContext", &XRSession::GetNativeXrContext, nullptr),
                        InstanceAccessor("nativeXrContextType", &XRSession::GetNativeXrContextType, nullptr),
                        InstanceMethod("addEventListener", &XRSession::AddEventListener),
                        InstanceMethod("removeEventListener", &XRSession::RemoveEventListener),
                        InstanceMethod("requestReferenceSpace", &XRSession::RequestReferenceSpace),
                        InstanceMethod("updateRenderState", &XRSession::UpdateRenderState),
                        InstanceMethod("requestAnimationFrame", &XRSession::RequestAnimationFrame),
                        InstanceMethod("end", &XRSession::End),
                        InstanceMethod("requestHitTestSource", &XRSession::RequestHitTestSource),
                        InstanceMethod("updateWorldTrackingState", &XRSession::UpdateWorldTrackingState),
                        InstanceMethod("trySetFeaturePointCloudEnabled", &XRSession::TrySetFeaturePointCloudEnabled),
                        InstanceMethod("trySetPreferredPlaneDetectorOptions", &XRSession::TrySetPreferredPlaneDetectorOptions),
                        InstanceMethod("trySetMeshDetectorEnabled", &XRSession::TrySetMeshDetectorEnabled),
                        InstanceMethod("trySetPreferredMeshDetectorOptions", &XRSession::TrySetPreferredMeshDetectorOptions),
                    });

                Napi::Value sessionStateChangedCallbackExternal{Napi::External<std::function<void(bool)>>::New(env, new std::function<void(bool)>(std::move(sessionStateChangedCallback)), [](Napi::Env, std::function<void(bool)>* callback) { delete callback; }) };
                func.Set(JS_SESSION_STATE_CALLBACK_NAME, sessionStateChangedCallbackExternal);

                Napi::Value windowProviderExternal{ Napi::External<std::function<void*()>>::New(env, new std::function<void*()>(std::move(windowProvider)), [](Napi::Env, std::function<void*()>* callback) { delete callback; }) };
                func.Set(JS_WINDOW_PROVIDER_NAME, windowProviderExternal);

                env.Global().Set(JS_CLASS_NAME, func);
            }

            static Napi::Promise CreateAsync(const Napi::CallbackInfo& info)
            {
                auto jsSession{Napi::Persistent(info.Env().Global().Get(JS_CLASS_NAME).As<Napi::Function>().New({info[0]}))};
                auto& session{*XRSession::Unwrap(jsSession.Value())};

                auto deferred{Napi::Promise::Deferred::New(info.Env())};
                session.m_xr->BeginSessionAsync()
                    .then(session.m_runtimeScheduler, arcana::cancellation::none(),
                        [deferred, jsSession{std::move(jsSession)}, env{info.Env()}](const arcana::expected<void, std::exception_ptr>& result) {
                            if (result.has_error())
                            {
                                deferred.Reject(Napi::Error::New(env, result.error()).Value());
                            }
                            else
                            {
                                deferred.Resolve(jsSession.Value());
                            }
                        });

                return deferred.Promise();
            }

            XRSession(const Napi::CallbackInfo& info)
                : Napi::ObjectWrap<XRSession>{info}
                , m_runtimeScheduler{JsRuntime::GetFromJavaScript(info.Env())}
                , m_xr{std::make_shared<Plugins::NativeXr::Impl>(
                          info.Env(),
                          m_runtimeScheduler,
                          *info.NewTarget().As<Napi::Function>().Get(JS_SESSION_STATE_CALLBACK_NAME).As<Napi::External<std::function<void(bool)>>>().Data(),
                          *info.NewTarget().As<Napi::Function>().Get(JS_WINDOW_PROVIDER_NAME).As<Napi::External<std::function<void*()>>>().Data())}
                , m_jsXRFrame{Napi::Persistent(XRFrame::New(info))}
                , m_xrFrame{*XRFrame::Unwrap(m_jsXRFrame.Value())}
                , m_jsInputSources{Napi::Persistent(Napi::Array::New(info.Env()))}
            {
                // Currently only immersive VR and immersive AR are supported.
                assert(info[0].As<Napi::String>().Utf8Value() == XRSessionType::IMMERSIVE_VR ||
                    info[0].As<Napi::String>().Utf8Value() == XRSessionType::IMMERSIVE_AR);
            }

            void InitializeXrLayer(Napi::Object layer)
            {
                // NOTE: We currently only support rendering to the entire frame. Because the following values
                // are only used in the context of each other, width and hight as used here don't need to have
                // anything to do with actual pixel widths. This behavior is permitted by the draft WebXR spec,
                // which states that the, "exact interpretation of the viewport values depends on the conventions
                // of the graphics API the viewport is associated with." Since Babylon.js is here doing the
                // the interpretation for our graphics API, we are able to provide Babylon.js with simple values
                // that will communicate the correct behavior. In theory, for partial texture rendering, the
                // only part of this that will need to be fixed is the viewport (the layer will need one for
                // each view, not just the one that currently exists).
                // Spec reference: https://immersive-web.github.io/webxr/#dom-xrviewport-width
                constexpr size_t WIDTH = 1;
                constexpr size_t HEIGHT = 1;

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

            Napi::Value GetRenderTargetForEye(const std::string& eye) const
            {
                return m_xr->GetRenderTargetForViewIndex(XREye::EyeToIndex(eye));
            }

            void SetRenderTextureFunctions(const Napi::Function& createFunction, const Napi::Function& destroyFunction)
            {
                return m_xr->SetRenderTextureFunctions(createFunction, destroyFunction);
            }

        private:
            JsRuntimeScheduler m_runtimeScheduler;
            std::shared_ptr<Plugins::NativeXr::Impl> m_xr;
            Napi::ObjectReference m_jsXRFrame{};
            XRFrame& m_xrFrame;
            uint32_t m_timestamp{0};

            std::vector<std::pair<std::string, Napi::FunctionReference>> m_eventNamesAndCallbacks{};

            Napi::Reference<Napi::Array> m_jsInputSources{};
            std::map<xr::System::Session::Frame::InputSource::Identifier, Napi::ObjectReference> m_idToInputSource{};

            Napi::Value GetInputSources(const Napi::CallbackInfo& /*info*/)
            {
                return m_jsInputSources.Value();
            }

            void AddEventListener(const Napi::CallbackInfo& info)
            {
                m_eventNamesAndCallbacks.emplace_back(
                    info[0].As<Napi::String>().Utf8Value(),
                    Napi::Persistent(info[1].As<Napi::Function>()));
            }

            void RemoveEventListener(const Napi::CallbackInfo& info)
            {
                auto name = info[0].As<Napi::String>().Utf8Value();
                auto callback = info[1].As<Napi::Function>();
                m_eventNamesAndCallbacks.erase(std::remove_if(
                    m_eventNamesAndCallbacks.begin(),
                    m_eventNamesAndCallbacks.end(),
                    [&name, &callback](const std::pair<std::string, Napi::FunctionReference>& listener)
                {
                    return listener.first == name && listener.second.Value() == callback;
                }), m_eventNamesAndCallbacks.end());
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

                float depthNear = renderState.Get("depthNear").As<Napi::Number>().FloatValue();
                float depthFar = renderState.Get("depthFar").As<Napi::Number>().FloatValue();
                m_xr->SetDepthsNarFar(depthNear, depthFar);

                auto deferred = Napi::Promise::Deferred::New(info.Env());
                deferred.Resolve(info.Env().Undefined());
                return deferred.Promise();
            }

            void ProcessInputSources(const xr::System::Session::Frame& frame, Napi::Env env)
            {
                // Figure out the new state.
                std::set<xr::System::Session::Frame::InputSource::Identifier> added{};
                std::set<xr::System::Session::Frame::InputSource::Identifier> current{};
                std::set<xr::System::Session::Frame::InputSource::Identifier> removed{};

                for (auto& inputSource : frame.InputSources)
                {
                    if (!inputSource.TrackedThisFrame)
                    {
                        continue;
                    }

                    current.insert(inputSource.ID);

                    auto inputSourceFound = m_idToInputSource.find(inputSource.ID);
                    if (inputSourceFound == m_idToInputSource.end())
                    {
                        // Create the new input source, which will have the correct spaces associated with it.
                        m_idToInputSource.insert({inputSource.ID, CreateXRInputSource(inputSource, env)});

                        //Now that input Source is created, create a gamepad object if enabled for the input source
                        inputSourceFound = m_idToInputSource.find(inputSource.ID);
                        if (inputSource.GamepadTrackedThisFrame)
                        {
                            auto inputSourceVal = inputSourceFound->second.Value();
                            CreateXRGamepadObject(inputSourceVal, inputSource);
                        }

                        added.insert(inputSource.ID);
                    }
                    else
                    {
                        // Ensure the correct spaces are associated with the existing input source.
                        auto inputSourceVal = inputSourceFound->second.Value();
                        SetXRInputSourceData(inputSourceVal, inputSource);

                        //inputSource already exists, find the corresponding gamepad object if enabled and set to correct values
                        if (inputSourceVal.Has("gamepad"))
                        {
                            auto gamepadObject = inputSourceVal.Get("gamepad").As<Napi::Object>();
                            SetXRGamepadObjectData(inputSourceVal, gamepadObject, inputSource);
                        }
                    }
                }
                for (const auto& [id, ref] : m_idToInputSource)
                {
                    if (current.find(id) == current.end())
                    {
                        // Do not update space association since said spaces no longer exist.
                        removed.insert(id);
                    }
                }

                // Only need to do more if there's been a change. Note that this block of code assumes
                // that ALL known input sources -- including ones added AND REMOVED this frame -- are
                // currently up-to-date and accessible though m_idToInputSource.
                if (added.size() > 0 || removed.size() > 0)
                {
                    // Update the input sources array.
                    auto jsCurrent = Napi::Array::New(env);
                    for (const auto id : current)
                    {
                        jsCurrent.Set(jsCurrent.Length(), m_idToInputSource[id].Value());
                    }
                    m_jsInputSources = Napi::Persistent(jsCurrent);

                    // Create and send the sources changed event.
                    Napi::Array jsAdded = Napi::Array::New(env);
                    for (const auto id : added)
                    {
                        jsAdded.Set(jsAdded.Length(), m_idToInputSource[id].Value());
                    }
                    Napi::Array jsRemoved = Napi::Array::New(env);
                    for (const auto id : removed)
                    {
                        jsRemoved.Set(jsRemoved.Length(), m_idToInputSource[id].Value());
                    }
                    auto sourcesChangeEvent = Napi::Object::New(env);
                    sourcesChangeEvent.Set("added", jsAdded);
                    sourcesChangeEvent.Set("removed", jsRemoved);
                    for (const auto& [name, callback] : m_eventNamesAndCallbacks)
                    {
                        if (name == JS_EVENT_NAME_INPUT_SOURCES_CHANGE)
                        {
                            callback.Call({sourcesChangeEvent});
                        }
                    }

                    // Finally, remove the removed.
                    for (const auto id : removed)
                    {
                        m_idToInputSource.erase(id);
                    }
                }
            }

            Napi::Value RequestAnimationFrame(const Napi::CallbackInfo& info)
            {
                Napi::Function callback{info[0].As<Napi::Function>()};

                m_xr->ScheduleFrame([this, callbackPtr{std::make_shared<Napi::FunctionReference>(Napi::Persistent(callback))}](const auto& frame) {
                    ProcessInputSources(frame, Env());

                    m_xrFrame.Update(Env(), frame, m_timestamp);

                    callbackPtr->Value().Call({Napi::Value::From(Env(), m_timestamp), m_jsXRFrame.Value()});
                });

                // The return value should be a request ID to allow for requesting cancellation, this is unused in Babylon.js currently.
                // For now just pass our "timestamp" as that uniquely identifies the frame.
                return Napi::Value::From(info.Env(), m_timestamp++);
            }

            void UpdateWorldTrackingState(const Napi::CallbackInfo& info)
            {
                auto optionsObj = info[0].As<Napi::Object>();
                if (optionsObj.Has("planeDetectionState"))
                {
                    bool planeDetectionEnabled = optionsObj.Get("planeDetectionState").As<Napi::Object>().Get("enabled").ToBoolean();
                    m_xr->SetPlaneDetectionEnabled(planeDetectionEnabled);
                }
            }

            Napi::Value TrySetFeaturePointCloudEnabled(const Napi::CallbackInfo& info)
            {
                bool featurePointCloudEnabled = info[0].ToBoolean();
                bool enabled = m_xr->TrySetFeaturePointCloudEnabled(featurePointCloudEnabled);

                return Napi::Value::From(info.Env(), enabled);
            }

            Napi::Value End(const Napi::CallbackInfo& info)
            {
                auto deferred{Napi::Promise::Deferred::New(info.Env())};
                m_xr->EndSessionAsync().then(m_runtimeScheduler, arcana::cancellation::none(),
                    [this, deferred](const arcana::expected<void, std::exception_ptr>& result) {
                        if (result.has_error())
                        {
                            deferred.Reject(Napi::Error::New(Env(), result.error()).Value());
                            return;
                        }

                        for (const auto& [name, callback] : m_eventNamesAndCallbacks)
                        {
                            if (name == JS_EVENT_NAME_END)
                            {
                                callback.Call({});
                            }
                        }


                        deferred.Resolve(Env().Undefined());
                    });

                return deferred.Promise();
            }

            Napi::Value RequestHitTestSource(const Napi::CallbackInfo& info)
            {
                auto deferred = Napi::Promise::Deferred::New(info.Env());
                deferred.Resolve(XRHitTestSource::New(info));
                return deferred.Promise();
            }

            Napi::Value TrySetPreferredPlaneDetectorOptions(const Napi::CallbackInfo& info)
            {
                if (info.Length() != 1 ||
                    !info[0].IsObject())
                {
                    throw std::exception(/*invalid arguments*/);
                }

                const auto options = CreateDetectorOptions(info[0].As<Napi::Object>());
                const auto result = m_xr->TrySetPreferredPlaneDetectorOptions(options);
                return Napi::Value::From(info.Env(), result);
            }

            Napi::Value TrySetMeshDetectorEnabled(const Napi::CallbackInfo& info)
            {
                if (info.Length() != 1 ||
                    !info[0].IsBoolean())
                {
                    throw std::exception(/*invalid arguments*/);
                }

                const auto enabled = info[0].As<Napi::Boolean>();
                const auto result = m_xr->TrySetMeshDetectorEnabled(enabled);
                return Napi::Value::From(info.Env(), result);
            }

            Napi::Value TrySetPreferredMeshDetectorOptions(const Napi::CallbackInfo& info)
            {
                if (info.Length() != 1 ||
                    !info[0].IsObject())
                {
                    throw std::exception(/*invalid arguments*/);
                }

                const auto options = CreateDetectorOptions(info[0].As<Napi::Object>());
                const auto result = m_xr->TrySetPreferredMeshDetectorOptions(options);
                return Napi::Value::From(info.Env(), result);
            }

            Napi::Value GetNativeXrContext(const Napi::CallbackInfo& info)
            {
                const auto nativeExtension = m_xr->GetNativeXrContext();
                if (nativeExtension)
                {
                    return Napi::Number::From(info.Env(), nativeExtension);
                }

                return info.Env().Undefined();
            }

            Napi::Value GetNativeXrContextType(const Napi::CallbackInfo& info)
            {
                const auto nativeExtensionType = m_xr->GetNativeXrContextType();
                if (!nativeExtensionType.empty())
                {
                    return Napi::String::From(info.Env(), nativeExtensionType);
                }

                return info.Env().Undefined();
            }
        };

        class NativeWebXRRenderTarget : public Napi::ObjectWrap<NativeWebXRRenderTarget>
        {
            static constexpr auto JS_CLASS_NAME = "NativeWebXRRenderTarget";

        public:
            static void Initialize(Napi::Env env)
            {
                Napi::HandleScope scope{env};

                Napi::Function func = DefineClass(
                    env,
                    JS_CLASS_NAME,
                    {
                        InstanceMethod("initializeXRLayerAsync", &NativeWebXRRenderTarget::InitializeXRLayerAsync),
                        InstanceMethod("dispose", &NativeWebXRRenderTarget::Dispose),
                    });

                env.Global().Set(JS_CLASS_NAME, func);
            }

            static Napi::Object New(const Napi::CallbackInfo& info)
            {
                return info.Env().Global().Get(JS_CLASS_NAME).As<Napi::Function>().New({info[0]});
            }

            NativeWebXRRenderTarget(const Napi::CallbackInfo& info)
                : Napi::ObjectWrap<NativeWebXRRenderTarget>{info}
                , m_jsEngineReference{Napi::Persistent(info[0].As<Napi::Object>())}
            {
            }

        private:
            // Lifetime control to prevent the cleanup of the NativeEngine while XR is still alive.
            Napi::ObjectReference m_jsEngineReference{};

            Napi::Value InitializeXRLayerAsync(const Napi::CallbackInfo& info)
            {
                auto& session = *XRSession::Unwrap(info[0].As<Napi::Object>());

                auto xrLayer = XRWebGLLayer::New(info);
                session.InitializeXrLayer(xrLayer);
                info.This().As<Napi::Object>().Set("xrLayer", xrLayer);

                auto deferred = Napi::Promise::Deferred::New(info.Env());
                deferred.Resolve(info.Env().Undefined());
                return deferred.Promise();
            }

            Napi::Value Dispose(const Napi::CallbackInfo& info)
            {
                return info.Env().Undefined();
            }
        };

        class NativeRenderTargetProvider : public Napi::ObjectWrap<NativeRenderTargetProvider>
        {
            static constexpr auto JS_CLASS_NAME = "NativeRenderTargetProvider";

        public:
            static void Initialize(Napi::Env env)
            {
                Napi::HandleScope scope{env};

                Napi::Function func = DefineClass(
                    env,
                    JS_CLASS_NAME,
                    {
                        InstanceMethod("getRenderTargetForEye", &NativeRenderTargetProvider::GetRenderTargetForEye),
                    });

                env.Global().Set(JS_CLASS_NAME, func);
            }

            static Napi::Object New(const Napi::CallbackInfo& info)
            {
                return info.Env().Global().Get(JS_CLASS_NAME).As<Napi::Function>().New({info[0], info[1], info[2]});
            }

            NativeRenderTargetProvider(const Napi::CallbackInfo& info)
                : Napi::ObjectWrap<NativeRenderTargetProvider>{info}
                , m_jsSession{Napi::Persistent(info[0].As<Napi::Object>())}
                , m_session{*XRSession::Unwrap(m_jsSession.Value())}
            {
                auto createRenderTexture{info[1].As<Napi::Function>()};
                auto destroyRenderTexture{info[2].As<Napi::Function>()};
                m_session.SetRenderTextureFunctions(createRenderTexture, destroyRenderTexture);
            }

        private:
            Napi::ObjectReference m_jsSession{};
            XRSession& m_session;

            Napi::Value GetRenderTargetForEye(const Napi::CallbackInfo& info)
            {
                const std::string eye{info[0].As<Napi::String>().Utf8Value()};
                return m_session.GetRenderTargetForEye(eye);
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
            static void Initialize(Napi::Env env)
            {
                Napi::HandleScope scope{env};

                Napi::Function func = DefineClass(
                    env,
                    JS_CLASS_NAME,
                    {
                        InstanceMethod("isSessionSupported", &XR::IsSessionSupported),
                        InstanceMethod("requestSession", &XR::RequestSession),
                        InstanceMethod("getWebXRRenderTarget", &XR::GetWebXRRenderTarget),
                        InstanceMethod("getNativeRenderTargetProvider", &XR::GetNativeRenderTargetProvider),
                        InstanceValue(JS_NATIVE_NAME, Napi::Value::From(env, true)),
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
                : Napi::ObjectWrap<XR>{info}
                , m_runtimeScheduler{JsRuntime::GetFromJavaScript(info.Env())}
            {
            }

        private:
            JsRuntimeScheduler m_runtimeScheduler;

            Napi::Value IsSessionSupported(const Napi::CallbackInfo& info)
            {
                std::string sessionTypeString = info[0].As<Napi::String>().Utf8Value();
                xr::SessionType sessionType{xr::SessionType::INVALID};

                if (sessionTypeString == XRSessionType::IMMERSIVE_VR)
                {
                    sessionType = xr::SessionType::IMMERSIVE_VR;
                }
                else if (sessionTypeString == XRSessionType::IMMERSIVE_AR)
                {
                    sessionType = xr::SessionType::IMMERSIVE_AR;
                }
                else if (sessionTypeString == XRSessionType::INLINE)
                {
                    sessionType = xr::SessionType::INLINE;
                }

                auto deferred = Napi::Promise::Deferred::New(info.Env());

                // Fire off the IsSessionSupported task.
                xr::System::IsSessionSupportedAsync(sessionType)
                    .then(m_runtimeScheduler,
                        arcana::cancellation::none(),
                        [deferred, env = info.Env()](bool result) {
                            deferred.Resolve(Napi::Boolean::New(env, result));
                        });

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

    namespace Plugins
    {
        NativeXr::NativeXr(std::shared_ptr<Impl> impl)
            : m_impl{ std::move(impl) }
        {
        }

        NativeXr::~NativeXr()
        {
        }

        NativeXr NativeXr::Initialize(Napi::Env env, Configuration config)
        {
            PointerEvent::Initialize(env);

            XRWebGLLayer::Initialize(env);
            XRRigidTransform::Initialize(env);
            XRView::Initialize(env);
            XRViewerPose::Initialize(env);
            XRPose::Initialize(env);
            XRReferenceSpace::Initialize(env);
            XRFrame::Initialize(env);
            XRHand::Initialize(env);
            XRPlane::Initialize(env);
            XRMesh::Initialize(env);
            XRAnchor::Initialize(env);
            XRHitTestSource::Initialize(env);
            XRHitTestResult::Initialize(env);
            XRRay::Initialize(env);
            XRSession::Initialize(env, std::move(config.SessionStateChangedCallback), std::move(config.WindowProvider));
            NativeWebXRRenderTarget::Initialize(env);
            NativeRenderTargetProvider::Initialize(env);
            XR::Initialize(env);

            return {{}};
        }
    }
}
