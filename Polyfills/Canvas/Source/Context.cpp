#include <bx/math.h>
#include <map>
#include <algorithm>
#include <cassert>
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

/*
Most of these context methods are preliminary work. They are currenbly not tested properly.
*/
namespace Babylon::Polyfills::Internal
{
    static constexpr auto JS_CONTEXT_CONSTRUCTOR_NAME = "Context";

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
                InstanceMethod("setLineDash", &Context::SetLineDash),
                InstanceMethod("fillText", &Context::FillText),
                InstanceMethod("strokeText", &Context::StrokeText),
                InstanceMethod("createLinearGradient", &Context::CreateLinearGradient),
                InstanceMethod("createRadialGradient", &Context::CreateRadialGradient),
                InstanceMethod("getTransform", &Context::GetTransform),
                InstanceMethod("setTransform", &Context::SetTransform),
                InstanceMethod("transform", &Context::Transform),
                InstanceMethod("dispose", &Context::Dispose),
                InstanceAccessor("lineCap", &Context::GetLineCap, &Context::SetLineCap),
                InstanceAccessor("lineJoin", &Context::GetLineJoin, &Context::SetLineJoin),
                InstanceAccessor("miterLimit", &Context::GetMiterLimit, &Context::SetMiterLimit),
                InstanceAccessor("filter", &Context::GetFilter, &Context::SetFilter),
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
                InstanceAccessor("canvas", &Context::GetCanvas, nullptr),
            });
        JsRuntime::NativeObject::GetFromJavaScript(env).Set(JS_CONTEXT_CONSTRUCTOR_NAME, func);
    }

    Napi::Value Context::CreateInstance(Napi::Env env, NativeCanvas* canvas)
    {
        Napi::HandleScope scope{env};

        auto func = JsRuntime::NativeObject::GetFromJavaScript(env).Get(JS_CONTEXT_CONSTRUCTOR_NAME).As<Napi::Function>();
        return func.New({Napi::External<NativeCanvas>::New(env, canvas)});
    }

    Context::Context(const Napi::CallbackInfo& info)
        : Napi::ObjectWrap<Context>{info}
        , m_canvas{info[0].As<Napi::External<NativeCanvas>>().Data()}
        , m_nvg{std::make_shared<NVGcontext*>(nvgCreate(1))}
        , m_graphicsContext{m_canvas->GetGraphicsContext()}
        , m_update{m_graphicsContext.GetUpdate("update")}
        , m_cancellationSource{std::make_shared<arcana::cancellation_source>()}
        , m_runtimeScheduler{Babylon::JsRuntime::GetFromJavaScript(info.Env())}
        , Polyfills::Canvas::Impl::MonitoredResource{Polyfills::Canvas::Impl::GetFromJavaScript(info.Env())}
    {
        for (auto& font : NativeCanvas::fontsInfos)
        {
            m_fonts[font.first] = nvgCreateFontMem(*m_nvg, font.first.c_str(), font.second.data(), static_cast<int>(font.second.size()), 0);
        }
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

        m_isClipped = false;
    }

    void Context::BindFillStyle(const Napi::CallbackInfo& info, float left, float top, float width, float height)
    {
        if (std::holds_alternative<std::string>(m_fillStyle))
        {
            const auto color = StringToColor(info.Env(), std::get<std::string>(m_fillStyle));
            nvgFillColor(*m_nvg, color);
        }
        else if (std::holds_alternative<CanvasGradient*>(m_fillStyle))
        {
            CanvasGradient* gradient = std::get<CanvasGradient*>(m_fillStyle);
            gradient->UpdateCache();
            // TODO: replace left/lop/width/height by context bounds
            NVGpaint imagePaint = nvgImagePattern(*m_nvg, 0.f, 0.f, width + left, height, 0.f, gradient->CachedImage(), 1.f);
            nvgFillPaint(*m_nvg, imagePaint);
        }
        else
        {
            throw Napi::Error::New(info.Env(), "Fillstyle is not a color string or a gradient.");
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

        if (!m_isClipped)
        {
            nvgBeginPath(*m_nvg);
        }

        nvgRect(*m_nvg, left, top, width, height);

        BindFillStyle(info, left, top, width, height);

        SetFilterStack();
        nvgFill(*m_nvg);
        SetDirty();
    }

    Napi::Value Context::GetFillStyle(const Napi::CallbackInfo&)
    {
        if (std::holds_alternative<std::string>(m_fillStyle))
        {
            return Napi::Value::From(Env(), std::get<std::string>(m_fillStyle));
        }
        else
        {
            return Napi::External<CanvasGradient>::New(Env(), std::get<CanvasGradient*>(m_fillStyle));
        }
    }

    void Context::SetFillStyle(const Napi::CallbackInfo& info, const Napi::Value& value)
    {
        if (value.IsString())
        {
            auto string = value.As<Napi::String>().Utf8Value();
            const auto color = StringToColor(info.Env(), string);
            m_fillStyle = string;
            nvgFillColor(*m_nvg, color);
        }
        else
        {
            CanvasGradient* canvasGradient = CanvasGradient::Unwrap(info[0].As<Napi::Object>());
            m_fillStyle = canvasGradient;
        }
        SetDirty();
    }

    Napi::Value Context::GetStrokeStyle(const Napi::CallbackInfo&)
    {
        return Napi::Value::From(Env(), m_strokeStyle);
    }

    void Context::SetStrokeStyle(const Napi::CallbackInfo& info, const Napi::Value& value)
    {
        m_strokeStyle = value.As<Napi::String>().Utf8Value();
        auto color = StringToColor(info.Env(), m_strokeStyle);
        nvgStrokeColor(*m_nvg, color);
        SetDirty();
    }

    Napi::Value Context::GetLineWidth(const Napi::CallbackInfo&)
    {
        return Napi::Value::From(Env(), m_lineWidth);
    }

    void Context::SetLineWidth(const Napi::CallbackInfo&, const Napi::Value& value)
    {
        m_lineWidth = value.As<Napi::Number>().FloatValue();
        nvgStrokeWidth(*m_nvg, m_lineWidth);
        SetDirty();
    }

    void Context::Fill(const Napi::CallbackInfo&)
    {
        SetFilterStack();
        nvgFill(*m_nvg);
        SetDirty();
    }

    void Context::Save(const Napi::CallbackInfo&)
    {
        nvgSave(*m_nvg);
        SetDirty();
    }

    void Context::Restore(const Napi::CallbackInfo&)
    {
        nvgRestore(*m_nvg);
        SetDirty();
        m_isClipped = false;
    }

    void Context::ClearRect(const Napi::CallbackInfo& info)
    {
        const float x = info[0].As<Napi::Number>().FloatValue();
        const float y = info[1].As<Napi::Number>().FloatValue();
        const float width = info[2].As<Napi::Number>().FloatValue();
        const float height = info[3].As<Napi::Number>().FloatValue();

        nvgSave(*m_nvg);
        nvgGlobalCompositeOperation(*m_nvg, NVG_COPY);

        if (!m_isClipped)
        {
            nvgBeginPath(*m_nvg);
        }

        nvgRect(*m_nvg, x, y, width, height);

        if (!m_isClipped)
        {
            nvgClosePath(*m_nvg);
        }

        nvgFillColor(*m_nvg, TRANSPARENT_BLACK);
        nvgFill(*m_nvg);
        nvgRestore(*m_nvg);
        SetDirty();
    }

    void Context::Translate(const Napi::CallbackInfo& info)
    {
        const auto x = info[0].As<Napi::Number>().FloatValue();
        const auto y = info[1].As<Napi::Number>().FloatValue();
        nvgTranslate(*m_nvg, x, y);
        SetDirty();
    }

    void Context::Rotate(const Napi::CallbackInfo& info)
    {
        const auto angle = info[0].As<Napi::Number>().FloatValue();
        nvgRotate(*m_nvg, angle);
        SetDirty();
    }

    void Context::Scale(const Napi::CallbackInfo& info)
    {
        const auto x = info[0].As<Napi::Number>().FloatValue();
        const auto y = info[1].As<Napi::Number>().FloatValue();
        nvgScale(*m_nvg, x, y);
        SetDirty();
    }

    void Context::BeginPath(const Napi::CallbackInfo&)
    {
        nvgBeginPath(*m_nvg);
        SetDirty();
    }

    void Context::ClosePath(const Napi::CallbackInfo&)
    {
        nvgClosePath(*m_nvg);
        SetDirty();
    }

    void Context::Rect(const Napi::CallbackInfo& info)
    {
        const auto left = info[0].As<Napi::Number>().FloatValue();
        const auto top = info[1].As<Napi::Number>().FloatValue();
        const auto width = info[2].As<Napi::Number>().FloatValue();
        const auto height = info[3].As<Napi::Number>().FloatValue();

        nvgRect(*m_nvg, left, top, width, height);
        m_rectangleClipping = {left, top, width, height};
        SetDirty();
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
        else
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
                const auto topRightBottomLeft = radiiArray[1].As<Napi::Number>().FloatValue();

                nvgRoundedRectVarying(*m_nvg, x, y, width, height, topLeftBottomRight, topRightBottomLeft, topLeftBottomRight, topRightBottomLeft);
            }
            else if (radiiArrayLength == 3)
            {
                const auto topLeft = radiiArray[0u].As<Napi::Number>().FloatValue();
                const auto topRightBottomLeft = radiiArray[1].As<Napi::Number>().FloatValue();
                const auto bottomRight = radiiArray[2].As<Napi::Number>().FloatValue();

                nvgRoundedRectVarying(*m_nvg, x, y, width, height, topLeft, topRightBottomLeft, bottomRight, topRightBottomLeft);
            }
            else if (radiiArrayLength == 4)
            {
                const auto topLeft = radiiArray[0u].As<Napi::Number>().FloatValue();
                const auto topRight = radiiArray[1].As<Napi::Number>().FloatValue();
                const auto bottomRight = radiiArray[2].As<Napi::Number>().FloatValue();
                const auto bottomLeft = radiiArray[3].As<Napi::Number>().FloatValue();

                nvgRoundedRectVarying(*m_nvg, x, y, width, height, topLeft, topRight, bottomRight, bottomLeft);
            }
            else
            {
                throw Napi::Error::New(info.Env(), "Invalid number of parameters for radii");
            }
        }

        m_rectangleClipping = {x, y, width, height};
        SetDirty();
    }

    void Context::Clip(const Napi::CallbackInfo& /*info*/)
    {
        m_isClipped = true;

        //By default m_rectangleClipping is not set, in this case we use the default render target width and height.
        auto w = m_rectangleClipping.width != 0 ? m_rectangleClipping.width : m_canvas->GetFrameBuffer().Width();
        auto h = m_rectangleClipping.height != 0 ? m_rectangleClipping.height : m_canvas->GetFrameBuffer().Height();

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
        SetFilterStack();
        nvgStroke(*m_nvg);
        SetDirty();
    }

    void Context::Stroke(const Napi::CallbackInfo& info)
    {
        // draw Path2D if exists
        const NativeCanvasPath2D* path = info.Length() == 1 ? NativeCanvasPath2D::Unwrap(info[0].As<Napi::Object>()) : nullptr;
        if (path != nullptr)
        {
            nvgBeginPath(*m_nvg);
            for (const auto& command : *path)
            {
                const auto args = command.args;
                bool setDirty = true;
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
                    case P2D_TRANSFORM:
                        nvgTransform(*m_nvg,
                            args.transform.a, args.transform.b, args.transform.c,
                            args.transform.d, args.transform.e, args.transform.f);
                        break;
                    default:
                        setDirty = false; // noop
                        break;
                }
                if (setDirty)
                {
                    SetDirty();
                }
            }
        }

        SetFilterStack();
        nvgStroke(*m_nvg);
        SetDirty();
    }

    void Context::MoveTo(const Napi::CallbackInfo& info)
    {
        const auto x = info[0].As<Napi::Number>().FloatValue();
        const auto y = info[1].As<Napi::Number>().FloatValue();

        nvgMoveTo(*m_nvg, x, y);
        SetDirty();
    }

    void Context::LineTo(const Napi::CallbackInfo& info)
    {
        const auto x = info[0].As<Napi::Number>().FloatValue();
        const auto y = info[1].As<Napi::Number>().FloatValue();

        nvgLineTo(*m_nvg, x, y);
        SetDirty();
    }

    void Context::QuadraticCurveTo(const Napi::CallbackInfo& info)
    {
        const auto cx = info[0].As<Napi::Number>().FloatValue();
        const auto cy = info[1].As<Napi::Number>().FloatValue();
        const auto x = info[2].As<Napi::Number>().FloatValue();
        const auto y = info[3].As<Napi::Number>().FloatValue();

        nvgBezierTo(*m_nvg, cx, cy, cx, cy, x, y);
        SetDirty();
    }

    Napi::Value Context::MeasureText(const Napi::CallbackInfo& info)
    {
        std::string text{info[0].As<Napi::String>()};
        return MeasureText::CreateInstance(info.Env(), this, text);
    }

    bool Context::SetFontFaceId()
    {
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
        const std::string text = info[0].As<Napi::String>().Utf8Value();
        auto x = info[1].As<Napi::Number>().FloatValue();
        auto y = info[2].As<Napi::Number>().FloatValue();

        if (SetFontFaceId())
        {
            BindFillStyle(info, 0.f, 0.f, x, y);

            if (m_filter.length())
            {
                nanovg_filterstack filterStack;
                filterStack.ParseString(m_filter);
                nvgFilterStack(*m_nvg, filterStack); // sets filterStack on nanovg
            }

            nvgText(*m_nvg, x, y, text.c_str(), nullptr);
            SetDirty();
        }
    }

    void Context::SetDirty()
    {
        if (!m_dirty)
        {
            m_dirty = true;
            DeferredFlushFrame();
        }
    }

    void Context::DeferredFlushFrame()
    {
        // on some systems (Ubuntu), the framebuffer contains garbage.
        // Unlike other systems where it's cleared.
        bool needClear = m_canvas->UpdateRenderTarget();

        arcana::make_task(m_update.Scheduler(), *m_cancellationSource, [this, needClear, cancellationSource{m_cancellationSource}]() {
            return arcana::make_task(m_runtimeScheduler, *m_cancellationSource, [this, needClear, updateToken{m_update.GetUpdateToken()}, cancellationSource{m_cancellationSource}]() {
                // JS Thread
                Graphics::FrameBuffer& frameBuffer = m_canvas->GetFrameBuffer();
                bgfx::Encoder* encoder = m_update.GetUpdateToken().GetEncoder();
                frameBuffer.Bind(*encoder);
                if (needClear)
                {
                    frameBuffer.Clear(*encoder, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH | BGFX_CLEAR_STENCIL, 0, 1.f, 0);
                }
                frameBuffer.SetViewPort(*encoder, 0.f, 0.f, 1.f, 1.f);
                const auto width = m_canvas->GetWidth();
                const auto height = m_canvas->GetHeight();

                // we reuse progs + uniforms across calls between frames
                nanovg_filterstack::InitBgfx();
                for (auto& buffer : m_canvas->mPoolBuffers)
                {
                    // sanity check no buffers should have been acquired yet
                    assert(buffer.isAvailable == true);
                    // buffer.frameBuffer->Clear(*encoder, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH | BGFX_CLEAR_STENCIL, 0, 1.f, 0); // TODO: confirm that this not necessary because of ScreenSpaceQuad
                }
                std::function<Babylon::Graphics::FrameBuffer*()> acquire = [this, encoder]() -> Babylon::Graphics::FrameBuffer* {
                    Babylon::Graphics::FrameBuffer *frameBuffer = this->m_canvas->PoolAcquire();
                    frameBuffer->Bind(*encoder);
                    return frameBuffer;
                };
                std::function<void(Babylon::Graphics::FrameBuffer*)> release = [this, encoder](Babylon::Graphics::FrameBuffer* frameBuffer) -> void {
                    frameBuffer->Unbind(*encoder);
                    this->m_canvas->PoolRelease(frameBuffer);
                };

                nvgBeginFrame(*m_nvg, float(width), float(height), 1.0f);
                nvgSetFrameBufferAndEncoder(*m_nvg, frameBuffer, encoder);
                nvgSetTargetManager({
                    acquire,
                    release
                });
                nvgEndFrame(*m_nvg);
                frameBuffer.Unbind(*encoder);

                for (auto& buffer : m_canvas->mPoolBuffers)
                {
                    // sanity check no unreleased buffers
                    assert(buffer.isAvailable == true);
                }
                nanovg_filterstack::DisposeBgfx();

                m_dirty = false;
            }).then(arcana::inline_scheduler, *m_cancellationSource, [this, cancellationSource{m_cancellationSource}](const arcana::expected<void, std::exception_ptr>& result) {
                if (!cancellationSource->cancelled() && result.has_error())
                {
                    Napi::Error::New(Env(), result.error()).ThrowAsJavaScriptException();
                }
            });
        });
    }

    void Context::PutImageData(const Napi::CallbackInfo&)
    {
        throw std::runtime_error{"not implemented"};
    }

    void Context::Arc(const Napi::CallbackInfo& info)
    {
        const auto x = static_cast<float>(info[0].As<Napi::Number>().DoubleValue());
        const auto y = static_cast<float>(info[1].As<Napi::Number>().DoubleValue());
        const auto radius = static_cast<float>(info[2].As<Napi::Number>().DoubleValue());
        const auto startAngle = static_cast<float>(info[3].As<Napi::Number>().DoubleValue());
        const auto endAngle = static_cast<float>(info[4].As<Napi::Number>().DoubleValue());
        const NVGwinding winding = (info.Length() == 6 && info[5].As<Napi::Boolean>()) ? NVGwinding::NVG_CCW : NVGwinding::NVG_CW;
        nvgArc(*m_nvg, x, y, radius, startAngle, endAngle, winding);
        SetDirty();
    }

    void Context::DrawImage(const Napi::CallbackInfo& info)
    {
        const NativeCanvasImage* canvasImage = NativeCanvasImage::Unwrap(info[0].As<Napi::Object>());

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

        if (info.Length() == 3)
        {
            const auto dx = static_cast<float>(info[1].As<Napi::Number>().Int32Value());
            const auto dy = static_cast<float>(info[2].As<Napi::Number>().Int32Value());
            const auto width = static_cast<float>(canvasImage->GetWidth());
            const auto height = static_cast<float>(canvasImage->GetHeight());

            NVGpaint imagePaint = nvgImagePattern(*m_nvg, 0.f, 0.f, width, height, 0.f, imageIndex, 1.f);

            if (!m_isClipped)
            {
                nvgBeginPath(*m_nvg);
            }

            nvgRect(*m_nvg, dx, dy, width, height);
            nvgFillPaint(*m_nvg, imagePaint);
            SetFilterStack();
            nvgFill(*m_nvg);
            SetDirty();
        }
        else if (info.Length() == 5)
        {
            const auto dx = static_cast<float>(info[1].As<Napi::Number>().Int32Value());
            const auto dy = static_cast<float>(info[2].As<Napi::Number>().Int32Value());
            const auto dWidth = static_cast<float>(info[3].As<Napi::Number>().Uint32Value());
            const auto dHeight = static_cast<float>(info[4].As<Napi::Number>().Uint32Value());

            NVGpaint imagePaint = nvgImagePattern(*m_nvg, dx, dy, dWidth, dHeight, 0.f, imageIndex, 1.f);

            if (!m_isClipped)
            {
                nvgBeginPath(*m_nvg);
            }

            nvgRect(*m_nvg, dx, dy, dWidth, dHeight);
            nvgFillPaint(*m_nvg, imagePaint);
            SetFilterStack();
            nvgFill(*m_nvg);
            SetDirty();
        }
        else if (info.Length() == 9)
        {
            const auto sx = info[1].As<Napi::Number>().Int32Value();
            const auto sy = info[2].As<Napi::Number>().Int32Value();
            const auto sWidth = info[3].As<Napi::Number>().Uint32Value();
            const auto sHeight = info[4].As<Napi::Number>().Uint32Value();
            const auto dx = static_cast<float>(info[5].As<Napi::Number>().Int32Value());
            const auto dy = static_cast<float>(info[6].As<Napi::Number>().Int32Value());
            const auto dWidth = static_cast<float>(info[7].As<Napi::Number>().Uint32Value());
            const auto dHeight = static_cast<float>(info[8].As<Napi::Number>().Uint32Value());
            const auto width = static_cast<float>(canvasImage->GetWidth());
            const auto height = static_cast<float>(canvasImage->GetHeight());

            NVGpaint imagePaint = nvgImagePattern(*m_nvg, dx, dy, dWidth, dHeight, 0.f, imageIndex, 1.f);

            if (!m_isClipped)
            {
                nvgBeginPath(*m_nvg);
            }

            nvgRect(*m_nvg, dx, dy, dWidth, dHeight);
            nvgFillPaint(*m_nvg, imagePaint);
            SetFilterStack();
            nvgFill(*m_nvg);
            SetDirty();
        }
        else
        {
            throw Napi::Error::New(info.Env(), "Invalid number of parameters for DrawImage");
        }
    }

    Napi::Value Context::GetImageData(const Napi::CallbackInfo& info)
    {
        // TODO: support source x and y
        //const auto sx = info[0].As<Napi::Number>().Uint32Value();
        //const auto sy = info[1].As<Napi::Number>().Uint32Value();
        const auto sw = info[2].As<Napi::Number>().Uint32Value();
        const auto sh = info[3].As<Napi::Number>().Uint32Value();

        return ImageData::CreateInstance(info.Env(), this, sw, sh);
    }

    void Context::SetLineDash(const Napi::CallbackInfo& info)
    {
        throw Napi::Error::New(info.Env(), "not implemented");
    }

    void Context::StrokeText(const Napi::CallbackInfo& info)
    {
        const std::string text = info[0].As<Napi::String>().Utf8Value();
        auto x = info[1].As<Napi::Number>().FloatValue();
        auto y = info[2].As<Napi::Number>().FloatValue();

        if (SetFontFaceId())
        {
            nvgStrokeText(*m_nvg, x, y, text.c_str(), nullptr);
            SetDirty();
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

        Napi::Object obj = Napi::Object::New(Env());
        obj.Set("a", xform[0]);
        obj.Set("b", xform[1]);
        obj.Set("c", xform[2]);
        obj.Set("d", xform[3]);
        obj.Set("e", xform[4]);
        obj.Set("f", xform[5]);
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
        SetDirty();
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
        SetDirty();
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
        SetDirty();
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
        SetDirty();
    }

    Napi::Value Context::GetMiterLimit(const Napi::CallbackInfo& info)
    {
        return Napi::Value::From(Env(), m_miterLimit);
    }

    void Context::SetMiterLimit(const Napi::CallbackInfo& info, const Napi::Value& value)
    {
        m_miterLimit = value.As<Napi::Number>().FloatValue();
        nvgMiterLimit(*m_nvg, m_miterLimit);
        SetDirty();
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

    Napi::Value Context::GetFont(const Napi::CallbackInfo& info)
    {
        return Napi::Value::From(Env(), m_font);
    }

    void Context::SetFont(const Napi::CallbackInfo& info, const Napi::Value& value)
    {
        if (!value.IsString())
        {
            throw Napi::Error::New(info.Env(), "invalid argument");
        }

        const std::string fontOptions = value.ToString();

        // Default font id, and font size values.
        // TODO: Determine better way of signaling to user that font specified is invalid.
        m_currentFontId = -1;
        float fontSize{16.f};

        // Regex to parse font styling information. For now we are only capturing font size (capture group 3) and font family name (capture group 4).
        static const std::regex fontStyleRegex("([[a-zA-Z]+\\s+)*((\\d+(\\.\\d+)?)px\\s+)?(\\w+)");
        std::smatch fontStyleMatch;

        // Perform the actual regex_match.
        if (std::regex_match(fontOptions, fontStyleMatch, fontStyleRegex))
        {
            // Check if font size was specified.
            if (fontStyleMatch[3].matched)
            {
                fontSize = std::stof(fontStyleMatch[3]);
            }

            // Check if the specified font family name is valid, and if so assign the current font id.
            if (m_fonts.find(fontStyleMatch[4]) != m_fonts.end())
            {
                m_currentFontId = m_fonts.at(fontStyleMatch[4]);
                m_font = fontOptions;
            }
        }

        // Set font size on the current context.
        nvgFontSize(*m_nvg, fontSize);
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
            m_letterSpacing = std::stof(letterSpacingMatch[1]);
        }
        nvgTextLetterSpacing(*m_nvg, m_letterSpacing);
    }

    void Context::SetGlobalAlpha(const Napi::CallbackInfo& info, const Napi::Value& value)
    {
        const float alpha = value.As<Napi::Number>().FloatValue();
        nvgGlobalAlpha(*m_nvg, alpha);
    }

    Napi::Value Context::GetShadowColor(const Napi::CallbackInfo& info)
    {
        throw Napi::Error::New(info.Env(), "not implemented");
    }

    void Context::SetShadowColor(const Napi::CallbackInfo& info, const Napi::Value& value)
    {
        throw Napi::Error::New(info.Env(), "not implemented");
    }

    Napi::Value Context::GetShadowBlur(const Napi::CallbackInfo& info)
    {
        throw Napi::Error::New(info.Env(), "not implemented");
    }

    void Context::SetShadowBlur(const Napi::CallbackInfo& info, const Napi::Value& value)
    {
        throw Napi::Error::New(info.Env(), "not implemented");
    }

    Napi::Value Context::GetShadowOffsetX(const Napi::CallbackInfo& info)
    {
        throw Napi::Error::New(info.Env(), "not implemented");
    }

    void Context::SetShadowOffsetX(const Napi::CallbackInfo& info, const Napi::Value& value)
    {
        throw Napi::Error::New(info.Env(), "not implemented");
    }

    Napi::Value Context::GetShadowOffsetY(const Napi::CallbackInfo& info)
    {
        throw Napi::Error::New(info.Env(), "not implemented");
    }

    void Context::SetShadowOffsetY(const Napi::CallbackInfo& info, const Napi::Value& value)
    {
        throw Napi::Error::New(info.Env(), "not implemented");
    }

    Napi::Value Context::GetCanvas(const Napi::CallbackInfo& info)
    {
        throw Napi::Error::New(info.Env(), "not implemented");
    }
}
