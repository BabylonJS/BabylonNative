// clang-format off
/*
 * Copyright 2011-2020 Branimir Karadzic. All rights reserved.
 * License: https://github.com/bkaradzic/bgfx#license-bsd-2-clause
 */

//
// Copyright (c) 2009-2013 Mikko Mononen memon@inside.org
//
// This software is provided 'as-is', without any express or implied
// warranty.  In no event will the authors be held liable for any damages
// arising from the use of this software.
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would be
//    appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
//    misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.
//
#define NVG_ANTIALIAS 1

#include "nanovg/nanovg_babylon.h"

#include <stdlib.h>
#include <math.h>
#include "nanovg.h"

#include <bgfx/bgfx.h>
#include <bgfx/embedded_shader.h>

#include <bx/bx.h>
#include <bx/allocator.h>

#include <Babylon/Graphics/DeviceContext.h>
#include <Babylon/Graphics/FrameBuffer.h>
BX_PRAGMA_DIAGNOSTIC_IGNORED_MSVC(4244) // warning C4244: '=' : conversion from '' to '', possible loss of data

#include "Shaders/dx11/vs_nanovg_fill.h"
#include "Shaders/dx11/fs_nanovg_fill.h"
#include "Shaders/metal/vs_nanovg_fill.h"
#include "Shaders/metal/fs_nanovg_fill.h"
#include "Shaders/glsl/vs_nanovg_fill.h"
#include "Shaders/glsl/fs_nanovg_fill.h"
#include "Shaders/essl/vs_nanovg_fill.h"
#include "Shaders/essl/fs_nanovg_fill.h"
#include "Shaders/spirv/vs_nanovg_fill.h"
#include "Shaders/spirv/fs_nanovg_fill.h"

#include "nanovg_filterstack.h"

struct PosTexCoord0Vertex
{
    float m_x;
    float m_y;
    float m_z;
    float m_u;
    float m_v;

    static void init()
    {
        s_layout
            .begin()
            .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
            .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
            .end();
    }

    static inline bgfx::VertexLayout s_layout;
};

void screenSpaceQuad(bgfx::Encoder* encoder, bool _originBottomLeft, float _width = 1.0f, float _height = 1.0f)
{
    if (3 == bgfx::getAvailTransientVertexBuffer(3, PosTexCoord0Vertex::s_layout))
    {
        bgfx::TransientVertexBuffer vb;
        bgfx::allocTransientVertexBuffer(&vb, 3, PosTexCoord0Vertex::s_layout);
        PosTexCoord0Vertex* vertex = (PosTexCoord0Vertex*)vb.data;

        const float minx = -_width;
        const float maxx = _width;
        const float miny = 0.0f;
        const float maxy = _height * 2.0f;

        const float minu = -1.0f;
        const float maxu = 1.0f;

        const float zz = 0.0f;

        float minv = 0.0f;
        float maxv = 2.0f;

        if (_originBottomLeft)
        {
            float temp = minv;
            minv = maxv;
            maxv = temp;

            minv -= 1.0f;
            maxv -= 1.0f;
        }

        vertex[0].m_x = minx;
        vertex[0].m_y = miny;
        vertex[0].m_z = zz;
        vertex[0].m_u = minu;
        vertex[0].m_v = minv;

        vertex[1].m_x = maxx;
        vertex[1].m_y = miny;
        vertex[1].m_z = zz;
        vertex[1].m_u = maxu;
        vertex[1].m_v = minv;

        vertex[2].m_x = maxx;
        vertex[2].m_y = maxy;
        vertex[2].m_z = zz;
        vertex[2].m_u = maxu;
        vertex[2].m_v = maxv;

        //bgfx::setVertexBuffer(0, &vb);
        encoder->setVertexBuffer(0, &vb);
    }
}


static const bgfx::EmbeddedShader s_embeddedShadersBabylon[] =
{
    BGFX_EMBEDDED_SHADER(vs_nanovg_fill),
    BGFX_EMBEDDED_SHADER(fs_nanovg_fill),

    BGFX_EMBEDDED_SHADER_END()
};

namespace
{
    static bgfx::VertexLayout s_nvgLayout;

    enum GLNVGshaderType
    {
        NSVG_SHADER_FILLGRAD,
        NSVG_SHADER_FILLIMG,
        NSVG_SHADER_SIMPLE,
        NSVG_SHADER_IMG,
        NSVG_SHADER_IMG_MODULATEGRAD,
    };

    // These are additional flags on top of NVGimageFlags.
    enum NVGimageFlagsGL {
        NVG_IMAGE_NODELETE = 1<<16, // Do not delete GL texture handle.
    };

    struct GLNVGtexture
    {
        bgfx::TextureHandle id;
        int width, height;
        int type;
        int flags;
    };

    struct GLNVGblend
    {
        uint64_t srcRGB;
        uint64_t dstRGB;
        uint64_t srcAlpha;
        uint64_t dstAlpha;
    };

    enum GLNVGcallType
    {
        GLNVG_FILL,
        GLNVG_CONVEXFILL,
        GLNVG_STROKE,
        GLNVG_TRIANGLES,
    };

    struct GLNVGcall
    {
        int type;
        int image;
        int image2;
        int pathOffset;
        int pathCount;
        int vertexOffset;
        int vertexCount;
        int uniformOffset;
        GLNVGblend blendFunc;
        nanovg_filterstack filterStack;
    };

    struct GLNVGpath
    {
        int fillOffset;
        int fillCount;
        int strokeOffset;
        int strokeCount;
    };

    struct GLNVGfragUniforms
    {
        float scissorMat[12]; // matrices are actually 3 vec4s
        float paintMat[12];
        NVGcolor innerCol;
        NVGcolor outerCol;

        // u_scissorExtScale
        float scissorExt[2];
        float scissorScale[2];

        // u_extentRadius
        float extent[2];
        float radius;

        // u_params
        float feather;
        float strokeMult;
        float texType;
        float type;

        // u_sdf
        float sdfMin;
        float sdfMax;
        float sdfBlur;
        float unused;
    };

    struct GLNVGcontext
    {
        bx::AllocatorI* allocator;

        bgfx::ProgramHandle prog;
        bgfx::UniformHandle u_scissorMat;
        bgfx::UniformHandle u_paintMat;
        bgfx::UniformHandle u_innerCol;
        bgfx::UniformHandle u_outerCol;
        bgfx::UniformHandle u_viewSize;
        bgfx::UniformHandle u_scissorExtScale;
        bgfx::UniformHandle u_extentRadius;
        bgfx::UniformHandle u_params;
        bgfx::UniformHandle u_halfTexel;
        bgfx::UniformHandle u_sdf;

        bgfx::UniformHandle s_tex;
        bgfx::UniformHandle s_tex2;
        

        uint64_t state;
        bgfx::TextureHandle th;
        bgfx::TextureHandle th2;
        bgfx::TextureHandle texMissing;

        bgfx::TransientVertexBuffer tvb;
        Babylon::Graphics::FrameBuffer* frameBuffer;
        PoolInterface frameBufferPool;
        bgfx::Encoder* encoder;

        struct GLNVGtexture* textures;
        float view[2];
        int ntextures;
        int ctextures;
        int textureId;
        int vertBuf;
        int fragSize;
        int edgeAntiAlias;

