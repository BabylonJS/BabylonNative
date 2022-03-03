#include "NativeEngine.h"
#include "ShaderCompiler.h"
#include "Texture.h"
#include "JsConsoleLogger.h"

#include <arcana/threading/task.h>
#include <arcana/threading/task_schedulers.h>
#include <arcana/macros.h>
#include <arcana/tracing/trace_region.h>

#include <napi/env.h>
#include <napi/napi_pointer.h>

#include <bgfx/bgfx.h>

#include <bimg/bimg.h>
#include <bimg/decode.h>
#include <bimg/encode.h>

// STB_IMAGE_RESIZE_IMPLEMENTATION will define implementation in stb_image_resize.h. Many .cpp can include it 
// but only one can define implementation or linking errors will popup.
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include <stb/stb_image_resize.h>
#include <bx/math.h>

namespace Babylon
{
    namespace
    {
        namespace TextureSampling
        {
            constexpr uint32_t SAMPLER_MAG_POINT = BGFX_SAMPLER_MAG_POINT;
            constexpr uint32_t SAMPLER_MAG_LINEAR = 0;
            constexpr uint32_t SAMPLER_MIN_POINT = BGFX_SAMPLER_MIN_POINT;
            constexpr uint32_t SAMPLER_MIN_LINEAR = 0;
            constexpr uint32_t SAMPLER_MIP_POINT = BGFX_SAMPLER_MIP_POINT;
            constexpr uint32_t SAMPLER_MIP_LINEAR = 0;
            constexpr uint32_t SAMPLER_MIP_IGNORE = BGFX_SAMPLER_MIP_POINT; // HACK: bgfx has no support for ignoring mips

            // clang-format off
            // Names, as in constants.ts are MAG_MIN(_MIP?)     MAG                     MIN                         MIP
            constexpr uint32_t NEAREST_NEAREST =                SAMPLER_MAG_POINT   |   SAMPLER_MIN_POINT     |     SAMPLER_MIP_IGNORE    ;
            constexpr uint32_t LINEAR_LINEAR =                  SAMPLER_MAG_LINEAR  |   SAMPLER_MIN_LINEAR    |     SAMPLER_MIP_IGNORE    ;
            constexpr uint32_t LINEAR_LINEAR_MIPLINEAR =        SAMPLER_MAG_LINEAR  |   SAMPLER_MIN_LINEAR    |     SAMPLER_MIP_LINEAR    ;
            constexpr uint32_t NEAREST_NEAREST_MIPNEAREST =     SAMPLER_MAG_POINT   |   SAMPLER_MIN_POINT     |     SAMPLER_MIP_POINT     ;
            constexpr uint32_t NEAREST_LINEAR_MIPNEAREST =      SAMPLER_MAG_POINT   |   SAMPLER_MIN_LINEAR    |     SAMPLER_MIP_POINT     ;
            constexpr uint32_t NEAREST_LINEAR_MIPLINEAR =       SAMPLER_MAG_POINT   |   SAMPLER_MIN_LINEAR    |     SAMPLER_MIP_LINEAR    ;
            constexpr uint32_t NEAREST_LINEAR =                 SAMPLER_MAG_POINT   |   SAMPLER_MIN_LINEAR    |     SAMPLER_MIP_IGNORE    ;
            constexpr uint32_t NEAREST_NEAREST_MIPLINEAR =      SAMPLER_MAG_POINT   |   SAMPLER_MIN_POINT     |     SAMPLER_MIP_LINEAR    ;
            constexpr uint32_t LINEAR_NEAREST_MIPNEAREST =      SAMPLER_MAG_LINEAR  |   SAMPLER_MIN_POINT     |     SAMPLER_MIP_POINT     ;
            constexpr uint32_t LINEAR_NEAREST_MIPLINEAR =       SAMPLER_MAG_LINEAR  |   SAMPLER_MIN_POINT     |     SAMPLER_MIP_LINEAR    ;
            constexpr uint32_t LINEAR_LINEAR_MIPNEAREST =       SAMPLER_MAG_LINEAR  |   SAMPLER_MIN_LINEAR    |     SAMPLER_MIP_POINT     ;
            constexpr uint32_t LINEAR_NEAREST =                 SAMPLER_MAG_LINEAR  |   SAMPLER_MIN_POINT     |     SAMPLER_MIP_IGNORE    ;
            // clang-format on
        }

        namespace AlphaMode
        {
            constexpr uint64_t DISABLE = 0x0;
            constexpr uint64_t ADD = BGFX_STATE_BLEND_FUNC_SEPARATE(BGFX_STATE_BLEND_SRC_ALPHA, BGFX_STATE_BLEND_ONE, BGFX_STATE_BLEND_ZERO, BGFX_STATE_BLEND_ONE);
            constexpr uint64_t COMBINE = BGFX_STATE_BLEND_FUNC_SEPARATE(BGFX_STATE_BLEND_SRC_ALPHA, BGFX_STATE_BLEND_INV_SRC_ALPHA, BGFX_STATE_BLEND_ONE, BGFX_STATE_BLEND_ONE);
            constexpr uint64_t SUBTRACT = BGFX_STATE_BLEND_FUNC_SEPARATE(BGFX_STATE_BLEND_ZERO, BGFX_STATE_BLEND_INV_SRC_COLOR, BGFX_STATE_BLEND_ONE, BGFX_STATE_BLEND_ONE);
            constexpr uint64_t MULTIPLY = BGFX_STATE_BLEND_FUNC_SEPARATE(BGFX_STATE_BLEND_DST_COLOR, BGFX_STATE_BLEND_ZERO, BGFX_STATE_BLEND_ONE, BGFX_STATE_BLEND_ONE);
            constexpr uint64_t MAXIMIZED = BGFX_STATE_BLEND_FUNC_SEPARATE(BGFX_STATE_BLEND_SRC_ALPHA, BGFX_STATE_BLEND_INV_SRC_COLOR, BGFX_STATE_BLEND_ONE, BGFX_STATE_BLEND_ONE);
            constexpr uint64_t ONEONE = BGFX_STATE_BLEND_FUNC_SEPARATE(BGFX_STATE_BLEND_ONE, BGFX_STATE_BLEND_ONE, BGFX_STATE_BLEND_ZERO, BGFX_STATE_BLEND_ONE);
            constexpr uint64_t PREMULTIPLIED = BGFX_STATE_BLEND_FUNC_SEPARATE(BGFX_STATE_BLEND_ONE, BGFX_STATE_BLEND_INV_SRC_ALPHA, BGFX_STATE_BLEND_ONE, BGFX_STATE_BLEND_ONE);
            constexpr uint64_t PREMULTIPLIED_PORTERDUFF = BGFX_STATE_BLEND_FUNC_SEPARATE(BGFX_STATE_BLEND_ONE, BGFX_STATE_BLEND_INV_SRC_ALPHA, BGFX_STATE_BLEND_ONE, BGFX_STATE_BLEND_INV_SRC_ALPHA);
            constexpr uint64_t INTERPOLATE = BGFX_STATE_BLEND_FUNC_SEPARATE(BGFX_STATE_BLEND_FACTOR, BGFX_STATE_BLEND_INV_FACTOR, BGFX_STATE_BLEND_FACTOR, BGFX_STATE_BLEND_INV_FACTOR);
            constexpr uint64_t SCREENMODE = BGFX_STATE_BLEND_FUNC_SEPARATE(BGFX_STATE_BLEND_ONE, BGFX_STATE_BLEND_INV_SRC_COLOR, BGFX_STATE_BLEND_ONE, BGFX_STATE_BLEND_INV_SRC_ALPHA);
        }

        static_assert(static_cast<bgfx::TextureFormat::Enum>(bimg::TextureFormat::Count) == bgfx::TextureFormat::Count);
        static_assert(static_cast<bgfx::TextureFormat::Enum>(bimg::TextureFormat::RGBA8) == bgfx::TextureFormat::RGBA8);
        static_assert(static_cast<bgfx::TextureFormat::Enum>(bimg::TextureFormat::RGB8) == bgfx::TextureFormat::RGB8);

        bgfx::TextureFormat::Enum Cast(bimg::TextureFormat::Enum format)
        {
            return static_cast<bgfx::TextureFormat::Enum>(format);
        }

        using TransformFn = void (*)(const uint8_t*, uint8_t*);
        void TransformImage(const bimg::ImageContainer* src, bimg::ImageContainer* dst, TransformFn transformFn)
        {
            const uint8_t* srcData{static_cast<const uint8_t*>(src->m_data)};
            const uint8_t srcBytesPerPixel{static_cast<uint8_t>(bimg::getBitsPerPixel(src->m_format) >> 3)};
            const uint8_t* srcDataEnd{srcData + src->m_size};

            uint8_t* dstData{static_cast<uint8_t*>(dst->m_data)};
            const uint8_t dstBytesPerPixel{static_cast<uint8_t>(bimg::getBitsPerPixel(dst->m_format) >> 3)};
            const uint8_t* dstDataEnd{dstData + dst->m_size};

            while (srcData < srcDataEnd && dstData < dstDataEnd)
            {
                transformFn(srcData, dstData);
                srcData += srcBytesPerPixel;
                dstData += dstBytesPerPixel;
            }
        }

        bimg::ImageContainer* ParseImage(bx::AllocatorI& allocator, gsl::span<uint8_t> data)
        {
            bimg::ImageContainer* image{bimg::imageParse(&allocator, data.data(), static_cast<uint32_t>(data.size()))};
            if (image == nullptr)
            {
                throw std::runtime_error{"Failed to parse image."};
            }

            if (image->m_format == bimg::TextureFormat::R8 ||
                image->m_format == bimg::TextureFormat::RG8)
            {
                static const TransformFn UnpackR8{[](const uint8_t* src, uint8_t* dst) { dst[0] = dst[1] = dst[2] = src[0]; dst[3] = 1; }};
                static const TransformFn UnpackRG8{[](const uint8_t* src, uint8_t* dst) { dst[0] = dst[1] = dst[2] = src[0]; dst[3] = src[1]; }};

                // bimg loads grayscale textures with and without alpha as R8 and RG8 respectively.
                // Unpack to RGB and RGBA such that RGB is the grayscale and the A is the alpha.
                bimg::ImageContainer* oldImage{image};
                image = bimg::imageAlloc(&allocator, bimg::TextureFormat::RGBA8, static_cast<uint16_t>(image->m_width), static_cast<uint16_t>(image->m_height), 1, 1, false, false);
                TransformImage(oldImage, image, image->m_format == bimg::TextureFormat::R8 ? UnpackR8 : UnpackRG8);
                bimg::imageFree(oldImage);
            }

            return image;
        }

        bimg::ImageContainer* PrepareImage(bx::AllocatorI& allocator, bimg::ImageContainer* image, bool invertY, bool srgb, bool generateMips)
        {
            assert(
                image->m_format == bimg::TextureFormat::RGB8 ||
                image->m_format == bimg::TextureFormat::RGBA8 ||
                image->m_format == bimg::TextureFormat::RGBA16 ||
                image->m_format == bimg::TextureFormat::RGBA32F);

            assert(image->m_depth == 1);
            assert(image->m_numLayers == 1);
            assert(image->m_numMips == 1);
            assert(image->m_cubeMap == false);

            if (bgfx::getCaps()->originBottomLeft ? invertY : !invertY)
            {
                uint8_t* bytes{static_cast<uint8_t*>(image->m_data)};
                const uint32_t rowCount{image->m_height};
                const uint32_t rowPitch{image->m_size / image->m_height};

                std::vector<uint8_t> buffer(rowPitch);
                for (size_t row = 0; row < rowCount / 2; row++)
                {
                    uint8_t* frontPtr{bytes + (row * rowPitch)};
                    uint8_t* backPtr{bytes + ((rowCount - row - 1) * rowPitch)};

                    std::memcpy(buffer.data(), frontPtr, rowPitch);
                    std::memcpy(frontPtr, backPtr, rowPitch);
                    std::memcpy(backPtr, buffer.data(), rowPitch);
                }
            }

            if (srgb && !bgfx::isTextureValid(1, false, 1, Cast(image->m_format), BGFX_TEXTURE_SRGB))
            {
                bimg::ImageContainer* oldImage{image};
                image = bimg::imageConvert(&allocator, bimg::TextureFormat::RGBA8, *image, false);
                bimg::imageFree(oldImage);

                assert(bgfx::isTextureValid(1, false, 1, Cast(image->m_format), BGFX_TEXTURE_SRGB));
            }

            if (generateMips)
            {
                if (image->m_format == bimg::TextureFormat::RGB8)
                {
                    bimg::ImageContainer* oldImage{image};
                    image = bimg::imageConvert(&allocator, bimg::TextureFormat::RGBA8, *image, false);
                    bimg::imageFree(oldImage);
                }
                else if (image->m_format == bimg::TextureFormat::RGBA16)
                {
                    bimg::ImageContainer* oldImage{image};
                    image = bimg::imageConvert(&allocator, bimg::TextureFormat::RGBA32F, *image, false);
                    bimg::imageFree(oldImage);
                }

                bimg::ImageContainer* oldImage{image};
                image = bimg::imageGenerateMips(&allocator, *image);
                bimg::imageFree(oldImage);
            }

            assert(image != nullptr);
            return image;
        }

