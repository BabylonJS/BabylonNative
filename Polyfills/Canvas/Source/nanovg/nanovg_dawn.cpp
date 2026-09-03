// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
//
// NanoVG render backend for Dawn / WebGPU. See nanovg_dawn.h.
//
// Structure deliberately mirrors nanovg_babylon.cpp (the bgfx backend) so the
// two can be diffed: same call/path/uniform bookkeeping, same draw ordering.
// The differences are all in submission:
//
//   * bgfx uniforms -> one dynamic-offset uniform buffer, one record per draw.
//   * bgfx render state -> pre-created WGPURenderPipeline objects, cached on the
//     (topology, stencil mode, color-write, blend) tuple.
//   * bgfx transient buffers -> a single growable vertex buffer rewritten each
//     flush, plus a growable index buffer for the triangle-fan emulation.
//
// WebGPU has no triangle-fan topology, so fans are expanded into indexed
// triangle lists exactly like the bgfx backend's fan() helper does.

#include "nanovg/nanovg_dawn.h"
#include "nanovg_dawn_shader.h"

#include "nanovg.h"

#include <algorithm>
#include <cmath>
#include <cstring>
#include <map>
#include <vector>

namespace Babylon::Polyfills::Internal
{
    namespace
    {
        // Must match FragUniforms in nanovg_dawn_shader.h.
        struct DawnFragUniforms
        {
            float scissorMat[12]; // 3 padded vec4 columns
            float paintMat[12];   // 3 padded vec4 columns
            float innerCol[4];
            float outerCol[4];
            float scissorExtScale[4]; // xy = ext, zw = scale
            float extentRadius[4];    // xy = extent, z = radius, w = feather
            float params[4];          // feather, strokeMult, texType, type
            float sdf[4];             // sdfMin, sdfMax, sdfBlur, unused
            float viewRect[4];        // width, height, unused, unused
        };
        static_assert(sizeof(DawnFragUniforms) == 13 * 16, "uniform layout must match the WGSL struct");

        // WebGPU's default minUniformBufferOffsetAlignment. Dynamic offsets must
        // be a multiple of this, so each per-draw record is padded up to it.
        constexpr uint32_t UNIFORM_STRIDE = 256;
        static_assert(sizeof(DawnFragUniforms) <= UNIFORM_STRIDE, "uniform record must fit the stride");

        constexpr WGPUTextureFormat STENCIL_FORMAT = WGPUTextureFormat_Depth24PlusStencil8;

        // Mirrors GLNVGshaderType in nanovg_babylon.cpp; the values are the
        // `type` discriminant the fragment shader switches on.
        enum NvgShaderType
        {
            NSVG_SHADER_FILLGRAD = 0,
            NSVG_SHADER_FILLIMG = 1,
            NSVG_SHADER_SIMPLE = 2,
            NSVG_SHADER_IMG = 3,
            NSVG_SHADER_IMG_MODULATEGRAD = 4,
        };

        enum CallType
        {
            DAWNNVG_NONE = 0,
            DAWNNVG_FILL,
            DAWNNVG_CONVEXFILL,
            DAWNNVG_STROKE,
            DAWNNVG_TRIANGLES,
        };

        // The distinct stencil configurations the fill algorithm needs.
        enum class StencilMode
        {
            None,       // no stencil interaction
            FillShapes, // write winding counts, no color
            FillFringe, // test == 0, keep
            FillEnd,    // test != 0, zero out
        };

        struct PipelineKey
        {
            WGPUPrimitiveTopology topology;
            StencilMode stencil;
            bool colorWrite;
            WGPUBlendFactor srcRGB;
            WGPUBlendFactor dstRGB;
            WGPUBlendFactor srcAlpha;
            WGPUBlendFactor dstAlpha;

            bool operator<(const PipelineKey& o) const
            {
                return std::tie(topology, stencil, colorWrite, srcRGB, dstRGB, srcAlpha, dstAlpha) <
                    std::tie(o.topology, o.stencil, o.colorWrite, o.srcRGB, o.dstRGB, o.srcAlpha, o.dstAlpha);
            }
        };

        struct DawnBlend
        {
            WGPUBlendFactor srcRGB;
            WGPUBlendFactor dstRGB;
            WGPUBlendFactor srcAlpha;
            WGPUBlendFactor dstAlpha;
        };

        struct DawnCall
        {
            int type;
            int image;
            int image2;
            int pathOffset;
            int pathCount;
            int triangleOffset;
            int triangleCount;
            int uniformOffset;
            DawnBlend blendFunc;
        };

        struct DawnPath
        {
            int fillOffset;
            int fillCount;
            int strokeOffset;
            int strokeCount;
            // Offset into the index buffer of this path's triangle-fan indices,
            // assigned while the index data is built in renderFlush.
            uint32_t fanIndexOffset;
        };

        struct DawnTexture
        {
            int id;
            WGPUTexture texture;
            WGPUTextureView view;
            int width;
            int height;
            int type;
            int flags;
        };

        struct DawnContext
        {
            WGPUDevice device{};
            WGPUQueue queue{};
            WGPUTextureFormat colorFormat{};

            WGPUShaderModule shader{};
            WGPUBindGroupLayout bindGroupLayout{};
            WGPUPipelineLayout pipelineLayout{};
            std::map<PipelineKey, WGPURenderPipeline> pipelines;

            WGPUSampler samplerRepeat{};
            WGPUSampler samplerClamp{};

            WGPUBuffer vertexBuffer{};
            uint64_t vertexCapacity{};
            WGPUBuffer indexBuffer{};
            uint64_t indexCapacity{};
            WGPUBuffer uniformBuffer{};
            uint64_t uniformCapacity{};

            // 1x1 transparent texture bound wherever a draw has no image, so the
            // bind group layout is always satisfiable.
            WGPUTexture texMissing{};
            WGPUTextureView texMissingView{};

            WGPURenderPassEncoder pass{};

            std::vector<DawnTexture> textures;
            // Textures nanovg has asked us to delete but whose contents are
            // still referenced by draw calls recorded earlier in this frame.
            // nanovg resolves texture handles at flush time, not at record
            // time, so releasing on request would either dangle (fontstash
            // grows its atlas mid-frame and immediately deletes the old one) or
            // silently fall back to the missing-texture placeholder. Retired
            // here and released once the flush has submitted its work.
            std::vector<DawnTexture> retiredTextures;
            int textureId{};

            float view[2]{};
            int edgeAntiAlias{};

            // Per-frame buffers, reset by renderFlush.
            std::vector<DawnCall> calls;
            std::vector<DawnPath> paths;
            std::vector<NVGvertex> verts;
            std::vector<uint8_t> uniforms; // UNIFORM_STRIDE-strided records
            std::vector<uint32_t> indices;

            // Bind groups are cached per (uniform record, tex, tex2) draw; they
            // are cheap to recreate but not free, so they are pooled per flush.
            std::vector<WGPUBindGroup> frameBindGroups;
        };