        // Per frame buffers
        struct GLNVGcall* calls;
        int ccalls;
        int ncalls;
        struct GLNVGpath* paths;
        int cpaths;
        int npaths;
        struct NVGvertex* verts;
        int cverts;
        int nverts;
        unsigned char* uniforms;
        int cuniforms;
        int nuniforms;
    };

    static struct GLNVGtexture* glnvg__allocTexture(struct GLNVGcontext* gl)
    {
        struct GLNVGtexture* tex = NULL;
        int i;

        for (i = 0; i < gl->ntextures; i++)
        {
            if (gl->textures[i].id.idx == bgfx::kInvalidHandle)
            {
                tex = &gl->textures[i];
                break;
            }
        }

        if (tex == NULL)
        {
            if (gl->ntextures+1 > gl->ctextures)
            {
                int old = gl->ctextures;
                gl->ctextures = (gl->ctextures == 0) ? 2 : gl->ctextures*2;
                gl->textures = (struct GLNVGtexture*)bx::realloc(gl->allocator, gl->textures, sizeof(struct GLNVGtexture)*gl->ctextures);
                bx::memSet(&gl->textures[old], 0xff, (gl->ctextures-old)*sizeof(struct GLNVGtexture) );

                if (gl->textures == NULL)
                {
                    return NULL;
                }
            }
            tex = &gl->textures[gl->ntextures++];
        }

        bx::memSet(tex, 0, sizeof(*tex) );

        return tex;
    }

    static struct GLNVGtexture* glnvg__findTexture(struct GLNVGcontext* gl, int id)
    {
        int i;
        for (i = 0; i < gl->ntextures; i++)
        {
            if (gl->textures[i].id.idx == id)
            {
                return &gl->textures[i];
            }
        }

        return NULL;
    }

    static int glnvg__deleteTexture(struct GLNVGcontext* gl, int id)
    {
        for (int ii = 0; ii < gl->ntextures; ii++)
        {
            if (gl->textures[ii].id.idx == id)
            {
                if (bgfx::isValid(gl->textures[ii].id)
                && (gl->textures[ii].flags & NVG_IMAGE_NODELETE) == 0)
                {
                    bgfx::destroy(gl->textures[ii].id);
                }
                bx::memSet(&gl->textures[ii], 0, sizeof(gl->textures[ii]) );
                gl->textures[ii].id.idx = bgfx::kInvalidHandle;
                return 1;
            }
        }

        return 0;
    }

    static int nvgRenderCreate(void* _userPtr)
    {
        struct GLNVGcontext* gl = (struct GLNVGcontext*)_userPtr;
        const bgfx::Memory* mem = bgfx::alloc(4*4*4);
        uint32_t* bgra8 = (uint32_t*)mem->data;
        bx::memSet(bgra8, 0, 4*4*4);
        gl->texMissing = bgfx::createTexture2D(4, 4, false, 1, bgfx::TextureFormat::BGRA8, 0, mem);

        gl->u_scissorMat      = bgfx::createUniform("u_scissorMat",      bgfx::UniformType::Mat3);
        gl->u_paintMat        = bgfx::createUniform("u_paintMat",        bgfx::UniformType::Mat3);
        gl->u_innerCol        = bgfx::createUniform("u_innerCol",        bgfx::UniformType::Vec4);
        gl->u_outerCol        = bgfx::createUniform("u_outerCol",        bgfx::UniformType::Vec4);
        gl->u_viewSize        = bgfx::createUniform("u_viewSize",        bgfx::UniformType::Vec4);
        gl->u_scissorExtScale = bgfx::createUniform("u_scissorExtScale", bgfx::UniformType::Vec4);
        gl->u_extentRadius    = bgfx::createUniform("u_extentRadius",    bgfx::UniformType::Vec4);
        gl->u_params          = bgfx::createUniform("u_params",          bgfx::UniformType::Vec4);
        gl->u_sdf             = bgfx::createUniform("u_sdf",             bgfx::UniformType::Vec4);
        gl->s_tex             = bgfx::createUniform("s_tex",             bgfx::UniformType::Sampler);
        gl->s_tex2            = bgfx::createUniform("s_tex2",            bgfx::UniformType::Sampler);
        nanovg_filterstack::InitBgfx(); // initialize filter stack uniforms + programs

        gl->u_halfTexel.idx = bgfx::kInvalidHandle;

        s_nvgLayout
            .begin()
            .add(bgfx::Attrib::Position,  2, bgfx::AttribType::Float)
            .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
            .end();

        int align = 16;
        gl->fragSize = sizeof(struct GLNVGfragUniforms) + align - sizeof(struct GLNVGfragUniforms) % align;

        return 1;
    }

    static int nvgRenderCreateTexture(
          void* _userPtr
        , int _type
        , int _width
        , int _height
        , int _flags
        , const unsigned char* _rgba
        )
    {
        struct GLNVGcontext* gl = (struct GLNVGcontext*)_userPtr;
        struct GLNVGtexture* tex = glnvg__allocTexture(gl);

        if (tex == NULL)
        {
            return 0;
        }

        tex->width  = _width;
        tex->height = _height;
        tex->type   = _type;
        tex->flags  = _flags;

        uint32_t bytesPerPixel = NVG_TEXTURE_RGBA == tex->type ? 4 : 1;
        uint32_t pitch = tex->width * bytesPerPixel;

        const bgfx::Memory* mem = NULL;
        if (NULL != _rgba)
        {
            mem = bgfx::copy(_rgba, tex->height * pitch);
        }

        tex->id = bgfx::createTexture2D(
                          tex->width
                        , tex->height
                        , false
                        , 1
                        , NVG_TEXTURE_RGBA == _type ? bgfx::TextureFormat::RGBA8 : bgfx::TextureFormat::R8
                        , BGFX_SAMPLER_NONE
                        );

        if (NULL != mem)
        {
            bgfx::updateTexture2D(
                  tex->id
                , 0
                , 0
                , 0
                , 0
                , tex->width
                , tex->height
                , mem
                );
        }

        return bgfx::isValid(tex->id) ? tex->id.idx : 0;
    }

    static int nvgRenderDeleteTexture(void* _userPtr, int image)
    {
        struct GLNVGcontext* gl = (struct GLNVGcontext*)_userPtr;
        return glnvg__deleteTexture(gl, image);
    }

    static int nvgRenderUpdateTexture(void* _userPtr, int image, int x, int y, int w, int h, const unsigned char* data)
    {
        struct GLNVGcontext* gl = (struct GLNVGcontext*)_userPtr;
        struct GLNVGtexture* tex = glnvg__findTexture(gl, image);
        if (tex == NULL)
        {
            return 0;
        }

        uint32_t bytesPerPixel = NVG_TEXTURE_RGBA == tex->type ? 4 : 1;
        uint32_t pitch = tex->width * bytesPerPixel;

        const bgfx::Memory* mem = bgfx::alloc(w * h * bytesPerPixel);
        bx::gather(mem->data,                            // dst
                   data + y * pitch + x * bytesPerPixel, // src
                   pitch,                                // srcStride
                   w * bytesPerPixel,                    // stride
                   h);                                   // num

        bgfx::updateTexture2D(
              tex->id
            , 0
            , 0
            , x
            , y
            , w
            , h
            , mem
            , UINT16_MAX
            );

        return 1;
    }