        void LoadTextureFromImage(TextureData* texture, bimg::ImageContainer* image, bool srgb)
        {
            if (bgfx::isValid(texture->Handle))
            {
                if (texture->Width != image->m_width || texture->Height != image->m_height)
                {
                    throw std::runtime_error{"Cannot update texture from image of different size"};
                }
            }
            else
            {
                uint64_t flags{(srgb ? BGFX_TEXTURE_SRGB : BGFX_TEXTURE_NONE) | BGFX_SAMPLER_NONE};
                texture->Handle = bgfx::createTexture2D(static_cast<uint16_t>(image->m_width), static_cast<uint16_t>(image->m_height), (image->m_numMips > 1), 1, Cast(image->m_format), flags);
                texture->Width = image->m_width;
                texture->Height = image->m_height;
            }

            for (uint8_t mip = 0; mip < image->m_numMips; ++mip)
            {
                bimg::ImageMip imageMip{};
                if (bimg::imageGetRawData(*image, 0, mip, image->m_data, image->m_size, imageMip))
                {
                    bgfx::ReleaseFn releaseFn{};
                    if (mip == image->m_numMips - 1)
                    {
                        releaseFn = [](void*, void* userData) {
                            bimg::imageFree(static_cast<bimg::ImageContainer*>(userData));
                        };
                    }

                    const bgfx::Memory* mem{bgfx::makeRef(imageMip.m_data, imageMip.m_size, releaseFn, image)};
                    bgfx::updateTexture2D(texture->Handle, 0, mip, 0, 0, static_cast<uint16_t>(imageMip.m_width), static_cast<uint16_t>(imageMip.m_height), mem);
                }
            }
        }

        void LoadCubeTextureFromImages(TextureData* texture, std::vector<bimg::ImageContainer*>& images, bool srgb)
        {
            const bimg::ImageContainer* firstImage{images.front()};
            assert(firstImage->m_width == firstImage->m_height);
            uint32_t size{firstImage->m_width};

            if (bgfx::isValid(texture->Handle))
            {
                if (texture->Width != size || texture->Height != size)
                {
                    throw std::runtime_error{"Cannot update texture from image of different size"};
                }
            }
            else
            {
                bool hasMips{images.size() > 6 || firstImage->m_numMips > 1};
                bgfx::TextureFormat::Enum format{Cast(firstImage->m_format)};
                uint64_t flags{(srgb ? BGFX_TEXTURE_SRGB : BGFX_TEXTURE_NONE) | BGFX_SAMPLER_NONE};
                texture->Handle = bgfx::createTextureCube(static_cast<uint16_t>(size), hasMips, 1, format, flags);
                texture->Width = size;
                texture->Height = size;
            }

            if (images.size() > 6)
            {
                assert(images.size() % 6 == 0);
                const uint8_t numMips{static_cast<uint8_t>(images.size() / 6)};
                for (uint8_t side = 0; side < 6; ++side)
                {
                    for (uint8_t mip = 0; mip < numMips; ++mip)
                    {
                        bimg::ImageContainer* image{images[(side * numMips) + mip]};

                        bgfx::ReleaseFn releaseFn{[](void*, void* userData) {
                            bimg::imageFree(static_cast<bimg::ImageContainer*>(userData));
                        }};

                        const bgfx::Memory* mem{bgfx::makeRef(image->m_data, image->m_size, releaseFn, image)};
                        bgfx::updateTextureCube(texture->Handle, 0, side, mip, 0, 0, static_cast<uint16_t>(image->m_width), static_cast<uint16_t>(image->m_height), mem);
                    }
                }
            }
            else
            {
                for (uint8_t side = 0; side < 6; ++side)
                {
                    bimg::ImageContainer* image{images[side]};
                    for (uint8_t mip = 0; mip < image->m_numMips; ++mip)
                    {
                        bimg::ImageMip imageMip{};
                        if (bimg::imageGetRawData(*image, 0, mip, image->m_data, image->m_size, imageMip))
                        {
                            bgfx::ReleaseFn releaseFn{};
                            if (mip == image->m_numMips - 1)
                            {
                                releaseFn = [](void*, void* userData) {
                                    bimg::imageFree(static_cast<bimg::ImageContainer*>(userData));
                                };
                            }

                            const bgfx::Memory* mem{bgfx::makeRef(imageMip.m_data, imageMip.m_size, releaseFn, image)};
                            bgfx::updateTextureCube(texture->Handle, 0, side, mip, 0, 0, static_cast<uint16_t>(imageMip.m_width), static_cast<uint16_t>(imageMip.m_height), mem);
                        }
                    }
                }
            }
        }

        void CreateBlitTexture(TextureData* texture)
        {
            if (texture->CreationFlags & BGFX_TEXTURE_BLIT_DST)
            {
                return;
            }
            bgfx::destroy(texture->Handle);
            texture->Handle = bgfx::createTexture2D((uint16_t)texture->Width, (uint16_t)texture->Height, false, 1, bgfx::TextureFormat::RGBA8, BGFX_TEXTURE_BLIT_DST);
            texture->CreationFlags |= BGFX_TEXTURE_BLIT_DST;
        }

        using CommandFunctionPointerT = void(NativeEngine::*)(NativeDataStream::Reader&);
    }

