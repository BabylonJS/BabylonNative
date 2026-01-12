#include <bgfx/bgfx.h>
#include <cassert>
#include <map>
#include "Canvas.h"
#include "Path2D.h"
#include <napi/pointer.h>

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#endif

#define NANOSVG_IMPLEMENTATION // Expands implementation
#include "nanosvg.h"

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

namespace Babylon::Polyfills::Internal
{
    static constexpr auto JS_PATH2D_CONSTRUCTOR_NAME = "Path2D";

    void NativeCanvasPath2D::Initialize(Napi::Env env)
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
        , m_commands{std::deque<Path2DCommand>()}
    {
        const NativeCanvasPath2D* path = info.Length() == 1 && info[0].IsObject()
            ? NativeCanvasPath2D::Unwrap(info[0].As<Napi::Object>())
            : nullptr;
        const std::string d = info.Length() == 1 && info[0].IsString() ? info[0].As<Napi::String>().Utf8Value() : "";

        if (path != nullptr)
        {
            for (const auto& command : *path)
            {
                m_commands.push_back(command);
            }
        }

        if (!d.empty())
        {
            NSVGparser* parser = nsvg__createParser();
            const char* path[] = {"d", d.c_str(), NULL};
            const char** attr = {path};

            assert(strcmp(attr[0], "d") == 0);
            assert(!attr[2]); // nsvg__parsePath terminates attr parsing on falsy

            nsvg__parsePath(parser, attr);

            for (NSVGshape *shape = parser->image->shapes; shape != NULL; shape = shape->next) {
                for (NSVGpath *path = shape->paths; path != NULL; path = path->next) {
                    for (int i = 0; i < path->npts-1; i += 3) {
                        float* p = &path->pts[i*2];

                        auto x0 = p[0]; // start x, same as end x of previous
                        auto y0 = p[1]; // start y, same as end y of previous
                        auto cpx1 = p[2];
                        auto cpy1 = p[3];
                        auto cpx2 = p[4];
                        auto cpy2 = p[5];
                        auto x1 = p[6]; // end x
                        auto y1 = p[7]; // end y

                        // Only need to move on new shape
                        if (i == 0)
                        {
                            Path2DCommandArgs moveArgs = {};
                            moveArgs.moveTo = {x0, y0};
                            AppendCommand(P2D_MOVETO, moveArgs);
                        }

                        Path2DCommandArgs args = {};
                        args.bezierTo = {cpx1, cpy1, cpx2, cpy2, x1, y1};
                        AppendCommand(P2D_BEZIERTO, args);
                    }
                }
            }

            nsvg__deleteParser(parser);
        }
    }

    typename std::deque<Path2DCommand>::iterator NativeCanvasPath2D::begin()
    {
        return m_commands.begin();
    }

    typename std::deque<Path2DCommand>::iterator NativeCanvasPath2D::end()
    {
        return m_commands.end();
    }

    typename std::deque<Path2DCommand>::const_iterator NativeCanvasPath2D::begin() const
    {
        return m_commands.begin();
    }

    typename std::deque<Path2DCommand>::const_iterator NativeCanvasPath2D::end() const
    {
        return m_commands.end();
    }

    void NativeCanvasPath2D::AppendCommand(Path2DCommandTypes type, Path2DCommandArgs args)
    {
        m_commands.push_back({type, args});
    }

