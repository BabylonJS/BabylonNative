#include <bgfx/bgfx.h>
#include <map>
#include "Canvas.h"
#include "Path2D.h"

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#endif

#include "nanovg/nanovg.h"

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

namespace Babylon::Polyfills::Internal
{
    static constexpr auto JS_PATH2D_CONSTRUCTOR_NAME = "Path2D";

    Napi::Value Path2D::CreateInstance(Napi::Env env, Napi::String path)
    // Napi::Value Path2D::CreateInstance(Napi::Env env, Path2D* path2d) // TODO: Path2D constructor
    {
        Napi::HandleScope scope{env};
        Napi::Function func = DefineClass(
            env,
            JS_PATH2D_CONSTRUCTOR_NAME,
            {
                InstanceMethod("addPath", &Path2D::AddPath),
                InstanceMethod("closePath", &Path2D::ClosePath),
                InstanceMethod("moveTo", &Path2D::MoveTo),
                InstanceMethod("lineTo", &Path2D::LineTo),
                InstanceMethod("bezierCurveTo", &Path2D::BezierCurveTo),
                InstanceMethod("quadraticCurveTo", &Path2D::QuadraticCurveTo),
                InstanceMethod("arc", &Path2D::Arc),
                InstanceMethod("arcTo", &Path2D::ArcTo),
                InstanceMethod("ellipse", &Path2D::Ellipse),
                InstanceMethod("rect", &Path2D::Rect),
                InstanceMethod("roundRect", &Path2D::RoundRect),
            });
        return func.New({Napi::Value::From(env, path)});
        // return func.New({Napi::External<Path2D>::New(env, path2d)}); // TODO: Path2D constructor
    }

    Path2D::Path2D(const Napi::CallbackInfo& info)
        : Napi::ObjectWrap<Path2D>{info}
    {
        auto path{info[0].As<Napi::String>().ToString()};
        // auto context{info[0].As<Napi::External<Path2D>>().Data()}; // TODO: Path2D constructor
        // TODO: Convert path to nsvg object
    }

    void Path2D::AddPath(const Napi::CallbackInfo& info)
    {
        // auto path = info[0].As<Napi::Object?> // TODO: get Path2D object
        throw Napi::Error::New(info.Env(), "not implemented");
    }

    void Path2D::ClosePath(const Napi::CallbackInfo& info)
    {
        throw Napi::Error::New(info.Env(), "not implemented");
    }

    void Path2D::MoveTo(const Napi::CallbackInfo& info)
    {
        throw Napi::Error::New(info.Env(), "not implemented");
    }

	void Path2D::LineTo(const Napi::CallbackInfo& info)
    {
        throw Napi::Error::New(info.Env(), "not implemented");
    }

	void Path2D::BezierCurveTo(const Napi::CallbackInfo& info)
    {
        throw Napi::Error::New(info.Env(), "not implemented");
    }

	void Path2D::QuadraticCurveTo(const Napi::CallbackInfo& info)
    {
        throw Napi::Error::New(info.Env(), "not implemented");
    }

	void Path2D::Arc(const Napi::CallbackInfo& info)
    {
        throw Napi::Error::New(info.Env(), "not implemented");
    }

	void Path2D::ArcTo(const Napi::CallbackInfo& info)
    {
        throw Napi::Error::New(info.Env(), "not implemented");
    }

	void Path2D::Ellipse(const Napi::CallbackInfo& info)
    {
        throw Napi::Error::New(info.Env(), "not implemented");
    }

	void Path2D::Rect(const Napi::CallbackInfo& info)
    {
        throw Napi::Error::New(info.Env(), "not implemented");
    }

	void Path2D::RoundRect(const Napi::CallbackInfo& info)
    {
        throw Napi::Error::New(info.Env(), "not implemented");
    }
}