    void NativeEngine::Initialize(Napi::Env env)
    {
        // Initialize the JavaScript side.
        Napi::HandleScope scope{env};

        // clang-format off
        Napi::Function func = DefineClass(
            env,
            JS_CLASS_NAME,
            {
                // This must match the version in nativeEngine.ts
                StaticValue("PROTOCOL_VERSION", Napi::Number::From(env, 4)),

                StaticValue("TEXTURE_NEAREST_NEAREST", Napi::Number::From(env, TextureSampling::NEAREST_NEAREST)),
                StaticValue("TEXTURE_LINEAR_LINEAR", Napi::Number::From(env, TextureSampling::LINEAR_LINEAR)),
                StaticValue("TEXTURE_LINEAR_LINEAR_MIPLINEAR", Napi::Number::From(env, TextureSampling::LINEAR_LINEAR_MIPLINEAR)),
                StaticValue("TEXTURE_NEAREST_NEAREST_MIPNEAREST", Napi::Number::From(env, TextureSampling::NEAREST_NEAREST_MIPNEAREST)),
                StaticValue("TEXTURE_NEAREST_LINEAR_MIPNEAREST", Napi::Number::From(env, TextureSampling::NEAREST_LINEAR_MIPNEAREST)),
                StaticValue("TEXTURE_NEAREST_LINEAR_MIPLINEAR", Napi::Number::From(env, TextureSampling::NEAREST_LINEAR_MIPLINEAR)),
                StaticValue("TEXTURE_NEAREST_LINEAR", Napi::Number::From(env, TextureSampling::NEAREST_LINEAR)),
                StaticValue("TEXTURE_NEAREST_NEAREST_MIPLINEAR", Napi::Number::From(env, TextureSampling::NEAREST_NEAREST_MIPLINEAR)),
                StaticValue("TEXTURE_LINEAR_NEAREST_MIPNEAREST", Napi::Number::From(env, TextureSampling::LINEAR_NEAREST_MIPNEAREST)),
                StaticValue("TEXTURE_LINEAR_NEAREST_MIPLINEAR", Napi::Number::From(env, TextureSampling::LINEAR_NEAREST_MIPLINEAR)),
                StaticValue("TEXTURE_LINEAR_LINEAR_MIPNEAREST", Napi::Number::From(env, TextureSampling::LINEAR_LINEAR_MIPNEAREST)),
                StaticValue("TEXTURE_LINEAR_NEAREST", Napi::Number::From(env, TextureSampling::LINEAR_NEAREST)),

                StaticValue("DEPTH_TEST_LESS", Napi::Number::From(env, BGFX_STATE_DEPTH_TEST_LESS)),
                StaticValue("DEPTH_TEST_LEQUAL", Napi::Number::From(env, BGFX_STATE_DEPTH_TEST_LEQUAL)),
                StaticValue("DEPTH_TEST_EQUAL", Napi::Number::From(env, BGFX_STATE_DEPTH_TEST_EQUAL)),
                StaticValue("DEPTH_TEST_GEQUAL", Napi::Number::From(env, BGFX_STATE_DEPTH_TEST_GEQUAL)),
                StaticValue("DEPTH_TEST_GREATER", Napi::Number::From(env, BGFX_STATE_DEPTH_TEST_GREATER)),
                StaticValue("DEPTH_TEST_NOTEQUAL", Napi::Number::From(env, BGFX_STATE_DEPTH_TEST_NOTEQUAL)),
                StaticValue("DEPTH_TEST_NEVER", Napi::Number::From(env, BGFX_STATE_DEPTH_TEST_NEVER)),
                StaticValue("DEPTH_TEST_ALWAYS", Napi::Number::From(env, BGFX_STATE_DEPTH_TEST_ALWAYS)),

                StaticValue("CLEAR_FLAG_COLOR", Napi::Number::From(env, BGFX_CLEAR_COLOR)),
                StaticValue("CLEAR_FLAG_DEPTH", Napi::Number::From(env, BGFX_CLEAR_DEPTH)),
                StaticValue("CLEAR_FLAG_STENCIL", Napi::Number::From(env, BGFX_CLEAR_STENCIL)),

                StaticValue("ADDRESS_MODE_WRAP", Napi::Number::From(env, 0)),
                StaticValue("ADDRESS_MODE_MIRROR", Napi::Number::From(env, BGFX_SAMPLER_U_MIRROR)),
                StaticValue("ADDRESS_MODE_CLAMP", Napi::Number::From(env, BGFX_SAMPLER_U_CLAMP)),
                StaticValue("ADDRESS_MODE_BORDER", Napi::Number::From(env, BGFX_SAMPLER_U_BORDER)),
                StaticValue("ADDRESS_MODE_MIRROR_ONCE", Napi::Number::From(env, BGFX_SAMPLER_U_MIRROR)),

                StaticValue("TEXTURE_FORMAT_RGB8", Napi::Number::From(env, static_cast<uint32_t>(bgfx::TextureFormat::RGB8))),
                StaticValue("TEXTURE_FORMAT_RGBA8", Napi::Number::From(env, static_cast<uint32_t>(bgfx::TextureFormat::RGBA8))),
                StaticValue("TEXTURE_FORMAT_RGBA32F", Napi::Number::From(env, static_cast<uint32_t>(bgfx::TextureFormat::RGBA32F))),

                StaticValue("ATTRIB_TYPE_INT8", Napi::Number::From(env, static_cast<uint32_t>(bgfx::AttribType::Int8))),
                StaticValue("ATTRIB_TYPE_UINT8", Napi::Number::From(env, static_cast<uint32_t>(bgfx::AttribType::Uint8))),
                StaticValue("ATTRIB_TYPE_INT16", Napi::Number::From(env, static_cast<uint32_t>(bgfx::AttribType::Int16))),
                StaticValue("ATTRIB_TYPE_UINT16", Napi::Number::From(env, static_cast<uint32_t>(bgfx::AttribType::Uint16))),
                StaticValue("ATTRIB_TYPE_FLOAT", Napi::Number::From(env, static_cast<uint32_t>(bgfx::AttribType::Float))),

                StaticValue("ALPHA_DISABLE", Napi::Number::From(env, AlphaMode::DISABLE)),
                StaticValue("ALPHA_ADD", Napi::Number::From(env, AlphaMode::ADD)),
                StaticValue("ALPHA_COMBINE", Napi::Number::From(env, AlphaMode::COMBINE)),
                StaticValue("ALPHA_SUBTRACT", Napi::Number::From(env, AlphaMode::SUBTRACT)),
                StaticValue("ALPHA_MULTIPLY", Napi::Number::From(env, AlphaMode::MULTIPLY)),
                StaticValue("ALPHA_MAXIMIZED", Napi::Number::From(env, AlphaMode::MAXIMIZED)),
                StaticValue("ALPHA_ONEONE", Napi::Number::From(env, AlphaMode::ONEONE)),
                StaticValue("ALPHA_PREMULTIPLIED", Napi::Number::From(env, AlphaMode::PREMULTIPLIED)),
                StaticValue("ALPHA_PREMULTIPLIED_PORTERDUFF", Napi::Number::From(env, AlphaMode::PREMULTIPLIED_PORTERDUFF)),
                StaticValue("ALPHA_INTERPOLATE", Napi::Number::From(env, AlphaMode::INTERPOLATE)),
                StaticValue("ALPHA_SCREENMODE", Napi::Number::From(env, AlphaMode::SCREENMODE)),

                StaticValue("STENCIL_TEST_LESS", Napi::Number::From(env, BGFX_STENCIL_TEST_LESS)),
                StaticValue("STENCIL_TEST_LEQUAL", Napi::Number::From(env, BGFX_STENCIL_TEST_LEQUAL)),
                StaticValue("STENCIL_TEST_EQUAL", Napi::Number::From(env, BGFX_STENCIL_TEST_EQUAL)),
                StaticValue("STENCIL_TEST_GEQUAL", Napi::Number::From(env, BGFX_STENCIL_TEST_GEQUAL)),
                StaticValue("STENCIL_TEST_GREATER", Napi::Number::From(env, BGFX_STENCIL_TEST_GREATER)),
                StaticValue("STENCIL_TEST_NOTEQUAL", Napi::Number::From(env, BGFX_STENCIL_TEST_NOTEQUAL)),
                StaticValue("STENCIL_TEST_NEVER", Napi::Number::From(env, BGFX_STENCIL_TEST_NEVER)),
                StaticValue("STENCIL_TEST_ALWAYS", Napi::Number::From(env, BGFX_STENCIL_TEST_ALWAYS)),

                StaticValue("STENCIL_OP_FAIL_S_ZERO", Napi::Number::From(env, BGFX_STENCIL_OP_FAIL_S_ZERO)),
                StaticValue("STENCIL_OP_FAIL_S_KEEP", Napi::Number::From(env, BGFX_STENCIL_OP_FAIL_S_KEEP)),
                StaticValue("STENCIL_OP_FAIL_S_REPLACE", Napi::Number::From(env, BGFX_STENCIL_OP_FAIL_S_REPLACE)),
                StaticValue("STENCIL_OP_FAIL_S_INCR", Napi::Number::From(env, BGFX_STENCIL_OP_FAIL_S_INCR)),
                StaticValue("STENCIL_OP_FAIL_S_INCRSAT", Napi::Number::From(env, BGFX_STENCIL_OP_FAIL_S_INCRSAT)),
                StaticValue("STENCIL_OP_FAIL_S_DECR", Napi::Number::From(env, BGFX_STENCIL_OP_FAIL_S_DECR)),
                StaticValue("STENCIL_OP_FAIL_S_DECRSAT", Napi::Number::From(env, BGFX_STENCIL_OP_FAIL_S_DECRSAT)),
                StaticValue("STENCIL_OP_FAIL_S_INVERT", Napi::Number::From(env, BGFX_STENCIL_OP_FAIL_S_INVERT)),

                StaticValue("STENCIL_OP_FAIL_Z_ZERO", Napi::Number::From(env, BGFX_STENCIL_OP_FAIL_Z_ZERO)),
                StaticValue("STENCIL_OP_FAIL_Z_KEEP", Napi::Number::From(env, BGFX_STENCIL_OP_FAIL_Z_KEEP)),
                StaticValue("STENCIL_OP_FAIL_Z_REPLACE", Napi::Number::From(env, BGFX_STENCIL_OP_FAIL_Z_REPLACE)),
                StaticValue("STENCIL_OP_FAIL_Z_INCR", Napi::Number::From(env, BGFX_STENCIL_OP_FAIL_Z_INCR)),
                StaticValue("STENCIL_OP_FAIL_Z_INCRSAT", Napi::Number::From(env, BGFX_STENCIL_OP_FAIL_Z_INCRSAT)),
                StaticValue("STENCIL_OP_FAIL_Z_DECR", Napi::Number::From(env, BGFX_STENCIL_OP_FAIL_Z_DECR)),
                StaticValue("STENCIL_OP_FAIL_Z_DECRSAT", Napi::Number::From(env, BGFX_STENCIL_OP_FAIL_Z_DECRSAT)),
                StaticValue("STENCIL_OP_FAIL_Z_INVERT", Napi::Number::From(env, BGFX_STENCIL_OP_FAIL_Z_INVERT)),

                StaticValue("STENCIL_OP_PASS_Z_ZERO", Napi::Number::From(env, BGFX_STENCIL_OP_PASS_Z_ZERO)),
                StaticValue("STENCIL_OP_PASS_Z_KEEP", Napi::Number::From(env, BGFX_STENCIL_OP_PASS_Z_KEEP)),
                StaticValue("STENCIL_OP_PASS_Z_REPLACE", Napi::Number::From(env, BGFX_STENCIL_OP_PASS_Z_REPLACE)),
                StaticValue("STENCIL_OP_PASS_Z_INCR", Napi::Number::From(env, BGFX_STENCIL_OP_PASS_Z_INCR)),
                StaticValue("STENCIL_OP_PASS_Z_INCRSAT", Napi::Number::From(env, BGFX_STENCIL_OP_PASS_Z_INCRSAT)),
                StaticValue("STENCIL_OP_PASS_Z_DECR", Napi::Number::From(env, BGFX_STENCIL_OP_PASS_Z_DECR)),
                StaticValue("STENCIL_OP_PASS_Z_DECRSAT", Napi::Number::From(env, BGFX_STENCIL_OP_PASS_Z_DECRSAT)),
                StaticValue("STENCIL_OP_PASS_Z_INVERT", Napi::Number::From(env, BGFX_STENCIL_OP_PASS_Z_INVERT)),

                StaticValue("COMMAND_DELETEVERTEXARRAY", Napi::FunctionPointer::Create(env, &NativeEngine::DeleteVertexArray)),
                StaticValue("COMMAND_DELETEINDEXBUFFER", Napi::FunctionPointer::Create(env, &NativeEngine::DeleteIndexBuffer)),
                StaticValue("COMMAND_DELETEVERTEXBUFFER", Napi::FunctionPointer::Create(env, &NativeEngine::DeleteVertexBuffer)),
                StaticValue("COMMAND_SETPROGRAM", Napi::FunctionPointer::Create(env, &NativeEngine::SetProgram)),
                StaticValue("COMMAND_DELETEPROGRAM", Napi::FunctionPointer::Create(env, &NativeEngine::DeleteProgram)),
                StaticValue("COMMAND_SETMATRICES", Napi::FunctionPointer::Create(env, &NativeEngine::SetMatrices)),
                StaticValue("COMMAND_SETMATRIX", Napi::FunctionPointer::Create(env, &NativeEngine::SetMatrix)),
                StaticValue("COMMAND_SETMATRIX3X3", Napi::FunctionPointer::Create(env, &NativeEngine::SetMatrix3x3)),
                StaticValue("COMMAND_SETMATRIX2X2", Napi::FunctionPointer::Create(env, &NativeEngine::SetMatrix2x2)),
                StaticValue("COMMAND_SETINT", Napi::FunctionPointer::Create(env, &NativeEngine::SetInt)),
                StaticValue("COMMAND_SETINTARRAY", Napi::FunctionPointer::Create(env, &NativeEngine::SetIntArray)),
                StaticValue("COMMAND_SETINTARRAY2", Napi::FunctionPointer::Create(env, &NativeEngine::SetIntArray2)),
                StaticValue("COMMAND_SETINTARRAY3", Napi::FunctionPointer::Create(env, &NativeEngine::SetIntArray3)),
                StaticValue("COMMAND_SETINTARRAY4", Napi::FunctionPointer::Create(env, &NativeEngine::SetIntArray4)),
                StaticValue("COMMAND_SETFLOATARRAY", Napi::FunctionPointer::Create(env, &NativeEngine::SetFloatArray)),
                StaticValue("COMMAND_SETFLOATARRAY2", Napi::FunctionPointer::Create(env, &NativeEngine::SetFloatArray2)),
                StaticValue("COMMAND_SETFLOATARRAY3", Napi::FunctionPointer::Create(env, &NativeEngine::SetFloatArray3)),
                StaticValue("COMMAND_SETFLOATARRAY4", Napi::FunctionPointer::Create(env, &NativeEngine::SetFloatArray4)),
                StaticValue("COMMAND_SETTEXTURESAMPLING", Napi::FunctionPointer::Create(env, &NativeEngine::SetTextureSampling)),
                StaticValue("COMMAND_SETTEXTUREWRAPMODE", Napi::FunctionPointer::Create(env, &NativeEngine::SetTextureWrapMode)),
                StaticValue("COMMAND_SETTEXTUREANISOTROPICLEVEL", Napi::FunctionPointer::Create(env, &NativeEngine::SetTextureAnisotropicLevel)),
                StaticValue("COMMAND_SETTEXTURE", Napi::FunctionPointer::Create(env, &NativeEngine::SetTexture)),
                StaticValue("COMMAND_BINDVERTEXARRAY", Napi::FunctionPointer::Create(env, &NativeEngine::BindVertexArray)),
                StaticValue("COMMAND_SETSTATE", Napi::FunctionPointer::Create(env, &NativeEngine::SetState)),
                StaticValue("COMMAND_SETZOFFSET", Napi::FunctionPointer::Create(env, &NativeEngine::SetZOffset)),
                StaticValue("COMMAND_SETZOFFSETUNITS", Napi::FunctionPointer::Create(env, &NativeEngine::SetZOffsetUnits)),
                StaticValue("COMMAND_SETDEPTHTEST", Napi::FunctionPointer::Create(env, &NativeEngine::SetDepthTest)),
                StaticValue("COMMAND_SETDEPTHWRITE", Napi::FunctionPointer::Create(env, &NativeEngine::SetDepthWrite)),
                StaticValue("COMMAND_SETCOLORWRITE", Napi::FunctionPointer::Create(env, &NativeEngine::SetColorWrite)),
                StaticValue("COMMAND_SETBLENDMODE", Napi::FunctionPointer::Create(env, &NativeEngine::SetBlendMode)),
                StaticValue("COMMAND_SETFLOAT", Napi::FunctionPointer::Create(env, &NativeEngine::SetFloat)),
                StaticValue("COMMAND_SETFLOAT2", Napi::FunctionPointer::Create(env, &NativeEngine::SetFloat2)),
                StaticValue("COMMAND_SETFLOAT3", Napi::FunctionPointer::Create(env, &NativeEngine::SetFloat3)),
                StaticValue("COMMAND_SETFLOAT4", Napi::FunctionPointer::Create(env, &NativeEngine::SetFloat4)),
                StaticValue("COMMAND_BINDFRAMEBUFFER", Napi::FunctionPointer::Create(env, &NativeEngine::BindFrameBuffer)),
                StaticValue("COMMAND_UNBINDFRAMEBUFFER", Napi::FunctionPointer::Create(env, &NativeEngine::UnbindFrameBuffer)),
                StaticValue("COMMAND_DELETEFRAMEBUFFER", Napi::FunctionPointer::Create(env, &NativeEngine::DeleteFrameBuffer)),
                StaticValue("COMMAND_DRAWINDEXED", Napi::FunctionPointer::Create(env, &NativeEngine::DrawIndexed)),
                StaticValue("COMMAND_DRAW", Napi::FunctionPointer::Create(env, &NativeEngine::Draw)),
                StaticValue("COMMAND_CLEAR", Napi::FunctionPointer::Create(env, &NativeEngine::Clear)),
                StaticValue("COMMAND_SETSTENCIL", Napi::FunctionPointer::Create(env, &NativeEngine::SetStencil)),

                InstanceMethod("dispose", &NativeEngine::Dispose),

                InstanceMethod("requestAnimationFrame", &NativeEngine::RequestAnimationFrame),

                InstanceMethod("createVertexArray", &NativeEngine::CreateVertexArray),

                InstanceMethod("createIndexBuffer", &NativeEngine::CreateIndexBuffer),
                InstanceMethod("recordIndexBuffer", &NativeEngine::RecordIndexBuffer),
                InstanceMethod("updateDynamicIndexBuffer", &NativeEngine::UpdateDynamicIndexBuffer),

                InstanceMethod("createVertexBuffer", &NativeEngine::CreateVertexBuffer),
                InstanceMethod("recordVertexBuffer", &NativeEngine::RecordVertexBuffer),
                InstanceMethod("updateDynamicVertexBuffer", &NativeEngine::UpdateDynamicVertexBuffer),

                InstanceMethod("createProgram", &NativeEngine::CreateProgram),
                InstanceMethod("getUniforms", &NativeEngine::GetUniforms),
                InstanceMethod("getAttributes", &NativeEngine::GetAttributes),

                InstanceMethod("createTexture", &NativeEngine::CreateTexture),
                InstanceMethod("loadTexture", &NativeEngine::LoadTexture),
                InstanceMethod("loadRawTexture", &NativeEngine::LoadRawTexture),
                InstanceMethod("loadRawTexture2DArray", &NativeEngine::LoadRawTexture2DArray),
                InstanceMethod("loadCubeTexture", &NativeEngine::LoadCubeTexture),
                InstanceMethod("loadCubeTextureWithMips", &NativeEngine::LoadCubeTextureWithMips),
                InstanceMethod("getTextureWidth", &NativeEngine::GetTextureWidth),
                InstanceMethod("getTextureHeight", &NativeEngine::GetTextureHeight),
                InstanceMethod("copyTexture", &NativeEngine::CopyTexture),
                InstanceMethod("deleteTexture", &NativeEngine::DeleteTexture),

                InstanceMethod("createImageBitmap", &NativeEngine::CreateImageBitmap),
                InstanceMethod("resizeImageBitmap", &NativeEngine::ResizeImageBitmap),

                InstanceMethod("createFrameBuffer", &NativeEngine::CreateFrameBuffer),

                InstanceMethod("getRenderWidth", &NativeEngine::GetRenderWidth),
                InstanceMethod("getRenderHeight", &NativeEngine::GetRenderHeight),
                InstanceMethod("getHardwareScalingLevel", &NativeEngine::GetHardwareScalingLevel),
                InstanceMethod("setHardwareScalingLevel", &NativeEngine::SetHardwareScalingLevel),

                InstanceMethod("setViewPort", &NativeEngine::SetViewPort),

                InstanceMethod("setCommandDataStream", &NativeEngine::SetCommandDataStream),
                InstanceMethod("submitCommands", &NativeEngine::SubmitCommands),

                // REVIEW: Should this be here if only used by ValidationTest?
                InstanceMethod("getFrameBufferData", &NativeEngine::GetFrameBufferData),
            });
        // clang-format on

        JsRuntime::NativeObject::GetFromJavaScript(env).Set(JS_CONSTRUCTOR_NAME, func);
    }

