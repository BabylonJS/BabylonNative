#include "NativeEngine.h"

#include <Babylon/Graphics/Texture.h>

#include "JsConsoleLogger.h"

#include <arcana/threading/task.h>
#include <arcana/threading/task_schedulers.h>
#include <arcana/macros.h>
#include <arcana/tracing/trace_region.h>

#include <napi/env.h>
#include <napi/pointer.h>

#include <bgfx/bgfx.h>

#include <bimg/bimg.h>
#include <bimg/decode.h>
#include <bimg/encode.h>

#include <stb/stb_image_resize.h>
#include <bx/math.h>

#include <cmath>

#ifdef WEBP
#include <webp/decode.h>
#endif

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

        void FlipImage(gsl::span<uint8_t> image, uint32_t height)
        {
            const size_t rowPitch{image.size() / height};

            std::vector<uint8_t> buffer(rowPitch);
            for (size_t row = 0; row < height / 2; row++)
            {
                uint8_t* frontPtr{image.data() + (row * rowPitch)};
                uint8_t* backPtr{image.data() + ((height - row - 1) * rowPitch)};

                std::memcpy(buffer.data(), frontPtr, rowPitch);
                std::memcpy(frontPtr, backPtr, rowPitch);
                std::memcpy(backPtr, buffer.data(), rowPitch);
            }
        }

        std::function<std::pair<uint32_t, uint32_t>(uint32_t x, uint32_t y)> GetPixelMapper(bimg::Orientation::Enum orientation, uint32_t width, uint32_t height)
        {
            // clang-format off
            switch (orientation)
            {
                case bimg::Orientation::R0: return [](uint32_t x, uint32_t y) { return std::make_pair(x, y); };
                case bimg::Orientation::R90: return [height](uint32_t x, uint32_t y) { return std::make_pair(height - y - 1, x); };
                case bimg::Orientation::R180: return [width, height](uint32_t x, uint32_t y) { return std::make_pair(width - x - 1, height - y - 1); };
                case bimg::Orientation::R270: return [width](uint32_t x, uint32_t y) { return std::make_pair(y, width - x - 1); };
                case bimg::Orientation::HFlip: return [width](uint32_t x, uint32_t y) { return std::make_pair(width - x - 1, y); };
                case bimg::Orientation::HFlipR90: return [width, height](uint32_t x, uint32_t y) { return std::make_pair(height - y - 1, width - x - 1); };
                case bimg::Orientation::HFlipR270: return [](uint32_t x, uint32_t y) { return std::make_pair(y, x); };
                case bimg::Orientation::VFlip: return [height](uint32_t x, uint32_t y) { return std::make_pair(x, height - y - 1); };
                default: throw std::runtime_error{"Unexpected image orientation."};
            }
            // clang-format on
        }

        using RGBA8ImageData = gsl::span<uint32_t>;
        void ReorientImage(RGBA8ImageData image, uint32_t& width, uint32_t& height, bimg::Orientation::Enum orientation)
        {
            // If the orientation is the default, nothing needs to be done.
            if (orientation == bimg::Orientation::R0)
            {
                // No-op
            }
            // If we only need a vflip, use the more efficient FlipImage function.
            else if (orientation == bimg::Orientation::VFlip)
            {
                FlipImage(gsl::make_span(reinterpret_cast<uint8_t*>(image.data()), image.size_bytes()), height);
            }
            // Otherwise we'll do a pixel by pixel transformation, using a temporary image buffer.
            else if (orientation != bimg::Orientation::R0)
            {
                uint32_t newWidth = width;
                uint32_t newHeight = height;

                // If the orientation is 90 or 270, the final image width and height will be swapped.
                if (orientation == bimg::Orientation::R90 || orientation == bimg::Orientation::R270 || orientation == bimg::Orientation::HFlipR90 || orientation == bimg::Orientation::HFlipR270)
                {
                    std::swap(newWidth, newHeight);
                }

                // Transform the pixels from the original image data to a temp image buffer.
                std::vector<uint32_t> buffer(image.size());
                auto mapPixel = GetPixelMapper(orientation, width, height);
                for (uint32_t y = 0; y < height; y++)
                {
                    for (uint32_t x = 0; x < width; x++)
                    {
                        const RGBA8ImageData::size_type index = y * width + x;
                        const std::pair<uint32_t, uint32_t> mappedPixel{mapPixel(x, y)};
                        const RGBA8ImageData::size_type newIndex = mappedPixel.second * newWidth + mappedPixel.first;
                        buffer[newIndex] = image[index];
                    }
                }

                // Copy the temp image buffer over the original image data.
                std::memcpy(image.data(), buffer.data(), image.size_bytes());

                // Update the final width and height.
                width = newWidth;
                height = newHeight;
            }
        }

        bimg::ImageContainer* ParseImage(bx::AllocatorI& allocator, gsl::span<uint8_t> data)
        {
            bimg::ImageContainer* image{bimg::imageParse(&allocator, data.data(), static_cast<uint32_t>(data.size()))};
            if (image == nullptr)
            {
#ifdef WEBP
                int width;
                int height;
                if (WebPGetInfo(data.data(), data.size(), &width, &height))
                {
                    image = bimg::imageAlloc(&allocator, bimg::TextureFormat::RGBA8, static_cast<uint16_t>(width), static_cast<uint16_t>(height), 1, 1, false, false);
                    if (WebPDecodeRGBAInto(data.data(), data.size(), static_cast<uint8_t*>(image->m_data), static_cast<size_t>(image->m_size), width * 4))
                    {
                        return image;
                    }
                }
#endif

                throw std::runtime_error{"Failed to parse image."};
            }

            assert(image->m_offset == 0);
            assert(image->m_depth == 1);
            assert(image->m_numLayers == 1);
            assert(image->m_numMips == 1);
            assert(!image->m_cubeMap);

            if (image->m_format == bimg::TextureFormat::R8 ||
                image->m_format == bimg::TextureFormat::RG8)
            {
                static const TransformFn UnpackR8{[](const uint8_t* src, uint8_t* dst) { dst[0] = dst[1] = dst[2] = src[0]; dst[3] = 1; }};
                static const TransformFn UnpackRG8{[](const uint8_t* src, uint8_t* dst) { dst[0] = dst[1] = dst[2] = src[0]; dst[3] = src[1]; }};

                // bimg loads grayscale textures with and without alpha as R8 and RG8 respectively.
                // Unpack to RGB and RGBA such that RGB is the grayscale and the A is the alpha.
                bimg::ImageContainer* oldImage{image};
                image = bimg::imageAlloc(&allocator, bimg::TextureFormat::RGBA8, static_cast<uint16_t>(image->m_width), static_cast<uint16_t>(image->m_height), 1, 1, false, false);
                TransformImage(oldImage, image, oldImage->m_format == bimg::TextureFormat::R8 ? UnpackR8 : UnpackRG8);
                bimg::imageFree(oldImage);
            }

            // If the image has a non-zero orientation (e.g. via jpeg metadata), take this into account by updating the orientation of the underlying image data.
            if (image->m_orientation != bimg::Orientation::R0)
            {
                // Convert from RGB8 to RGBA8.
                if (image->m_format == bimg::TextureFormat::RGB8)
                {
                    bimg::ImageContainer* oldImage{image};
                    image = bimg::imageConvert(&allocator, bimg::TextureFormat::RGBA8, *image, false);
                    image->m_orientation = oldImage->m_orientation;
                    bimg::imageFree(oldImage);
                }

                // If the image is RGBA8, update the image data according to the orientation.
                // Other formats could be handled in the future, but this should at least cover jpeg images.
                if (image->m_format == bimg::TextureFormat::RGBA8)
                {
                    assert(bimg::getBitsPerPixel(image->m_format) == sizeof(uint32_t) * 8);
                    ReorientImage(gsl::make_span(static_cast<uint32_t*>(image->m_data), image->m_size / sizeof(uint32_t)), image->m_width, image->m_height, image->m_orientation);
                }
            }

            return image;
        }

        bimg::ImageContainer* PrepareImage(bx::AllocatorI& allocator, bimg::ImageContainer* image, bool invertY, bool srgb, bool generateMips)
        {
            assert(
                image->m_format == bimg::TextureFormat::R16 ||
                image->m_format == bimg::TextureFormat::RGB8 ||
                image->m_format == bimg::TextureFormat::RGBA8 ||
                image->m_format == bimg::TextureFormat::RGBA16 ||
                image->m_format == bimg::TextureFormat::RGBA16F ||
                image->m_format == bimg::TextureFormat::RG16F ||
                image->m_format == bimg::TextureFormat::RGBA32F);

            assert(image->m_depth == 1);
            assert(image->m_numLayers == 1);
            assert(image->m_numMips == 1);
            assert(image->m_cubeMap == false);

            if (bgfx::getCaps()->originBottomLeft ? invertY : !invertY)
            {
                FlipImage({static_cast<uint8_t*>(image->m_data), image->m_size}, image->m_height);
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
                else if (image->m_format == bimg::TextureFormat::RG16F || image->m_format == bimg::TextureFormat::RGBA16F || image->m_format == bimg::TextureFormat::RGBA16 || image->m_format == bimg::TextureFormat::R16)
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

        void LoadTextureFromImage(Graphics::Texture* texture, bimg::ImageContainer* image, bool srgb)
        {
            if (texture->IsValid())
            {
                if (texture->Width() != image->m_width || texture->Height() != image->m_height)
                {
                    throw std::runtime_error{"Cannot update texture from image of different size"};
                }
            }
            else
            {
                uint64_t flags = srgb ? BGFX_TEXTURE_SRGB : BGFX_TEXTURE_NONE;
                texture->Create2D(static_cast<uint16_t>(image->m_width), static_cast<uint16_t>(image->m_height), (image->m_numMips > 1), 1, Cast(image->m_format), flags);
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
                    texture->Update2D(0, mip, 0, 0, static_cast<uint16_t>(imageMip.m_width), static_cast<uint16_t>(imageMip.m_height), mem);
                }
            }
        }

        void LoadCubeTextureFromImages(Graphics::Texture* texture, std::vector<bimg::ImageContainer*>& images, bool srgb)
        {
            const bimg::ImageContainer* firstImage{images.front()};
            assert(firstImage->m_width == firstImage->m_height);
            uint32_t size{firstImage->m_width};

            if (texture->IsValid())
            {
                if (texture->Width() != size || texture->Height() != size)
                {
                    throw std::runtime_error{"Cannot update texture from image of different size"};
                }
            }
            else
            {
                bool hasMips = images.size() > 6 || firstImage->m_numMips > 1;
                bgfx::TextureFormat::Enum format = Cast(firstImage->m_format);
                uint64_t flags = srgb ? BGFX_TEXTURE_SRGB : BGFX_TEXTURE_NONE;
                texture->CreateCube(static_cast<uint16_t>(size), hasMips, 1, format, flags);
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

                        if (bgfx::getCaps()->originBottomLeft)
                        {
                            FlipImage({static_cast<uint8_t*>(image->m_data), image->m_size}, image->m_height);
                        }

                        bgfx::ReleaseFn releaseFn{[](void*, void* userData) {
                            bimg::imageFree(static_cast<bimg::ImageContainer*>(userData));
                        }};

                        const bgfx::Memory* mem{bgfx::makeRef(image->m_data, image->m_size, releaseFn, image)};
                        texture->UpdateCube(0, side, mip, 0, 0, static_cast<uint16_t>(image->m_width), static_cast<uint16_t>(image->m_height), mem);
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
                            if (bgfx::getCaps()->originBottomLeft)
                            {
                                FlipImage({const_cast<uint8_t*>(imageMip.m_data), imageMip.m_size}, image->m_height);
                            }

                            bgfx::ReleaseFn releaseFn{};
                            if (mip == image->m_numMips - 1)
                            {
                                releaseFn = [](void*, void* userData) {
                                    bimg::imageFree(static_cast<bimg::ImageContainer*>(userData));
                                };
                            }

                            const bgfx::Memory* mem{bgfx::makeRef(imageMip.m_data, imageMip.m_size, releaseFn, image)};
                            texture->UpdateCube(0, side, mip, 0, 0, static_cast<uint16_t>(imageMip.m_width), static_cast<uint16_t>(imageMip.m_height), mem);
                        }
                    }
                }
            }
        }

        auto RenderTargetSamplesToBgfxMsaaFlag(uint32_t renderTargetSamples)
        {
            switch (renderTargetSamples)
            {
                case 2:
                    return BGFX_TEXTURE_RT_MSAA_X2;
                case 4:
                    return BGFX_TEXTURE_RT_MSAA_X4;
                case 8:
                    return BGFX_TEXTURE_RT_MSAA_X8;
                case 16:
                    return BGFX_TEXTURE_RT_MSAA_X16;
            }

            return BGFX_TEXTURE_NONE;
        }

        using CommandFunctionPointerT = void (NativeEngine::*)(NativeDataStream::Reader&);
    }

    void BABYLON_API NativeEngine::Initialize(Napi::Env env)
    {
        // Initialize the JavaScript side.
        Napi::HandleScope scope{env};

        auto limits = bgfx::getCaps()->limits;

        Napi::Function func = DefineClass(
            env,
            JS_CLASS_NAME,
            {
                // This must match the version in nativeEngine.ts
                StaticValue("PROTOCOL_VERSION", Napi::Number::From(env, 9)),

                StaticValue("CAPS_LIMITS_MAX_TEXTURE_SIZE", Napi::Number::From(env, limits.maxTextureSize)),
                StaticValue("CAPS_LIMITS_MAX_TEXTURE_LAYERS", Napi::Number::From(env, limits.maxTextureLayers)),

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

                StaticValue("TEXTURE_FORMAT_BC1", Napi::Number::From(env, static_cast<uint32_t>(bgfx::TextureFormat::BC1))),
                StaticValue("TEXTURE_FORMAT_BC2", Napi::Number::From(env, static_cast<uint32_t>(bgfx::TextureFormat::BC2))),
                StaticValue("TEXTURE_FORMAT_BC3", Napi::Number::From(env, static_cast<uint32_t>(bgfx::TextureFormat::BC3))),
                StaticValue("TEXTURE_FORMAT_BC4", Napi::Number::From(env, static_cast<uint32_t>(bgfx::TextureFormat::BC4))),
                StaticValue("TEXTURE_FORMAT_BC5", Napi::Number::From(env, static_cast<uint32_t>(bgfx::TextureFormat::BC5))),
                StaticValue("TEXTURE_FORMAT_BC6H", Napi::Number::From(env, static_cast<uint32_t>(bgfx::TextureFormat::BC6H))),
                StaticValue("TEXTURE_FORMAT_BC7", Napi::Number::From(env, static_cast<uint32_t>(bgfx::TextureFormat::BC7))),
                StaticValue("TEXTURE_FORMAT_ETC1", Napi::Number::From(env, static_cast<uint32_t>(bgfx::TextureFormat::ETC1))),
                StaticValue("TEXTURE_FORMAT_ETC2", Napi::Number::From(env, static_cast<uint32_t>(bgfx::TextureFormat::ETC2))),
                StaticValue("TEXTURE_FORMAT_ETC2A", Napi::Number::From(env, static_cast<uint32_t>(bgfx::TextureFormat::ETC2A))),
                StaticValue("TEXTURE_FORMAT_ETC2A1", Napi::Number::From(env, static_cast<uint32_t>(bgfx::TextureFormat::ETC2A1))),
                StaticValue("TEXTURE_FORMAT_PTC12", Napi::Number::From(env, static_cast<uint32_t>(bgfx::TextureFormat::PTC12))),
                StaticValue("TEXTURE_FORMAT_PTC14", Napi::Number::From(env, static_cast<uint32_t>(bgfx::TextureFormat::PTC14))),
                StaticValue("TEXTURE_FORMAT_PTC12A", Napi::Number::From(env, static_cast<uint32_t>(bgfx::TextureFormat::PTC12A))),
                StaticValue("TEXTURE_FORMAT_PTC14A", Napi::Number::From(env, static_cast<uint32_t>(bgfx::TextureFormat::PTC14A))),
                StaticValue("TEXTURE_FORMAT_PTC22", Napi::Number::From(env, static_cast<uint32_t>(bgfx::TextureFormat::PTC22))),
                StaticValue("TEXTURE_FORMAT_PTC24", Napi::Number::From(env, static_cast<uint32_t>(bgfx::TextureFormat::PTC24))),
                StaticValue("TEXTURE_FORMAT_ATC", Napi::Number::From(env, static_cast<uint32_t>(bgfx::TextureFormat::ATC))),
                StaticValue("TEXTURE_FORMAT_ATCE", Napi::Number::From(env, static_cast<uint32_t>(bgfx::TextureFormat::ATCE))),
                StaticValue("TEXTURE_FORMAT_ATCI", Napi::Number::From(env, static_cast<uint32_t>(bgfx::TextureFormat::ATCI))),
                StaticValue("TEXTURE_FORMAT_ASTC4x4", Napi::Number::From(env, static_cast<uint32_t>(bgfx::TextureFormat::ASTC4x4))),
                StaticValue("TEXTURE_FORMAT_ASTC5x4", Napi::Number::From(env, static_cast<uint32_t>(bgfx::TextureFormat::ASTC5x4))),
                StaticValue("TEXTURE_FORMAT_ASTC5x5", Napi::Number::From(env, static_cast<uint32_t>(bgfx::TextureFormat::ASTC5x5))),
                StaticValue("TEXTURE_FORMAT_ASTC6x5", Napi::Number::From(env, static_cast<uint32_t>(bgfx::TextureFormat::ASTC6x5))),
                StaticValue("TEXTURE_FORMAT_ASTC6x6", Napi::Number::From(env, static_cast<uint32_t>(bgfx::TextureFormat::ASTC6x6))),
                StaticValue("TEXTURE_FORMAT_ASTC8x5", Napi::Number::From(env, static_cast<uint32_t>(bgfx::TextureFormat::ASTC8x5))),
                StaticValue("TEXTURE_FORMAT_ASTC8x6", Napi::Number::From(env, static_cast<uint32_t>(bgfx::TextureFormat::ASTC8x6))),
                StaticValue("TEXTURE_FORMAT_ASTC8x8", Napi::Number::From(env, static_cast<uint32_t>(bgfx::TextureFormat::ASTC8x8))),
                StaticValue("TEXTURE_FORMAT_ASTC10x5", Napi::Number::From(env, static_cast<uint32_t>(bgfx::TextureFormat::ASTC10x5))),
                StaticValue("TEXTURE_FORMAT_ASTC10x6", Napi::Number::From(env, static_cast<uint32_t>(bgfx::TextureFormat::ASTC10x6))),
                StaticValue("TEXTURE_FORMAT_ASTC10x8", Napi::Number::From(env, static_cast<uint32_t>(bgfx::TextureFormat::ASTC10x8))),
                StaticValue("TEXTURE_FORMAT_ASTC10x10", Napi::Number::From(env, static_cast<uint32_t>(bgfx::TextureFormat::ASTC10x10))),
                StaticValue("TEXTURE_FORMAT_ASTC12x10", Napi::Number::From(env, static_cast<uint32_t>(bgfx::TextureFormat::ASTC12x10))),
                StaticValue("TEXTURE_FORMAT_ASTC12x12", Napi::Number::From(env, static_cast<uint32_t>(bgfx::TextureFormat::ASTC12x12))),

                StaticValue("TEXTURE_FORMAT_R1", Napi::Number::From(env, static_cast<uint32_t>(bgfx::TextureFormat::R1))),
                StaticValue("TEXTURE_FORMAT_A8", Napi::Number::From(env, static_cast<uint32_t>(bgfx::TextureFormat::A8))),
                StaticValue("TEXTURE_FORMAT_R8", Napi::Number::From(env, static_cast<uint32_t>(bgfx::TextureFormat::R8))),
                StaticValue("TEXTURE_FORMAT_R8I", Napi::Number::From(env, static_cast<uint32_t>(bgfx::TextureFormat::R8I))),
                StaticValue("TEXTURE_FORMAT_R8U", Napi::Number::From(env, static_cast<uint32_t>(bgfx::TextureFormat::R8U))),
                StaticValue("TEXTURE_FORMAT_R8S", Napi::Number::From(env, static_cast<uint32_t>(bgfx::TextureFormat::R8S))),
                StaticValue("TEXTURE_FORMAT_R16", Napi::Number::From(env, static_cast<uint32_t>(bgfx::TextureFormat::R16))),
                StaticValue("TEXTURE_FORMAT_R16I", Napi::Number::From(env, static_cast<uint32_t>(bgfx::TextureFormat::R16I))),
                StaticValue("TEXTURE_FORMAT_R16U", Napi::Number::From(env, static_cast<uint32_t>(bgfx::TextureFormat::R16U))),
                StaticValue("TEXTURE_FORMAT_R16F", Napi::Number::From(env, static_cast<uint32_t>(bgfx::TextureFormat::R16F))),
                StaticValue("TEXTURE_FORMAT_R16S", Napi::Number::From(env, static_cast<uint32_t>(bgfx::TextureFormat::R16S))),
                StaticValue("TEXTURE_FORMAT_R32I", Napi::Number::From(env, static_cast<uint32_t>(bgfx::TextureFormat::R32I))),
                StaticValue("TEXTURE_FORMAT_R32U", Napi::Number::From(env, static_cast<uint32_t>(bgfx::TextureFormat::R32U))),
                StaticValue("TEXTURE_FORMAT_R32F", Napi::Number::From(env, static_cast<uint32_t>(bgfx::TextureFormat::R32F))),
                StaticValue("TEXTURE_FORMAT_RG8", Napi::Number::From(env, static_cast<uint32_t>(bgfx::TextureFormat::RG8))),
                StaticValue("TEXTURE_FORMAT_RG8I", Napi::Number::From(env, static_cast<uint32_t>(bgfx::TextureFormat::RG8I))),
                StaticValue("TEXTURE_FORMAT_RG8U", Napi::Number::From(env, static_cast<uint32_t>(bgfx::TextureFormat::RG8U))),
                StaticValue("TEXTURE_FORMAT_RG8S", Napi::Number::From(env, static_cast<uint32_t>(bgfx::TextureFormat::RG8S))),
                StaticValue("TEXTURE_FORMAT_RG16", Napi::Number::From(env, static_cast<uint32_t>(bgfx::TextureFormat::RG16))),
                StaticValue("TEXTURE_FORMAT_RG16I", Napi::Number::From(env, static_cast<uint32_t>(bgfx::TextureFormat::RG16I))),
                StaticValue("TEXTURE_FORMAT_RG16U", Napi::Number::From(env, static_cast<uint32_t>(bgfx::TextureFormat::RG16U))),
                StaticValue("TEXTURE_FORMAT_RG16F", Napi::Number::From(env, static_cast<uint32_t>(bgfx::TextureFormat::RG16F))),
                StaticValue("TEXTURE_FORMAT_RG16S", Napi::Number::From(env, static_cast<uint32_t>(bgfx::TextureFormat::RG16S))),
                StaticValue("TEXTURE_FORMAT_RG32I", Napi::Number::From(env, static_cast<uint32_t>(bgfx::TextureFormat::RG32I))),
                StaticValue("TEXTURE_FORMAT_RG32U", Napi::Number::From(env, static_cast<uint32_t>(bgfx::TextureFormat::RG32U))),
                StaticValue("TEXTURE_FORMAT_RG32F", Napi::Number::From(env, static_cast<uint32_t>(bgfx::TextureFormat::RG32F))),
                StaticValue("TEXTURE_FORMAT_RGB8", Napi::Number::From(env, static_cast<uint32_t>(bgfx::TextureFormat::RGB8))),
                StaticValue("TEXTURE_FORMAT_RGB8I", Napi::Number::From(env, static_cast<uint32_t>(bgfx::TextureFormat::RGB8I))),
                StaticValue("TEXTURE_FORMAT_RGB8U", Napi::Number::From(env, static_cast<uint32_t>(bgfx::TextureFormat::RGB8U))),
                StaticValue("TEXTURE_FORMAT_RGB8S", Napi::Number::From(env, static_cast<uint32_t>(bgfx::TextureFormat::RGB8S))),
                StaticValue("TEXTURE_FORMAT_RGB9E5F", Napi::Number::From(env, static_cast<uint32_t>(bgfx::TextureFormat::RGB9E5F))),
                StaticValue("TEXTURE_FORMAT_BGRA8", Napi::Number::From(env, static_cast<uint32_t>(bgfx::TextureFormat::BGRA8))),
                StaticValue("TEXTURE_FORMAT_RGBA8", Napi::Number::From(env, static_cast<uint32_t>(bgfx::TextureFormat::RGBA8))),
                StaticValue("TEXTURE_FORMAT_RGBA8I", Napi::Number::From(env, static_cast<uint32_t>(bgfx::TextureFormat::RGBA8I))),
                StaticValue("TEXTURE_FORMAT_RGBA8U", Napi::Number::From(env, static_cast<uint32_t>(bgfx::TextureFormat::RGBA8U))),
                StaticValue("TEXTURE_FORMAT_RGBA8S", Napi::Number::From(env, static_cast<uint32_t>(bgfx::TextureFormat::RGBA8S))),
                StaticValue("TEXTURE_FORMAT_RGBA16", Napi::Number::From(env, static_cast<uint32_t>(bgfx::TextureFormat::RGBA16))),
                StaticValue("TEXTURE_FORMAT_RGBA16I", Napi::Number::From(env, static_cast<uint32_t>(bgfx::TextureFormat::RGBA16I))),
                StaticValue("TEXTURE_FORMAT_RGBA16U", Napi::Number::From(env, static_cast<uint32_t>(bgfx::TextureFormat::RGBA16U))),
                StaticValue("TEXTURE_FORMAT_RGBA16F", Napi::Number::From(env, static_cast<uint32_t>(bgfx::TextureFormat::RGBA16F))),
                StaticValue("TEXTURE_FORMAT_RGBA16S", Napi::Number::From(env, static_cast<uint32_t>(bgfx::TextureFormat::RGBA16S))),
                StaticValue("TEXTURE_FORMAT_RGBA32I", Napi::Number::From(env, static_cast<uint32_t>(bgfx::TextureFormat::RGBA32I))),
                StaticValue("TEXTURE_FORMAT_RGBA32U", Napi::Number::From(env, static_cast<uint32_t>(bgfx::TextureFormat::RGBA32U))),
                StaticValue("TEXTURE_FORMAT_RGBA32F", Napi::Number::From(env, static_cast<uint32_t>(bgfx::TextureFormat::RGBA32F))),
                StaticValue("TEXTURE_FORMAT_B5G6R5", Napi::Number::From(env, static_cast<uint32_t>(bgfx::TextureFormat::B5G6R5))),
                StaticValue("TEXTURE_FORMAT_R5G6B5", Napi::Number::From(env, static_cast<uint32_t>(bgfx::TextureFormat::R5G6B5))),
                StaticValue("TEXTURE_FORMAT_BGRA4", Napi::Number::From(env, static_cast<uint32_t>(bgfx::TextureFormat::BGRA4))),
                StaticValue("TEXTURE_FORMAT_RGBA4", Napi::Number::From(env, static_cast<uint32_t>(bgfx::TextureFormat::RGBA4))),
                StaticValue("TEXTURE_FORMAT_BGR5A1", Napi::Number::From(env, static_cast<uint32_t>(bgfx::TextureFormat::BGR5A1))),
                StaticValue("TEXTURE_FORMAT_RGB5A1", Napi::Number::From(env, static_cast<uint32_t>(bgfx::TextureFormat::RGB5A1))),
                StaticValue("TEXTURE_FORMAT_RGB10A2", Napi::Number::From(env, static_cast<uint32_t>(bgfx::TextureFormat::RGB10A2))),
                StaticValue("TEXTURE_FORMAT_RG11B10F", Napi::Number::From(env, static_cast<uint32_t>(bgfx::TextureFormat::RG11B10F))),

                StaticValue("TEXTURE_FORMAT_D16", Napi::Number::From(env, static_cast<uint32_t>(bgfx::TextureFormat::D16))),
                StaticValue("TEXTURE_FORMAT_D24", Napi::Number::From(env, static_cast<uint32_t>(bgfx::TextureFormat::D24))),
                StaticValue("TEXTURE_FORMAT_D24S8", Napi::Number::From(env, static_cast<uint32_t>(bgfx::TextureFormat::D24S8))),
                StaticValue("TEXTURE_FORMAT_D32", Napi::Number::From(env, static_cast<uint32_t>(bgfx::TextureFormat::D32))),
                StaticValue("TEXTURE_FORMAT_D16F", Napi::Number::From(env, static_cast<uint32_t>(bgfx::TextureFormat::D16F))),
                StaticValue("TEXTURE_FORMAT_D24F", Napi::Number::From(env, static_cast<uint32_t>(bgfx::TextureFormat::D24F))),
                StaticValue("TEXTURE_FORMAT_D32F", Napi::Number::From(env, static_cast<uint32_t>(bgfx::TextureFormat::D32F))),
                StaticValue("TEXTURE_FORMAT_D0S8", Napi::Number::From(env, static_cast<uint32_t>(bgfx::TextureFormat::D0S8))),

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
                StaticValue("COMMAND_UNSETTEXTURE", Napi::FunctionPointer::Create(env, &NativeEngine::UnsetTexture)),
                StaticValue("COMMAND_DISCARDALLTEXTURES", Napi::FunctionPointer::Create(env, &NativeEngine::DiscardAllTextures)),
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
                StaticValue("COMMAND_DRAWINDEXEDINSTANCED", Napi::FunctionPointer::Create(env, &NativeEngine::DrawIndexedInstanced)),
                StaticValue("COMMAND_DRAW", Napi::FunctionPointer::Create(env, &NativeEngine::Draw)),
                StaticValue("COMMAND_DRAWINSTANCED", Napi::FunctionPointer::Create(env, &NativeEngine::DrawInstanced)),
                StaticValue("COMMAND_CLEAR", Napi::FunctionPointer::Create(env, &NativeEngine::Clear)),
                StaticValue("COMMAND_SETSTENCIL", Napi::FunctionPointer::Create(env, &NativeEngine::SetStencil)),
                StaticValue("COMMAND_SETVIEWPORT", Napi::FunctionPointer::Create(env, &NativeEngine::SetViewPort)),
                StaticValue("COMMAND_SETSCISSOR", Napi::FunctionPointer::Create(env, &NativeEngine::SetScissor)),
                StaticValue("COMMAND_COPYTEXTURE", Napi::FunctionPointer::Create(env, &NativeEngine::CopyTexture)),

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
                InstanceMethod("createProgramAsync", &NativeEngine::CreateProgramAsync),
                InstanceMethod("getUniforms", &NativeEngine::GetUniforms),
                InstanceMethod("getAttributes", &NativeEngine::GetAttributes),

                InstanceMethod("createTexture", &NativeEngine::CreateTexture),
                InstanceMethod("initializeTexture", &NativeEngine::InitializeTexture),
                InstanceMethod("loadTexture", &NativeEngine::LoadTexture),
                InstanceMethod("loadRawTexture", &NativeEngine::LoadRawTexture),
                InstanceMethod("loadRawTexture2DArray", &NativeEngine::LoadRawTexture2DArray),
                InstanceMethod("loadCubeTexture", &NativeEngine::LoadCubeTexture),
                InstanceMethod("loadCubeTextureWithMips", &NativeEngine::LoadCubeTextureWithMips),
                InstanceMethod("getTextureWidth", &NativeEngine::GetTextureWidth),
                InstanceMethod("getTextureHeight", &NativeEngine::GetTextureHeight),
                InstanceMethod("deleteTexture", &NativeEngine::DeleteTexture),
                InstanceMethod("readTexture", &NativeEngine::ReadTexture),

                InstanceMethod("createImageBitmap", &NativeEngine::CreateImageBitmap),
                InstanceMethod("resizeImageBitmap", &NativeEngine::ResizeImageBitmap),

                InstanceMethod("createFrameBuffer", &NativeEngine::CreateFrameBuffer),

                InstanceMethod("getRenderWidth", &NativeEngine::GetRenderWidth),
                InstanceMethod("getRenderHeight", &NativeEngine::GetRenderHeight),
                InstanceMethod("getHardwareScalingLevel", &NativeEngine::GetHardwareScalingLevel),
                InstanceMethod("setHardwareScalingLevel", &NativeEngine::SetHardwareScalingLevel),

                InstanceMethod("setCommandDataStream", &NativeEngine::SetCommandDataStream),
                InstanceMethod("submitCommands", &NativeEngine::SubmitCommands),

                InstanceMethod("populateFrameStats", &NativeEngine::PopulateFrameStats),

                InstanceMethod("setDeviceLostCallback", &NativeEngine::SetRenderResetCallback),
            });

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
        , m_deviceContext{Graphics::DeviceContext::GetFromJavaScript(info.Env())}
        , m_update{m_deviceContext.GetUpdate("update")}
        , m_runtimeScheduler{runtime}
        , m_defaultFrameBuffer{m_deviceContext, BGFX_INVALID_HANDLE, 0, 0, true, true, true}
        , m_boundFrameBuffer{&m_defaultFrameBuffer}
        , m_boundFrameBufferNeedsRebinding{m_deviceContext, *m_cancellationSource, true}
    {
        // Set features supported by the NativeEngine from Babylon.js.
        if (!info[0].IsUndefined())
        {
            auto jsInfo = info[0].As<Napi::Object>();
            m_jsInfo.NonFloatVertexBuffers = jsInfo.Get("nonFloatVertexBuffers").As<Napi::Boolean>();
        }
    }

    NativeEngine::~NativeEngine()
    {
        Dispose();
    }

    void NativeEngine::Dispose()
    {
        m_deviceContext.SetRenderResetCallback(nullptr);

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
        const Napi::ArrayBuffer dataBuffer = info[0].As<Napi::ArrayBuffer>();
        const uint32_t dataByteOffset = info[1].As<Napi::Number>().Uint32Value();
        const uint32_t dataByteLength = info[2].As<Napi::Number>().Uint32Value();
        const bool is32Bits = info[3].As<Napi::Boolean>().Value();
        const bool dynamic = info[4].As<Napi::Boolean>().Value();

        const uint16_t flags = (is32Bits ? BGFX_BUFFER_INDEX32 : 0);
        IndexBuffer* indexBuffer = new IndexBuffer{m_deviceContext, gsl::make_span(static_cast<uint8_t*>(dataBuffer.Data()) + dataByteOffset, dataByteLength), flags, dynamic};
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

        try
        {
            vertexArray->RecordIndexBuffer(indexBuffer);
        }
        catch (std::exception& ex)
        {
            JsConsoleLogger::LogError(info.Env(), ex.what());
        }
        catch (...)
        {
            JsConsoleLogger::LogError(info.Env(), "Failed to record index buffer");
        }
    }

    void NativeEngine::UpdateDynamicIndexBuffer(const Napi::CallbackInfo& info)
    {
        IndexBuffer* indexBuffer = info[0].As<Napi::Pointer<IndexBuffer>>().Get();
        const Napi::ArrayBuffer dataBuffer = info[1].As<Napi::ArrayBuffer>();
        const uint32_t dataByteOffset = info[2].As<Napi::Number>().Uint32Value();
        const uint32_t dataByteLength = info[3].As<Napi::Number>().Uint32Value();
        const uint32_t startingIndex = info[4].As<Napi::Number>().Uint32Value();

        try
        {
            indexBuffer->Update(gsl::make_span(static_cast<uint8_t*>(dataBuffer.Data()) + dataByteOffset, dataByteLength), startingIndex);
        }
        catch (std::exception& ex)
        {
            JsConsoleLogger::LogError(info.Env(), ex.what());
        }
        catch (...)
        {
            JsConsoleLogger::LogError(info.Env(), "Failed to update index buffer");
        }
    }

    Napi::Value NativeEngine::CreateVertexBuffer(const Napi::CallbackInfo& info)
    {
        const Napi::ArrayBuffer dataBuffer = info[0].As<Napi::ArrayBuffer>();
        const uint32_t dataByteOffset = info[1].As<Napi::Number>().Uint32Value();
        const uint32_t dataByteLength = info[2].As<Napi::Number>().Uint32Value();
        const bool dynamic = info[3].As<Napi::Boolean>().Value();

        VertexBuffer* vertexBuffer = new VertexBuffer(m_deviceContext, gsl::make_span(static_cast<uint8_t*>(dataBuffer.Data()) + dataByteOffset, dataByteLength), dynamic);
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

        if (!m_jsInfo.NonFloatVertexBuffers)
        {
            auto rendererType = bgfx::getCaps()->rendererType;

            // clang-format off
            bool nonFloatVertexBuffers = !normalized
                && (rendererType == bgfx::RendererType::Direct3D11 ||
                    rendererType == bgfx::RendererType::Direct3D12 ||
                    rendererType == bgfx::RendererType::Vulkan)
                && (type == bgfx::AttribType::Int8 ||
                    type == bgfx::AttribType::Uint8 ||
                    type == bgfx::AttribType::Uint10 ||
                    type == bgfx::AttribType::Int16 ||
                    type == bgfx::AttribType::Uint16);
            // clang-format on

            if (nonFloatVertexBuffers)
            {
                JsConsoleLogger::LogError(info.Env(), "Non-normalized, non-float vertex buffer is not supported for D3D11, D3D12, and Vulkan. Please update to a newer version of Babylon.js.");
                return;
            }
        }

        try
        {
            vertexArray->RecordVertexBuffer(vertexBuffer, location, byteOffset, byteStride, numElements, type, normalized, divisor);
        }
        catch (std::exception& ex)
        {
            JsConsoleLogger::LogError(info.Env(), ex.what());
        }
        catch (...)
        {
            JsConsoleLogger::LogError(info.Env(), "Failed to record vertex buffer");
        }
    }

    void NativeEngine::UpdateDynamicVertexBuffer(const Napi::CallbackInfo& info)
    {
        VertexBuffer* vertexBuffer = info[0].As<Napi::Pointer<VertexBuffer>>().Get();
        const Napi::ArrayBuffer dataBuffer = info[1].As<Napi::ArrayBuffer>();
        const uint32_t dataByteOffset = info[2].As<Napi::Number>().Uint32Value();
        const uint32_t dataByteLength = info[3].As<Napi::Number>().Uint32Value();
        const uint32_t vertexByteOffset = info[4].IsUndefined() ? 0 : info[4].As<Napi::Number>().Uint32Value();

        try
        {
            vertexBuffer->Update(gsl::make_span(static_cast<uint8_t*>(dataBuffer.Data()) + dataByteOffset, dataByteLength), vertexByteOffset);
        }
        catch (std::exception& ex)
        {
            JsConsoleLogger::LogError(info.Env(), ex.what());
        }
        catch (...)
        {
            JsConsoleLogger::LogError(info.Env(), "Failed to update vertex buffer");
        }
    }

    Napi::Value NativeEngine::CreateProgram(const Napi::CallbackInfo& info)
    {
        const std::string vertexSource = info[0].As<Napi::String>().Utf8Value();
        const std::string fragmentSource = info[1].As<Napi::String>().Utf8Value();
        Program* program = new Program{m_deviceContext};
        Napi::Value jsProgram = Napi::Pointer<Program>::Create(info.Env(), program, Napi::NapiPointerDeleter(program));
        try
        {
            program->Initialize(m_shaderProvider.Get(vertexSource, fragmentSource));
        }
        catch (const std::exception& ex)
        {
            throw Napi::Error::New(info.Env(), ex.what());
        }
        return jsProgram;
    }

    Napi::Value NativeEngine::CreateProgramAsync(const Napi::CallbackInfo& info)
    {
        std::string vertexSource = info[0].As<Napi::String>().Utf8Value();
        std::string fragmentSource = info[1].As<Napi::String>().Utf8Value();
        const Napi::Function onSuccess = info[2].As<Napi::Function>();
        const Napi::Function onError = info[3].As<Napi::Function>();

        Program* program = new Program{m_deviceContext};
        Napi::Value jsProgram = Napi::Pointer<Program>::Create(info.Env(), program, Napi::NapiPointerDeleter(program));

        arcana::make_task(arcana::threadpool_scheduler, *m_cancellationSource,
            [this, vertexSource = std::move(vertexSource), fragmentSource = std::move(fragmentSource), program, cancellationSource{m_cancellationSource}]() {
                program->Initialize(m_shaderProvider.Get(vertexSource, fragmentSource));
            })
            .then(m_runtimeScheduler, *m_cancellationSource, [jsProgramRef{Napi::Persistent(jsProgram)}, onSuccessRef{Napi::Persistent(onSuccess)}, onErrorRef{Napi::Persistent(onError)}, cancellationSource{m_cancellationSource}](const arcana::expected<void, std::exception_ptr>& result) {
                if (result.has_error())
                {
                    onErrorRef.Call({Napi::Error::New(onErrorRef.Env(), result.error()).Value()});
                }
                else
                {
                    onSuccessRef.Call({});
                }
            });

        return jsProgram;
    }

    Napi::Value NativeEngine::GetUniforms(const Napi::CallbackInfo& info)
    {
        const Program* program = info[0].As<Napi::Pointer<Program>>().Get();
        const Napi::Array names = info[1].As<Napi::Array>();

        const auto length{names.Length()};
        auto uniforms{Napi::Array::New(info.Env(), length)};
        for (uint32_t index = 0; index < length; ++index)
        {
            if (names[index].IsString())
            {
                const auto name{names[index].As<Napi::String>().Utf8Value()};
                const UniformInfo* uniformInfo = program->GetUniformInfo(name);
                if (uniformInfo != nullptr)
                {
                    uniforms[index] = Napi::Pointer<UniformInfo>::Create(info.Env(), uniformInfo);
                    continue;
                }
            }

            uniforms[index] = info.Env().Null();
        }

        return uniforms;
    }

    Napi::Value NativeEngine::GetAttributes(const Napi::CallbackInfo& info)
    {
        const Program* program = info[0].As<Napi::Pointer<Program>>().Get();
        const Napi::Array names = info[1].As<Napi::Array>();

        const auto& attributeLocations = program->VertexAttributeLocations();

        auto length = names.Length();
        auto attributes = Napi::Array::New(info.Env(), length);
        for (uint32_t index = 0; index < length; ++index)
        {
            const std::string name = names[index].As<Napi::String>().Utf8Value();
            const auto it = attributeLocations.find(name);
            int location = (it == attributeLocations.end() ? -1 : gsl::narrow_cast<int>(it->second));
            attributes[index] = Napi::Value::From(info.Env(), location);
        }

        return attributes;
    }

    void NativeEngine::SetProgram(NativeDataStream::Reader& data)
    {
        m_currentProgram = data.ReadPointer<Program>();
    }

    void NativeEngine::SetState(NativeDataStream::Reader& data)
    {
        const bool culling = data.ReadUint32();
        // TODO: zOffset
        /*const float zOffset =*/data.ReadFloat32();
        /*const float zOffsetUnits =*/data.ReadFloat32();
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
        data.ReadPointer<Program>()->Dispose();
    }

    void NativeEngine::SetZOffset(NativeDataStream::Reader& data)
    {
        /*const auto zOffset =*/data.ReadFloat32();

        // STUB: Stub.
    }

    void NativeEngine::SetZOffsetUnits(NativeDataStream::Reader& data)
    {
        /*const auto zOffsetUnits =*/data.ReadFloat32();

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

        if constexpr (size < 3)
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
        Graphics::Texture* texture = new Graphics::Texture(m_deviceContext);
        return Napi::Pointer<Graphics::Texture>::Create(info.Env(), texture, Napi::NapiPointerDeleter(texture));
    }

    void NativeEngine::InitializeTexture(const Napi::CallbackInfo& info)
    {
        const auto texture = info[0].As<Napi::Pointer<Graphics::Texture>>().Get();
        const uint16_t width = static_cast<uint16_t>(info[1].As<Napi::Number>().Uint32Value());
        const uint16_t height = static_cast<uint16_t>(info[2].As<Napi::Number>().Uint32Value());
        const bool hasMips = info[3].As<Napi::Boolean>();
        const bgfx::TextureFormat::Enum format = static_cast<bgfx::TextureFormat::Enum>(info[4].As<Napi::Number>().Uint32Value());
        const bool renderTarget = info[5].As<Napi::Boolean>();
        const bool srgb = info[6].As<Napi::Boolean>();
        const uint32_t samples = info[7].IsUndefined() ? 1 : info[7].As<Napi::Number>().Uint32Value();

        auto flags = BGFX_TEXTURE_NONE;
        if (renderTarget)
        {
            flags |= BGFX_TEXTURE_RT | RenderTargetSamplesToBgfxMsaaFlag(samples);
        }
        if (srgb)
        {
            flags |= BGFX_TEXTURE_SRGB;
        }

        texture->Create2D(width, height, hasMips, 1, format, flags);
    }

    void NativeEngine::LoadTexture(const Napi::CallbackInfo& info)
    {
        const auto texture = info[0].As<Napi::Pointer<Graphics::Texture>>().Get();
        const auto data = info[1].As<Napi::TypedArray>();
        const auto generateMips = info[2].As<Napi::Boolean>().Value();
        const auto invertY = info[3].As<Napi::Boolean>().Value();
        const auto srgb = info[4].As<Napi::Boolean>().Value();
        const auto onSuccess = info[5].As<Napi::Function>();
        const auto onError = info[6].As<Napi::Function>();

        const auto dataSpan = gsl::make_span(static_cast<uint8_t*>(data.ArrayBuffer().Data()) + data.ByteOffset(), data.ByteLength());

        arcana::make_task(arcana::threadpool_scheduler, *m_cancellationSource,
            [dataSpan, generateMips, invertY, srgb, texture, cancellationSource{m_cancellationSource}]() {
                arcana::trace_region loadRegion{"NativeEngine::LoadTexture"};
                bimg::ImageContainer* image{ParseImage(Graphics::DeviceContext::GetDefaultAllocator(), dataSpan)};
                image = PrepareImage(Graphics::DeviceContext::GetDefaultAllocator(), image, invertY, srgb, generateMips);
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

    void NativeEngine::CopyTexture(NativeDataStream::Reader& data)
    {
        bgfx::Encoder* encoder = GetUpdateToken().GetEncoder();

        const auto textureSource = data.ReadPointer<Graphics::Texture>();
        const auto textureDestination = data.ReadPointer<Graphics::Texture>();

        GetBoundFrameBuffer(*encoder).Blit(*encoder, textureDestination->Handle(), 0, 0, textureSource->Handle());
    }

    void NativeEngine::LoadRawTexture(const Napi::CallbackInfo& info)
    {
        const auto texture{info[0].As<Napi::Pointer<Graphics::Texture>>().Get()};
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

        bimg::ImageContainer* image{bimg::imageAlloc(&Graphics::DeviceContext::GetDefaultAllocator(), format, width, height, 1, 1, false, false, bytes)};
        image = PrepareImage(Graphics::DeviceContext::GetDefaultAllocator(), image, invertY, false, generateMips);
        LoadTextureFromImage(texture, image, false);
    }

    void NativeEngine::LoadRawTexture2DArray(const Napi::CallbackInfo& info)
    {
        const auto texture{info[0].As<Napi::Pointer<Graphics::Texture>>().Get()};
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

        uint64_t flags{BGFX_TEXTURE_NONE | BGFX_SAMPLER_NONE | BGFX_CAPS_TEXTURE_2D_ARRAY};
        texture->Create2D(width, height, generateMips, depth, Cast(format), flags);

        if (!data.IsNull())
        {
            if (data.ByteLength() != bimg::imageGetSize(nullptr, width, height, 1, false, false, depth, format))
            {
                throw Napi::Error::New(Env(), "The data size does not match width, height, depth and format");
            }

            uint8_t* dataPtr = static_cast<uint8_t*>(data.ArrayBuffer().Data()) + data.ByteOffset();
            size_t dataSize = data.ByteLength();

            size_t textureSize = dataSize / static_cast<size_t>(depth);

            for (uint16_t i = 0; i < depth; i++)
            {
                uint8_t* begin = dataPtr + (textureSize * static_cast<size_t>(i));
                const bgfx::Memory* dataCopy = bgfx::copy(begin, static_cast<uint32_t>(textureSize)); // This is required since BGFX must manage the data the memory.
                texture->Update2D(i, 0, 0, 0, width, height, dataCopy);
            }
        }
    }

    void NativeEngine::LoadCubeTexture(const Napi::CallbackInfo& info)
    {
        const auto texture = info[0].As<Napi::Pointer<Graphics::Texture>>().Get();
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
            tasks[face] = arcana::make_task(arcana::threadpool_scheduler, *m_cancellationSource, [dataSpan, invertY, generateMips, srgb]() {
                bimg::ImageContainer* image{ParseImage(Graphics::DeviceContext::GetDefaultAllocator(), dataSpan)};
                image = PrepareImage(Graphics::DeviceContext::GetDefaultAllocator(), image, invertY, srgb, generateMips);
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
        const auto texture = info[0].As<Napi::Pointer<Graphics::Texture>>().Get();
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
                tasks[(face * numMips) + mip] = arcana::make_task(arcana::threadpool_scheduler, *m_cancellationSource, [dataSpan, invertY, srgb]() {
                    bimg::ImageContainer* image{ParseImage(Graphics::DeviceContext::GetDefaultAllocator(), dataSpan)};
                    image = PrepareImage(Graphics::DeviceContext::GetDefaultAllocator(), image, invertY, srgb, false);
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
        const Graphics::Texture* texture = info[0].As<Napi::Pointer<Graphics::Texture>>().Get();
        return Napi::Value::From(info.Env(), texture->Width());
    }

    Napi::Value NativeEngine::GetTextureHeight(const Napi::CallbackInfo& info)
    {
        const Graphics::Texture* texture = info[0].As<Napi::Pointer<Graphics::Texture>>().Get();
        return Napi::Value::From(info.Env(), texture->Height());
    }

    void NativeEngine::SetTextureSampling(NativeDataStream::Reader& data)
    {
        auto& texture = *data.ReadPointer<Graphics::Texture>();
        const auto value = data.ReadUint32();

        uint32_t flags = texture.SamplerFlags();

        flags &= ~(BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT | BGFX_SAMPLER_MIP_POINT);
        flags |= value;

        // Disable anisotropy if either min/mag are point.
        if ((flags & BGFX_SAMPLER_MIN_POINT) != 0 || (flags & BGFX_SAMPLER_MAG_POINT) != 0)
        {
            flags &= ~(BGFX_SAMPLER_MIN_ANISOTROPIC | BGFX_SAMPLER_MAG_ANISOTROPIC);
        }

        texture.SamplerFlags(flags);
    }

    void NativeEngine::SetTextureWrapMode(NativeDataStream::Reader& data)
    {
        auto& texture = *data.ReadPointer<Graphics::Texture>();
        auto addressModeU = data.ReadUint32();
        auto addressModeV = data.ReadUint32();
        auto addressModeW = data.ReadUint32();

        uint32_t addressMode = addressModeU +
                               (addressModeV << BGFX_SAMPLER_V_SHIFT) +
                               (addressModeW << BGFX_SAMPLER_W_SHIFT);

        uint32_t flags = texture.SamplerFlags();
        flags &= ~(BGFX_SAMPLER_U_MASK | BGFX_SAMPLER_V_MASK | BGFX_SAMPLER_W_MASK);
        flags |= addressMode;
        texture.SamplerFlags(flags);
    }

    void NativeEngine::SetTextureAnisotropicLevel(NativeDataStream::Reader& data)
    {
        auto& texture = *data.ReadPointer<Graphics::Texture>();
        const auto value = data.ReadUint32();

        uint32_t flags = texture.SamplerFlags();

        flags &= ~(BGFX_SAMPLER_MIN_ANISOTROPIC | BGFX_SAMPLER_MAG_ANISOTROPIC);

        // Enable anisotropy only if neither min/mag are point.
        // Note that bgfx currently only supports no anisotropy or max anisotropy.
        if ((flags & BGFX_SAMPLER_MIN_POINT) == 0 && (flags & BGFX_SAMPLER_MAG_POINT) == 0 && value > 1)
        {
            flags |= BGFX_SAMPLER_MIN_ANISOTROPIC | BGFX_SAMPLER_MAG_ANISOTROPIC;
        }

        texture.SamplerFlags(flags);
    }

    void NativeEngine::SetTexture(NativeDataStream::Reader& data)
    {
        bgfx::Encoder* encoder = GetUpdateToken().GetEncoder();

        const UniformInfo* uniformInfo = data.ReadPointer<UniformInfo>();
        const Graphics::Texture* texture = data.ReadPointer<Graphics::Texture>();

        encoder->setTexture(uniformInfo->Stage, uniformInfo->Handle, texture->Handle(), texture->SamplerFlags());
    }

    void NativeEngine::UnsetTexture(NativeDataStream::Reader& data)
    {
        bgfx::Encoder* encoder = GetUpdateToken().GetEncoder();

        const UniformInfo* uniformInfo = data.ReadPointer<UniformInfo>();

        encoder->setTexture(uniformInfo->Stage, uniformInfo->Handle, BGFX_INVALID_HANDLE);
    }

    void NativeEngine::DiscardAllTextures(NativeDataStream::Reader&)
    {
        bgfx::Encoder* encoder = GetUpdateToken().GetEncoder();
        encoder->discard(BGFX_DISCARD_BINDINGS);
    }

    void NativeEngine::DeleteTexture(const Napi::CallbackInfo& info)
    {
        Graphics::Texture* texture = info[0].As<Napi::Pointer<Graphics::Texture>>().Get();
        m_deviceContext.RemoveTexture(texture->Handle());
        texture->Dispose();
    }

    Napi::Value NativeEngine::ReadTexture(const Napi::CallbackInfo& info)
    {
        const Napi::Env env{info.Env()};

        Graphics::Texture* texture{info[0].As<Napi::Pointer<Graphics::Texture>>().Get()};
        uint8_t mipLevel{static_cast<uint8_t>(info[1].As<Napi::Number>().Uint32Value())};
        const uint16_t x{static_cast<uint16_t>(info[2].As<Napi::Number>().Uint32Value())};
        const uint16_t y{static_cast<uint16_t>(info[3].As<Napi::Number>().Uint32Value())};
        const uint16_t width{static_cast<uint16_t>(info[4].As<Napi::Number>().Uint32Value())};
        const uint16_t height{static_cast<uint16_t>(info[5].As<Napi::Number>().Uint32Value())};
        auto buffer{info[6].As<Napi::ArrayBuffer>()};
        uint32_t bufferOffset{info[7].As<Napi::Number>().Uint32Value()};
        uint32_t bufferLength{info[8].As<Napi::Number>().Uint32Value()};

        const auto deferred{Napi::Promise::Deferred::New(env)};

        // Calculate source texture storage size.
        const auto sourceTextureFormat{texture->Format()};
        bgfx::TextureInfo sourceTextureInfo{};
        bgfx::calcTextureSize(sourceTextureInfo, width, height, /*depth*/ 1, /*cubeMap*/ false, /*hasMips*/ false, /*numLayers*/ 1, sourceTextureFormat);

        // Calculate target texture storage size.
        // Always return pixel data in RBGA8 to match the web.
        const auto targetTextureFormat{bgfx::TextureFormat::Enum::RGBA8};
        bgfx::TextureInfo targetTextureInfo{};
        bgfx::calcTextureSize(targetTextureInfo, width, height, /*depth*/ 1, /*cubeMap*/ false, /*hasMips*/ false, /*numLayers*/ 1, targetTextureFormat);

        // Create the output buffer if one wasn't passed in.
        if (buffer.IsNull())
        {
            bufferOffset = 0;
            bufferLength = targetTextureInfo.storageSize;
            buffer = Napi::ArrayBuffer::New(env, bufferLength);
        }

        // Make sure the buffer is big enough for the offset + length.
        if (buffer.ByteLength() < bufferOffset + bufferLength)
        {
            deferred.Reject(Napi::Error::New(env, "Provided buffer is too small for the specified offset and length.").Value());
        }
        // Make sure the buffer is big enough to fit the output data.
        else if (targetTextureInfo.storageSize > bufferLength)
        {
            deferred.Reject(Napi::Error::New(env, "Provided buffer is too small to contain the pixel data.").Value());
        }
        else
        {
            bgfx::TextureHandle sourceTextureHandle{texture->Handle()};
            auto tempTexture = std::make_shared<bool>(false);

            // If the image needs to be cropped (not starting at 0, or less than full width/height (accounting for requested mip level)),
            // or if the texture was not created with the BGFX_TEXTURE_READ_BACK flag, then blit it to a temp texture.
            if (x != 0 || y != 0 || width != (texture->Width() >> mipLevel) || height != (texture->Height() >> mipLevel) || (texture->Flags() & BGFX_TEXTURE_READ_BACK) == 0)
            {
                const bgfx::TextureHandle blitTextureHandle{bgfx::createTexture2D(width, height, /*hasMips*/ false, /*numLayers*/ 1, sourceTextureFormat, BGFX_TEXTURE_BLIT_DST | BGFX_TEXTURE_READ_BACK)};
                bgfx::Encoder* encoder{GetUpdateToken().GetEncoder()};
                encoder->blit(static_cast<uint16_t>(bgfx::getCaps()->limits.maxViews - 1), blitTextureHandle, /*dstMip*/ 0, /*dstX*/ 0, /*dstY*/ 0, /*dstZ*/ 0, sourceTextureHandle, mipLevel, x, y, /*srcZ*/ 0, width, height, /*depth*/ 0);

                sourceTextureHandle = blitTextureHandle;
                *tempTexture = true;

                // The requested mip level was blitted, so the source texture now has just one mip, so reset the mip level to 0.
                mipLevel = 0;
            }

            // Allocate a buffer to store the source pixel data.
            std::vector<uint8_t> textureBuffer(sourceTextureInfo.storageSize);

            // Read the source texture.
            m_deviceContext.ReadTextureAsync(sourceTextureHandle, textureBuffer, mipLevel)
                .then(arcana::inline_scheduler, *m_cancellationSource, [textureBuffer{std::move(textureBuffer)}, sourceTextureInfo, targetTextureInfo]() mutable {
                    // If the source texture format does not match the target texture format, convert it.
                    if (targetTextureInfo.format != sourceTextureInfo.format)
                    {
                        std::vector<uint8_t> convertedTextureBuffer(targetTextureInfo.storageSize);
                        if (!bimg::imageConvert(&Graphics::DeviceContext::GetDefaultAllocator(), convertedTextureBuffer.data(), bimg::TextureFormat::Enum(targetTextureInfo.format), textureBuffer.data(), bimg::TextureFormat::Enum(sourceTextureInfo.format), sourceTextureInfo.width, sourceTextureInfo.height, /*depth*/ 1))
                        {
                            throw std::runtime_error{"Texture conversion to RBGA8 failed."};
                        }
                        textureBuffer = convertedTextureBuffer;
                    }

                    // Ensure the final texture buffer has the expected size.
                    assert(textureBuffer.size() == targetTextureInfo.storageSize);

                    // Flip the image vertically if needed.
                    if (bgfx::getCaps()->originBottomLeft)
                    {
                        FlipImage(textureBuffer, targetTextureInfo.height);
                    }

                    return textureBuffer;
                })
                .then(m_runtimeScheduler, *m_cancellationSource, [this, bufferRef{Napi::Persistent(buffer)}, bufferOffset, deferred, tempTexture, sourceTextureHandle](std::vector<uint8_t> textureBuffer) mutable {
                    // Double check the destination buffer length. This is redundant with prior checks, but we'll be extra sure before the memcpy.
                    assert(bufferRef.Value().ByteLength() - bufferOffset >= textureBuffer.size());

                    // Copy the pixel data into the JS ArrayBuffer.
                    uint8_t* buffer{static_cast<uint8_t*>(bufferRef.Value().Data())};
                    std::memcpy(buffer + bufferOffset, textureBuffer.data(), textureBuffer.size());

                    // Dispose of the texture handle before resolving the promise.
                    // TODO: Handle properly handle stale handles after BGFX shutdown
                    if (*tempTexture && !m_cancellationSource->cancelled())
                    {
                        bgfx::destroy(sourceTextureHandle);
                        *tempTexture = false;
                    }

                    deferred.Resolve(bufferRef.Value());
                })
                .then(m_runtimeScheduler, arcana::cancellation::none(), [this, env, deferred, tempTexture, sourceTextureHandle](const arcana::expected<void, std::exception_ptr>& result) {
                    // Dispose of the texture handle if not yet disposed.
                    // TODO: Handle properly handle stale handles after BGFX shutdown
                    if (*tempTexture && !m_cancellationSource->cancelled())
                    {
                        bgfx::destroy(sourceTextureHandle);
                    }

                    if (result.has_error())
                    {
                        deferred.Reject(Napi::Error::New(env, result.error()).Value());
                    }
                });
        }

        return deferred.Promise();
    }

    Napi::Value NativeEngine::CreateFrameBuffer(const Napi::CallbackInfo& info)
    {
        Graphics::Texture* texture = info[0].IsNull() ? nullptr : info[0].As<Napi::Pointer<Graphics::Texture>>().Get();
        const uint16_t width = static_cast<uint16_t>(info[1].As<Napi::Number>().Uint32Value());
        const uint16_t height = static_cast<uint16_t>(info[2].As<Napi::Number>().Uint32Value());
        const bool generateStencilBuffer = info[3].As<Napi::Boolean>();
        const bool generateDepth = info[4].As<Napi::Boolean>();
        const uint32_t samples = info[5].IsUndefined() ? 1 : info[5].As<Napi::Number>().Uint32Value();

        std::array<bgfx::Attachment, 2> attachments{};
        uint8_t numAttachments = 0;

        if (texture != nullptr)
        {
            attachments[numAttachments++].init(texture->Handle());
        }

        bgfx::TextureHandle depthStencilTextureHandle = BGFX_INVALID_HANDLE;
        int8_t depthStencilAttachmentIndex = -1;
        if (generateStencilBuffer || generateDepth)
        {
            if (generateStencilBuffer && !generateDepth)
            {
                JsConsoleLogger::LogWarn(info.Env(), "Stencil without depth is not supported, assuming depth and stencil");
            }

            auto flags = BGFX_TEXTURE_RT_WRITE_ONLY | RenderTargetSamplesToBgfxMsaaFlag(samples);
#ifdef ANDROID
            // On Android with Mali GPU (Oppo Find x5 lite, Google Pixel 8, Samsung Galaxy Tab Active 3, ...)
            // D32 depth buffer gives glitches. Everything is fine with D24S8.
            // see https://forum.babylonjs.com/t/post-processing-graphics-glitch/49523
            // As 24bits should be enough for 99.99% cases, defaulting to that format on Android.
            const auto depthStencilFormat{bgfx::TextureFormat::D24S8};
#else
            const auto depthStencilFormat{generateStencilBuffer ? bgfx::TextureFormat::D24S8 : bgfx::TextureFormat::D32};
#endif
            assert(bgfx::isTextureValid(0, false, 1, depthStencilFormat, flags));
            depthStencilTextureHandle = bgfx::createTexture2D(width, height, false, 1, depthStencilFormat, flags);

            // bgfx doesn't add flag D3D11_RESOURCE_MISC_GENERATE_MIPS for depth textures (missing that flag will crash D3D with resolving)
            // And not sure it makes sense to generate mipmaps from a depth buffer with exponential values.
            // only allows mipmaps resolve step when mipmapping is asked and for the color texture, not the depth.
            // https://github.com/bkaradzic/bgfx/blob/2c21f68998595fa388e25cb6527e82254d0e9bff/src/renderer_d3d11.cpp#L4525
            depthStencilAttachmentIndex = numAttachments;
            attachments[numAttachments++].init(depthStencilTextureHandle);
        }

        bgfx::FrameBufferHandle frameBufferHandle = bgfx::createFrameBuffer(numAttachments, attachments.data());
        if (!bgfx::isValid(frameBufferHandle))
        {
            if (bgfx::isValid(depthStencilTextureHandle))
            {
                bgfx::destroy(depthStencilTextureHandle);
            }

            throw Napi::Error::New(info.Env(), "Failed to create frame buffer");
        }

        Graphics::FrameBuffer* frameBuffer = new Graphics::FrameBuffer(m_deviceContext, frameBufferHandle, width, height, false, generateDepth, generateStencilBuffer, depthStencilAttachmentIndex);
        return Napi::Pointer<Graphics::FrameBuffer>::Create(info.Env(), frameBuffer, Napi::NapiPointerDeleter(frameBuffer));
    }

    // TODO: This doesn't get called when an Engine instance is disposed.
    void NativeEngine::DeleteFrameBuffer(NativeDataStream::Reader& data)
    {
        data.ReadPointer<Graphics::FrameBuffer>()->Dispose();
    }

    void NativeEngine::BindFrameBuffer(NativeDataStream::Reader& data)
    {
        auto encoder = GetUpdateToken().GetEncoder();

        Graphics::FrameBuffer* frameBuffer = data.ReadPointer<Graphics::FrameBuffer>();
        m_boundFrameBuffer->Unbind(*encoder);
        m_boundFrameBuffer = frameBuffer;
        m_boundFrameBuffer->Bind(*encoder);
        m_boundFrameBufferNeedsRebinding.Set(*encoder, false);
    }

    void NativeEngine::UnbindFrameBuffer(NativeDataStream::Reader& data)
    {
        bgfx::Encoder* encoder = GetUpdateToken().GetEncoder();

        const Graphics::FrameBuffer* frameBuffer = data.ReadPointer<Graphics::FrameBuffer>();

        assert(m_boundFrameBuffer == frameBuffer);
        UNUSED(frameBuffer);

        m_boundFrameBuffer->Unbind(*encoder);
        m_boundFrameBuffer = nullptr;
        m_boundFrameBufferNeedsRebinding.Set(*encoder, false);
    }

    // Note: For legacy reasons JS might call this function for instance drawing.
    // In that case the instanceCount will be calculated inside the SetVertexBuffers method.
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

    void NativeEngine::DrawIndexedInstanced(NativeDataStream::Reader& data)
    {
        bgfx::Encoder* encoder{GetUpdateToken().GetEncoder()};

        const uint32_t fillMode = data.ReadUint32();
        const uint32_t indexStart = data.ReadUint32();
        const uint32_t indexCount = data.ReadUint32();
        const uint32_t instanceCount = data.ReadUint32();

        if (m_boundVertexArray != nullptr)
        {
            m_boundVertexArray->SetIndexBuffer(encoder, indexStart, indexCount);
            m_boundVertexArray->SetVertexBuffers(encoder, 0, std::numeric_limits<uint32_t>::max(), instanceCount);
        }

        DrawInternal(encoder, fillMode);
    }

    // Note: For legacy reasons JS might call this function for instance drawing.
    // In that case the instanceCount will be calculated inside the SetVertexBuffers method.
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

    void NativeEngine::DrawInstanced(NativeDataStream::Reader& data)
    {
        bgfx::Encoder* encoder{GetUpdateToken().GetEncoder()};

        const uint32_t fillMode = data.ReadUint32();
        const uint32_t verticesStart = data.ReadUint32();
        const uint32_t verticesCount = data.ReadUint32();
        const uint32_t instanceCount = data.ReadUint32();

        if (m_boundVertexArray != nullptr)
        {
            m_boundVertexArray->SetVertexBuffers(encoder, verticesStart, verticesCount, instanceCount);
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
        return Napi::Value::From(info.Env(), std::floor(m_deviceContext.GetWidth() / m_deviceContext.GetHardwareScalingLevel()));
    }

    Napi::Value NativeEngine::GetRenderHeight(const Napi::CallbackInfo& info)
    {
        return Napi::Value::From(info.Env(), std::floor(m_deviceContext.GetHeight() / m_deviceContext.GetHardwareScalingLevel()));
    }

    Napi::Value NativeEngine::GetHardwareScalingLevel(const Napi::CallbackInfo& info)
    {
        return Napi::Value::From(info.Env(), m_deviceContext.GetHardwareScalingLevel());
    }

    void NativeEngine::SetHardwareScalingLevel(const Napi::CallbackInfo& info)
    {
        const auto level = info[0].As<Napi::Number>().FloatValue();
        m_deviceContext.SetHardwareScalingLevel(level);
    }

    Napi::Value NativeEngine::CreateImageBitmap(const Napi::CallbackInfo& info)
    {
        const Napi::Env env{info.Env()};
        bimg::ImageContainer* image{nullptr};
        bool allocatedImage{false};

        // CreateImageBitmap supports passing in either an ArrayBuffer or an Image object.
        if (info[0].IsArrayBuffer())
        {
            const auto data{info[0].As<Napi::ArrayBuffer>()};

            if (!data.ByteLength())
            {
                throw Napi::Error::New(env, "CreateImageBitmap array buffer is empty.");
            }

            image = ParseImage(Graphics::DeviceContext::GetDefaultAllocator(), gsl::make_span(static_cast<uint8_t*>(data.Data()), data.ByteLength()));
            allocatedImage = true;
        }
        else if (info[0].IsObject())
        {
            // If this is an object, then check if it has the _imageContainer property defined.
            auto imageObject = info[0].As<Napi::Object>();
            if (imageObject.Has("_imageContainer"))
            {
                const auto napiImageContainer = imageObject.Get("_imageContainer");
                if (!napiImageContainer.IsNull())
                {
                    const auto napiPointer = napiImageContainer.As<Napi::Pointer<bimg::ImageContainer>>();
                    image = napiPointer.Get();
                }
            }
        }

        if (image == nullptr)
        {
            throw Napi::Error::New(env, "CreateImageBitmap parameter is not an array buffer or Image object.");
        }

        Napi::Object imageBitmap{Napi::Object::New(env)};
        Napi::Uint8Array buffer{Napi::Uint8Array::New(env, image->m_size)};
        std::memcpy(buffer.Data(), image->m_data, image->m_size);

        imageBitmap.Set("data", buffer);
        imageBitmap.Set("width", Napi::Value::From(env, image->m_width));
        imageBitmap.Set("height", Napi::Value::From(env, image->m_height));
        imageBitmap.Set("depth", Napi::Value::From(env, image->m_depth));
        imageBitmap.Set("numLayers", Napi::Value::From(env, image->m_numLayers));
        imageBitmap.Set("format", Napi::Value::From(env, static_cast<uint32_t>(image->m_format)));

        // Clean up the image if we allocated it from an ArrayBuffer.
        if (allocatedImage)
        {
            bimg::imageFree(image);
        }

        return imageBitmap;
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

        bimg::ImageContainer* image = bimg::imageAlloc(&Graphics::DeviceContext::GetDefaultAllocator(), format, static_cast<uint16_t>(width), static_cast<uint16_t>(height), 1, 1, false, false, data.Data());
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
            bimg::ImageContainer* rgba = bimg::imageConvert(&Graphics::DeviceContext::GetDefaultAllocator(), bimg::TextureFormat::RGBA8, *image, false);
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

    void NativeEngine::SetRenderResetCallback(const Napi::CallbackInfo& info)
    {
        const auto callback{info[0].As<Napi::Function>()};
        auto callbackPtr{std::make_shared<Napi::FunctionReference>(Napi::Persistent(callback))};

        m_deviceContext.SetRenderResetCallback([this, renderResetCallback = std::move(callbackPtr)]() {
            m_runtime.Dispatch([renderResetCallback = std::move(renderResetCallback)](auto) {
                renderResetCallback->Call({});
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

    void NativeEngine::SetViewPort(NativeDataStream::Reader& data)
    {
        bgfx::Encoder* encoder{GetUpdateToken().GetEncoder()};

        const float x{data.ReadFloat32()};
        const float y{data.ReadFloat32()};
        const float width{data.ReadFloat32()};
        const float height{data.ReadFloat32()};
        const float yOrigin = bgfx::getCaps()->originBottomLeft ? y : (1.f - y - height);

        GetBoundFrameBuffer(*encoder).SetViewPort(*encoder, x, yOrigin, width, height);
    }

    void NativeEngine::SetScissor(NativeDataStream::Reader& data)
    {
        bgfx::Encoder* encoder{GetUpdateToken().GetEncoder()};

        const float x{data.ReadFloat32()};
        const float y{data.ReadFloat32()};
        const float width{data.ReadFloat32()};
        const float height{data.ReadFloat32()};

        GetBoundFrameBuffer(*encoder).SetScissor(*encoder, x, y, width, height);
    }

    void NativeEngine::SetCommandDataStream(const Napi::CallbackInfo& info)
    {
        // TODO: This should be moved to the constructor once multi-update is available.
        Napi::Object jsCommandStream = info[0].ToObject();
        m_commandStream = Napi::ObjectWrap<NativeDataStream>::Unwrap(jsCommandStream.Get("_nativeDataStream").As<Napi::Object>());
    }

    void NativeEngine::SubmitCommands(const Napi::CallbackInfo& info)
    {
        try
        {
            NativeDataStream::Reader reader = m_commandStream->GetReader();
            while (reader.CanRead())
            {
                std::invoke(reader.ReadPointer<CommandFunctionPointerT>(), this, reader);
            }
        }
        catch (const std::exception& exception)
        {
            throw Napi::Error::New(info.Env(), exception);
        }
    }

    void NativeEngine::PopulateFrameStats(const Napi::CallbackInfo& info)
    {
        const auto updateToken{m_update.GetUpdateToken()};
        const auto stats{bgfx::getStats()};
        const double toGpuNs = 1000000000.0 / double(stats->gpuTimerFreq);
        const double gpuTimeNs = (stats->gpuTimeEnd - stats->gpuTimeBegin) * toGpuNs;
        Napi::Object jsStatsObject = info[0].As<Napi::Object>();
        jsStatsObject.Set("gpuTimeNs", gpuTimeNs);
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

        for (const auto& it : m_currentProgram->Uniforms())
        {
            const UniformValue& value = it.second;
            encoder->setUniform({it.first}, value.Data.data(), value.ElementLength);
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

        boundFrameBuffer.SetStencil(*encoder, m_stencilState);

        // Discard everything except textures since we keep the state of everything else.
        boundFrameBuffer.Submit(*encoder, m_currentProgram->Handle(), BGFX_DISCARD_ALL & ~BGFX_DISCARD_BINDINGS);
    }

    Graphics::UpdateToken& NativeEngine::GetUpdateToken()
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

    Graphics::FrameBuffer& NativeEngine::GetBoundFrameBuffer(bgfx::Encoder& encoder)
    {
        if (m_boundFrameBuffer == nullptr)
        {
            m_boundFrameBuffer = &m_defaultFrameBuffer;
            m_defaultFrameBuffer.Bind(encoder);
        }
        else if (m_boundFrameBufferNeedsRebinding.Get(encoder))
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