        DawnTexture* FindTexture(DawnContext* dc, int id)
        {
            if (id == 0)
            {
                return nullptr;
            }
            for (auto& t : dc->textures)
            {
                if (t.id == id)
                {
                    return &t;
                }
            }
            // nanovg allocates texture ids monotonically and never reuses them,
            // so a retired texture can still be matched unambiguously by a draw
            // call that was recorded before the delete request.
            for (auto& t : dc->retiredTextures)
            {
                if (t.id == id)
                {
                    return &t;
                }
            }
            return nullptr;
        }

        WGPUTextureView TextureViewOr(DawnContext* dc, int image)
        {
            DawnTexture* t = FindTexture(dc, image);
            return (t != nullptr && t->view != nullptr) ? t->view : dc->texMissingView;
        }

        void XformToMat3x4(float* m3, const float* t)
        {
            m3[0] = t[0];  m3[1] = t[1];  m3[2] = 0.0f;  m3[3] = 0.0f;
            m3[4] = t[2];  m3[5] = t[3];  m3[6] = 0.0f;  m3[7] = 0.0f;
            m3[8] = t[4];  m3[9] = t[5];  m3[10] = 1.0f; m3[11] = 0.0f;
        }

        NVGcolor PremulColor(NVGcolor c)
        {
            c.r *= c.a;
            c.g *= c.a;
            c.b *= c.a;
            return c;
        }

        // ---- resource helpers ------------------------------------------------

        WGPUBuffer CreateBuffer(WGPUDevice device, uint64_t size, WGPUBufferUsage usage)
        {
            WGPUBufferDescriptor desc{};
            desc.size = size;
            desc.usage = usage | WGPUBufferUsage_CopyDst;
            desc.mappedAtCreation = false;
            return wgpuDeviceCreateBuffer(device, &desc);
        }

        // Grows `buffer` to at least `needed` bytes, rounding up so a canvas that
        // steadily adds geometry does not reallocate on every flush.
        void EnsureBuffer(DawnContext* dc, WGPUBuffer& buffer, uint64_t& capacity, uint64_t needed, WGPUBufferUsage usage)
        {
            if (needed <= capacity && buffer != nullptr)
            {
                return;
            }
            if (buffer != nullptr)
            {
                wgpuBufferDestroy(buffer);
                wgpuBufferRelease(buffer);
                buffer = nullptr;
            }
            uint64_t size = std::max<uint64_t>(needed, 4096);
            size = (size + 4095) & ~static_cast<uint64_t>(4095);
            buffer = CreateBuffer(dc->device, size, usage);
            capacity = size;
        }

        WGPUStencilFaceState MakeStencilFace(WGPUCompareFunction compare, WGPUStencilOperation pass)
        {
            WGPUStencilFaceState s{};
            s.compare = compare;
            s.failOp = WGPUStencilOperation_Keep;
            s.depthFailOp = WGPUStencilOperation_Keep;
            s.passOp = pass;
            return s;
        }

        WGPURenderPipeline GetPipeline(DawnContext* dc, const PipelineKey& key)
        {
            auto it = dc->pipelines.find(key);
            if (it != dc->pipelines.end())
            {
                return it->second;
            }

            WGPUVertexAttribute attrs[2]{};
            attrs[0].format = WGPUVertexFormat_Float32x2;
            attrs[0].offset = 0;
            attrs[0].shaderLocation = 0;
            attrs[1].format = WGPUVertexFormat_Float32x2;
            attrs[1].offset = 8;
            attrs[1].shaderLocation = 1;

            WGPUVertexBufferLayout vbl{};
            vbl.arrayStride = sizeof(NVGvertex);
            vbl.stepMode = WGPUVertexStepMode_Vertex;
            vbl.attributeCount = 2;
            vbl.attributes = attrs;

            WGPUVertexState vertex{};
            vertex.module = dc->shader;
            vertex.entryPoint = {"vertexMain", WGPU_STRLEN};
            vertex.bufferCount = 1;
            vertex.buffers = &vbl;

            WGPUBlendState blend{};
            blend.color.operation = WGPUBlendOperation_Add;
            blend.color.srcFactor = key.srcRGB;
            blend.color.dstFactor = key.dstRGB;
            blend.alpha.operation = WGPUBlendOperation_Add;
            blend.alpha.srcFactor = key.srcAlpha;
            blend.alpha.dstFactor = key.dstAlpha;

            WGPUColorTargetState target{};
            target.format = dc->colorFormat;
            target.blend = &blend;
            target.writeMask = key.colorWrite ? WGPUColorWriteMask_All : WGPUColorWriteMask_None;

            WGPUFragmentState fragment{};
            fragment.module = dc->shader;
            fragment.entryPoint = {"fragmentMain", WGPU_STRLEN};
            fragment.targetCount = 1;
            fragment.targets = &target;

            WGPUDepthStencilState depthStencil{};
            depthStencil.format = STENCIL_FORMAT;
            depthStencil.depthWriteEnabled = WGPUOptionalBool_False;
            depthStencil.depthCompare = WGPUCompareFunction_Always;
            depthStencil.stencilReadMask = 0xFF;
            depthStencil.stencilWriteMask = 0xFF;

            switch (key.stencil)
            {
                case StencilMode::FillShapes:
                    // Non-zero winding: front faces increment, back faces
                    // decrement, both wrapping (matches the bgfx backend's
                    // PASS_Z_INCR / PASS_Z_DECR pair).
                    depthStencil.stencilFront = MakeStencilFace(WGPUCompareFunction_Always, WGPUStencilOperation_IncrementWrap);
                    depthStencil.stencilBack = MakeStencilFace(WGPUCompareFunction_Always, WGPUStencilOperation_DecrementWrap);
                    break;
                case StencilMode::FillFringe:
                    depthStencil.stencilFront = MakeStencilFace(WGPUCompareFunction_Equal, WGPUStencilOperation_Keep);
                    depthStencil.stencilBack = depthStencil.stencilFront;
                    break;
                case StencilMode::FillEnd:
                    depthStencil.stencilFront = MakeStencilFace(WGPUCompareFunction_NotEqual, WGPUStencilOperation_Zero);
                    depthStencil.stencilFront.failOp = WGPUStencilOperation_Zero;
                    depthStencil.stencilFront.depthFailOp = WGPUStencilOperation_Zero;
                    depthStencil.stencilBack = depthStencil.stencilFront;
                    break;
                case StencilMode::None:
                default:
                    depthStencil.stencilFront = MakeStencilFace(WGPUCompareFunction_Always, WGPUStencilOperation_Keep);
                    depthStencil.stencilBack = depthStencil.stencilFront;
                    depthStencil.stencilWriteMask = 0;
                    break;
            }

            WGPUPrimitiveState primitive{};
            primitive.topology = key.topology;
            primitive.cullMode = WGPUCullMode_None;
            primitive.frontFace = WGPUFrontFace_CCW;
            if (key.topology == WGPUPrimitiveTopology_TriangleStrip)
            {
                primitive.stripIndexFormat = WGPUIndexFormat_Undefined;
            }

            WGPURenderPipelineDescriptor desc{};
            desc.layout = dc->pipelineLayout;
            desc.vertex = vertex;
            desc.primitive = primitive;
            desc.depthStencil = &depthStencil;
            desc.fragment = &fragment;
            desc.multisample.count = 1;
            desc.multisample.mask = 0xFFFFFFFF;

            WGPURenderPipeline pipeline = wgpuDeviceCreateRenderPipeline(dc->device, &desc);
            dc->pipelines.emplace(key, pipeline);
            return pipeline;
        }