    NativeEngine::NativeEngine(const Napi::CallbackInfo& info)
        : NativeEngine(info, JsRuntime::GetFromJavaScript(info.Env()))
    {
    }

    NativeEngine::NativeEngine(const Napi::CallbackInfo& info, JsRuntime& runtime)
        : Napi::ObjectWrap<NativeEngine>{info}
        , m_cancellationSource{std::make_shared<arcana::cancellation_source>()}
        , m_runtime{runtime}
        , m_graphicsImpl{GraphicsImpl::GetFromJavaScript(info.Env())}
        , m_update{m_graphicsImpl.GetUpdate("update")}
        , m_runtimeScheduler{runtime}
        , m_defaultFrameBuffer{m_graphicsImpl, BGFX_INVALID_HANDLE, 0, 0, true, true, true}
        , m_boundFrameBuffer{&m_defaultFrameBuffer}
        , m_boundFrameBufferNeedsRebinding{m_graphicsImpl, *m_cancellationSource, true}
    {
    }

    NativeEngine::~NativeEngine()
    {
        Dispose();
    }

    void NativeEngine::Dispose()
    {
        m_cancellationSource->cancel();
    }

    void NativeEngine::Dispose(const Napi::CallbackInfo& /*info*/)
    {
        Dispose();
    }

    void NativeEngine::RequestAnimationFrame(const Napi::CallbackInfo& info)
    {
        auto callback{info[0].As<Napi::Function>()};

        m_requestAnimationFrameCallbacks.emplace_back(Napi::Persistent(callback));
        ScheduleRequestAnimationFrameCallbacks();
    }

    Napi::Value NativeEngine::CreateVertexArray(const Napi::CallbackInfo& info)
    {
        VertexArray* vertexArray = new VertexArray{};
        return Napi::Pointer<VertexArray>::Create(info.Env(), vertexArray, Napi::NapiPointerDeleter(vertexArray));
    }

    void NativeEngine::DeleteVertexArray(NativeDataStream::Reader& data)
    {
        data.ReadPointer<VertexArray>()->Dispose();
        // TODO: should we clear the m_boundVertexArray if it gets deleted?
        //assert(vertexArray != m_boundVertexArray);
    }

    void NativeEngine::BindVertexArray(NativeDataStream::Reader& data)
    {
        m_boundVertexArray = data.ReadPointer<VertexArray>();
    }

    Napi::Value NativeEngine::CreateIndexBuffer(const Napi::CallbackInfo& info)
    {
        const Napi::ArrayBuffer bytes = info[0].As<Napi::ArrayBuffer>();
        const uint32_t byteOffset = info[1].As<Napi::Number>().Uint32Value();
        const uint32_t byteLength = info[2].As<Napi::Number>().Uint32Value();
        const bool is32Bits = info[3].As<Napi::Boolean>().Value();
        const bool dynamic = info[4].As<Napi::Boolean>().Value();

        const uint16_t flags = (is32Bits ? BGFX_BUFFER_INDEX32 : 0);
        IndexBuffer* indexBuffer = new IndexBuffer{gsl::make_span(static_cast<uint8_t*>(bytes.Data()) + byteOffset, byteLength), flags, dynamic};
        return Napi::Pointer<IndexBuffer>::Create(info.Env(), indexBuffer, Napi::NapiPointerDeleter(indexBuffer));
    }

    void NativeEngine::DeleteIndexBuffer(NativeDataStream::Reader& data)
    {
        data.ReadPointer<IndexBuffer>()->Dispose();
    }

    void NativeEngine::RecordIndexBuffer(const Napi::CallbackInfo& info)
    {
        VertexArray* vertexArray = info[0].As<Napi::Pointer<VertexArray>>().Get();
        IndexBuffer* indexBuffer = info[1].As<Napi::Pointer<IndexBuffer>>().Get();

        if (!vertexArray->RecordIndexBuffer(indexBuffer))
        {
            JsConsoleLogger::LogWarn(info.Env(), "WARNING: Fail to create index buffer. Number of index buffers higher than max count.");
        }
    }

    void NativeEngine::UpdateDynamicIndexBuffer(const Napi::CallbackInfo& info)
    {
        IndexBuffer* indexBuffer = info[0].As<Napi::Pointer<IndexBuffer>>().Get();
        const Napi::ArrayBuffer bytes = info[1].As<Napi::ArrayBuffer>();
        const uint32_t byteOffset = info[2].As<Napi::Number>().Uint32Value();
        const uint32_t byteLength = info[3].As<Napi::Number>().Uint32Value();
        const uint32_t startingIndex = info[4].As<Napi::Number>().Uint32Value();

        indexBuffer->Update(info.Env(), gsl::make_span(static_cast<uint8_t*>(bytes.Data()) + byteOffset, byteLength), startingIndex);
    }

    Napi::Value NativeEngine::CreateVertexBuffer(const Napi::CallbackInfo& info)
    {
        const Napi::ArrayBuffer bytes = info[0].As<Napi::ArrayBuffer>();
        const uint32_t byteOffset = info[1].As<Napi::Number>().Uint32Value();
        const uint32_t byteLength = info[2].As<Napi::Number>().Uint32Value();
        const bool dynamic = info[3].As<Napi::Boolean>().Value();

        VertexBuffer* vertexBuffer = new VertexBuffer(gsl::make_span(static_cast<uint8_t*>(bytes.Data()) + byteOffset, byteLength), dynamic);
        return Napi::Pointer<VertexBuffer>::Create(info.Env(), vertexBuffer, Napi::NapiPointerDeleter(vertexBuffer));
    }

    void NativeEngine::DeleteVertexBuffer(NativeDataStream::Reader& data)
    {
        data.ReadPointer<VertexBuffer>()->Dispose();
    }

    void NativeEngine::RecordVertexBuffer(const Napi::CallbackInfo& info)
    {
        VertexArray* vertexArray = info[0].As<Napi::Pointer<VertexArray>>().Get();
        VertexBuffer* vertexBuffer = info[1].As<Napi::Pointer<VertexBuffer>>().Get();
        const uint32_t location = info[2].As<Napi::Number>().Uint32Value();
        const uint32_t byteOffset = info[3].As<Napi::Number>().Uint32Value();
        const uint32_t byteStride = info[4].As<Napi::Number>().Uint32Value();
        const uint32_t numElements = info[5].As<Napi::Number>().Uint32Value();
        const uint32_t type = info[6].As<Napi::Number>().Uint32Value();
        const bool normalized = info[7].As<Napi::Boolean>().Value();
        const uint32_t divisor = info[8].As<Napi::Number>().Uint32Value();

        if (!vertexArray->RecordVertexBuffer(vertexBuffer, location, byteOffset, byteStride, numElements, type, normalized, divisor))
        {
            JsConsoleLogger::LogWarn(info.Env(), "WARNING: Fail to create vertex buffer. Number of vertex buffers higher than max count or too many instanced streams.");
        }
    }

    void NativeEngine::UpdateDynamicVertexBuffer(const Napi::CallbackInfo& info)
    {
        VertexBuffer* vertexBuffer = info[0].As<Napi::Pointer<VertexBuffer>>().Get();
        const Napi::ArrayBuffer bytes = info[1].As<Napi::ArrayBuffer>();
        const uint32_t byteOffset = info[2].As<Napi::Number>().Uint32Value();
        const uint32_t byteLength = info[3].As<Napi::Number>().Uint32Value();

        vertexBuffer->Update(info.Env(), gsl::make_span(static_cast<uint8_t*>(bytes.Data()) + byteOffset, byteLength));
    }

    // Change VS output coordinate system
    std::string NativeEngine::ProcessShaderCoordinates(const std::string& vertexSource)
    {
        const auto* caps = bgfx::getCaps();
        // patching shader code to append clip space coordinates for the current rendering API
        // Can be done with glslang shader traversal. Done with string patching for now.
        if (!caps->homogeneousDepth)
        {
            std::string patchedVertexSource;
            const auto lastClosingCurly = vertexSource.find_last_of('}');
            patchedVertexSource = vertexSource.substr(0, lastClosingCurly);

            patchedVertexSource += "gl_Position.z = (gl_Position.z + gl_Position.w) / 2.0; }";
            return patchedVertexSource;
        }
        return vertexSource;
    }

