#include <bx/math.h>
#include <map>
#include <algorithm>
#include <assert.h>
#include "nanovg/nanovg.h"
#define STB_TRUETYPE_IMPLEMENTATION
#include "stb/stb_truetype.h"
#include "Canvas.h"
#include "Context.h"
#include "MeasureText.h"

namespace Babylon::Polyfills::Internal
{
    Napi::Value Context::CreateInstance(Napi::Env env, Canvas* canvas, uint32_t viewId)
    {
        Napi::HandleScope scope{ env };

        Napi::Function func = ParentT::DefineClass(
            env,
            JS_CONSTRUCTOR_NAME,
            {
                ParentT::InstanceMethod("fillRect", &Context::FillRect),
                ParentT::InstanceMethod("measureText", &Context::MeasureText),
                ParentT::InstanceMethod("fillText", &Context::FillText),
                ParentT::InstanceMethod("fill", &Context::Fill),
                ParentT::InstanceMethod("save", &Context::Save),
                ParentT::InstanceMethod("restore", &Context::Restore),
                ParentT::InstanceMethod("clearRect", &Context::ClearRect),
                ParentT::InstanceMethod("translate", &Context::Translate),
                ParentT::InstanceMethod("rotate", &Context::Rotate),
                ParentT::InstanceMethod("scale", &Context::Scale),
                ParentT::InstanceMethod("beginPath", &Context::BeginPath),
                ParentT::InstanceMethod("closePath", &Context::ClosePath),
                ParentT::InstanceMethod("rect", &Context::Rect),
                ParentT::InstanceMethod("clip", &Context::Clip),
                ParentT::InstanceMethod("strokeRect", &Context::StrokeRect),
                ParentT::InstanceMethod("stroke", &Context::Stroke),
                ParentT::InstanceMethod("moveTo", &Context::MoveTo),
                ParentT::InstanceMethod("lineTo", &Context::LineTo),
                ParentT::InstanceMethod("quadraticCurveTo", &Context::QuadraticCurveTo),
                InstanceAccessor("fillStyle", &Context::GetFillStyle, &Context::SetFillStyle),
                InstanceAccessor("strokeStyle", &Context::GetStrokeStyle, &Context::SetStrokeStyle),
                InstanceAccessor("lineWidth", &Context::GetLineWidth, &Context::SetLineWidth),
            });
        return func.New({ Napi::External<Canvas>::New(env, canvas), Napi::Value::From(env, viewId)});
    }

    Context::Context(const Napi::CallbackInfo& info)
        : ParentT{ info }
        , m_canvas{ info[0].As<Napi::External<Canvas>>().Data() }
        , m_viewId{ static_cast<bgfx::ViewId>(info[1].As<Napi::Number>().Uint32Value()) }
        , m_nvg{ nvgCreate(1, m_viewId) }
    {
        for (auto& font : Canvas::fontsInfos)
        {
            m_fonts[font.first] = nvgCreateFontMem(m_nvg, font.first.c_str(), font.second.data(), font.second.size(), 0);
        }
        registeredContexts.push_back(this);
    }

    Context::~Context()
    {
        nvgDelete(m_nvg);

        auto iter = std::find(registeredContexts.begin(), registeredContexts.end(), this);
        if (iter != registeredContexts.end())
        {
            registeredContexts.erase(iter);
        }
    }

    void Context::BeginContextsFrame()
    {
        for (auto& context : registeredContexts)
        {
            context->BeginFrame();
        }
    }
    void Context::EndContextsFrame()
    {
        for (auto& context : registeredContexts)
        {
            context->EndFrame();
        }
    }

