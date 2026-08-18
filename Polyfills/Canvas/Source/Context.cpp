#include <bx/math.h>
#include <map>
#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <limits>
#include <optional>
#include <regex>

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#endif

#include "nanovg/nanovg.h"
#include "nanovg/nanovg_babylon.h"

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb/stb_truetype.h"
#undef STB_TRUETYPE_IMPLEMENTATION

#include "Canvas.h"
#include "Context.h"
#include "MeasureText.h"
#include "Image.h"
#include "ImageData.h"
#include "Path2D.h"
#include "Colors.h"
#include "LineCaps.h"
#include "Gradient.h"

#ifdef BABYLON_NATIVE_PLUGIN_NATIVEENGINE_LOAD_IMAGES
#include <bimg/bimg.h>
#endif

namespace Babylon::Polyfills::Internal
{
    static constexpr auto JS_CONTEXT_CONSTRUCTOR_NAME = "Context";

    namespace
    {
        // True only for a finite, non-negative integer that fits in a uint32_t. Used where a
        // dimension arrives from a duck-typed object and so has not been through WebIDL's
        // unsigned long conversion; Uint32Value() would silently wrap -1 into 4294967295.
        bool IsValidExtent(double value)
        {
            return std::isfinite(value) &&
                value >= 0.0 &&
                value <= static_cast<double>(std::numeric_limits<uint32_t>::max()) &&
                value == std::trunc(value);
        }
    }

    void Context::Initialize(Napi::Env env)
    {
        Napi::HandleScope scope{env};

        Napi::Function func = DefineClass(
            env,
            JS_CONTEXT_CONSTRUCTOR_NAME,
            {
                InstanceMethod("clearRect", &Context::ClearRect),
                InstanceMethod("save", &Context::Save),
                InstanceMethod("restore", &Context::Restore),
                InstanceMethod("fillRect", &Context::FillRect),
                InstanceMethod("scale", &Context::Scale),
                InstanceMethod("rotate", &Context::Rotate),
                InstanceMethod("translate", &Context::Translate),
                InstanceMethod("strokeRect", &Context::StrokeRect),
                InstanceMethod("rect", &Context::Rect),
                InstanceMethod("roundRect", &Context::RoundRect),
                InstanceMethod("clip", &Context::Clip),
                InstanceMethod("putImageData", &Context::PutImageData),
                InstanceMethod("arc", &Context::Arc),
                InstanceMethod("beginPath", &Context::BeginPath),
                InstanceMethod("closePath", &Context::ClosePath),
                InstanceMethod("moveTo", &Context::MoveTo),
                InstanceMethod("lineTo", &Context::LineTo),
                InstanceMethod("quadraticCurveTo", &Context::QuadraticCurveTo),
                InstanceMethod("measureText", &Context::MeasureText),
                InstanceMethod("stroke", &Context::Stroke),
                InstanceMethod("fill", &Context::Fill),
                InstanceMethod("drawImage", &Context::DrawImage),
                InstanceMethod("getImageData", &Context::GetImageData),
                InstanceMethod("createImageData", &Context::CreateImageData),
                InstanceMethod("setLineDash", &Context::SetLineDash),
                InstanceMethod("getLineDash", &Context::GetLineDash),
                InstanceMethod("fillText", &Context::FillText),
                InstanceMethod("strokeText", &Context::StrokeText),
                InstanceMethod("createLinearGradient", &Context::CreateLinearGradient),
                InstanceMethod("createRadialGradient", &Context::CreateRadialGradient),
                InstanceMethod("getTransform", &Context::GetTransform),
                InstanceMethod("setTransform", &Context::SetTransform),
                InstanceMethod("transform", &Context::Transform),
                InstanceMethod("dispose", &Context::Dispose),
                InstanceMethod("flush", &Context::Flush),
                InstanceAccessor("lineCap", &Context::GetLineCap, &Context::SetLineCap),
                InstanceAccessor("lineJoin", &Context::GetLineJoin, &Context::SetLineJoin),
                InstanceAccessor("miterLimit", &Context::GetMiterLimit, &Context::SetMiterLimit),
                InstanceAccessor("filter", &Context::GetFilter, &Context::SetFilter),
                InstanceAccessor("direction", &Context::GetDirection, &Context::SetDirection),
                InstanceAccessor("font", &Context::GetFont, &Context::SetFont),
                InstanceAccessor("letterSpacing", &Context::GetLetterSpacing, &Context::SetLetterSpacing),
                InstanceAccessor("strokeStyle", &Context::GetStrokeStyle, &Context::SetStrokeStyle),
                InstanceAccessor("fillStyle", &Context::GetFillStyle, &Context::SetFillStyle),
                InstanceAccessor("globalAlpha", nullptr, &Context::SetGlobalAlpha),
                InstanceAccessor("shadowColor", &Context::GetShadowColor, &Context::SetShadowColor),
                InstanceAccessor("shadowBlur", &Context::GetShadowBlur, &Context::SetShadowBlur),
                InstanceAccessor("shadowOffsetX", &Context::GetShadowOffsetX, &Context::SetShadowOffsetX),
                InstanceAccessor("shadowOffsetY", &Context::GetShadowOffsetY, &Context::SetShadowOffsetY),
                InstanceAccessor("lineWidth", &Context::GetLineWidth, &Context::SetLineWidth),
            });
        JsRuntime::NativeObject::GetFromJavaScript(env).Set(JS_CONTEXT_CONSTRUCTOR_NAME, func);
    }

    Napi::Value Context::CreateInstance(Napi::Env env, Napi::Value canvas)
    {
        auto func = JsRuntime::NativeObject::GetFromJavaScript(env).Get(JS_CONTEXT_CONSTRUCTOR_NAME).As<Napi::Function>();
        return func.New({canvas});
    }

    Context::Context(const Napi::CallbackInfo& info)
        : Napi::ObjectWrap<Context>{info}
        , m_canvas{NativeCanvas::Unwrap(info[0].As<Napi::Object>())}
        , m_nvg{std::make_shared<NVGcontext*>(nvgCreate(1))}
        , m_graphicsContext{m_canvas->GetGraphicsContext()}
        , m_cancellationSource{std::make_shared<arcana::cancellation_source>()}
        , m_runtimeScheduler{Babylon::JsRuntime::GetFromJavaScript(info.Env())}
        , Polyfills::Canvas::Impl::MonitoredResource{Polyfills::Canvas::Impl::GetFromJavaScript(info.Env())}
    {
        // TODO: commented code doesn't compile with napi-jsi. Using non read-only property for now
        //info.This().ToObject().DefineProperty(Napi::PropertyDescriptor::Value("canvas", info[0], napi_enumerable));
        info.This().ToObject().Set("canvas", info[0]);

        // Fonts loaded after this Context is created are picked up in Flush().
    }

    Context::~Context()
    {
        Dispose();
        m_cancellationSource->cancel();
    }

    void Context::Dispose(const Napi::CallbackInfo&)
    {
        Dispose();
    }

    void Context::FlushGraphicResources()
    {
        Dispose();
    }

    void Context::Dispose()
    {
        if (m_nvg)
        {
            for (auto& image : m_nvgImageIndices)
            {
                nvgDeleteImage(*m_nvg, image.second);
            }
            nvgDelete(*m_nvg);
            m_nvg = nullptr;
        }
    }

    void Context::BindFillStyle(const Napi::CallbackInfo& info)
    {
        if (std::holds_alternative<std::string>(m_fillStyle))
        {
            const auto& str = std::get<std::string>(m_fillStyle);
            // Treat unset/empty fillStyle as opaque white (nvg's default fill color) instead of
            // the transparent black returned by StringToColor("") — this matches how fillStyle
            // behaves before any explicit assignment via SetFillStyle.
            const auto color = str.empty() ? nvgRGBA(255, 255, 255, 255) : StringToColor(info.Env(), str);
            nvgFillColor(*m_nvg, color);
        }
        else if (std::holds_alternative<GradientStyle>(m_fillStyle))
        {
            CanvasGradient* gradient = CanvasGradient::Unwrap(std::get<GradientStyle>(m_fillStyle)->Value());
            nvgFillPaint(*m_nvg, gradient->Paint());
        }
        else
        {
            throw Napi::Error::New(info.Env(), "Fillstyle is not a color string or a gradient.");
        }
    }

    void Context::BindStrokeStyle(const Napi::CallbackInfo& info)
    {
        if (std::holds_alternative<std::string>(m_strokeStyle))
        {
            const auto& str = std::get<std::string>(m_strokeStyle);
            // Treat unset/empty strokeStyle as opaque black -- both the Canvas2D default
            // ("#000000") and nvg's default stroke color -- instead of the transparent black
            // StringToColor("") would return.
            const auto color = str.empty() ? nvgRGBA(0, 0, 0, 255) : StringToColor(info.Env(), str);
            nvgStrokeColor(*m_nvg, color);
        }
        else if (std::holds_alternative<GradientStyle>(m_strokeStyle))
        {
            CanvasGradient* gradient = CanvasGradient::Unwrap(std::get<GradientStyle>(m_strokeStyle)->Value());
            nvgStrokePaint(*m_nvg, gradient->Paint());
        }
        else
        {
            throw Napi::Error::New(info.Env(), "Strokestyle is not a color string or a gradient.");
        }
    }

    void Context::SetFilterStack()
    {
        if (m_filter.length())
        {
            nanovg_filterstack filterStack;
            filterStack.ParseString(m_filter);
            nvgFilterStack(*m_nvg, filterStack); // sets filterStack on nanovg
        }
    }

    void Context::FillRect(const Napi::CallbackInfo& info)
    {
        auto left = info[0].As<Napi::Number>().FloatValue();
        auto top = info[1].As<Napi::Number>().FloatValue();
        auto width = info[2].As<Napi::Number>().FloatValue();
        auto height = info[3].As<Napi::Number>().FloatValue();

        // fillRect neither reads nor modifies the current path per spec, so it
        // would normally start its own. But Clip() can only express a rectangle
        // (nvgScissor), so a non-rectangular clip path is emulated by leaving it
        // in the current path and letting this fill render it. See Clip().
        if (!m_isClipped)
        {
            nvgBeginPath(*m_nvg);
        }

        nvgRect(*m_nvg, left, top, width, height);

        BindFillStyle(info);

        SetFilterStack();
        nvgFill(*m_nvg);
    }