    std::string ProcessSamplerFlip(const std::string& vertexSource)
    {
        const auto* caps = bgfx::getCaps();
        // for d3d, vulkan, metal, flip the texture sampling on vertical axis
        if (!caps->originBottomLeft)
        {
            std::string patchedVertexSource = vertexSource;

            static const std::string shaderNameDefineStr = "#define SHADER_NAME";
            const auto shaderNameDefine = vertexSource.find(shaderNameDefineStr);
            if (shaderNameDefine != std::string::npos)
            {
                static const auto textureSamplerFunctions = R"(
                    highp vec2 flip(highp vec2 uv)
                    {
                        return vec2(uv.x, 1. - uv.y);
                    }
                    highp vec3 flip(highp vec3 uv)
                    {
                        return uv;
                    }
                    #define texture(x,y) texture(x, flip(y))
                    #define SHADER_NAME)";

                patchedVertexSource.replace(shaderNameDefine, shaderNameDefineStr.length(), textureSamplerFunctions);
            }
            return patchedVertexSource;
        }
        return vertexSource;
    }

    Napi::Value NativeEngine::CreateProgram(const Napi::CallbackInfo& info)
    {
        const std::string vertexSource = info[0].As<Napi::String>().Utf8Value();
        const std::string fragmentSource = info[1].As<Napi::String>().Utf8Value();

        ProgramData* program = new ProgramData{};
        ShaderCompiler::BgfxShaderInfo shaderInfo{};

        try
        {
            shaderInfo = m_shaderCompiler.Compile(ProcessShaderCoordinates(vertexSource), ProcessSamplerFlip(fragmentSource));
        }
        catch (const std::exception& ex)
        {
            throw Napi::Error::New(info.Env(), ex.what());
        }

        static auto InitUniformInfos{
            [](bgfx::ShaderHandle shader, const std::unordered_map<std::string, uint8_t>& uniformStages, std::unordered_map<uint16_t, UniformInfo>& uniformInfos, std::unordered_map<std::string, uint16_t>& uniformNameToIndex)
            {
                auto numUniforms = bgfx::getShaderUniforms(shader);
                std::vector<bgfx::UniformHandle> uniforms{numUniforms};
                bgfx::getShaderUniforms(shader, uniforms.data(), gsl::narrow_cast<uint16_t>(uniforms.size()));

                for (uint8_t index = 0; index < numUniforms; index++)
                {
                    bgfx::UniformInfo info{};
                    uint16_t handleIndex = uniforms[index].idx;
                    bgfx::getUniformInfo(uniforms[index], info);
                    auto itStage = uniformStages.find(info.name);
                    auto& handle = uniforms[index];
                    uniformInfos.emplace(std::make_pair(handle.idx, UniformInfo{itStage == uniformStages.end() ? uint8_t{} : itStage->second, handle, info.num}));
                    uniformNameToIndex[info.name] = handleIndex;
                }
            }};

        auto vertexShader = bgfx::createShader(bgfx::copy(shaderInfo.VertexBytes.data(), static_cast<uint32_t>(shaderInfo.VertexBytes.size())));
        InitUniformInfos(vertexShader, shaderInfo.UniformStages, program->UniformInfos, program->UniformNameToIndex);
        program->VertexAttributeLocations = std::move(shaderInfo.VertexAttributeLocations);

        auto fragmentShader = bgfx::createShader(bgfx::copy(shaderInfo.FragmentBytes.data(), static_cast<uint32_t>(shaderInfo.FragmentBytes.size())));
        InitUniformInfos(fragmentShader, shaderInfo.UniformStages, program->UniformInfos, program->UniformNameToIndex);

        program->Handle = bgfx::createProgram(vertexShader, fragmentShader, true);
        return Napi::Pointer<ProgramData>::Create(info.Env(), program, Napi::NapiPointerDeleter(program));
    }

    Napi::Value NativeEngine::GetUniforms(const Napi::CallbackInfo& info)
    {
        const ProgramData* program = info[0].As<Napi::Pointer<ProgramData>>().Get();
        const Napi::Array names = info[1].As<Napi::Array>();

        const auto length{names.Length()};
        auto uniforms{Napi::Array::New(info.Env(), length)};
        for (uint32_t index = 0; index < length; ++index)
        {
            if (names[index].IsString())
            {
                const auto name{names[index].As<Napi::String>().Utf8Value()};

                const auto itUniformIndex = program->UniformNameToIndex.find(name);

                if (itUniformIndex != program->UniformNameToIndex.end())
                {
                    const auto itUniformInfo{program->UniformInfos.find(itUniformIndex->second)};

                    if (itUniformInfo != program->UniformInfos.end())
                    {
                        uniforms[index] = Napi::Pointer<UniformInfo>::Create(info.Env(), &itUniformInfo->second);
                        continue;
                    }
                }
            }

            uniforms[index] = info.Env().Null();
        }

        return std::move(uniforms);
    }

    Napi::Value NativeEngine::GetAttributes(const Napi::CallbackInfo& info)
    {
        const ProgramData* program = info[0].As<Napi::Pointer<ProgramData>>().Get();
        const Napi::Array names = info[1].As<Napi::Array>();

        const auto& attributeLocations = program->VertexAttributeLocations;

        auto length = names.Length();
        auto attributes = Napi::Array::New(info.Env(), length);
        for (uint32_t index = 0; index < length; ++index)
        {
            const std::string name = names[index].As<Napi::String>().Utf8Value();
            const auto it = attributeLocations.find(name);
            int location = (it == attributeLocations.end() ? -1 : gsl::narrow_cast<int>(it->second));
            attributes[index] = Napi::Value::From(info.Env(), location);
        }

        return std::move(attributes);
    }

    void NativeEngine::SetProgram(NativeDataStream::Reader& data)
    {
        m_currentProgram = data.ReadPointer<ProgramData>();
    }

    void NativeEngine::SetState(NativeDataStream::Reader& data)
    {
        const bool culling = data.ReadUint32();
        // TODO: zOffset
        /*const float zOffset =*/ data.ReadFloat32();
        /*const float zOffsetUnits =*/ data.ReadFloat32();
        const bool cullBackFaces = data.ReadUint32();
        const bool reverseSide = data.ReadUint32();

        m_engineState &= ~(BGFX_STATE_CULL_MASK | BGFX_STATE_FRONT_CCW);
        m_engineState |= reverseSide ? 0 : BGFX_STATE_FRONT_CCW;

        if (culling)
        {
            m_engineState |= cullBackFaces ? BGFX_STATE_CULL_CCW : BGFX_STATE_CULL_CW;
        }
    }

    void NativeEngine::DeleteProgram(NativeDataStream::Reader& data)
    {
        data.ReadPointer<ProgramData>()->Dispose();
    }

    void NativeEngine::SetZOffset(NativeDataStream::Reader& data)
    {
        /*const auto zOffset =*/ data.ReadFloat32();

        // STUB: Stub.
    }

    void NativeEngine::SetZOffsetUnits(NativeDataStream::Reader& data)
    {
        /*const auto zOffsetUnits =*/ data.ReadFloat32();

        // STUB: Stub.
    }

    void NativeEngine::SetDepthTest(NativeDataStream::Reader& data)
    {
        const auto depthTest = data.ReadUint32();

        m_engineState &= ~BGFX_STATE_DEPTH_TEST_MASK;
        m_engineState |= depthTest;
    }

    void NativeEngine::SetDepthWrite(NativeDataStream::Reader& data)
    {
        const auto enable = static_cast<bool>(data.ReadUint32());

        m_engineState &= ~BGFX_STATE_WRITE_Z;
        m_engineState |= enable ? BGFX_STATE_WRITE_Z : 0;
    }

    void NativeEngine::SetColorWrite(NativeDataStream::Reader& data)
    {
        const auto enable = static_cast<bool>(data.ReadUint32());

        m_engineState &= ~(BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A);
        m_engineState |= enable ? (BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A) : 0;
    }

    void NativeEngine::SetBlendMode(NativeDataStream::Reader& data)
    {
        const auto blendMode = data.ReadUint32();

        m_engineState &= ~BGFX_STATE_BLEND_MASK;
        m_engineState |= blendMode;
    }
    
    void NativeEngine::SetInt(NativeDataStream::Reader& data)
    {
        const auto& uniformInfo{*data.ReadPointer<UniformInfo>()};
        const auto value{static_cast<float>(data.ReadInt32())};
        m_currentProgram->SetUniform(uniformInfo.Handle, gsl::make_span(&value, 1));
    }

    template<int size, typename arrayType>
    void NativeEngine::SetTypeArrayN(const UniformInfo& uniformInfo, const uint32_t elementLength, const arrayType& array)
    {
        m_scratch.clear();
        for (uint32_t index = 0; index < elementLength; index += size)
        {
            const float values[] = {
                static_cast<float>(array[index]),
                (size > 1) ? static_cast<float>(array[index + 1]) : 0.f,
                (size > 2) ? static_cast<float>(array[index + 2]) : 0.f,
                (size > 3) ? static_cast<float>(array[index + 3]) : 0.f,
            };
            m_scratch.insert(m_scratch.end(), values, values + 4);
        }

        m_currentProgram->SetUniform(uniformInfo.Handle, m_scratch, elementLength / size);
    }

    template<int size>
    void NativeEngine::SetFloatN(NativeDataStream::Reader& data)
    {
        const auto& uniformInfo = *data.ReadPointer<UniformInfo>();
        const float values[] = {
            data.ReadFloat32(),
            (size > 1) ? data.ReadFloat32() : 0.f,
            (size > 2) ? data.ReadFloat32() : 0.f,
            (size > 3) ? data.ReadFloat32() : 0.f,
        };

        m_currentProgram->SetUniform(uniformInfo.Handle, values);
    }

    template<int size>
    void NativeEngine::SetMatrixN(NativeDataStream::Reader& data)
    {
        const auto& uniformInfo{*data.ReadPointer<UniformInfo>()};
        const auto matrix{data.ReadFloat32Array()};

        assert(matrix.size() == size * size);

        if constexpr (size < 4)
        {
            std::array<float, 16> matrixValues{};
            size_t index = 0;
            for (int line = 0; line < size; line++)
            {
                for (int col = 0; col < size; col++)
                {
                    matrixValues[line * 4 + col] = matrix[index++];
                }
            }
            m_currentProgram->SetUniform(uniformInfo.Handle, matrixValues);
        }
        else
        {
            m_currentProgram->SetUniform(uniformInfo.Handle, matrix);
        }
    }

    template<int size>
    void NativeEngine::SetIntArrayN(NativeDataStream::Reader& data)
    {
        const auto& uniformInfo{*data.ReadPointer<UniformInfo>()};
        const auto array{data.ReadInt32Array()};
        SetTypeArrayN<size>(uniformInfo, static_cast<uint32_t>(array.size()), array);
    }

    void NativeEngine::SetIntArray(NativeDataStream::Reader& data)
    {
        SetIntArrayN<1>(data);
    }

    void NativeEngine::SetIntArray2(NativeDataStream::Reader& data)
    {
        SetIntArrayN<2>(data);
    }

    void NativeEngine::SetIntArray3(NativeDataStream::Reader& data)
    {
        SetIntArrayN<3>(data);
    }

    void NativeEngine::SetIntArray4(NativeDataStream::Reader& data)
    {
        SetIntArrayN<4>(data);
    }

    template<int size>
    void NativeEngine::SetFloatArrayN(NativeDataStream::Reader& data)
    {
        const auto& uniformInfo{*data.ReadPointer<UniformInfo>()};
        const auto array{data.ReadFloat32Array()};
        SetTypeArrayN<size>(uniformInfo, static_cast<uint32_t>(array.size()), array);
    }

    void NativeEngine::SetFloatArray(NativeDataStream::Reader& data)
    {
        SetFloatArrayN<1>(data);
    }

    void NativeEngine::SetFloatArray2(NativeDataStream::Reader& data)
    {
        SetFloatArrayN<2>(data);
    }

