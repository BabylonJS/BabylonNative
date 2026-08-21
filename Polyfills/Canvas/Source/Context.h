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

        NativeCanvas* m_canvas;
        std::shared_ptr<NVGcontext*> m_nvg;

        // A gradient style holds the assigned JavaScript object, not a bare CanvasGradient*.
        // CanvasGradient is an ObjectWrap, so its native instance is deleted by the wrapper's
        // finalizer; `ctx.fillStyle = ctx.createLinearGradient(...)` leaves no other reference,
        // and the pointer dangles as soon as the collector runs. shared_ptr rather than a bare
        // Napi::ObjectReference because State is copied on save()/restore() and a reference is
        // move-only -- the copies share one strong reference to the same object.
        using GradientStyle = std::shared_ptr<Napi::ObjectReference>;

        // The wrapper-side drawing state: every attribute save()/restore() has to rewind.
        // These are kept together, and pushed/popped as a whole, so that adding an attribute
        // cannot silently miss save()/restore() the way a hand-written field list allows.
        //
        // They exist because nvgSave/nvgRestore rewinds nanovg's own copy of the attributes it
        // knows about but never these C++ mirrors, so without rewinding them a getter keeps
        // reporting its post-save() value after restore(). The shadow/dash/filter/direction
        // fields have no nanovg counterpart at all, so nothing else would ever rewind them.
        //
        // The current path is deliberately not here: per the spec save() does not save it, so
        // the path-tracking members below stay outside the stack.
        struct State
        {
            std::variant<std::string, GradientStyle> fillStyle{};
            std::variant<std::string, GradientStyle> strokeStyle{};
            // These four mirror state nanovg also holds. Their initial values must match what
            // nvgReset actually installs (NVG_BUTT, NVG_MITER, strokeWidth 1, miterLimit 10),
            // which is also what the spec requires, or a fresh context reports a default it is
            // not drawing with.
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
            double globalAlpha{1.0};
            float letterSpacing{0.f};

            // font is worse than the getter-only cases above: currentFontId is what the text
            // draw path binds, so without rewinding it the wrong face actually renders after a
            // restore(). nvgRestore rewinds the size it set, but not either of these.
            Font font{};
            int currentFontId{-1};
        };
        State m_state{};
        std::vector<State> m_savedStates;

        // Registry of font faces loaded into nanovg, keyed by family. Not part of State:
        // it is a resource cache for the whole context, not an attribute save() rewinds.
        std::map<std::string, int> m_fonts;

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