    NVGcolor StringToColor(const std::string& colorString)
    {
        std::string str = colorString;
        std::transform(str.begin(), str.end(), str.begin(),
            [](unsigned char c) { return std::tolower(c); });

        if (str == "white") {
            return nvgRGBA(255, 255, 255, 255);
        }
        else if (str == "green") {
            return nvgRGBA(0, 255, 0, 255);
        }
        else if (str == "red") {
            return nvgRGBA(255, 0, 0, 255);
        }
        else if (str == "blue") {
            return nvgRGBA(0, 0, 255, 255);
        }
        else if (str == "grey") {
            return nvgRGBA(128, 128, 128, 255);
        }
        else if (str == "transparent") {
            return nvgRGBA(0, 0, 0, 0);
        }
        else if (str[0] == '#') {
            unsigned int components[4];
            int count = sscanf(str.c_str(), "#%02x%02x%02x%02x", &components[0], &components[1], &components[2], &components[3]);
            for (int i = count; count < 4; count++)
            {
                components[i] = 255;
            }
            return nvgRGBA(components[0], components[1], components[2], components[3]);
        }
        else {
            assert(0);
        }
        return nvgRGBA(255, 0, 255, 255);
    }

    void Context::FillRect(const Napi::CallbackInfo& info)
    {
        auto left = info[0].As<Napi::Number>().FloatValue();
        auto top = info[1].As<Napi::Number>().FloatValue();
        auto width = info[2].As<Napi::Number>().FloatValue();
        auto height = info[3].As<Napi::Number>().FloatValue();
        //nvgRect(m_nvg, left, top, width, height);

        NVGpaint paint = nvgLinearGradient(m_nvg, 0, 5, 0, 10, nvgRGBA(0, 160, 192, 255), nvgRGBA(0, 160, 192, 255));
        nvgBeginPath(m_nvg);
        nvgRect(m_nvg, left, top, width, height);

        nvgFillPaint(m_nvg, paint);
        nvgFill(m_nvg);
    }

    Napi::Value Context::GetFillStyle(const Napi::CallbackInfo&)
    {
        return Napi::Value::From(Env(), 0);
    }

    void Context::SetFillStyle(const Napi::CallbackInfo&, const Napi::Value& value)
    {
        const auto color = StringToColor(value.As<Napi::String>().Utf8Value());
        nvgFillColor(m_nvg, color);
    }

    Napi::Value Context::GetStrokeStyle(const Napi::CallbackInfo&)
    {
        return Napi::Value::From(Env(), 0);
    }

    void Context::SetStrokeStyle(const Napi::CallbackInfo&, const Napi::Value& value)
    {
        auto color = StringToColor(value.As<Napi::String>().Utf8Value());
        nvgStrokeColor(m_nvg, color);
    }

    Napi::Value Context::GetLineWidth(const Napi::CallbackInfo& )
    {
        return Napi::Value::From(Env(), 0);
    }

    void Context::SetLineWidth(const Napi::CallbackInfo&, const Napi::Value& value)
    {
        const auto width = value.As<Napi::Number>().FloatValue();
        nvgStrokeWidth(m_nvg, width);
    }

    void Context::Fill(const Napi::CallbackInfo&)
    {
        nvgFill(m_nvg);
    }

    void Context::Save(const Napi::CallbackInfo&)
    {
        nvgSave(m_nvg);
    }

    void Context::Restore(const Napi::CallbackInfo&)
    {
        nvgRestore(m_nvg);
    }

    void Context::ClearRect(const Napi::CallbackInfo&)
    {
    }

    void Context::Translate(const Napi::CallbackInfo& info)
    {
        const auto x = info[0].As<Napi::Number>().FloatValue();
        const auto y = info[1].As<Napi::Number>().FloatValue();
        nvgTranslate(m_nvg, x, y);
    }

    void Context::Rotate(const Napi::CallbackInfo& info)
    {
        const auto angle = info[0].As<Napi::Number>().FloatValue();
        nvgRotate(m_nvg, nvgDegToRad(angle));
    }

    void Context::Scale(const Napi::CallbackInfo& info)
    {
        const auto x = info[0].As<Napi::Number>().FloatValue();
        const auto y = info[1].As<Napi::Number>().FloatValue();
        nvgScale(m_nvg, x, y);
    }