        WGPUBindGroup MakeBindGroup(DawnContext* dc, int image, int image2)
        {
            WGPUBindGroupEntry entries[4]{};
            entries[0].binding = 0;
            entries[0].buffer = dc->uniformBuffer;
            entries[0].offset = 0;
            entries[0].size = sizeof(DawnFragUniforms);

            DawnTexture* tex = FindTexture(dc, image);
            const bool repeat = tex != nullptr && (tex->flags & (NVG_IMAGE_REPEATX | NVG_IMAGE_REPEATY)) != 0;
            entries[1].binding = 1;
            entries[1].sampler = repeat ? dc->samplerRepeat : dc->samplerClamp;

            entries[2].binding = 2;
            entries[2].textureView = TextureViewOr(dc, image);
            entries[3].binding = 3;
            entries[3].textureView = TextureViewOr(dc, image2);

            WGPUBindGroupDescriptor desc{};
            desc.layout = dc->bindGroupLayout;
            desc.entryCount = 4;
            desc.entries = entries;
            WGPUBindGroup bg = wgpuDeviceCreateBindGroup(dc->device, &desc);
            dc->frameBindGroups.push_back(bg);
            return bg;
        }

        // ---- NVGparams callbacks --------------------------------------------

        int RenderCreate(void* userPtr)
        {
            DawnContext* dc = static_cast<DawnContext*>(userPtr);

            WGPUShaderSourceWGSL wgslDesc{};
            wgslDesc.chain.sType = WGPUSType_ShaderSourceWGSL;
            wgslDesc.code = {NANOVG_DAWN_WGSL, WGPU_STRLEN};
            WGPUShaderModuleDescriptor smDesc{};
            smDesc.nextInChain = &wgslDesc.chain;
            dc->shader = wgpuDeviceCreateShaderModule(dc->device, &smDesc);
            if (dc->shader == nullptr)
            {
                return 0;
            }

            WGPUBindGroupLayoutEntry entries[4]{};
            entries[0].binding = 0;
            entries[0].visibility = WGPUShaderStage_Vertex | WGPUShaderStage_Fragment;
            entries[0].buffer.type = WGPUBufferBindingType_Uniform;
            entries[0].buffer.hasDynamicOffset = true;
            entries[0].buffer.minBindingSize = sizeof(DawnFragUniforms);

            entries[1].binding = 1;
            entries[1].visibility = WGPUShaderStage_Fragment;
            entries[1].sampler.type = WGPUSamplerBindingType_Filtering;

            entries[2].binding = 2;
            entries[2].visibility = WGPUShaderStage_Fragment;
            entries[2].texture.sampleType = WGPUTextureSampleType_Float;
            entries[2].texture.viewDimension = WGPUTextureViewDimension_2D;

            entries[3].binding = 3;
            entries[3].visibility = WGPUShaderStage_Fragment;
            entries[3].texture.sampleType = WGPUTextureSampleType_Float;
            entries[3].texture.viewDimension = WGPUTextureViewDimension_2D;

            WGPUBindGroupLayoutDescriptor bglDesc{};
            bglDesc.entryCount = 4;
            bglDesc.entries = entries;
            dc->bindGroupLayout = wgpuDeviceCreateBindGroupLayout(dc->device, &bglDesc);

            WGPUPipelineLayoutDescriptor plDesc{};
            plDesc.bindGroupLayoutCount = 1;
            plDesc.bindGroupLayouts = &dc->bindGroupLayout;
            dc->pipelineLayout = wgpuDeviceCreatePipelineLayout(dc->device, &plDesc);

            WGPUSamplerDescriptor sampDesc{};
            sampDesc.magFilter = WGPUFilterMode_Linear;
            sampDesc.minFilter = WGPUFilterMode_Linear;
            sampDesc.mipmapFilter = WGPUMipmapFilterMode_Linear;
            sampDesc.maxAnisotropy = 1;
            sampDesc.addressModeU = WGPUAddressMode_ClampToEdge;
            sampDesc.addressModeV = WGPUAddressMode_ClampToEdge;
            sampDesc.addressModeW = WGPUAddressMode_ClampToEdge;
            sampDesc.lodMaxClamp = 32.0f;
            dc->samplerClamp = wgpuDeviceCreateSampler(dc->device, &sampDesc);
            sampDesc.addressModeU = WGPUAddressMode_Repeat;
            sampDesc.addressModeV = WGPUAddressMode_Repeat;
            dc->samplerRepeat = wgpuDeviceCreateSampler(dc->device, &sampDesc);

            // 1x1 transparent placeholder so every bind group has a valid view.
            WGPUTextureDescriptor td{};
            td.dimension = WGPUTextureDimension_2D;
            td.size = {1, 1, 1};
            td.format = WGPUTextureFormat_RGBA8Unorm;
            td.mipLevelCount = 1;
            td.sampleCount = 1;
            td.usage = WGPUTextureUsage_TextureBinding | WGPUTextureUsage_CopyDst;
            dc->texMissing = wgpuDeviceCreateTexture(dc->device, &td);
            dc->texMissingView = wgpuTextureCreateView(dc->texMissing, nullptr);

            const uint32_t zero = 0;
            WGPUTexelCopyTextureInfo dst{};
            dst.texture = dc->texMissing;
            dst.mipLevel = 0;
            dst.aspect = WGPUTextureAspect_All;
            WGPUTexelCopyBufferLayout layout{};
            layout.bytesPerRow = 4;
            layout.rowsPerImage = 1;
            WGPUExtent3D extent{1, 1, 1};
            wgpuQueueWriteTexture(dc->queue, &dst, &zero, sizeof(zero), &layout, &extent);

            return 1;
        }

