#pragma once

// WebXR raw camera access: JS XRCamera object, per the WebXR Raw Camera
// Access module (https://immersive-web.github.io/raw-camera-access/). Exposed via
// XRView.camera; the camera image is retrieved with XRWebGLBinding.getCameraImage,
// which returns a Napi::Pointer<Graphics::Texture> — the same object type
// NativeEngine uses as an InternalTexture's underlyingResource, so Babylon.js can
// bind it as a regular sampler.
//
// The texture pointer originates from the xr backend (View::CameraTexturePointer, an
// MTLTexture on ARKit) and is wired to bgfx with the same createTexture2D +
// overrideInternal pattern used by NativeCamera's CameraDevice.

#include <Babylon/Graphics/DeviceContext.h>
#include <Babylon/Graphics/Texture.h>
#include <napi/pointer.h>
#include <arcana/threading/task.h>
#include <bgfx/bgfx.h>

#include <atomic>
#include <memory>

namespace Babylon
{
    class XRCamera : public Napi::ObjectWrap<XRCamera>
    {
        static constexpr auto JS_CLASS_NAME = "XRCamera";

    public:
        static void Initialize(Napi::Env env)
        {
            Napi::HandleScope scope{env};

            Napi::Function func = DefineClass(
                env,
                JS_CLASS_NAME,
                {
                    InstanceAccessor("width", &XRCamera::GetWidth, nullptr),
                    InstanceAccessor("height", &XRCamera::GetHeight, nullptr),
                });

            env.Global().Set(JS_CLASS_NAME, func);
        }

        static Napi::Object New(const Napi::CallbackInfo& info)
        {
            return info.Env().Global().Get(JS_CLASS_NAME).As<Napi::Function>().New({});
        }

        XRCamera(const Napi::CallbackInfo& info)
            : Napi::ObjectWrap<XRCamera>{info}
            , m_deviceContext{&Graphics::DeviceContext::GetFromJavaScript(info.Env())}
        {
        }

        // Called per getViewerPose from XRView::Update with the current platform camera
        // texture (MTLTexture on ARKit). Creates/refreshes the bgfx-wrapped texture when
        // the platform texture identity or size changes.
        void Update(Napi::Env env, void* texturePointer, size_t width, size_t height)
        {
            if (texturePointer == nullptr || width == 0 || height == 0)
            {
                m_hasTexture = false;
                return;
            }

            if (texturePointer != m_texturePointer || width != m_width || height != m_height)
            {
                m_texturePointer = texturePointer;
                m_width = width;
                m_height = height;

                // Create a bgfx texture to be overridden with the platform texture. The
                // Graphics::Texture (which owns the bgfx handle) is owned by the JS pointer
                // object below; replacing the persistent ref releases the old one via GC.
                const auto bgfxHandle = bgfx::createTexture2D(
                    static_cast<uint16_t>(width), static_cast<uint16_t>(height), false, 1,
                    bgfx::TextureFormat::BGRA8, BGFX_TEXTURE_NONE | BGFX_SAMPLER_NONE);

                auto* texture = new Graphics::Texture(*m_deviceContext);
                texture->Attach(bgfxHandle, true, static_cast<uint16_t>(width), static_cast<uint16_t>(height),
                    false, 1, bgfx::TextureFormat::BGRA8, BGFX_TEXTURE_NONE | BGFX_SAMPLER_NONE);

                m_textureJs = Napi::Persistent(Napi::Pointer<Graphics::Texture>::Create(env, texture, Napi::NapiPointerDeleter(texture)));

                // New handle needs a (re)override.
                m_overrideState = std::make_shared<OverrideState>();
                m_bgfxHandle = bgfxHandle;
            }

            // bgfx::overrideInternal only succeeds once the handle has been initialized by a
            // bgfx frame, so run it on the render thread after rendering and retry until it
            // sticks (same approach as NativeCamera's CameraDevice::UpdateCameraTexture).
            if (!m_overrideState->done.load() && !m_overrideState->pending.exchange(true))
            {
                arcana::make_task(m_deviceContext->AfterRenderScheduler(), arcana::cancellation::none(),
                    [state = m_overrideState, handle = m_bgfxHandle, pointer = m_texturePointer]() {
                        state->done.store(bgfx::overrideInternal(handle, reinterpret_cast<uintptr_t>(pointer)) != 0);
                        state->pending.store(false);
                    });
            }

            m_hasTexture = true;
        }

        bool HasTexture() const
        {
            return m_hasTexture && !m_textureJs.IsEmpty();
        }

        Napi::Value GetCameraImageValue(Napi::Env env)
        {
            if (!HasTexture())
            {
                return env.Undefined();
            }
            return m_textureJs.Value();
        }

    private:
        struct OverrideState
        {
            std::atomic<bool> done{false};
            std::atomic<bool> pending{false};
        };

        Graphics::DeviceContext* m_deviceContext{};
        void* m_texturePointer{};
        size_t m_width{};
        size_t m_height{};
        bool m_hasTexture{};
        bgfx::TextureHandle m_bgfxHandle{bgfx::kInvalidHandle};
        std::shared_ptr<OverrideState> m_overrideState{std::make_shared<OverrideState>()};
        Napi::Reference<Napi::Value> m_textureJs{};

        Napi::Value GetWidth(const Napi::CallbackInfo& info)
        {
            return Napi::Value::From(info.Env(), static_cast<uint32_t>(m_width));
        }

        Napi::Value GetHeight(const Napi::CallbackInfo& info)
        {
            return Napi::Value::From(info.Env(), static_cast<uint32_t>(m_height));
        }
    };
} // Babylon