    void Context::BeginPath(const Napi::CallbackInfo&)
    {
        nvgBeginPath(m_nvg);
    }

    void Context::ClosePath(const Napi::CallbackInfo&)
    {
        nvgClosePath(m_nvg);
    }

    void Context::Rect(const Napi::CallbackInfo& info)
    {
        const auto left = info[0].As<Napi::Number>().FloatValue();
        const auto top = info[1].As<Napi::Number>().FloatValue();
        const auto width = info[2].As<Napi::Number>().FloatValue();
        const auto height = info[3].As<Napi::Number>().FloatValue();
        nvgRect(m_nvg, left, top, width, height);
    }

    void Context::Clip(const Napi::CallbackInfo&)
    {
    }

    void Context::StrokeRect(const Napi::CallbackInfo& info)
    {
        const auto left = info[0].As<Napi::Number>().FloatValue();
        const auto top = info[1].As<Napi::Number>().FloatValue();
        const auto width = info[2].As<Napi::Number>().FloatValue();
        const auto height = info[3].As<Napi::Number>().FloatValue();
        nvgRect(m_nvg, left, top, width, height);
    }

    void Context::Stroke(const Napi::CallbackInfo&)
    {
    }

    void Context::MoveTo(const Napi::CallbackInfo& info)
    {
        const auto x = info[0].As<Napi::Number>().FloatValue();
        const auto y = info[1].As<Napi::Number>().FloatValue();
        nvgMoveTo(m_nvg, x, y);
    }

    void Context::LineTo(const Napi::CallbackInfo& info)
    {
        const auto x = info[0].As<Napi::Number>().FloatValue();
        const auto y = info[1].As<Napi::Number>().FloatValue();
        nvgLineTo(m_nvg, x, y);
    }

    void Context::QuadraticCurveTo(const Napi::CallbackInfo& info)
    {
        const auto cx = info[0].As<Napi::Number>().FloatValue();
        const auto cy = info[1].As<Napi::Number>().FloatValue();
        const auto x = info[2].As<Napi::Number>().FloatValue();
        const auto y = info[3].As<Napi::Number>().FloatValue();
        nvgBezierTo(m_nvg, cx, cy, cx, cy, x, y);
    }

    Napi::Value Context::MeasureText(const Napi::CallbackInfo& info)
    {
        const std::string text = info[0].As<Napi::String>().Utf8Value();
        return MeasureText::CreateInstance(info.Env(), this, text);
    }

    void Context::FillText(const Napi::CallbackInfo& info)
    {
        const std::string text = info[0].As<Napi::String>().Utf8Value();
        auto x = info[1].As<Napi::Number>().FloatValue();
        auto y = info[2].As<Napi::Number>().FloatValue();

        if (!m_fonts.empty())
        {
            nvgFontFaceId(m_nvg, m_fonts.begin()->second);
            nvgText(m_nvg, x, y, text.c_str(), nullptr);
        }
    }

    void Context::BeginFrame()
    {
        const auto width = m_canvas->GetWidth();
        const auto height = m_canvas->GetHeight();

        bgfx::setViewFrameBuffer(m_viewId, m_canvas->GetFrameBufferHandle());
        bgfx::setViewClear(m_viewId, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0, 1.0f, 0);
        bgfx::setViewMode(m_viewId, bgfx::ViewMode::Sequential);

        bgfx::discard();
        
        nvgBeginFrame(m_nvg, float(width), float(height), 1.0f);

        const bgfx::Caps* caps = bgfx::getCaps();
        bool flipY = bgfx::getCaps()->originBottomLeft;
        if (!flipY)
        {
            nvgScale(m_nvg, 1.f, -1.f);
            nvgTranslate(m_nvg, 0.f, -float(height));
        }
    }

    void Context::EndFrame()
    {
        bgfx::discard();
        nvgEndFrame(m_nvg);
    }
}