        int RenderCreateTexture(void* userPtr, int type, int w, int h, int imageFlags, const unsigned char* data)
        {
            DawnContext* dc = static_cast<DawnContext*>(userPtr);
            if (w <= 0 || h <= 0)
            {
                return 0;
            }

            DawnTexture tex{};
            tex.id = ++dc->textureId;
            tex.width = w;
            tex.height = h;
            tex.type = type;
            tex.flags = imageFlags;

            // NanoVG's alpha textures feed the font atlas. WebGPU has no
            // single-channel filterable format guaranteed for sampling as
            // R8Unorm with a float sampler, but R8Unorm is filterable in core
            // WebGPU, so it is used directly and the shader reads .x.
            const WGPUTextureFormat format =
                (type == NVG_TEXTURE_RGBA) ? WGPUTextureFormat_RGBA8Unorm : WGPUTextureFormat_R8Unorm;
            const uint32_t bytesPerPixel = (type == NVG_TEXTURE_RGBA) ? 4u : 1u;

            WGPUTextureDescriptor td{};
            td.dimension = WGPUTextureDimension_2D;
            td.size = {static_cast<uint32_t>(w), static_cast<uint32_t>(h), 1};
            td.format = format;
            td.mipLevelCount = 1;
            td.sampleCount = 1;
            td.usage = WGPUTextureUsage_TextureBinding | WGPUTextureUsage_CopyDst | WGPUTextureUsage_CopySrc;
            tex.texture = wgpuDeviceCreateTexture(dc->device, &td);
            if (tex.texture == nullptr)
            {
                return 0;
            }
            tex.view = wgpuTextureCreateView(tex.texture, nullptr);

            if (data != nullptr)
            {
                WGPUTexelCopyTextureInfo dst{};
                dst.texture = tex.texture;
                dst.mipLevel = 0;
                dst.aspect = WGPUTextureAspect_All;
                WGPUTexelCopyBufferLayout layout{};
                layout.bytesPerRow = static_cast<uint32_t>(w) * bytesPerPixel;
                layout.rowsPerImage = static_cast<uint32_t>(h);
                WGPUExtent3D extent{static_cast<uint32_t>(w), static_cast<uint32_t>(h), 1};
                wgpuQueueWriteTexture(dc->queue, &dst, data,
                    static_cast<size_t>(w) * static_cast<size_t>(h) * bytesPerPixel, &layout, &extent);
            }

            dc->textures.push_back(tex);
            return tex.id;
        }

        int RenderDeleteTexture(void* userPtr, int image)
        {
            DawnContext* dc = static_cast<DawnContext*>(userPtr);
            for (size_t i = 0; i < dc->textures.size(); ++i)
            {
                if (dc->textures[i].id == image)
                {
                    // Retire rather than release: draw calls already recorded
                    // for this frame still resolve their texture handles when
                    // renderFlush() builds the bind groups. RenderFlush()
                    // releases the retired entries once it has submitted.
                    dc->retiredTextures.push_back(dc->textures[i]);
                    dc->textures.erase(dc->textures.begin() + static_cast<ptrdiff_t>(i));
                    return 1;
                }
            }
            return 0;
        }

        void ReleaseRetiredTextures(DawnContext* dc)
        {
            for (auto& t : dc->retiredTextures)
            {
                if (t.view != nullptr)
                {
                    wgpuTextureViewRelease(t.view);
                }
                if (t.texture != nullptr)
                {
                    // Release, never Destroy: wgpuTextureDestroy() invalidates
                    // the resource immediately even though Dawn still holds
                    // internal references from the command buffer we just
                    // submitted, which fails validation with "Destroyed texture
                    // used in a submit". Dropping our reference lets Dawn keep
                    // it alive until the GPU is done with it.
                    wgpuTextureRelease(t.texture);
                }
            }
            dc->retiredTextures.clear();
        }

        int RenderUpdateTexture(void* userPtr, int image, int x, int y, int w, int h, const unsigned char* data)
        {
            DawnContext* dc = static_cast<DawnContext*>(userPtr);
            DawnTexture* tex = FindTexture(dc, image);
            if (tex == nullptr || data == nullptr || w <= 0 || h <= 0)
            {
                return 0;
            }

            const uint32_t bytesPerPixel = (tex->type == NVG_TEXTURE_RGBA) ? 4u : 1u;
            const uint32_t srcStride = static_cast<uint32_t>(tex->width) * bytesPerPixel;

            // NanoVG hands us a pointer to the whole atlas, so the dirty
            // sub-rectangle has to be repacked into a tightly-strided staging
            // buffer before upload.
            std::vector<uint8_t> staging(static_cast<size_t>(w) * static_cast<size_t>(h) * bytesPerPixel);
            for (int row = 0; row < h; ++row)
            {
                const uint8_t* src = data + static_cast<size_t>(y + row) * srcStride + static_cast<size_t>(x) * bytesPerPixel;
                uint8_t* dstRow = staging.data() + static_cast<size_t>(row) * static_cast<size_t>(w) * bytesPerPixel;
                std::memcpy(dstRow, src, static_cast<size_t>(w) * bytesPerPixel);
            }

            WGPUTexelCopyTextureInfo dst{};
            dst.texture = tex->texture;
            dst.mipLevel = 0;
            dst.origin = {static_cast<uint32_t>(x), static_cast<uint32_t>(y), 0};
            dst.aspect = WGPUTextureAspect_All;
            WGPUTexelCopyBufferLayout layout{};
            layout.bytesPerRow = static_cast<uint32_t>(w) * bytesPerPixel;
            layout.rowsPerImage = static_cast<uint32_t>(h);
            WGPUExtent3D extent{static_cast<uint32_t>(w), static_cast<uint32_t>(h), 1};
            wgpuQueueWriteTexture(dc->queue, &dst, staging.data(), staging.size(), &layout, &extent);
            return 1;
        }

        int RenderGetTextureSize(void* userPtr, int image, int* w, int* h)
        {
            DawnContext* dc = static_cast<DawnContext*>(userPtr);
            DawnTexture* tex = FindTexture(dc, image);
            if (tex == nullptr)
            {
                return 0;
            }
            *w = tex->width;
            *h = tex->height;
            return 1;
        }

        void RenderViewport(void* userPtr, float width, float height, float /*devicePixelRatio*/)
        {
            DawnContext* dc = static_cast<DawnContext*>(userPtr);
            dc->view[0] = width;
            dc->view[1] = height;
        }

        WGPUBlendFactor ConvertBlendFactor(int factor)
        {
            switch (factor)
            {
                case NVG_ZERO: return WGPUBlendFactor_Zero;
                case NVG_ONE: return WGPUBlendFactor_One;
                case NVG_SRC_COLOR: return WGPUBlendFactor_Src;
                case NVG_ONE_MINUS_SRC_COLOR: return WGPUBlendFactor_OneMinusSrc;
                case NVG_DST_COLOR: return WGPUBlendFactor_Dst;
                case NVG_ONE_MINUS_DST_COLOR: return WGPUBlendFactor_OneMinusDst;
                case NVG_SRC_ALPHA: return WGPUBlendFactor_SrcAlpha;
                case NVG_ONE_MINUS_SRC_ALPHA: return WGPUBlendFactor_OneMinusSrcAlpha;
                case NVG_DST_ALPHA: return WGPUBlendFactor_DstAlpha;
                case NVG_ONE_MINUS_DST_ALPHA: return WGPUBlendFactor_OneMinusDstAlpha;
                case NVG_SRC_ALPHA_SATURATE: return WGPUBlendFactor_SrcAlphaSaturated;
                default: return WGPUBlendFactor_One;
            }
        }

