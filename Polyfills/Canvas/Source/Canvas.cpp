#include <bx/math.h>
#include <bgfx/bgfx.h>
#include "font/font_manager.h"
#include "font/text_buffer_manager.h"
#include "nanovg/nanovg.h"

#include "Canvas.h"

#include <functional>
#include <sstream>
#define STB_TRUETYPE_IMPLEMENTATION
#include "stb/stb_truetype.h"

namespace Babylon::Polyfills::Internal
{
    void Canvas::CreateInstance(Napi::Env env)
    {
        Napi::HandleScope scope{env};

        Napi::Function func = ParentT::DefineClass(
            env,
            JS_CONSTRUCTOR_NAME,
            {
                ParentT::StaticMethod("loadTTF", &Canvas::LoadTTF),
                ParentT::InstanceMethod("getContext", &Canvas::GetContext),
                InstanceAccessor("width", &Canvas::GetWidth, &Canvas::SetWidth),
                InstanceAccessor("height", &Canvas::GetHeight, &Canvas::SetHeight),
                ParentT::InstanceMethod("getCanvasTexture", &Canvas::GetCanvasTexture),
                
            });

        env.Global().Set(JS_CONSTRUCTOR_NAME, func);
    }

    Canvas::Canvas(const Napi::CallbackInfo& info)
        : ParentT{info}
    {
    }

    void Canvas::LoadTTF(const Napi::CallbackInfo& info)
    {
        std::string fontName = info[0].As<Napi::String>().Utf8Value();
        const auto buffer = info[1].As<Napi::ArrayBuffer>();
        
        if (!fontsInfos)
        {
            // Init the text rendering system.
            fontsInfos = std::make_unique<FontsInfo>(512);
        }
        TrueTypeHandle handle = fontsInfos->fontManager.createTtf((uint8_t*)buffer.Data(), buffer.ByteLength());
        fontsInfos->font = fontsInfos->fontManager.createFontByPixelSize(handle, 0, 32);
        fontsInfos->fontManager.preloadGlyph(fontsInfos->font, L"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.!@#$%^&*()[]{};:<>?,/\\ \n");
        fontsInfos->fontManager.destroyTtf(handle);
    }

    Napi::Value Canvas::GetContext(const Napi::CallbackInfo& info)
    {
        return Context::CreateInstance(info.Env(), this);
    }

    Napi::Value Canvas::GetWidth(const Napi::CallbackInfo&)
    {
        return Napi::Value::From(Env(), m_width);
    }

    void Canvas::SetWidth(const Napi::CallbackInfo&, const Napi::Value& value)
    {
        auto width = value.As<Napi::Number>().Uint32Value();
        if (width != m_width && width)
        {
            m_width = width;
            Context::UpdateRenderTarget(m_width, m_height);
        }
    }

    Napi::Value Canvas::GetHeight(const Napi::CallbackInfo&)
    {
        return Napi::Value::From(Env(), m_height);
    }

    void Canvas::SetHeight(const Napi::CallbackInfo&, const Napi::Value& value)
    {
        auto height = value.As<Napi::Number>().Uint32Value();
        if (height != m_height && height)
        {
            m_height = height;
            Context::UpdateRenderTarget(m_width, m_height);
        }
    }

    struct TextureData final
    {
        ~TextureData()
        {
            if (bgfx::isValid(Handle))
            {
                bgfx::destroy(Handle);
            }
        }

        bgfx::TextureHandle Handle{ bgfx::kInvalidHandle };
        uint32_t Width{ 0 };
        uint32_t Height{ 0 };
        uint32_t Flags{ 0 };
        uint8_t AnisotropicLevel{ 0 };
    };

    Napi::Value Canvas::GetCanvasTexture(const Napi::CallbackInfo& info)
    {
        auto data = new TextureData();
        data->Handle = bgfx::getTexture(Context::frameBufferHandle);
        data->Width = m_width;
        data->Height = m_height;
        return Napi::External<TextureData>::New(info.Env(), data);
    }

    // Context
    void Context::UpdateRenderTarget(uint32_t width, uint32_t height)
    {
        if (frameBufferHandle.idx != bgfx::kInvalidHandle)
        {
            bgfx::destroy(frameBufferHandle);
        }
        frameBufferHandle = bgfx::createFrameBuffer(static_cast<uint16_t>(width), static_cast<uint16_t>(height), bgfx::TextureFormat::RGBA8, BGFX_TEXTURE_RT);
    }

