#pragma once

#include <Babylon/Polyfills/Canvas.h>

namespace Babylon::Polyfills::Internal
{
    class Canvas final : public Napi::ObjectWrap<Canvas>
    {
    public:
        static constexpr auto JS_CONSTRUCTOR_NAME = "OffscreenCanvas";

        using ParentT = Napi::ObjectWrap<Canvas>;

        static void CreateInstance(Napi::Env env);

        explicit Canvas(const Napi::CallbackInfo& info);

        uint32_t GetWidth() const { return m_width; }
        uint32_t GetHeight() const { return m_height; }

        struct FontsInfo
        {
            FontsInfo(uint32_t atlasSize)
                : fontManager(atlasSize)
            {
            }
            FontManager fontManager;
            FontHandle font;
        };

        static inline std::unique_ptr<FontsInfo> fontsInfos;
    private:

        Napi::Value GetContext(const Napi::CallbackInfo&);
        Napi::Value GetWidth(const Napi::CallbackInfo&);
        void SetWidth(const Napi::CallbackInfo&, const Napi::Value& value);
        Napi::Value GetHeight(const Napi::CallbackInfo&);
        void SetHeight(const Napi::CallbackInfo&, const Napi::Value& value);
        Napi::Value GetCanvasTexture(const Napi::CallbackInfo& info);
        static void LoadTTF(const Napi::CallbackInfo& info);

        uint32_t m_width{1};
        uint32_t m_height{1};
    };

    class Context final : public Napi::ObjectWrap<Context>
    {
    public:
        static constexpr auto JS_CONSTRUCTOR_NAME = "Context";

        using ParentT = Napi::ObjectWrap<Context>;

        static Napi::Value CreateInstance(Napi::Env env, Canvas* canvas);

        explicit Context(const Napi::CallbackInfo& info);

        

    private:

        void FillRect(const Napi::CallbackInfo&);
        Napi::Value MeasureText(const Napi::CallbackInfo&);
        void FillText(const Napi::CallbackInfo&);

        static inline bgfx::FrameBufferHandle frameBufferHandle{bgfx::kInvalidHandle};
        static void UpdateRenderTarget(uint32_t width, uint32_t height);

        Canvas* m_canvas;

        TextBufferManager m_textBufferManager;
        TextBufferHandle m_transientText;


        friend class Canvas;
    };
}
