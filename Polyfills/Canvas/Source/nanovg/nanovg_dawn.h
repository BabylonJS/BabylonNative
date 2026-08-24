// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
//
// NanoVG render backend for Dawn / WebGPU.
//
// This is the WebGPU counterpart of nanovg_babylon.cpp (the bgfx backend). It
// implements the same NVGparams callback surface, so all of NanoVG's geometry
// generation, tessellation and font handling is shared unchanged; only the
// submission layer differs.
//
// Unlike the bgfx backend it does not depend on Babylon::Graphics at all (there
// is no bgfx device on the Dawn path). The caller owns the target texture and
// tells the backend which render pass to record into.

#ifndef NANOVG_DAWN_H_HEADER_GUARD
#define NANOVG_DAWN_H_HEADER_GUARD

#include <cstdint>

#include <webgpu/webgpu.h>

struct NVGcontext;

namespace Babylon::Polyfills::Internal
{
    // Creates a NanoVG context backed by WebGPU. `device` must outlive the
    // context. `colorFormat` is the format of the texture that will be rendered
    // into; pipelines are created against it.
    //
    // `edgeAntiAlias` mirrors nvgCreate's flag in the bgfx backend.
    NVGcontext* NvgCreateDawn(WGPUDevice device, WGPUTextureFormat colorFormat, int32_t edgeAntiAlias);

    // Destroys a context created by NvgCreateDawn.
    void NvgDeleteDawn(NVGcontext* ctx);

    // Binds the render pass that subsequent nvg draw calls record into. NanoVG
    // buffers all geometry until nvgEndFrame, so this must be set before the
    // frame is ended (i.e. before the backend's renderFlush runs).
    //
    // The pass must have a depth-stencil attachment using the format reported by
    // NvgDawnStencilFormat(), because the non-convex fill path is stencil based.
    void NvgSetDawnRenderPass(NVGcontext* ctx, WGPURenderPassEncoder pass);

    // Depth-stencil format the pipelines are created with. The caller must
    // allocate its depth-stencil attachment with this format.
    //
    // The stencil buffer must be cleared to 0 at the start of the pass: the
    // non-convex fill path assumes it starts zeroed and restores it to zero as
    // it draws.
    WGPUTextureFormat NvgDawnStencilFormat();
}

#endif // NANOVG_DAWN_H_HEADER_GUARD
