#include "NativeEngine.h"
#include "ShaderCompiler.h"
#include <arcana/threading/task.h>
#include <arcana/threading/task_schedulers.h>
#include <arcana/macros.h>

#include <napi/env.h>

#include <bgfx/bgfx.h>

#include <bimg/bimg.h>
#include <bimg/decode.h>
#include <bimg/encode.h>

// STB_IMAGE_RESIZE_IMPLEMENTATION will define implementation in stb_image_resize.h. Many .cpp can include it 
// but only one can define implementation or linking errors will popup.
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include <stb/stb_image_resize.h>
#include <bx/math.h>

#include <queue>
#include <regex>
#include <sstream>
#include <variant>

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
            constexpr uint32_t SAMPLER_MIP_IGNORE = 0; // HACK: bgfx has no support for ignoring mips

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
    }

    template<typename Handle1T, typename Handle2T>
    class VariantHandleHolder
    {
    public:
        std::variant<Handle1T, Handle2T> m_handle{};

        template<typename NonDynamicCallableT, typename DynamicCallableT>
        void DoForHandleTypes(NonDynamicCallableT& nonDynamicCallable, DynamicCallableT& dynamicCallable) const
        {
            if (auto handle = std::get_if<Handle1T>(&m_handle))
            {
                nonDynamicCallable(*handle);
            }
            else
            {
                dynamicCallable(std::get<Handle2T>(m_handle));
            }
        }
    };

    class IndexBufferData final : private VariantHandleHolder<bgfx::IndexBufferHandle, bgfx::DynamicIndexBufferHandle>
    {
    public:
        IndexBufferData(const Napi::TypedArray& bytes, uint16_t flags, bool dynamic)
        {
            const bgfx::Memory* memory = bgfx::copy(bytes.As<Napi::Uint8Array>().Data(), static_cast<uint32_t>(bytes.ByteLength()));
            if (!dynamic)
            {
                m_handle = bgfx::createIndexBuffer(memory, flags);
            }
            else
            {
                m_handle = bgfx::createDynamicIndexBuffer(memory, flags);
            }
        }

        ~IndexBufferData()
        {
            constexpr auto nonDynamic = [](auto handle) {
                bgfx::destroy(handle);
            };
            constexpr auto dynamic = [](auto handle) {
                bgfx::destroy(handle);
            };
            DoForHandleTypes(nonDynamic, dynamic);
        }

        void Update(Napi::Env env, const Napi::TypedArray& bytes, uint32_t startingIdx)
        {
            const bgfx::Memory* memory = bgfx::copy(bytes.As<Napi::Uint8Array>().Data(), static_cast<uint32_t>(bytes.ByteLength()));

            auto nonDynamic = [env](auto) {
                throw Napi::Error::New(env, "Cannot update a non-dynamic index buffer.");
            };
            const auto dynamic = [memory, startingIdx](auto handle) {
                bgfx::update(handle, startingIdx, memory);
            };
            DoForHandleTypes(nonDynamic, dynamic);
        }

        void SetBgfxIndexBuffer(bgfx::Encoder* encoder, uint32_t firstIndex, uint32_t numIndices) const
        {
            const auto nonDynamic = [&encoder, firstIndex, numIndices](auto handle) {
                encoder->setIndexBuffer(handle, firstIndex, numIndices);
            };
            const auto dynamic = [&encoder, firstIndex, numIndices](auto handle) {
                encoder->setIndexBuffer(handle, firstIndex, numIndices);
            };
            DoForHandleTypes(nonDynamic, dynamic);
        }
    };

    class VertexBufferData final : VariantHandleHolder<bgfx::VertexBufferHandle, bgfx::DynamicVertexBufferHandle>
    {
    public:
        VertexBufferData(const Napi::Uint8Array& bytes, bool dynamic)
            : m_bytes{bytes.Data(), bytes.Data() + bytes.ByteLength()}
        {
            if (!dynamic)
            {
                m_handle = bgfx::VertexBufferHandle{bgfx::kInvalidHandle};
            }
            else
            {
                m_handle = bgfx::DynamicVertexBufferHandle{bgfx::kInvalidHandle};
            }
        }

        ~VertexBufferData()
        {
            constexpr auto nonDynamic = [](auto handle) {
                if (handle.idx != bgfx::kInvalidHandle)
                {
                    bgfx::destroy(handle);
                }
            };
            constexpr auto dynamic = [](auto handle) {
                if (handle.idx != bgfx::kInvalidHandle)
                {
                    bgfx::destroy(handle);
                }
            };
            DoForHandleTypes(nonDynamic, dynamic);
        }

        template<typename sourceType> void PromoteToFloats(uint32_t numElements, uint32_t byteOffset, uint32_t byteStride)
        {
            const size_t count = m_bytes.size() / byteStride;
            const size_t destinationSize = count * numElements * sizeof(float);
            if (destinationSize != m_bytes.size()) // ensure both vectors have different size
            {
                std::vector<uint8_t> bytes(destinationSize);
                float* destination = reinterpret_cast<float*>(bytes.data());
                for (size_t i = 0; i < count; i++)
                {
                    sourceType* source = reinterpret_cast<sourceType*>(m_bytes.data() + byteOffset + byteStride * i);
                    for (size_t element = 0; element < numElements; element++)
                    {
                        *destination++ = static_cast<float>(*source++);
                    }
                }
                m_bytes = std::move(bytes);
            }
        }

        void PromoteToFloats(bgfx::AttribType::Enum attribType, uint32_t numElements, uint32_t byteOffset, uint32_t byteStride)
        {
            switch (attribType)
            {
            case bgfx::AttribType::Int8:
                PromoteToFloats<int8_t>(numElements, byteOffset, byteStride);
                break;
            case bgfx::AttribType::Uint8:
                PromoteToFloats<uint8_t>(numElements, byteOffset, byteStride);
                break;
            case bgfx::AttribType::Int16:
                PromoteToFloats<int16_t>(numElements, byteOffset, byteStride);
                break;
            case bgfx::AttribType::Uint16:
                PromoteToFloats<uint16_t>(numElements, byteOffset, byteStride);
                break;
            case bgfx::AttribType::Uint10: // is supported by any format ?
            default:
                throw std::runtime_error("Unable to promote vertex stream to a float array.");
            }
        }

        void EnsureFinalized(Napi::Env /*env*/, const bgfx::VertexLayout& layout)
        {
            const auto nonDynamic = [&layout, this](auto handle) {
                if (handle.idx != bgfx::kInvalidHandle)
                {
                    return;
                }

                const bgfx::Memory* memory = bgfx::makeRef(
                    m_bytes.data(), static_cast<uint32_t>(m_bytes.size()), [](void*, void* userData) {
                        auto* bytes = reinterpret_cast<std::vector<uint8_t>*>(userData);
                        bytes->clear();
                    },
                    &m_bytes);

                m_handle = bgfx::createVertexBuffer(memory, layout);
            };
            const auto dynamic = [&layout, this](auto handle) {
                if (handle.idx != bgfx::kInvalidHandle)
                {
                    return;
                }

                const bgfx::Memory* memory = bgfx::makeRef(
                    m_bytes.data(), static_cast<uint32_t>(m_bytes.size()), [](void*, void* userData) {
                        auto* bytes = reinterpret_cast<std::vector<uint8_t>*>(userData);
                        bytes->clear();
                    },
                    &m_bytes);

                m_handle = bgfx::createDynamicVertexBuffer(memory, layout);
            };
            DoForHandleTypes(nonDynamic, dynamic);
        }

        void Update(Napi::Env env, const Napi::Uint8Array& bytes, uint32_t offset, uint32_t byteLength)
        {
            auto nonDynamic = [env](auto) {
                throw Napi::Error::New(env, "Cannot update non-dynamic vertex buffer.");
            };
            const auto dynamic = [&bytes, offset, byteLength, this](auto handle) {
                if (handle.idx == bgfx::kInvalidHandle)
                {
                    // Buffer hasn't been finalized yet, all that's necessary is to swap out the bytes.
                    m_bytes = {bytes.Data() + offset, bytes.Data() + offset + byteLength};
                }
                else
                {
                    // Buffer was already created, do a real update operation.
                    const bgfx::Memory* memory = bgfx::copy(bytes.Data() + offset, byteLength);
                    bgfx::update(handle, 0, memory);
                }
            };
            DoForHandleTypes(nonDynamic, dynamic);
        }

        void SetAsBgfxVertexBuffer(bgfx::Encoder* encoder, uint8_t index, uint32_t startVertex, uint32_t numVertices, bgfx::VertexLayoutHandle layout) const
        {
            const auto nonDynamic = [&encoder, index, startVertex, numVertices, layout](auto handle) {
                encoder->setVertexBuffer(index, handle, startVertex, numVertices, layout);
            };
            const auto dynamic = [&encoder, index, startVertex, numVertices, layout](auto handle) {
                encoder->setVertexBuffer(index, handle, startVertex, numVertices, layout);
            };
            DoForHandleTypes(nonDynamic, dynamic);
        }

    private:
        std::vector<uint8_t> m_bytes{};
    };

    void NativeEngine::Initialize(Napi::Env env)
    {
        // Initialize the JavaScript side.
        Napi::HandleScope scope{env};

        // clang-format off
        Napi::Function func = DefineClass(
            env,
            JS_CLASS_NAME,
            {
                InstanceMethod("dispose", &NativeEngine::Dispose),
                InstanceMethod("requestAnimationFrame", &NativeEngine::RequestAnimationFrame),
                InstanceMethod("createVertexArray", &NativeEngine::CreateVertexArray),
                InstanceMethod("deleteVertexArray", &NativeEngine::DeleteVertexArray),
                InstanceMethod("bindVertexArray", &NativeEngine::BindVertexArray),
                InstanceMethod("createIndexBuffer", &NativeEngine::CreateIndexBuffer),
                InstanceMethod("deleteIndexBuffer", &NativeEngine::DeleteIndexBuffer),
                InstanceMethod("recordIndexBuffer", &NativeEngine::RecordIndexBuffer),
                InstanceMethod("updateDynamicIndexBuffer", &NativeEngine::UpdateDynamicIndexBuffer),
                InstanceMethod("createVertexBuffer", &NativeEngine::CreateVertexBuffer),
                InstanceMethod("deleteVertexBuffer", &NativeEngine::DeleteVertexBuffer),
                InstanceMethod("recordVertexBuffer", &NativeEngine::RecordVertexBuffer),
                InstanceMethod("updateDynamicVertexBuffer", &NativeEngine::UpdateDynamicVertexBuffer),
                InstanceMethod("createProgram", &NativeEngine::CreateProgram),
                InstanceMethod("getUniforms", &NativeEngine::GetUniforms),
                InstanceMethod("getAttributes", &NativeEngine::GetAttributes),
                InstanceMethod("setProgram", &NativeEngine::SetProgram),
                InstanceMethod("setState", &NativeEngine::SetState),
                InstanceMethod("setZOffset", &NativeEngine::SetZOffset),
                InstanceMethod("getZOffset", &NativeEngine::GetZOffset),
                InstanceMethod("setDepthTest", &NativeEngine::SetDepthTest),
                InstanceMethod("getDepthWrite", &NativeEngine::GetDepthWrite),
                InstanceMethod("setDepthWrite", &NativeEngine::SetDepthWrite),
                InstanceMethod("setColorWrite", &NativeEngine::SetColorWrite),
                InstanceMethod("setBlendMode", &NativeEngine::SetBlendMode),
                InstanceMethod("setMatrix", &NativeEngine::SetMatrix),
                InstanceMethod("setInt", &NativeEngine::SetInt),
                InstanceMethod("setIntArray", &NativeEngine::SetIntArray),
                InstanceMethod("setIntArray2", &NativeEngine::SetIntArray2),
                InstanceMethod("setIntArray3", &NativeEngine::SetIntArray3),
                InstanceMethod("setIntArray4", &NativeEngine::SetIntArray4),
                InstanceMethod("setFloatArray", &NativeEngine::SetFloatArray),
                InstanceMethod("setFloatArray2", &NativeEngine::SetFloatArray2),
                InstanceMethod("setFloatArray3", &NativeEngine::SetFloatArray3),
                InstanceMethod("setFloatArray4", &NativeEngine::SetFloatArray4),
                InstanceMethod("setMatrices", &NativeEngine::SetMatrices),
                InstanceMethod("setMatrix3x3", &NativeEngine::SetMatrix3x3),
                InstanceMethod("setMatrix2x2", &NativeEngine::SetMatrix2x2),
                InstanceMethod("setFloat", &NativeEngine::SetFloat),
                InstanceMethod("setFloat2", &NativeEngine::SetFloat2),
                InstanceMethod("setFloat3", &NativeEngine::SetFloat3),
                InstanceMethod("setFloat4", &NativeEngine::SetFloat4),
                InstanceMethod("createTexture", &NativeEngine::CreateTexture),
                InstanceMethod("loadTexture", &NativeEngine::LoadTexture),
                InstanceMethod("loadRawTexture", &NativeEngine::LoadRawTexture),
                InstanceMethod("loadCubeTexture", &NativeEngine::LoadCubeTexture),
                InstanceMethod("copyTexture", &NativeEngine::CopyTexture),
                InstanceMethod("loadCubeTextureWithMips", &NativeEngine::LoadCubeTextureWithMips),
                InstanceMethod("getTextureWidth", &NativeEngine::GetTextureWidth),
                InstanceMethod("getTextureHeight", &NativeEngine::GetTextureHeight),
                InstanceMethod("setTextureSampling", &NativeEngine::SetTextureSampling),
                InstanceMethod("setTextureWrapMode", &NativeEngine::SetTextureWrapMode),
                InstanceMethod("setTextureAnisotropicLevel", &NativeEngine::SetTextureAnisotropicLevel),
                InstanceMethod("setTexture", &NativeEngine::SetTexture),
                InstanceMethod("deleteTexture", &NativeEngine::DeleteTexture),
                InstanceMethod("createFrameBuffer", &NativeEngine::CreateFrameBuffer),
                InstanceMethod("deleteFrameBuffer", &NativeEngine::DeleteFrameBuffer),
                InstanceMethod("bindFrameBuffer", &NativeEngine::BindFrameBuffer),
                InstanceMethod("unbindFrameBuffer", &NativeEngine::UnbindFrameBuffer),
                InstanceMethod("drawIndexed", &NativeEngine::DrawIndexed),
                InstanceMethod("draw", &NativeEngine::Draw),
                InstanceMethod("clear", &NativeEngine::Clear),
                InstanceMethod("getRenderWidth", &NativeEngine::GetRenderWidth),
                InstanceMethod("getRenderHeight", &NativeEngine::GetRenderHeight),
                InstanceMethod("setViewPort", &NativeEngine::SetViewPort),
                InstanceMethod("getHardwareScalingLevel", &NativeEngine::GetHardwareScalingLevel),
                InstanceMethod("setHardwareScalingLevel", &NativeEngine::SetHardwareScalingLevel),
                InstanceMethod("createImageBitmap", &NativeEngine::CreateImageBitmap),
                InstanceMethod("resizeImageBitmap", &NativeEngine::ResizeImageBitmap),
                InstanceMethod("getFrameBufferData", &NativeEngine::GetFrameBufferData),
                InstanceMethod("setStencil", &NativeEngine::SetStencil),

                InstanceValue("TEXTURE_NEAREST_NEAREST", Napi::Number::From(env, TextureSampling::NEAREST_NEAREST)),
                InstanceValue("TEXTURE_LINEAR_LINEAR", Napi::Number::From(env, TextureSampling::LINEAR_LINEAR)),
                InstanceValue("TEXTURE_LINEAR_LINEAR_MIPLINEAR", Napi::Number::From(env, TextureSampling::LINEAR_LINEAR_MIPLINEAR)),
                InstanceValue("TEXTURE_NEAREST_NEAREST_MIPNEAREST", Napi::Number::From(env, TextureSampling::NEAREST_NEAREST_MIPNEAREST)),
                InstanceValue("TEXTURE_NEAREST_LINEAR_MIPNEAREST", Napi::Number::From(env, TextureSampling::NEAREST_LINEAR_MIPNEAREST)),
                InstanceValue("TEXTURE_NEAREST_LINEAR_MIPLINEAR", Napi::Number::From(env, TextureSampling::NEAREST_LINEAR_MIPLINEAR)),
                InstanceValue("TEXTURE_NEAREST_LINEAR", Napi::Number::From(env, TextureSampling::NEAREST_LINEAR)),
                InstanceValue("TEXTURE_NEAREST_NEAREST_MIPLINEAR", Napi::Number::From(env, TextureSampling::NEAREST_NEAREST_MIPLINEAR)),
                InstanceValue("TEXTURE_LINEAR_NEAREST_MIPNEAREST", Napi::Number::From(env, TextureSampling::LINEAR_NEAREST_MIPNEAREST)),
                InstanceValue("TEXTURE_LINEAR_NEAREST_MIPLINEAR", Napi::Number::From(env, TextureSampling::LINEAR_NEAREST_MIPLINEAR)),
                InstanceValue("TEXTURE_LINEAR_LINEAR_MIPNEAREST", Napi::Number::From(env, TextureSampling::LINEAR_LINEAR_MIPNEAREST)),
                InstanceValue("TEXTURE_LINEAR_NEAREST", Napi::Number::From(env, TextureSampling::LINEAR_NEAREST)),

                InstanceValue("DEPTH_TEST_LESS", Napi::Number::From(env, BGFX_STATE_DEPTH_TEST_LESS)),
                InstanceValue("DEPTH_TEST_LEQUAL", Napi::Number::From(env, BGFX_STATE_DEPTH_TEST_LEQUAL)),
                InstanceValue("DEPTH_TEST_EQUAL", Napi::Number::From(env, BGFX_STATE_DEPTH_TEST_EQUAL)),
                InstanceValue("DEPTH_TEST_GEQUAL", Napi::Number::From(env, BGFX_STATE_DEPTH_TEST_GEQUAL)),
                InstanceValue("DEPTH_TEST_GREATER", Napi::Number::From(env, BGFX_STATE_DEPTH_TEST_GREATER)),
                InstanceValue("DEPTH_TEST_NOTEQUAL", Napi::Number::From(env, BGFX_STATE_DEPTH_TEST_NOTEQUAL)),
                InstanceValue("DEPTH_TEST_NEVER", Napi::Number::From(env, BGFX_STATE_DEPTH_TEST_NEVER)),
                InstanceValue("DEPTH_TEST_ALWAYS", Napi::Number::From(env, BGFX_STATE_DEPTH_TEST_ALWAYS)),

                InstanceValue("CLEAR_FLAG_COLOR", Napi::Number::From(env, BGFX_CLEAR_COLOR)),
                InstanceValue("CLEAR_FLAG_DEPTH", Napi::Number::From(env, BGFX_CLEAR_DEPTH)),
                InstanceValue("CLEAR_FLAG_STENCIL", Napi::Number::From(env, BGFX_CLEAR_STENCIL)),

                InstanceValue("ADDRESS_MODE_WRAP", Napi::Number::From(env, 0)),
                InstanceValue("ADDRESS_MODE_MIRROR", Napi::Number::From(env, BGFX_SAMPLER_U_MIRROR)),
                InstanceValue("ADDRESS_MODE_CLAMP", Napi::Number::From(env, BGFX_SAMPLER_U_CLAMP)),
                InstanceValue("ADDRESS_MODE_BORDER", Napi::Number::From(env, BGFX_SAMPLER_U_BORDER)),
                InstanceValue("ADDRESS_MODE_MIRROR_ONCE", Napi::Number::From(env, BGFX_SAMPLER_U_MIRROR)),

                InstanceValue("TEXTURE_FORMAT_RGB8", Napi::Number::From(env, static_cast<uint32_t>(bgfx::TextureFormat::RGB8))),
                InstanceValue("TEXTURE_FORMAT_RGBA8", Napi::Number::From(env, static_cast<uint32_t>(bgfx::TextureFormat::RGBA8))),
                InstanceValue("TEXTURE_FORMAT_RGBA32F", Napi::Number::From(env, static_cast<uint32_t>(bgfx::TextureFormat::RGBA32F))),

                InstanceValue("ATTRIB_TYPE_INT8", Napi::Number::From(env, static_cast<uint32_t>(bgfx::AttribType::Int8))),
                InstanceValue("ATTRIB_TYPE_UINT8", Napi::Number::From(env, static_cast<uint32_t>(bgfx::AttribType::Uint8))),
                InstanceValue("ATTRIB_TYPE_INT16", Napi::Number::From(env, static_cast<uint32_t>(bgfx::AttribType::Int16))),
                InstanceValue("ATTRIB_TYPE_UINT16", Napi::Number::From(env, static_cast<uint32_t>(bgfx::AttribType::Uint16))),
                InstanceValue("ATTRIB_TYPE_FLOAT", Napi::Number::From(env, static_cast<uint32_t>(bgfx::AttribType::Float))),

                InstanceValue("ALPHA_DISABLE", Napi::Number::From(env, AlphaMode::DISABLE)),
                InstanceValue("ALPHA_ADD", Napi::Number::From(env, AlphaMode::ADD)),
                InstanceValue("ALPHA_COMBINE", Napi::Number::From(env, AlphaMode::COMBINE)),
                InstanceValue("ALPHA_SUBTRACT", Napi::Number::From(env, AlphaMode::SUBTRACT)),
                InstanceValue("ALPHA_MULTIPLY", Napi::Number::From(env, AlphaMode::MULTIPLY)),
                InstanceValue("ALPHA_MAXIMIZED", Napi::Number::From(env, AlphaMode::MAXIMIZED)),
                InstanceValue("ALPHA_ONEONE", Napi::Number::From(env, AlphaMode::ONEONE)),
                InstanceValue("ALPHA_PREMULTIPLIED", Napi::Number::From(env, AlphaMode::PREMULTIPLIED)),
                InstanceValue("ALPHA_PREMULTIPLIED_PORTERDUFF", Napi::Number::From(env, AlphaMode::PREMULTIPLIED_PORTERDUFF)),
                InstanceValue("ALPHA_INTERPOLATE", Napi::Number::From(env, AlphaMode::INTERPOLATE)),
                InstanceValue("ALPHA_SCREENMODE", Napi::Number::From(env, AlphaMode::SCREENMODE)),

                InstanceValue("STENCIL_TEST_LESS", Napi::Number::From(env, BGFX_STENCIL_TEST_LESS)),
                InstanceValue("STENCIL_TEST_LEQUAL", Napi::Number::From(env, BGFX_STENCIL_TEST_LEQUAL)),
                InstanceValue("STENCIL_TEST_EQUAL", Napi::Number::From(env, BGFX_STENCIL_TEST_EQUAL)),
                InstanceValue("STENCIL_TEST_GEQUAL", Napi::Number::From(env, BGFX_STENCIL_TEST_GEQUAL)),
                InstanceValue("STENCIL_TEST_GREATER", Napi::Number::From(env, BGFX_STENCIL_TEST_GREATER)),
                InstanceValue("STENCIL_TEST_NOTEQUAL", Napi::Number::From(env, BGFX_STENCIL_TEST_NOTEQUAL)),
                InstanceValue("STENCIL_TEST_NEVER", Napi::Number::From(env, BGFX_STENCIL_TEST_NEVER)),
                InstanceValue("STENCIL_TEST_ALWAYS", Napi::Number::From(env, BGFX_STENCIL_TEST_ALWAYS)),

                InstanceValue("STENCIL_OP_FAIL_S_ZERO", Napi::Number::From(env, BGFX_STENCIL_OP_FAIL_S_ZERO)),
                InstanceValue("STENCIL_OP_FAIL_S_KEEP", Napi::Number::From(env, BGFX_STENCIL_OP_FAIL_S_KEEP)),
                InstanceValue("STENCIL_OP_FAIL_S_REPLACE", Napi::Number::From(env, BGFX_STENCIL_OP_FAIL_S_REPLACE)),
                InstanceValue("STENCIL_OP_FAIL_S_INCR", Napi::Number::From(env, BGFX_STENCIL_OP_FAIL_S_INCR)),
                InstanceValue("STENCIL_OP_FAIL_S_INCRSAT", Napi::Number::From(env, BGFX_STENCIL_OP_FAIL_S_INCRSAT)),
                InstanceValue("STENCIL_OP_FAIL_S_DECR", Napi::Number::From(env, BGFX_STENCIL_OP_FAIL_S_DECR)),
                InstanceValue("STENCIL_OP_FAIL_S_DECRSAT", Napi::Number::From(env, BGFX_STENCIL_OP_FAIL_S_DECRSAT)),
                InstanceValue("STENCIL_OP_FAIL_S_INVERT", Napi::Number::From(env, BGFX_STENCIL_OP_FAIL_S_INVERT)),

                InstanceValue("STENCIL_OP_FAIL_Z_ZERO", Napi::Number::From(env, BGFX_STENCIL_OP_FAIL_Z_ZERO)),
                InstanceValue("STENCIL_OP_FAIL_Z_KEEP", Napi::Number::From(env, BGFX_STENCIL_OP_FAIL_Z_KEEP)),
                InstanceValue("STENCIL_OP_FAIL_Z_REPLACE", Napi::Number::From(env, BGFX_STENCIL_OP_FAIL_Z_REPLACE)),
                InstanceValue("STENCIL_OP_FAIL_Z_INCR", Napi::Number::From(env, BGFX_STENCIL_OP_FAIL_Z_INCR)),
                InstanceValue("STENCIL_OP_FAIL_Z_INCRSAT", Napi::Number::From(env, BGFX_STENCIL_OP_FAIL_Z_INCRSAT)),
                InstanceValue("STENCIL_OP_FAIL_Z_DECR", Napi::Number::From(env, BGFX_STENCIL_OP_FAIL_Z_DECR)),
                InstanceValue("STENCIL_OP_FAIL_Z_DECRSAT", Napi::Number::From(env, BGFX_STENCIL_OP_FAIL_Z_DECRSAT)),
                InstanceValue("STENCIL_OP_FAIL_Z_INVERT", Napi::Number::From(env, BGFX_STENCIL_OP_FAIL_Z_INVERT)),

                InstanceValue("STENCIL_OP_PASS_Z_ZERO", Napi::Number::From(env, BGFX_STENCIL_OP_PASS_Z_ZERO)),
                InstanceValue("STENCIL_OP_PASS_Z_KEEP", Napi::Number::From(env, BGFX_STENCIL_OP_PASS_Z_KEEP)),
                InstanceValue("STENCIL_OP_PASS_Z_REPLACE", Napi::Number::From(env, BGFX_STENCIL_OP_PASS_Z_REPLACE)),
                InstanceValue("STENCIL_OP_PASS_Z_INCR", Napi::Number::From(env, BGFX_STENCIL_OP_PASS_Z_INCR)),
                InstanceValue("STENCIL_OP_PASS_Z_INCRSAT", Napi::Number::From(env, BGFX_STENCIL_OP_PASS_Z_INCRSAT)),
                InstanceValue("STENCIL_OP_PASS_Z_DECR", Napi::Number::From(env, BGFX_STENCIL_OP_PASS_Z_DECR)),
                InstanceValue("STENCIL_OP_PASS_Z_DECRSAT", Napi::Number::From(env, BGFX_STENCIL_OP_PASS_Z_DECRSAT)),
                InstanceValue("STENCIL_OP_PASS_Z_INVERT", Napi::Number::From(env, BGFX_STENCIL_OP_PASS_Z_INVERT)),
            });
        // clang-format on

        JsRuntime::NativeObject::GetFromJavaScript(env).Set(JS_ENGINE_CONSTRUCTOR_NAME, func);
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

        // TODO: clean up bound vertex array

        // This collection contains bgfx data, so it must be cleared before bgfx::shutdown is called.
        m_programDataCollection.clear();
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
        return Napi::External<VertexArray>::New(info.Env(), new VertexArray{});
    }

    void NativeEngine::DeleteVertexArray(const Napi::CallbackInfo& info)
    {
        auto vertexArray{info[0].As<Napi::External<VertexArray>>().Data()};
        // TODO: should we clear the m_boundVertexArray if it gets deleted?
        //assert(vertexArray != m_boundVertexArray);
        delete vertexArray;
    }

    void NativeEngine::BindVertexArray(const Napi::CallbackInfo& info)
    {
        const VertexArray& vertexArray = *(info[0].As<Napi::External<VertexArray>>().Data());
        m_boundVertexArray = &vertexArray;
    }

    Napi::Value NativeEngine::CreateIndexBuffer(const Napi::CallbackInfo& info)
    {
        const Napi::TypedArray data = info[0].As<Napi::TypedArray>();
        const bool dynamic = info[1].As<Napi::Boolean>().Value();

        const uint16_t flags = data.TypedArrayType() == napi_typedarray_type::napi_uint16_array ? 0 : BGFX_BUFFER_INDEX32;

        return Napi::External<IndexBufferData>::New(info.Env(), new IndexBufferData(data, flags, dynamic));
    }

    void NativeEngine::DeleteIndexBuffer(const Napi::CallbackInfo& info)
    {
        IndexBufferData* indexBufferData = info[0].As<Napi::External<IndexBufferData>>().Data();
        delete indexBufferData;
    }

    void NativeEngine::RecordIndexBuffer(const Napi::CallbackInfo& info)
    {
        VertexArray& vertexArray = *(info[0].As<Napi::External<VertexArray>>().Data());
        const IndexBufferData* indexBufferData = info[1].As<Napi::External<IndexBufferData>>().Data();

        vertexArray.indexBuffer.Data = indexBufferData;
    }

    void NativeEngine::UpdateDynamicIndexBuffer(const Napi::CallbackInfo& info)
    {
        IndexBufferData& indexBufferData = *(info[0].As<Napi::External<IndexBufferData>>().Data());

        const Napi::TypedArray data = info[1].As<Napi::TypedArray>();
        const uint32_t startingIdx = info[2].As<Napi::Number>().Uint32Value();

        indexBufferData.Update(info.Env(), data, startingIdx);
    }

    Napi::Value NativeEngine::CreateVertexBuffer(const Napi::CallbackInfo& info)
    {
        const Napi::Uint8Array data = info[0].As<Napi::Uint8Array>();
        const bool dynamic = info[1].As<Napi::Boolean>().Value();

        return Napi::External<VertexBufferData>::New(info.Env(), new VertexBufferData(data, dynamic));
    }

    void NativeEngine::DeleteVertexBuffer(const Napi::CallbackInfo& info)
    {
        auto* vertexBufferData = info[0].As<Napi::External<VertexBufferData>>().Data();
        delete vertexBufferData;
    }

    void NativeEngine::RecordVertexBuffer(const Napi::CallbackInfo& info)
    {
        VertexArray& vertexArray = *(info[0].As<Napi::External<VertexArray>>().Data());
        VertexBufferData* vertexBufferData = info[1].As<Napi::External<VertexBufferData>>().Data();

        const uint32_t location = info[2].As<Napi::Number>().Uint32Value();
        const uint32_t byteOffset = info[3].As<Napi::Number>().Uint32Value();
        const uint32_t byteStride = info[4].As<Napi::Number>().Uint32Value();
        const uint32_t numElements = info[5].As<Napi::Number>().Uint32Value();
        const uint32_t type = info[6].As<Napi::Number>().Uint32Value();
        const bool normalized = info[7].As<Napi::Boolean>().Value();

        bgfx::VertexLayout vertexLayout{};
        vertexLayout.begin();

        const bgfx::Attrib::Enum attrib = static_cast<bgfx::Attrib::Enum>(location);
        const bgfx::AttribType::Enum attribType = static_cast<bgfx::AttribType::Enum>(type);

        const bool promoteToFloats = !normalized
            && (bgfx::getCaps()->rendererType == bgfx::RendererType::Direct3D11 ||
                bgfx::getCaps()->rendererType == bgfx::RendererType::Direct3D12 ||
                bgfx::getCaps()->rendererType == bgfx::RendererType::Vulkan)
            && (attribType == bgfx::AttribType::Int8 ||
                attribType == bgfx::AttribType::Uint8 ||
                attribType == bgfx::AttribType::Uint10 ||
                attribType == bgfx::AttribType::Int16 ||
                attribType == bgfx::AttribType::Uint16);

        if (promoteToFloats)
        {
            vertexLayout.add(attrib, static_cast<uint8_t>(numElements), bgfx::AttribType::Float);
            vertexLayout.m_stride = static_cast<uint16_t>(sizeof(float) * numElements);
            vertexBufferData->PromoteToFloats(attribType, numElements, byteOffset, byteStride);
        }
        else
        {
            vertexLayout.add(attrib, static_cast<uint8_t>(numElements), attribType, normalized);
            vertexLayout.m_stride = static_cast<uint16_t>(byteStride);
            vertexLayout.m_offset[attrib] = static_cast<uint16_t>(byteOffset % byteStride);
        }

        vertexLayout.end();
        vertexBufferData->EnsureFinalized(info.Env(), vertexLayout);

        // Second parameter is first vertex. byteOffset and byteStride are both using original values (without float promotion)
        vertexArray.VertexBuffers[location] = {vertexBufferData, byteOffset / byteStride, bgfx::createVertexLayout(vertexLayout)};
    }

    void NativeEngine::UpdateDynamicVertexBuffer(const Napi::CallbackInfo& info)
    {
        VertexBufferData& vertexBufferData = *(info[0].As<Napi::External<VertexBufferData>>().Data());
        const Napi::Uint8Array data = info[1].As<Napi::Uint8Array>();
        const uint32_t byteOffset = info[2].As<Napi::Number>().Uint32Value();

        uint32_t byteLength = info[2].As<Napi::Number>().Uint32Value();
        if (byteLength == 0)
        {
            byteLength = static_cast<uint32_t>(data.ByteLength());
        }

        vertexBufferData.Update(info.Env(), data, byteOffset, byteLength);
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
        const std::string vertexSource{info[0].As<Napi::String>().Utf8Value()};
        const std::string fragmentSource{info[1].As<Napi::String>().Utf8Value()};

        std::unique_ptr<ProgramData> programData{std::make_unique<ProgramData>()};
        ShaderCompiler::BgfxShaderInfo shaderInfo{};

        try
        {
            shaderInfo = m_shaderCompiler.Compile(ProcessShaderCoordinates(vertexSource), ProcessSamplerFlip(fragmentSource));
        }
        catch (const std::exception& ex)
        {
            throw Napi::Error::New(info.Env(), ex.what());
        }

        static auto InitUniformInfos{[](bgfx::ShaderHandle shader, const std::unordered_map<std::string, uint8_t>& uniformStages, std::unordered_map<std::string, UniformInfo>& uniformInfos) {
            auto numUniforms = bgfx::getShaderUniforms(shader);
            std::vector<bgfx::UniformHandle> uniforms{numUniforms};
            bgfx::getShaderUniforms(shader, uniforms.data(), gsl::narrow_cast<uint16_t>(uniforms.size()));

            for (uint8_t index = 0; index < numUniforms; index++)
            {
                bgfx::UniformInfo info{};
                bgfx::getUniformInfo(uniforms[index], info);
                auto itStage = uniformStages.find(info.name);
                uniformInfos[info.name] = {itStage == uniformStages.end() ? uint8_t{} : itStage->second, uniforms[index]};
            }
        }};

        auto vertexShader = bgfx::createShader(bgfx::copy(shaderInfo.VertexBytes.data(), static_cast<uint32_t>(shaderInfo.VertexBytes.size())));
        InitUniformInfos(vertexShader, shaderInfo.UniformStages, programData->UniformInfos);
        programData->VertexAttributeLocations = std::move(shaderInfo.VertexAttributeLocations);

        auto fragmentShader = bgfx::createShader(bgfx::copy(shaderInfo.FragmentBytes.data(), static_cast<uint32_t>(shaderInfo.FragmentBytes.size())));
        InitUniformInfos(fragmentShader, shaderInfo.UniformStages, programData->UniformInfos);

        programData->Handle = bgfx::createProgram(vertexShader, fragmentShader, true);
        auto* rawProgramData = programData.get();
        auto ticket = m_programDataCollection.insert(std::move(programData));
        auto finalizer = [ticket = std::move(ticket)](Napi::Env, ProgramData*) {};
        return Napi::External<ProgramData>::New(info.Env(), rawProgramData, std::move(finalizer));
    }

    Napi::Value NativeEngine::GetUniforms(const Napi::CallbackInfo& info)
    {
        const auto program{info[0].As<Napi::External<ProgramData>>().Data()};
        const auto names{info[1].As<Napi::Array>()};

        const auto length{names.Length()};
        auto uniforms{Napi::Array::New(info.Env(), length)};
        for (uint32_t index = 0; index < length; ++index)
        {
            if (names[index].IsString())
            {
                const auto name{names[index].As<Napi::String>().Utf8Value()};
                const auto itUniformInfo{program->UniformInfos.find(name)};
                if (itUniformInfo != program->UniformInfos.end())
                {
                    uniforms[index] = Napi::External<UniformInfo>::New(info.Env(), &itUniformInfo->second);
                    continue;
                }
            }

            uniforms[index] = info.Env().Null();
        }

        return std::move(uniforms);
    }

    Napi::Value NativeEngine::GetAttributes(const Napi::CallbackInfo& info)
    {
        const auto program = info[0].As<Napi::External<ProgramData>>().Data();
        const auto names = info[1].As<Napi::Array>();

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

    void NativeEngine::SetProgram(const Napi::CallbackInfo& info)
    {
        auto program = info[0].As<Napi::External<ProgramData>>().Data();
        m_currentProgram = program;
    }

    void NativeEngine::SetState(const Napi::CallbackInfo& info)
    {
        const auto culling = info[0].As<Napi::Boolean>().Value();
        const auto cullBackFaces = info[2].As<Napi::Boolean>().Value();
        const auto reverseSide = info[3].As<Napi::Boolean>().Value();

        m_engineState &= ~(BGFX_STATE_CULL_MASK | BGFX_STATE_FRONT_CCW);
        m_engineState |= reverseSide ? 0 : BGFX_STATE_FRONT_CCW;

        if (culling)
        {
            m_engineState |= cullBackFaces ? BGFX_STATE_CULL_CCW : BGFX_STATE_CULL_CW;
        }
        // TODO: zOffset
        //const auto zOffset = info[1].As<Napi::Number>().FloatValue();
    }

    void NativeEngine::SetZOffset(const Napi::CallbackInfo& /*info*/)
    {
        //const auto zOffset = info[0].As<Napi::Number>().FloatValue();

        // STUB: Stub.
    }

    Napi::Value NativeEngine::GetZOffset(const Napi::CallbackInfo& /*info*/)
    {
        // STUB: Stub.
        return {};
    }

    void NativeEngine::SetDepthTest(const Napi::CallbackInfo& info)
    {
        const auto depthTest = info[0].As<Napi::Number>().Uint32Value();

        m_engineState &= ~BGFX_STATE_DEPTH_TEST_MASK;
        m_engineState |= depthTest;
    }

    Napi::Value NativeEngine::GetDepthWrite(const Napi::CallbackInfo& info)
    {
        return Napi::Value::From(info.Env(), !!(m_engineState & BGFX_STATE_WRITE_Z));
    }

    void NativeEngine::SetDepthWrite(const Napi::CallbackInfo& info)
    {
        const auto enable = info[0].As<Napi::Boolean>().Value();

        m_engineState &= ~BGFX_STATE_WRITE_Z;
        m_engineState |= enable ? BGFX_STATE_WRITE_Z : 0;
    }

    void NativeEngine::SetColorWrite(const Napi::CallbackInfo& info)
    {
        const auto enable = info[0].As<Napi::Boolean>().Value();

        m_engineState &= ~(BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A);
        m_engineState |= enable ? (BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A) : 0;
    }

    void NativeEngine::SetBlendMode(const Napi::CallbackInfo& info)
    {
        const auto blendMode = info[0].As<Napi::Number>().Int64Value();

        m_engineState &= ~BGFX_STATE_BLEND_MASK;
        m_engineState |= blendMode;
    }

    void NativeEngine::SetInt(const Napi::CallbackInfo& info)
    {
        const auto uniformInfo = info[0].As<Napi::External<UniformInfo>>().Data();
        const auto value = info[1].As<Napi::Number>().FloatValue();
        m_currentProgram->SetUniform(uniformInfo->Handle, gsl::make_span(&value, 1));
    }

    template<int size, typename arrayType>
    void NativeEngine::SetTypeArrayN(const Napi::CallbackInfo& info)
    {
        const auto uniformInfo = info[0].As<Napi::External<UniformInfo>>().Data();
        const auto array = info[1].As<arrayType>();

        size_t elementLength = array.ElementLength();

        m_scratch.clear();
        for (size_t index = 0; index < elementLength; index += size)
        {
            const float values[] = {
                static_cast<float>(array[index]),
                (size > 1) ? static_cast<float>(array[index + 1]) : 0.f,
                (size > 2) ? static_cast<float>(array[index + 2]) : 0.f,
                (size > 3) ? static_cast<float>(array[index + 3]) : 0.f,
            };
            m_scratch.insert(m_scratch.end(), values, values + 4);
        }

        m_currentProgram->SetUniform(uniformInfo->Handle, m_scratch, elementLength / size);
    }

    template<int size>
    void NativeEngine::SetFloatN(const Napi::CallbackInfo& info)
    {
        const auto uniformInfo = info[0].As<Napi::External<UniformInfo>>().Data();
        const float values[] = {
            info[1].As<Napi::Number>().FloatValue(),
            (size > 1) ? info[2].As<Napi::Number>().FloatValue() : 0.f,
            (size > 2) ? info[3].As<Napi::Number>().FloatValue() : 0.f,
            (size > 3) ? info[4].As<Napi::Number>().FloatValue() : 0.f,
        };

        m_currentProgram->SetUniform(uniformInfo->Handle, values);
    }

    template<int size>
    void NativeEngine::SetMatrixN(const Napi::CallbackInfo& info)
    {
        const auto uniformInfo = info[0].As<Napi::External<UniformInfo>>().Data();
        const auto matrix = info[1].As<Napi::Float32Array>();

        const size_t elementLength = matrix.ElementLength();
        assert(elementLength == size * size);
        (void)elementLength;

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

            m_currentProgram->SetUniform(uniformInfo->Handle, gsl::make_span(matrixValues.data(), 16));
        }
        else
        {
            m_currentProgram->SetUniform(uniformInfo->Handle, gsl::make_span(matrix.Data(), elementLength));
        }
    }

    void NativeEngine::SetIntArray(const Napi::CallbackInfo& info)
    {
        SetTypeArrayN<1, Napi::Int32Array>(info);
    }

    void NativeEngine::SetIntArray2(const Napi::CallbackInfo& info)
    {
        SetTypeArrayN<2, Napi::Int32Array>(info);
    }

    void NativeEngine::SetIntArray3(const Napi::CallbackInfo& info)
    {
        SetTypeArrayN<3, Napi::Int32Array>(info);
    }

    void NativeEngine::SetIntArray4(const Napi::CallbackInfo& info)
    {
        SetTypeArrayN<4, Napi::Int32Array>(info);
    }

    void NativeEngine::SetFloatArray(const Napi::CallbackInfo& info)
    {
        SetTypeArrayN<1, Napi::Float32Array>(info);
    }

    void NativeEngine::SetFloatArray2(const Napi::CallbackInfo& info)
    {
        SetTypeArrayN<2, Napi::Float32Array>(info);
    }

    void NativeEngine::SetFloatArray3(const Napi::CallbackInfo& info)
    {
        SetTypeArrayN<3, Napi::Float32Array>(info);
    }

    void NativeEngine::SetFloatArray4(const Napi::CallbackInfo& info)
    {
        SetTypeArrayN<4, Napi::Float32Array>(info);
    }

    void NativeEngine::SetMatrices(const Napi::CallbackInfo& info)
    {
        const auto uniformInfo = info[0].As<Napi::External<UniformInfo>>().Data();
        const auto matricesArray = info[1].As<Napi::Float32Array>();

        const size_t elementLength = matricesArray.ElementLength();
        assert(elementLength % 16 == 0);

        m_currentProgram->SetUniform(uniformInfo->Handle, gsl::span(matricesArray.Data(), elementLength), elementLength / 16);
    }

    void NativeEngine::SetMatrix2x2(const Napi::CallbackInfo& info)
    {
        SetMatrixN<2>(info);
    }

    void NativeEngine::SetMatrix3x3(const Napi::CallbackInfo& info)
    {
        SetMatrixN<3>(info);
    }

    void NativeEngine::SetMatrix(const Napi::CallbackInfo& info)
    {
        SetMatrixN<4>(info);
    }

    void NativeEngine::SetFloat(const Napi::CallbackInfo& info)
    {
        SetFloatN<1>(info);
    }

    void NativeEngine::SetFloat2(const Napi::CallbackInfo& info)
    {
        SetFloatN<2>(info);
    }

    void NativeEngine::SetFloat3(const Napi::CallbackInfo& info)
    {
        SetFloatN<3>(info);
    }

    void NativeEngine::SetFloat4(const Napi::CallbackInfo& info)
    {
        SetFloatN<4>(info);
    }

    Napi::Value NativeEngine::CreateTexture(const Napi::CallbackInfo& info)
    {
        return Napi::External<TextureData>::New(info.Env(), new TextureData());
    }

    void NativeEngine::LoadTexture(const Napi::CallbackInfo& info)
    {
        const auto texture{info[0].As<Napi::External<TextureData>>().Data()};
        const auto data{info[1].As<Napi::TypedArray>()};
        const auto generateMips{info[2].As<Napi::Boolean>().Value()};
        const auto invertY{info[3].As<Napi::Boolean>().Value()};
        const auto srgb{info[4].As<Napi::Boolean>().Value()};
        const auto onSuccess{info[5].As<Napi::Function>()};
        const auto onError{info[6].As<Napi::Function>()};

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
        const auto textureDestination = info[0].As<Napi::External<TextureData>>().Data();
        const auto textureSource = info[1].As<Napi::External<TextureData>>().Data();
        const auto handleSource{textureSource->Handle};
        // Make sure destination texture is valid for BLIT and is not created from static datas.
        CreateBlitTexture(textureDestination);
        const auto handleDestination{ textureDestination->Handle };

        arcana::make_task(m_graphicsImpl.BeforeRenderScheduler(), *m_cancellationSource, [this, handleSource, handleDestination, cancellationSource{ m_cancellationSource }]() {
            return arcana::make_task(m_runtimeScheduler, *m_cancellationSource, [this, handleSource, handleDestination, updateToken{ m_graphicsImpl.GetUpdateToken() }, cancellationSource{ m_cancellationSource }]() {
                // JS Thread
                if (bgfx::getCaps()->supported & BGFX_CAPS_TEXTURE_BLIT)
                {
                    bgfx::Encoder* encoder = m_graphicsImpl.GetUpdateToken().GetEncoder();
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
        const auto texture{info[0].As<Napi::External<TextureData>>().Data()};
        const auto data{info[1].As<Napi::TypedArray>()};
        const auto width{static_cast<uint16_t>(info[2].As<Napi::Number>().Uint32Value())};
        const auto height{static_cast<uint16_t>(info[3].As<Napi::Number>().Uint32Value())};
        const auto format{static_cast<bimg::TextureFormat::Enum>(info[4].As<Napi::Number>().Uint32Value())};
        const auto generateMips{info[5].As<Napi::Boolean>().Value()};
        const auto invertY{info[6].As<Napi::Boolean>().Value()};

        const auto bytes{static_cast<uint8_t*>(data.ArrayBuffer().Data()) + data.ByteOffset()};
        if (data.ByteLength() != bimg::imageGetSize(nullptr, width, height, 1, false, false, 1, format))
        {
            throw std::runtime_error{"The data size does not match width, height, and format"};
        }

        bimg::ImageContainer* image{bimg::imageAlloc(&m_allocator, format, width, height, 1, 1, false, false, bytes)};
        image = PrepareImage(m_allocator, image, invertY, false, generateMips);
        LoadTextureFromImage(texture, image, false);
    }

    void NativeEngine::LoadCubeTexture(const Napi::CallbackInfo& info)
    {
        const auto texture{info[0].As<Napi::External<TextureData>>().Data()};
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
        const auto texture{info[0].As<Napi::External<TextureData>>().Data()};
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
        const auto texture = info[0].As<Napi::External<TextureData>>().Data();
        return Napi::Value::From(info.Env(), texture->Width);
    }

    Napi::Value NativeEngine::GetTextureHeight(const Napi::CallbackInfo& info)
    {
        const auto texture = info[0].As<Napi::External<TextureData>>().Data();
        return Napi::Value::From(info.Env(), texture->Height);
    }

    void NativeEngine::SetTextureSampling(const Napi::CallbackInfo& info)
    {
        const auto texture = info[0].As<Napi::External<TextureData>>().Data();
        const auto value = static_cast<uint32_t>(info[1].As<Napi::Number>().Uint32Value());

        texture->Flags &= ~(BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT | BGFX_SAMPLER_MIP_POINT);
        texture->Flags |= value;

        // Disable anisotropy if either min/mag are point.
        if ((texture->Flags & BGFX_SAMPLER_MIN_POINT) != 0 || (texture->Flags & BGFX_SAMPLER_MAG_POINT) != 0)
        {
            texture->Flags &= ~(BGFX_SAMPLER_MIN_ANISOTROPIC | BGFX_SAMPLER_MAG_ANISOTROPIC);
        }
    }

    void NativeEngine::SetTextureWrapMode(const Napi::CallbackInfo& info)
    {
        const auto texture = info[0].As<Napi::External<TextureData>>().Data();
        auto addressModeU = static_cast<uint32_t>(info[1].As<Napi::Number>().Uint32Value());
        auto addressModeV = static_cast<uint32_t>(info[2].As<Napi::Number>().Uint32Value());
        auto addressModeW = static_cast<uint32_t>(info[3].As<Napi::Number>().Uint32Value());

        uint32_t addressMode = addressModeU +
            (addressModeV << BGFX_SAMPLER_V_SHIFT) +
            (addressModeW << BGFX_SAMPLER_W_SHIFT);

        texture->Flags &= ~(BGFX_SAMPLER_U_MASK | BGFX_SAMPLER_V_MASK | BGFX_SAMPLER_W_MASK);
        texture->Flags |= addressMode;
    }

    void NativeEngine::SetTextureAnisotropicLevel(const Napi::CallbackInfo& info)
    {
        const auto texture = info[0].As<Napi::External<TextureData>>().Data();
        const auto value = info[1].As<Napi::Number>().Uint32Value();

        texture->Flags &= ~(BGFX_SAMPLER_MIN_ANISOTROPIC | BGFX_SAMPLER_MAG_ANISOTROPIC);

        // Enable anisotropy only if neither min/mag are point.
        // Note that bgfx currently only supports no anisotropy or max anisotropy.
        if ((texture->Flags & BGFX_SAMPLER_MIN_POINT) == 0 && (texture->Flags & BGFX_SAMPLER_MAG_POINT) == 0 && value > 1)
        {
            texture->Flags |= BGFX_SAMPLER_MIN_ANISOTROPIC | BGFX_SAMPLER_MAG_ANISOTROPIC;
        }
    }

    void NativeEngine::SetTexture(const Napi::CallbackInfo& info)
    {
        bgfx::Encoder* encoder{GetUpdateToken().GetEncoder()};

        const auto uniformInfo = info[0].As<Napi::External<UniformInfo>>().Data();
        const auto texture = info[1].As<Napi::External<TextureData>>().Data();

        encoder->setTexture(uniformInfo->Stage, uniformInfo->Handle, texture->Handle, texture->Flags);
    }

    void NativeEngine::DeleteTexture(const Napi::CallbackInfo& info)
    {
        const auto texture = info[0].As<Napi::External<TextureData>>().Data();
        m_graphicsImpl.RemoveTexture(texture->Handle);
        delete texture;
    }

    Napi::Value NativeEngine::CreateFrameBuffer(const Napi::CallbackInfo& info)
    {
        TextureData* texture = info[0].As<Napi::External<TextureData>>().Data();
        uint16_t width{static_cast<uint16_t>(info[1].As<Napi::Number>().Uint32Value())};
        uint16_t height{static_cast<uint16_t>(info[2].As<Napi::Number>().Uint32Value())};
        bgfx::TextureFormat::Enum format{static_cast<bgfx::TextureFormat::Enum>(info[3].As<Napi::Number>().Uint32Value())};
        bool generateStencilBuffer{info[4].As<Napi::Boolean>()};
        bool generateDepth{info[5].As<Napi::Boolean>()};
        bool generateMips{info[6].As<Napi::Boolean>()};

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

        auto* frameBuffer = new FrameBuffer(m_graphicsImpl, frameBufferHandle, width, height, false, generateDepth, generateStencilBuffer);
        return Napi::External<FrameBuffer>::New(info.Env(), frameBuffer, [](Napi::Env, FrameBuffer* frameBuffer) { delete frameBuffer; });
    }

    void NativeEngine::DeleteFrameBuffer(const Napi::CallbackInfo& info)
    {
        auto frameBuffer{info[0].As<Napi::External<FrameBuffer>>().Data()};
        frameBuffer->Dispose();
    }

    void NativeEngine::BindFrameBuffer(const Napi::CallbackInfo& info)
    {
        auto frameBuffer{info[0].As<Napi::External<FrameBuffer>>().Data()};
        auto* encoder = GetUpdateToken().GetEncoder();

        m_boundFrameBuffer->Unbind(*encoder);
        m_boundFrameBuffer = frameBuffer;
        m_boundFrameBuffer->Bind(*encoder);
        m_boundFrameBufferNeedsRebinding.Set(*encoder, false);
    }

    void NativeEngine::UnbindFrameBuffer(const Napi::CallbackInfo& info)
    {
        const auto frameBuffer{info[0].As<Napi::External<FrameBuffer>>().Data()};
        auto* encoder = GetUpdateToken().GetEncoder();

        assert(frameBuffer == m_boundFrameBuffer);
        UNUSED(frameBuffer);

        m_boundFrameBuffer->Unbind(*encoder);
        m_boundFrameBuffer = nullptr;
        m_boundFrameBufferNeedsRebinding.Set(*encoder, false);
    }

    void NativeEngine::DrawIndexed(const Napi::CallbackInfo& info)
    {
        bgfx::Encoder* encoder{GetUpdateToken().GetEncoder()};

        const auto fillMode = info[0].As<Napi::Number>().Int32Value();
        const auto indexStart = info[1].As<Napi::Number>().Int32Value();
        const auto indexCount = info[2].As<Napi::Number>().Int32Value();

        if (m_boundVertexArray != nullptr)
        {
            const auto indexBufferData{m_boundVertexArray->indexBuffer.Data};
            if (indexBufferData != nullptr)
            {
                indexBufferData->SetBgfxIndexBuffer(encoder, indexStart, indexCount);
            }

            const auto& vertexBuffers = m_boundVertexArray->VertexBuffers;
            for (const auto& vertexBufferPair : vertexBuffers)
            {
                const auto index{static_cast<uint8_t>(vertexBufferPair.first)};
                const auto& vertexBuffer{vertexBufferPair.second};
                vertexBuffer.Data->SetAsBgfxVertexBuffer(encoder, index, vertexBuffer.StartVertex, std::numeric_limits<uint32_t>::max(), vertexBuffer.VertexLayoutHandle);
            }
        }

        Draw(encoder, fillMode);
    }

    void NativeEngine::Draw(const Napi::CallbackInfo& info)
    {
        bgfx::Encoder* encoder{GetUpdateToken().GetEncoder()};

        const auto fillMode = info[0].As<Napi::Number>().Int32Value();
        const auto verticesStart = info[1].As<Napi::Number>().Int32Value();
        const auto verticesCount = info[2].As<Napi::Number>().Int32Value();

        if (m_boundVertexArray != nullptr)
        {
            const auto& vertexBuffers = m_boundVertexArray->VertexBuffers;
            for (const auto& vertexBufferPair : vertexBuffers)
            {
                const auto index{static_cast<uint8_t>(vertexBufferPair.first)};
                const auto& vertexBuffer = vertexBufferPair.second;
                vertexBuffer.Data->SetAsBgfxVertexBuffer(encoder, index, vertexBuffer.StartVertex + verticesStart, verticesCount, vertexBuffer.VertexLayoutHandle);
            }
        }

        Draw(encoder, fillMode);
    }

    void NativeEngine::Clear(const Napi::CallbackInfo& info)
    {
        bgfx::Encoder* encoder{GetUpdateToken().GetEncoder()};

        uint16_t flags{0};
        uint32_t rgba{0x000000ff};
        float depth{1.0f};
        uint8_t stencil{0};

        if (!info[0].IsUndefined())
        {
            const auto color{info[0].As<Napi::Object>()};
            const auto r = color.Get("r");
            const auto g = color.Get("g");
            const auto b = color.Get("b");
            const auto a = color.Get("a");

            rgba =
                (static_cast<uint8_t>(r.As<Napi::Number>().FloatValue() * std::numeric_limits<uint8_t>::max()) << 24) +
                (static_cast<uint8_t>(g.As<Napi::Number>().FloatValue() * std::numeric_limits<uint8_t>::max()) << 16) +
                (static_cast<uint8_t>(b.As<Napi::Number>().FloatValue() * std::numeric_limits<uint8_t>::max()) << 8) +
                static_cast<uint8_t>((a.IsUndefined() ? 1.f : a.As<Napi::Number>().FloatValue()) * std::numeric_limits<uint8_t>::max());

            flags |= BGFX_CLEAR_COLOR;
        }

        if (!info[1].IsUndefined() && m_boundFrameBuffer->HasDepth())
        {
            depth = info[1].As<Napi::Number>().FloatValue();
            flags |= BGFX_CLEAR_DEPTH;
        }

        if (!info[2].IsUndefined() && m_boundFrameBuffer->HasStencil())
        {
            stencil = static_cast<uint8_t>(info[2].As<Napi::Number>().Uint32Value());
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

    void NativeEngine::SetStencil(const Napi::CallbackInfo& info)
    {
        const auto writeMask{info[0].As<Napi::Number>().Uint32Value()};
        const auto stencilOpFail{info[1].As<Napi::Number>().Uint32Value()};
        const auto depthOpFail{info[2].As<Napi::Number>().Uint32Value()};
        const auto depthOpPass{info[3].As<Napi::Number>().Uint32Value()};
        const auto func{info[4].As<Napi::Number>().Uint32Value()};
        const auto ref{info[5].As<Napi::Number>().Uint32Value()};
        
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

    void NativeEngine::Draw(bgfx::Encoder* encoder, int fillMode)
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
            m_updateToken.emplace(m_graphicsImpl.GetUpdateToken());
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

        arcana::make_task(m_graphicsImpl.BeforeRenderScheduler(), *m_cancellationSource, [this, cancellationSource{m_cancellationSource}]() {
            return arcana::make_task(m_runtimeScheduler, *m_cancellationSource, [this, updateToken{m_graphicsImpl.GetUpdateToken()}, cancellationSource{m_cancellationSource}]() {
                m_requestAnimationFrameCallbacksScheduled = false;

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