    void NativeEngine::SetFloatArray3(NativeDataStream::Reader& data)
    {
        SetFloatArrayN<3>(data);
    }

    void NativeEngine::SetFloatArray4(NativeDataStream::Reader& data)
    {
        SetFloatArrayN<4>(data);
    }

    void NativeEngine::SetMatrices(NativeDataStream::Reader& data)
    {
        auto& uniform{*data.ReadPointer<UniformInfo>()};
        auto matrices{data.ReadFloat32Array()};

        assert(matrices.size() % 16 == 0);

        m_currentProgram->SetUniform(uniform.Handle, matrices, matrices.size() / 16);
    }

    void NativeEngine::SetMatrix2x2(NativeDataStream::Reader& data)
    {
        SetMatrixN<2>(data);
    }

    void NativeEngine::SetMatrix3x3(NativeDataStream::Reader& data)
    {
        SetMatrixN<3>(data);
    }

    void NativeEngine::SetMatrix(NativeDataStream::Reader& data)
    {
        SetMatrixN<4>(data);
    }

    void NativeEngine::SetFloat(NativeDataStream::Reader& data)
    {
        SetFloatN<1>(data);
    }

    void NativeEngine::SetFloat2(NativeDataStream::Reader& data)
    {
        SetFloatN<2>(data);
    }

    void NativeEngine::SetFloat3(NativeDataStream::Reader& data)
    {
        SetFloatN<3>(data);
    }

    void NativeEngine::SetFloat4(NativeDataStream::Reader& data)
    {
        SetFloatN<4>(data);
    }

    Napi::Value NativeEngine::CreateTexture(const Napi::CallbackInfo& info)
    {
        TextureData* texture = new TextureData();
        return Napi::Pointer<TextureData>::Create(info.Env(), texture, Napi::NapiPointerDeleter(texture));
    }

    void NativeEngine::LoadTexture(const Napi::CallbackInfo& info)
    {
        const auto texture = info[0].As<Napi::Pointer<TextureData>>().Get();
        const auto data = info[1].As<Napi::TypedArray>();
        const auto generateMips = info[2].As<Napi::Boolean>().Value();
        const auto invertY = info[3].As<Napi::Boolean>().Value();
        const auto srgb = info[4].As<Napi::Boolean>().Value();
        const auto onSuccess = info[5].As<Napi::Function>();
        const auto onError = info[6].As<Napi::Function>();

        const auto dataSpan = gsl::make_span(static_cast<uint8_t*>(data.ArrayBuffer().Data()) + data.ByteOffset(), data.ByteLength());

        arcana::make_task(arcana::threadpool_scheduler, *m_cancellationSource,
            [this, dataSpan, generateMips, invertY, srgb, texture, cancellationSource{m_cancellationSource}]() {
                bimg::ImageContainer* image{ParseImage(m_allocator, dataSpan)};
                image = PrepareImage(m_allocator, image, invertY, srgb, generateMips);
                LoadTextureFromImage(texture, image, srgb);
            })
            .then(m_runtimeScheduler, *m_cancellationSource, [dataRef{Napi::Persistent(data)}, onSuccessRef{Napi::Persistent(onSuccess)}, onErrorRef{Napi::Persistent(onError)}, cancellationSource{m_cancellationSource}](arcana::expected<void, std::exception_ptr> result) {
                if (result.has_error())
                {
                    onErrorRef.Call({});
                }
                else
                {
                    onSuccessRef.Call({});
                }
            });
    }

    void NativeEngine::CopyTexture(const Napi::CallbackInfo& info)
    {
        const auto textureDestination = info[0].As<Napi::Pointer<TextureData>>().Get();
        const auto textureSource = info[1].As<Napi::Pointer<TextureData>>().Get();
        const auto handleSource{textureSource->Handle};
        // Make sure destination texture is valid for BLIT and is not created from static datas.
        CreateBlitTexture(textureDestination);
        const auto handleDestination{ textureDestination->Handle };

        arcana::make_task(m_update.Scheduler(), *m_cancellationSource, [this, handleSource, handleDestination, cancellationSource{ m_cancellationSource }]() {
        return arcana::make_task(m_runtimeScheduler, *m_cancellationSource, [this, handleSource, handleDestination, updateToken{m_update.GetUpdateToken()}, cancellationSource{m_cancellationSource}]()
            {
                // JS Thread
                if (bgfx::getCaps()->supported & BGFX_CAPS_TEXTURE_BLIT)
                {
                    bgfx::Encoder* encoder = m_update.GetUpdateToken().GetEncoder();
                    GetBoundFrameBuffer(*encoder).Blit(*encoder, handleDestination, 0, 0, handleSource);
                }
                else
                {
                    throw std::runtime_error{ "This platform does not support texture blit." };
                }
            }).then(arcana::inline_scheduler, *m_cancellationSource, [this, cancellationSource{ m_cancellationSource }](const arcana::expected<void, std::exception_ptr>& result) {
                if (!cancellationSource->cancelled() && result.has_error())
                {
                    Napi::Error::New(Env(), result.error()).ThrowAsJavaScriptException();
                }
            });
        });
    }

    void NativeEngine::LoadRawTexture(const Napi::CallbackInfo& info)
    {
        const auto texture{info[0].As<Napi::Pointer<TextureData>>().Get()};
        const auto data{info[1].As<Napi::TypedArray>()};
        const auto width{static_cast<uint16_t>(info[2].As<Napi::Number>().Uint32Value())};
        const auto height{static_cast<uint16_t>(info[3].As<Napi::Number>().Uint32Value())};
        const auto format{static_cast<bimg::TextureFormat::Enum>(info[4].As<Napi::Number>().Uint32Value())};
        const auto generateMips{info[5].As<Napi::Boolean>().Value()};
        const auto invertY{info[6].As<Napi::Boolean>().Value()};

        const auto bytes{static_cast<uint8_t*>(data.ArrayBuffer().Data()) + data.ByteOffset()};
        if (data.ByteLength() != bimg::imageGetSize(nullptr, width, height, 1, false, false, 1, format))
        {
            throw Napi::Error::New(Env(), "The data size does not match width, height, and format");
        }

        bimg::ImageContainer* image{bimg::imageAlloc(&m_allocator, format, width, height, 1, 1, false, false, bytes)};
        image = PrepareImage(m_allocator, image, invertY, false, generateMips);
        LoadTextureFromImage(texture, image, false);
    }

    void NativeEngine::LoadRawTexture2DArray(const Napi::CallbackInfo& info)
    {
        const auto texture{info[0].As<Napi::Pointer<TextureData>>().Get()};
        const auto data = info[1].As<Napi::TypedArray>();
        const auto width{static_cast<uint16_t>(info[2].As<Napi::Number>().Uint32Value())};
        const auto height{static_cast<uint16_t>(info[3].As<Napi::Number>().Uint32Value())};
        const auto depth{static_cast<uint16_t>(info[4].As<Napi::Number>().Int32Value())};
        const auto format{static_cast<bimg::TextureFormat::Enum>(info[5].As<Napi::Number>().Uint32Value())};
        const auto generateMips = info[6].As<Napi::Boolean>().Value();
        const auto invertY = info[7].As<Napi::Boolean>().Value();

        if (generateMips)
        {
            throw Napi::Error::New(Env(), "Texture 2D array currently do not support mipmaps.");
        }

        if (invertY)
        {
            throw Napi::Error::New(Env(), "Texture 2D array currently do not support invert Y.");
        }

        texture->Width = width;
        texture->Height = height;
        uint64_t flags{BGFX_TEXTURE_NONE | BGFX_SAMPLER_NONE | BGFX_CAPS_TEXTURE_2D_ARRAY};
        texture->Handle = bgfx::createTexture2D(width, height, generateMips, depth, Cast(format), flags);
        texture->OwnsHandle = true;

        if (!data.IsNull())
        {
            if (data.ByteLength() != bimg::imageGetSize(nullptr, width, height, 1, false, false, depth, format))
            {
                throw Napi::Error::New(Env(), "The data size does not match width, height, depth and format");
            }

            uint8_t* dataPtr = static_cast<uint8_t*>(data.ArrayBuffer().Data()) + data.ByteOffset();
            uint32_t dataSize = static_cast<uint32_t>(data.ByteLength());
            const bgfx::Memory* dataCopy = bgfx::copy(dataPtr, dataSize); // This is required since BGFX must manage the data the memory.
            bgfx::updateTexture2D(texture->Handle, 0, 0, 0, 0, width, height, dataCopy);
        }
    }

    void NativeEngine::LoadCubeTexture(const Napi::CallbackInfo& info)
    {
        const auto texture = info[0].As<Napi::Pointer<TextureData>>().Get();
        const auto data{info[1].As<Napi::Array>()};
        const auto generateMips{info[2].As<Napi::Boolean>().Value()};
        const auto invertY{info[3].As<Napi::Boolean>().Value()};
        const auto srgb{info[4].As<Napi::Boolean>().Value()};
        const auto onSuccess{info[5].As<Napi::Function>()};
        const auto onError{info[6].As<Napi::Function>()};

        std::array<Napi::Reference<Napi::TypedArray>, 6> dataRefs;
        std::array<arcana::task<bimg::ImageContainer*, std::exception_ptr>, 6> tasks;
        for (uint32_t face = 0; face < data.Length(); face++)
        {
            const auto typedArray{data[face].As<Napi::TypedArray>()};
            const auto dataSpan{gsl::make_span(static_cast<uint8_t*>(typedArray.ArrayBuffer().Data()) + typedArray.ByteOffset(), typedArray.ByteLength())};
            dataRefs[face] = Napi::Persistent(typedArray);
            tasks[face] = arcana::make_task(arcana::threadpool_scheduler, *m_cancellationSource, [this, dataSpan, invertY, generateMips, srgb]() {
                bimg::ImageContainer* image{ParseImage(m_allocator, dataSpan)};
                image = PrepareImage(m_allocator, image, invertY, srgb, generateMips);
                return image;
            });
        }

        arcana::when_all(gsl::make_span(tasks))
            .then(arcana::inline_scheduler, *m_cancellationSource, [texture, srgb, cancellationSource{m_cancellationSource}](std::vector<bimg::ImageContainer*> images) {
                LoadCubeTextureFromImages(texture, images, srgb);
            })
            .then(m_runtimeScheduler, *m_cancellationSource, [dataRefs{std::move(dataRefs)}, onSuccessRef{Napi::Persistent(onSuccess)}, onErrorRef{Napi::Persistent(onError)}, cancellationSource{m_cancellationSource}](arcana::expected<void, std::exception_ptr> result) {
                if (result.has_error())
                {
                    onErrorRef.Call({});
                }
                else
                {
                    onSuccessRef.Call({});
                }
            });
    }

    void NativeEngine::LoadCubeTextureWithMips(const Napi::CallbackInfo& info)
    {
        const auto texture = info[0].As<Napi::Pointer<TextureData>>().Get();
        const auto data{info[1].As<Napi::Array>()};
        const auto invertY{info[2].As<Napi::Boolean>().Value()};
        const auto srgb{info[3].As<Napi::Boolean>().Value()};
        const auto onSuccess{info[4].As<Napi::Function>()};
        const auto onError{info[5].As<Napi::Function>()};

        const auto numMips{static_cast<size_t>(data.Length())};
        std::vector<Napi::Reference<Napi::TypedArray>> dataRefs(6 * numMips);
        std::vector<arcana::task<bimg::ImageContainer*, std::exception_ptr>> tasks(6 * numMips);
        for (uint32_t mip = 0; mip < numMips; mip++)
        {
            const auto faceData = data[mip].As<Napi::Array>();
            for (uint32_t face = 0; face < 6; face++)
            {
                const auto typedArray = faceData[face].As<Napi::TypedArray>();
                const auto dataSpan = gsl::make_span(static_cast<uint8_t*>(typedArray.ArrayBuffer().Data()) + typedArray.ByteOffset(), typedArray.ByteLength());
                dataRefs[(face * numMips) + mip] = Napi::Persistent(typedArray);
                tasks[(face * numMips) + mip] = arcana::make_task(arcana::threadpool_scheduler, *m_cancellationSource, [this, dataSpan, invertY, srgb]() {
                    bimg::ImageContainer* image{ParseImage(m_allocator, dataSpan)};
                    image = PrepareImage(m_allocator, image, invertY, srgb, false);
                    return image;
                });
            }
        }

        arcana::when_all(gsl::make_span(tasks))
            .then(arcana::inline_scheduler, *m_cancellationSource, [texture, srgb, cancellationSource{m_cancellationSource}](std::vector<bimg::ImageContainer*> images) {
                LoadCubeTextureFromImages(texture, images, srgb);
            })
            .then(m_runtimeScheduler, *m_cancellationSource, [dataRefs{std::move(dataRefs)}, onSuccessRef{Napi::Persistent(onSuccess)}, onErrorRef{Napi::Persistent(onError)}, cancellationSource{m_cancellationSource}](arcana::expected<void, std::exception_ptr> result) {
                if (result.has_error())
                {
                    onErrorRef.Call({});
                }
                else
                {
                    onSuccessRef.Call({});
                }
            });
    }