    Napi::Value Context::CreateInstance(Napi::Env env, Canvas* canvas)
    {
        Napi::HandleScope scope{ env };

        // defined at first call, makes construction faster for subsequent calls.
        static Napi::Function func = ParentT::DefineClass(
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
                
            });
        return func.New({ Napi::External<Canvas>::New(env, canvas) });
    }

    Context::Context(const Napi::CallbackInfo& info)
        : ParentT{ info }
        , m_canvas {info[0].As<Napi::External<Canvas>>().Data()}
        , m_textBufferManager(&Canvas::fontsInfos->fontManager)
        , m_transientText{m_textBufferManager.createTextBuffer(FONT_TYPE_DISTANCE_SUBPIXEL, BufferType::Transient)}
        , m_nvg{nvgCreate(1, 0)}
    {
    }

    Context::~Context()
    {
        nvgDelete(m_nvg);
    }

    void Context::FillRect(const Napi::CallbackInfo&)
    {
    }

    void Context::Fill(const Napi::CallbackInfo&)
    {
    }

    void Context::Save(const Napi::CallbackInfo&)
    {
        nvgSave(m_nvg);

        // Draw first rect and set scissor to it's area.
        
        
        
        
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
        auto x = info[0].As<Napi::Number>().FloatValue();
        auto y = info[1].As<Napi::Number>().FloatValue();
        nvgTranslate(m_nvg, x, y);
    }

    void Context::Rotate(const Napi::CallbackInfo& info)
    {
        auto angle = info[0].As<Napi::Number>().FloatValue();
        nvgRotate(m_nvg, nvgDegToRad(angle));
    }

    void Context::Scale(const Napi::CallbackInfo& info)
    {
        auto x = info[0].As<Napi::Number>().FloatValue();
        auto y = info[1].As<Napi::Number>().FloatValue();
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
        auto left = info[0].As<Napi::Number>().FloatValue();
        auto top = info[1].As<Napi::Number>().FloatValue();
        auto bottom = info[2].As<Napi::Number>().FloatValue();
        auto right = info[3].As<Napi::Number>().FloatValue();
        nvgRect(m_nvg, left, top, bottom, right);
    }

    void Context::Clip(const Napi::CallbackInfo&)
    {
    }

    void Context::StrokeRect(const Napi::CallbackInfo&)
    {
    }

    void Context::Stroke(const Napi::CallbackInfo&)
    {
    }

    void Context::MoveTo(const Napi::CallbackInfo&)
    {
    }

    void Context::LineTo(const Napi::CallbackInfo&)
    {
    }

    void Context::QuadraticCurveTo(const Napi::CallbackInfo&)
    {
    }

    Napi::Value Context::MeasureText(const Napi::CallbackInfo& info)
    {
        return Napi::Value::From(info.Env(), 8);
    }

    void Context::FillText(const Napi::CallbackInfo& info)
    {
        auto fontsInfos = m_canvas->fontsInfos.get();

        if (!fontsInfos)
        {
            return;
        }

        std::string text = info[0].As<Napi::String>().Utf8Value();
        auto x = info[1].As<Napi::Number>().FloatValue();
        auto y = info[2].As<Napi::Number>().FloatValue();

        m_textBufferManager.clearTextBuffer(m_transientText);
        m_textBufferManager.setTextColor(m_transientText, 0xFFFFFFFF);
        m_textBufferManager.appendText(m_transientText, fontsInfos->font, text.c_str());

        bgfx::ViewId canvasViewId = 100;
        bgfx::setViewFrameBuffer(canvasViewId, frameBufferHandle);
        bgfx::setViewClear(canvasViewId, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0, 1.0f, 0);
        const bx::Vec3 at = { -x, -y,  0.0f };
        const bx::Vec3 eye = { -x, -y, -1.0f };


        x = 0;
        float view[16];
        bx::mtxLookAt(view, eye, at);
        //bx::mtxTranslate(view, -x, -y, 1.f);


        const float centering = 0.5f;

        auto width = m_canvas->GetWidth();
        auto height = m_canvas->GetHeight();

        // Setup a top-left ortho matrix for screen space drawing.
        const bgfx::Caps* caps = bgfx::getCaps();
        bool flipY = bgfx::getCaps()->originBottomLeft;
        {
            float ortho[16];
            bx::mtxOrtho(
                ortho
                , centering
                , static_cast<float>(width) + centering
                , flipY ? (static_cast<float>(height) + centering) : centering
                , flipY ? centering : (static_cast<float>(height) + centering)
                , -100.0f
                , 100.0f
                , 0.0f
                , caps->homogeneousDepth
            );
            bgfx::setViewTransform(canvasViewId, view, ortho);
            bgfx::setViewRect(canvasViewId, 0, 0, uint16_t(width), uint16_t(height));
        }
        m_textBufferManager.submitTextBuffer(m_transientText, canvasViewId);
        //bgfx::frame();
    }
}

namespace Babylon::Polyfills::Canvas
{
    void Initialize(Napi::Env env)
    {
        Internal::Canvas::CreateInstance(env);
    }
}
