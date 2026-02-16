#include "Canvas.h"
#include "Image.h"
#include "Path2D.h"
#include "Context.h"
#include "Colors.h"
#include "Gradient.h"

#include <algorithm>
#include <cstdint>

namespace
{
    constexpr auto JS_CANVAS_NAME = "_CanvasImpl";

    struct CanvasNativeTextureHandle final
    {
        const void* texture{};
        const void* device{};
        const void* queue{};
        uint32_t width{};
        uint32_t height{};
        uint64_t generation{};
    };
}

namespace Babylon::Polyfills::Internal
{
    static constexpr auto JS_CONSTRUCTOR_NAME = "Canvas";

    void NativeCanvas::Initialize(Napi::Env env)
    {
        Napi::HandleScope scope{env};

        Napi::Function func = DefineClass(
            env,
            JS_CONSTRUCTOR_NAME,
            {
                StaticMethod("loadTTF", &NativeCanvas::LoadTTF),
                StaticMethod("loadTTFAsync", &NativeCanvas::LoadTTFAsync),
                InstanceAccessor("width", &NativeCanvas::GetWidth, &NativeCanvas::SetWidth),
                InstanceAccessor("height", &NativeCanvas::GetHeight, &NativeCanvas::SetHeight),
                InstanceMethod("getContext", &NativeCanvas::GetContext),
                InstanceMethod("getCanvasTexture", &NativeCanvas::GetCanvasTexture),
                InstanceMethod("dispose", &NativeCanvas::Dispose),
                InstanceMethod("destroy", &NativeCanvas::Dispose),
                InstanceMethod("remove", &NativeCanvas::Remove),
                StaticMethod("parseColor", &NativeCanvas::ParseColor)});

        JsRuntime::NativeObject::GetFromJavaScript(env).Set(JS_CONSTRUCTOR_NAME, func);
    }

    NativeCanvas::NativeCanvas(const Napi::CallbackInfo& info)
        : Napi::ObjectWrap<NativeCanvas>{info}
        , Polyfills::Canvas::Impl::MonitoredResource{Polyfills::Canvas::Impl::GetFromJavaScript(info.Env())}
    {
    }

    NativeCanvas::~NativeCanvas()
    {
        Dispose();
    }

    void NativeCanvas::FlushGraphicResources()
    {
        Dispose();
    }

    void NativeCanvas::Remove(const Napi::CallbackInfo&)
    {
        // called when removed from document which has no meaning for Native
    }

    void NativeCanvas::LoadTTF(const Napi::CallbackInfo& info)
    {
        // don't allow same font to be loaded more than once
        // why? because Context doesn't update nvgCreateFontMem when old fontBuffer released
        auto fontName = info[0].As<Napi::String>().Utf8Value();
        if (fontsInfos.find(fontName) == fontsInfos.end())
        {
            const auto buffer = info[1].As<Napi::ArrayBuffer>();
            std::vector<uint8_t> fontBuffer(buffer.ByteLength());
            memcpy(fontBuffer.data(), static_cast<uint8_t*>(buffer.Data()), buffer.ByteLength());
            fontsInfos[fontName] = std::move(fontBuffer);
        }
    }

    // @deprecated: LoadTTFAsync is always synchronous, use LoadTTF instead
    Napi::Value NativeCanvas::LoadTTFAsync(const Napi::CallbackInfo& info)
    {
        LoadTTF(info);

        auto deferred{Napi::Promise::Deferred::New(info.Env())};
        deferred.Resolve(info.Env().Undefined());
        return deferred.Promise();
    }