    Napi::Value NativeEngine::GetTextureWidth(const Napi::CallbackInfo& info)
    {
        const TextureData* texture = info[0].As<Napi::Pointer<TextureData>>().Get();
        return Napi::Value::From(info.Env(), texture->Width);
    }

    Napi::Value NativeEngine::GetTextureHeight(const Napi::CallbackInfo& info)
    {
        const TextureData* texture = info[0].As<Napi::Pointer<TextureData>>().Get();
        return Napi::Value::From(info.Env(), texture->Height);
    }

    void NativeEngine::SetTextureSampling(NativeDataStream::Reader& data)
    {
        auto& texture = *data.ReadPointer<TextureData>();
        const auto value = data.ReadUint32();

        texture.Flags &= ~(BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT | BGFX_SAMPLER_MIP_POINT);
        texture.Flags |= value;

        // Disable anisotropy if either min/mag are point.
        if ((texture.Flags & BGFX_SAMPLER_MIN_POINT) != 0 || (texture.Flags & BGFX_SAMPLER_MAG_POINT) != 0)
        {
            texture.Flags &= ~(BGFX_SAMPLER_MIN_ANISOTROPIC | BGFX_SAMPLER_MAG_ANISOTROPIC);
        }
    }

    void NativeEngine::SetTextureWrapMode(NativeDataStream::Reader& data)
    {
        auto& texture = *data.ReadPointer<TextureData>();
        auto addressModeU = data.ReadUint32();
        auto addressModeV = data.ReadUint32();
        auto addressModeW = data.ReadUint32();

        uint32_t addressMode = addressModeU +
            (addressModeV << BGFX_SAMPLER_V_SHIFT) +
            (addressModeW << BGFX_SAMPLER_W_SHIFT);

        texture.Flags &= ~(BGFX_SAMPLER_U_MASK | BGFX_SAMPLER_V_MASK | BGFX_SAMPLER_W_MASK);
        texture.Flags |= addressMode;
    }

    void NativeEngine::SetTextureAnisotropicLevel(NativeDataStream::Reader& data)
    {
        auto& texture = *data.ReadPointer<TextureData>();
        const auto value = data.ReadUint32();

        texture.Flags &= ~(BGFX_SAMPLER_MIN_ANISOTROPIC | BGFX_SAMPLER_MAG_ANISOTROPIC);

        // Enable anisotropy only if neither min/mag are point.
        // Note that bgfx currently only supports no anisotropy or max anisotropy.
        if ((texture.Flags & BGFX_SAMPLER_MIN_POINT) == 0 && (texture.Flags & BGFX_SAMPLER_MAG_POINT) == 0 && value > 1)
        {
            texture.Flags |= BGFX_SAMPLER_MIN_ANISOTROPIC | BGFX_SAMPLER_MAG_ANISOTROPIC;

            // There is a bug in bgfx that causes the samplers to do the wrong thing in DirectX.
            // Remove this once https://github.com/bkaradzic/bgfx/pull/2609 is merged.
            texture.Flags &= ~BGFX_SAMPLER_MIP_MASK;
        }
    }

    void NativeEngine::SetTexture(NativeDataStream::Reader& data)
    {
        bgfx::Encoder* encoder = GetUpdateToken().GetEncoder();

        const UniformInfo* uniformInfo = data.ReadPointer<UniformInfo>();
        const TextureData* texture = data.ReadPointer<TextureData>();

        encoder->setTexture(uniformInfo->Stage, uniformInfo->Handle, texture->Handle, texture->Flags);
    }

    void NativeEngine::DeleteTexture(const Napi::CallbackInfo& info)
    {
        TextureData* texture = info[0].As<Napi::Pointer<TextureData>>().Get();
        m_graphicsImpl.RemoveTexture(texture->Handle);
        texture->Dispose();
    }

    Napi::Value NativeEngine::CreateFrameBuffer(const Napi::CallbackInfo& info)
    {
        TextureData* texture = info[0].As<Napi::Pointer<TextureData>>().Get();
        uint16_t width = static_cast<uint16_t>(info[1].As<Napi::Number>().Uint32Value());
        uint16_t height = static_cast<uint16_t>(info[2].As<Napi::Number>().Uint32Value());
        bgfx::TextureFormat::Enum format = static_cast<bgfx::TextureFormat::Enum>(info[3].As<Napi::Number>().Uint32Value());
        bool generateStencilBuffer = info[4].As<Napi::Boolean>();
        bool generateDepth = info[5].As<Napi::Boolean>();
        bool generateMips = info[6].As<Napi::Boolean>();

        bgfx::FrameBufferHandle frameBufferHandle{};
        if (generateDepth)
        {
            auto depthStencilFormat = bgfx::TextureFormat::D32;
            if (generateStencilBuffer)
            {
                depthStencilFormat = bgfx::TextureFormat::D24S8;
            }

            assert(bgfx::isTextureValid(0, false, 1, format, BGFX_TEXTURE_RT));
            assert(bgfx::isTextureValid(0, false, 1, depthStencilFormat, BGFX_TEXTURE_RT));

            // bgfx doesn't add flag D3D11_RESOURCE_MISC_GENERATE_MIPS for depth textures (missing that flag will crash D3D with resolving)
            // And not sure it makes sense to generate mipmaps from a depth buffer with exponential values.
            // only allows mipmaps resolve step when mipmapping is asked and for the color texture, not the depth.
            // https://github.com/bkaradzic/bgfx/blob/2c21f68998595fa388e25cb6527e82254d0e9bff/src/renderer_d3d11.cpp#L4525
            std::array<bgfx::TextureHandle, 2> textures{
                bgfx::createTexture2D(width, height, generateMips, 1, format, BGFX_TEXTURE_RT),
                bgfx::createTexture2D(width, height, false, 1, depthStencilFormat, BGFX_TEXTURE_RT)};
            std::array<bgfx::Attachment, textures.size()> attachments{};
            for (size_t idx = 0; idx < attachments.size(); ++idx)
            {
                attachments[idx].init(textures[idx]);
            }
            frameBufferHandle = bgfx::createFrameBuffer(static_cast<uint8_t>(attachments.size()), attachments.data(), true);
        }
        else
        {
            assert(!generateStencilBuffer);
            frameBufferHandle = bgfx::createFrameBuffer(width, height, format, BGFX_TEXTURE_RT);
        }

        texture->Handle = bgfx::getTexture(frameBufferHandle);
        texture->OwnsHandle = false;

        m_graphicsImpl.AddTexture(texture->Handle, width, height, generateMips, 1, format);

        FrameBuffer* frameBuffer = new FrameBuffer(m_graphicsImpl, frameBufferHandle, width, height, false, generateDepth, generateStencilBuffer);
        return Napi::Pointer<FrameBuffer>::Create(info.Env(), frameBuffer, Napi::NapiPointerDeleter(frameBuffer));
    }

    // TODO: This doesn't get called when an Engine instance is disposed.
    void NativeEngine::DeleteFrameBuffer(NativeDataStream::Reader& data)
    {
        data.ReadPointer<FrameBuffer>()->Dispose();
    }

    void NativeEngine::BindFrameBuffer(NativeDataStream::Reader& data)
    {
        auto encoder = GetUpdateToken().GetEncoder();

        FrameBuffer* frameBuffer = data.ReadPointer<FrameBuffer>();
        m_boundFrameBuffer->Unbind(*encoder);
        m_boundFrameBuffer = frameBuffer;
        m_boundFrameBuffer->Bind(*encoder);
        m_boundFrameBufferNeedsRebinding.Set(*encoder, false);
    }

    void NativeEngine::UnbindFrameBuffer(NativeDataStream::Reader& data)
    {
        bgfx::Encoder* encoder = GetUpdateToken().GetEncoder();

        const FrameBuffer* frameBuffer = data.ReadPointer<FrameBuffer>();

        assert(m_boundFrameBuffer == frameBuffer);
        UNUSED(frameBuffer);

        m_boundFrameBuffer->Unbind(*encoder);
        m_boundFrameBuffer = nullptr;
        m_boundFrameBufferNeedsRebinding.Set(*encoder, false);
    }

    void NativeEngine::DrawIndexed(NativeDataStream::Reader& data)
    {
        bgfx::Encoder* encoder{GetUpdateToken().GetEncoder()};

        const uint32_t fillMode = data.ReadUint32();
        const uint32_t indexStart = data.ReadUint32();
        const uint32_t indexCount = data.ReadUint32();

        if (m_boundVertexArray != nullptr)
        {
            m_boundVertexArray->SetIndexBuffer(encoder, indexStart, indexCount);
            m_boundVertexArray->SetVertexBuffers(encoder, 0, std::numeric_limits<uint32_t>::max());
        }

        DrawInternal(encoder, fillMode);
    }

    void NativeEngine::Draw(NativeDataStream::Reader& data)
    {
        bgfx::Encoder* encoder{GetUpdateToken().GetEncoder()};

        const uint32_t fillMode = data.ReadUint32();
        const uint32_t verticesStart = data.ReadUint32();
        const uint32_t verticesCount = data.ReadUint32();

        if (m_boundVertexArray != nullptr)
        {
            m_boundVertexArray->SetVertexBuffers(encoder, verticesStart, verticesCount);
        }

        DrawInternal(encoder, fillMode);
    }

    void NativeEngine::Clear(NativeDataStream::Reader& data)
    {
        bgfx::Encoder* encoder{GetUpdateToken().GetEncoder()};

        uint16_t flags{0};
        uint32_t rgba{0x000000ff};

        const bool shouldClearColor{static_cast<bool>(data.ReadUint32())};
        const float r{data.ReadFloat32()};
        const float g{data.ReadFloat32()};
        const float b{data.ReadFloat32()};
        const float a{data.ReadFloat32()};
        const bool shouldClearDepth{static_cast<bool>(data.ReadUint32())};
        const float depth{data.ReadFloat32()};
        const bool shouldClearStencil{static_cast<bool>(data.ReadUint32())};
        const uint8_t stencil{static_cast<uint8_t>(data.ReadUint32())};

        if (shouldClearColor)
        {
            rgba =
                (static_cast<uint8_t>(r * std::numeric_limits<uint8_t>::max()) << 24) +
                (static_cast<uint8_t>(g * std::numeric_limits<uint8_t>::max()) << 16) +
                (static_cast<uint8_t>(b * std::numeric_limits<uint8_t>::max()) << 8) +
                static_cast<uint8_t>(a * std::numeric_limits<uint8_t>::max());

            flags |= BGFX_CLEAR_COLOR;
        }

        if (shouldClearDepth && (!m_boundFrameBuffer || m_boundFrameBuffer->HasDepth()))
        {
            flags |= BGFX_CLEAR_DEPTH;
        }

        if (shouldClearStencil && (!m_boundFrameBuffer || m_boundFrameBuffer->HasStencil()))
        {
            flags |= BGFX_CLEAR_STENCIL;
        }

        GetBoundFrameBuffer(*encoder).Clear(*encoder, flags, rgba, depth, stencil);
    }