    static int nvgRenderGetTextureSize(void* _userPtr, int image, int* w, int* h)
    {
        struct GLNVGcontext* gl = (struct GLNVGcontext*)_userPtr;
        struct GLNVGtexture* tex = glnvg__findTexture(gl, image);

        if (NULL == tex
        || !bgfx::isValid(tex->id) )
        {
            return 0;
        }

        *w = tex->width;
        *h = tex->height;

        return 1;
    }

    static void glnvg__xformToMat3x4(float* m3, float* t)
    {
        m3[ 0] = t[0];
        m3[ 1] = t[1];
        m3[ 2] = 0.0f;
        m3[ 3] = 0.0f;
        m3[ 4] = t[2];
        m3[ 5] = t[3];
        m3[ 6] = 0.0f;
        m3[ 7] = 0.0f;
        m3[ 8] = t[4];
        m3[ 9] = t[5];
        m3[10] = 1.0f;
        m3[11] = 0.0f;
    }

    static NVGcolor glnvg__premulColor(NVGcolor c)
    {
        c.r *= c.a;
        c.g *= c.a;
        c.b *= c.a;
        return c;
    }

    static int glnvg__convertPaint(
          struct GLNVGcontext* gl
        , struct GLNVGfragUniforms* frag
        , struct NVGpaint* paint
        , struct NVGscissor* scissor
        , float width
        , float fringe
        )
    {
        struct GLNVGtexture* tex = NULL;
        struct GLNVGtexture* tex2 = NULL;
        float invxform[6] = {};

        bx::memSet(frag, 0, sizeof(*frag) );

        frag->innerCol = glnvg__premulColor(paint->innerColor);
        frag->outerCol = glnvg__premulColor(paint->outerColor);

        if (scissor->extent[0] < -0.5f || scissor->extent[1] < -0.5f)
        {
            bx::memSet(frag->scissorMat, 0, sizeof(frag->scissorMat) );
            frag->scissorExt[0] = 1.0f;
            frag->scissorExt[1] = 1.0f;
            frag->scissorScale[0] = 1.0f;
            frag->scissorScale[1] = 1.0f;
        }
        else
        {
            nvgTransformInverse(invxform, scissor->xform);
            glnvg__xformToMat3x4(frag->scissorMat, invxform);
            frag->scissorExt[0] = scissor->extent[0];
            frag->scissorExt[1] = scissor->extent[1];
            frag->scissorScale[0] = sqrtf(scissor->xform[0]*scissor->xform[0] + scissor->xform[2]*scissor->xform[2]) / fringe;
            frag->scissorScale[1] = sqrtf(scissor->xform[1]*scissor->xform[1] + scissor->xform[3]*scissor->xform[3]) / fringe;
        }
        bx::memCopy(frag->extent, paint->extent, sizeof(frag->extent) );
        frag->strokeMult = (width*0.5f + fringe*0.5f) / fringe;

        gl->th = gl->texMissing;
        gl->th2 = { bgfx::kInvalidHandle };

        if (paint->image != 0)
        {
            tex = glnvg__findTexture(gl, paint->image);
            if (tex == NULL)
            {
                return 0;
            }
            nvgTransformInverse(invxform, paint->xform);
            frag->type = NSVG_SHADER_FILLIMG;

            if (tex->type == NVG_TEXTURE_RGBA)
            {
                frag->texType = (tex->flags & NVG_IMAGE_PREMULTIPLIED) ? 0.0f : 1.0f;
            }
            else
            {
                frag->texType = 2.0f;
            }
            gl->th = tex->id;

            // tex2 is optional
            if (paint->image2 != 0)
            {
                tex2 = glnvg__findTexture(gl, paint->image2); // TODO get paint image
                if (tex)
                {
                    gl->th2 = tex2->id;
                    frag->type = NSVG_SHADER_IMG_MODULATEGRAD;
                }
            }
        }
        else
        {
            frag->type = NSVG_SHADER_FILLGRAD;
            frag->radius  = paint->radius;
            frag->feather = paint->feather;
            nvgTransformInverse(invxform, paint->xform);
        }

        glnvg__xformToMat3x4(frag->paintMat, invxform);
        frag->sdfMin = paint->sdfMin;
        frag->sdfMax = paint->sdfMax;
        frag->sdfBlur = paint->sdfBlur;

        return 1;
    }

    static void glnvg__mat3(float* dst, float* src)
    {
        dst[0] = src[ 0];
        dst[1] = src[ 1];
        dst[2] = src[ 2];

        dst[3] = src[ 4];
        dst[4] = src[ 5];
        dst[5] = src[ 6];

        dst[6] = src[ 8];
        dst[7] = src[ 9];
        dst[8] = src[10];
    }

    static struct GLNVGfragUniforms* nvg__fragUniformPtr(struct GLNVGcontext* gl, int i)
    {
        return (struct GLNVGfragUniforms*)&gl->uniforms[i];
    }

    static void nvgRenderSetUniforms(struct GLNVGcontext* gl, int uniformOffset, int image, int image2)
    {
        struct GLNVGfragUniforms* frag = nvg__fragUniformPtr(gl, uniformOffset);
        float tmp[9]; // Maybe there's a way to get rid of this...
        glnvg__mat3(tmp, frag->scissorMat);
        gl->encoder->setUniform(gl->u_scissorMat, tmp);
        glnvg__mat3(tmp, frag->paintMat);
        gl->encoder->setUniform(gl->u_paintMat, tmp);

        gl->encoder->setUniform(gl->u_innerCol,        frag->innerCol.rgba);
        gl->encoder->setUniform(gl->u_outerCol,        frag->outerCol.rgba);
        gl->encoder->setUniform(gl->u_scissorExtScale, &frag->scissorExt[0]);
        gl->encoder->setUniform(gl->u_extentRadius,    &frag->extent[0]);
        gl->encoder->setUniform(gl->u_params,          &frag->feather);
        gl->encoder->setUniform(gl->u_sdf,             &frag->sdfMin);

        bgfx::TextureHandle handle = gl->texMissing;

        if (image != 0)
        {
            struct GLNVGtexture* tex = glnvg__findTexture(gl, image);
            if (tex != NULL)
            {
                handle = tex->id;

                if (bgfx::isValid(gl->u_halfTexel) )
                {
                    float halfTexel[4] = { 0.5f / tex->width, 0.5f / tex->height };
                    gl->encoder->setUniform(gl->u_halfTexel, halfTexel);
                }
            }
        }
        gl->th = handle;

        bgfx::TextureHandle handle2 = gl->texMissing;
        if (image2 != 0)
        {
            struct GLNVGtexture* tex = glnvg__findTexture(gl, image2);
            if (tex != NULL)
            {
                handle2 = tex->id;
            }
        }
        gl->th2 = handle2;
    }

    static void nvgRenderViewport(void* _userPtr, float width, float height, float /*devicePixelRatio*/)
    {
        struct GLNVGcontext* gl = (struct GLNVGcontext*)_userPtr;
        gl->view[0] = width;
        gl->view[1] = height;
    }

