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
            constexpr uint32_t BGFX_SAMPLER_DEFAULT = 0;

            // clang-format off
            // Names, as in constants.ts are MAG_MIN(_MIP?)     MAG                         MIN                         MIP
            constexpr uint32_t NEAREST_NEAREST =                BGFX_SAMPLER_MAG_POINT  |   BGFX_SAMPLER_MIN_POINT  |   BGFX_SAMPLER_DEFAULT    ;
            constexpr uint32_t LINEAR_LINEAR =                  BGFX_SAMPLER_DEFAULT    |   BGFX_SAMPLER_DEFAULT    |   BGFX_SAMPLER_DEFAULT    ;
            constexpr uint32_t LINEAR_LINEAR_MIPLINEAR =        BGFX_SAMPLER_DEFAULT    |   BGFX_SAMPLER_DEFAULT    |   BGFX_SAMPLER_DEFAULT    ;
            constexpr uint32_t NEAREST_NEAREST_MIPNEAREST =     BGFX_SAMPLER_MAG_POINT  |   BGFX_SAMPLER_MIN_POINT  |   BGFX_SAMPLER_MIP_POINT  ;
            constexpr uint32_t NEAREST_LINEAR_MIPNEAREST =      BGFX_SAMPLER_MAG_POINT  |   BGFX_SAMPLER_DEFAULT    |   BGFX_SAMPLER_MIP_POINT  ;
            constexpr uint32_t NEAREST_LINEAR_MIPLINEAR =       BGFX_SAMPLER_MAG_POINT  |   BGFX_SAMPLER_DEFAULT    |   BGFX_SAMPLER_DEFAULT    ;
            constexpr uint32_t NEAREST_LINEAR =                 BGFX_SAMPLER_MAG_POINT  |   BGFX_SAMPLER_DEFAULT    |   BGFX_SAMPLER_DEFAULT    ;
            constexpr uint32_t NEAREST_NEAREST_MIPLINEAR =      BGFX_SAMPLER_MAG_POINT  |   BGFX_SAMPLER_MIN_POINT  |   BGFX_SAMPLER_DEFAULT    ;
            constexpr uint32_t LINEAR_NEAREST_MIPNEAREST =      BGFX_SAMPLER_DEFAULT    |   BGFX_SAMPLER_MIN_POINT  |   BGFX_SAMPLER_MIP_POINT  ;
            constexpr uint32_t LINEAR_NEAREST_MIPLINEAR =       BGFX_SAMPLER_DEFAULT    |   BGFX_SAMPLER_MIN_POINT  |   BGFX_SAMPLER_DEFAULT    ;
            constexpr uint32_t LINEAR_LINEAR_MIPNEAREST =       BGFX_SAMPLER_DEFAULT    |   BGFX_SAMPLER_DEFAULT    |   BGFX_SAMPLER_MIP_POINT  ;
            constexpr uint32_t LINEAR_NEAREST =                 BGFX_SAMPLER_DEFAULT    |   BGFX_SAMPLER_MIN_POINT  |   BGFX_SAMPLER_DEFAULT    ;
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

        void FlipY(bimg::ImageContainer* image)
        {
            uint8_t* bytes = static_cast<uint8_t*>(image->m_data);
            uint32_t rowCount = image->m_height;
            uint32_t rowPitch = image->m_size / image->m_height;

            std::vector<uint8_t> buffer(rowPitch);

            for (size_t row = 0; row < rowCount / 2; row++)
            {
                auto frontPtr = bytes + (row * rowPitch);
                auto backPtr = bytes + ((rowCount - row - 1) * rowPitch);

                std::memcpy(buffer.data(), frontPtr, rowPitch);
                std::memcpy(frontPtr, backPtr, rowPitch);
                std::memcpy(backPtr, buffer.data(), rowPitch);
            }
        }

        void GenerateMips(bx::AllocatorI* allocator, bimg::ImageContainer** image)
        {
            bimg::ImageContainer* input = *image;

            bimg::ImageContainer* output = bimg::imageGenerateMips(allocator, *input);
            if (output == nullptr)
            {
                bimg::TextureFormat::Enum format = input->m_format;
                bimg::ImageContainer* rgba = bimg::imageConvert(allocator, bimg::TextureFormat::RGBA8, *input, false);
                bimg::imageFree(input);
                bimg::ImageContainer* mips = bimg::imageGenerateMips(allocator, *rgba);
                bimg::imageFree(rgba);
                output = bimg::imageConvert(allocator, format, *mips);
                bimg::imageFree(mips);
            }
            else
            {
                bimg::imageFree(input);
            }

            *image = output;
        }

        void CreateTextureFromImage(TextureData* texture, bimg::ImageContainer* image)
        {
            auto releaseFn = [](void* /*ptr*/, void* userData) {
                bimg::imageFree(static_cast<bimg::ImageContainer*>(userData));
            };

            auto mem = bgfx::makeRef(image->m_data, image->m_size, releaseFn, image);

            texture->Handle = bgfx::createTexture2D(static_cast<uint16_t>(image->m_width), static_cast<uint16_t>(image->m_height), (image->m_numMips > 1), 1, Cast(image->m_format), BGFX_TEXTURE_NONE | BGFX_SAMPLER_NONE, mem);
            texture->Width = image->m_width;
            texture->Height = image->m_height;
        }

        void CreateBlitTexture(TextureData* texture)
        {
            if (texture->Flags & BGFX_TEXTURE_BLIT_DST)
            {
                return;
            }
            bgfx::destroy(texture->Handle);
            texture->Handle = bgfx::createTexture2D((uint16_t)texture->Width, (uint16_t)texture->Height, false, 1, bgfx::TextureFormat::RGBA8, BGFX_TEXTURE_BLIT_DST);
            texture->Flags |= BGFX_TEXTURE_BLIT_DST;
        }

        void CreateCubeTextureFromImages(TextureData* texture, const std::vector<bimg::ImageContainer*>& images, bool hasMips)
        {
            const bimg::ImageContainer* firstImage = images.front();
            uint32_t width = firstImage->m_width;
            uint32_t height = firstImage->m_height;
            bgfx::TextureFormat::Enum format = Cast(firstImage->m_format);

            uint32_t totalSize = 0;
            for (auto image : images)
            {
                totalSize += image->m_size;
            }

            // Combine all the faces into one chunk.
            const bgfx::Memory* mem = bgfx::alloc(totalSize);
            uint8_t* ptr = mem->data;
            for (bimg::ImageContainer* image : images)
            {
                std::memcpy(ptr, image->m_data, image->m_size);
                ptr += image->m_size;
                bimg::imageFree(image);
            }

            texture->Handle = bgfx::createTextureCube(static_cast<uint16_t>(width), hasMips, 1, format, BGFX_TEXTURE_NONE | BGFX_SAMPLER_NONE, mem);
            texture->Width = width;
            texture->Height = height;
        }
    }

    IndexBufferData::IndexBufferData(const Napi::TypedArray& bytes, uint16_t flags, bool dynamic)
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

    IndexBufferData::~IndexBufferData()
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

    void IndexBufferData::Update(Napi::Env env, const Napi::TypedArray &bytes, uint32_t startingIdx)
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

    void IndexBufferData::SetBgfxIndexBuffer(bgfx::Encoder *encoder, uint32_t firstIndex, uint32_t numIndices) const
    {
        const auto nonDynamic = [&encoder, firstIndex, numIndices](auto handle) {
            encoder->setIndexBuffer(handle, firstIndex, numIndices);
        };
        const auto dynamic = [&encoder, firstIndex, numIndices](auto handle) {
            encoder->setIndexBuffer(handle, firstIndex, numIndices);
        };
        DoForHandleTypes(nonDynamic, dynamic);
    }

    VertexBufferData::VertexBufferData(const Napi::Uint8Array& bytes, bool dynamic)
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

    VertexBufferData::~VertexBufferData()
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

    template<typename sourceType> void VertexBufferData::PromoteToFloats(uint32_t numElements, uint32_t byteOffset, uint32_t byteStride)
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

    void VertexBufferData::PromoteToFloats(bgfx::AttribType::Enum attribType, uint32_t numElements, uint32_t byteOffset, uint32_t byteStride)
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

    void VertexBufferData::EnsureFinalized(Napi::Env /*env*/, const bgfx::VertexLayout& layout)
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

    void VertexBufferData::Update(Napi::Env env, const Napi::Uint8Array& bytes, uint32_t offset, uint32_t byteLength)
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

    void VertexBufferData::SetAsBgfxVertexBuffer(bgfx::Encoder* encoder, uint8_t index, uint32_t startVertex, uint32_t numVertices, bgfx::VertexLayoutHandle layout) const
    {
        const auto nonDynamic = [&encoder, index, startVertex, numVertices, layout](auto handle) {
            encoder->setVertexBuffer(index, handle, startVertex, numVertices, layout);
        };
        const auto dynamic = [&encoder, index, startVertex, numVertices, layout](auto handle) {
            encoder->setVertexBuffer(index, handle, startVertex, numVertices, layout);
        };
        DoForHandleTypes(nonDynamic, dynamic);
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
                InstanceMethod("loadCubeTexture", &NativeEngine::LoadCubeTexture),
                InstanceMethod("copyTexture", &NativeEngine::CopyTexture),
                InstanceMethod("loadCubeTextureWithMips", &NativeEngine::LoadCubeTextureWithMips),
                InstanceMethod("getTextureWidth", &NativeEngine::GetTextureWidth),
                InstanceMethod("getTextureHeight", &NativeEngine::GetTextureHeight),
                InstanceMethod("setTextureSampling", &NativeEngine::SetTextureSampling),
                InstanceMethod("setTextureAnisotropicLevel", &NativeEngine::SetTextureAnisotropicLevel),
                InstanceMethod("deleteTexture", &NativeEngine::DeleteTexture),
                InstanceMethod("createFrameBuffer", &NativeEngine::CreateFrameBuffer),
                InstanceMethod("getRenderWidth", &NativeEngine::GetRenderWidth),
                InstanceMethod("getRenderHeight", &NativeEngine::GetRenderHeight),
                InstanceMethod("setViewPort", &NativeEngine::SetViewPort),
                InstanceMethod("getHardwareScalingLevel", &NativeEngine::GetHardwareScalingLevel),
                InstanceMethod("setHardwareScalingLevel", &NativeEngine::SetHardwareScalingLevel),
                InstanceMethod("createImageBitmap", &NativeEngine::CreateImageBitmap),
                InstanceMethod("resizeImageBitmap", &NativeEngine::ResizeImageBitmap),
                InstanceMethod("getFrameBufferData", &NativeEngine::GetFrameBufferData),
                InstanceMethod("setCommandBuffer", &NativeEngine::SetCommandBuffer),
                InstanceMethod("setCommandUint32Buffer", &NativeEngine::SetCommandUint32Buffer),
                InstanceMethod("setCommandInt32Buffer", &NativeEngine::SetCommandInt32Buffer),
                InstanceMethod("setCommandFloat32Buffer", &NativeEngine::SetCommandFloat32Buffer),
                InstanceMethod("setCommandValidationBuffer", &NativeEngine::SetCommandValidationBuffer),
                InstanceMethod("submitCommandBuffer", &NativeEngine::SubmitCommandBuffer),

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

                InstanceValue("COMMAND_DELETEVERTEXARRAY", Napi::Number::From(env, s_commandTable.Add(&NativeEngine::DeleteVertexArray))),
                InstanceValue("COMMAND_DELETEINDEXBUFFER", Napi::Number::From(env, s_commandTable.Add(&NativeEngine::DeleteIndexBuffer))),
                InstanceValue("COMMAND_DELETEVERTEXBUFFER", Napi::Number::From(env, s_commandTable.Add(&NativeEngine::DeleteVertexBuffer))),
                InstanceValue("COMMAND_SETPROGRAM", Napi::Number::From(env, s_commandTable.Add(&NativeEngine::SetProgram))),
                InstanceValue("COMMAND_SETMATRICES", Napi::Number::From(env, s_commandTable.Add(&NativeEngine::SetMatrices))),
                InstanceValue("COMMAND_SETMATRIX", Napi::Number::From(env, s_commandTable.Add(&NativeEngine::SetMatrix))),
                InstanceValue("COMMAND_SETMATRIX3X3", Napi::Number::From(env, s_commandTable.Add(&NativeEngine::SetMatrix3x3))),
                InstanceValue("COMMAND_SETMATRIX2X2", Napi::Number::From(env, s_commandTable.Add(&NativeEngine::SetMatrix2x2))),
                InstanceValue("COMMAND_SETINT", Napi::Number::From(env, s_commandTable.Add(&NativeEngine::SetInt))),
                InstanceValue("COMMAND_SETINTARRAY", Napi::Number::From(env, s_commandTable.Add(&NativeEngine::SetIntArray))),
                InstanceValue("COMMAND_SETINTARRAY2", Napi::Number::From(env, s_commandTable.Add(&NativeEngine::SetIntArray2))),
                InstanceValue("COMMAND_SETINTARRAY3", Napi::Number::From(env, s_commandTable.Add(&NativeEngine::SetIntArray3))),
                InstanceValue("COMMAND_SETINTARRAY4", Napi::Number::From(env, s_commandTable.Add(&NativeEngine::SetIntArray4))),
                InstanceValue("COMMAND_SETFLOATARRAY", Napi::Number::From(env, s_commandTable.Add(&NativeEngine::SetFloatArray))),
                InstanceValue("COMMAND_SETFLOATARRAY2", Napi::Number::From(env, s_commandTable.Add(&NativeEngine::SetFloatArray2))),
                InstanceValue("COMMAND_SETFLOATARRAY3", Napi::Number::From(env, s_commandTable.Add(&NativeEngine::SetFloatArray3))),
                InstanceValue("COMMAND_SETFLOATARRAY4", Napi::Number::From(env, s_commandTable.Add(&NativeEngine::SetFloatArray4))),
                InstanceValue("COMMAND_SETTEXTURE", Napi::Number::From(env, s_commandTable.Add(&NativeEngine::SetTexture))),
                InstanceValue("COMMAND_BINDVERTEXARRAY", Napi::Number::From(env, s_commandTable.Add(&NativeEngine::BindVertexArray))),
                InstanceValue("COMMAND_SETSTATE", Napi::Number::From(env, s_commandTable.Add(&NativeEngine::SetState))),
                InstanceValue("COMMAND_SETZOFFSET", Napi::Number::From(env, s_commandTable.Add(&NativeEngine::SetZOffset))),
                InstanceValue("COMMAND_SETDEPTHTEST", Napi::Number::From(env, s_commandTable.Add(&NativeEngine::SetDepthTest))),
                InstanceValue("COMMAND_SETDEPTHWRITE", Napi::Number::From(env, s_commandTable.Add(&NativeEngine::SetDepthWrite))),
                InstanceValue("COMMAND_SETCOLORWRITE", Napi::Number::From(env, s_commandTable.Add(&NativeEngine::SetColorWrite))),
                InstanceValue("COMMAND_SETBLENDMODE", Napi::Number::From(env, s_commandTable.Add(&NativeEngine::SetBlendMode))),
                InstanceValue("COMMAND_SETFLOAT", Napi::Number::From(env, s_commandTable.Add(&NativeEngine::SetFloat))),
                InstanceValue("COMMAND_SETFLOAT2", Napi::Number::From(env, s_commandTable.Add(&NativeEngine::SetFloat2))),
                InstanceValue("COMMAND_SETFLOAT3", Napi::Number::From(env, s_commandTable.Add(&NativeEngine::SetFloat3))),
                InstanceValue("COMMAND_SETFLOAT4", Napi::Number::From(env, s_commandTable.Add(&NativeEngine::SetFloat4))),
                InstanceValue("COMMAND_SETTEXTUREWRAPMODE", Napi::Number::From(env, s_commandTable.Add(&NativeEngine::SetTextureWrapMode))),
                InstanceValue("COMMAND_BINDFRAMEBUFFER", Napi::Number::From(env, s_commandTable.Add(&NativeEngine::BindFrameBuffer))),
                InstanceValue("COMMAND_UNBINDFRAMEBUFFER", Napi::Number::From(env, s_commandTable.Add(&NativeEngine::UnbindFrameBuffer))),
                InstanceValue("COMMAND_DELETEFRAMEBUFFER", Napi::Number::From(env, s_commandTable.Add(&NativeEngine::DeleteFrameBuffer))),
                InstanceValue("COMMAND_DRAWINDEXED", Napi::Number::From(env, s_commandTable.Add(&NativeEngine::DrawIndexed))),
                InstanceValue("COMMAND_DRAW", Napi::Number::From(env, s_commandTable.Add(&NativeEngine::Draw))),
                InstanceValue("COMMAND_CLEAR", Napi::Number::From(env, s_commandTable.Add(&NativeEngine::Clear))),
                InstanceValue("COMMAND_SETSTENCIL", Napi::Number::From(env, s_commandTable.Add(&NativeEngine::SetStencil))),

                InstanceValue("COMMAND_VALIDATION_COMMAND", Napi::Number::From(env, CommandBufferDecoder::COMMAND_VALIDATION_COMMAND)),
                InstanceValue("COMMAND_VALIDATION_UINT32", Napi::Number::From(env, CommandBufferDecoder::COMMAND_VALIDATION_UINT32)),
                InstanceValue("COMMAND_VALIDATION_INT32", Napi::Number::From(env, CommandBufferDecoder::COMMAND_VALIDATION_INT32)),
                InstanceValue("COMMAND_VALIDATION_FLOAT", Napi::Number::From(env, CommandBufferDecoder::COMMAND_VALIDATION_FLOAT))
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
        for (const auto handle : m_programDataCollection)
        {
            ProgramData::Delete(handle);
        }
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
        return Napi::Value::From(info.Env(), VertexArray::Create());
    }

    void NativeEngine::DeleteVertexArray(CommandBufferDecoder& decoder)
    {
        VertexArray::Delete(decoder.DecodeCommandArgAsUInt32());
        // TODO: should we clear the m_boundVertexArray if it gets deleted?
        //assert(vertexArray != m_boundVertexArray);
    }

    void NativeEngine::BindVertexArray(CommandBufferDecoder& decoder)
    {
        const VertexArray& vertexArray = VertexArray::Get(decoder.DecodeCommandArgAsUInt32());
        m_boundVertexArray = &vertexArray;
    }

    Napi::Value NativeEngine::CreateIndexBuffer(const Napi::CallbackInfo& info)
    {
        const Napi::TypedArray data = info[0].As<Napi::TypedArray>();
        const bool dynamic = info[1].As<Napi::Boolean>().Value();

        const uint16_t flags = data.TypedArrayType() == napi_typedarray_type::napi_uint16_array ? 0 : BGFX_BUFFER_INDEX32;

        return Napi::Value::From(info.Env(), IndexBufferData::Create(data, flags, dynamic));
    }

    void NativeEngine::DeleteIndexBuffer(CommandBufferDecoder& decoder)
    {
        IndexBufferData::Delete(decoder.DecodeCommandArgAsUInt32());
    }

    void NativeEngine::RecordIndexBuffer(const Napi::CallbackInfo& info)
    {
        VertexArray& vertexArray = VertexArray::Get(info[0].ToNumber().Uint32Value());
        const IndexBufferData& indexBufferData = IndexBufferData::Get(info[1].ToNumber().Uint32Value());

        vertexArray.indexBuffer.Data = &indexBufferData;
    }

    void NativeEngine::UpdateDynamicIndexBuffer(const Napi::CallbackInfo& info)
    {
        IndexBufferData& indexBufferData = IndexBufferData::Get(info[0].ToNumber().Uint32Value());

        const Napi::TypedArray data = info[1].As<Napi::TypedArray>();
        const uint32_t startingIdx = info[2].As<Napi::Number>().Uint32Value();

        indexBufferData.Update(info.Env(), data, startingIdx);
    }

    Napi::Value NativeEngine::CreateVertexBuffer(const Napi::CallbackInfo& info)
    {
        const Napi::Uint8Array data = info[0].As<Napi::Uint8Array>();
        const bool dynamic = info[1].As<Napi::Boolean>().Value();

        return Napi::Value::From(info.Env(), VertexBufferData::Create(data, dynamic));
    }

    void NativeEngine::DeleteVertexBuffer(CommandBufferDecoder& decoder)
    {
        VertexBufferData::Delete(decoder.DecodeCommandArgAsUInt32());
    }

    void NativeEngine::RecordVertexBuffer(const Napi::CallbackInfo& info)
    {
        VertexArray& vertexArray = VertexArray::Get(info[0].ToNumber().Uint32Value());
        VertexBufferData& vertexBufferData = VertexBufferData::Get(info[1].ToNumber().Uint32Value());

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
            vertexBufferData.PromoteToFloats(attribType, numElements, byteOffset, byteStride);
        }
        else
        {
            vertexLayout.add(attrib, static_cast<uint8_t>(numElements), attribType, normalized);
            vertexLayout.m_stride = static_cast<uint16_t>(byteStride);
            vertexLayout.m_offset[attrib] = static_cast<uint16_t>(byteOffset % byteStride);
        }

        vertexLayout.end();
        vertexBufferData.EnsureFinalized(info.Env(), vertexLayout);

        // Second parameter is first vertex. byteOffset and byteStride are both using original values (without float promotion)
        vertexArray.VertexBuffers[location] = {&vertexBufferData, byteOffset / byteStride, bgfx::createVertexLayout(vertexLayout)};
    }

    void NativeEngine::UpdateDynamicVertexBuffer(const Napi::CallbackInfo& info)
    {
        VertexBufferData& vertexBufferData = VertexBufferData::Get(info[0].ToNumber().Uint32Value());
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

        uint32_t programDataHandle{ProgramData::Create()};
        ProgramData& programData{ProgramData::Get(programDataHandle)};
        ShaderCompiler::BgfxShaderInfo shaderInfo{};

        try
        {
            shaderInfo = m_shaderCompiler.Compile(ProcessShaderCoordinates(vertexSource), ProcessSamplerFlip(fragmentSource));
        }
        catch (const std::exception& ex)
        {
            throw Napi::Error::New(info.Env(), ex.what());
        }

        static auto InitUniformInfos{[](bgfx::ShaderHandle shader, const std::unordered_map<std::string, uint8_t>& uniformStages, std::unordered_map<std::string, uint32_t>& uniformInfos) {
            auto numUniforms = bgfx::getShaderUniforms(shader);
            std::vector<bgfx::UniformHandle> uniforms{numUniforms};
            bgfx::getShaderUniforms(shader, uniforms.data(), gsl::narrow_cast<uint16_t>(uniforms.size()));

            for (uint8_t index = 0; index < numUniforms; index++)
            {
                bgfx::UniformInfo info{};
                bgfx::getUniformInfo(uniforms[index], info);
                auto itStage = uniformStages.find(info.name);
                uniformInfos[info.name] = UniformInfo::Create(itStage == uniformStages.end() ? uint8_t{} : itStage->second, uniforms[index]);
            }
        }};

        auto vertexShader = bgfx::createShader(bgfx::copy(shaderInfo.VertexBytes.data(), static_cast<uint32_t>(shaderInfo.VertexBytes.size())));
        InitUniformInfos(vertexShader, shaderInfo.VertexUniformStages, programData.VertexUniformInfos);
        programData.VertexAttributeLocations = std::move(shaderInfo.VertexAttributeLocations);

        auto fragmentShader = bgfx::createShader(bgfx::copy(shaderInfo.FragmentBytes.data(), static_cast<uint32_t>(shaderInfo.FragmentBytes.size())));
        InitUniformInfos(fragmentShader, shaderInfo.FragmentUniformStages, programData.FragmentUniformInfos);

        programData.Handle = bgfx::createProgram(vertexShader, fragmentShader, true);
        m_programDataCollection.insert(programDataHandle);
        return Napi::Value::From(info.Env(), programDataHandle);
    }

    // TODO: Add a DeleteProgram, maybe called from NativeEngine._releaseEffect (on the JS side)?

    Napi::Value NativeEngine::GetUniforms(const Napi::CallbackInfo& info)
    {
        const auto& program = ProgramData::Get(info[0].ToNumber().Uint32Value());
        const auto names = info[1].As<Napi::Array>();

        auto length = names.Length();
        auto uniforms = Napi::Array::New(info.Env(), length);
        for (uint32_t index = 0; index < length; ++index)
        {
            const auto name = names[index].IsString() ? names[index].As<Napi::String>().Utf8Value() : "";

            auto vertexFound = program.VertexUniformInfos.find(name);
            auto fragmentFound = program.FragmentUniformInfos.find(name);

            if (vertexFound != program.VertexUniformInfos.end())
            {
                uniforms[index] = Napi::Value::From(info.Env(), vertexFound->second);
            }
            else if (fragmentFound != program.FragmentUniformInfos.end())
            {
                uniforms[index] = Napi::Value::From(info.Env(), fragmentFound->second);
            }
            else
            {
                uniforms[index] = info.Env().Null();
            }
        }

        return std::move(uniforms);
    }

    Napi::Value NativeEngine::GetAttributes(const Napi::CallbackInfo& info)
    {
        const auto& program = ProgramData::Get(info[0].ToNumber().Uint32Value());
        const auto names = info[1].As<Napi::Array>();

        const auto& attributeLocations = program.VertexAttributeLocations;

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

    void NativeEngine::SetProgram(CommandBufferDecoder& decoder)
    {
        m_currentProgram = &ProgramData::Get(decoder.DecodeCommandArgAsUInt32());
    }

    void NativeEngine::SetState(CommandBufferDecoder& decoder)
    {
        const bool culling = decoder.DecodeCommandArgAsUInt32();
        // TODO: zOffset
        /*const float zOffset =*/ decoder.DecodeCommandArgAsFloat32();
        const bool cullBackFaces = decoder.DecodeCommandArgAsUInt32();
        const bool reverseSide = decoder.DecodeCommandArgAsUInt32();

        m_engineState &= ~(BGFX_STATE_CULL_MASK | BGFX_STATE_FRONT_CCW);
        m_engineState |= reverseSide ? 0 : BGFX_STATE_FRONT_CCW;

        if (culling)
        {
            m_engineState |= cullBackFaces ? BGFX_STATE_CULL_CCW : BGFX_STATE_CULL_CW;
        }
    }

    void NativeEngine::SetZOffset(CommandBufferDecoder& decoder)
    {
        /*const auto zOffset =*/ decoder.DecodeCommandArgAsFloat32();

        // STUB: Stub.
    }

    void NativeEngine::SetDepthTest(CommandBufferDecoder& decoder)
    {
        const auto depthTest = decoder.DecodeCommandArgAsUInt32();

        m_engineState &= ~BGFX_STATE_DEPTH_TEST_MASK;
        m_engineState |= depthTest;
    }

    void NativeEngine::SetDepthWrite(CommandBufferDecoder& decoder)
    {
        const auto enable = static_cast<bool>(decoder.DecodeCommandArgAsUInt32());

        m_engineState &= ~BGFX_STATE_WRITE_Z;
        m_engineState |= enable ? BGFX_STATE_WRITE_Z : 0;
    }

    void NativeEngine::SetColorWrite(CommandBufferDecoder& decoder)
    {
        const auto enable = static_cast<bool>(decoder.DecodeCommandArgAsUInt32());

        m_engineState &= ~(BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A);
        m_engineState |= enable ? (BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A) : 0;
    }

    void NativeEngine::SetBlendMode(CommandBufferDecoder& decoder)
    {
        const auto blendMode = decoder.DecodeCommandArgAsUInt32();

        m_engineState &= ~BGFX_STATE_BLEND_MASK;
        m_engineState |= blendMode;
    }

    void NativeEngine::SetInt(CommandBufferDecoder& decoder)
    {
        const auto& uniformInfo{UniformInfo::Get(decoder.DecodeCommandArgAsUInt32())};
        const auto value{static_cast<float>(decoder.DecodeCommandArgAsInt32())};
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
    void NativeEngine::SetFloatN(CommandBufferDecoder& decoder)
    {
        const auto& uniformInfo = UniformInfo::Get(decoder.DecodeCommandArgAsUInt32());
        const float values[] = {
            decoder.DecodeCommandArgAsFloat32(),
            (size > 1) ? decoder.DecodeCommandArgAsFloat32() : 0.f,
            (size > 2) ? decoder.DecodeCommandArgAsFloat32() : 0.f,
            (size > 3) ? decoder.DecodeCommandArgAsFloat32() : 0.f,
        };

        m_currentProgram->SetUniform(uniformInfo.Handle, values);
    }

    template<int size>
    void NativeEngine::SetMatrixN(CommandBufferDecoder& decoder)
    {
        const auto& uniformInfo{UniformInfo::Get(decoder.DecodeCommandArgAsUInt32())};
        const auto elementLength{decoder.DecodeCommandArgAsUInt32()};
        const auto matrix{decoder.DecodeCommandArgAsFloat32s(elementLength)};

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
            m_currentProgram->SetUniform(uniformInfo.Handle, gsl::make_span(matrixValues.data(), 16));
        }
        else
        {
            m_currentProgram->SetUniform(uniformInfo.Handle, gsl::make_span(matrix.data(), elementLength));
        }
    }

    template<int size>
    void NativeEngine::SetIntArrayN(CommandBufferDecoder& decoder)
    {
        const auto& uniformInfo{UniformInfo::Get(decoder.DecodeCommandArgAsUInt32())};
        const auto elementLength{decoder.DecodeCommandArgAsUInt32()};
        const auto array{decoder.DecodeCommandArgAsInt32s(elementLength)};
        SetTypeArrayN<size>(uniformInfo, elementLength, array);
    }

    void NativeEngine::SetIntArray(CommandBufferDecoder& decoder)
    {
        SetIntArrayN<1>(decoder);
    }

    void NativeEngine::SetIntArray2(CommandBufferDecoder& decoder)
    {
        SetIntArrayN<2>(decoder);
    }

    void NativeEngine::SetIntArray3(CommandBufferDecoder& decoder)
    {
        SetIntArrayN<3>(decoder);
    }

    void NativeEngine::SetIntArray4(CommandBufferDecoder& decoder)
    {
        SetIntArrayN<4>(decoder);
    }

    template<int size>
    void NativeEngine::SetFloatArrayN(CommandBufferDecoder& decoder)
    {
        const auto& uniformInfo{UniformInfo::Get(decoder.DecodeCommandArgAsUInt32())};
        const auto elementLength{decoder.DecodeCommandArgAsUInt32()};
        const auto array{decoder.DecodeCommandArgAsFloat32s(elementLength)};
        SetTypeArrayN<size>(uniformInfo, elementLength, array);
    }

    void NativeEngine::SetFloatArray(CommandBufferDecoder& decoder)
    {
        SetFloatArrayN<1>(decoder);
    }

    void NativeEngine::SetFloatArray2(CommandBufferDecoder& decoder)
    {
        SetFloatArrayN<2>(decoder);
    }

    void NativeEngine::SetFloatArray3(CommandBufferDecoder& decoder)
    {
        SetFloatArrayN<3>(decoder);
    }

    void NativeEngine::SetFloatArray4(CommandBufferDecoder& decoder)
    {
        SetFloatArrayN<4>(decoder);
    }

    void NativeEngine::SetMatrices(CommandBufferDecoder& decoder)
    {
        auto uniformHandle{decoder.DecodeCommandArgAsUInt32()};
        auto length{decoder.DecodeCommandArgAsUInt32()};
        auto matrices{decoder.DecodeCommandArgAsFloat32s(length)};

        assert(length % 16 == 0);

        m_currentProgram->SetUniform(UniformInfo::Get(uniformHandle).Handle, matrices, length / 16);
    }

    void NativeEngine::SetMatrix2x2(CommandBufferDecoder& decoder)
    {
        SetMatrixN<2>(decoder);
    }

    void NativeEngine::SetMatrix3x3(CommandBufferDecoder& decoder)
    {
        SetMatrixN<3>(decoder);
    }

    void NativeEngine::SetMatrix(CommandBufferDecoder& decoder)
    {
        SetMatrixN<4>(decoder);
    }

    void NativeEngine::SetFloat(CommandBufferDecoder& decoder)
    {
        SetFloatN<1>(decoder);
    }

    void NativeEngine::SetFloat2(CommandBufferDecoder& decoder)
    {
        SetFloatN<2>(decoder);
    }

    void NativeEngine::SetFloat3(CommandBufferDecoder& decoder)
    {
        SetFloatN<3>(decoder);
    }

    void NativeEngine::SetFloat4(CommandBufferDecoder& decoder)
    {
        SetFloatN<4>(decoder);
    }

    Napi::Value NativeEngine::CreateTexture(const Napi::CallbackInfo& info)
    {
        return Napi::Value::From(info.Env(), TextureData::Create());
    }

    void NativeEngine::LoadTexture(const Napi::CallbackInfo& info)
    {
        const auto texture = &TextureData::Get(info[0].ToNumber().Uint32Value());
        const auto data = info[1].As<Napi::TypedArray>();
        const auto generateMips = info[2].As<Napi::Boolean>().Value();
        auto invertY = info[3].As<Napi::Boolean>().Value();
        const auto onSuccess = info[4].As<Napi::Function>();
        const auto onError = info[5].As<Napi::Function>();

        const auto dataSpan = gsl::make_span(static_cast<uint8_t*>(data.ArrayBuffer().Data()) + data.ByteOffset(), data.ByteLength());

        if (!bgfx::getCaps()->originBottomLeft)
        {
            invertY = !invertY;
        }

        arcana::make_task(arcana::threadpool_scheduler, *m_cancellationSource,
            [this, dataSpan, generateMips, invertY, texture, cancellationSource{m_cancellationSource}]() {
                bimg::ImageContainer* image = bimg::imageParse(&m_allocator, dataSpan.data(), static_cast<uint32_t>(dataSpan.size()));
                if (image == nullptr)
                {
                    throw std::runtime_error("Unable to decode image."); // exception will be forwarded to JS
                }

                if (image->m_format == bimg::TextureFormat::R8)
                {
                    // Images with only 1 channel are interpreted as luminance texture with RGB containing the same value as R and alpha as 255
                    // To emulate this behavior, the format is switched to alpha8 and converted to RGB8 when packing and unpacking take care of
                    // component swizzling.
                    image->m_format = bimg::TextureFormat::A8;
                    bimg::ImageContainer* rgba = bimg::imageConvert(&m_allocator, bimg::TextureFormat::RGB8, *image, false);
                    bimg::imageFree(image);
                    image = rgba;
                }

                if (invertY)
                {
                    FlipY(image);
                }

                if (generateMips)
                {
                    GenerateMips(&m_allocator, &image);
                }

                CreateTextureFromImage(texture, image);
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
        const auto textureDestination = &TextureData::Get(info[0].ToNumber().Uint32Value());
        const auto textureSource = &TextureData::Get(info[1].ToNumber().Uint32Value());
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
        const auto texture{&TextureData::Get(info[0].ToNumber().Uint32Value())};
        const auto data{info[1].As<Napi::TypedArray>()};
        const auto width{static_cast<uint16_t>(info[2].As<Napi::Number>().Uint32Value())};
        const auto height{static_cast<uint16_t>(info[3].As<Napi::Number>().Uint32Value())};
        const auto format{static_cast<bimg::TextureFormat::Enum>(info[4].As<Napi::Number>().Uint32Value())};
        const auto generateMips{info[5].As<Napi::Boolean>().Value()};
        auto invertY{info[6].As<Napi::Boolean>().Value()};

        const auto bytes{static_cast<uint8_t*>(data.ArrayBuffer().Data()) + data.ByteOffset()};
        if (data.ByteLength() != bimg::imageGetSize(nullptr, width, height, 1, false, false, 1, format))
        {
            throw std::runtime_error{"The data size does not match width, height, and format"};
        }

        bimg::ImageContainer* image{bimg::imageAlloc(&m_allocator, format, width, height, 1, 1, false, false, bytes)};

        if (!bgfx::getCaps()->originBottomLeft)
        {
            invertY = !invertY;
        }

        if (invertY)
        {
            FlipY(image);
        }

        if (generateMips)
        {
            GenerateMips(&m_allocator, &image);
        }

        CreateTextureFromImage(texture, image);
    }

    void NativeEngine::LoadCubeTexture(const Napi::CallbackInfo& info)
    {
        const auto texture = &TextureData::Get(info[0].ToNumber().Uint32Value());
        const auto data = info[1].As<Napi::Array>();
        const auto generateMips = info[2].As<Napi::Boolean>().Value();
        const auto onSuccess = info[3].As<Napi::Function>();
        const auto onError = info[4].As<Napi::Function>();

        std::array<Napi::Reference<Napi::TypedArray>, 6> dataRefs;
        std::array<arcana::task<bimg::ImageContainer*, std::exception_ptr>, 6> tasks;
        for (uint32_t face = 0; face < data.Length(); face++)
        {
            const auto typedArray{data[face].As<Napi::TypedArray>()};
            const auto dataSpan{gsl::make_span(static_cast<uint8_t*>(typedArray.ArrayBuffer().Data()) + typedArray.ByteOffset(), typedArray.ByteLength())};
            dataRefs[face] = Napi::Persistent(typedArray);
            tasks[face] = arcana::make_task(arcana::threadpool_scheduler, *m_cancellationSource, [this, dataSpan, generateMips]() {
                bimg::ImageContainer* image = bimg::imageParse(&m_allocator, dataSpan.data(), static_cast<uint32_t>(dataSpan.size()));
                // if texture is R8, it needs to be converted as luminance (r=g=b=luminance and alpha = 1)
                // see what's done in loadTexture
                // keeping an assert here until we find some assets to test.
                assert(image->m_format != bimg::TextureFormat::R8);
                if (generateMips)
                {
                    GenerateMips(&m_allocator, &image);
                }
                return image;
            });
        }

        arcana::when_all(gsl::make_span(tasks))
            .then(arcana::inline_scheduler, *m_cancellationSource, [texture, generateMips, cancellationSource{m_cancellationSource}](std::vector<bimg::ImageContainer*> images) {
                CreateCubeTextureFromImages(texture, images, generateMips);
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
        const auto texture = &TextureData::Get(info[0].ToNumber().Uint32Value());
        const auto data = info[1].As<Napi::Array>();
        const auto onSuccess = info[2].As<Napi::Function>();
        const auto onError = info[3].As<Napi::Function>();

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
                tasks[(face * numMips) + mip] = arcana::make_task(arcana::threadpool_scheduler, *m_cancellationSource, [this, dataSpan, cancellationSource{m_cancellationSource}]() {
                    bimg::ImageContainer* image = bimg::imageParse(&m_allocator, dataSpan.data(), static_cast<uint32_t>(dataSpan.size()));
                    assert(image->m_format != bimg::TextureFormat::R8);
                    FlipY(image);
                    return image;
                });
            }
        }

        arcana::when_all(gsl::make_span(tasks))
            .then(arcana::inline_scheduler, *m_cancellationSource, [texture, cancellationSource{m_cancellationSource}](std::vector<bimg::ImageContainer*> images) {
                CreateCubeTextureFromImages(texture, images, true);
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
        const auto& texture = TextureData::Get(info[0].ToNumber().Uint32Value());
        return Napi::Value::From(info.Env(), texture.Width);
    }

    Napi::Value NativeEngine::GetTextureHeight(const Napi::CallbackInfo& info)
    {
        const auto& texture = TextureData::Get(info[0].ToNumber().Uint32Value());
        return Napi::Value::From(info.Env(), texture.Height);
    }

    void NativeEngine::SetTextureSampling(const Napi::CallbackInfo& info)
    {
        auto& texture = TextureData::Get(info[0].ToNumber().Uint32Value());
        auto filter = static_cast<uint32_t>(info[1].As<Napi::Number>().Uint32Value());

        texture.Flags &= ~(BGFX_SAMPLER_MIN_MASK | BGFX_SAMPLER_MAG_MASK | BGFX_SAMPLER_MIP_MASK);

        if (texture.AnisotropicLevel > 1)
        {
            texture.Flags |= BGFX_SAMPLER_MIN_ANISOTROPIC | BGFX_SAMPLER_MAG_ANISOTROPIC;
        }
        else
        {
            texture.Flags |= filter;
        }
    }

    void NativeEngine::SetTextureWrapMode(CommandBufferDecoder& decoder)
    {
        auto& texture = TextureData::Get(decoder.DecodeCommandArgAsUInt32());
        auto addressModeU = decoder.DecodeCommandArgAsUInt32();
        auto addressModeV = decoder.DecodeCommandArgAsUInt32();
        auto addressModeW = decoder.DecodeCommandArgAsUInt32();

        uint32_t addressMode = addressModeU +
            (addressModeV << BGFX_SAMPLER_V_SHIFT) +
            (addressModeW << BGFX_SAMPLER_W_SHIFT);

        texture.Flags &= ~(BGFX_SAMPLER_U_MASK | BGFX_SAMPLER_V_MASK | BGFX_SAMPLER_W_MASK);
        texture.Flags |= addressMode;
    }

    void NativeEngine::SetTextureAnisotropicLevel(const Napi::CallbackInfo& info)
    {
        auto& texture = TextureData::Get(info[0].ToNumber().Uint32Value());
        const auto value = info[1].As<Napi::Number>().Uint32Value();

        texture.AnisotropicLevel = static_cast<uint8_t>(value);

        // if Anisotropic is set to 0 after being >1, then set texture flags back to linear
        texture.Flags &= ~(BGFX_SAMPLER_MIN_MASK | BGFX_SAMPLER_MAG_MASK | BGFX_SAMPLER_MIP_MASK);
        if (value)
        {
            texture.Flags |= BGFX_SAMPLER_MIN_ANISOTROPIC | BGFX_SAMPLER_MAG_ANISOTROPIC;
        }
    }

    void NativeEngine::SetTexture(CommandBufferDecoder& decoder)
    {
        bgfx::Encoder* encoder{GetUpdateToken().GetEncoder()};

        const auto uniformHandle{decoder.DecodeCommandArgAsUInt32()};
        const auto textureHandle{decoder.DecodeCommandArgAsUInt32()};

        const auto& uniformInfo{UniformInfo::Get(uniformHandle)};
        const auto& texture{TextureData::Get(textureHandle)};

        encoder->setTexture(uniformInfo.Stage, uniformInfo.Handle, texture.Handle, texture.Flags);
    }

    void NativeEngine::DeleteTexture(const Napi::CallbackInfo& info)
    {
        const auto& texture = TextureData::Get(info[0].ToNumber().Uint32Value());
        m_graphicsImpl.RemoveTexture(texture.Handle);
    }

    Napi::Value NativeEngine::CreateFrameBuffer(const Napi::CallbackInfo& info)
    {
        auto& texture = TextureData::Get(info[0].ToNumber().Uint32Value());
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

        texture.Handle = bgfx::getTexture(frameBufferHandle);
        texture.OwnsHandle = false;

        return Napi::Value::From(info.Env(), FrameBuffer::Create(m_graphicsImpl, frameBufferHandle, width, height, false, generateDepth, generateStencilBuffer));
    }

    // TODO: This doesn't get called when an Engine instance is disposed.
    void NativeEngine::DeleteFrameBuffer(CommandBufferDecoder& decoder)
    {
        FrameBuffer::Delete(decoder.DecodeCommandArgAsUInt32());
    }

    void NativeEngine::BindFrameBuffer(CommandBufferDecoder& decoder)
    {
        auto& frameBuffer{FrameBuffer::Get(decoder.DecodeCommandArgAsUInt32())};
        auto* encoder = GetUpdateToken().GetEncoder();

        m_boundFrameBuffer->Unbind(*encoder);
        m_boundFrameBuffer = &frameBuffer;
        m_boundFrameBuffer->Bind(*encoder);
        m_boundFrameBufferNeedsRebinding.Set(*encoder, false);
    }

    void NativeEngine::UnbindFrameBuffer(CommandBufferDecoder& decoder)
    {
        const auto& frameBuffer{FrameBuffer::Get(decoder.DecodeCommandArgAsUInt32())};
        auto* encoder = GetUpdateToken().GetEncoder();

        assert(&frameBuffer == m_boundFrameBuffer);
        UNUSED(frameBuffer);

        m_boundFrameBuffer->Unbind(*encoder);
        m_boundFrameBuffer = nullptr;
        m_boundFrameBufferNeedsRebinding.Set(*encoder, false);
    }

    void NativeEngine::DrawIndexed(CommandBufferDecoder& decoder)
    {
        bgfx::Encoder* encoder{GetUpdateToken().GetEncoder()};

        const auto fillMode = decoder.DecodeCommandArgAsUInt32();
        const auto indexStart = decoder.DecodeCommandArgAsUInt32();
        const auto indexCount = decoder.DecodeCommandArgAsUInt32();

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

    void NativeEngine::Draw(CommandBufferDecoder& decoder)
    {
        bgfx::Encoder* encoder{GetUpdateToken().GetEncoder()};

        const auto fillMode = decoder.DecodeCommandArgAsUInt32();
        const auto verticesStart = decoder.DecodeCommandArgAsUInt32();
        const auto verticesCount = decoder.DecodeCommandArgAsUInt32();

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

    void NativeEngine::Clear(CommandBufferDecoder& decoder)
    {
        bgfx::Encoder* encoder{GetUpdateToken().GetEncoder()};

        uint16_t flags{0};
        uint32_t rgba{0x000000ff};

        const bool shouldClearColor{static_cast<bool>(decoder.DecodeCommandArgAsUInt32())};
        const float r{decoder.DecodeCommandArgAsFloat32()};
        const float g{decoder.DecodeCommandArgAsFloat32()};
        const float b{decoder.DecodeCommandArgAsFloat32()};
        const float a{decoder.DecodeCommandArgAsFloat32()};
        const bool shouldClearDepth{static_cast<bool>(decoder.DecodeCommandArgAsUInt32())};
        const float depth{decoder.DecodeCommandArgAsFloat32()};
        const bool shouldClearStencil{static_cast<bool>(decoder.DecodeCommandArgAsUInt32())};
        const uint8_t stencil{static_cast<uint8_t>(decoder.DecodeCommandArgAsUInt32())};

        if (shouldClearColor)
        {
            rgba =
                (static_cast<uint8_t>(r * std::numeric_limits<uint8_t>::max()) << 24) +
                (static_cast<uint8_t>(g * std::numeric_limits<uint8_t>::max()) << 16) +
                (static_cast<uint8_t>(b * std::numeric_limits<uint8_t>::max()) << 8) +
                static_cast<uint8_t>(a * std::numeric_limits<uint8_t>::max());

            flags |= BGFX_CLEAR_COLOR;
        }

        if (shouldClearDepth && m_boundFrameBuffer->HasDepth())
        {
            flags |= BGFX_CLEAR_DEPTH;
        }

        if (shouldClearStencil && m_boundFrameBuffer->HasStencil())
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

        const auto data = info[0].As<Napi::ArrayBuffer>();
        if (!data.ByteLength())
        {
            throw Napi::Error::New(env, "CreateImageBitmap array buffer is empty.");
        }

        bimg::ImageContainer* image = bimg::imageParse(&m_allocator, data.Data(), static_cast<uint32_t>(data.ByteLength()));
        if (image == nullptr)
        {
            throw Napi::Error::New(env, "Unable to decode image in createImageBitmap function.");
        }

        Napi::Object imageBitmap = Napi::Object::New(env);
        auto buffer = Napi::Uint8Array::New(env, image->m_size);
        memcpy(buffer.Data(), image->m_data, image->m_size);

        imageBitmap.Set("data", buffer);
        imageBitmap.Set("width", Napi::Number::New(env, image->m_width).As<Napi::Value>());
        imageBitmap.Set("height", Napi::Number::New(env, image->m_height).As<Napi::Value>());
        imageBitmap.Set("depth", Napi::Number::New(env, image->m_depth).As<Napi::Value>());
        imageBitmap.Set("numLayers", Napi::Number::New(env, image->m_numLayers).As<Napi::Value>());
        imageBitmap.Set("format", Napi::Number::New(env, image->m_format).As<Napi::Value>());

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
            memcpy(outputData.Data(), image->m_data, image->m_size);
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

    void NativeEngine::SetStencil(CommandBufferDecoder& decoder)
    {
        const auto writeMask{decoder.DecodeCommandArgAsUInt32()};
        const auto stencilOpFail{decoder.DecodeCommandArgAsUInt32()};
        const auto depthOpFail{decoder.DecodeCommandArgAsUInt32()};
        const auto depthOpPass{decoder.DecodeCommandArgAsUInt32()};
        const auto func{decoder.DecodeCommandArgAsUInt32()};
        const auto ref{decoder.DecodeCommandArgAsUInt32()};

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

    void NativeEngine::SetCommandBuffer(const Napi::CallbackInfo& info)
    {
        m_commandBuffer = Napi::Persistent(info[0].As<Napi::Uint8Array>());
    }

    void NativeEngine::SetCommandUint32Buffer(const Napi::CallbackInfo& info)
    {
        m_commandUint32Buffer = Napi::Persistent(info[0].As<Napi::Uint32Array>());
    }

    void NativeEngine::SetCommandInt32Buffer(const Napi::CallbackInfo& info)
    {
        m_commandInt32Buffer = Napi::Persistent(info[0].As<Napi::Int32Array>());
    }

    void NativeEngine::SetCommandFloat32Buffer(const Napi::CallbackInfo& info)
    {
        m_commandFloat32Buffer = Napi::Persistent(info[0].As<Napi::Float32Array>());
    }

    void NativeEngine::SetCommandValidationBuffer(const Napi::CallbackInfo& info)
    {
        m_commandValidationBuffer = Napi::Persistent(info[0].As<Napi::Uint8Array>());
    }

    void NativeEngine::SubmitCommandBuffer(const Napi::CallbackInfo& info)
    {
        const auto commandCount{info[0].ToNumber().Uint32Value()};

        std::optional<CommandBufferDecoder::UInt8Buffer> validationBuffer{};
        if (m_commandValidationBuffer) {
            validationBuffer = gsl::make_span(m_commandValidationBuffer.Value().Data(), m_commandValidationBuffer.Value().ElementLength());
        }

        CommandBufferDecoder commandBufferDecoder{
            gsl::make_span(m_commandBuffer.Value().Data(), commandCount),
            gsl::make_span(m_commandUint32Buffer.Value().Data(), m_commandUint32Buffer.Value().ElementLength()),
            gsl::make_span(m_commandInt32Buffer.Value().Data(), m_commandInt32Buffer.Value().ElementLength()),
            gsl::make_span(m_commandFloat32Buffer.Value().Data(), m_commandFloat32Buffer.Value().ElementLength()),
            validationBuffer
        };

        uint8_t command{};
        while (commandBufferDecoder.TryDecodeCommand(command))
        {
            std::invoke(s_commandTable.Get(command), this, commandBufferDecoder);
        }
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
