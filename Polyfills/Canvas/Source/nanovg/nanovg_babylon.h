/*
 * Copyright 2011-2020 Branimir Karadzic. All rights reserved.
 * License: https://github.com/bkaradzic/bgfx#license-bsd-2-clause
 */

#ifndef NANOVG_BABYLON_H_HEADER_GUARD
#define NANOVG_BABYLON_H_HEADER_GUARD

#include <functional>
#include <bgfx/bgfx.h>
#include <Babylon/Graphics/FrameBuffer.h>
namespace bx
{
    struct AllocatorI;
}

struct NVGcontext;

///
NVGcontext* nvgCreate(int32_t _edgeaa, bx::AllocatorI* _allocator);

///
NVGcontext* nvgCreate(int32_t _edgeaa);

struct PoolInterface
{
    std::function<Babylon::Graphics::FrameBuffer*()> acquire;
    std::function<void(Babylon::Graphics::FrameBuffer*)> release;
};

void nvgSetFrameBufferPool(NVGcontext* _ctx, PoolInterface pool);
void nvgSetFrameBufferAndEncoder(NVGcontext* _ctx, Babylon::Graphics::FrameBuffer& frameBuffer, bgfx::Encoder* encoder);

///
void nvgDelete(NVGcontext* _ctx);

/// Register an existing bgfx texture as an NVG image without taking ownership
/// (NVG_IMAGE_NODELETE). Returns the image id, or 0 on failure.
int nvgCreateImageFromHandle(NVGcontext* _ctx, bgfx::TextureHandle _handle, int _width, int _height, int _flags);

#endif // NANOVG_BABYLON_H_HEADER_GUARD