    static void fan(bgfx::Encoder* encoder, uint32_t _start, uint32_t _count)
    {
        uint32_t numTris = _count-2;
        bgfx::TransientIndexBuffer tib;
        bgfx::allocTransientIndexBuffer(&tib, numTris*3);
        uint16_t* data = (uint16_t*)tib.data;
        for (uint32_t ii = 0; ii < numTris; ++ii)
        {
            data[ii*3+0] = _start;
            data[ii*3+1] = _start + ii + 1;
            data[ii*3+2] = _start + ii + 2;
        }

        encoder->setIndexBuffer(&tib);
    }

    static void glnvg__fill(struct GLNVGcontext* gl, struct GLNVGcall* call)
    {
        bgfx::ProgramHandle firstProg = gl->prog;
        std::function setUniform = [gl](bgfx::UniformHandle u, const void *value, const uint16_t num) {
            gl->encoder->setUniform(u, value, num);
        };
        std::function firstPass = [gl, call](bgfx::ProgramHandle prog, Babylon::Graphics::FrameBuffer *outBuffer) {

            struct GLNVGpath* paths = &gl->paths[call->pathOffset];
            int i, npaths = call->pathCount;

            // set bindpoint for solid loc
            nvgRenderSetUniforms(gl, call->uniformOffset, 0, 0);

            for (i = 0; i < npaths; i++)
            {
                if (2 < paths[i].fillCount)
                {
                    gl->encoder->setState(0);
                    gl->encoder->setStencil(0
                        | BGFX_STENCIL_TEST_ALWAYS
                        | BGFX_STENCIL_FUNC_RMASK(0xff)
                        | BGFX_STENCIL_OP_FAIL_S_KEEP
                        | BGFX_STENCIL_OP_FAIL_Z_KEEP
                        | BGFX_STENCIL_OP_PASS_Z_INCR
                        , 0
                        | BGFX_STENCIL_TEST_ALWAYS
                        | BGFX_STENCIL_FUNC_RMASK(0xff)
                        | BGFX_STENCIL_OP_FAIL_S_KEEP
                        | BGFX_STENCIL_OP_FAIL_Z_KEEP
                        | BGFX_STENCIL_OP_PASS_Z_DECR
                        );
                    gl->encoder->setVertexBuffer(0, &gl->tvb);
                    gl->encoder->setTexture(0, gl->s_tex, gl->th);
                    gl->encoder->setTexture(1, gl->s_tex2, gl->th2);
                    fan(gl->encoder, paths[i].fillOffset, paths[i].fillCount);
                    outBuffer->Submit(*gl->encoder, prog, BGFX_DISCARD_ALL);
                }
            }

            // Draw aliased off-pixels
            nvgRenderSetUniforms(gl, call->uniformOffset + gl->fragSize, call->image, call->image2);

            if (gl->edgeAntiAlias)
            {
                // Draw fringes
                for (i = 0; i < npaths; i++)
                {
                    gl->encoder->setState(gl->state
                        | BGFX_STATE_PT_TRISTRIP
                        );
                    gl->encoder->setStencil(0
                        | BGFX_STENCIL_TEST_EQUAL
                        | BGFX_STENCIL_FUNC_RMASK(0xff)
                        | BGFX_STENCIL_OP_FAIL_S_KEEP
                        | BGFX_STENCIL_OP_FAIL_Z_KEEP
                        | BGFX_STENCIL_OP_PASS_Z_KEEP
                        );
                    gl->encoder->setVertexBuffer(0, &gl->tvb, paths[i].strokeOffset, paths[i].strokeCount);
                    gl->encoder->setTexture(0, gl->s_tex, gl->th);
                    gl->encoder->setTexture(1, gl->s_tex2, gl->th2);
                    outBuffer->Submit(*gl->encoder, prog, BGFX_DISCARD_ALL);
                }
            }

            // Draw fill
            gl->encoder->setState(gl->state);
            gl->encoder->setVertexBuffer(0, &gl->tvb, call->vertexOffset, call->vertexCount);
            gl->encoder->setTexture(0, gl->s_tex, gl->th);
            gl->encoder->setTexture(1, gl->s_tex2, gl->th2);
            gl->encoder->setStencil(0
                    | BGFX_STENCIL_TEST_NOTEQUAL
                    | BGFX_STENCIL_FUNC_RMASK(0xff)
                    | BGFX_STENCIL_OP_FAIL_S_ZERO
                    | BGFX_STENCIL_OP_FAIL_Z_ZERO
                    | BGFX_STENCIL_OP_PASS_Z_ZERO
                    );
            outBuffer->Submit(*gl->encoder, prog, BGFX_DISCARD_ALL);
        };
        std::function filterPass = [gl, call](bgfx::ProgramHandle prog, Babylon::Graphics::FrameBuffer *inBuffer, Babylon::Graphics::FrameBuffer *outBuffer) {
            gl->encoder->setUniform(gl->u_viewSize, gl->view); // TODO: also set other common uniforms
            gl->encoder->setState(BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A);
            gl->encoder->setTexture(0, gl->s_tex, bgfx::getTexture(inBuffer->Handle()));
            bool s_originBottomLeft = bgfx::getCaps()->originBottomLeft;
            screenSpaceQuad(gl->encoder, s_originBottomLeft);
            outBuffer->Submit(*gl->encoder, prog, BGFX_DISCARD_ALL);
        };
        std::function finalPass = [gl, call](bgfx::ProgramHandle prog, Babylon::Graphics::FrameBuffer *inBuffer, Babylon::Graphics::FrameBuffer *outBuffer) {
            gl->encoder->setUniform(gl->u_viewSize, gl->view); // TODO: also set other common uniforms
            gl->encoder->setState(BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A
                | BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_ONE, BGFX_STATE_BLEND_INV_SRC_ALPHA)
                | BGFX_STATE_BLEND_EQUATION(BGFX_STATE_BLEND_EQUATION_ADD));
            gl->encoder->setTexture(0, gl->s_tex, bgfx::getTexture(inBuffer->Handle()));
            bool s_originBottomLeft = bgfx::getCaps()->originBottomLeft;
            screenSpaceQuad(gl->encoder, s_originBottomLeft);
            outBuffer->Submit(*gl->encoder, prog, BGFX_DISCARD_ALL);
        };
        Babylon::Graphics::FrameBuffer *finalFrameBuffer = gl->frameBuffer;
        finalFrameBuffer->Bind(*gl->encoder); // Should this be bound elsewhere?

