#pragma once

#include <Babylon/Polyfills/Canvas.h>
#include <Babylon/JsRuntimeScheduler.h>
#include <Babylon/Graphics/DeviceContext.h>
#include "Image.h"
#include "Path2D.h"
#include "Font.h"
#include "nanovg/nanovg_filterstack.h"
#include <variant>
#include <vector>
#include <cstdint>
#include <memory>

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

        // Copies a region of the CPU-side pixel mirror (populated by DrawImage) into dst (w*h*4 RGBA8 bytes).
        // Out-of-range pixels are written as zero. Used to implement getImageData without a GPU readback.
        void ReadPixels(int32_t sx, int32_t sy, uint32_t w, uint32_t h, uint8_t* dst);

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
        Napi::Value CreateImageData(const Napi::CallbackInfo&);
        void SetLineDash(const Napi::CallbackInfo&);
        Napi::Value GetLineDash(const Napi::CallbackInfo&);
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
        void WarnShadowUnsupported();
        void Dispose(const Napi::CallbackInfo&);
        void Dispose();
        bool SetFontFaceId();
        void EnsureFontsLoaded();
        void Flush(const Napi::CallbackInfo&);

        NativeCanvas* m_canvas;
        std::shared_ptr<NVGcontext*> m_nvg;

        Font m_font;
        // A gradient style holds the assigned JavaScript object, not a bare CanvasGradient*.
        // CanvasGradient is an ObjectWrap, so its native instance is deleted by the wrapper's
        // finalizer; `ctx.fillStyle = ctx.createLinearGradient(...)` leaves no other reference,
        // and the pointer dangles as soon as the collector runs. shared_ptr rather than a bare
        // Napi::ObjectReference because SavedStyle copies these on save()/restore() and a
        // reference is move-only -- the copies share one strong reference to the same object.
        using GradientStyle = std::shared_ptr<Napi::ObjectReference>;
        std::variant<std::string, GradientStyle> m_fillStyle{};
        std::variant<std::string, GradientStyle> m_strokeStyle{};
        std::string m_lineCap{};  // 'butt', 'round', 'square'
        std::string m_lineJoin{}; // 'round', 'bevel', 'miter'

        // Dash pattern from setLineDash. Retained only so getLineDash() round-trips;
        // strokes are always drawn solid (nanovg has no dashed stroke).
        std::vector<double> m_lineDash{};

        // Shadow attributes from shadowColor/shadowBlur/shadowOffsetX/shadowOffsetY.
        // Retained only so the getters round-trip; nanovg has no shadow primitive,
        // so nothing is ever drawn from them. Defaults are the spec's.
        std::string m_shadowColor{"rgba(0, 0, 0, 0)"};
        double m_shadowBlur{0.0};
        double m_shadowOffsetX{0.0};
        double m_shadowOffsetY{0.0};
        std::string m_filter{};
        std::string m_direction{"ltr"}; // 'ltr', 'rtl'
        float m_miterLimit{0.f};
        float m_lineWidth{0.f};
        float m_globalAlpha{1.f};
        float m_letterSpacing{0.f};

        std::map<std::string, int> m_fonts;
        int m_currentFontId{-1};

        struct SavedStyle
        {
            std::variant<std::string, GradientStyle> fillStyle;
            std::variant<std::string, GradientStyle> strokeStyle;

            // The rest of the wrapper-side drawing state. nvgSave/nvgRestore rewinds
            // nanovg's own copy of the attributes it knows about, but never these C++
            // mirrors, so without them a getter keeps reporting the post-save() value
            // after restore(). The shadow/dash/filter/direction fields have no nanovg
            // counterpart at all, so they would otherwise never be rewound.
            std::string lineCap;
            std::string lineJoin;
            std::vector<double> lineDash;
            std::string shadowColor;
            double shadowBlur;
            double shadowOffsetX;
            double shadowOffsetY;
            std::string filter;
            std::string direction;
            float miterLimit;
            float lineWidth;
            float globalAlpha;
            float letterSpacing;

            // font is worse than the getter-only cases above: m_currentFontId is what the
            // text draw path binds, so without it the wrong face actually renders after a
            // restore(). nvgRestore rewinds the size it set, but not either of these.
            Font font;
            int currentFontId;
        };
        std::vector<SavedStyle> m_savedStyles;

        Graphics::DeviceContext& m_graphicsContext;

        struct RectangleClipping
        {
            float left, top, width, height;
        } m_rectangleClipping{};

        // Set once the current path contains anything nvgScissor cannot express.
        bool m_pathHasNonRect{false};
        // Set when clip() had such a path and had to fall back to path emulation.
        bool m_isClipped{false};

        std::shared_ptr<arcana::cancellation_source> m_cancellationSource{};
        JsRuntimeScheduler m_runtimeScheduler;

        std::unordered_map<const NativeCanvasImage*, int> m_nvgImageIndices;
        void BindFillStyle(const Napi::CallbackInfo& info);
        void BindStrokeStyle(const Napi::CallbackInfo& info);
        void FlushGraphicResources() override;
        void PlayPath2D(const NativeCanvasPath2D* path);
        void SetFilterStack();

        // Start a fresh nanovg path and drop the clip state that described the old one.
        // clip() emulates a non-rectangular path by leaving it current and letting the next
        // fill draw it, so any operation that resets the path invalidates that emulation:
        // leaving m_isClipped set makes FillRect skip its own nvgBeginPath and append to a
        // path that no longer exists, and leaving m_pathHasNonRect set makes a later clip()
        // take the emulated branch on what is now a plain rect.
        void ResetPathState();

        // CPU-side RGBA8 mirror of the canvas, sized to the canvas, populated by DrawImage so that
        // getImageData can return the exact decoded pixels (the GPU nanovg framebuffer is not read back).
        std::vector<uint8_t> m_cpuPixels;
        uint32_t m_cpuWidth{0};
        uint32_t m_cpuHeight{0};
        void EnsureCpuBuffer();
        // Core RGBA8 blit used by both the NativeCanvasImage and (plain) ImageBitmap drawImage paths.
        void BlitPixelsToCpu(const uint8_t* src, uint32_t srcWidth, uint32_t srcHeight, int32_t sx, int32_t sy, uint32_t sw, uint32_t sh, int32_t dx, int32_t dy, uint32_t dw, uint32_t dh);
        // Shared drawImage body: draws the nanovg image (arity 3/5/9) and mirrors it to the CPU buffer.
        void DrawImageCommon(const Napi::CallbackInfo& info, int imageIndex, const uint8_t* srcPixels, uint32_t srcWidth, uint32_t srcHeight);

        friend class Canvas;
    };
}
