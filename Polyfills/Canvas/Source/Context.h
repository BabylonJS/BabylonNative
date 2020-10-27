#pragma once

#include <Babylon/Polyfills/Canvas.h>

struct NVGcontext;

namespace Babylon::Polyfills::Internal
{
    class Context final : public Napi::ObjectWrap<Context>
    {
    public:
        static constexpr auto JS_CONSTRUCTOR_NAME = "Context";

        using ParentT = Napi::ObjectWrap<Context>;

        static Napi::Value CreateInstance(Napi::Env env, Canvas* canvas, uint32_t viewId);

        explicit Context(const Napi::CallbackInfo& info);
        virtual ~Context();

        NVGcontext* GetNVGContext() const { return m_nvg; }

        static void BeginContextsFrame();
        static void EndContextsFrame();
    private:

        void FillRect(const Napi::CallbackInfo&);
        Napi::Value MeasureText(const Napi::CallbackInfo&);
        void FillText(const Napi::CallbackInfo&);
        void Fill(const Napi::CallbackInfo&);
        void Save(const Napi::CallbackInfo&);
        void Restore(const Napi::CallbackInfo&);
        void ClearRect(const Napi::CallbackInfo&);
        void Translate(const Napi::CallbackInfo&);
        void Rotate(const Napi::CallbackInfo&);
        void Scale(const Napi::CallbackInfo&);
        void BeginPath(const Napi::CallbackInfo&);
        void ClosePath(const Napi::CallbackInfo&);
        void Clip(const Napi::CallbackInfo&);
        void Rect(const Napi::CallbackInfo&);
        void StrokeRect(const Napi::CallbackInfo&);
        void Stroke(const Napi::CallbackInfo&);
        void MoveTo(const Napi::CallbackInfo&);
        void LineTo(const Napi::CallbackInfo&);
        void QuadraticCurveTo(const Napi::CallbackInfo&);
        Napi::Value GetFillStyle(const Napi::CallbackInfo&);
        void SetFillStyle(const Napi::CallbackInfo&, const Napi::Value& value);
        Napi::Value GetStrokeStyle(const Napi::CallbackInfo&);
        void SetStrokeStyle(const Napi::CallbackInfo&, const Napi::Value& value);
        Napi::Value GetLineWidth(const Napi::CallbackInfo&);
        void SetLineWidth(const Napi::CallbackInfo&, const Napi::Value& value);

        void BeginFrame();
        void EndFrame();

        static inline bgfx::FrameBufferHandle frameBufferHandle{ bgfx::kInvalidHandle };
        static void UpdateRenderTarget(uint32_t width, uint32_t height);

        Canvas* m_canvas;
        bgfx::ViewId m_viewId;
        NVGcontext* m_nvg;

        std::map<std::string, int> m_fonts;

        static inline std::vector<Context*> registeredContexts{};

        friend class Canvas;
    };

}