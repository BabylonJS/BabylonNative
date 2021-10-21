#include <bgfx/bgfx.h>
#include <map>
#include "Canvas.h"
#include "Image.h"
#include "Context.h"
#include <functional>
#include <sstream>
#include <assert.h>
#include <NativeEngine.h>
#include <bimg/bimg.h>
#include <bimg/decode.h>
#include "nanovg.h"
#include <cassert>

namespace Babylon::Polyfills::Internal
{
    static constexpr auto JS_CONSTRUCTOR_NAME = "Image";

    void NativeCanvasImage::CreateInstance(Napi::Env env)
    {
        Napi::HandleScope scope{env};

        Napi::Function func = DefineClass(
            env,
            JS_CONSTRUCTOR_NAME,
            {
                InstanceAccessor("width", &NativeCanvasImage::GetWidth, nullptr),
                InstanceAccessor("height", &NativeCanvasImage::GetHeight, nullptr),
                InstanceAccessor("naturalWidth", &NativeCanvasImage::GetNaturalWidth, nullptr),
                InstanceAccessor("naturalHeight", &NativeCanvasImage::GetNaturalHeight, nullptr),
                InstanceAccessor("src", &NativeCanvasImage::GetSrc, &NativeCanvasImage::SetSrc),
                InstanceAccessor("onload", nullptr, &NativeCanvasImage::SetOnload),
            });

        JsRuntime::NativeObject::GetFromJavaScript(env).Set(JS_CONSTRUCTOR_NAME, func);
    }

    NativeCanvasImage::NativeCanvasImage(const Napi::CallbackInfo& info)
        : Napi::ObjectWrap<NativeCanvasImage>{info}
        , m_runtimeScheduler{JsRuntime::GetFromJavaScript(info.Env())}
        , m_cancellationSource{std::make_shared<arcana::cancellation_source>()}
    {
    }

    NativeCanvasImage::~NativeCanvasImage()
    {
        Dispose();
    }

    void NativeCanvasImage::Dispose()
    {
        if (m_imageContainer)
        {
            bimg::imageFree(m_imageContainer);
            m_imageContainer = nullptr;
        }
        m_cancellationSource->cancel();
    }

    Napi::Value NativeCanvasImage::GetWidth(const Napi::CallbackInfo&)
    {
        return Napi::Value::From(Env(), m_width);
    }

    Napi::Value NativeCanvasImage::GetHeight(const Napi::CallbackInfo&)
    {
        return Napi::Value::From(Env(), m_height);
    }

    Napi::Value NativeCanvasImage::GetNaturalWidth(const Napi::CallbackInfo&)
    {
        return Napi::Value::From(Env(), m_width);
    }

    Napi::Value NativeCanvasImage::GetNaturalHeight(const Napi::CallbackInfo&)
    {
        return Napi::Value::From(Env(), m_height);
    }

    Napi::Value NativeCanvasImage::GetSrc(const Napi::CallbackInfo&)
    {
        return Napi::Value::From(Env(), m_src);
    }

    void NativeCanvasImage::SetSrc(const Napi::CallbackInfo& info, const Napi::Value& value)
    {
        auto text{value.As<Napi::String>().Utf8Value()};
        UrlLib::UrlRequest request{};
        request.Open(UrlLib::UrlMethod::Get, text);
        request.ResponseType(UrlLib::UrlResponseType::Buffer);
        request.SendAsync().then(m_runtimeScheduler, *m_cancellationSource, [env{info.Env()}, this, request{std::move(request)}](arcana::expected<void, std::exception_ptr> result) {
            if (result.has_error())
            {
                throw Napi::Error::New(env, result.error());
            }

            Dispose();

            auto buffer{request.ResponseBuffer()};
            m_imageContainer = bimg::imageParse(&m_allocator, buffer.data(), static_cast<uint32_t>(buffer.size_bytes()));
            if (m_imageContainer == nullptr)
            {
                Napi::Error::New(env, "Unable to decode image with provided src for in Canvas.").ThrowAsJavaScriptException();
            }

            m_width = m_imageContainer->m_width;
            m_height = m_imageContainer->m_height;

            m_onloadHandlerRef.Call({});
        });
    }

    void NativeCanvasImage::SetOnload(const Napi::CallbackInfo&, const Napi::Value& value)
    {
        Napi::Function eventHandler{value.As<Napi::Function>()};
        m_onloadHandlerRef = Napi::Persistent(eventHandler);
    }

    int NativeCanvasImage::CreateNVGImageForContext(NVGcontext* nvgContext) const
    {
        return nvgCreateImageRGBA(nvgContext, m_width, m_height, 0, static_cast<const unsigned char*>(m_imageContainer->m_data));
    }
}
