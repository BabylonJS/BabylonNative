#pragma once

#include <queue>
#include <Babylon/Polyfills/Canvas.h>
#include <Babylon/JsRuntimeScheduler.h>

enum Path2DCommandTypes
{
    P2D_CLOSE = 0,
    P2D_MOVETO = 1,
    P2D_LINETO = 2,
    P2D_BEZIERTO = 3,
    P2D_QUADTO = 4,
    P2D_ARC = 5,
    P2D_ARCTO = 6,
    P2D_ELLIPSE = 7,
    P2D_RECT = 8,
    P2D_ROUNDRECT = 9,
    P2D_ROUNDRECTVARYING = 10,
    P2D_ROUNDRECTELLIPTIC = 11,
    P2D_TRANSFORM = 12,
};

struct Path2DClose {}; // TODO: don't bother if no args?
struct Path2DMoveTo { float x; float y; };
struct Path2DLineTo { float x; float y; }; 
struct Path2DBezierTo { float cp1x; float cp1y; float cp2x; float cp2y; float x; float y; };
struct Path2DQuadTo { float cpx; float cpy; float x; float y; };
struct Path2DArc { float x; float y; float radius; float startAngle; float endAngle; bool counterclockwise; };
struct Path2DArcTo { float x1; float y1; float x2; float y2; float radius; };
struct Path2DEllipse { float x; float y; float radiusX; float radiusY; float rotation; float startAngle; float endAngle; bool counterclockwise; };
struct Path2DRect { float x; float y; float width; float height; };
struct Path2DRoundRect { float x; float y; float width; float height; float radii; };
struct Path2DRoundRectVarying { float x; float y; float width; float height; float topLeft; float topRight; float bottomRight; float bottomLeft; };
struct Path2DRoundRectElliptic { float x; float y; float width; float height; float topLeftX; float topLeftY; float topRightX; float topRightY; float bottomRightX; float bottomRightY; float bottomLeftX; float bottomLeftY; };
struct Path2DTransform { float a; float b; float c; float d; float e; float f; };

union Path2DCommandArgs
{
    Path2DClose close;
    Path2DMoveTo moveTo;
    Path2DLineTo lineTo;
    Path2DBezierTo bezierTo;
    Path2DQuadTo quadTo;
    Path2DArc arc;
    Path2DArcTo arcTo;
    Path2DEllipse ellipse;
    Path2DRect rect;
    Path2DRoundRect roundRect;
    Path2DRoundRectVarying roundRectVarying;
    Path2DRoundRectElliptic roundRectElliptic;
    Path2DTransform transform;
};

struct Path2DCommand
{
    Path2DCommandTypes type;
    Path2DCommandArgs args;
};

namespace Babylon::Polyfills::Internal
{
    class NativeCanvasPath2D final : public Napi::ObjectWrap<NativeCanvasPath2D>
    {
    public:
        static void Initialize(Napi::Env env);

        explicit NativeCanvasPath2D(const Napi::CallbackInfo& info);
        // virtual ~NativeCanvasPath2D(); // TODO: destructor? empty queue?

        typename std::deque<Path2DCommand>::iterator begin();
        typename std::deque<Path2DCommand>::iterator end();
        typename std::deque<Path2DCommand>::const_iterator begin() const;
        typename std::deque<Path2DCommand>::const_iterator end() const;

    private:
        void AddPath(const Napi::CallbackInfo&);
        void ClosePath(const Napi::CallbackInfo&);
        void MoveTo(const Napi::CallbackInfo&);
        void LineTo(const Napi::CallbackInfo&);
        void BezierCurveTo(const Napi::CallbackInfo&);
        void QuadraticCurveTo(const Napi::CallbackInfo&);
        void Arc(const Napi::CallbackInfo&);
        void ArcTo(const Napi::CallbackInfo&);
        void Ellipse(const Napi::CallbackInfo&);
        void Rect(const Napi::CallbackInfo&);
        void RoundRect(const Napi::CallbackInfo&);
        void RoundRectVarying(const Napi::CallbackInfo&);

        void AppendCommand(Path2DCommandTypes type, Path2DCommandArgs args);

        std::deque<Path2DCommand> m_commands; // use deque because iterable
    };
}
