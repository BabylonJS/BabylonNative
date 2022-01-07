/*
 * Copyright 2011-2020 Branimir Karadzic. All rights reserved.
 * License: https://github.com/bkaradzic/bgfx#license-bsd-2-clause
 */

#ifndef NANOVG_BABYLON_H_HEADER_GUARD
#define NANOVG_BABYLON_H_HEADER_GUARD

#include <bgfx/bgfx.h>
#include <GraphicsContext.h>
#include <FrameBuffer.h>
namespace bx { struct AllocatorI; }

struct NVGcontext;

///
NVGcontext* nvgCreate(int32_t _edgeaa, bx::AllocatorI* _allocator);

///
NVGcontext* nvgCreate(int32_t _edgeaa);

void nvgSetFrameBufferAndEncoder(NVGcontext* _ctx, Babylon::FrameBuffer& frameBuffer, bgfx::Encoder* encoder);

///
void nvgDelete(NVGcontext* _ctx);

#endif // NANOVG_BABYLON_H_HEADER_GUARD
