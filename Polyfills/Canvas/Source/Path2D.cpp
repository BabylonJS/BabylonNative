#include <cassert>
#include <cstring>
#include "NativeInstanceRegistry.h"
#include "Path2D.h"
#include "RoundRectUtils.h"

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

    namespace
    {
        constexpr size_t SVG_PATH_ATTRIBUTE_TERMINATOR_INDEX = 2;
        constexpr int CUBIC_POINT_STRIDE = 3;
        constexpr int POINT_COMPONENT_COUNT = 2;
        constexpr size_t CUBIC_START_X = 0;
        constexpr size_t CUBIC_START_Y = 1;
        constexpr size_t CUBIC_CONTROL_1_X = 2;
        constexpr size_t CUBIC_CONTROL_1_Y = 3;
        constexpr size_t CUBIC_CONTROL_2_X = 4;
        constexpr size_t CUBIC_CONTROL_2_Y = 5;
        constexpr size_t CUBIC_END_X = 6;
        constexpr size_t CUBIC_END_Y = 7;
        constexpr size_t TRANSFORM_COMPONENT_COUNT = 6;
        constexpr size_t ADD_PATH_WITH_TRANSFORM_ARGUMENT_COUNT = 2;
        constexpr size_t ARC_COUNTERCLOCKWISE_ARGUMENT_COUNT = 6;
        constexpr size_t ELLIPSE_COUNTERCLOCKWISE_ARGUMENT_COUNT = 8;
    }

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

        // Portable Babylon.js paths construct Path2D through the global.
        auto global = env.Global();

        // Install our constructor unconditionally: this Context cannot draw foreign native wraps.
        global.Set(JS_PATH2D_CONSTRUCTOR_NAME, func);
    }

    NativeCanvasPath2D* NativeCanvasPath2D::TryUnwrap(Napi::Env env, const Napi::Value& value)
    {
        return NativeInstanceRegistry<NativeCanvasPath2D>::TryUnwrap(env, value);
    }

    NativeCanvasPath2D::NativeCanvasPath2D(const Napi::CallbackInfo& info)
        : Napi::ObjectWrap<NativeCanvasPath2D>{info}
    {
        // Path2D | DOMString union: only a real Path2D is copied; everything else stringifies.
        const NativeCanvasPath2D* path = info.Length() >= 1
            ? NativeCanvasPath2D::TryUnwrap(info.Env(), info[0])
            : nullptr;
        const std::string d = path == nullptr && info.Length() >= 1 && !info[0].IsUndefined()
            ? info[0].ToString().Utf8Value()
            : "";

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
            assert(!attr[SVG_PATH_ATTRIBUTE_TERMINATOR_INDEX]); // nsvg__parsePath terminates attr parsing on falsy

            nsvg__parsePath(parser, attr);

            for (NSVGshape *shape = parser->image->shapes; shape != NULL; shape = shape->next) {
                for (NSVGpath *path = shape->paths; path != NULL; path = path->next) {
                    for (int i = 0; i < path->npts - 1; i += CUBIC_POINT_STRIDE) {
                        float* p = &path->pts[i * POINT_COMPONENT_COUNT];

                        auto x0 = p[CUBIC_START_X];
                        auto y0 = p[CUBIC_START_Y];
                        auto cpx1 = p[CUBIC_CONTROL_1_X];
                        auto cpy1 = p[CUBIC_CONTROL_1_Y];
                        auto cpx2 = p[CUBIC_CONTROL_2_X];
                        auto cpy2 = p[CUBIC_CONTROL_2_Y];
                        auto x1 = p[CUBIC_END_X];
                        auto y1 = p[CUBIC_END_Y];

                        // Only need to move on new shape
                        if (i == 0)
                        {
                            Path2DCommandArgs moveArgs = {};
                            moveArgs.moveTo = {x0, y0};
                            AppendCommand(Path2DCommandType::MoveTo, moveArgs);
                        }

                        Path2DCommandArgs args = {};
                        args.bezierTo = {cpx1, cpy1, cpx2, cpy2, x1, y1};
                        AppendCommand(Path2DCommandType::BezierTo, args);
                    }
                }
            }

            nsvg__deleteParser(parser);
        }

        // Register after successful construction only.
        NativeInstanceRegistry<NativeCanvasPath2D>::Add(info, this);
    }

    NativeCanvasPath2D::~NativeCanvasPath2D()
    {
        NativeInstanceRegistry<NativeCanvasPath2D>::Remove(this);
    }

    std::deque<Path2DCommand>::iterator NativeCanvasPath2D::begin()
    {
        return m_commands.begin();
    }

    std::deque<Path2DCommand>::iterator NativeCanvasPath2D::end()
    {
        return m_commands.end();
    }

    std::deque<Path2DCommand>::const_iterator NativeCanvasPath2D::begin() const
    {
        return m_commands.begin();
    }

    std::deque<Path2DCommand>::const_iterator NativeCanvasPath2D::end() const
    {
        return m_commands.end();
    }

    void NativeCanvasPath2D::AppendCommand(Path2DCommandType type, Path2DCommandArgs args)
    {
        m_commands.push_back({type, args});
    }

    void NativeCanvasPath2D::AddPath(const Napi::CallbackInfo& info)
    {
        if (info.Length() < 1)
        {
            throw Napi::TypeError::New(info.Env(), "Path2D.addPath: requires at least 1 argument (path).");
        }

        NativeCanvasPath2D* const path = NativeCanvasPath2D::TryUnwrap(info.Env(), info[0]);
        if (path == nullptr)
        {
            throw Napi::TypeError::New(info.Env(), "Path2D.addPath: the first argument is not a Path2D.");
        }

        bool xformInvReady{false};
        float xformInv[TRANSFORM_COMPONENT_COUNT];
        if (info.Length() == ADD_PATH_WITH_TRANSFORM_ARGUMENT_COUNT)
        {
            Napi::Object transform = info[1].As<Napi::Object>();
            auto a = transform.Get("a").As<Napi::Number>().FloatValue();
            auto b = transform.Get("b").As<Napi::Number>().FloatValue();
            auto c = transform.Get("c").As<Napi::Number>().FloatValue();
            auto d = transform.Get("d").As<Napi::Number>().FloatValue();
            auto e = transform.Get("e").As<Napi::Number>().FloatValue();
            auto f = transform.Get("f").As<Napi::Number>().FloatValue();

            float xform[TRANSFORM_COMPONENT_COUNT] = {a, b, c, d, e, f};
            nsvg__xformInverse(xformInv, xform);
            xformInvReady = true;

            Path2DCommandArgs args = {};
            args.transform = { xform[0], xform[1], xform[2], xform[3], xform[4], xform[5] };
            AppendCommand(Path2DCommandType::Transform, args);
        }

        for (const auto& command : *path)
        {
            m_commands.push_back(command);
        }

        if (info.Length() == ADD_PATH_WITH_TRANSFORM_ARGUMENT_COUNT)
        {
            assert(xformInvReady);
            Path2DCommandArgs argsInv = {};
            argsInv.transform = {xformInv[0], xformInv[1], xformInv[2], xformInv[3], xformInv[4], xformInv[5]};
            AppendCommand(Path2DCommandType::Transform, argsInv);
        }
    }

    void NativeCanvasPath2D::ClosePath(const Napi::CallbackInfo&)
    {
        AppendCommand(Path2DCommandType::Close, {});
    }

    void NativeCanvasPath2D::MoveTo(const Napi::CallbackInfo& info)
    {
        const auto x = static_cast<float>(info[0].As<Napi::Number>().DoubleValue());
        const auto y = static_cast<float>(info[1].As<Napi::Number>().DoubleValue());

        Path2DCommandArgs args = {};
        args.moveTo = {x, y};
        AppendCommand(Path2DCommandType::MoveTo, args);
    }

    void NativeCanvasPath2D::LineTo(const Napi::CallbackInfo& info)
    {
        const auto x = static_cast<float>(info[0].As<Napi::Number>().DoubleValue());
        const auto y = static_cast<float>(info[1].As<Napi::Number>().DoubleValue());

        Path2DCommandArgs args = {};
        args.lineTo = {x, y};
        AppendCommand(Path2DCommandType::LineTo, args);
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
        AppendCommand(Path2DCommandType::BezierTo, args);
    }

    void NativeCanvasPath2D::QuadraticCurveTo(const Napi::CallbackInfo& info)
    {
        const auto cpx = static_cast<float>(info[0].As<Napi::Number>().DoubleValue());
        const auto cpy = static_cast<float>(info[1].As<Napi::Number>().DoubleValue());
        const auto x = static_cast<float>(info[2].As<Napi::Number>().DoubleValue());
        const auto y = static_cast<float>(info[3].As<Napi::Number>().DoubleValue());

        Path2DCommandArgs args = {};
        args.quadTo = {cpx, cpy, x, y};
        AppendCommand(Path2DCommandType::QuadTo, args);
    }

    void NativeCanvasPath2D::Arc(const Napi::CallbackInfo& info)
    {
        const auto x = static_cast<float>(info[0].As<Napi::Number>().DoubleValue());
        const auto y = static_cast<float>(info[1].As<Napi::Number>().DoubleValue());
        const auto radius = static_cast<float>(info[2].As<Napi::Number>().DoubleValue());
        const auto startAngle = static_cast<float>(info[3].As<Napi::Number>().DoubleValue());
        const auto endAngle = static_cast<float>(info[4].As<Napi::Number>().DoubleValue());
        const auto counterclockwise = info.Length() == ARC_COUNTERCLOCKWISE_ARGUMENT_COUNT ? info[5].As<Napi::Boolean>() : false;

        Path2DCommandArgs args = {};
        args.arc = {x, y, radius, startAngle, endAngle, counterclockwise};
        AppendCommand(Path2DCommandType::Arc, args);
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
        AppendCommand(Path2DCommandType::ArcTo, args);
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
        const auto counterclockwise = info.Length() == ELLIPSE_COUNTERCLOCKWISE_ARGUMENT_COUNT ? info[7].As<Napi::Boolean>() : false;

        Path2DCommandArgs args = {};
        args.ellipse = {x, y, radiusX, radiusY, rotation, startAngle, endAngle, counterclockwise};
        AppendCommand(Path2DCommandType::Ellipse, args);
    }

    void NativeCanvasPath2D::Rect(const Napi::CallbackInfo& info)
    {
        const auto x = static_cast<float>(info[0].As<Napi::Number>().DoubleValue());
        const auto y = static_cast<float>(info[1].As<Napi::Number>().DoubleValue());
        const auto width = static_cast<float>(info[2].As<Napi::Number>().DoubleValue());
        const auto height = static_cast<float>(info[3].As<Napi::Number>().DoubleValue());

        Path2DCommandArgs args = {};
        args.rect = {x, y, width, height};
        AppendCommand(Path2DCommandType::Rect, args);
    }

    void NativeCanvasPath2D::RoundRect(const Napi::CallbackInfo& info)
    {
        const auto x = static_cast<float>(info[0].As<Napi::Number>().DoubleValue());
        const auto y = static_cast<float>(info[1].As<Napi::Number>().DoubleValue());
        const auto width = static_cast<float>(info[2].As<Napi::Number>().DoubleValue());
        const auto height = static_cast<float>(info[3].As<Napi::Number>().DoubleValue());
        const auto radii = ParseRoundRectRadii(info.Env(), info[4]);
        Path2DCommandArgs args{};
        switch (radii.type)
        {
            case RoundRectRadiiType::Uniform:
                args.roundRect = {x, y, width, height, radii.values[0]};
                AppendCommand(Path2DCommandType::RoundRect, args);
                break;
            case RoundRectRadiiType::Varying:
                args.roundRectVarying = {x, y, width, height,
                    radii.values[0], radii.values[1], radii.values[2], radii.values[3]};
                AppendCommand(Path2DCommandType::RoundRectVarying, args);
                break;
            case RoundRectRadiiType::Elliptic:
                args.roundRectElliptic = {x, y, width, height,
                    radii.values[0], radii.values[1], radii.values[2], radii.values[3],
                    radii.values[4], radii.values[5], radii.values[6], radii.values[7]};
                AppendCommand(Path2DCommandType::RoundRectElliptic, args);
                break;
        }
    }
}
