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
        , m_commands{std::queue<Path2DCommand>()}
    {
        auto path{info[0].As<Napi::String>().ToString()};
        // auto context{info[0].As<Napi::External<NativeCanvasPath2D>>().Data()}; // TODO: Path2D constructor
        // TODO: Convert path to list of commands to init queue
    }

    void NativeCanvasPath2D::AppendCommand(Path2DCommandTypes type, Path2DCommandArgs args)
    {
        m_commands.push({ type, args });
    }

    void NativeCanvasPath2D::AddPath(const Napi::CallbackInfo& info)
    {
        // NOTE: stroke works same as addpath, and
        const NativeCanvasPath2D* path = NativeCanvasPath2D::Unwrap(info[0].As<Napi::Object>());
        // const NativeCanvasDOMMatrix path = NativeCanvasDOMMatrix::Unwrap(info[1].As<Napi::Object>()); // TODO: DOMMatrix
        throw Napi::Error::New(info.Env(), "not implemented");
    }

    void NativeCanvasPath2D::ClosePath(const Napi::CallbackInfo& info)
    {
        AppendCommand(P2D_CLOSE, {});
    }

    void NativeCanvasPath2D::MoveTo(const Napi::CallbackInfo& info)
    {
        const auto x = static_cast<float>(info[0].As<Napi::Number>().DoubleValue());
        const auto y = static_cast<float>(info[1].As<Napi::Number>().DoubleValue());

        Path2DCommandArgs args = {};
        args.moveTo = {x, y};
        AppendCommand(P2D_MOVETO, args);
    }

	void NativeCanvasPath2D::LineTo(const Napi::CallbackInfo& info)
    {
        const auto x = static_cast<float>(info[0].As<Napi::Number>().DoubleValue());
        const auto y = static_cast<float>(info[1].As<Napi::Number>().DoubleValue());

        Path2DCommandArgs args = {};
        args.lineTo = {x, y};
        AppendCommand(P2D_LINETO, args);
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