        DawnBlend ConvertBlend(NVGcompositeOperationState op)
        {
            DawnBlend b{};
            b.srcRGB = ConvertBlendFactor(op.srcRGB);
            b.dstRGB = ConvertBlendFactor(op.dstRGB);
            b.srcAlpha = ConvertBlendFactor(op.srcAlpha);
            b.dstAlpha = ConvertBlendFactor(op.dstAlpha);
            return b;
        }

        int AllocFragUniforms(DawnContext* dc, int n)
        {
            const int offset = static_cast<int>(dc->uniforms.size());
            dc->uniforms.resize(dc->uniforms.size() + static_cast<size_t>(n) * UNIFORM_STRIDE, 0);
            return offset;
        }

        DawnFragUniforms* FragUniformPtr(DawnContext* dc, int offset)
        {
            return reinterpret_cast<DawnFragUniforms*>(dc->uniforms.data() + offset);
        }

        int ConvertPaint(DawnContext* dc, DawnFragUniforms* frag, NVGpaint* paint,
            NVGscissor* scissor, float width, float fringe)
        {
            float invxform[6]{};
            std::memset(frag, 0, sizeof(*frag));

            const NVGcolor inner = PremulColor(paint->innerColor);
            const NVGcolor outer = PremulColor(paint->outerColor);
            std::memcpy(frag->innerCol, inner.rgba, sizeof(frag->innerCol));
            std::memcpy(frag->outerCol, outer.rgba, sizeof(frag->outerCol));

            if (scissor->extent[0] < -0.5f || scissor->extent[1] < -0.5f)
            {
                std::memset(frag->scissorMat, 0, sizeof(frag->scissorMat));
                frag->scissorExtScale[0] = 1.0f;
                frag->scissorExtScale[1] = 1.0f;
                frag->scissorExtScale[2] = 1.0f;
                frag->scissorExtScale[3] = 1.0f;
            }
            else
            {
                nvgTransformInverse(invxform, scissor->xform);
                XformToMat3x4(frag->scissorMat, invxform);
                frag->scissorExtScale[0] = scissor->extent[0];
                frag->scissorExtScale[1] = scissor->extent[1];
                frag->scissorExtScale[2] = std::sqrt(scissor->xform[0] * scissor->xform[0] + scissor->xform[2] * scissor->xform[2]) / fringe;
                frag->scissorExtScale[3] = std::sqrt(scissor->xform[1] * scissor->xform[1] + scissor->xform[3] * scissor->xform[3]) / fringe;
            }

            frag->extentRadius[0] = paint->extent[0];
            frag->extentRadius[1] = paint->extent[1];
            frag->params[1] = (width * 0.5f + fringe * 0.5f) / fringe; // strokeMult

            if (paint->image != 0)
            {
                DawnTexture* tex = FindTexture(dc, paint->image);
                if (tex == nullptr)
                {
                    return 0;
                }
                nvgTransformInverse(invxform, paint->xform);
                frag->params[3] = static_cast<float>(NSVG_SHADER_FILLIMG);
                if (tex->type == NVG_TEXTURE_RGBA)
                {
                    frag->params[2] = (tex->flags & NVG_IMAGE_PREMULTIPLIED) ? 0.0f : 1.0f;
                }
                else
                {
                    frag->params[2] = 2.0f;
                }

                if (paint->image2 != 0 && FindTexture(dc, paint->image2) != nullptr)
                {
                    frag->params[3] = static_cast<float>(NSVG_SHADER_IMG_MODULATEGRAD);
                }
            }
            else
            {
                frag->params[3] = static_cast<float>(NSVG_SHADER_FILLGRAD);
                frag->extentRadius[2] = paint->radius;
                frag->extentRadius[3] = paint->feather;
                frag->params[0] = paint->feather;
                nvgTransformInverse(invxform, paint->xform);
            }

            XformToMat3x4(frag->paintMat, invxform);
            frag->sdf[0] = paint->sdfMin;
            frag->sdf[1] = paint->sdfMax;
            frag->sdf[2] = paint->sdfBlur;

            // viewRect is stamped in RenderFlush; dc->view is not yet valid here.
            return 1;
        }

        // Appends indices expanding [start, start+count) as a triangle fan and
        // returns the index-buffer offset. WebGPU has no fan topology.
        uint32_t AppendFanIndices(DawnContext* dc, uint32_t start, uint32_t count)
        {
            const uint32_t offset = static_cast<uint32_t>(dc->indices.size());
            if (count < 3)
            {
                return offset;
            }
            const uint32_t numTris = count - 2;
            dc->indices.reserve(dc->indices.size() + static_cast<size_t>(numTris) * 3);
            for (uint32_t i = 0; i < numTris; ++i)
            {
                dc->indices.push_back(start);
                dc->indices.push_back(start + i + 1);
                dc->indices.push_back(start + i + 2);
            }
            return offset;
        }

        // The trailing nanovg_filterstack& is this fork's extension for blur
        // filters; the Dawn backend does not implement them yet.
        void RenderFill(void* userPtr, NVGpaint* paint, NVGcompositeOperationState compositeOperation,
            NVGscissor* scissor, float fringe, const float* bounds, const NVGpath* paths, int npaths,
            nanovg_filterstack&)
        {
            DawnContext* dc = static_cast<DawnContext*>(userPtr);

            DawnCall call{};
            call.type = DAWNNVG_FILL;
            call.triangleCount = 4;
            call.pathOffset = static_cast<int>(dc->paths.size());
            call.pathCount = npaths;
            call.image = paint->image;
            call.image2 = paint->image2;
            call.blendFunc = ConvertBlend(compositeOperation);

            if (npaths == 1 && paths[0].convex)
            {
                call.type = DAWNNVG_CONVEXFILL;
                call.triangleCount = 0;
            }

            for (int i = 0; i < npaths; ++i)
            {
                DawnPath copy{};
                if (paths[i].nfill > 0)
                {
                    copy.fillOffset = static_cast<int>(dc->verts.size());
                    copy.fillCount = paths[i].nfill;
                    dc->verts.insert(dc->verts.end(), paths[i].fill, paths[i].fill + paths[i].nfill);
                }
                if (paths[i].nstroke > 0)
                {
                    copy.strokeOffset = static_cast<int>(dc->verts.size());
                    copy.strokeCount = paths[i].nstroke;
                    dc->verts.insert(dc->verts.end(), paths[i].stroke, paths[i].stroke + paths[i].nstroke);
                }
                dc->paths.push_back(copy);
            }

            if (call.type == DAWNNVG_FILL)
            {
                // Quad covering the shape bounds, drawn with the stencil test to
                // resolve the winding counts written by the shape pass.
                call.triangleOffset = static_cast<int>(dc->verts.size());
                NVGvertex quad[4]{};
                quad[0] = {bounds[2], bounds[3], 0.5f, 1.0f};
                quad[1] = {bounds[2], bounds[1], 0.5f, 1.0f};
                quad[2] = {bounds[0], bounds[3], 0.5f, 1.0f};
                quad[3] = {bounds[0], bounds[1], 0.5f, 1.0f};
                dc->verts.insert(dc->verts.end(), quad, quad + 4);

                call.uniformOffset = AllocFragUniforms(dc, 2);
                // Shape pass: simple stencil-only write.
                DawnFragUniforms* stencilUniforms = FragUniformPtr(dc, call.uniformOffset);
                std::memset(stencilUniforms, 0, sizeof(*stencilUniforms));
                stencilUniforms->params[1] = 1.0f;                                     // strokeMult
                stencilUniforms->params[3] = static_cast<float>(NSVG_SHADER_SIMPLE);    // type
                // viewRect is stamped in RenderFlush; dc->view is not yet valid here.
                ConvertPaint(dc, FragUniformPtr(dc, call.uniformOffset + static_cast<int>(UNIFORM_STRIDE)),
                    paint, scissor, fringe, fringe);
            }
            else
            {
                call.uniformOffset = AllocFragUniforms(dc, 1);
                ConvertPaint(dc, FragUniformPtr(dc, call.uniformOffset), paint, scissor, fringe, fringe);
            }

            dc->calls.push_back(call);
        }