    Napi::Value NativeCanvas::GetContext(const Napi::CallbackInfo& info)
    {
        auto thisObj = info.This().ToObject();
        const auto contextPropertyName = Napi::Value::From(Env(), "_context");

        auto context = thisObj.Get(contextPropertyName);
        if (context.IsUndefined())
        {
            context = Context::CreateInstance(info.Env(), info.This());
            thisObj.Set(contextPropertyName, context);
            m_contextObject = Napi::Persistent(context.As<Napi::Object>());
        }
        else if (m_contextObject.IsEmpty() && context.IsObject())
        {
            m_contextObject = Napi::Persistent(context.As<Napi::Object>());
        }

        return context;
    }

    Napi::Value NativeCanvas::GetWidth(const Napi::CallbackInfo&)
    {
        return Napi::Value::From(Env(), m_width);
    }

    void NativeCanvas::SetWidth(const Napi::CallbackInfo&, const Napi::Value& value)
    {
        auto width = static_cast<uint16_t>(value.As<Napi::Number>().Uint32Value());
        if (!width)
        {
            return;
        }

        if (width == m_width)
        {
            m_clear = true;
        }
        else
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
        auto height = static_cast<uint16_t>(value.As<Napi::Number>().Uint32Value());
        if (!height)
        {
            return;
        }

        if (height == m_height)
        {
            m_clear = true;
        }
        else
        {
            m_height = height;
            m_dirty = true;
        }
    }

    bool NativeCanvas::UpdateRenderTarget()
    {
        bool needClear = m_clear || m_dirty;
        m_clear = false;
        m_dirty = false;
        return needClear;
    }

    Napi::Value NativeCanvas::GetCanvasTexture(const Napi::CallbackInfo& info)
    {
        // This payload is consumed by NativeWebGPU's implementation of
        // `GPUQueue.copyExternalImageToTexture({ source: canvas }, ...)`.
        auto contextValue = GetContext(info);
        if (!contextValue.IsObject())
        {
            return Env().Null();
        }

        auto* context = Context::Unwrap(contextValue.As<Napi::Object>());
        if (context == nullptr)
        {
            return Env().Null();
        }

        void* nativeTexture = context->GetNativeRenderTexture();
        if (nativeTexture == nullptr)
        {
            return Env().Null();
        }

        Napi::Object payload{};
        if (m_canvasTexturePayload.IsEmpty())
        {
            payload = Napi::Object::New(info.Env());
            m_canvasTexturePayload = Napi::Persistent(payload);
        }
        else
        {
            payload = m_canvasTexturePayload.Value();
        }

        const auto* nativeHandle = reinterpret_cast<const CanvasNativeTextureHandle*>(nativeTexture);
        if (m_canvasTextureNativeHandle != nativeTexture)
        {
            payload.Set("nativeTexture", Napi::External<void>::New(info.Env(), nativeTexture));
            m_canvasTextureNativeHandle = nativeTexture;
        }

        const auto payloadWidth = static_cast<uint16_t>(std::max<uint32_t>(1, nativeHandle->width));
        const auto payloadHeight = static_cast<uint16_t>(std::max<uint32_t>(1, nativeHandle->height));

        if (m_canvasTexturePayloadWidth != payloadWidth)
        {
            payload.Set("width", Napi::Number::From(info.Env(), payloadWidth));
            m_canvasTexturePayloadWidth = payloadWidth;
        }

        if (m_canvasTexturePayloadHeight != payloadHeight)
        {
            payload.Set("height", Napi::Number::From(info.Env(), payloadHeight));
            m_canvasTexturePayloadHeight = payloadHeight;
        }

        if (m_canvasTexturePayloadGeneration != nativeHandle->generation)
        {
            payload.Set("generation", Napi::Number::From(info.Env(), static_cast<double>(nativeHandle->generation)));
            m_canvasTexturePayloadGeneration = nativeHandle->generation;
        }

        return payload;
    }

    Napi::Value NativeCanvas::ParseColor(const Napi::CallbackInfo& info)
    {
        const auto colorString = info[0].As<Napi::String>().Utf8Value();
        const auto color = StringToColor(info.Env(), colorString);

        return Napi::Value::From(info.Env(), ((uint32_t(color.a * 255.f) & 0xFF) << 24) + ((uint32_t(color.b * 255.f) & 0xFF) << 16) + ((uint32_t(color.g * 255.f) & 0xFF) << 8) + (uint32_t(color.r * 255.f) & 0xFF));
    }

