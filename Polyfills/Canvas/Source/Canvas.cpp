#include <bgfx/bgfx.h>
#include <map>
#include "Canvas.h"
#include "Context.h"
#include <functional>
#include <sstream>
#include <assert.h>

namespace Babylon::Polyfills::Internal
{
    void Canvas::CreateInstance(Napi::Env env)
    {
        Napi::HandleScope scope{env};

        Napi::Function func = ParentT::DefineClass(
            env,
            JS_CONSTRUCTOR_NAME,
            {
                ParentT::StaticMethod("loadTTF", &Canvas::LoadTTF),
                ParentT::InstanceMethod("getContext", &Canvas::GetContext),
                InstanceAccessor("width", &Canvas::GetWidth, &Canvas::SetWidth),
                InstanceAccessor("height", &Canvas::GetHeight, &Canvas::SetHeight),
                ParentT::InstanceMethod("getCanvasTexture", &Canvas::GetCanvasTexture),
                ParentT::StaticMethod("beginContextsFrame", &Canvas::BeginContextsFrame),
                ParentT::StaticMethod("endContextsFrame", &Canvas::EndContextsFrame),
                
            });

        env.Global().Set(JS_CONSTRUCTOR_NAME, func);
    }

    Canvas::Canvas(const Napi::CallbackInfo& info)
        : ParentT{info}
    {
    }

    Canvas::~Canvas()
    {
        if (m_frameBufferHandle.idx != bgfx::kInvalidHandle)
        {
            bgfx::destroy(m_frameBufferHandle);
        }
    }

    void Canvas::BeginContextsFrame(const Napi::CallbackInfo&)
    {
        Context::BeginContextsFrame();
    }

    void Canvas::EndContextsFrame(const Napi::CallbackInfo&)
    {
        Context::EndContextsFrame();
    }

    void Canvas::LoadTTF(const Napi::CallbackInfo& info)
    {
        std::string fontName = info[0].As<Napi::String>().Utf8Value();
        const auto buffer = info[1].As<Napi::ArrayBuffer>();

        fontsInfos[fontName] = std::vector<uint8_t>(buffer.ByteLength());
        memcpy(fontsInfos[fontName].data(), (uint8_t*)buffer.Data(), buffer.ByteLength());
    }

    Napi::Value Canvas::GetContext(const Napi::CallbackInfo& info)
    {
        return Context::CreateInstance(info.Env(), this, m_nextViewId++);
    }

    Napi::Value Canvas::GetWidth(const Napi::CallbackInfo&)
    {
        return Napi::Value::From(Env(), m_width);
    }

    void Canvas::SetWidth(const Napi::CallbackInfo&, const Napi::Value& value)
    {
        auto width = value.As<Napi::Number>().Uint32Value();
        if (width != m_width && width)
        {
            m_width = width;
            UpdateRenderTarget();
        }
    }

    Napi::Value Canvas::GetHeight(const Napi::CallbackInfo&)
    {
        return Napi::Value::From(Env(), m_height);
    }

    void Canvas::SetHeight(const Napi::CallbackInfo&, const Napi::Value& value)
    {
        auto height = value.As<Napi::Number>().Uint32Value();
        if (height != m_height && height)
        {
            m_height = height;
            UpdateRenderTarget();
        }
    }

    void Canvas::UpdateRenderTarget()
    {
        if (m_frameBufferHandle.idx != bgfx::kInvalidHandle)
        {
            bgfx::destroy(m_frameBufferHandle);
        }
        m_frameBufferHandle = bgfx::createFrameBuffer(static_cast<uint16_t>(m_width), static_cast<uint16_t>(m_height), bgfx::TextureFormat::RGBA8, BGFX_TEXTURE_RT);
        // destroyed frame buffer will have commited resources (and available resources back)
        // during frame rendering. Calling bgfx::frame here to get frame buffer handle
        bgfx::frame();
        assert(m_frameBufferHandle.idx != bgfx::kInvalidHandle);
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

    Napi::Value Canvas::GetCanvasTexture(const Napi::CallbackInfo& info)
    {
        assert(m_frameBufferHandle.idx != bgfx::kInvalidHandle);
        auto data = new TextureData();
        data->Handle = bgfx::getTexture(m_frameBufferHandle);
        data->Width = m_width;
        data->Height = m_height;
        return Napi::External<TextureData>::New(info.Env(), data);
    }
}

namespace Babylon::Polyfills::Canvas
{
    void Initialize(Napi::Env env)
    {
        Internal::Canvas::CreateInstance(env);
    }
}