        void RenderStroke(void* userPtr, NVGpaint* paint, NVGcompositeOperationState compositeOperation,
            NVGscissor* scissor, float fringe, float strokeWidth, const NVGpath* paths, int npaths,
            nanovg_filterstack&)
        {
            DawnContext* dc = static_cast<DawnContext*>(userPtr);

            DawnCall call{};
            call.type = DAWNNVG_STROKE;
            call.pathOffset = static_cast<int>(dc->paths.size());
            call.pathCount = npaths;
            call.image = paint->image;
            call.image2 = paint->image2;
            call.blendFunc = ConvertBlend(compositeOperation);

            for (int i = 0; i < npaths; ++i)
            {
                DawnPath copy{};
                if (paths[i].nstroke > 0)
                {
                    copy.strokeOffset = static_cast<int>(dc->verts.size());
                    copy.strokeCount = paths[i].nstroke;
                    dc->verts.insert(dc->verts.end(), paths[i].stroke, paths[i].stroke + paths[i].nstroke);
                }
                dc->paths.push_back(copy);
            }

            call.uniformOffset = AllocFragUniforms(dc, 1);
            ConvertPaint(dc, FragUniformPtr(dc, call.uniformOffset), paint, scissor, strokeWidth, fringe);

            dc->calls.push_back(call);
        }

        void RenderTriangles(void* userPtr, NVGpaint* paint, NVGcompositeOperationState compositeOperation,
            NVGscissor* scissor, const NVGvertex* verts, int nverts, nanovg_filterstack&)
        {
            DawnContext* dc = static_cast<DawnContext*>(userPtr);
            if (nverts <= 0)
            {
                return;
            }

            DawnCall call{};
            call.type = DAWNNVG_TRIANGLES;
            call.image = paint->image;
            call.image2 = paint->image2;
            call.blendFunc = ConvertBlend(compositeOperation);
            call.triangleOffset = static_cast<int>(dc->verts.size());
            call.triangleCount = nverts;
            dc->verts.insert(dc->verts.end(), verts, verts + nverts);

            call.uniformOffset = AllocFragUniforms(dc, 1);
            DawnFragUniforms* frag = FragUniformPtr(dc, call.uniformOffset);
            ConvertPaint(dc, frag, paint, scissor, 1.0f, 1.0f);
            frag->params[3] = static_cast<float>(NSVG_SHADER_IMG); // textured tris

            dc->calls.push_back(call);
        }

        void SetBindGroup(DawnContext* dc, const DawnCall& call, int uniformOffset, int image, int image2)
        {
            WGPUBindGroup bg = MakeBindGroup(dc, image, image2);
            const uint32_t dynamicOffset = static_cast<uint32_t>(uniformOffset);
            wgpuRenderPassEncoderSetBindGroup(dc->pass, 0, bg, 1, &dynamicOffset);
            (void)call;
        }

        void DrawFill(DawnContext* dc, const DawnCall& call)
        {
            const DawnPath* paths = dc->paths.data() + call.pathOffset;

            // Pass 1: write winding numbers into stencil, no color.
            PipelineKey key{};
            key.topology = WGPUPrimitiveTopology_TriangleList;
            key.stencil = StencilMode::FillShapes;
            key.colorWrite = false;
            key.srcRGB = key.srcAlpha = WGPUBlendFactor_One;
            key.dstRGB = key.dstAlpha = WGPUBlendFactor_Zero;
            wgpuRenderPassEncoderSetPipeline(dc->pass, GetPipeline(dc, key));
            SetBindGroup(dc, call, call.uniformOffset, 0, 0);

            for (int i = 0; i < call.pathCount; ++i)
            {
                if (paths[i].fillCount > 2)
                {
                    const uint32_t numIndices = (static_cast<uint32_t>(paths[i].fillCount) - 2) * 3;
                    wgpuRenderPassEncoderDrawIndexed(dc->pass, numIndices, 1, paths[i].fanIndexOffset, 0, 0);
                }
            }

            // Pass 2: anti-aliased fringes, tested against the winding stencil.
            const int paintUniform = call.uniformOffset + static_cast<int>(UNIFORM_STRIDE);
            if (dc->edgeAntiAlias != 0)
            {
                PipelineKey fringeKey{};
                fringeKey.topology = WGPUPrimitiveTopology_TriangleStrip;
                fringeKey.stencil = StencilMode::FillFringe;
                fringeKey.colorWrite = true;
                fringeKey.srcRGB = call.blendFunc.srcRGB;
                fringeKey.dstRGB = call.blendFunc.dstRGB;
                fringeKey.srcAlpha = call.blendFunc.srcAlpha;
                fringeKey.dstAlpha = call.blendFunc.dstAlpha;
                wgpuRenderPassEncoderSetPipeline(dc->pass, GetPipeline(dc, fringeKey));
                wgpuRenderPassEncoderSetStencilReference(dc->pass, 0);
                SetBindGroup(dc, call, paintUniform, call.image, call.image2);

                for (int i = 0; i < call.pathCount; ++i)
                {
                    if (paths[i].strokeCount > 0)
                    {
                        wgpuRenderPassEncoderDraw(dc->pass, static_cast<uint32_t>(paths[i].strokeCount), 1,
                            static_cast<uint32_t>(paths[i].strokeOffset), 0);
                    }
                }
            }

            // Pass 3: fill the covered area and reset the stencil to zero.
            PipelineKey endKey{};
            endKey.topology = WGPUPrimitiveTopology_TriangleStrip;
            endKey.stencil = StencilMode::FillEnd;
            endKey.colorWrite = true;
            endKey.srcRGB = call.blendFunc.srcRGB;
            endKey.dstRGB = call.blendFunc.dstRGB;
            endKey.srcAlpha = call.blendFunc.srcAlpha;
            endKey.dstAlpha = call.blendFunc.dstAlpha;
            wgpuRenderPassEncoderSetPipeline(dc->pass, GetPipeline(dc, endKey));
            wgpuRenderPassEncoderSetStencilReference(dc->pass, 0);
            SetBindGroup(dc, call, paintUniform, call.image, call.image2);
            wgpuRenderPassEncoderDraw(dc->pass, static_cast<uint32_t>(call.triangleCount), 1,
                static_cast<uint32_t>(call.triangleOffset), 0);
        }