    Napi::Value Context::GetFillStyle(const Napi::CallbackInfo&)
    {
        if (std::holds_alternative<std::string>(m_fillStyle))
        {
            return Napi::Value::From(Env(), std::get<std::string>(m_fillStyle));
        }
        else
        {
            // Return the gradient object that was assigned, as the spec requires, so that
            // `otherCtx.fillStyle = ctx.fillStyle` round-trips.
            return std::get<GradientStyle>(m_fillStyle)->Value();
        }
    }

    void Context::SetFillStyle(const Napi::CallbackInfo& info, const Napi::Value& value)
    {
        // Per spec the setter takes (DOMString or CanvasGradient or CanvasPattern); anything
        // else stringifies, fails color parsing and is ignored. Test for the gradient rather
        // than for "is an object", so `ctx.fillStyle = {}` cannot reach Unwrap.
        if (value.IsString())
        {
            auto string = value.As<Napi::String>().Utf8Value();
            const auto color = StringToColor(info.Env(), string);
            m_fillStyle = string;
            nvgFillColor(*m_nvg, color);
        }
        else if (CanvasGradient::IsInstance(info.Env(), value))
        {
            m_fillStyle = std::make_shared<Napi::ObjectReference>(Napi::Persistent(value.As<Napi::Object>()));
        }
        // Anything else leaves fillStyle unchanged, as the spec requires.
    }

    Napi::Value Context::GetStrokeStyle(const Napi::CallbackInfo&)
    {
        if (std::holds_alternative<std::string>(m_strokeStyle))
        {
            return Napi::Value::From(Env(), std::get<std::string>(m_strokeStyle));
        }
        else
        {
            return std::get<GradientStyle>(m_strokeStyle)->Value();
        }
    }

    void Context::SetStrokeStyle(const Napi::CallbackInfo& info, const Napi::Value& value)
    {
        // strokeStyle accepts a CanvasGradient just like fillStyle does; GUI controls such as
        // Line and the border of a Button assign one directly.
        if (value.IsString())
        {
            auto string = value.As<Napi::String>().Utf8Value();
            const auto color = StringToColor(info.Env(), string);
            m_strokeStyle = string;
            nvgStrokeColor(*m_nvg, color);
        }
        else if (CanvasGradient::IsInstance(info.Env(), value))
        {
            m_strokeStyle = std::make_shared<Napi::ObjectReference>(Napi::Persistent(value.As<Napi::Object>()));
        }
        // Per spec, assigning anything that is neither a color string nor a
        // gradient/pattern leaves strokeStyle unchanged.
    }

    Napi::Value Context::GetLineWidth(const Napi::CallbackInfo&)
    {
        return Napi::Value::From(Env(), m_lineWidth);
    }

    void Context::SetLineWidth(const Napi::CallbackInfo&, const Napi::Value& value)
    {
        m_lineWidth = value.As<Napi::Number>().FloatValue();
        nvgStrokeWidth(*m_nvg, m_lineWidth);
    }

    void Context::Fill(const Napi::CallbackInfo& info)
    {
        SetFilterStack();

        const NativeCanvasPath2D* path = info.Length() >= 1 && info[0].IsObject()
            ? NativeCanvasPath2D::Unwrap(info[0].As<Napi::Object>())
            : nullptr;
        // TODO: handle fillRule: nonzero, evenodd

        // draw Path2D if exists
        if (path != nullptr)
        {
            PlayPath2D(path);
        }

        // Bind the current fillStyle here rather than relying on the nvg state SetFillStyle
        // leaves behind: assigning a gradient only records the pointer (the paint has to be
        // rebuilt per draw), and nvgRestore can pop a color set after the last assignment.
        BindFillStyle(info);

        nvgFill(*m_nvg);
    }

    void Context::Save(const Napi::CallbackInfo&)
    {
        nvgSave(*m_nvg);
        // Track the wrapper-side drawing state alongside the nvg state stack so that
        // ctx.restore() correctly rewinds it — otherwise FillText/BindFillStyle would re-bind
        // a stale color from after a fillStyle change that nvg has since popped, and every
        // attribute getter would keep reporting its post-save() value.
        m_savedStyles.push_back({m_fillStyle, m_strokeStyle, m_lineCap, m_lineJoin, m_lineDash,
            m_shadowColor, m_shadowBlur, m_shadowOffsetX, m_shadowOffsetY, m_filter, m_direction,
            m_miterLimit, m_lineWidth, m_globalAlpha, m_letterSpacing, m_font, m_currentFontId});
    }

    void Context::Restore(const Napi::CallbackInfo&)
    {
        nvgRestore(*m_nvg);
        m_isClipped = false;
        if (!m_savedStyles.empty())
        {
            const auto& saved = m_savedStyles.back();
            m_fillStyle = saved.fillStyle;
            m_strokeStyle = saved.strokeStyle;
            m_lineCap = saved.lineCap;
            m_lineJoin = saved.lineJoin;
            m_lineDash = saved.lineDash;
            m_shadowColor = saved.shadowColor;
            m_shadowBlur = saved.shadowBlur;
            m_shadowOffsetX = saved.shadowOffsetX;
            m_shadowOffsetY = saved.shadowOffsetY;
            m_filter = saved.filter;
            m_direction = saved.direction;
            m_miterLimit = saved.miterLimit;
            m_lineWidth = saved.lineWidth;
            m_globalAlpha = saved.globalAlpha;
            m_letterSpacing = saved.letterSpacing;
            m_font = saved.font;
            m_currentFontId = saved.currentFontId;
            m_savedStyles.pop_back();
        }
    }

    void Context::ClearRect(const Napi::CallbackInfo& info)
    {
        const float x = info[0].As<Napi::Number>().FloatValue();
        const float y = info[1].As<Napi::Number>().FloatValue();
        const float width = info[2].As<Napi::Number>().FloatValue();
        const float height = info[3].As<Napi::Number>().FloatValue();

        nvgSave(*m_nvg);
        nvgGlobalCompositeOperation(*m_nvg, NVG_COPY);

        // See FillRect: clipping is a scissor, so the path must always be reset. Resetting it
        // invalidates the emulated clip, which points at a path that no longer exists, and the
        // nvgRestore below only pops ctx->states -- it does not put the old path back.
        ResetPathState();

        nvgRect(*m_nvg, x, y, width, height);

        nvgClosePath(*m_nvg);

        nvgFillColor(*m_nvg, TRANSPARENT_BLACK);
        nvgFill(*m_nvg);
        nvgRestore(*m_nvg);
    }

    void Context::Translate(const Napi::CallbackInfo& info)
    {
        const auto x = info[0].As<Napi::Number>().FloatValue();
        const auto y = info[1].As<Napi::Number>().FloatValue();
        nvgTranslate(*m_nvg, x, y);
    }

    void Context::Rotate(const Napi::CallbackInfo& info)
    {
        const auto angle = info[0].As<Napi::Number>().FloatValue();
        nvgRotate(*m_nvg, angle);
    }

    void Context::Scale(const Napi::CallbackInfo& info)
    {
        const auto x = info[0].As<Napi::Number>().FloatValue();
        const auto y = info[1].As<Napi::Number>().FloatValue();
        nvgScale(*m_nvg, x, y);
    }

    void Context::BeginPath(const Napi::CallbackInfo&)
    {
        ResetPathState();
    }

    void Context::ResetPathState()
    {
        m_isClipped = false;
        m_pathHasNonRect = false;
        nvgBeginPath(*m_nvg);
    }

    void Context::ClosePath(const Napi::CallbackInfo&)
    {
        nvgClosePath(*m_nvg);
    }

    void Context::Rect(const Napi::CallbackInfo& info)
    {
        const auto left = info[0].As<Napi::Number>().FloatValue();
        const auto top = info[1].As<Napi::Number>().FloatValue();
        const auto width = info[2].As<Napi::Number>().FloatValue();
        const auto height = info[3].As<Napi::Number>().FloatValue();

        nvgRect(*m_nvg, left, top, width, height);
        m_rectangleClipping = {left, top, width, height};
    }

