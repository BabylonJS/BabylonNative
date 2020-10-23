#include <bx/math.h>
#include <bgfx/bgfx.h>
#include "font/font_manager.h"
#include "font/text_buffer_manager.h"

#include "Canvas.h"

#include <functional>
#include <sstream>
#define STB_TRUETYPE_IMPLEMENTATION
#include "stb/stb_truetype.h"

#if defined(_MSC_VER)
#pragma warning(disable : 4996)
#endif

namespace Babylon::Polyfills::Internal
{
    inline void* load(const char* szFileName, uint32_t& bufSize)
    {
        FILE* fp = fopen(szFileName, "rb");
        if (fp)
        {
            fseek(fp, 0, SEEK_END);
            bufSize = ftell(fp);
            fseek(fp, 0, SEEK_SET);
            char* buf = new char[bufSize];
            fread(buf, bufSize, 1, fp);
            fclose(fp);
            return buf;
        }
        return nullptr;
    }

    TrueTypeHandle loadTtf(FontManager* _fm, const char* _filePath)
    {
        uint32_t size;
        void* data = load(_filePath, size);

        if (NULL != data)
        {
            TrueTypeHandle handle = _fm->createTtf((uint8_t*)data, size);
            //BX_FREE(entry::getAllocator(), data);
            return handle;
        }

        TrueTypeHandle invalid = BGFX_INVALID_HANDLE;
        return invalid;
    }


    void Canvas::CreateInstance(Napi::Env env)
    {
        Napi::HandleScope scope{env};

        Napi::Function func = ParentT::DefineClass(
            env,
            JS_CONSTRUCTOR_NAME,
            {
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

    Napi::Value Canvas::GetContext(const Napi::CallbackInfo& info)
    {
        return Context::CreateInstance(info.Env());
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
        auto format = bgfx::TextureFormat::RGBA8;
        frameBufferHandle = bgfx::createFrameBuffer(static_cast<uint16_t>(width), static_cast<uint16_t>(height), format, BGFX_TEXTURE_RT);
    }

    Napi::Value Context::CreateInstance(Napi::Env env)
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
            });
        return func.New({});
    }

    Context::Context(const Napi::CallbackInfo& info)
        : ParentT{ info }
    {
    }

    void Context::FillRect(const Napi::CallbackInfo&)
    {
    }

    Napi::Value Context::MeasureText(const Napi::CallbackInfo& info)
    {
        return Napi::Value::From(info.Env(), 8);
    }

    void Context::FillText(const Napi::CallbackInfo& info)
    {
        std::string text = info[0].As<Napi::String>().Utf8Value();
        auto x = info[1].As<Napi::Number>().Int32Value();
        auto y = info[2].As<Napi::Number>().Int32Value();

        x;y;

        if (!m_fontManager)
        {
            // Init the text rendering system.
            m_fontManager = new FontManager(512);
            m_textBufferManager = new TextBufferManager(m_fontManager);

            m_fontFile = loadTtf(m_fontManager, "droidsans.ttf");
            m_font = m_fontManager->createFontByPixelSize(m_fontFile, 0, 32);

            // Preload glyphs and blit them to atlas.
            m_fontManager->preloadGlyph(m_font, L"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ. \n");

            // You can unload the truetype files at this stage, but in that
            // case, the set of glyph's will be limited to the set of preloaded
            // glyph.
            m_fontManager->destroyTtf(m_fontFile);

            m_transientText = m_textBufferManager->createTextBuffer(FONT_TYPE_ALPHA, BufferType::Transient);
        }
        m_textBufferManager->clearTextBuffer(m_transientText);
        m_textBufferManager->setTextColor(m_transientText, 0xFFFFFFFF);
        m_textBufferManager->appendText(m_transientText, m_font, text.c_str());

        bgfx::ViewId canvasViewId = 100;
        bgfx::setViewFrameBuffer(canvasViewId, frameBufferHandle);
        const bx::Vec3 at = { 0.0f, 0.0f,  0.0f };
        const bx::Vec3 eye = { 0.0f, 0.0f, -1.0f };

        float view[16];
        bx::mtxLookAt(view, eye, at);

        const float centering = 0.5f;

        // Setup a top-left ortho matrix for screen space drawing.
        const bgfx::Caps* caps = bgfx::getCaps();
        {
            float ortho[16];
            bx::mtxOrtho(
                ortho
                , centering
                , 512/*m_width*/ + centering
                , 512/*m_height*/ + centering
                , centering
                , 0.0f
                , 100.0f
                , 0.0f
                , caps->homogeneousDepth
            );
            bgfx::setViewTransform(canvasViewId, view, ortho);
            bgfx::setViewRect(canvasViewId, 0, 0, uint16_t(512/*m_width*/), uint16_t(512/*m_height*/));
        }
        m_textBufferManager->submitTextBuffer(m_transientText, canvasViewId);
        bgfx::frame();
    }
}

namespace Babylon::Polyfills::Canvas
{
    void Initialize(Napi::Env env)
    {
        Internal::Canvas::CreateInstance(env);
    }
}
