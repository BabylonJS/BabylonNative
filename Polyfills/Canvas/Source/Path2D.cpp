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

#include "nanovg.h"
#define NANOSVG_IMPLEMENTATION	// Expands implementation
#include "nanosvg.h"

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

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
        , m_commands{std::deque<Path2DCommand>()}
    {
        const std::string d = info.Length() == 1 ? info[0].As<Napi::String>().Utf8Value() : "";
        // auto context{info[0].As<Napi::External<NativeCanvasPath2D>>().Data()}; // TODO: Path2D constructor
        if (!d.empty())
        {
            NSVGparser* parser = nsvg__createParser();
            const char* path[] = {"d", d.c_str(), NULL};
            const char** attr = {path};

            assert(strcmp(attr[0], "d") == 0);
            assert(!attr[2]); // nsvg__parsePath terminates attr parsing on falsy

            nsvg__parsePath(parser, attr);

			for (NSVGshape *s = parser->image->shapes; s != NULL; s = s->next) {
				for (NSVGpath *p = s->paths; p != NULL; p = p->next) {
					assert(p->npts >= 6); // guaranteed to be cubic bezier?
					auto pts = p->pts;
					auto x0 = pts[0];
					auto y0 = pts[1];
					auto cpx1 = pts[2];
					auto cpy1 = pts[3];
					auto cpx2 = pts[4];
					auto cpy2 = pts[5];

					Path2DCommandArgs args = {};
					args.bezierTo = {cpx1, cpy1, cpx2, cpy2, x0, y0};
					AppendCommand(P2D_BEZIERTO, args);
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
        // const NativeCanvasDOMMatrix path = NativeCanvasDOMMatrix::Unwrap(info[1].As<Napi::Object>()); // TODO: DOMMatrix

        for (const auto& command : *path)
        {
            m_commands.push_back(command);
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
        const auto radii = static_cast<float>(info[4].As<Napi::Number>().DoubleValue()); // TODO: support list of numbers

        Path2DCommandArgs args = {};
        args.roundRect = {x, y, width, height, radii};
        AppendCommand(P2D_ROUNDRECT, args);
    }
}