    void NativeCanvasPath2D::AddPath(const Napi::CallbackInfo& info)
    {
        const NativeCanvasPath2D* path = NativeCanvasPath2D::Unwrap(info[0].As<Napi::Object>());

        // optional transform arg
        bool xformInvReady{false};
        float xformInv[6];
        if (info.Length() == 2)
        {
            Napi::Object transform = info[1].As<Napi::Object>();
            auto a = transform.Get("a").As<Napi::Number>().FloatValue();
            auto b = transform.Get("b").As<Napi::Number>().FloatValue();
            auto c = transform.Get("c").As<Napi::Number>().FloatValue();
            auto d = transform.Get("d").As<Napi::Number>().FloatValue();
            auto e = transform.Get("e").As<Napi::Number>().FloatValue();
            auto f = transform.Get("f").As<Napi::Number>().FloatValue();

            float xform[6] = {a, b, c, d, e, f};
            nsvg__xformInverse(xformInv, xform);
            xformInvReady = true;

            Path2DCommandArgs args = {};
            args.transform = { xform[0], xform[1], xform[2], xform[3], xform[4], xform[5] };
            AppendCommand(P2D_TRANSFORM, args);
        }

        for (const auto& command : *path)
        {
            m_commands.push_back(command);
        }

        // invert transform after all commands played
        if (info.Length() == 2)
        {
            assert(xformInvReady);
            Path2DCommandArgs argsInv = {};
            argsInv.transform = {xformInv[0], xformInv[1], xformInv[2], xformInv[3], xformInv[4], xformInv[5]};
            AppendCommand(P2D_TRANSFORM, argsInv);
        }
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
        const auto cp1x = static_cast<float>(info[0].As<Napi::Number>().DoubleValue());
        const auto cp1y = static_cast<float>(info[1].As<Napi::Number>().DoubleValue());
        const auto cp2x = static_cast<float>(info[2].As<Napi::Number>().DoubleValue());
        const auto cp2y = static_cast<float>(info[3].As<Napi::Number>().DoubleValue());
        const auto x = static_cast<float>(info[4].As<Napi::Number>().DoubleValue());
        const auto y = static_cast<float>(info[5].As<Napi::Number>().DoubleValue());

        Path2DCommandArgs args = {};
        args.bezierTo = {cp1x, cp1y, cp2x, cp2y, x, y};
        AppendCommand(P2D_BEZIERTO, args);
    }

    void NativeCanvasPath2D::QuadraticCurveTo(const Napi::CallbackInfo& info)
    {
        const auto cpx = static_cast<float>(info[0].As<Napi::Number>().DoubleValue());
        const auto cpy = static_cast<float>(info[1].As<Napi::Number>().DoubleValue());
        const auto x = static_cast<float>(info[2].As<Napi::Number>().DoubleValue());
        const auto y = static_cast<float>(info[3].As<Napi::Number>().DoubleValue());

        Path2DCommandArgs args = {};
        args.quadTo = {cpx, cpy, x, y};
        AppendCommand(P2D_QUADTO, args);
    }

    void NativeCanvasPath2D::Arc(const Napi::CallbackInfo& info)
    {
        const auto x = static_cast<float>(info[0].As<Napi::Number>().DoubleValue());
        const auto y = static_cast<float>(info[1].As<Napi::Number>().DoubleValue());
        const auto radius = static_cast<float>(info[2].As<Napi::Number>().DoubleValue());
        const auto startAngle = static_cast<float>(info[3].As<Napi::Number>().DoubleValue());
        const auto endAngle = static_cast<float>(info[4].As<Napi::Number>().DoubleValue());
        const auto counterclockwise = info.Length() == 6 ? info[5].As<Napi::Boolean>() : false;

        Path2DCommandArgs args = {};
        args.arc = {x, y, radius, startAngle, endAngle, counterclockwise};
        AppendCommand(P2D_ARC, args);
    }

    void NativeCanvasPath2D::ArcTo(const Napi::CallbackInfo& info)
    {
        const auto x1 = static_cast<float>(info[0].As<Napi::Number>().DoubleValue());
        const auto y1 = static_cast<float>(info[1].As<Napi::Number>().DoubleValue());
        const auto x2 = static_cast<float>(info[2].As<Napi::Number>().DoubleValue());
        const auto y2 = static_cast<float>(info[3].As<Napi::Number>().DoubleValue());
        const auto radius = static_cast<float>(info[4].As<Napi::Number>().DoubleValue());

        Path2DCommandArgs args = {};
        args.arcTo = {x1, y1, x2, y2, radius};
        AppendCommand(P2D_ARCTO, args);
    }

    void NativeCanvasPath2D::Ellipse(const Napi::CallbackInfo& info)
    {
        const auto x = static_cast<float>(info[0].As<Napi::Number>().DoubleValue());
        const auto y = static_cast<float>(info[1].As<Napi::Number>().DoubleValue());
        const auto radiusX = static_cast<float>(info[2].As<Napi::Number>().DoubleValue());
        const auto radiusY = static_cast<float>(info[3].As<Napi::Number>().DoubleValue());
        const auto rotation = static_cast<float>(info[4].As<Napi::Number>().DoubleValue());
        const auto startAngle = static_cast<float>(info[5].As<Napi::Number>().DoubleValue());
        const auto endAngle = static_cast<float>(info[6].As<Napi::Number>().DoubleValue());
        const auto counterclockwise = info.Length() == 8 ? info[7].As<Napi::Boolean>() : false;

        Path2DCommandArgs args = {};
        args.ellipse = {x, y, radiusX, radiusY, rotation, startAngle, endAngle, counterclockwise};
        AppendCommand(P2D_ELLIPSE, args);
    }

