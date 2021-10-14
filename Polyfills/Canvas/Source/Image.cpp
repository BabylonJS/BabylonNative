#include <bgfx/bgfx.h>
#include <map>
#include "Canvas.h"
#include "Image.h"
#include "Context.h"
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
    {
    }

    NativeCanvasImage::~NativeCanvasImage()
    {
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

    void NativeCanvasImage::SetSrc(const Napi::CallbackInfo&, const Napi::Value& value)
    {
        auto text{value.As<Napi::String>().Utf8Value()};
        m_src = text;
    }

    void NativeCanvasImage::SetOnload(const Napi::CallbackInfo&, const Napi::Value&)
    {
    }
}
