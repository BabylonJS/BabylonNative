#pragma once

#include <Babylon/Polyfills/Canvas.h>
#include <Babylon/JsRuntimeScheduler.h>
#include <Babylon/Graphics/DeviceContext.h>
#include "Path2D.h"
#include "Font.h"
#include "nanovg/nanovg_filterstack.h"
#include <variant>
#include <vector>
#include <cstdint>
#include <memory>
#include <map>
#include <unordered_map>
#include <optional>
#include <span>

struct NVGcontext;

namespace Babylon::Polyfills::Internal
{
    class CanvasGradient;
    class NativeCanvasImage;

    class Context final : public Napi::ObjectWrap<Context>, Polyfills::Canvas::Impl::MonitoredResource
    {
    public:
        static void Initialize(Napi::Env);
        static Napi::Value CreateInstance(Napi::Env env, Napi::Value canvas);

        explicit Context(const Napi::CallbackInfo& info);
        virtual ~Context();

        NVGcontext* GetNVGContext() const { return *m_nvg.get(); }

        // Reads straight-alpha RGBA8; pixels outside the canvas are transparent black.
        void ReadPixels(int64_t sx, int64_t sy, uint32_t w, uint32_t h, uint8_t* dst);

        // Flushes pending draws and reads the full framebuffer as RGBA8.
        std::vector<uint8_t> CaptureRGBA();

        // Breaks the reverse pointer when Canvas is finalized first.
        void DetachCanvas();

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
        Napi::Value GetGlobalAlpha(const Napi::CallbackInfo&);
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
        void FlushCore();
        void RetainImageUntilFlush(int imageIndex);
        void ReleaseImagesAfterFlush();
        void DrawText(const Napi::CallbackInfo&, bool stroke);

        NativeCanvas* m_canvas;
        std::shared_ptr<NVGcontext*> m_nvg;

        // Keep the JavaScript gradient alive; State copies share this move-only reference.
        using GradientStyle = std::shared_ptr<Napi::ObjectReference>;
        using Style = std::variant<std::string, GradientStyle>;

        // NanoVG does not rewind these C++ mirrors. Keep them together for save/restore;
        // the current path remains separate because Canvas state does not include it.
        struct ClipMask;
        struct State
        {
            std::shared_ptr<ClipMask> clipMask;
            Style fillStyle{};
            Style strokeStyle{};
            // Defaults must match both NanoVG's reset state and Canvas2D.
            std::string lineCap{"butt"};   // 'butt', 'round', 'square'
            std::string lineJoin{"miter"}; // 'round', 'bevel', 'miter'

            // Dash pattern from setLineDash. Retained only so getLineDash() round-trips;
            // strokes are always drawn solid (nanovg has no dashed stroke).
            std::vector<double> lineDash{};

            // Shadow attributes from shadowColor/shadowBlur/shadowOffsetX/shadowOffsetY.
            // Retained only so the getters round-trip; nanovg has no shadow primitive,
            // so nothing is ever drawn from them. Defaults are the spec's.
            std::string shadowColor{"rgba(0, 0, 0, 0)"};
            double shadowBlur{0.0};
            double shadowOffsetX{0.0};
            double shadowOffsetY{0.0};
            std::string filter{};
            std::string direction{"ltr"}; // 'ltr', 'rtl'
            float miterLimit{10.f};
            float lineWidth{1.f};
            float globalAlpha{1.f};
            float letterSpacing{0.f};

            // NanoVG restores font size, but not these wrapper-side font values.
            Font font{};
            int currentFontId{-1};
        };
        State m_state{};
        std::vector<State> m_savedStates;

        // Context-wide resource cache; save/restore does not rewind loaded fonts.
        std::map<std::string, int> m_fonts;

        Graphics::DeviceContext& m_graphicsContext;

        struct RectangleClipping
        {
            float left, top, width, height;
        } m_rectangleClipping{};

        // Set once the current path contains anything nvgScissor cannot express.
        bool m_pathHasNonRect{false};
        uint32_t m_pathRectangleCount{};
        std::vector<std::shared_ptr<ClipMask>> m_clipMasks;
        void ReleaseClipMasksAfterFlush();

        std::shared_ptr<arcana::cancellation_source> m_cancellationSource{};
        JsRuntimeScheduler m_runtimeScheduler;

        struct CachedNVGImage
        {
            int index{-1};
            uint32_t generation{0};
        };
        std::unordered_map<const NativeCanvasImage*, CachedNVGImage> m_nvgImageIndices;
        // Transient ImageData/canvas snapshots stay alive until nvgEndFrame has
        // consumed every queued draw that references them.
        std::vector<int> m_imagesPendingFlush;
        Napi::Value GetStyle(const Style& style);
        void SetStyle(const Napi::CallbackInfo& info, const Napi::Value& value, Style& style, bool stroke);
        void BindStyle(const Napi::CallbackInfo& info, const Style& style, bool stroke);
        void BindFillStyle(const Napi::CallbackInfo& info);
        void BindStrokeStyle(const Napi::CallbackInfo& info);
        void FlushGraphicResources() override;
        void PlayPath2D(const NativeCanvasPath2D* path);
        void SetFilterStack();

        // The current path is independent of the saved clipping region.
        void ResetPathState();

        struct DrawImageRectangles
        {
            float X, Y, Width, Height;
            float PatternX, PatternY, PatternWidth, PatternHeight;
        };
        static std::optional<DrawImageRectangles> ParseDrawImageRectangles(std::span<const double> arguments, uint32_t srcWidth, uint32_t srcHeight);
        void DrawImageCommon(int imageIndex, const DrawImageRectangles& rectangles);

        friend class Canvas;
    };
}
