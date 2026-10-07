#include <bgfx/bgfx.h>
#include <map>
#include "Canvas.h"
#include "Image.h"
#include "Context.h"
#include "NativeInstanceRegistry.h"
#include <functional>
#include <sstream>
#include <assert.h>
#include <vector>
#ifdef BABYLON_NATIVE_PLUGIN_NATIVEENGINE_LOAD_IMAGES
#include <Babylon/Graphics/ImageFormat.h>
#include <bimg/bimg.h>
#include <bimg/decode.h>
#endif
#include "nanovg/nanovg.h"
#include <cassert>
#include <stdexcept>
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
                // TODO: This should be set directly on the JS Object rather than via an instanceAccessor see: https://github.com/BabylonJS/BabylonNative/issues/1030
                InstanceAccessor("_imageContainer", &NativeCanvasImage::GetImageContainer, nullptr),
            });

        JsRuntime::NativeObject::GetFromJavaScript(env).Set(JS_IMAGE_CONSTRUCTOR_NAME, func);
    }

    NativeCanvasImage* NativeCanvasImage::TryUnwrap(Napi::Env env, const Napi::Value& value)
    {
        return NativeInstanceRegistry<NativeCanvasImage>::TryUnwrap(env, value);
    }

    NativeCanvasImage::NativeCanvasImage(const Napi::CallbackInfo& info)
        : Napi::ObjectWrap<NativeCanvasImage>{info}
        , m_runtimeScheduler{JsRuntime::GetFromJavaScript(info.Env())}
        , m_cancellationSource{std::make_shared<arcana::cancellation_source>()}
    {
        // Keep callback cycles visible to the JavaScript garbage collector.
        auto self = info.This().As<Napi::Object>();
        self.Set("onload", info.Env().Null());
        self.Set("onerror", info.Env().Null());
        // Register after successful construction only.
        NativeInstanceRegistry<NativeCanvasImage>::Add(info, this);
    }

    NativeCanvasImage::~NativeCanvasImage()
    {
        NativeInstanceRegistry<NativeCanvasImage>::Remove(this);
        Dispose();
    }

    void NativeCanvasImage::Dispose()
    {
        ReleaseImage();
        m_cancellationSource->cancel();
    }

    void NativeCanvasImage::ReleaseImage()
    {
#ifdef BABYLON_NATIVE_PLUGIN_NATIVEENGINE_LOAD_IMAGES
        if (m_imageContainer)
        {
            bimg::imageFree(m_imageContainer);
            m_imageContainer = nullptr;
        }
#endif
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
#ifdef BABYLON_NATIVE_PLUGIN_NATIVEENGINE_LOAD_IMAGES
        if (m_imageContainer != nullptr)
        {
            return Napi::Pointer<bimg::ImageContainer>::Create(Env(), m_imageContainer);
        }
#endif
        return Env().Null();
    }

    bool NativeCanvasImage::SetBuffer(const Napi::Object& self, gsl::span<const std::byte> buffer)
    {
#ifdef BABYLON_NATIVE_PLUGIN_NATIVEENGINE_LOAD_IMAGES
        ReleaseImage();
        auto& allocator = Graphics::DeviceContext::GetDefaultAllocator();
        m_imageContainer = bimg::imageParse(&allocator, buffer.data(), static_cast<uint32_t>(buffer.size_bytes()));
        if (m_imageContainer != nullptr)
        {
            m_imageContainer = Graphics::NormalizePngImage(allocator, m_imageContainer);
            if (m_imageContainer != nullptr && m_imageContainer->m_format != bimg::TextureFormat::RGBA8)
            {
                auto* source = m_imageContainer;
                m_imageContainer = bimg::imageConvert(&allocator, bimg::TextureFormat::RGBA8, *source);
                bimg::imageFree(source);
            }
        }
        if (m_imageContainer == nullptr)
        {
            return false;
        }

        m_width = m_imageContainer->m_width;
        m_height = m_imageContainer->m_height;
        // Bump before onload. A draw in that callback must not reuse the previous texture.
        ++m_contentGeneration;

        const auto onload = self.Get("onload");
        if (onload.IsFunction())
        {
            onload.As<Napi::Function>().Call(self, {});
        }
        return true;
#else
        (void)self;
        (void)buffer;
        return false;
#endif
    }

    void NativeCanvasImage::SetSrc(const Napi::CallbackInfo& info, const Napi::Value& value)
    {
#ifndef BABYLON_NATIVE_PLUGIN_NATIVEENGINE_LOAD_IMAGES
        (void)value;
        HandleLoadImageError(info.This().As<Napi::Object>(), Napi::Error::New(info.Env(), "Image loading is disabled in this build (BABYLON_NATIVE_PLUGIN_NATIVEENGINE_LOAD_IMAGES=OFF)."));
        return;
#else
        auto text{value.As<Napi::String>().Utf8Value()};
        m_src = text;
        m_cancellationSource->cancel();
        m_cancellationSource = std::make_shared<arcana::cancellation_source>();

        // try with base64
        static const std::string base64{"base64,"};
        const auto pos = text.find(base64);
        if (pos != std::string::npos)
        {
            // SetSrc, disposal, decoding and event delivery share the JS runtime thread;
            // cancellation cannot interleave with this synchronous decode.
            arcana::make_task(m_runtimeScheduler, *m_cancellationSource, [image{Napi::Persistent(info.This().As<Napi::Object>())}, this, cancellationSource{m_cancellationSource}, text{std::move(text)}, pos]() {
                if (cancellationSource->cancelled())
                {
                    return;
                }
                std::vector<uint8_t> base64Buffer;
                bn::decode_b64(text.begin() + pos + base64.length(), text.end(), std::back_inserter(base64Buffer));
                gsl::span<const std::byte> buffer = {reinterpret_cast<std::byte*>(base64Buffer.data()), base64Buffer.size()};

                const auto self = image.Value();
                if (!SetBuffer(self, buffer))
                {
                    HandleLoadImageError(self, Napi::Error::New(image.Env(), "Unable to decode image with provided base64 source."));
                }
            });
            return;
        }

        // try with URL
        UrlLib::UrlRequest request{};
        request.Open(UrlLib::UrlMethod::Get, text);
        request.ResponseType(UrlLib::UrlResponseType::Buffer);
        request.SendAsync().then(m_runtimeScheduler, *m_cancellationSource, [image{Napi::Persistent(info.This().As<Napi::Object>())}, this, cancellationSource{m_cancellationSource}, request{std::move(request)}](arcana::expected<void, std::exception_ptr> result) {
            if (cancellationSource->cancelled())
            {
                return;
            }
            const auto self = image.Value();
            if (result.has_error())
            {
                HandleLoadImageError(self, Napi::Error::New(image.Env(), result.error()));
                return;
            }

            auto buffer{request.ResponseBuffer()};
            if (buffer.data() == nullptr || buffer.size_bytes() == 0)
            {
                HandleLoadImageError(self, Napi::Error::New(image.Env(), "Image with provided source returned empty response or invalid base64."));
                return;
            }

            if (!SetBuffer(self, buffer))
            {
                HandleLoadImageError(self, Napi::Error::New(image.Env(), "Unable to decode image with provided source URL."));
            }
        });
#endif
    }

    int NativeCanvasImage::CreateNVGImageForContext(NVGcontext* nvgContext) const
    {
#ifdef BABYLON_NATIVE_PLUGIN_NATIVEENGINE_LOAD_IMAGES
        return nvgCreateImageRGBA(nvgContext, m_width, m_height, 0, static_cast<const unsigned char*>(m_imageContainer->m_data));
#else
        (void)nvgContext;
        throw std::runtime_error{"Image loading is disabled in this build (BABYLON_NATIVE_PLUGIN_NATIVEENGINE_LOAD_IMAGES=OFF)."};
#endif
    }

    void NativeCanvasImage::HandleLoadImageError(const Napi::Object& self, const Napi::Error& error)
    {
        // Match HTML <img>: fire onerror when set; otherwise fail silently.
        // Throwing here made GUI image tests flaky (async decode after/during ready).
        const auto onerror = self.Get("onerror");
        if (onerror.IsFunction())
        {
            onerror.As<Napi::Function>().Call(self, {error.Value()});
        }
    }
}
