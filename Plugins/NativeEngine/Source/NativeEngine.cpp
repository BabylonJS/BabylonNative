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
                InstanceAccessor("homogeneousDepth", &NativeEngine::HomogeneousDepth, nullptr),
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

                InstanceValue("ATTRIB_TYPE_UINT8", Napi::Number::From(env, static_cast<uint32_t>(bgfx::AttribType::Uint8))),
                InstanceValue("ATTRIB_TYPE_INT16", Napi::Number::From(env, static_cast<uint32_t>(bgfx::AttribType::Int16))),
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
        , m_graphicsImpl{Graphics::Impl::GetFromJavaScript(info.Env())}
        , m_runtimeScheduler{runtime}
        , m_boundFrameBuffer{&m_graphicsImpl.DefaultFrameBuffer()}
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

    Napi::Value NativeEngine::HomogeneousDepth(const Napi::CallbackInfo& info)
    {
        return Napi::Value::From(info.Env(), bgfx::getCaps()->homogeneousDepth);
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
        vertexLayout.add(attrib, static_cast<uint8_t>(numElements), attribType, normalized);
        vertexLayout.m_stride = static_cast<uint16_t>(byteStride);
        vertexLayout.end();

        vertexBufferData->EnsureFinalized(info.Env(), vertexLayout);

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

    Napi::Value NativeEngine::CreateProgram(const Napi::CallbackInfo& info)
    {
        const std::string vertexSource{info[0].As<Napi::String>().Utf8Value()};
        const std::string fragmentSource{info[1].As<Napi::String>().Utf8Value()};

        std::unique_ptr<ProgramData> programData{std::make_unique<ProgramData>()};
        ShaderCompiler::BgfxShaderInfo shaderInfo{};

        try
        {
            shaderInfo = m_shaderCompiler.Compile(vertexSource, fragmentSource);
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
                bool YFlip{false};
                if (!bgfx::getCaps()->originBottomLeft)
                {
                    YFlip = (!strcmp(info.name, "projection")) || (!strcmp(info.name, "viewProjection"));
                }
                uniformInfos[info.name].YFlip = YFlip;
            }
        }};

        auto vertexShader = bgfx::createShader(bgfx::copy(shaderInfo.VertexBytes.data(), static_cast<uint32_t>(shaderInfo.VertexBytes.size())));
        InitUniformInfos(vertexShader, shaderInfo.VertexUniformStages, programData->VertexUniformInfos);
        programData->VertexAttributeLocations = std::move(shaderInfo.VertexAttributeLocations);

        auto fragmentShader = bgfx::createShader(bgfx::copy(shaderInfo.FragmentBytes.data(), static_cast<uint32_t>(shaderInfo.FragmentBytes.size())));
        InitUniformInfos(fragmentShader, shaderInfo.FragmentUniformStages, programData->FragmentUniformInfos);

        programData->Handle = bgfx::createProgram(vertexShader, fragmentShader, true);
        auto* rawProgramData = programData.get();
        auto ticket = m_programDataCollection.insert(std::move(programData));
        auto finalizer = [ticket = std::move(ticket)](Napi::Env, ProgramData*) {};
        return Napi::External<ProgramData>::New(info.Env(), rawProgramData, std::move(finalizer));
    }

    Napi::Value NativeEngine::GetUniforms(const Napi::CallbackInfo& info)
    {
        const auto program = info[0].As<Napi::External<ProgramData>>().Data();
        const auto names = info[1].As<Napi::Array>();

        auto length = names.Length();
        auto uniforms = Napi::Array::New(info.Env(), length);
        for (uint32_t index = 0; index < length; ++index)
        {
            const auto name = names[index].As<Napi::String>().Utf8Value();

            auto vertexFound = program->VertexUniformInfos.find(name);
            auto fragmentFound = program->FragmentUniformInfos.find(name);

            if (vertexFound != program->VertexUniformInfos.end())
            {
                uniforms[index] = Napi::External<UniformInfo>::New(info.Env(), &vertexFound->second);
            }
            else if (fragmentFound != program->FragmentUniformInfos.end())
            {
                uniforms[index] = Napi::External<UniformInfo>::New(info.Env(), &fragmentFound->second);
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
        m_currentProgram->SetUniform(uniformInfo->Handle, gsl::make_span(&value, 1), uniformInfo->YFlip);
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

        m_currentProgram->SetUniform(uniformInfo->Handle, m_scratch, uniformInfo->YFlip, elementLength / size);
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

        m_currentProgram->SetUniform(uniformInfo->Handle, values, uniformInfo->YFlip);
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

            m_currentProgram->SetUniform(uniformInfo->Handle, gsl::make_span(matrixValues.data(), 16), uniformInfo->YFlip);
        }
        else
        {
            m_currentProgram->SetUniform(uniformInfo->Handle, gsl::make_span(matrix.Data(), elementLength), uniformInfo->YFlip);
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

        m_currentProgram->SetUniform(uniformInfo->Handle, gsl::span(matricesArray.Data(), elementLength), uniformInfo->YFlip, elementLength / 16);
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
        const auto texture = info[0].As<Napi::External<TextureData>>().Data();
        const auto data = info[1].As<Napi::TypedArray>();
        const auto generateMips = info[2].As<Napi::Boolean>().Value();
        const auto invertY = info[3].As<Napi::Boolean>().Value();
        const auto onSuccess = info[4].As<Napi::Function>();
        const auto onError = info[5].As<Napi::Function>();

        const auto dataSpan = gsl::make_span(static_cast<uint8_t*>(data.ArrayBuffer().Data()) + data.ByteOffset(), data.ByteLength());

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

    void NativeEngine::LoadRawTexture(const Napi::CallbackInfo& info)
    {
        const auto texture = info[0].As<Napi::External<TextureData>>().Data();
        const auto data = info[1].As<Napi::TypedArray>();
        const auto width = info[2].As<Napi::Number>().Uint32Value();
        const auto height = info[3].As<Napi::Number>().Uint32Value();
        const auto format = static_cast<bimg::TextureFormat::Enum>(info[4].As<Napi::Number>().Uint32Value());
        const auto generateMips = info[5].As<Napi::Boolean>().Value();
        const auto invertY = info[6].As<Napi::Boolean>().Value();

        // TODO: probably should assert data byte length is equal to allocated size
        const auto bytes = static_cast<uint8_t*>(data.ArrayBuffer().Data()) + data.ByteOffset();
        bimg::ImageContainer* image = bimg::imageAlloc(&m_allocator, format, static_cast<uint16_t>(width), static_cast<uint16_t>(height), 1, 1, false, false, bytes);

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
        const auto texture = info[0].As<Napi::External<TextureData>>().Data();
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
        const auto texture = info[0].As<Napi::External<TextureData>>().Data();
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
        auto filter = static_cast<uint32_t>(info[1].As<Napi::Number>().Uint32Value());

        texture->Flags &= ~(BGFX_SAMPLER_MIN_MASK | BGFX_SAMPLER_MAG_MASK | BGFX_SAMPLER_MIP_MASK);

        if (texture->AnisotropicLevel > 1)
        {
            texture->Flags |= BGFX_SAMPLER_MIN_ANISOTROPIC | BGFX_SAMPLER_MAG_ANISOTROPIC;
        }
        else
        {
            texture->Flags |= filter;
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

        texture->AnisotropicLevel = static_cast<uint8_t>(value);

        // if Anisotropic is set to 0 after being >1, then set texture flags back to linear
        texture->Flags &= ~(BGFX_SAMPLER_MIN_MASK | BGFX_SAMPLER_MAG_MASK | BGFX_SAMPLER_MIP_MASK);
        if (value)
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

            std::array<bgfx::TextureHandle, 2> textures{
                bgfx::createTexture2D(width, height, generateMips, 1, format, BGFX_TEXTURE_RT),
                bgfx::createTexture2D(width, height, generateMips, 1, depthStencilFormat, BGFX_TEXTURE_RT)};
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

        auto& frameBuffer{m_graphicsImpl.AddFrameBuffer(frameBufferHandle, width, height, false)};
        return Napi::External<FrameBuffer>::New(info.Env(), &frameBuffer);
    }

    void NativeEngine::DeleteFrameBuffer(const Napi::CallbackInfo& info)
    {
        const auto& frameBuffer{*info[0].As<Napi::External<FrameBuffer>>().Data()};
        m_graphicsImpl.RemoveFrameBuffer(frameBuffer);
    }

    void NativeEngine::BindFrameBuffer(const Napi::CallbackInfo& info)
    {
        auto frameBuffer{info[0].As<Napi::External<FrameBuffer>>().Data()};
        m_boundFrameBuffer = frameBuffer;
    }

    void NativeEngine::UnbindFrameBuffer(const Napi::CallbackInfo& info)
    {
        const auto frameBuffer{info[0].As<Napi::External<FrameBuffer>>().Data()};

        assert(frameBuffer == m_boundFrameBuffer);
        UNUSED(frameBuffer);

        m_boundFrameBuffer = &m_graphicsImpl.DefaultFrameBuffer();
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

        if (!info[1].IsUndefined())
        {
            depth = info[1].As<Napi::Number>().FloatValue();
            flags |= BGFX_CLEAR_DEPTH;
        }

        if (!info[2].IsUndefined())
        {
            stencil = static_cast<uint8_t>(info[2].As<Napi::Number>().Uint32Value());
            flags |= BGFX_CLEAR_STENCIL;
        }

        m_boundFrameBuffer->Clear(encoder, flags, rgba, depth, stencil);
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

        m_boundFrameBuffer->SetViewPort(encoder, x, yOrigin, width, height);
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

        if (!m_boundFrameBuffer->DefaultBackBuffer() && !bgfx::getCaps()->originBottomLeft)
        {
            // UV coordinates system are different between OpenGL and Direct3D/Metal
            // This is not an issue with loaded textures (png/jpg...) because
            // texel rows bytes are also using a different convention
            // see https://www.puredevsoftware.com/blog/2018/03/17/texture-coordinates-d3d-vs-opengl/
            // for render to texture, as the texel bytes are not reversed, sampling a RTT for
            // post process or shadows will result in inversion on V axis (Y)
            // to compensate for that, any matrix that is used to project onto clip-space has
            // to be flipped.
            // The involved matrices are determined by name and a boolean YFlip is set to true.
            // When rendering to texture, those matrices are flipped and set as uniform datas.
            // But because flipping clip-space coordinates also flips triangles winding,
            // Culling also has to be flipped.
            for (const auto& it : m_currentProgram->Uniforms)
            {
                const ProgramData::UniformValue& value = it.second;
                if (value.YFlip)
                {
                    float tmpMatrix[16];
                    static const float flipMatrix[16] = {
                        1.f, 0.f, 0.f, 0.f,
                        0.f, -1.f, 0.f, 0.f,
                        0.f, 0.f, 1.f, 0.f,
                        0.f, 0.f, 0.f, 1.f};
                    bx::mtxMul(tmpMatrix, value.Data.data(), flipMatrix);
                    encoder->setUniform({it.first}, tmpMatrix, value.ElementLength);
                }
                else
                {
                    encoder->setUniform({it.first}, value.Data.data(), value.ElementLength);
                }
            }

            // We need to explicitly swap the culling state flags (instead of XOR)
            // because we would like to preserve the no culling configuration, which is 00.
            const auto cullCW = (m_engineState & BGFX_STATE_CULL_CCW) != 0 ? BGFX_STATE_CULL_CW : 0;
            const auto cullCCW = (m_engineState & BGFX_STATE_CULL_CW) != 0 ? BGFX_STATE_CULL_CCW : 0;

            uint64_t engineStateYFlipped = m_engineState;
            engineStateYFlipped &= ~BGFX_STATE_CULL_MASK;
            engineStateYFlipped |= (cullCW | cullCCW) << BGFX_STATE_CULL_SHIFT;

            encoder->setState(engineStateYFlipped | fillModeState);
        }
        else
        {
            for (const auto& it : m_currentProgram->Uniforms)
            {
                const ProgramData::UniformValue& value = it.second;
                encoder->setUniform({it.first}, value.Data.data(), value.ElementLength);
            }

            encoder->setState(m_engineState | fillModeState);
        }

        // Discard everything except bindings since we keep the state of everything else.
        m_boundFrameBuffer->Submit(encoder, m_currentProgram->Handle, BGFX_DISCARD_ALL & ~BGFX_DISCARD_BINDINGS);
    }

    Graphics::Impl::UpdateToken& NativeEngine::GetUpdateToken()
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
