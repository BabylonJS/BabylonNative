#pragma once

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct NVGcontext NVGcontext;

typedef struct NVGcolor
{
    float r;
    float g;
    float b;
    float a;
} NVGcolor;

typedef struct NVGpaint
{
    int image;
    float x;
    float y;
    float width;
    float height;
    float angle;
    float alpha;
    int kind;
    NVGcolor innerColor;
    NVGcolor outerColor;
} NVGpaint;

typedef enum NVGwinding
{
    NVG_CCW = 1,
    NVG_CW = 2,
} NVGwinding;

typedef enum NVGlineCap
{
    NVG_BUTT = 0,
    NVG_ROUND = 1,
    NVG_SQUARE = 2,
    NVG_BEVEL = 3,
    NVG_MITER = 4,
} NVGlineCap;

typedef enum NVGcompositeOperation
{
    NVG_SOURCE_OVER = 0,
    NVG_SOURCE_IN = 1,
    NVG_SOURCE_OUT = 2,
    NVG_ATOP = 3,
    NVG_DESTINATION_OVER = 4,
    NVG_DESTINATION_IN = 5,
    NVG_DESTINATION_OUT = 6,
    NVG_DESTINATION_ATOP = 7,
    NVG_LIGHTER = 8,
    NVG_COPY = 9,
    NVG_XOR = 10,
} NVGcompositeOperation;

static inline NVGcolor nvgRGBA(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
    NVGcolor c{};
    c.r = static_cast<float>(r) / 255.0f;
    c.g = static_cast<float>(g) / 255.0f;
    c.b = static_cast<float>(b) / 255.0f;
    c.a = static_cast<float>(a) / 255.0f;
    return c;
}

static inline NVGcolor nvgRGB(unsigned char r, unsigned char g, unsigned char b)
{
    return nvgRGBA(r, g, b, 255);
}

static inline NVGcolor nvgRGBAf(float r, float g, float b, float a)
{
    NVGcolor c{};
    c.r = r;
    c.g = g;
    c.b = b;
    c.a = a;
    return c;
}

NVGcontext* nvgCreate(int flags);
void nvgDelete(NVGcontext* ctx);

int nvgCreateFontMem(NVGcontext* ctx, const char* name, unsigned char* data, int size, int freeData);
void nvgFontFaceId(NVGcontext* ctx, int font);
void nvgFontSize(NVGcontext* ctx, float size);
void nvgTextLetterSpacing(NVGcontext* ctx, float spacing);
float nvgText(NVGcontext* ctx, float x, float y, const char* string, const char* end);
float nvgStrokeText(NVGcontext* ctx, float x, float y, const char* string, const char* end);
float nvgTextBounds(NVGcontext* ctx, float x, float y, const char* string, const char* end, float* bounds);
void nvgTextMetrics(NVGcontext* ctx, float* ascender, float* descender, float* lineh);

void nvgBeginFrame(NVGcontext* ctx, float windowWidth, float windowHeight, float devicePixelRatio);
void nvgEndFrame(NVGcontext* ctx);

void nvgSave(NVGcontext* ctx);
void nvgRestore(NVGcontext* ctx);
void nvgResetTransform(NVGcontext* ctx);
void nvgTransform(NVGcontext* ctx, float a, float b, float c, float d, float e, float f);
void nvgCurrentTransform(NVGcontext* ctx, float* xform);
void nvgTranslate(NVGcontext* ctx, float x, float y);
void nvgRotate(NVGcontext* ctx, float angle);
void nvgScale(NVGcontext* ctx, float x, float y);

void nvgScissor(NVGcontext* ctx, float x, float y, float w, float h);

void nvgBeginPath(NVGcontext* ctx);
void nvgClosePath(NVGcontext* ctx);
void nvgMoveTo(NVGcontext* ctx, float x, float y);
void nvgLineTo(NVGcontext* ctx, float x, float y);
void nvgBezierTo(NVGcontext* ctx, float c1x, float c1y, float c2x, float c2y, float x, float y);
void nvgQuadTo(NVGcontext* ctx, float cx, float cy, float x, float y);
void nvgArc(NVGcontext* ctx, float cx, float cy, float r, float a0, float a1, NVGwinding dir);
void nvgArcTo(NVGcontext* ctx, float x1, float y1, float x2, float y2, float radius);
void nvgRect(NVGcontext* ctx, float x, float y, float w, float h);
void nvgRoundedRect(NVGcontext* ctx, float x, float y, float w, float h, float r);
void nvgRoundedRectVarying(NVGcontext* ctx, float x, float y, float w, float h, float radTopLeft, float radTopRight, float radBottomRight, float radBottomLeft);
void nvgRoundedRectElliptic(NVGcontext* ctx, float x, float y, float w, float h, float rtlx, float rtly, float rtrx, float rtry, float rbrx, float rbry, float rblx, float rbly);
void nvgEllipse(NVGcontext* ctx, float cx, float cy, float rx, float ry);

void nvgFillColor(NVGcontext* ctx, NVGcolor color);
void nvgStrokeColor(NVGcontext* ctx, NVGcolor color);
void nvgFillPaint(NVGcontext* ctx, NVGpaint paint);
void nvgStrokeWidth(NVGcontext* ctx, float width);
void nvgLineCap(NVGcontext* ctx, NVGlineCap cap);
void nvgLineJoin(NVGcontext* ctx, NVGlineCap join);
void nvgMiterLimit(NVGcontext* ctx, float limit);
void nvgGlobalAlpha(NVGcontext* ctx, float alpha);
void nvgGlobalCompositeOperation(NVGcontext* ctx, int op);
void nvgSetFilterBlur(NVGcontext* ctx, float sigma);

void nvgFill(NVGcontext* ctx);
void nvgStroke(NVGcontext* ctx);

int nvgCreateImageRGBA(NVGcontext* ctx, int w, int h, int imageFlags, const unsigned char* data);
void nvgDeleteImage(NVGcontext* ctx, int image);
NVGpaint nvgImagePattern(NVGcontext* ctx, float ox, float oy, float ex, float ey, float angle, int image, float alpha);
const void* nvgGetRenderTexture(NVGcontext* ctx);

#ifdef __cplusplus
}
#endif
