#include <bgfx/bgfx.h>
#include <map>
#include "Canvas.h"
#include "Image.h"
#include "Context.h"
#include <functional>
#include <sstream>
#include <assert.h>
#include <NativeEngine.h>
#include <napi/napi_pointer.h>

namespace Babylon::Polyfills::Internal
{
    static constexpr auto JS_CONSTRUCTOR_NAME = "NativeCanvas";

    void NativeCanvas::CreateInstance(Napi::Env env)
    {
        Napi::HandleScope scope{env};

        Napi::Function func = DefineClass(
            env,
            JS_CONSTRUCTOR_NAME,
            {
                StaticMethod("loadTTFAsync", &NativeCanvas::LoadTTFAsync),
                InstanceMethod("getContext", &NativeCanvas::GetContext),
                InstanceAccessor("width", &NativeCanvas::GetWidth, &NativeCanvas::SetWidth),
                InstanceAccessor("height", &NativeCanvas::GetHeight, &NativeCanvas::SetHeight),
                InstanceMethod("getCanvasTexture", &NativeCanvas::GetCanvasTexture),
                InstanceMethod("dispose", &NativeCanvas::Dispose),
            });

        JsRuntime::NativeObject::GetFromJavaScript(env).Set(JS_CONSTRUCTOR_NAME, func);
    }

    NativeCanvas::NativeCanvas(const Napi::CallbackInfo& info)
        : Napi::ObjectWrap<NativeCanvas>{info}
        , m_graphicsImpl{ Babylon::GraphicsImpl::GetFromJavaScript(info.Env()) }
    {
    }

    NativeCanvas::~NativeCanvas()
    {
        Dispose();
    }

    Napi::Value NativeCanvas::LoadTTFAsync(const Napi::CallbackInfo& info)
    {
        const auto buffer = info[1].As<Napi::ArrayBuffer>();
        std::vector<uint8_t> fontBuffer(buffer.ByteLength());
        memcpy(fontBuffer.data(), (uint8_t*)buffer.Data(), buffer.ByteLength());

        auto& graphicsImpl{Babylon::GraphicsImpl::GetFromJavaScript(info.Env())};
        std::shared_ptr<JsRuntimeScheduler> runtimeScheduler{ std::make_shared<JsRuntimeScheduler>(JsRuntime::GetFromJavaScript(info.Env())) };
        auto deferred{Napi::Promise::Deferred::New(info.Env())};
        arcana::make_task(graphicsImpl.BeforeRenderScheduler(), arcana::cancellation::none(), [fontName{ info[0].As<Napi::String>().Utf8Value() }, fontData{ std::move(fontBuffer) }]() {
            fontsInfos[fontName] = fontData;
        }).then(*runtimeScheduler, arcana::cancellation::none(), [runtimeScheduler /*Keep reference alive*/, env{ info.Env() }, deferred]() {
            deferred.Resolve(env.Undefined());
        });

        return deferred.Promise();
    }

    Napi::Value NativeCanvas::GetContext(const Napi::CallbackInfo& info)
    {
        return Context::CreateInstance(info.Env(), this);
    }

    Napi::Value NativeCanvas::GetWidth(const Napi::CallbackInfo&)
    {
        return Napi::Value::From(Env(), m_width);
    }

    void NativeCanvas::SetWidth(const Napi::CallbackInfo&, const Napi::Value& value)
    {
        auto width = value.As<Napi::Number>().Uint32Value();
        if (width != m_width && width)
        {
            m_width = width;
            m_dirty = true;
        }
    }

    Napi::Value NativeCanvas::GetHeight(const Napi::CallbackInfo&)
    {
        return Napi::Value::From(Env(), m_height);
    }

    void NativeCanvas::SetHeight(const Napi::CallbackInfo&, const Napi::Value& value)
    {
        auto height = value.As<Napi::Number>().Uint32Value();
        if (height != m_height && height)
        {
            m_height = height;
            m_dirty = true;
        }
    }

    bool NativeCanvas::UpdateRenderTarget()
    {
        if (m_dirty)
        {
            auto handle = bgfx::createFrameBuffer(static_cast<uint16_t>(m_width), static_cast<uint16_t>(m_height), bgfx::TextureFormat::RGBA8, BGFX_TEXTURE_RT);
            assert(handle.idx != bgfx::kInvalidHandle);
            m_frameBuffer = std::make_unique<FrameBuffer>(m_graphicsImpl, handle, static_cast<uint16_t>(m_width), static_cast<uint16_t>(m_height), false, false, false);
            m_dirty = false;

            if (m_textureData != nullptr)
            {
                // TODO REFACTOR
                //m_textureData->Dispose();
                m_textureData = nullptr;
            }

            return true;
        }
        return false;
    }

    Napi::Value NativeCanvas::GetCanvasTexture(const Napi::CallbackInfo& info)
    {
        if (m_textureData == nullptr)
        {
            m_textureData = new TextureData();
        }

        auto& textureData{*m_textureData};

        assert(m_frameBuffer->Handle().idx != bgfx::kInvalidHandle);
        textureData.Handle = bgfx::getTexture(m_frameBuffer->Handle());
        textureData.OwnsHandle = false;
        textureData.Width = m_width;
        textureData.Height = m_height;

        return Napi::Pointer<TextureData>::Create(info.Env(), m_textureData);
    }

    void NativeCanvas::Dispose()
    {
        m_frameBuffer.reset();

        if (m_textureData != nullptr)
        {
            // TODO REFACTOR
            //m_textureData->Dispose();
        }
    }

    void NativeCanvas::Dispose(const Napi::CallbackInfo& /*info*/)
    {
        Dispose();
    }
}

namespace Babylon::Polyfills::Canvas
{
    void Initialize(Napi::Env env)
    {
        Internal::NativeCanvas::CreateInstance(env);
        Internal::NativeCanvasImage::CreateInstance(env);
    }
}