    void Context::RoundRect(const Napi::CallbackInfo& info)
    {
        const auto x = info[0].As<Napi::Number>().FloatValue();
        const auto y = info[1].As<Napi::Number>().FloatValue();
        const auto width = info[2].As<Napi::Number>().FloatValue();
        const auto height = info[3].As<Napi::Number>().FloatValue();
        const auto radii = info[4];
        if (radii.IsNumber())
        {
            const auto radius = radii.As<Napi::Number>().FloatValue();
            nvgRoundedRect(*m_nvg, x, y, width, height, radius);
        }
        else if (radii.IsArray())
        {
            const auto radiiArray = radii.As<Napi::Array>();
            const auto radiiArrayLength = radiiArray.Length();
            if (radiiArrayLength == 1)
            {
                const auto radius = radiiArray[0u].As<Napi::Number>().FloatValue();
                nvgRoundedRect(*m_nvg, x, y, width, height, radius);
            }
            else if (radiiArrayLength == 2)
            {
                const auto topLeftBottomRight = radiiArray[0u].As<Napi::Number>().FloatValue();
                const auto topRightBottomLeft = radiiArray[1u].As<Napi::Number>().FloatValue();
                nvgRoundedRectVarying(*m_nvg, x, y, width, height, topLeftBottomRight, topRightBottomLeft, topLeftBottomRight, topRightBottomLeft);
            }
            else if (radiiArrayLength == 3)
            {
                const auto topLeft = radiiArray[0u].As<Napi::Number>().FloatValue();
                const auto topRightBottomLeft = radiiArray[1u].As<Napi::Number>().FloatValue();
                const auto bottomRight = radiiArray[2u].As<Napi::Number>().FloatValue();
                nvgRoundedRectVarying(*m_nvg, x, y, width, height, topLeft, topRightBottomLeft, bottomRight, topRightBottomLeft);
            }
            else if (radiiArrayLength == 4)
            {
                const auto topLeft = radiiArray[0u].As<Napi::Number>().FloatValue();
                const auto topRight = radiiArray[1u].As<Napi::Number>().FloatValue();
                const auto bottomRight = radiiArray[2u].As<Napi::Number>().FloatValue();
                const auto bottomLeft = radiiArray[3u].As<Napi::Number>().FloatValue();
                nvgRoundedRectVarying(*m_nvg, x, y, width, height, topLeft, topRight, bottomRight, bottomLeft);
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
            const auto dpx = dompoint.Get("x").As<Napi::Number>().FloatValue();
            const auto dpy = dompoint.Get("y").As<Napi::Number>().FloatValue();
            nvgRoundedRectElliptic(*m_nvg, x, y, width, height, dpx, dpy, dpx, dpy, dpx, dpy, dpx, dpy);
        }
        else
        {
            throw Napi::Error::New(info.Env(), "Invalid radii parameter");
        }

        m_rectangleClipping = {x, y, width, height};

        // Deliberately does not set m_pathHasNonRect, even though rounded corners are not
        // something nvgScissor can express. Clip()'s emulation for a non-rectangular path is
        // to leave the path current and let the next fill draw it, and nanovg fills the union
        // of the subpaths, not their intersection -- so `roundRect(); clip(); fillRect()` would
        // paint the whole fillRect rather than the rounded region. Measured on the "Native
        // Canvas" visual test: routing roundRect into the emulation takes the pixel difference
        // from 1.850% to 20.980%, where the scissor's square bounding box stays at 1.850%.
        // Dropping the radii is wrong, but it is the far smaller error of the two, and a
        // correct fix needs real path clipping (a stencil pass in nanovg) rather than this
        // union trick.
    }

    void Context::Clip(const Napi::CallbackInfo& /*info*/)
    {
        // A non-rectangular clip path cannot be expressed as a scissor rectangle.
        // Emulate it by leaving the path current so the next fill draws it, and
        // leave any enclosing scissor untouched rather than clipping to a
        // rectangle this path never described.
        if (m_pathHasNonRect)
        {
            m_isClipped = true;
            return;
        }

        m_isClipped = false;

        //By default m_rectangleClipping is not set, in this case we use the canvas width and height.
        auto w = m_rectangleClipping.width != 0 ? m_rectangleClipping.width : m_canvas->GetWidth();
        auto h = m_rectangleClipping.height != 0 ? m_rectangleClipping.height : m_canvas->GetHeight();

        // expand clipping 1pix in each direction because nanovg AA gets cut a bit short.
        nvgScissor(*m_nvg, m_rectangleClipping.left - 1, m_rectangleClipping.top - 1, w + 1, h + 1);
    }

    void Context::StrokeRect(const Napi::CallbackInfo& info)
    {
        const auto left = info[0].As<Napi::Number>().FloatValue();
        const auto top = info[1].As<Napi::Number>().FloatValue();
        const auto width = info[2].As<Napi::Number>().FloatValue();
        const auto height = info[3].As<Napi::Number>().FloatValue();

        nvgRect(*m_nvg, left, top, width, height);
        BindStrokeStyle(info);
        SetFilterStack();
        nvgStroke(*m_nvg);
    }

    void Context::PlayPath2D(const NativeCanvasPath2D* path)
    {
        m_isClipped = false;
        m_pathHasNonRect = true;
        nvgBeginPath(*m_nvg);
        for (const auto& command : *path)
        {
            const auto args = command.args;
            switch (command.type)
            {
                case P2D_CLOSE:
                    nvgClosePath(*m_nvg);
                    break;
                case P2D_MOVETO:
                    nvgMoveTo(*m_nvg, args.moveTo.x, args.moveTo.y);
                    break;
                case P2D_LINETO:
                    nvgLineTo(*m_nvg, args.lineTo.x, args.lineTo.y);
                    break;
                case P2D_BEZIERTO:
                    nvgBezierTo(*m_nvg, args.bezierTo.cp1x, args.bezierTo.cp1y,
                        args.bezierTo.cp2x, args.bezierTo.cp2y,
                        args.bezierTo.x, args.bezierTo.y);
                    break;
                case P2D_QUADTO:
                    nvgQuadTo(*m_nvg, args.quadTo.cpx, args.quadTo.cpy,
                        args.quadTo.x, args.quadTo.y);
                    break;
                case P2D_ARC:
                    nvgArc(*m_nvg, args.arc.x, args.arc.y, args.arc.radius,
                        args.arc.startAngle, args.arc.endAngle,
                        args.arc.counterclockwise ? NVG_CCW : NVG_CW);
                    break;
                case P2D_ARCTO:
                    nvgArcTo(*m_nvg, args.arcTo.x1, args.arcTo.y1,
                        args.arcTo.x2, args.arcTo.y2,
                        args.arcTo.radius);
                    break;
                case P2D_ELLIPSE:
                    // TODO: handle clockwise for nvgElipse (args.ellipse.counterclockwise)
                    nvgEllipse(*m_nvg, args.ellipse.x, args.ellipse.y,
                        args.ellipse.radiusX, args.ellipse.radiusY);
                    break;
                case P2D_RECT:
                    nvgRect(*m_nvg, args.rect.x, args.rect.y,
                        args.rect.width, args.rect.height);
                    break;
                case P2D_ROUNDRECT:
                    nvgRoundedRect(*m_nvg, args.roundRect.x, args.roundRect.y,
                        args.roundRect.width, args.roundRect.height,
                        args.roundRect.radii);
                    break;
                case P2D_ROUNDRECTVARYING:
                    nvgRoundedRectVarying(*m_nvg, args.roundRectVarying.x, args.roundRectVarying.y,
                        args.roundRectVarying.width, args.roundRectVarying.height,
                        args.roundRectVarying.topLeft, args.roundRectVarying.topRight,
                        args.roundRectVarying.bottomRight, args.roundRectVarying.bottomLeft);
                    break;
                case P2D_ROUNDRECTELLIPTIC:
                    nvgRoundedRectElliptic(*m_nvg, args.roundRectElliptic.x, args.roundRectElliptic.y,
                        args.roundRectElliptic.width, args.roundRectElliptic.height,
                        args.roundRectElliptic.topLeftX, args.roundRectElliptic.topLeftY,
                        args.roundRectElliptic.topRightX, args.roundRectElliptic.topRightY,
                        args.roundRectElliptic.bottomRightX, args.roundRectElliptic.bottomRightY,
                        args.roundRectElliptic.bottomLeftX, args.roundRectElliptic.bottomLeftY);
                    break;
                case P2D_TRANSFORM:
                    nvgTransform(*m_nvg,
                        args.transform.a, args.transform.b, args.transform.c,
                        args.transform.d, args.transform.e, args.transform.f);
                    break;
                default:
                    break;
            }
        }
    }

    void Context::Stroke(const Napi::CallbackInfo& info)
    {
        // draw Path2D if exists
        const NativeCanvasPath2D* path = info.Length() == 1 ? NativeCanvasPath2D::Unwrap(info[0].As<Napi::Object>()) : nullptr;
        if (path != nullptr)
        {
            PlayPath2D(path);
        }

        BindStrokeStyle(info);
        SetFilterStack();
        nvgStroke(*m_nvg);
    }

    void Context::MoveTo(const Napi::CallbackInfo& info)
    {
        const auto x = info[0].As<Napi::Number>().FloatValue();
        const auto y = info[1].As<Napi::Number>().FloatValue();

        m_pathHasNonRect = true;
        nvgMoveTo(*m_nvg, x, y);
    }

    void Context::LineTo(const Napi::CallbackInfo& info)
    {
        const auto x = info[0].As<Napi::Number>().FloatValue();
        const auto y = info[1].As<Napi::Number>().FloatValue();

        m_pathHasNonRect = true;
        nvgLineTo(*m_nvg, x, y);
    }

    void Context::QuadraticCurveTo(const Napi::CallbackInfo& info)
    {
        const auto cx = info[0].As<Napi::Number>().FloatValue();
        const auto cy = info[1].As<Napi::Number>().FloatValue();
        const auto x = info[2].As<Napi::Number>().FloatValue();
        const auto y = info[3].As<Napi::Number>().FloatValue();

        m_pathHasNonRect = true;
        nvgBezierTo(*m_nvg, cx, cy, cx, cy, x, y);
    }

    Napi::Value Context::MeasureText(const Napi::CallbackInfo& info)
    {
        std::string text{info[0].As<Napi::String>()};

        // If the JS-requested font family hasn't been loaded, return Arial-equivalent metrics
        // instead of measuring with whatever fallback font is bound. Browsers use the system
        // Arial for "Arial"/"sans-serif"/etc.; here we have e.g. droidsans only, which is
        // ~1.7x wider per em. Returning droidsans widths makes Babylon helpers like
        // DynamicTexture.drawText center text via t = (canvas - measureText.width)/2 to a
        // negative x and clip the text off-canvas. Arial-ish synthesised metrics keep the
        // centering on-canvas, while the actual FillText still substitutes our loaded font.
        const bool familyAvailable = !m_font.Familiy().empty()
            && m_fonts.find(m_font.Familiy()) != m_fonts.end();

        if (!familyAvailable && m_font.Size() > 0.f)
        {
            // Approximate Arial proportional metrics: average advance ~ 0.55 em.
            const float fontSize = m_font.Size();
            const float advance = fontSize * 0.55f;
            const float width = advance * static_cast<float>(text.length());
            const float ascent = fontSize * 0.75f;
            const float descent = fontSize * 0.25f;

            auto obj{Napi::Object::New(info.Env())};
            obj.Set("width", Napi::Value::From(info.Env(), width));
            obj.Set("height", Napi::Value::From(info.Env(), ascent + descent));
            obj.Set("actualBoundingBoxLeft", Napi::Value::From(info.Env(), 0.f));
            obj.Set("actualBoundingBoxRight", Napi::Value::From(info.Env(), width));
            obj.Set("fontBoundingBoxAscent", Napi::Value::From(info.Env(), ascent));
            obj.Set("fontBoundingBoxDescent", Napi::Value::From(info.Env(), descent));
            return obj.As<Napi::Value>();
        }

        return MeasureText::CreateInstance(info.Env(), this, text);
    }

    bool Context::SetFontFaceId()
    {
        EnsureFontsLoaded();
        if (m_fonts.empty())
        {
            return false;
        }
        else if (m_currentFontId >= 0)
        {
            nvgFontFaceId(*m_nvg, m_currentFontId);
        }
        else
        {
            nvgFontFaceId(*m_nvg, m_fonts.begin()->second);
        }
        return true;
    }

    void Context::FillText(const Napi::CallbackInfo& info)
    {
        std::string text = info[0].As<Napi::String>().Utf8Value();
        auto x = info[1].As<Napi::Number>().FloatValue();
        auto y = info[2].As<Napi::Number>().FloatValue();

        // TODO: support ligatures, etc.
        if (m_direction.compare("rtl") == 0) {
            std::reverse(text.begin(), text.end());
        }

        if (SetFontFaceId())
        {
            BindFillStyle(info);

            if (m_filter.length())
            {
                nanovg_filterstack filterStack;
                filterStack.ParseString(m_filter);
                nvgFilterStack(*m_nvg, filterStack); // sets filterStack on nanovg
            }

            nvgText(*m_nvg, x, y, text.c_str(), nullptr);
        }
    }

    void Context::EnsureFontsLoaded()
    {
        // Pick up any fonts that were loaded after this Context was created.
        for (auto& font : NativeCanvas::fontsInfos)
        {
            if (m_fonts.end() == m_fonts.find(font.first))
            {
                // TODO: update nvgCreateFontMem safely when old font buffer invalidated
                m_fonts[font.first] = nvgCreateFontMem(*m_nvg, font.first.c_str(), font.second.data(), static_cast<int>(font.second.size()), 0);
            }
        }
    }

    void Context::Flush(const Napi::CallbackInfo& info)
    {
        // Pick up any fonts loaded after this Context was created (#1683).
        EnsureFontsLoaded();

        // If called outside the frame cycle (e.g., during initialization/font loading
        // or async texture load callbacks), acquire a FrameCompletionScope which blocks
        // until StartRenderingCurrentFrame provides the encoder.
        std::optional<Graphics::FrameCompletionScope> scope;
        if (m_graphicsContext.GetActiveEncoder() == nullptr)
        {
            scope.emplace(m_graphicsContext.AcquireFrameCompletionScope());
        }

        // A canvas flush is the one stretch of a frame that acquires views without ever
        // reaching NativeEngine::GetEncoder, so no budget check runs inside it. Its cost is
        // not bounded either: the pool recycles framebuffers but not view ids (Acquire ->
        // Bind() drops the cached id, the draw re-acquires, Release -> Clear() acquires
        // again), so it scales with filter-op count. That means kViewFlushMargin cannot be
        // sized to cover it, and a frame that is just under the flush threshold on entry
        // here would run past maxViews and throw "Too many views".
        //
        // Check once, here. This is the only safe point in the flush:
        //   - after the scope above, so m_pendingFrameScopes > 0 and the flush handshake is
        //     actually serviceable by the parked render thread (it no-ops otherwise);
        //   - before the encoder is read below, so a flush that swaps the frame encoder
        //     cannot leave us holding a stale pointer;
        //   - before nvgBeginFrame, so no nanovg transient buffer exists to be invalidated
        //     (those are allocated in glnvg__renderFlush during nvgEndFrame).
        // This does not bound a single canvas' cost, but it reduces the requirement from
        // "the margin must cover an arbitrary canvas" to "one canvas must fit in maxViews".
        m_graphicsContext.FlushViewsIfNeeded();

        bgfx::Encoder* encoder = m_graphicsContext.GetActiveEncoder();
        if (encoder == nullptr)
        {
            return;
        }

        // Discard any residual encoder state from NativeEngine rendering.
        // In the old model Canvas had its own per-thread encoder with clean state;
        // now it shares the frame encoder with NativeEngine.
        encoder->discard(BGFX_DISCARD_ALL);

        try
        {
            // The entire flush is wrapped: bgfx framebuffer pool exhaustion can throw both from
            // UpdateRenderTarget() and from FrameBufferPool::Acquire() reached via nvgEndFrame()
            // below. Converting any such C++ failure into a catchable JS error lets the offending
            // test fail cleanly instead of aborting the whole sweep.
            const bool needClear = m_canvas->UpdateRenderTarget();

            Graphics::FrameBuffer& frameBuffer = m_canvas->GetFrameBuffer();

            frameBuffer.Bind();
            if (needClear)
            {
                frameBuffer.Clear(*encoder, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH | BGFX_CLEAR_STENCIL, 0, 1.f, 0);
            }
            frameBuffer.SetViewPort(0.f, 0.f, 1.f, 1.f);
            const auto width = m_canvas->GetWidth();
            const auto height = m_canvas->GetHeight();

            for (auto& buffer : m_canvas->m_frameBufferPool.GetPoolBuffers())
            {
                // sanity check no buffers should have been acquired yet
                assert(buffer.isAvailable == true);
            }
            std::function<Babylon::Graphics::FrameBuffer*()> acquire = [this]() -> Babylon::Graphics::FrameBuffer* {
                Babylon::Graphics::FrameBuffer *frameBuffer = this->m_canvas->m_frameBufferPool.Acquire();
                frameBuffer->Bind();
                return frameBuffer;
            };
            std::function<void(Babylon::Graphics::FrameBuffer*)> release = [this, encoder](Babylon::Graphics::FrameBuffer* frameBuffer) -> void {
                // clear framebuffer when released
                frameBuffer->Clear(*encoder, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH | BGFX_CLEAR_STENCIL, 0, 1.f, 0);
                this->m_canvas->m_frameBufferPool.Release(frameBuffer);
                frameBuffer->Unbind();
            };

            nvgBeginFrame(*m_nvg, float(width), float(height), 1.0f);
            nvgSetFrameBufferAndEncoder(*m_nvg, frameBuffer, encoder);
            nvgSetFrameBufferPool(*m_nvg, { acquire, release });
            nvgEndFrame(*m_nvg);
            frameBuffer.Unbind();

            // Reserve the view id for the eventual canvas->texture blit NOW, while we are
            // sequenced immediately after this canvas' draws but before the scene/backbuffer
            // render is recorded. bgfx processes blits in numeric view-id order, so the copy
            // must land AFTER the canvas Flush (source ready) yet BEFORE the fullscreen ADT
            // layer samples the destination texture. Deferring to CopyTexture's
            // PeekNextViewId() would place the blit after the backbuffer view, so the layer
            // would sample the previous frame's content (a one-frame GUI latency).
            m_canvas->SetBlitViewId(m_graphicsContext.AcquireNewViewId(), m_graphicsContext.ViewIdGeneration());

            for (auto& buffer : m_canvas->m_frameBufferPool.GetPoolBuffers())
            {
                // sanity check no unreleased buffers
                assert(buffer.isAvailable == true);
            }
        }
        catch (const std::exception& ex)
        {
            throw Napi::Error::New(info.Env(), ex.what());
        }
    }

    void Context::PutImageData(const Napi::CallbackInfo& info)
    {
        Napi::Env env = info.Env();

        if (info.Length() < 3 || !info[0].IsObject())
        {
            throw Napi::TypeError::New(env, "Context2D.putImageData requires at least 3 arguments (imageData, dx, dy).");
        }

        // Read width/height/data as ordinary properties rather than unwrapping an ImageData:
        // napi_unwrap on an object that was never wrapped dereferences garbage, and callers
        // legitimately pass plain {data,width,height} objects.
        Napi::Object imageData = info[0].As<Napi::Object>();
        Napi::Value dataValue = imageData.Get("data");
        if (!dataValue.IsTypedArray())
        {
            throw Napi::TypeError::New(env, "Context2D.putImageData: the first argument is not an ImageData.");
        }

        const auto widthValue = imageData.Get("width");
        const auto heightValue = imageData.Get("height");
        if (!widthValue.IsNumber() || !heightValue.IsNumber())
        {
            throw Napi::TypeError::New(env, "Context2D.putImageData: the first argument is not an ImageData (missing numeric 'width'/'height').");
        }

        const auto srcWidthInt = widthValue.As<Napi::Number>().Int32Value();
        const auto srcHeightInt = heightValue.As<Napi::Number>().Int32Value();
        if (srcWidthInt <= 0 || srcHeightInt <= 0)
        {
            return;
        }

        const auto srcWidth = static_cast<uint32_t>(srcWidthInt);
        const auto srcHeight = static_cast<uint32_t>(srcHeightInt);

        const auto typedArray = dataValue.As<Napi::TypedArray>();
        const uint64_t requiredBytes = static_cast<uint64_t>(srcWidth) * srcHeight * 4u;
        if (typedArray.ByteLength() < requiredBytes)
        {
            throw Napi::TypeError::New(env, "Context2D.putImageData: the ImageData buffer is smaller than width*height*4.");
        }
        const auto* srcPixels = static_cast<const uint8_t*>(typedArray.ArrayBuffer().Data()) + typedArray.ByteOffset();

        const auto dx = info[1].ToNumber().Int32Value();
        const auto dy = info[2].ToNumber().Int32Value();

        // The dirty rectangle is optional and, per spec, may be given with
        // negative extents, which flips it rather than drawing nothing.
        // Normalized in int64_t: the values arrive from JS as arbitrary int32s, and negating
        // INT32_MIN or adding two INT32_MINs together is signed overflow (undefined behavior).
        // Every input fits comfortably in int64_t, and the result is clipped to the source
        // bitmap below, so the values are back in int32 range before they are used.
        int64_t dirtyX{0};
        int64_t dirtyY{0};
        int64_t dirtyWidth{srcWidthInt};
        int64_t dirtyHeight{srcHeightInt};
        if (info.Length() >= 7)
        {
            dirtyX = info[3].ToNumber().Int32Value();
            dirtyY = info[4].ToNumber().Int32Value();
            dirtyWidth = info[5].ToNumber().Int32Value();
            dirtyHeight = info[6].ToNumber().Int32Value();

            if (dirtyWidth < 0)
            {
                dirtyX += dirtyWidth;
                dirtyWidth = -dirtyWidth;
            }
            if (dirtyHeight < 0)
            {
                dirtyY += dirtyHeight;
                dirtyHeight = -dirtyHeight;
            }
        }

        // Clip the dirty rectangle to the source bitmap.
        const int64_t x0 = std::max<int64_t>(0, dirtyX);
        const int64_t y0 = std::max<int64_t>(0, dirtyY);
        const int64_t x1 = std::min<int64_t>(srcWidthInt, dirtyX + dirtyWidth);
        const int64_t y1 = std::min<int64_t>(srcHeightInt, dirtyY + dirtyHeight);
        if (x1 <= x0 || y1 <= y0)
        {
            return;
        }

        const auto copyWidth = static_cast<uint32_t>(x1 - x0);
        const auto copyHeight = static_cast<uint32_t>(y1 - y0);

        // nvgImagePattern needs a tightly packed image, so extract the dirty
        // sub-rectangle into its own buffer.
        std::vector<uint8_t> patch(static_cast<size_t>(copyWidth) * copyHeight * 4u);
        for (uint32_t row = 0; row < copyHeight; ++row)
        {
            std::memcpy(
                patch.data() + static_cast<size_t>(row) * copyWidth * 4u,
                srcPixels + (static_cast<size_t>(y0 + row) * srcWidth + static_cast<size_t>(x0)) * 4u,
                static_cast<size_t>(copyWidth) * 4u);
        }

        // dx/dy are arbitrary int32s from JS, so offsetting them by the clipped origin is done in
        // int64_t and saturated back. Anything that saturates is far enough off-canvas that it
        // clips to nothing downstream regardless.
        const auto toInt32 = [](int64_t value) {
            return static_cast<int32_t>(std::clamp<int64_t>(value, std::numeric_limits<int32_t>::min(), std::numeric_limits<int32_t>::max()));
        };
        const int32_t destLeft = toInt32(static_cast<int64_t>(dx) + x0);
        const int32_t destTop = toInt32(static_cast<int64_t>(dy) + y0);

        const auto destX = static_cast<float>(destLeft);
        const auto destY = static_cast<float>(destTop);
        const auto destWidth = static_cast<float>(copyWidth);
        const auto destHeight = static_cast<float>(copyHeight);

        const int imageIndex = nvgCreateImageRGBA(*m_nvg, static_cast<int>(copyWidth), static_cast<int>(copyHeight), 0, patch.data());
        if (imageIndex == 0)
        {
            throw Napi::Error::New(env, "Context2D.putImageData: failed to create the source image.");
        }

        // putImageData bypasses the transform, the clip region, globalAlpha and
        // the composite operation, and replaces the destination pixels outright.
        // nvgReset() clears exactly that state, and NVG_COPY gives the required
        // replace (rather than blend) semantics.
        nvgSave(*m_nvg);
        nvgReset(*m_nvg);
        nvgGlobalCompositeOperation(*m_nvg, NVG_COPY);

        NVGpaint imagePaint = nvgImagePattern(*m_nvg, destX, destY, destWidth, destHeight, 0.f, imageIndex, 1.f);
        ResetPathState();
        nvgRect(*m_nvg, destX, destY, destWidth, destHeight);
        nvgFillPaint(*m_nvg, imagePaint);
        nvgFill(*m_nvg);

        nvgRestore(*m_nvg);
        nvgDeleteImage(*m_nvg, imageIndex);

        // Keep the CPU mirror that getImageData() reads from in sync.
        BlitPixelsToCpu(patch.data(), copyWidth, copyHeight, 0, 0, copyWidth, copyHeight,
            destLeft, destTop, copyWidth, copyHeight);
    }

    void Context::Arc(const Napi::CallbackInfo& info)
    {
        const auto x = static_cast<float>(info[0].As<Napi::Number>().DoubleValue());
        const auto y = static_cast<float>(info[1].As<Napi::Number>().DoubleValue());
        const auto radius = static_cast<float>(info[2].As<Napi::Number>().DoubleValue());
        const auto startAngle = static_cast<float>(info[3].As<Napi::Number>().DoubleValue());
        const auto endAngle = static_cast<float>(info[4].As<Napi::Number>().DoubleValue());
        const NVGwinding winding = (info.Length() == 6 && info[5].As<Napi::Boolean>()) ? NVGwinding::NVG_CCW : NVGwinding::NVG_CW;
        m_pathHasNonRect = true;
        nvgArc(*m_nvg, x, y, radius, startAngle, endAngle, winding);
    }

    void Context::EnsureCpuBuffer()
    {
        const uint32_t width = m_canvas != nullptr ? m_canvas->GetWidth() : 0;
        const uint32_t height = m_canvas != nullptr ? m_canvas->GetHeight() : 0;
        if (width != m_cpuWidth || height != m_cpuHeight || m_cpuPixels.empty())
        {
            m_cpuWidth = width;
            m_cpuHeight = height;

            const uint64_t pixelCount = static_cast<uint64_t>(width) * height;
            if (pixelCount > std::numeric_limits<size_t>::max() / 4)
            {
                // Leave the mirror empty; drawImage and getImageData both no-op safely on it.
                m_cpuPixels.clear();
                return;
            }

            m_cpuPixels.assign(static_cast<size_t>(pixelCount) * 4, 0);
        }
    }

    void Context::BlitPixelsToCpu(const uint8_t* src, uint32_t srcWidth, uint32_t srcHeight, int32_t sx, int32_t sy, uint32_t sw, uint32_t sh, int32_t dx, int32_t dy, uint32_t dw, uint32_t dh)
    {
        if (src == nullptr || dw == 0 || dh == 0 || sw == 0 || sh == 0 || srcWidth == 0 || srcHeight == 0)
        {
            return;
        }

        EnsureCpuBuffer();
        if (m_cpuPixels.empty())
        {
            return;
        }

        // Clamp the iteration range to the destination rect's intersection with the canvas up
        // front. The destination size is caller-controlled (and reaches us as an unsigned value,
        // so a negative width wraps to ~4e9), and iterating the full rect just to reject every
        // pixel would stall the JS thread. int64_t keeps dx/dy + dw/dh from overflowing.
        const int64_t iBegin = std::max<int64_t>(0, -static_cast<int64_t>(dx));
        const int64_t iEnd = std::min<int64_t>(dw, static_cast<int64_t>(m_cpuWidth) - dx);
        const int64_t jBegin = std::max<int64_t>(0, -static_cast<int64_t>(dy));
        const int64_t jEnd = std::min<int64_t>(dh, static_cast<int64_t>(m_cpuHeight) - dy);

        for (int64_t j = jBegin; j < jEnd; ++j)
        {
            // Nearest-neighbor sample of the source row (exact when dh == sh).
            const int64_t srcY = sy + static_cast<int64_t>(j) * sh / dh;
            if (srcY < 0 || srcY >= static_cast<int64_t>(srcHeight))
            {
                continue;
            }

            const size_t destRow = static_cast<size_t>(dy + j) * m_cpuWidth;
            const size_t srcRow = static_cast<size_t>(srcY) * srcWidth;

            for (int64_t i = iBegin; i < iEnd; ++i)
            {
                const int64_t srcX = sx + static_cast<int64_t>(i) * sw / dw;
                if (srcX < 0 || srcX >= static_cast<int64_t>(srcWidth))
                {
                    continue;
                }

                const size_t srcIndex = (srcRow + static_cast<size_t>(srcX)) * 4;
                const size_t destIndex = (destRow + static_cast<size_t>(dx + i)) * 4;
                m_cpuPixels[destIndex + 0] = src[srcIndex + 0];
                m_cpuPixels[destIndex + 1] = src[srcIndex + 1];
                m_cpuPixels[destIndex + 2] = src[srcIndex + 2];
                m_cpuPixels[destIndex + 3] = src[srcIndex + 3];
            }
        }
    }

    void Context::ReadPixels(int32_t sx, int32_t sy, uint32_t w, uint32_t h, uint8_t* dst)
    {
        const size_t total = static_cast<size_t>(w) * h * 4;
        std::memset(dst, 0, total);

        // Resync the mirror to the canvas first. Without this, a canvas resize followed by
        // getImageData with no intervening drawImage would read the old buffer using the old
        // dimensions and hand back stale pixels; EnsureCpuBuffer reallocates and zero-fills.
        EnsureCpuBuffer();
        if (m_cpuPixels.empty())
        {
            return;
        }

        for (uint32_t j = 0; j < h; ++j)
        {
            const int32_t srcY = sy + static_cast<int32_t>(j);
            if (srcY < 0 || srcY >= static_cast<int32_t>(m_cpuHeight))
            {
                continue;
            }

            for (uint32_t i = 0; i < w; ++i)
            {
                const int32_t srcX = sx + static_cast<int32_t>(i);
                if (srcX < 0 || srcX >= static_cast<int32_t>(m_cpuWidth))
                {
                    continue;
                }

                const size_t srcIndex = (static_cast<size_t>(srcY) * m_cpuWidth + srcX) * 4;
                const size_t destIndex = (static_cast<size_t>(j) * w + i) * 4;
                dst[destIndex + 0] = m_cpuPixels[srcIndex + 0];
                dst[destIndex + 1] = m_cpuPixels[srcIndex + 1];
                dst[destIndex + 2] = m_cpuPixels[srcIndex + 2];
                dst[destIndex + 3] = m_cpuPixels[srcIndex + 3];
            }
        }
    }

    void Context::DrawImage(const Napi::CallbackInfo& info)
    {
        Napi::Object imageObj = info[0].As<Napi::Object>();

        // NativeEngine.createImageBitmap() returns a plain object carrying the raw decoded pixels
        // ({data, width, height, format}) rather than a wrapped NativeCanvasImage. Because Babylon
        // Native sets forceBitmapOverHTMLImageElement, LoadImage delivers these bitmaps to drawImage
        // (e.g. Mesh.applyDisplacementMap, height/flow maps). Unwrapping such a plain object as a
        // NativeCanvasImage would dereference garbage and crash, so handle it explicitly by
        // converting the pixels to RGBA8 and drawing/blitting them directly.
        if (imageObj.Has("data") && imageObj.Get("data").IsTypedArray())
        {
#ifdef BABYLON_NATIVE_PLUGIN_NATIVEENGINE_LOAD_IMAGES
            const auto data = imageObj.Get("data").As<Napi::Uint8Array>();
            const uint32_t width = imageObj.Get("width").As<Napi::Number>().Uint32Value();
            const uint32_t height = imageObj.Get("height").As<Napi::Number>().Uint32Value();
            const auto format = static_cast<bimg::TextureFormat::Enum>(imageObj.Get("format").As<Napi::Number>().Uint32Value());
            if (width == 0 || height == 0)
            {
                return;
            }

            // Everything below is caller-supplied. bimg::imageConvert reads the source and writes
            // width*height*4 bytes to the destination without knowing either buffer's real length,
            // so validate both before handing it any pointers.
            if (format >= bimg::TextureFormat::Count)
            {
                throw Napi::Error::New(info.Env(), "drawImage: ImageBitmap has an out-of-range pixel format.");
            }

            const uint64_t pixelCount = static_cast<uint64_t>(width) * height;
            if (pixelCount > std::numeric_limits<size_t>::max() / 4)
            {
                throw Napi::Error::New(info.Env(), "drawImage: ImageBitmap dimensions are too large.");
            }

            // imageGetSize takes uint16_t extents, so anything above 65535 would be truncated and
            // produce a size for a much smaller image. A 65537x1 RGBA8 bitmap would come back as
            // 1x1 (4 bytes), a 4-byte buffer would pass the check below, and the RGBA8 memcpy
            // would then read width*height*4 bytes from it. Reject what bimg cannot describe.
            if (width > std::numeric_limits<uint16_t>::max() || height > std::numeric_limits<uint16_t>::max())
            {
                throw Napi::Error::New(info.Env(), "drawImage: ImageBitmap dimensions exceed the maximum supported size.");
            }

            // Ask bimg for the size rather than computing width*height*bpp/8: block-compressed
            // formats round up to whole blocks and have a minimum block count, so a 1x1 BC1 image
            // still occupies one 8-byte block. The naive bits-per-pixel math would accept a
            // 1-byte buffer for it and imageConvert would then read past the end.
            const uint64_t requiredBytes = bimg::imageGetSize(nullptr, static_cast<uint16_t>(width), static_cast<uint16_t>(height), 1, false, false, 1, format);
            if (requiredBytes == 0 || data.ByteLength() < requiredBytes)
            {
                throw Napi::Error::New(info.Env(), "drawImage: ImageBitmap data is smaller than width*height for its format.");
            }

            const uint8_t* srcBytes = data.Data();
            std::vector<uint8_t> rgba(static_cast<size_t>(pixelCount) * 4);
            if (format == bimg::TextureFormat::RGBA8)
            {
                std::memcpy(rgba.data(), srcBytes, rgba.size());
            }
            else if (!bimg::imageConvert(&Graphics::DeviceContext::GetDefaultAllocator(), rgba.data(), bimg::TextureFormat::RGBA8, srcBytes, format, width, height, 1))
            {
                throw Napi::Error::New(info.Env(), "drawImage: unsupported ImageBitmap pixel format.");
            }

            const int imageIndex = nvgCreateImageRGBA(*m_nvg, static_cast<int>(width), static_cast<int>(height), 0, rgba.data());
            DrawImageCommon(info, imageIndex, rgba.data(), width, height);
            nvgDeleteImage(*m_nvg, imageIndex);
            return;
#else
            throw Napi::Error::New(info.Env(), "drawImage: image loading disabled in this build.");
#endif
        }

        const NativeCanvasImage* canvasImage = NativeCanvasImage::Unwrap(imageObj);

        int imageIndex{-1};
        const auto nvgImageIter = m_nvgImageIndices.find(canvasImage);
        if (nvgImageIter == m_nvgImageIndices.end())
        {
            imageIndex = canvasImage->CreateNVGImageForContext(*m_nvg);
            m_nvgImageIndices.try_emplace(canvasImage, imageIndex);
        }
        else
        {
            imageIndex = nvgImageIter->second;
        }
        assert(imageIndex != -1);

        DrawImageCommon(info, imageIndex, canvasImage->GetPixels(), canvasImage->GetWidth(), canvasImage->GetHeight());
    }

    void Context::DrawImageCommon(const Napi::CallbackInfo& info, int imageIndex, const uint8_t* srcPixels, uint32_t srcWidth, uint32_t srcHeight)
    {
        const auto imgWidth = static_cast<float>(srcWidth);
        const auto imgHeight = static_cast<float>(srcHeight);

        if (info.Length() == 3)
        {
            const auto dx = static_cast<float>(info[1].As<Napi::Number>().Int32Value());
            const auto dy = static_cast<float>(info[2].As<Napi::Number>().Int32Value());

            // Anchor the pattern at the destination, not the canvas origin: the
            // rect below is drawn at (dx,dy), so a pattern based at (0,0) makes
            // it sample from outside the image and clamp to the edge texels,
            // which silently draws nothing for any dx/dy other than (0,0).
            NVGpaint imagePaint = nvgImagePattern(*m_nvg, dx, dy, imgWidth, imgHeight, 0.f, imageIndex, 1.f);

            // See FillRect: clipping is a scissor, so the path must always be reset.
            ResetPathState();

            nvgRect(*m_nvg, dx, dy, imgWidth, imgHeight);
            nvgFillPaint(*m_nvg, imagePaint);
            SetFilterStack();
            nvgFill(*m_nvg);

            BlitPixelsToCpu(srcPixels, srcWidth, srcHeight, 0, 0, srcWidth, srcHeight,
                static_cast<int32_t>(dx), static_cast<int32_t>(dy), srcWidth, srcHeight);
        }
        else if (info.Length() == 5)
        {
            const auto dxInt = info[1].As<Napi::Number>().Int32Value();
            const auto dyInt = info[2].As<Napi::Number>().Int32Value();
            const auto dWidthInt = info[3].As<Napi::Number>().Int32Value();
            const auto dHeightInt = info[4].As<Napi::Number>().Int32Value();

            // Parse the extents as signed: read via Uint32Value, a negative width wraps to ~4e9,
            // which would queue an enormous nvgRect. A non-positive extent draws nothing.
            if (dWidthInt <= 0 || dHeightInt <= 0)
            {
                return;
            }

            const auto dx = static_cast<float>(dxInt);
            const auto dy = static_cast<float>(dyInt);
            const auto dWidth = static_cast<float>(dWidthInt);
            const auto dHeight = static_cast<float>(dHeightInt);

            NVGpaint imagePaint = nvgImagePattern(*m_nvg, dx, dy, dWidth, dHeight, 0.f, imageIndex, 1.f);

            // See FillRect: clipping is a scissor, so the path must always be reset.
            ResetPathState();

            nvgRect(*m_nvg, dx, dy, dWidth, dHeight);
            nvgFillPaint(*m_nvg, imagePaint);
            SetFilterStack();
            nvgFill(*m_nvg);

            BlitPixelsToCpu(srcPixels, srcWidth, srcHeight, 0, 0, srcWidth, srcHeight,
                dxInt, dyInt, static_cast<uint32_t>(dWidthInt), static_cast<uint32_t>(dHeightInt));
        }
        else if (info.Length() == 9)
        {
            const auto sx = info[1].As<Napi::Number>().Int32Value();
            const auto sy = info[2].As<Napi::Number>().Int32Value();
            const auto sWidthInt = info[3].As<Napi::Number>().Int32Value();
            const auto sHeightInt = info[4].As<Napi::Number>().Int32Value();
            const auto dxInt = info[5].As<Napi::Number>().Int32Value();
            const auto dyInt = info[6].As<Napi::Number>().Int32Value();
            const auto dWidthInt = info[7].As<Napi::Number>().Int32Value();
            const auto dHeightInt = info[8].As<Napi::Number>().Int32Value();

            if (sWidthInt <= 0 || sHeightInt <= 0 || dWidthInt <= 0 || dHeightInt <= 0)
            {
                return;
            }

            const auto sWidth = static_cast<uint32_t>(sWidthInt);
            const auto sHeight = static_cast<uint32_t>(sHeightInt);
            const auto dx = static_cast<float>(dxInt);
            const auto dy = static_cast<float>(dyInt);
            const auto dWidth = static_cast<float>(dWidthInt);
            const auto dHeight = static_cast<float>(dHeightInt);

            NVGpaint imagePaint = nvgImagePattern(*m_nvg, dx, dy, dWidth, dHeight, 0.f, imageIndex, 1.f);

            // See FillRect: clipping is a scissor, so the path must always be reset.
            ResetPathState();

            nvgRect(*m_nvg, dx, dy, dWidth, dHeight);
            nvgFillPaint(*m_nvg, imagePaint);
            SetFilterStack();
            nvgFill(*m_nvg);

            BlitPixelsToCpu(srcPixels, srcWidth, srcHeight, sx, sy, sWidth, sHeight,
                dxInt, dyInt, static_cast<uint32_t>(dWidthInt), static_cast<uint32_t>(dHeightInt));
        }
        else
        {
            throw Napi::Error::New(info.Env(), "Invalid number of parameters for DrawImage");
        }
    }

    Napi::Value Context::CreateImageData(const Napi::CallbackInfo& info)
    {
        // Two overloads: (width, height) and (imagedata). Both produce a blank,
        // transparent-black buffer -- only the dimensions differ in how they are sourced.
        uint32_t width{}, height{};

        if (info.Length() >= 2 && info[0].IsNumber() && info[1].IsNumber())
        {
            const auto wInt = info[0].As<Napi::Number>().Int32Value();
            const auto hInt = info[1].As<Napi::Number>().Int32Value();

            // The spec takes the magnitude of each extent, so a negative size is legal.
            // INT32_MIN has no positive counterpart and would negate into itself, so it is
            // rejected rather than wrapped into a huge unsigned extent.
            if (wInt == std::numeric_limits<int32_t>::min() || hInt == std::numeric_limits<int32_t>::min())
            {
                throw Napi::RangeError::New(info.Env(), "Context2D.createImageData: requested size is too large.");
            }

            width = static_cast<uint32_t>(wInt < 0 ? -wInt : wInt);
            height = static_cast<uint32_t>(hInt < 0 ? -hInt : hInt);
        }
        else if (info.Length() >= 1 && info[0].IsObject())
        {
            const auto source = info[0].As<Napi::Object>();
            const auto w = source.Get("width");
            const auto h = source.Get("height");
            if (!w.IsNumber() || !h.IsNumber())
            {
                throw Napi::TypeError::New(info.Env(), "Context2D.createImageData: argument is not an ImageData");
            }

            // This overload is duck-typed rather than restricted to a real ImageData, so the
            // dimensions have not been through WebIDL's unsigned long conversion. Uint32Value()
            // would wrap a negative width to 4294967295, which clears the overflow check below
            // on 64-bit and turns an invalid source into a ~17 GB allocation. The drawImage and
            // putImageData extents guard against the same wrap.
            const auto wNum = w.As<Napi::Number>().DoubleValue();
            const auto hNum = h.As<Napi::Number>().DoubleValue();
            if (!IsValidExtent(wNum) || !IsValidExtent(hNum))
            {
                throw Napi::RangeError::New(info.Env(), "Context2D.createImageData: source width and height are not valid extents");
            }

            width = static_cast<uint32_t>(wNum);
            height = static_cast<uint32_t>(hNum);
        }
        else
        {
            throw Napi::TypeError::New(info.Env(), "Context2D.createImageData: invalid arguments");
        }

        if (width == 0 || height == 0)
        {
            throw Napi::RangeError::New(info.Env(), "Context2D.createImageData: width and height must be non-zero");
        }

        // Backs a width*height*4 allocation, so reject sizes that would overflow size_t.
        if (static_cast<uint64_t>(width) * height > std::numeric_limits<size_t>::max() / 4)
        {
            throw Napi::RangeError::New(info.Env(), "Context2D.createImageData: requested size is too large.");
        }

        // A null context means "do not read back the framebuffer", so the ImageData is
        // left as the transparent black the spec requires.
        return ImageData::CreateInstance(info.Env(), nullptr, 0, 0, width, height);
    }

    Napi::Value Context::GetImageData(const Napi::CallbackInfo& info)
    {
        if (info.Length() < 4)
        {
            throw Napi::Error::New(info.Env(), "Context2D.getImageData: invalid number of parameters");
        }

        auto sx = info[0].As<Napi::Number>().Int32Value();
        auto sy = info[1].As<Napi::Number>().Int32Value();
        const auto swInt = info[2].As<Napi::Number>().Int32Value();
        const auto shInt = info[3].As<Napi::Number>().Int32Value();

        // Parse the extents as signed. Read via Uint32Value, a width of -1 wraps to 4294967295,
        // and the size_t guard below does not catch it on 64-bit because 4294967295 * 1 is far
        // below SIZE_MAX/4, so ImageData would go on to allocate roughly 17 GB.
        //
        // A negative extent is legal: the browser treats it as a rectangle running in the
        // opposite direction and returns the normalized region, so fold the sign into the
        // origin rather than rejecting it. INT32_MIN has no positive counterpart, so it is
        // rejected outright instead of being negated into itself.
        if (swInt == std::numeric_limits<int32_t>::min() || shInt == std::numeric_limits<int32_t>::min())
        {
            throw Napi::RangeError::New(info.Env(), "Context2D.getImageData: requested region is too large.");
        }

        uint32_t sw, sh;
        if (swInt < 0)
        {
            sw = static_cast<uint32_t>(-swInt);
            sx -= static_cast<int32_t>(sw);
        }
        else
        {
            sw = static_cast<uint32_t>(swInt);
        }

        if (shInt < 0)
        {
            sh = static_cast<uint32_t>(-shInt);
            sy -= static_cast<int32_t>(sh);
        }
        else
        {
            sh = static_cast<uint32_t>(shInt);
        }

        // The region is caller-supplied and backs a width*height*4 allocation, so reject sizes
        // that would overflow size_t before ImageData tries to allocate them.
        if (static_cast<uint64_t>(sw) * sh > std::numeric_limits<size_t>::max() / 4)
        {
            throw Napi::RangeError::New(info.Env(), "Context2D.getImageData: requested region is too large.");
        }

        return ImageData::CreateInstance(info.Env(), this, sx, sy, sw, sh);
    }

    void Context::SetLineDash(const Napi::CallbackInfo& info)
    {
        // An empty (or absent) dash list means "solid", which is exactly what we
        // already draw -- so accept it silently. Babylon GUI's Line and MultiLine
        // controls call setLineDash(this._dash) unconditionally on every render,
        // and _dash defaults to [], so throwing here aborted any scene containing
        // a GUI line even though nothing was actually being asked for.
        //
        // A non-empty pattern we genuinely cannot honor: nanovg has no dashed
        // stroke, and emulating one means splitting every path into segments.
        // Draw solid and warn once rather than failing the whole scene, and keep
        // the list so getLineDash() round-trips as the spec requires.
        //
        // Parsed into a temporary and committed only once every segment
        // validates, because the spec keeps the previous list when the argument
        // is rejected -- clearing m_lineDash up front would destroy it first.
        std::vector<double> parsed;

        if (info.Length() > 0)
        {
            if (!info[0].IsArray())
            {
                // Present but not a list: the spec rejects the call, which leaves the
                // previous list in place. Committing the empty temporary here would clear
                // it instead, so setLineDash("x") would wipe a list that setLineDash([-1])
                // correctly keeps.
                return;
            }

            const auto segments = info[0].As<Napi::Array>();
            for (uint32_t index = 0; index < segments.Length(); ++index)
            {
                const auto segment = segments.Get(index);
                if (!segment.IsNumber())
                {
                    // Per spec, a list containing a non-finite or negative value
                    // is ignored entirely and the previous list is retained; a
                    // non-numeric entry cannot be interpreted, so ignore it too.
                    return;
                }

                const double value = segment.As<Napi::Number>().DoubleValue();
                if (!std::isfinite(value) || value < 0.0)
                {
                    return;
                }

                parsed.push_back(value);
            }
        }

        m_lineDash = std::move(parsed);

        if (!m_lineDash.empty())
        {
            static bool warned = false;
            if (!warned)
            {
                warned = true;
                fprintf(stderr, "Context2D.setLineDash: dashed strokes are not supported; drawing solid.\n");
            }
        }
    }

    Napi::Value Context::GetLineDash(const Napi::CallbackInfo& info)
    {
        auto segments = Napi::Array::New(info.Env(), m_lineDash.size());
        for (size_t index = 0; index < m_lineDash.size(); ++index)
        {
            segments.Set(static_cast<uint32_t>(index), Napi::Number::New(info.Env(), m_lineDash[index]));
        }
        return segments;
    }

    void Context::StrokeText(const Napi::CallbackInfo& info)
    {
        std::string text = info[0].As<Napi::String>().Utf8Value();
        auto x = info[1].As<Napi::Number>().FloatValue();
        auto y = info[2].As<Napi::Number>().FloatValue();

        // TODO: support ligatures, etc.
        if (m_direction.compare("rtl") == 0) {
            std::reverse(text.begin(), text.end());
        }

        if (SetFontFaceId())
        {
            BindStrokeStyle(info);
            nvgStrokeText(*m_nvg, x, y, text.c_str(), nullptr);
        }
    }

    Napi::Value Context::CreateLinearGradient(const Napi::CallbackInfo& info)
    {
        const auto x0 = info[0].As<Napi::Number>().FloatValue();
        const auto y0 = info[1].As<Napi::Number>().FloatValue();
        const auto x1 = info[2].As<Napi::Number>().FloatValue();
        const auto y1 = info[3].As<Napi::Number>().FloatValue();

        auto gradient = CanvasGradient::CreateLinear(info.Env(), m_nvg, x0, y0, x1, y1);
        return gradient;
    }

    Napi::Value Context::CreateRadialGradient(const Napi::CallbackInfo& info)
    {
        const auto x0 = info[0].As<Napi::Number>().FloatValue();
        const auto y0 = info[1].As<Napi::Number>().FloatValue();
        const auto r0 = info[2].As<Napi::Number>().FloatValue();
        const auto x1 = info[3].As<Napi::Number>().FloatValue();
        const auto y1 = info[4].As<Napi::Number>().FloatValue();
        const auto r1 = info[5].As<Napi::Number>().FloatValue();

        auto gradient = CanvasGradient::CreateRadial(info.Env(), m_nvg, x0, y0, r0, x1, y1, r1);
        return gradient;
    }

    Napi::Value Context::GetTransform(const Napi::CallbackInfo&)
    {
        float xform[6];
        nvgCurrentTransform(*m_nvg, xform);

        // set DOMMatrix properties
        Napi::Object obj = Napi::Object::New(Env());
        obj.Set("a", xform[0]);
        obj.Set("b", xform[1]);
        obj.Set("c", xform[2]);
        obj.Set("d", xform[3]);
        obj.Set("e", xform[4]);
        obj.Set("f", xform[5]);
        obj.Set("m11", xform[0]);
        obj.Set("m12", xform[1]);
        obj.Set("m13", 0);
        obj.Set("m14", 0);
        obj.Set("m21", xform[2]);
        obj.Set("m22", xform[3]);
        obj.Set("m23", 0);
        obj.Set("m24", 0);
        obj.Set("m31", 0);
        obj.Set("m32", 0);
        obj.Set("m33", 1);
        obj.Set("m34", 0);
        obj.Set("m41", xform[4]);
        obj.Set("m42", xform[5]);
        obj.Set("m43", 0);
        obj.Set("m44", 1);
        obj.Set("is2D", true);
        obj.Set("isIdentity", false);
        return obj;
    }

    void Context::SetTransform(const Napi::CallbackInfo& info)
    {
        const auto a = info[0].As<Napi::Number>().FloatValue();
        const auto b = info[1].As<Napi::Number>().FloatValue();
        const auto c = info[2].As<Napi::Number>().FloatValue();
        const auto d = info[3].As<Napi::Number>().FloatValue();
        const auto e = info[4].As<Napi::Number>().FloatValue();
        const auto f = info[5].As<Napi::Number>().FloatValue();
        nvgResetTransform(*m_nvg);
        nvgTransform(*m_nvg, a, b, c, d, e, f);
    }

    void Context::Transform(const Napi::CallbackInfo& info)
    {
        const auto a = info[0].As<Napi::Number>().FloatValue();
        const auto b = info[1].As<Napi::Number>().FloatValue();
        const auto c = info[2].As<Napi::Number>().FloatValue();
        const auto d = info[3].As<Napi::Number>().FloatValue();
        const auto e = info[4].As<Napi::Number>().FloatValue();
        const auto f = info[5].As<Napi::Number>().FloatValue();
        nvgTransform(*m_nvg, a, b, c, d, e, f);
    }

    Napi::Value Context::GetLineCap(const Napi::CallbackInfo& info)
    {
        return Napi::Value::From(Env(), m_lineCap);
    }

    void Context::SetLineCap(const Napi::CallbackInfo& info, const Napi::Value& value)
    {
        m_lineCap = value.As<Napi::String>().Utf8Value();
        const auto lineCap = StringToLineCap(info.Env(), m_lineCap);
        nvgLineCap(*m_nvg, lineCap);
    }

    Napi::Value Context::GetLineJoin(const Napi::CallbackInfo& info)
    {
        return Napi::Value::From(Env(), m_lineJoin);
    }

    void Context::SetLineJoin(const Napi::CallbackInfo& info, const Napi::Value& value)
    {
        m_lineJoin = value.As<Napi::String>().Utf8Value();
        const auto lineJoin = StringToLineJoin(info.Env(), m_lineJoin);
        nvgLineJoin(*m_nvg, lineJoin);
    }

    Napi::Value Context::GetMiterLimit(const Napi::CallbackInfo& info)
    {
        return Napi::Value::From(Env(), m_miterLimit);
    }

    void Context::SetMiterLimit(const Napi::CallbackInfo& info, const Napi::Value& value)
    {
        m_miterLimit = value.As<Napi::Number>().FloatValue();
        nvgMiterLimit(*m_nvg, m_miterLimit);
    }

    Napi::Value Context::GetFilter(const Napi::CallbackInfo& info)
    {
        return Napi::Value::From(Env(), m_filter);
    }

    void Context::SetFilter(const Napi::CallbackInfo& info, const Napi::Value& value)
    {
        std::string filterString = value.As<Napi::String>().Utf8Value();
        // Keep existing filter if the new one is invalid
        if (nanovg_filterstack::ValidString(filterString))
        {
            m_filter = filterString;
        }
    }

    Napi::Value Context::GetDirection(const Napi::CallbackInfo& info)
    {
        return Napi::Value::From(Env(), m_direction);
    }

    void Context::SetDirection(const Napi::CallbackInfo& info, const Napi::Value& value)
    {
        std::string direction = value.As<Napi::String>().Utf8Value();
        const bool valid = !(direction.compare("ltr") && direction.compare("rtl"));
        if (valid)
        {
            m_direction = direction;
        }
    }

    Napi::Value Context::GetFont(const Napi::CallbackInfo& info)
    {
        return Napi::Value::From(Env(), static_cast<std::string>(m_font));
    }

    void Context::SetFont(const Napi::CallbackInfo& info, const Napi::Value& value)
    {
        if (!value.IsString())
        {
            throw Napi::Error::New(info.Env(), "invalid argument");
        }

        auto font = Font::Parse(value.ToString());
        if (!font)
        {
            return;
        }

        nvgFontSize(*m_nvg, font->Size());
        EnsureFontsLoaded();
        if (m_fonts.find(font->Familiy()) == m_fonts.end())
        {
            // TODO: handle finding font face for a specific weight and style
            m_currentFontId = -1;
        }
        else
        {
            m_currentFontId = m_fonts.at(font->Familiy());
        }

        m_font = std::move(*font);
    }

    Napi::Value Context::GetLetterSpacing(const Napi::CallbackInfo& info)
    {
        std::string letterSpacingStr = std::to_string(m_letterSpacing);
        letterSpacingStr.erase(letterSpacingStr.find_last_not_of('0') + 1, std::string::npos);
        letterSpacingStr.erase(letterSpacingStr.find_last_not_of('.') + 1, std::string::npos);
        return Napi::Value::From(Env(), letterSpacingStr + "px");
    }

    void Context::SetLetterSpacing(const Napi::CallbackInfo& info, const Napi::Value& value)
    {
        const std::string letterSpacingOption = value.ToString();

        // regex the letter spacing string
        static const std::regex letterSpacingRegex("(\\d+(\\.\\d+)?)px");
        std::smatch letterSpacingMatch;
        if (std::regex_match(letterSpacingOption, letterSpacingMatch, letterSpacingRegex))
        {
            // std::stof throws a fatal std::out_of_range for a digit run too long for a float,
            // and the regex bounds neither the length nor the magnitude. strtof cannot throw;
            // ignore a non-finite result rather than handing it to nanovg.
            const std::string letterSpacingText{letterSpacingMatch[1].str()};
            const float letterSpacing = std::strtof(letterSpacingText.c_str(), nullptr);
            if (std::isfinite(letterSpacing))
            {
                m_letterSpacing = letterSpacing;
            }
        }
        nvgTextLetterSpacing(*m_nvg, m_letterSpacing);
    }

    void Context::SetGlobalAlpha(const Napi::CallbackInfo& info, const Napi::Value& value)
    {
        const float alpha = value.As<Napi::Number>().FloatValue();
        nvgGlobalAlpha(*m_nvg, alpha);
    }

    // nanovg has no shadow primitive, so the shadow attributes are stored and
    // reported back but not rendered. Throwing was the wrong response: these are
    // ordinary state attributes, and reading one, or writing the default, asks
    // for nothing at all. Babylon GUI resets shadowBlur/shadowOffsetX/OffsetY to
    // 0 after drawing a shadowed control, so the throw killed the scene on the
    // *reset* path as well as the request path, and any control with a drop
    // shadow aborted outright rather than drawing without one.
    //
    // Storing them keeps the spec-required round trip working and lets content
    // that saves and restores canvas state continue to function; a shadow that
    // is genuinely requested warns once instead of failing the scene.
    void Context::WarnShadowUnsupported()
    {
        if (m_shadowBlur == 0.0 && m_shadowOffsetX == 0.0 && m_shadowOffsetY == 0.0)
        {
            // Nothing is being asked for: no offset and no blur draws no shadow.
            return;
        }

        static bool warned = false;
        if (!warned)
        {
            warned = true;
            fprintf(stderr, "Context2D: shadows are not supported; drawing without a shadow.\n");
        }
    }

    Napi::Value Context::GetShadowColor(const Napi::CallbackInfo& info)
    {
        return Napi::String::New(info.Env(), m_shadowColor);
    }

    void Context::SetShadowColor(const Napi::CallbackInfo& info, const Napi::Value& value)
    {
        // Per spec an unparseable shadowColor is ignored, keeping the old value.
        const auto color = value.As<Napi::String>().Utf8Value();
        try
        {
            StringToColor(info.Env(), color);
        }
        catch (const Napi::Error&)
        {
            return;
        }

        m_shadowColor = color;
    }

    Napi::Value Context::GetShadowBlur(const Napi::CallbackInfo& info)
    {
        return Napi::Number::New(info.Env(), m_shadowBlur);
    }

    void Context::SetShadowBlur(const Napi::CallbackInfo& info, const Napi::Value& value)
    {
        // Per spec, negative and non-finite values are ignored.
        const double blur = value.As<Napi::Number>().DoubleValue();
        if (!std::isfinite(blur) || blur < 0.0)
        {
            return;
        }

        m_shadowBlur = blur;
        WarnShadowUnsupported();
    }

    Napi::Value Context::GetShadowOffsetX(const Napi::CallbackInfo& info)
    {
        return Napi::Number::New(info.Env(), m_shadowOffsetX);
    }

    void Context::SetShadowOffsetX(const Napi::CallbackInfo& info, const Napi::Value& value)
    {
        const double offset = value.As<Napi::Number>().DoubleValue();
        if (!std::isfinite(offset))
        {
            return;
        }

        m_shadowOffsetX = offset;
        WarnShadowUnsupported();
    }

    Napi::Value Context::GetShadowOffsetY(const Napi::CallbackInfo& info)
    {
        return Napi::Number::New(info.Env(), m_shadowOffsetY);
    }

    void Context::SetShadowOffsetY(const Napi::CallbackInfo& info, const Napi::Value& value)
    {
        const double offset = value.As<Napi::Number>().DoubleValue();
        if (!std::isfinite(offset))
        {
            return;
        }

        m_shadowOffsetY = offset;
        WarnShadowUnsupported();
    }
}