        void DrawConvexFill(DawnContext* dc, const DawnCall& call)
        {
            const DawnPath* paths = dc->paths.data() + call.pathOffset;

            PipelineKey key{};
            key.topology = WGPUPrimitiveTopology_TriangleList;
            key.stencil = StencilMode::None;
            key.colorWrite = true;
            key.srcRGB = call.blendFunc.srcRGB;
            key.dstRGB = call.blendFunc.dstRGB;
            key.srcAlpha = call.blendFunc.srcAlpha;
            key.dstAlpha = call.blendFunc.dstAlpha;
            wgpuRenderPassEncoderSetPipeline(dc->pass, GetPipeline(dc, key));
            SetBindGroup(dc, call, call.uniformOffset, call.image, call.image2);

            for (int i = 0; i < call.pathCount; ++i)
            {
                if (paths[i].fillCount > 2)
                {
                    const uint32_t numIndices = (static_cast<uint32_t>(paths[i].fillCount) - 2) * 3;
                    wgpuRenderPassEncoderDrawIndexed(dc->pass, numIndices, 1, paths[i].fanIndexOffset, 0, 0);
                }
            }

            if (dc->edgeAntiAlias != 0)
            {
                PipelineKey stripKey = key;
                stripKey.topology = WGPUPrimitiveTopology_TriangleStrip;
                wgpuRenderPassEncoderSetPipeline(dc->pass, GetPipeline(dc, stripKey));
                SetBindGroup(dc, call, call.uniformOffset, call.image, call.image2);
                for (int i = 0; i < call.pathCount; ++i)
                {
                    if (paths[i].strokeCount > 0)
                    {
                        wgpuRenderPassEncoderDraw(dc->pass, static_cast<uint32_t>(paths[i].strokeCount), 1,
                            static_cast<uint32_t>(paths[i].strokeOffset), 0);
                    }
                }
            }
        }

        void DrawStroke(DawnContext* dc, const DawnCall& call)
        {
            const DawnPath* paths = dc->paths.data() + call.pathOffset;

            PipelineKey key{};
            key.topology = WGPUPrimitiveTopology_TriangleStrip;
            key.stencil = StencilMode::None;
            key.colorWrite = true;
            key.srcRGB = call.blendFunc.srcRGB;
            key.dstRGB = call.blendFunc.dstRGB;
            key.srcAlpha = call.blendFunc.srcAlpha;
            key.dstAlpha = call.blendFunc.dstAlpha;
            wgpuRenderPassEncoderSetPipeline(dc->pass, GetPipeline(dc, key));
            SetBindGroup(dc, call, call.uniformOffset, call.image, call.image2);

            for (int i = 0; i < call.pathCount; ++i)
            {
                if (paths[i].strokeCount > 0)
                {
                    wgpuRenderPassEncoderDraw(dc->pass, static_cast<uint32_t>(paths[i].strokeCount), 1,
                        static_cast<uint32_t>(paths[i].strokeOffset), 0);
                }
            }
        }

        void DrawTriangles(DawnContext* dc, const DawnCall& call)
        {
            PipelineKey key{};
            key.topology = WGPUPrimitiveTopology_TriangleList;
            key.stencil = StencilMode::None;
            key.colorWrite = true;
            key.srcRGB = call.blendFunc.srcRGB;
            key.dstRGB = call.blendFunc.dstRGB;
            key.srcAlpha = call.blendFunc.srcAlpha;
            key.dstAlpha = call.blendFunc.dstAlpha;
            wgpuRenderPassEncoderSetPipeline(dc->pass, GetPipeline(dc, key));
            SetBindGroup(dc, call, call.uniformOffset, call.image, call.image2);
            wgpuRenderPassEncoderDraw(dc->pass, static_cast<uint32_t>(call.triangleCount), 1,
                static_cast<uint32_t>(call.triangleOffset), 0);
        }

        void ReleaseFrameBindGroups(DawnContext* dc)
        {
            for (WGPUBindGroup bg : dc->frameBindGroups)
            {
                wgpuBindGroupRelease(bg);
            }
            dc->frameBindGroups.clear();
        }

