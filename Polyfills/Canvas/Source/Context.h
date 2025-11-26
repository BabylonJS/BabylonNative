#pragma once

#include <Babylon/Polyfills/Canvas.h>
#include <Babylon/JsRuntimeScheduler.h>
#include <Babylon/Graphics/DeviceContext.h>
#include "Image.h"
#include "Path2D.h"
#include "Font.h"
#include "nanovg/nanovg_filterstack.h"

struct NVGcontext;

namespace Babylon::Polyfills::Internal
{
    class CanvasGradient;

    class Context final : public Napi::ObjectWrap<Context>, Polyfills::Canvas::Impl::MonitoredResource
    {
    public:
        static void Initialize(Napi::Env);
        static Napi::Value CreateInstance(Napi::Env env, Napi::Value canvas);

        explicit Context(const Napi::CallbackInfo& info);
        virtual ~Context();

        NVGcontext* GetNVGContext() const { return *m_nvg.get(); }

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
        void RoundRect(const Napi::CallbackInfo&);
        void StrokeRect(const Napi::CallbackInfo&);
        void Stroke(const Napi::CallbackInfo&);
        void MoveTo(const Napi::CallbackInfo&);
        void LineTo(const Napi::CallbackInfo&);
        void PutImageData(const Napi::CallbackInfo&);
        void Arc(const Napi::CallbackInfo&);
        void DrawImage(const Napi::CallbackInfo&);
        Napi::Value GetImageData(const Napi::CallbackInfo&);
        void SetLineDash(const Napi::CallbackInfo&);
        void StrokeText(const Napi::CallbackInfo&);
        Napi::Value CreateLinearGradient(const Napi::CallbackInfo&);
        Napi::Value CreateRadialGradient(const Napi::CallbackInfo&);
        Napi::Value GetTransform(const Napi::CallbackInfo&);
        void SetTransform(const Napi::CallbackInfo&);
        void Transform(const Napi::CallbackInfo&);
        void QuadraticCurveTo(const Napi::CallbackInfo&);
        Napi::Value GetFillStyle(const Napi::CallbackInfo&);
        void SetFillStyle(const Napi::CallbackInfo&, const Napi::Value& value);
        Napi::Value GetStrokeStyle(const Napi::CallbackInfo&);
        void SetStrokeStyle(const Napi::CallbackInfo&, const Napi::Value& value);
        Napi::Value GetLineWidth(const Napi::CallbackInfo&);
        void SetLineWidth(const Napi::CallbackInfo&, const Napi::Value& value);
        Napi::Value GetLineCap(const Napi::CallbackInfo&);
        void SetLineCap(const Napi::CallbackInfo&, const Napi::Value& value);
        Napi::Value GetLineJoin(const Napi::CallbackInfo&);
        void SetLineJoin(const Napi::CallbackInfo&, const Napi::Value& value);
        Napi::Value GetMiterLimit(const Napi::CallbackInfo&);
        void SetMiterLimit(const Napi::CallbackInfo&, const Napi::Value& value);
        Napi::Value GetFilter(const Napi::CallbackInfo& info);
        void SetFilter(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetDirection(const Napi::CallbackInfo&);
        void SetDirection(const Napi::CallbackInfo&, const Napi::Value& value);
        Napi::Value GetFont(const Napi::CallbackInfo&);
        void SetFont(const Napi::CallbackInfo&, const Napi::Value& value);
        Napi::Value GetLetterSpacing(const Napi::CallbackInfo&);
        void SetLetterSpacing(const Napi::CallbackInfo&, const Napi::Value& value);
        void SetGlobalAlpha(const Napi::CallbackInfo&, const Napi::Value& value);
        Napi::Value GetShadowColor(const Napi::CallbackInfo&);
        void SetShadowColor(const Napi::CallbackInfo&, const Napi::Value& value);
        Napi::Value GetShadowBlur(const Napi::CallbackInfo&);
        void SetShadowBlur(const Napi::CallbackInfo&, const Napi::Value& value);
        Napi::Value GetShadowOffsetX(const Napi::CallbackInfo&);
        void SetShadowOffsetX(const Napi::CallbackInfo&, const Napi::Value& value);
        Napi::Value GetShadowOffsetY(const Napi::CallbackInfo&);
        void SetShadowOffsetY(const Napi::CallbackInfo&, const Napi::Value& value);
        void Dispose(const Napi::CallbackInfo&);
        void Dispose();
        bool SetFontFaceId();
        void Flush(const Napi::CallbackInfo&);

        NativeCanvas* m_canvas;
        std::shared_ptr<NVGcontext*> m_nvg;

        Font m_font;
        std::variant<std::string, CanvasGradient*> m_fillStyle{};
        std::string m_strokeStyle{};
        std::string m_lineCap{};  // 'butt', 'round', 'square'
        std::string m_lineJoin{}; // 'round', 'bevel', 'miter'
        std::string m_filter{};
        std::string m_direction{"ltr"}; // 'ltr', 'rtl'
        float m_miterLimit{0.f};
        float m_lineWidth{0.f};
        float m_globalAlpha{1.f};
        float m_letterSpacing{0.f};

        std::map<std::string, int> m_fonts;
        int m_currentFontId{-1};

        Graphics::DeviceContext& m_graphicsContext;
        Graphics::Update m_update;

        bool m_isClipped{false};

        struct RectangleClipping
        {
            float left, top, width, height;
        } m_rectangleClipping{};

        std::shared_ptr<arcana::cancellation_source> m_cancellationSource{};
        JsRuntimeScheduler m_runtimeScheduler;

        std::unordered_map<const NativeCanvasImage*, int> m_nvgImageIndices;
        void BindFillStyle(const Napi::CallbackInfo& info, float left, float top, float width, float height);
        void FlushGraphicResources() override;
        void PlayPath2D(const NativeCanvasPath2D* path);
        void SetFilterStack();

        friend class Canvas;
    };
}
