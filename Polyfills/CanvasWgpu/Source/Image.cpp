#include <map>
#include "Canvas.h"
#include "Image.h"
#include "Context.h"
#include <functional>
#include <sstream>
#include <assert.h>
#include "nanovg/nanovg.h"
#include <cassert>
#include <napi/pointer.h>
#include <basen.hpp>
#include <cstdint>

extern "C"
{
    int32_t babylon_canvas_decode_image_rgba(
        const uint8_t* data,
        size_t len,
        uint32_t* out_width,
        uint32_t* out_height,
        uint8_t** out_rgba,
        size_t* out_len);
    void babylon_canvas_free_bytes(uint8_t* data, size_t len);
}

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
        m_rgbaData.clear();
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
        // CanvasWgpu does not expose a bgfx/bimg image container.
        return Env().Null();
    }

    bool NativeCanvasImage::SetBuffer(gsl::span<const std::byte> buffer)
    {
        const auto* encodedData = reinterpret_cast<const uint8_t*>(buffer.data());
        const auto encodedSize = buffer.size_bytes();

        uint32_t decodedWidth{};
        uint32_t decodedHeight{};
        uint8_t* decodedRgba{};
        size_t decodedLength{};
        const auto decodeSuccess = babylon_canvas_decode_image_rgba(
            encodedData,
            encodedSize,
            &decodedWidth,
            &decodedHeight,
            &decodedRgba,
            &decodedLength);

        if (!decodeSuccess || decodedRgba == nullptr || decodedWidth == 0 || decodedHeight == 0)
        {
            return false;
        }

        const auto expectedLength = static_cast<uint64_t>(decodedWidth) * static_cast<uint64_t>(decodedHeight) * 4ull;
        if (expectedLength != decodedLength)
        {
            babylon_canvas_free_bytes(decodedRgba, decodedLength);
            return false;
        }

        m_width = decodedWidth;
        m_height = decodedHeight;
        m_rgbaData.assign(decodedRgba, decodedRgba + decodedLength);
        babylon_canvas_free_bytes(decodedRgba, decodedLength);

        if (!m_onloadHandlerRef.IsEmpty())
        {
            m_onloadHandlerRef.Call({});
        }
        return true;
    }

    void NativeCanvasImage::SetSrc(const Napi::CallbackInfo& info, const Napi::Value& value)
    {
        auto text{value.As<Napi::String>().Utf8Value()};
        m_src = text;

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
        request.SendAsync().then(m_runtimeScheduler, *m_cancellationSource, [env{info.Env()}, this, cancellationSource{m_cancellationSource}, request{std::move(request)}](arcana::expected<void, std::exception_ptr> result) {
            if (result.has_error())
            {
                HandleLoadImageError(Napi::Error::New(env, result.error()));
                return;
            }

            m_rgbaData.clear();
            m_width = 1;
            m_height = 1;

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
        if (m_rgbaData.empty())
        {
            static constexpr unsigned char transparentPixel[4] = {0, 0, 0, 0};
            return nvgCreateImageRGBA(nvgContext, 1, 1, 0, transparentPixel);
        }

        return nvgCreateImageRGBA(nvgContext, static_cast<int>(m_width), static_cast<int>(m_height), 0, m_rgbaData.data());
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