    void NativeCanvas::Dispose()
    {
        if (!m_contextObject.IsEmpty())
        {
            auto contextObject = m_contextObject.Value();
            auto disposeValue = contextObject.Get("dispose");
            if (disposeValue.IsFunction())
            {
                disposeValue.As<Napi::Function>().Call(contextObject, {});
            }
            m_contextObject.Reset();
        }

        auto thisObject = Value();
        if (!thisObject.IsEmpty())
        {
            thisObject.Delete("_context");
        }

        if (!m_canvasTexturePayload.IsEmpty())
        {
            m_canvasTexturePayload.Reset();
        }
        m_canvasTextureNativeHandle = nullptr;
        m_canvasTexturePayloadWidth = 0;
        m_canvasTexturePayloadHeight = 0;
        m_canvasTexturePayloadGeneration = 0;
    }

    void NativeCanvas::Dispose(const Napi::CallbackInfo& /*info*/)
    {
        Dispose();
    }
}

namespace Babylon::Polyfills
{
    Canvas::Impl::Impl(Napi::Env env)
        : m_env{env}
    {
        AddToJavaScript(env);
    }

    void Canvas::Impl::AddToJavaScript(Napi::Env env)
    {
        JsRuntime::NativeObject::GetFromJavaScript(env)
            .Set(JS_CANVAS_NAME, Napi::External<Canvas::Impl>::New(env, this));
    }

    Canvas::Impl& Canvas::Impl::GetFromJavaScript(Napi::Env env)
    {
        return *JsRuntime::NativeObject::GetFromJavaScript(env)
                    .Get(JS_CANVAS_NAME)
                    .As<Napi::External<Canvas::Impl>>()
                    .Data();
    }

    void Canvas::Impl::AddMonitoredResource(MonitoredResource* monitoredResource)
    {
        if (std::find(m_monitoredResources.begin(), m_monitoredResources.end(), monitoredResource) == m_monitoredResources.end())
        {
            m_monitoredResources.push_back(monitoredResource);
        }
    }

    void Canvas::Impl::RemoveMonitoredResource(MonitoredResource* monitoredResource)
    {
        if (m_monitoredResources.empty())
        {
            return;
        }
        auto iter = std::find(m_monitoredResources.begin(), m_monitoredResources.end(), monitoredResource);
        if (iter != m_monitoredResources.end())
        {
            m_monitoredResources.erase(iter);
        }
    }

    void Canvas::Impl::FlushGraphicResources()
    {
        for (auto monitoredResource : m_monitoredResources)
        {
            monitoredResource->FlushGraphicResources();
        }
    }

    Canvas::Canvas(std::shared_ptr<Impl> impl)
        : m_impl{std::move(impl)}
    {
    }

    Canvas::~Canvas()
    {
    }

    // Initialization contract: same as NativeWebGPU::Initialize — must be
    // called from an AppRuntime::Dispatch callback before any JS runs.
    // The FIFO WorkQueue guarantees _native.Canvas is synchronously available
    // to scripts loaded via ScriptLoader. Embedders do NOT need to poll.
    Canvas BABYLON_API Canvas::Initialize(Napi::Env env)
    {
        auto impl{std::make_shared<Canvas::Impl>(env)};

        Internal::NativeCanvas::Initialize(env);
        Internal::NativeCanvasImage::Initialize(env);
        Internal::NativeCanvasPath2D::Initialize(env);
        Internal::CanvasGradient::Initialize(env);
        Internal::Context::Initialize(env);

        return {impl};
    }

    void Canvas::FlushGraphicResources()
    {
        m_impl->FlushGraphicResources();
    }
}