        void RenderFlush(void* userPtr)
        {
            DawnContext* dc = static_cast<DawnContext*>(userPtr);

            if (dc->calls.empty() || dc->pass == nullptr)
            {
                dc->calls.clear();
                dc->paths.clear();
                dc->verts.clear();
                dc->uniforms.clear();
                dc->indices.clear();
                ReleaseRetiredTextures(dc);
                return;
            }

            // The fan indices are generated while recording draws, but the index
            // buffer has to be bound (and therefore sized and filled) up front.
            // Build it once here and record each path's offset, so the recording
            // pass below can simply read back fanIndexOffset.
            dc->indices.clear();
            for (const DawnCall& call : dc->calls)
            {
                if (call.type != DAWNNVG_FILL && call.type != DAWNNVG_CONVEXFILL)
                {
                    continue;
                }
                DawnPath* paths = dc->paths.data() + call.pathOffset;
                for (int i = 0; i < call.pathCount; ++i)
                {
                    if (paths[i].fillCount > 2)
                    {
                        paths[i].fanIndexOffset = AppendFanIndices(dc,
                            static_cast<uint32_t>(paths[i].fillOffset),
                            static_cast<uint32_t>(paths[i].fillCount));
                    }
                }
            }

            const uint64_t vertexBytes = dc->verts.size() * sizeof(NVGvertex);
            const uint64_t indexBytes = dc->indices.size() * sizeof(uint32_t);
            const uint64_t uniformBytes = dc->uniforms.size();

            // Stamp the viewport into every uniform record now, rather than when
            // each call was built. nanovg only reports the viewport from
            // nvgBeginFrame, which the canvas calls at flush time (immediately
            // before nvgEndFrame) -- so at the moment RenderFill/RenderStroke ran,
            // dc->view was still whatever the previous frame left behind, and on
            // the very first frame it was 0x0, which collapses every vertex to a
            // degenerate position and renders nothing. The GL backend has the same
            // split and handles it the same way: viewSize is a flush-time global,
            // not part of the per-draw record.
            for (size_t offset = 0; offset + sizeof(DawnFragUniforms) <= dc->uniforms.size(); offset += UNIFORM_STRIDE)
            {
                auto* frag = reinterpret_cast<DawnFragUniforms*>(dc->uniforms.data() + offset);
                frag->viewRect[0] = dc->view[0];
                frag->viewRect[1] = dc->view[1];
            }

            EnsureBuffer(dc, dc->vertexBuffer, dc->vertexCapacity, vertexBytes, WGPUBufferUsage_Vertex);
            EnsureBuffer(dc, dc->uniformBuffer, dc->uniformCapacity, uniformBytes, WGPUBufferUsage_Uniform);
            if (indexBytes > 0)
            {
                EnsureBuffer(dc, dc->indexBuffer, dc->indexCapacity, indexBytes, WGPUBufferUsage_Index);
            }

            if (vertexBytes > 0)
            {
                wgpuQueueWriteBuffer(dc->queue, dc->vertexBuffer, 0, dc->verts.data(), static_cast<size_t>(vertexBytes));
            }
            if (indexBytes > 0)
            {
                wgpuQueueWriteBuffer(dc->queue, dc->indexBuffer, 0, dc->indices.data(), static_cast<size_t>(indexBytes));
            }
            if (uniformBytes > 0)
            {
                wgpuQueueWriteBuffer(dc->queue, dc->uniformBuffer, 0, dc->uniforms.data(), static_cast<size_t>(uniformBytes));
            }

            if (vertexBytes > 0)
            {
                wgpuRenderPassEncoderSetVertexBuffer(dc->pass, 0, dc->vertexBuffer, 0, vertexBytes);
            }
            if (indexBytes > 0)
            {
                wgpuRenderPassEncoderSetIndexBuffer(dc->pass, dc->indexBuffer, WGPUIndexFormat_Uint32, 0, indexBytes);
            }

            for (const DawnCall& call : dc->calls)
            {
                switch (call.type)
                {
                    case DAWNNVG_FILL: DrawFill(dc, call); break;
                    case DAWNNVG_CONVEXFILL: DrawConvexFill(dc, call); break;
                    case DAWNNVG_STROKE: DrawStroke(dc, call); break;
                    case DAWNNVG_TRIANGLES: DrawTriangles(dc, call); break;
                    default: break;
                }
            }

            dc->calls.clear();
            dc->paths.clear();
            dc->verts.clear();
            dc->uniforms.clear();
            dc->indices.clear();
            ReleaseRetiredTextures(dc);
        }

        void RenderDelete(void* userPtr)
        {
            DawnContext* dc = static_cast<DawnContext*>(userPtr);
            if (dc == nullptr)
            {
                return;
            }

            ReleaseFrameBindGroups(dc);

            for (auto& kv : dc->pipelines)
            {
                wgpuRenderPipelineRelease(kv.second);
            }
            dc->pipelines.clear();

            ReleaseRetiredTextures(dc);
            for (auto& t : dc->textures)
            {
                if (t.view != nullptr) wgpuTextureViewRelease(t.view);
                // Release without Destroy: in-flight command buffers may still
                // reference these, and Dawn tracks that through the refcount.
                if (t.texture != nullptr) { wgpuTextureRelease(t.texture); }
            }
            dc->textures.clear();

            if (dc->texMissingView != nullptr) wgpuTextureViewRelease(dc->texMissingView);
            if (dc->texMissing != nullptr) { wgpuTextureRelease(dc->texMissing); }
            if (dc->samplerClamp != nullptr) wgpuSamplerRelease(dc->samplerClamp);
            if (dc->samplerRepeat != nullptr) wgpuSamplerRelease(dc->samplerRepeat);
            if (dc->vertexBuffer != nullptr) { wgpuBufferDestroy(dc->vertexBuffer); wgpuBufferRelease(dc->vertexBuffer); }
            if (dc->indexBuffer != nullptr) { wgpuBufferDestroy(dc->indexBuffer); wgpuBufferRelease(dc->indexBuffer); }
            if (dc->uniformBuffer != nullptr) { wgpuBufferDestroy(dc->uniformBuffer); wgpuBufferRelease(dc->uniformBuffer); }
            if (dc->pipelineLayout != nullptr) wgpuPipelineLayoutRelease(dc->pipelineLayout);
            if (dc->bindGroupLayout != nullptr) wgpuBindGroupLayoutRelease(dc->bindGroupLayout);
            if (dc->shader != nullptr) wgpuShaderModuleRelease(dc->shader);

            delete dc;
        }
    }

    NVGcontext* NvgCreateDawn(WGPUDevice device, WGPUTextureFormat colorFormat, int32_t edgeAntiAlias)
    {
        if (device == nullptr)
        {
            return nullptr;
        }

        DawnContext* dc = new DawnContext();
        dc->device = device;
        dc->queue = wgpuDeviceGetQueue(device);
        dc->colorFormat = colorFormat;
        dc->edgeAntiAlias = edgeAntiAlias;

        NVGparams params{};
        params.renderCreate = RenderCreate;
        params.renderCreateTexture = RenderCreateTexture;
        params.renderDeleteTexture = RenderDeleteTexture;
        params.renderUpdateTexture = RenderUpdateTexture;
        params.renderGetTextureSize = RenderGetTextureSize;
        params.renderViewport = RenderViewport;
        params.renderFlush = RenderFlush;
        params.renderFill = RenderFill;
        params.renderStroke = RenderStroke;
        params.renderTriangles = RenderTriangles;
        params.renderDelete = RenderDelete;
        params.userPtr = dc;
        params.edgeAntiAlias = edgeAntiAlias;

        NVGcontext* ctx = nvgCreateInternal(&params);
        if (ctx == nullptr)
        {
            delete dc;
            return nullptr;
        }
        return ctx;
    }

    void NvgDeleteDawn(NVGcontext* ctx)
    {
        if (ctx != nullptr)
        {
            // nvgDeleteInternal invokes renderDelete, which frees the DawnContext.
            nvgDeleteInternal(ctx);
        }
    }

    void NvgSetDawnRenderPass(NVGcontext* ctx, WGPURenderPassEncoder pass)
    {
        if (ctx == nullptr)
        {
            return;
        }
        DawnContext* dc = static_cast<DawnContext*>(nvgInternalParams(ctx)->userPtr);
        // Bind groups reference the uniform buffer, which may be reallocated
        // between passes; drop the previous pass' groups when rebinding.
        ReleaseFrameBindGroups(dc);
        dc->pass = pass;
    }

    WGPUTextureFormat NvgDawnStencilFormat()
    {
        return STENCIL_FORMAT;
    }
}
