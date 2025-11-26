#include <bgfx/bgfx.h>
#include <map>
#include "Canvas.h"
#include "Image.h"
#include "Context.h"
#include <functional>
#include <sstream>
#include <assert.h>
#include <bimg/bimg.h>
#include <bimg/decode.h>
#include "nanovg/nanovg.h"
#include <cassert>
#include <napi/pointer.h>
#include <basen.hpp>

namespace Babylon::Polyfills::Internal
{
    static constexpr auto JS_IMAGE_CONSTRUCTOR_NAME = "Image";

    void NativeCanvasImage::Initialize(Napi::Env env)
    {
        Napi::HandleScope scope{env};

        Napi::Function func = DefineClass(
            env,
            JS_IMAGE_CONSTRUCTOR_NAME,
            {
                InstanceAccessor("width", &NativeCanvasImage::GetWidth, nullptr),
                InstanceAccessor("height", &NativeCanvasImage::GetHeight, nullptr),
                InstanceAccessor("naturalWidth", &NativeCanvasImage::GetNaturalWidth, nullptr),
                InstanceAccessor("naturalHeight", &NativeCanvasImage::GetNaturalHeight, nullptr),
                InstanceAccessor("src", &NativeCanvasImage::GetSrc, &NativeCanvasImage::SetSrc),
                InstanceAccessor("onload", nullptr, &NativeCanvasImage::SetOnload),
                InstanceAccessor("onerror", nullptr, &NativeCanvasImage::SetOnerror),
                // TODO: This should be set directly on the JS Object rather than via an instanceAccessor see: https://github.com/BabylonJS/BabylonNative/issues/1030
                InstanceAccessor("_imageContainer", &NativeCanvasImage::GetImageContainer, nullptr),
            });

        JsRuntime::NativeObject::GetFromJavaScript(env).Set(JS_IMAGE_CONSTRUCTOR_NAME, func);
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

    Napi::Value NativeCanvasImage::GetImageContainer(const Napi::CallbackInfo&)
    {
        if (m_imageContainer != nullptr)
        {
            return Napi::Pointer<bimg::ImageContainer>::Create(Env(), m_imageContainer);
        }
        else
        {
            return Env().Null();
        }
    }

    bool NativeCanvasImage::SetBuffer(gsl::span<const std::byte> buffer)
    {
        m_imageContainer = bimg::imageParse(&Graphics::DeviceContext::GetDefaultAllocator(), buffer.data(), static_cast<uint32_t>(buffer.size_bytes()), bimg::TextureFormat::RGBA8);

        if (m_imageContainer == nullptr)
        {
            return false;
        }

        m_width = m_imageContainer->m_width;
        m_height = m_imageContainer->m_height;

        if (!m_onloadHandlerRef.IsEmpty())
        {
            m_onloadHandlerRef.Call({});
        }
        return true;
    }

    void NativeCanvasImage::SetSrc(const Napi::CallbackInfo& info, const Napi::Value& value)
    {
        auto text{value.As<Napi::String>().Utf8Value()};

        // try with base64
        static const std::string base64{"base64,"};
        const auto pos = text.find(base64);
        if (pos != std::string::npos)
        {
            arcana::make_task(m_runtimeScheduler, *m_cancellationSource, [env{info.Env()}, this, text{std::move(text)}, pos]() {
                std::vector<uint8_t> base64Buffer;
                bn::decode_b64(text.begin() + pos + base64.length(), text.end(), std::back_inserter(base64Buffer));
                gsl::span<const std::byte> buffer = {reinterpret_cast<std::byte*>(base64Buffer.data()), base64Buffer.size()};

                if (!SetBuffer(buffer))
                {
                    HandleLoadImageError(Napi::Error::New(env, "Unable to decode image with provided base64 source."));
                }
            });
            return;
        }

        // try with URL
        UrlLib::UrlRequest request{};
        request.Open(UrlLib::UrlMethod::Get, text);
        request.ResponseType(UrlLib::UrlResponseType::Buffer);
        request.SendAsync().then(m_runtimeScheduler, *m_cancellationSource, [env{info.Env()}, this, cancellationSource{m_cancellationSource}, request{std::move(request)}, text](arcana::expected<void, std::exception_ptr> result) {
            if (result.has_error())
            {
                HandleLoadImageError(Napi::Error::New(env, result.error()));
                return;
            }

            Dispose();

            auto buffer{request.ResponseBuffer()};
            if (buffer.data() == nullptr || buffer.size_bytes() == 0)
            {
                HandleLoadImageError(Napi::Error::New(env, "Image with provided source returned empty response or invalid base64."));
                return;
            }

            if (!SetBuffer(buffer))
            {
                HandleLoadImageError(Napi::Error::New(env, "Unable to decode image with provided source URL."));
            }
        });
    }

    void NativeCanvasImage::SetOnload(const Napi::CallbackInfo&, const Napi::Value& value)
    {
        Napi::Function eventHandler{value.As<Napi::Function>()};
        m_onloadHandlerRef = Napi::Persistent(eventHandler);
    }

    void NativeCanvasImage::SetOnerror(const Napi::CallbackInfo&, const Napi::Value& value)
    {
        Napi::Function eventHandler{value.As<Napi::Function>()};
        m_onerrorHandlerRef = Napi::Persistent(eventHandler);
    }

    int NativeCanvasImage::CreateNVGImageForContext(NVGcontext* nvgContext) const
    {
        return nvgCreateImageRGBA(nvgContext, m_width, m_height, 0, static_cast<const unsigned char*>(m_imageContainer->m_data));
    }

    void NativeCanvasImage::HandleLoadImageError(const Napi::Error& error)
    {
        if (!m_onerrorHandlerRef.IsEmpty())
        {
            m_onerrorHandlerRef.Call({error.Value()});
            return;
        }

        error.ThrowAsJavaScriptException();
    }
}