        call->filterStack.Render(firstProg, setUniform, firstPass, filterPass, finalPass, finalFrameBuffer, gl->frameBufferPool.acquire, gl->frameBufferPool.release);
    }

    static void glnvg__convexFill(struct GLNVGcontext* gl, struct GLNVGcall* call)
    {
        bgfx::ProgramHandle firstProg = gl->prog;
        std::function setUniform = [gl](bgfx::UniformHandle u, const void *value, const uint16_t num) {
            gl->encoder->setUniform(u, value, num);
        };
        std::function firstPass = [gl, call](bgfx::ProgramHandle prog, Babylon::Graphics::FrameBuffer *outBuffer) {
            struct GLNVGpath* paths = &gl->paths[call->pathOffset];
            int i, npaths = call->pathCount;

            nvgRenderSetUniforms(gl, call->uniformOffset, call->image, call->image2);

            for (i = 0; i < npaths; i++)
            {
                if (paths[i].fillCount == 0) continue;
                gl->encoder->setState(gl->state);
                gl->encoder->setVertexBuffer(0, &gl->tvb);
                gl->encoder->setTexture(0, gl->s_tex, gl->th);
                gl->encoder->setTexture(1, gl->s_tex2, gl->th2);
                fan(gl->encoder, paths[i].fillOffset, paths[i].fillCount);
                outBuffer->Submit(*gl->encoder, prog, BGFX_DISCARD_ALL);
            }

            if (gl->edgeAntiAlias)
            {
                // Draw fringes
                for (i = 0; i < npaths; i++)
                {
                    gl->encoder->setState(gl->state
                        | BGFX_STATE_PT_TRISTRIP
                        );
                    gl->encoder->setVertexBuffer(0, &gl->tvb, paths[i].strokeOffset, paths[i].strokeCount);
                    gl->encoder->setTexture(0, gl->s_tex, gl->th);
                    gl->encoder->setTexture(1, gl->s_tex2, gl->th2);
                    outBuffer->Submit(*gl->encoder, prog, BGFX_DISCARD_ALL);
                }
            }
        };
        std::function filterPass = [gl, call](bgfx::ProgramHandle prog, Babylon::Graphics::FrameBuffer *inBuffer, Babylon::Graphics::FrameBuffer *outBuffer) {
            gl->encoder->setUniform(gl->u_viewSize, gl->view); // TODO: also set other common uniforms
            gl->encoder->setState(BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A);
            gl->encoder->setTexture(0, gl->s_tex, bgfx::getTexture(inBuffer->Handle()));
            bool s_originBottomLeft = bgfx::getCaps()->originBottomLeft;
            screenSpaceQuad(gl->encoder, s_originBottomLeft);
            outBuffer->Submit(*gl->encoder, prog, BGFX_DISCARD_ALL);
        };
        std::function finalPass = [gl, call](bgfx::ProgramHandle prog, Babylon::Graphics::FrameBuffer *inBuffer, Babylon::Graphics::FrameBuffer *outBuffer) {
            gl->encoder->setUniform(gl->u_viewSize, gl->view); // TODO: also set other common uniforms
            gl->encoder->setState(BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A
                | BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_ONE, BGFX_STATE_BLEND_INV_SRC_ALPHA)
                | BGFX_STATE_BLEND_EQUATION(BGFX_STATE_BLEND_EQUATION_ADD));
            gl->encoder->setTexture(0, gl->s_tex, bgfx::getTexture(inBuffer->Handle()));
            bool s_originBottomLeft = bgfx::getCaps()->originBottomLeft;
            screenSpaceQuad(gl->encoder, s_originBottomLeft);
            outBuffer->Submit(*gl->encoder, prog, BGFX_DISCARD_ALL);
        };
        Babylon::Graphics::FrameBuffer *finalFrameBuffer = gl->frameBuffer;
        finalFrameBuffer->Bind(*gl->encoder); // Should this be bound elsewhere?

        call->filterStack.Render(firstProg, setUniform, firstPass, filterPass, finalPass, finalFrameBuffer, gl->frameBufferPool.acquire, gl->frameBufferPool.release);
    }

    static void glnvg__stroke(struct GLNVGcontext* gl, struct GLNVGcall* call)
    {
        bgfx::ProgramHandle firstProg = gl->prog;
        std::function setUniform = [gl](bgfx::UniformHandle u, const void *value, const uint16_t num) {
            gl->encoder->setUniform(u, value, num);
        };
        std::function firstPass = [gl, call](bgfx::ProgramHandle prog, Babylon::Graphics::FrameBuffer *outBuffer) {
            // Draw Strokes
            struct GLNVGpath* paths = &gl->paths[call->pathOffset];
            int npaths = call->pathCount, i;
            for (i = 0; i < npaths; i++)
            {
                nvgRenderSetUniforms(gl, call->uniformOffset, call->image, call->image2);

                gl->encoder->setState(gl->state | BGFX_STATE_PT_TRISTRIP );
                gl->encoder->setVertexBuffer(0, &gl->tvb, paths[i].strokeOffset, paths[i].strokeCount);
                gl->encoder->setTexture(0, gl->s_tex, gl->th);
                gl->encoder->setTexture(1, gl->s_tex2, gl->th2);
                outBuffer->Submit(*gl->encoder, prog, BGFX_DISCARD_ALL);
            }
        };
        std::function filterPass = [gl, call](bgfx::ProgramHandle prog, Babylon::Graphics::FrameBuffer *inBuffer, Babylon::Graphics::FrameBuffer *outBuffer) {
            gl->encoder->setUniform(gl->u_viewSize, gl->view); // TODO: also set other common uniforms
            gl->encoder->setState(BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A);
            gl->encoder->setTexture(0, gl->s_tex, bgfx::getTexture(inBuffer->Handle()));
            bool s_originBottomLeft = bgfx::getCaps()->originBottomLeft;
            screenSpaceQuad(gl->encoder, s_originBottomLeft);
            outBuffer->Submit(*gl->encoder, prog, BGFX_DISCARD_ALL);
        };
        std::function finalPass = [gl, call](bgfx::ProgramHandle prog, Babylon::Graphics::FrameBuffer *inBuffer, Babylon::Graphics::FrameBuffer *outBuffer) {
            gl->encoder->setUniform(gl->u_viewSize, gl->view); // TODO: also set other common uniforms
            gl->encoder->setState(BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A
                | BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_ONE, BGFX_STATE_BLEND_INV_SRC_ALPHA)
                | BGFX_STATE_BLEND_EQUATION(BGFX_STATE_BLEND_EQUATION_ADD));
            gl->encoder->setTexture(0, gl->s_tex, bgfx::getTexture(inBuffer->Handle()));
            bool s_originBottomLeft = bgfx::getCaps()->originBottomLeft;
            screenSpaceQuad(gl->encoder, s_originBottomLeft);
            outBuffer->Submit(*gl->encoder, prog, BGFX_DISCARD_ALL);
        };
        Babylon::Graphics::FrameBuffer *finalFrameBuffer = gl->frameBuffer;
        finalFrameBuffer->Bind(*gl->encoder); // Should this be bound elsewhere?

        call->filterStack.Render(firstProg, setUniform, firstPass, filterPass, finalPass, finalFrameBuffer, gl->frameBufferPool.acquire, gl->frameBufferPool.release);
    }

    static void glnvg__triangles(struct GLNVGcontext* gl, struct GLNVGcall* call)
    {
        if (3 <= call->vertexCount)
        {
            bgfx::ProgramHandle firstProg = gl->prog;
            std::function setUniform = [gl](bgfx::UniformHandle u, const void *value, const uint16_t num) {
                gl->encoder->setUniform(u, value, num);
            };
            std::function firstPass = [gl, call](bgfx::ProgramHandle prog, Babylon::Graphics::FrameBuffer *outBuffer) {
                nvgRenderSetUniforms(gl, call->uniformOffset, call->image, call->image2);
                gl->encoder->setState(gl->state);
                gl->encoder->setVertexBuffer(0, &gl->tvb, call->vertexOffset, call->vertexCount);
                gl->encoder->setTexture(0, gl->s_tex, gl->th);
                gl->encoder->setTexture(1, gl->s_tex2, gl->th2);
                outBuffer->Submit(*gl->encoder, prog, BGFX_DISCARD_ALL);
            };
            std::function filterPass = [gl, call](bgfx::ProgramHandle prog, Babylon::Graphics::FrameBuffer *inBuffer, Babylon::Graphics::FrameBuffer *outBuffer) {
                gl->encoder->setUniform(gl->u_viewSize, gl->view); // TODO: also set other common uniforms
                gl->encoder->setState(BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A);
                gl->encoder->setTexture(0, gl->s_tex, bgfx::getTexture(inBuffer->Handle()));
                bool s_originBottomLeft = bgfx::getCaps()->originBottomLeft;
                screenSpaceQuad(gl->encoder, s_originBottomLeft);
                outBuffer->Submit(*gl->encoder, prog, BGFX_DISCARD_ALL);
			};
            std::function finalPass = [gl, call](bgfx::ProgramHandle prog, Babylon::Graphics::FrameBuffer *inBuffer, Babylon::Graphics::FrameBuffer *outBuffer) {
                gl->encoder->setUniform(gl->u_viewSize, gl->view); // TODO: also set other common uniforms
                gl->encoder->setState(BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A
                    | BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_ONE, BGFX_STATE_BLEND_INV_SRC_ALPHA)
                    | BGFX_STATE_BLEND_EQUATION(BGFX_STATE_BLEND_EQUATION_ADD));
                gl->encoder->setTexture(0, gl->s_tex, bgfx::getTexture(inBuffer->Handle()));
                bool s_originBottomLeft = bgfx::getCaps()->originBottomLeft;
                screenSpaceQuad(gl->encoder, s_originBottomLeft);
                outBuffer->Submit(*gl->encoder, prog, BGFX_DISCARD_ALL);
			};
            Babylon::Graphics::FrameBuffer *finalFrameBuffer = gl->frameBuffer;
            finalFrameBuffer->Bind(*gl->encoder); // Should this be bound elsewhere?

            call->filterStack.Render(firstProg, setUniform, firstPass, filterPass, finalPass, finalFrameBuffer, gl->frameBufferPool.acquire, gl->frameBufferPool.release);
        }
    }

    static const uint64_t s_blend[] =
    {
        BGFX_STATE_BLEND_ZERO,
        BGFX_STATE_BLEND_ONE,
        BGFX_STATE_BLEND_SRC_COLOR,
        BGFX_STATE_BLEND_INV_SRC_COLOR,
        BGFX_STATE_BLEND_DST_COLOR,
        BGFX_STATE_BLEND_INV_DST_COLOR,
        BGFX_STATE_BLEND_SRC_ALPHA,
        BGFX_STATE_BLEND_INV_SRC_ALPHA,
        BGFX_STATE_BLEND_DST_ALPHA,
        BGFX_STATE_BLEND_INV_DST_ALPHA,
        BGFX_STATE_BLEND_SRC_ALPHA_SAT,
    };

    static uint64_t glnvg_convertBlendFuncFactor(int factor)
    {
        const uint32_t numtz = bx::uint32_cnttz(factor);
        const uint32_t idx   = bx::uint32_min(numtz, BX_COUNTOF(s_blend)-1);
        return s_blend[idx];
    }

    static GLNVGblend glnvg__blendCompositeOperation(NVGcompositeOperationState op)
    {
        GLNVGblend blend;
        blend.srcRGB = glnvg_convertBlendFuncFactor(op.srcRGB);
        blend.dstRGB = glnvg_convertBlendFuncFactor(op.dstRGB);
        blend.srcAlpha = glnvg_convertBlendFuncFactor(op.srcAlpha);
        blend.dstAlpha = glnvg_convertBlendFuncFactor(op.dstAlpha);
        if (blend.srcRGB == BGFX_STATE_NONE || blend.dstRGB == BGFX_STATE_NONE || blend.srcAlpha == BGFX_STATE_NONE || blend.dstAlpha == BGFX_STATE_NONE)
        {
            blend.srcRGB = BGFX_STATE_BLEND_ONE;
            blend.dstRGB = BGFX_STATE_BLEND_INV_SRC_ALPHA;
            blend.srcAlpha = BGFX_STATE_BLEND_ONE;
            blend.dstAlpha = BGFX_STATE_BLEND_INV_SRC_ALPHA;
        }
        return blend;
    }

    static void nvgRenderFlush(void* _userPtr)
    {
        struct GLNVGcontext* gl = (struct GLNVGcontext*)_userPtr;
        //gl->frameBuffer->SetViewPort(gl->encoder, 0.f, 0.f, gl->view[0], gl->view[1]);
        if (!gl->prog.idx)
        {
            bgfx::RendererType::Enum type = bgfx::getRendererType();
            gl->prog = bgfx::createProgram(
                  bgfx::createEmbeddedShader(s_embeddedShadersBabylon, type, "vs_nanovg_fill")
                , bgfx::createEmbeddedShader(s_embeddedShadersBabylon, type, "fs_nanovg_fill")
                , true
            );

            // Vertex layout
            PosTexCoord0Vertex::init();
        }

        if (gl->ncalls > 0)
        {
            bgfx::allocTransientVertexBuffer(&gl->tvb, gl->nverts, s_nvgLayout);

            int allocated = gl->tvb.size/gl->tvb.stride;

            if (allocated < gl->nverts)
            {
                gl->nverts = allocated;
                BX_WARN(true, "Vertex number truncated due to transient vertex buffer overflow");
            }

            bx::memCopy(gl->tvb.data, gl->verts, gl->nverts * sizeof(struct NVGvertex) );

            gl->encoder->setUniform(gl->u_viewSize, gl->view);

            for (uint32_t ii = 0, num = gl->ncalls; ii < num; ++ii)
            {
                struct GLNVGcall* call = &gl->calls[ii];

                const GLNVGblend* blend = &call->blendFunc;
                gl->state = BGFX_STATE_BLEND_FUNC_SEPARATE(blend->srcRGB, blend->dstRGB, blend->srcAlpha, blend->dstAlpha)
                    | BGFX_STATE_WRITE_RGB
                    | BGFX_STATE_WRITE_A
                    ;
                switch (call->type)
                {
                case GLNVG_FILL:
                    glnvg__fill(gl, call);
                    break;

                case GLNVG_CONVEXFILL:
                    glnvg__convexFill(gl, call);
                    break;

                case GLNVG_STROKE:
                    glnvg__stroke(gl, call);
                    break;

                case GLNVG_TRIANGLES:
                    glnvg__triangles(gl, call);
                    break;
                }
            }
        }

        // Reset calls
        gl->nverts    = 0;
        gl->npaths    = 0;
        gl->ncalls    = 0;
        gl->nuniforms = 0;
    }

    static int glnvg__maxVertCount(const struct NVGpath* paths, int npaths)
    {
        int i, count = 0;
        for (i = 0; i < npaths; i++)
        {
            count += paths[i].nfill;
            count += paths[i].nstroke;
        }
        return count;
    }

    static int glnvg__maxi(int a, int b) { return a > b ? a : b; }

    static struct GLNVGcall* glnvg__allocCall(struct GLNVGcontext* gl)
    {
        struct GLNVGcall* ret = NULL;
        if (gl->ncalls+1 > gl->ccalls)
        {
            gl->ccalls = gl->ccalls == 0 ? 32 : gl->ccalls * 2;
            gl->calls = (struct GLNVGcall*)bx::realloc(gl->allocator, gl->calls, sizeof(struct GLNVGcall) * gl->ccalls);
        }
        ret = &gl->calls[gl->ncalls++];
        bx::memSet(ret, 0, sizeof(struct GLNVGcall) );
        return ret;
    }

    static int glnvg__allocPaths(struct GLNVGcontext* gl, int n)
    {
        int ret = 0;
        if (gl->npaths + n > gl->cpaths) {
            GLNVGpath* paths;
            int cpaths = glnvg__maxi(gl->npaths + n, 128) + gl->cpaths / 2; // 1.5x Overallocate
            paths = (GLNVGpath*)bx::realloc(gl->allocator, gl->paths, sizeof(GLNVGpath) * cpaths);
            if (paths == NULL) return -1;
            gl->paths = paths;
            gl->cpaths = cpaths;
        }
        ret = gl->npaths;
        gl->npaths += n;
        return ret;
    }

    static int glnvg__allocVerts(GLNVGcontext* gl, int n)
    {
        int ret = 0;
        if (gl->nverts+n > gl->cverts)
        {
            NVGvertex* verts;
            int cverts = glnvg__maxi(gl->nverts + n, 4096) + gl->cverts/2; // 1.5x Overallocate
            verts = (NVGvertex*)bx::realloc(gl->allocator, gl->verts, sizeof(NVGvertex) * cverts);
            if (verts == NULL) return -1;
            gl->verts = verts;
            gl->cverts = cverts;
        }
        ret = gl->nverts;
        gl->nverts += n;
        return ret;
    }

    static int glnvg__allocFragUniforms(struct GLNVGcontext* gl, int n)
    {
        int ret = 0, structSize = gl->fragSize;
        if (gl->nuniforms+n > gl->cuniforms)
        {
            gl->cuniforms = gl->cuniforms == 0 ? glnvg__maxi(n, 32) : gl->cuniforms * 2;
            gl->uniforms = (unsigned char*)bx::realloc(gl->allocator, gl->uniforms, gl->cuniforms * structSize);
        }
        ret = gl->nuniforms * structSize;
        gl->nuniforms += n;
        return ret;
    }

    static void glnvg__vset(struct NVGvertex* vtx, float x, float y, float u, float v)
    {
        vtx->x = x;
        vtx->y = y;
        vtx->u = u;
        vtx->v = v;
    }

    static void nvgRenderFill(
          void* _userPtr
        , NVGpaint* paint
        , NVGcompositeOperationState compositeOperation
        , NVGscissor* scissor
        , float fringe
        , const float* bounds
        , const NVGpath* paths
        , int npaths
        , nanovg_filterstack& filterStack
        )
    {
        struct GLNVGcontext* gl = (struct GLNVGcontext*)_userPtr;

        struct GLNVGcall* call = glnvg__allocCall(gl);
        struct NVGvertex* quad;
        struct GLNVGfragUniforms* frag;
        int i, maxverts, offset;

        call->type = GLNVG_FILL;
        call->pathOffset = glnvg__allocPaths(gl, npaths);
        call->pathCount = npaths;
        call->image = paint->image;
        call->image2 = paint->image2;
        call->blendFunc = glnvg__blendCompositeOperation(compositeOperation);
        call->filterStack = filterStack;

        if (npaths == 1 && paths[0].convex)
        {
            call->type = GLNVG_CONVEXFILL;
        }

        // Allocate vertices for all the paths.
        maxverts = glnvg__maxVertCount(paths, npaths) + 6;
        offset = glnvg__allocVerts(gl, maxverts);

        for (i = 0; i < npaths; i++)
        {
            struct GLNVGpath* copy = &gl->paths[call->pathOffset + i];
            const struct NVGpath* path = &paths[i];
            bx::memSet(copy, 0, sizeof(struct GLNVGpath) );
            if (path->nfill > 0)
            {
                copy->fillOffset = offset;
                copy->fillCount = path->nfill;
                bx::memCopy(&gl->verts[offset], path->fill, sizeof(struct NVGvertex) * path->nfill);
                offset += path->nfill;
            }

            if (path->nstroke > 0)
            {
                copy->strokeOffset = offset;
                copy->strokeCount = path->nstroke;
                bx::memCopy(&gl->verts[offset], path->stroke, sizeof(struct NVGvertex) * path->nstroke);
                offset += path->nstroke;
            }
        }

        // Quad
        call->vertexOffset = offset;
        call->vertexCount = 6;
        quad = &gl->verts[call->vertexOffset];
        glnvg__vset(&quad[0], bounds[0], bounds[3], 0.5f, 1.0f);
        glnvg__vset(&quad[1], bounds[2], bounds[3], 0.5f, 1.0f);
        glnvg__vset(&quad[2], bounds[2], bounds[1], 0.5f, 1.0f);

        glnvg__vset(&quad[3], bounds[0], bounds[3], 0.5f, 1.0f);
        glnvg__vset(&quad[4], bounds[2], bounds[1], 0.5f, 1.0f);
        glnvg__vset(&quad[5], bounds[0], bounds[1], 0.5f, 1.0f);

        // Setup uniforms for draw calls
        if (call->type == GLNVG_FILL)
        {
            call->uniformOffset = glnvg__allocFragUniforms(gl, 2);
            // Simple shader for stencil
            frag = nvg__fragUniformPtr(gl, call->uniformOffset);
            bx::memSet(frag, 0, sizeof(*frag) );
            frag->type = NSVG_SHADER_SIMPLE;
            // Fill shader
            glnvg__convertPaint(gl, nvg__fragUniformPtr(gl, call->uniformOffset + gl->fragSize), paint, scissor, fringe, fringe);
        }
        else
        {
            call->uniformOffset = glnvg__allocFragUniforms(gl, 1);
            // Fill shader
            glnvg__convertPaint(gl, nvg__fragUniformPtr(gl, call->uniformOffset), paint, scissor, fringe, fringe);
        }
    }

    static void nvgRenderStroke(
          void* _userPtr
        , struct NVGpaint* paint
        , NVGcompositeOperationState compositeOperation
        , struct NVGscissor* scissor
        , float fringe
        , float strokeWidth
        , const struct NVGpath* paths
        , int npaths
        , nanovg_filterstack& filterStack
        )
    {
        struct GLNVGcontext* gl = (struct GLNVGcontext*)_userPtr;

        struct GLNVGcall* call = glnvg__allocCall(gl);
        int i, maxverts, offset;

        call->type = GLNVG_STROKE;
        call->pathOffset = glnvg__allocPaths(gl, npaths);
        call->pathCount = npaths;
        call->image = paint->image;
        call->image2 = paint->image2;
        call->blendFunc = glnvg__blendCompositeOperation(compositeOperation);
        call->filterStack = filterStack;

        // Allocate vertices for all the paths.
        maxverts = glnvg__maxVertCount(paths, npaths);
        offset = glnvg__allocVerts(gl, maxverts);

        for (i = 0; i < npaths; i++)
        {
            struct GLNVGpath* copy = &gl->paths[call->pathOffset + i];
            const struct NVGpath* path = &paths[i];
            bx::memSet(copy, 0, sizeof(struct GLNVGpath) );
            if (path->nstroke)
            {
                copy->strokeOffset = offset;
                copy->strokeCount = path->nstroke;
                bx::memCopy(&gl->verts[offset], path->stroke, sizeof(struct NVGvertex) * path->nstroke);
                offset += path->nstroke;
            }
        }

        // Fill shader
        call->uniformOffset = glnvg__allocFragUniforms(gl, 1);
        glnvg__convertPaint(gl, nvg__fragUniformPtr(gl, call->uniformOffset), paint, scissor, strokeWidth, fringe);
    }

    static void nvgRenderTriangles(void* _userPtr, struct NVGpaint* paint, NVGcompositeOperationState compositeOperation, struct NVGscissor* scissor,
                                       const struct NVGvertex* verts, int nverts, nanovg_filterstack& filterStack)
    {
        struct GLNVGcontext* gl = (struct GLNVGcontext*)_userPtr;
        struct GLNVGcall* call = glnvg__allocCall(gl);
        struct GLNVGfragUniforms* frag;

        call->type = GLNVG_TRIANGLES;
        call->image = paint->image;
        call->image2 = paint->image2;
        call->blendFunc = glnvg__blendCompositeOperation(compositeOperation);
        call->filterStack = filterStack;

        // Allocate vertices for all the paths.
        call->vertexOffset = glnvg__allocVerts(gl, nverts);
        call->vertexCount = nverts;
        bx::memCopy(&gl->verts[call->vertexOffset], verts, sizeof(struct NVGvertex) * nverts);

        // Fill shader
        call->uniformOffset = glnvg__allocFragUniforms(gl, 1);
        frag = nvg__fragUniformPtr(gl, call->uniformOffset);
        glnvg__convertPaint(gl, frag, paint, scissor, 1.0f, 1.0f);
        frag->type = bgfx::isValid(gl->th2) ? NSVG_SHADER_IMG_MODULATEGRAD : NSVG_SHADER_IMG;
    }

    static void nvgRenderDelete(void* _userPtr)
    {
        struct GLNVGcontext* gl = (struct GLNVGcontext*)_userPtr;

        if (gl == NULL)
        {
            return;
        }

        // gl->prog.idx can be 0 is a context is destroyed without a call to flush
        if (gl->prog.idx)
        {
            bgfx::destroy(gl->prog);
        }
        bgfx::destroy(gl->texMissing);

        bgfx::destroy(gl->u_scissorMat);
        bgfx::destroy(gl->u_paintMat);
        bgfx::destroy(gl->u_innerCol);
        bgfx::destroy(gl->u_outerCol);
        bgfx::destroy(gl->u_viewSize);
        bgfx::destroy(gl->u_scissorExtScale);
        bgfx::destroy(gl->u_extentRadius);
        bgfx::destroy(gl->u_params);
        bgfx::destroy(gl->u_sdf);
        bgfx::destroy(gl->s_tex);
        bgfx::destroy(gl->s_tex2);
        nanovg_filterstack::DisposeBgfx();

        if (bgfx::isValid(gl->u_halfTexel) )
        {
            bgfx::destroy(gl->u_halfTexel);
        }

        for (uint32_t ii = 0, num = gl->ntextures; ii < num; ++ii)
        {
            if (bgfx::isValid(gl->textures[ii].id)
            && (gl->textures[ii].flags & NVG_IMAGE_NODELETE) == 0)
            {
                bgfx::destroy(gl->textures[ii].id);
            }
        }

        bx::free(gl->allocator, gl->uniforms);
        bx::free(gl->allocator, gl->verts);
        bx::free(gl->allocator, gl->paths);
        bx::free(gl->allocator, gl->calls);
        bx::free(gl->allocator, gl->textures);
        bx::free(gl->allocator, gl);
    }

} // namespace