    void NativeCanvasPath2D::Rect(const Napi::CallbackInfo& info)
    {
        const auto x = static_cast<float>(info[0].As<Napi::Number>().DoubleValue());
        const auto y = static_cast<float>(info[1].As<Napi::Number>().DoubleValue());
        const auto width = static_cast<float>(info[2].As<Napi::Number>().DoubleValue());
        const auto height = static_cast<float>(info[3].As<Napi::Number>().DoubleValue());

        Path2DCommandArgs args = {};
        args.rect = {x, y, width, height};
        AppendCommand(P2D_RECT, args);
    }

    void NativeCanvasPath2D::RoundRect(const Napi::CallbackInfo& info)
    {
        const auto x = static_cast<float>(info[0].As<Napi::Number>().DoubleValue());
        const auto y = static_cast<float>(info[1].As<Napi::Number>().DoubleValue());
        const auto width = static_cast<float>(info[2].As<Napi::Number>().DoubleValue());
        const auto height = static_cast<float>(info[3].As<Napi::Number>().DoubleValue());
        const auto radii = info[4];

        if (radii.IsNumber())
        {
            const auto radius = radii.As<Napi::Number>().FloatValue();
            Path2DCommandArgs args = {};
            args.roundRect = {x, y, width, height, radius};
            AppendCommand(P2D_ROUNDRECT, args);
        }
        else if (radii.IsArray())
        {
            const auto radiiArray = radii.As<Napi::Array>();
            const auto radiiArrayLength = radiiArray.Length();
            if (radiiArrayLength == 1)
            {
                const auto radius = radiiArray[0u].As<Napi::Number>().FloatValue();
                Path2DCommandArgs args = {};
                args.roundRect = {x, y, width, height, radius};
                AppendCommand(P2D_ROUNDRECT, args);
            }
            else if (radiiArrayLength == 2)
            {
                const auto topLeftBottomRight = radiiArray[0u].As<Napi::Number>().FloatValue();
                const auto topRightBottomLeft = radiiArray[1u].As<Napi::Number>().FloatValue();
                Path2DCommandArgs args = {};
                args.roundRectVarying = {x, y, width, height, topLeftBottomRight, topRightBottomLeft, topLeftBottomRight, topRightBottomLeft};
                AppendCommand(P2D_ROUNDRECTVARYING, args);
            }
            else if (radiiArrayLength == 3)
            {
                const auto topLeft = radiiArray[0u].As<Napi::Number>().FloatValue();
                const auto topRightBottomLeft = radiiArray[1u].As<Napi::Number>().FloatValue();
                const auto bottomRight = radiiArray[2u].As<Napi::Number>().FloatValue();
                Path2DCommandArgs args = {};
                args.roundRectVarying = {x, y, width, height, topLeft, topRightBottomLeft, bottomRight, topRightBottomLeft};
                AppendCommand(P2D_ROUNDRECTVARYING, args);
            }
            else if (radiiArrayLength == 4)
            {
                const auto topLeft = radiiArray[0u].As<Napi::Number>().FloatValue();
                const auto topRight = radiiArray[1u].As<Napi::Number>().FloatValue();
                const auto bottomRight = radiiArray[2u].As<Napi::Number>().FloatValue();
                const auto bottomLeft = radiiArray[3u].As<Napi::Number>().FloatValue();
                Path2DCommandArgs args = {};
                args.roundRectVarying = {x, y, width, height, topLeft, topRight, bottomRight, bottomLeft};
                AppendCommand(P2D_ROUNDRECTVARYING, args);
            }
            else
            {
                throw Napi::Error::New(info.Env(), "Invalid number of parameters for radii");
            }
        }
        // DOMPoint
        // TODO: move duplicate Path2D & Context args parsing into a utils.cpp
        else if (radii.IsObject())
        {
            const auto dompoint = radii.As<Napi::Object>();
            const auto radiusX = dompoint.Get("x").As<Napi::Number>().FloatValue();
            const auto radiusY = dompoint.Get("y").As<Napi::Number>().FloatValue();
            Path2DCommandArgs args = {};
            args.roundRectElliptic = {x, y, width, height, radiusX, radiusY, radiusX, radiusY, radiusX, radiusY, radiusX, radiusY};
            AppendCommand(P2D_ROUNDRECTELLIPTIC, args);
        }
        else
        {
            throw Napi::Error::New(info.Env(), "Invalid radii parameter");
        }
    }
}