    Napi::Value NativeEngine::GetRenderWidth(const Napi::CallbackInfo& info)
    {
        return Napi::Value::From(info.Env(), bgfx::getStats()->width);
    }

    Napi::Value NativeEngine::GetRenderHeight(const Napi::CallbackInfo& info)
    {
        return Napi::Value::From(info.Env(), bgfx::getStats()->height);
    }

    void NativeEngine::SetViewPort(const Napi::CallbackInfo& info)
    {
        bgfx::Encoder* encoder{GetUpdateToken().GetEncoder()};

        const auto x = info[0].As<Napi::Number>().FloatValue();
        const auto y = info[1].As<Napi::Number>().FloatValue();
        const auto width = info[2].As<Napi::Number>().FloatValue();
        const auto height = info[3].As<Napi::Number>().FloatValue();
        const float yOrigin = bgfx::getCaps()->originBottomLeft ? y : (1.f - y - height);

        GetBoundFrameBuffer(*encoder).SetViewPort(*encoder, x, yOrigin, width, height);
    }

    Napi::Value NativeEngine::GetHardwareScalingLevel(const Napi::CallbackInfo& info)
    {
        return Napi::Value::From(info.Env(), m_graphicsImpl.GetHardwareScalingLevel());
    }

    void NativeEngine::SetHardwareScalingLevel(const Napi::CallbackInfo& info)
    {
        const auto level = info[0].As<Napi::Number>().FloatValue();
        m_graphicsImpl.SetHardwareScalingLevel(level);
    }

    Napi::Value NativeEngine::CreateImageBitmap(const Napi::CallbackInfo& info)
    {
        const Napi::Env env{info.Env()};

        if (!info[0].IsArrayBuffer())
        {
            throw Napi::Error::New(env, "CreateImageBitmap parameter is not an array buffer.");
        }

        const auto data{info[0].As<Napi::ArrayBuffer>()};

        if (!data.ByteLength())
        {
            throw Napi::Error::New(env, "CreateImageBitmap array buffer is empty.");
        }

        bimg::ImageContainer* image{ParseImage(m_allocator, gsl::make_span(static_cast<uint8_t*>(data.Data()), data.ByteLength()))};

        Napi::Object imageBitmap{Napi::Object::New(env)};
        Napi::Uint8Array buffer{Napi::Uint8Array::New(env, image->m_size)};
        std::memcpy(buffer.Data(), image->m_data, image->m_size);

        imageBitmap.Set("data", buffer);
        imageBitmap.Set("width", Napi::Value::From(env, image->m_width));
        imageBitmap.Set("height", Napi::Value::From(env, image->m_height));
        imageBitmap.Set("depth", Napi::Value::From(env, image->m_depth));
        imageBitmap.Set("numLayers", Napi::Value::From(env, image->m_numLayers));
        imageBitmap.Set("format", Napi::Value::From(env, static_cast<uint32_t>(image->m_format)));

        bimg::imageFree(image);
        return std::move(imageBitmap);
    }

    Napi::Value NativeEngine::ResizeImageBitmap(const Napi::CallbackInfo& info)
    {
        const auto imageBitmap = info[0].As<Napi::Object>();
        const auto bufferWidth = info[1].As<Napi::Number>().Uint32Value();
        const auto bufferHeight = info[2].As<Napi::Number>().Uint32Value();

        const auto data = imageBitmap.Get("data").As<Napi::Uint8Array>();
        const auto width = imageBitmap.Get("width").As<Napi::Number>().Uint32Value();
        const auto height = imageBitmap.Get("height").As<Napi::Number>().Uint32Value();
        const auto format = static_cast<bimg::TextureFormat::Enum>(imageBitmap.Get("format").As<Napi::Number>().Uint32Value());

        const Napi::Env env{info.Env()};

        bimg::ImageContainer* image = bimg::imageAlloc(&m_allocator, format, static_cast<uint16_t>(width), static_cast<uint16_t>(height), 1, 1, false, false, data.Data());
        if (image == nullptr)
        {
            throw Napi::Error::New(env, "Unable to allocate image for ResizeImageBitmap.");
        }

        if (format != bimg::TextureFormat::RGBA8)
        {
            if (format == bimg::TextureFormat::R8)
            {
                image->m_format = bimg::TextureFormat::A8;
            }
            bimg::ImageContainer* rgba = bimg::imageConvert(&m_allocator, bimg::TextureFormat::RGBA8, *image, false);
            if (rgba == nullptr)
            {
                throw Napi::Error::New(env, "Unable to convert image to RGBA pixel format for ResizeImageBitmap.");
            }
            bimg::imageFree(image);
            image = rgba;
        }

        auto outputData = Napi::Uint8Array::New(env, bufferWidth * bufferHeight * 4);
        if (width != bufferWidth || height != bufferHeight)
        {
            stbir_resize_uint8(static_cast<unsigned char*>(image->m_data), width, height, 0,
                outputData.Data(), bufferWidth, bufferHeight, 0, 4);
        }
        else
        {
            std::memcpy(outputData.Data(), image->m_data, image->m_size);
        }
        bimg::imageFree(image);
        return Napi::Value::From(env, outputData);
    }

    void NativeEngine::GetFrameBufferData(const Napi::CallbackInfo& info)
    {
        const auto callback{info[0].As<Napi::Function>()};

        auto callbackPtr{std::make_shared<Napi::FunctionReference>(Napi::Persistent(callback))};
        m_graphicsImpl.RequestScreenShot([this, callbackPtr{std::move(callbackPtr)}](std::vector<uint8_t> array) {
            m_runtime.Dispatch([callbackPtr{std::move(callbackPtr)}, array{std::move(array)}](Napi::Env env) {
                auto arrayBuffer{Napi::ArrayBuffer::New(env, const_cast<uint8_t*>(array.data()), array.size())};
                auto typedArray{Napi::Uint8Array::New(env, array.size(), arrayBuffer, 0)};
                callbackPtr->Value().Call({typedArray});
            });
        });
    }

    void NativeEngine::SetStencil(NativeDataStream::Reader& data)
    {
        const uint32_t writeMask{data.ReadUint32()};
        const uint32_t stencilOpFail{data.ReadUint32()};
        const uint32_t depthOpFail{data.ReadUint32()};
        const uint32_t depthOpPass{data.ReadUint32()};
        const uint32_t func{data.ReadUint32()};
        const uint32_t ref{data.ReadUint32()};

        m_stencilState = BGFX_STENCIL_FUNC_RMASK(0xFF); //  always 0xFF
        m_stencilState |= stencilOpFail;
        m_stencilState |= depthOpFail;
        // bgfx write mask is always 0xFF, to not change stencil value when writemask is 0
        // its value is kept unchanged.
        // https://github.com/bkaradzic/bgfx/blob/2c21f68998595fa388e25cb6527e82254d0e9bff/src/renderer_d3d11.cpp#L2874
        if (writeMask == 0)
        {
            m_stencilState |= BGFX_STENCIL_OP_PASS_Z_KEEP;
        }
        else
        {
            m_stencilState |= depthOpPass;
        }
        m_stencilState |= func;
        m_stencilState |= BGFX_STENCIL_FUNC_REF(ref);
    }

    void NativeEngine::SetCommandDataStream(const Napi::CallbackInfo& info)
    {
        // TODO: This should be moved to the constructor once multi-update is available.
        Napi::Object jsCommandStream = info[0].ToObject();
        m_commandStream = Napi::ObjectWrap<NativeDataStream>::Unwrap(jsCommandStream.Get("_nativeDataStream").As<Napi::Object>());
    }

    void NativeEngine::SubmitCommands(const Napi::CallbackInfo&)
    {
        NativeDataStream::Reader reader = m_commandStream->GetReader();
        while (reader.CanRead())
        {
            std::invoke(reader.ReadPointer<CommandFunctionPointerT>(), this, reader);
        }
    }

    void NativeEngine::DrawInternal(bgfx::Encoder* encoder, uint32_t fillMode)
    {
        uint64_t fillModeState{0}; // indexed triangle list
        switch (fillMode)
        {
            case 0: // MATERIAL_TriangleFillMode
            {
                fillModeState = 0;
                break;
            }
            case 1: // MATERIAL_WireFrameFillMode
            case 4: // MATERIAL_LineListDrawMode
            {
                fillModeState = BGFX_STATE_PT_LINES;
                break;
            }
            case 2: // MATERIAL_PointFillMode
            case 3: // MATERIAL_PointListDrawMode
            {
                fillModeState = BGFX_STATE_PT_POINTS;
                break;
            }
            case 5: // MATERIAL_LineLoopDrawMode
            {
                // TODO: unsupported mode
                break;
            }
            case 6: // MATERIAL_LineStripDrawMode
            {
                fillModeState = BGFX_STATE_PT_LINESTRIP;
                break;
            }
            case 7: // MATERIAL_TriangleStripDrawMode
            {
                fillModeState = BGFX_STATE_PT_TRISTRIP;
                break;
            }
            case 8: // MATERIAL_TriangleFanDrawMode
            {
                // TODO: unsupported mode
                break;
            }
        }

        for (const auto& it : m_currentProgram->Uniforms)
        {
            const ProgramData::UniformValue& value = it.second;
            encoder->setUniform({ it.first }, value.Data.data(), value.ElementLength);
        }

        auto& boundFrameBuffer = GetBoundFrameBuffer(*encoder);
        if (boundFrameBuffer.HasDepth())
        {
            encoder->setState(m_engineState | fillModeState);
        }
        else
        {
            encoder->setState((m_engineState & ~BGFX_STATE_WRITE_Z) | fillModeState);
        }

        // stencil
        boundFrameBuffer.SetStencil(*encoder, m_stencilState);

        // Discard everything except bindings since we keep the state of everything else.
        boundFrameBuffer.Submit(*encoder, m_currentProgram->Handle, BGFX_DISCARD_ALL & ~BGFX_DISCARD_BINDINGS);
    }

    GraphicsImpl::UpdateToken& NativeEngine::GetUpdateToken()
    {
        if (!m_updateToken)
        {
            m_updateToken.emplace(m_update.GetUpdateToken());
            m_runtime.Dispatch([this](auto) {
                m_updateToken.reset();
            });
        }

        return m_updateToken.value();
    }

    FrameBuffer& NativeEngine::GetBoundFrameBuffer(bgfx::Encoder& encoder)
    {
        if (m_boundFrameBuffer == nullptr)
        {
            m_boundFrameBuffer = &m_defaultFrameBuffer;
            m_defaultFrameBuffer.Bind(encoder);
        } else if (m_boundFrameBufferNeedsRebinding.Get(encoder))
        {
            m_boundFrameBuffer->Unbind(encoder);
            m_boundFrameBuffer->Bind(encoder);
        }
        
        m_boundFrameBufferNeedsRebinding.Set(encoder, false);
        return *m_boundFrameBuffer;
    }

    void NativeEngine::ScheduleRequestAnimationFrameCallbacks()
    {
        if (m_requestAnimationFrameCallbacksScheduled)
        {
            return;
        }

        m_requestAnimationFrameCallbacksScheduled = true;

        arcana::make_task(m_update.Scheduler(), *m_cancellationSource, [this, cancellationSource{m_cancellationSource}]() {
            return arcana::make_task(m_runtimeScheduler, *m_cancellationSource, [this, updateToken{m_update.GetUpdateToken()}, cancellationSource{m_cancellationSource}]() {
                m_requestAnimationFrameCallbacksScheduled = false;

                arcana::trace_region scheduleRegion{"NativeEngine::ScheduleRequestAnimationFrameCallbacks invoke JS callbacks"};
                auto callbacks{std::move(m_requestAnimationFrameCallbacks)};
                for (auto& callback : callbacks)
                {
                    callback.Value().Call({});
                }
            }).then(arcana::inline_scheduler, *m_cancellationSource, [this, cancellationSource{m_cancellationSource}](const arcana::expected<void, std::exception_ptr>& result) {
                if (!cancellationSource->cancelled() && result.has_error())
                {
                    Napi::Error::New(Env(), result.error()).ThrowAsJavaScriptException();
                }
            });
        });
    }
}