NVGcontext* nvgCreate(int32_t _edgeaa, bx::AllocatorI* _allocator)
{
    if (NULL == _allocator)
    {
        _allocator = &Babylon::Graphics::DeviceContext::GetDefaultAllocator();
    }

    struct NVGparams params;
    struct NVGcontext* ctx = NULL;
    struct GLNVGcontext* gl = (struct GLNVGcontext*)bx::alloc(_allocator, sizeof(struct GLNVGcontext) );
    if (gl == NULL)
    {
        goto error;
    }

    bx::memSet(gl, 0, sizeof(struct GLNVGcontext) );

    bx::memSet(&params, 0, sizeof(params) );
    params.renderCreate         = nvgRenderCreate;
    params.renderCreateTexture  = nvgRenderCreateTexture;
    params.renderDeleteTexture  = nvgRenderDeleteTexture;
    params.renderUpdateTexture  = nvgRenderUpdateTexture;
    params.renderGetTextureSize = nvgRenderGetTextureSize;
    params.renderViewport       = nvgRenderViewport;
    params.renderFlush          = nvgRenderFlush;
    params.renderFill           = nvgRenderFill;
    params.renderStroke         = nvgRenderStroke;
    params.renderTriangles      = nvgRenderTriangles;
    params.renderDelete         = nvgRenderDelete;
    params.userPtr              = gl;
    params.edgeAntiAlias        = _edgeaa;

    gl->allocator     = _allocator;
    gl->edgeAntiAlias = _edgeaa;

    ctx = nvgCreateInternal(&params);
    if (ctx == NULL) goto error;

    return ctx;

error:
    // 'gl' is freed by nvgDeleteInternal.
    if (ctx != NULL)
    {
        nvgDeleteInternal(ctx);
    }

    return NULL;
}

void nvgSetFrameBufferPool(NVGcontext* _ctx, PoolInterface pool)
{
    struct GLNVGcontext* gl = (GLNVGcontext*)nvgInternalParams(_ctx)->userPtr;
    gl->frameBufferPool = pool;
}

void nvgSetFrameBufferAndEncoder(NVGcontext* _ctx, Babylon::Graphics::FrameBuffer& frameBuffer, bgfx::Encoder* encoder)
{
    struct GLNVGcontext* gl = (GLNVGcontext*)nvgInternalParams(_ctx)->userPtr;
    gl->encoder = encoder;
    gl->frameBuffer = &frameBuffer;
}

NVGcontext* nvgCreate(int32_t _edgeaa) {
    return nvgCreate(_edgeaa, nullptr);
}

void nvgDelete(NVGcontext* _ctx)
{
    nvgDeleteInternal(_ctx);
}

bgfx::TextureHandle nvglImageHandle(NVGcontext* _ctx, int32_t _image)
{
    GLNVGcontext* gl = (GLNVGcontext*)nvgInternalParams(_ctx)->userPtr;
    GLNVGtexture* tex = glnvg__findTexture(gl, _image);
    return tex->id;
}
