#include <bgfx/bgfx.h>
#include <map>
#include "Canvas.h"
#include "Path2D.h"
#include "nanovg.h"
#include <napi/pointer.h>

namespace Babylon::Polyfills::Internal
{
    static constexpr auto JS_PATH2D_CONSTRUCTOR_NAME = "Path2D";

    void NativeCanvasPath2D::CreateInstance(Napi::Env env)
    {
        Napi::HandleScope scope{env};

        Napi::Function func = DefineClass(
            env,
            JS_PATH2D_CONSTRUCTOR_NAME,
            {
                InstanceMethod("addPath", &NativeCanvasPath2D::AddPath),
                InstanceMethod("closePath", &NativeCanvasPath2D::ClosePath),
                InstanceMethod("moveTo", &NativeCanvasPath2D::MoveTo),
                InstanceMethod("lineTo", &NativeCanvasPath2D::LineTo),
                InstanceMethod("bezierCurveTo", &NativeCanvasPath2D::BezierCurveTo),
                InstanceMethod("quadraticCurveTo", &NativeCanvasPath2D::QuadraticCurveTo),
                InstanceMethod("arc", &NativeCanvasPath2D::Arc),
                InstanceMethod("arcTo", &NativeCanvasPath2D::ArcTo),
                InstanceMethod("ellipse", &NativeCanvasPath2D::Ellipse),
                InstanceMethod("rect", &NativeCanvasPath2D::Rect),
                InstanceMethod("roundRect", &NativeCanvasPath2D::RoundRect),
            });

        JsRuntime::NativeObject::GetFromJavaScript(env).Set(JS_PATH2D_CONSTRUCTOR_NAME, func);
    }

    NativeCanvasPath2D::NativeCanvasPath2D(const Napi::CallbackInfo& info)
        : Napi::ObjectWrap<NativeCanvasPath2D>{info}
    {
        auto path{info[0].As<Napi::String>().ToString()};
        // auto context{info[0].As<Napi::External<NativeCanvasPath2D>>().Data()}; // TODO: Path2D constructor
        // TODO: Convert path to nsvg object
    }

    void NativeCanvasPath2D::AddPath(const Napi::CallbackInfo& info)
    {
        // auto path = info[0].As<Napi::Object?> // TODO: get NativeCanvasPath2D object
        throw Napi::Error::New(info.Env(), "not implemented");
    }

    void NativeCanvasPath2D::ClosePath(const Napi::CallbackInfo& info)
    {
        throw Napi::Error::New(info.Env(), "not implemented");
    }

    void NativeCanvasPath2D::MoveTo(const Napi::CallbackInfo& info)
    {
        throw Napi::Error::New(info.Env(), "not implemented");
    }

	void NativeCanvasPath2D::LineTo(const Napi::CallbackInfo& info)
    {
        throw Napi::Error::New(info.Env(), "not implemented");
    }

	void NativeCanvasPath2D::BezierCurveTo(const Napi::CallbackInfo& info)
    {
        throw Napi::Error::New(info.Env(), "not implemented");
    }

	void NativeCanvasPath2D::QuadraticCurveTo(const Napi::CallbackInfo& info)
    {
        throw Napi::Error::New(info.Env(), "not implemented");
    }

	void NativeCanvasPath2D::Arc(const Napi::CallbackInfo& info)
    {
        throw Napi::Error::New(info.Env(), "not implemented");
    }

	void NativeCanvasPath2D::ArcTo(const Napi::CallbackInfo& info)
    {
        throw Napi::Error::New(info.Env(), "not implemented");
    }

	void NativeCanvasPath2D::Ellipse(const Napi::CallbackInfo& info)
    {
        throw Napi::Error::New(info.Env(), "not implemented");
    }

	void NativeCanvasPath2D::Rect(const Napi::CallbackInfo& info)
    {
        throw Napi::Error::New(info.Env(), "not implemented");
    }

	void NativeCanvasPath2D::RoundRect(const Napi::CallbackInfo& info)
    {
        throw Napi::Error::New(info.Env(), "not implemented");
    }
}
