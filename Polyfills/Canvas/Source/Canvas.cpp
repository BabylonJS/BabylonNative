#include <bgfx/bgfx.h>
#include <map>
#include "Canvas.h"
#include "Context.h"
#include <functional>
#include <sstream>
#include <assert.h>

namespace Babylon::Polyfills::Internal
{
    void NativeCanvas::CreateInstance(Napi::Env env)
    {
        Napi::HandleScope scope{env};

        Napi::Function func = ParentT::DefineClass(
            env,
            JS_CONSTRUCTOR_NAME,
            {
                ParentT::StaticMethod("loadTTF", &NativeCanvas::LoadTTF),
                ParentT::InstanceMethod("getContext", &NativeCanvas::GetContext),
                InstanceAccessor("width", &NativeCanvas::GetWidth, &NativeCanvas::SetWidth),
                InstanceAccessor("height", &NativeCanvas::GetHeight, &NativeCanvas::SetHeight),
                ParentT::InstanceMethod("getCanvasTexture", &NativeCanvas::GetCanvasTexture),
                ParentT::InstanceMethod("dispose", &NativeCanvas::Dispose),
            });

        JsRuntime::NativeObject::GetFromJavaScript(env).Set(JS_CONSTRUCTOR_NAME, func);
    }

    NativeCanvas::NativeCanvas(const Napi::CallbackInfo& info)
        : ParentT{info}
        , m_graphicsImpl{ Babylon::GraphicsImpl::GetFromJavaScript(info.Env()) }
    {
    }

    NativeCanvas::~NativeCanvas()
    {
        if (m_frameBufferHandle.idx != bgfx::kInvalidHandle)
        {
            bgfx::destroy(m_frameBufferHandle);
        }
    }

    void NativeCanvas::LoadTTF(const Napi::CallbackInfo& info)
    {
        std::string fontName = info[0].As<Napi::String>().Utf8Value();
        const auto buffer = info[1].As<Napi::ArrayBuffer>();

        fontsInfos[fontName] = std::vector<uint8_t>(buffer.ByteLength());
        memcpy(fontsInfos[fontName].data(), (uint8_t*)buffer.Data(), buffer.ByteLength());
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
            /*arcana::make_task(m_graphicsImpl.BeforeRenderScheduler(), arcana::cancellation::none(), [this] {
                UpdateRenderTarget();
                });*/
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
            /*arcana::make_task(m_graphicsImpl.BeforeRenderScheduler(), arcana::cancellation::none(), [this] {
                UpdateRenderTarget();
                });*/
        }
    }

    void NativeCanvas::UpdateRenderTarget()
    {
        if (m_dirty)
        {
            if (m_frameBufferHandle.idx != bgfx::kInvalidHandle)
            {
                m_graphicsImpl.RemoveFrameBuffer(*m_frameBuffer);
                bgfx::destroy(m_frameBufferHandle);
            }
            m_frameBufferHandle = bgfx::createFrameBuffer(static_cast<uint16_t>(m_width), static_cast<uint16_t>(m_height), bgfx::TextureFormat::RGBA8, BGFX_TEXTURE_RT);
            m_frameBuffer = &m_graphicsImpl.AddFrameBuffer(m_frameBufferHandle, static_cast<uint16_t>(m_width), static_cast<uint16_t>(m_height), false);
            // destroyed frame buffer will have commited resources (and available resources back)
            // during frame rendering. Calling bgfx::frame here to get frame buffer handle
            //bgfx::frame();
            assert(m_frameBufferHandle.idx != bgfx::kInvalidHandle);
            m_dirty = false;
        }
    }

    struct TextureData final
    {
        ~TextureData()
        {
            if (bgfx::isValid(Handle))
            {
                bgfx::destroy(Handle);
            }
        }

        bgfx::TextureHandle Handle{ bgfx::kInvalidHandle };
        uint32_t Width{ 0 };
        uint32_t Height{ 0 };
        uint32_t Flags{ 0 };
        uint8_t AnisotropicLevel{ 0 };
    };

    Napi::Value NativeCanvas::GetCanvasTexture(const Napi::CallbackInfo& info)
    {
        assert(m_frameBufferHandle.idx != bgfx::kInvalidHandle);
        auto data = new TextureData();
        data->Handle = bgfx::getTexture(m_frameBufferHandle);
        data->Width = m_width;
        data->Height = m_height;
        return Napi::External<TextureData>::New(info.Env(), data);
    }

    void NativeCanvas::Dispose(const Napi::CallbackInfo& info)
    {
    }
}

namespace Babylon::Polyfills::Canvas
{
    void Initialize(Napi::Env env)
    {
        Internal::NativeCanvas::CreateInstance(env);
    }
}
