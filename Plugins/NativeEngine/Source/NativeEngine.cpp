#include "NativeEngine.h"
#include "ShaderCompiler.h"
#include <arcana/threading/task.h>
#include <arcana/threading/task_schedulers.h>

#include <napi/env.h>

#include <bgfx/bgfx.h>

#define BGFX_RESET_FLAGS (BGFX_RESET_VSYNC | BGFX_RESET_MSAA_X4 | BGFX_RESET_MAXANISOTROPY)

#include <bimg/bimg.h>
#include <bimg/decode.h>
#include <bimg/encode.h>

#include <queue>
#include <regex>
#include <sstream>
#include <variant>

namespace Babylon
{
    namespace
    {
        enum class WebGLAttribType
        {
            BYTE = 5120,
            UNSIGNED_BYTE = 5121,
            SHORT = 5122,
            UNSIGNED_SHORT = 5123,
            INT = 5124,
            UNSIGNED_INT = 5125,
            FLOAT = 5126
        };

        bgfx::AttribType::Enum ConvertAttribType(WebGLAttribType type)
        {
            switch (type)
            {
                case WebGLAttribType::UNSIGNED_BYTE:
                    return bgfx::AttribType::Uint8;
                case WebGLAttribType::SHORT:
                    return bgfx::AttribType::Int16;
                case WebGLAttribType::FLOAT:
                    return bgfx::AttribType::Float;
                default: // avoid "warning: 4 enumeration values not handled"
                    throw std::exception();
                    break;
            }
        }

        // Must match constants.ts in Babylon.js.
        constexpr std::array<uint64_t, 11> ALPHA_MODE{
            // ALPHA_DISABLE
            0x0,

            // ALPHA_ADD: SRC ALPHA * SRC + DEST
            BGFX_STATE_BLEND_FUNC_SEPARATE(BGFX_STATE_BLEND_SRC_ALPHA, BGFX_STATE_BLEND_ONE, BGFX_STATE_BLEND_ZERO, BGFX_STATE_BLEND_ONE),

            // ALPHA_COMBINE: SRC ALPHA * SRC + (1 - SRC ALPHA) * DEST
            BGFX_STATE_BLEND_FUNC_SEPARATE(BGFX_STATE_BLEND_SRC_ALPHA, BGFX_STATE_BLEND_INV_SRC_ALPHA, BGFX_STATE_BLEND_ONE, BGFX_STATE_BLEND_ONE),

            // ALPHA_SUBTRACT: DEST - SRC * DEST
            BGFX_STATE_BLEND_FUNC_SEPARATE(BGFX_STATE_BLEND_ZERO, BGFX_STATE_BLEND_INV_SRC_COLOR, BGFX_STATE_BLEND_ONE, BGFX_STATE_BLEND_ONE),

            // ALPHA_MULTIPLY: SRC * DEST
            BGFX_STATE_BLEND_FUNC_SEPARATE(BGFX_STATE_BLEND_DST_COLOR, BGFX_STATE_BLEND_ZERO, BGFX_STATE_BLEND_ONE, BGFX_STATE_BLEND_ONE),

            // ALPHA_MAXIMIZED: SRC ALPHA * SRC + (1 - SRC) * DEST
            BGFX_STATE_BLEND_FUNC_SEPARATE(BGFX_STATE_BLEND_SRC_ALPHA, BGFX_STATE_BLEND_INV_SRC_COLOR, BGFX_STATE_BLEND_ONE, BGFX_STATE_BLEND_ONE),

            // ALPHA_ONEONE: SRC + DEST
            BGFX_STATE_BLEND_FUNC_SEPARATE(BGFX_STATE_BLEND_ONE, BGFX_STATE_BLEND_ONE, BGFX_STATE_BLEND_ZERO, BGFX_STATE_BLEND_ONE),

            // ALPHA_PREMULTIPLIED: SRC + (1 - SRC ALPHA) * DEST
            BGFX_STATE_BLEND_FUNC_SEPARATE(BGFX_STATE_BLEND_ONE, BGFX_STATE_BLEND_INV_SRC_ALPHA, BGFX_STATE_BLEND_ONE, BGFX_STATE_BLEND_ONE),

            // ALPHA_PREMULTIPLIED_PORTERDUFF: SRC + (1 - SRC ALPHA) * DEST, (1 - SRC ALPHA) * DEST ALPHA
            BGFX_STATE_BLEND_FUNC_SEPARATE(BGFX_STATE_BLEND_ONE, BGFX_STATE_BLEND_INV_SRC_ALPHA, BGFX_STATE_BLEND_ONE, BGFX_STATE_BLEND_INV_SRC_ALPHA),

            // ALPHA_INTERPOLATE: CST * SRC + (1 - CST) * DEST
            BGFX_STATE_BLEND_FUNC_SEPARATE(BGFX_STATE_BLEND_FACTOR, BGFX_STATE_BLEND_INV_FACTOR, BGFX_STATE_BLEND_FACTOR, BGFX_STATE_BLEND_INV_FACTOR),

            // ALPHA_SCREENMODE: SRC + (1 - SRC) * DEST, SRC ALPHA + (1 - SRC ALPHA) * DEST ALPHA
            BGFX_STATE_BLEND_FUNC_SEPARATE(BGFX_STATE_BLEND_ONE, BGFX_STATE_BLEND_INV_SRC_COLOR, BGFX_STATE_BLEND_ONE, BGFX_STATE_BLEND_INV_SRC_ALPHA),
        };

        constexpr std::array<bgfx::TextureFormat::Enum, 2> TEXTURE_FORMAT{
            bgfx::TextureFormat::RGBA8,
            bgfx::TextureFormat::RGBA32F};

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

        void Update(const Napi::TypedArray& bytes, uint32_t startingIdx)
        {
            const bgfx::Memory* memory = bgfx::copy(bytes.As<Napi::Uint8Array>().Data(), static_cast<uint32_t>(bytes.ByteLength()));

            constexpr auto nonDynamic = [](auto) {
                throw std::runtime_error("Cannot update a non-dynamic index buffer.");
            };
            const auto dynamic = [memory, startingIdx](auto handle) {
                bgfx::update(handle, startingIdx, memory);
            };
            DoForHandleTypes(nonDynamic, dynamic);
        }
        
        void SetBgfxIndexBuffer(uint32_t firstIndex, uint32_t numIndices) const
        {
            const auto nonDynamic = [firstIndex, numIndices](auto handle) {
                bgfx::setIndexBuffer(handle, firstIndex, numIndices);
            };
            const auto dynamic = [firstIndex, numIndices](auto handle) {
                bgfx::setIndexBuffer(handle, firstIndex, numIndices);
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

        void Update(const Napi::Uint8Array& bytes, uint32_t offset, uint32_t byteLength)
        {
            constexpr auto nonDynamic = [](auto) {
                throw std::runtime_error("Cannot update non-dynamic vertex buffer.");
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

        void SetAsBgfxVertexBuffer(uint8_t index, uint32_t startVertex, bgfx::VertexLayoutHandle layout) const
        {
            const auto nonDynamic = [index, startVertex, layout](auto handle) {
                bgfx::setVertexBuffer(index, handle, startVertex, UINT32_MAX, layout);
            };
            const auto dynamic = [index, startVertex, layout](auto handle) {
                bgfx::setVertexBuffer(index, handle, startVertex, UINT32_MAX, layout);
            };
            DoForHandleTypes(nonDynamic, dynamic);
        }

    private:
        std::vector<uint8_t> m_bytes{};
    };

    void NativeEngine::Initialize(Napi::Env env, bool autoRender)
    {
        // Initialize the JavaScript side.
        Napi::HandleScope scope{env};

        Napi::Function func = DefineClass(
            env,
            JS_CLASS_NAME,
            {
                InstanceMethod("dispose", &NativeEngine::Dispose),
                InstanceMethod("getEngine", &NativeEngine::GetEngine),
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
                InstanceMethod("loadCubeTexture", &NativeEngine::LoadCubeTexture),
                InstanceMethod("loadCubeTextureWithMips", &NativeEngine::LoadCubeTextureWithMips),
                InstanceMethod("getTextureWidth", &NativeEngine::GetTextureWidth),
                InstanceMethod("getTextureHeight", &NativeEngine::GetTextureHeight),
                InstanceMethod("setTextureSampling", &NativeEngine::SetTextureSampling),
                InstanceMethod("setTextureWrapMode", &NativeEngine::SetTextureWrapMode),
                InstanceMethod("setTextureAnisotropicLevel", &NativeEngine::SetTextureAnisotropicLevel),
                InstanceMethod("setTexture", &NativeEngine::SetTexture),
                InstanceMethod("deleteTexture", &NativeEngine::DeleteTexture),
                InstanceMethod("createFramebuffer", &NativeEngine::CreateFrameBuffer),
                InstanceMethod("deleteFramebuffer", &NativeEngine::DeleteFrameBuffer),
                InstanceMethod("bindFramebuffer", &NativeEngine::BindFrameBuffer),
                InstanceMethod("unbindFramebuffer", &NativeEngine::UnbindFrameBuffer),
                InstanceMethod("drawIndexed", &NativeEngine::DrawIndexed),
                InstanceMethod("draw", &NativeEngine::Draw),
                InstanceMethod("clear", &NativeEngine::Clear),
                InstanceMethod("clearColor", &NativeEngine::ClearColor),
                InstanceMethod("clearDepth", &NativeEngine::ClearDepth),
                InstanceMethod("clearStencil", &NativeEngine::ClearStencil),
                InstanceMethod("getRenderWidth", &NativeEngine::GetRenderWidth),
                InstanceMethod("getRenderHeight", &NativeEngine::GetRenderHeight),
                InstanceMethod("setViewPort", &NativeEngine::SetViewPort),
                InstanceMethod("getFramebufferData", &NativeEngine::GetFramebufferData),
                InstanceMethod("getRenderAPI", &NativeEngine::GetRenderAPI),
                InstanceValue(JS_AUTO_RENDER_PROPERTY_NAME, Napi::Boolean::New(env, autoRender))
            });

        JsRuntime::NativeObject::GetFromJavaScript(env).Set(JS_ENGINE_CONSTRUCTOR_NAME, func);
    }

    NativeEngine::NativeEngine(const Napi::CallbackInfo& info)
        : NativeEngine(info, JsRuntime::GetFromJavaScript(info.Env()), Plugins::Internal::NativeWindow::GetFromJavaScript(info.Env()))
    {
    }

    NativeEngine::NativeEngine(const Napi::CallbackInfo& info, JsRuntime& runtime, Plugins::Internal::NativeWindow& nativeWindow)
        : Napi::ObjectWrap<NativeEngine>{info}
        , AutomaticRenderingEnabled{info.This().As<Napi::Object>().Get(JS_AUTO_RENDER_PROPERTY_NAME).ToBoolean()}
        , RuntimeScheduler{runtime}
        , m_runtime{runtime}
        , m_graphicsImpl{Graphics::Impl::GetFromJavaScript(info.Env())}
        , m_engineState{BGFX_STATE_DEFAULT}
        , m_resizeCallbackTicket{nativeWindow.AddOnResizeCallback([this](size_t width, size_t height) { this->UpdateSize(width, height); })}
    {
        UpdateSize(static_cast<uint32_t>(nativeWindow.GetWidth()), static_cast<uint32_t>(nativeWindow.GetHeight()));
    }

    NativeEngine::~NativeEngine()
    {
        Dispose();
    }

    void NativeEngine::UpdateSize(size_t width, size_t height)
    {
        const auto w = static_cast<uint16_t>(width);
        const auto h = static_cast<uint16_t>(height);

        auto bgfxStats = bgfx::getStats();
        if (w != bgfxStats->width || h != bgfxStats->height)
        {
            bgfx::reset(w, h, BGFX_RESET_FLAGS);
            bgfx::setViewRect(0, 0, 0, w, h);
#ifdef __APPLE__
            bgfx::frame();
#else
            bgfx::touch(0);
#endif
        }
    }

    template<typename SchedulerT>
    arcana::task<void, std::exception_ptr> NativeEngine::GetRequestAnimationFrameTask(SchedulerT& scheduler)
    {
        return arcana::make_task(scheduler, m_cancelSource, [this] {
            m_isRenderScheduled = false;

            try
            {
                m_requestAnimationFrameCalback.Call({});
                GetFrameBufferManager().Reset();
            }
            catch (const std::exception& ex)
            {
                m_runtime.Dispatch([ex](Napi::Env env) {
                    Napi::Error::New(env, ex.what()).ThrowAsJavaScriptException();
                });
            }
        });
    }

    void NativeEngine::ScheduleRender()
    {
        if (!m_isRenderScheduled)
        {
            m_isRenderScheduled = true;

            m_graphicsImpl.GetBeforeRenderTask().then(arcana::inline_scheduler, m_cancelSource, [this]() mutable {
                if (AutomaticRenderingEnabled)
                {
                    m_graphicsImpl.AddRenderWorkTask(GetRequestAnimationFrameTask(arcana::inline_scheduler));
                }
                else
                {
                    m_graphicsImpl.AddRenderWorkTask(GetRequestAnimationFrameTask(RuntimeScheduler));
                }
            });
            if (AutomaticRenderingEnabled)
            {
                Dispatch([this] {
                    m_graphicsImpl.RenderCurrentFrame();
                });
            }
        }
    }

    FrameBufferManager& NativeEngine::GetFrameBufferManager()
    {
        return m_frameBufferManager;
    }

    void NativeEngine::Dispose()
    {
        m_cancelSource.cancel();

        // This collection contains bgfx data, so it must be cleared before bgfx::shutdown is called.
        m_programDataCollection.clear();
    }

    void NativeEngine::Dispose(const Napi::CallbackInfo& /*info*/)
    {
        Dispose();
    }

    // NativeEngine definitions
    Napi::Value NativeEngine::GetEngine(const Napi::CallbackInfo& info)
    {
        return Napi::External<NativeEngine>::New(info.Env(), this);
    }

    void NativeEngine::RequestAnimationFrame(const Napi::CallbackInfo& info)
    {
        auto callback = info[0].As<Napi::Function>();

        if (m_requestAnimationFrameCalback.IsEmpty() ||
            m_requestAnimationFrameCalback.Value() != callback)
        {
            m_requestAnimationFrameCalback = Napi::Persistent(callback);
        }

        ScheduleRender();
    }

    Napi::Value NativeEngine::CreateVertexArray(const Napi::CallbackInfo& info)
    {
        return Napi::External<VertexArray>::New(info.Env(), new VertexArray{});
    }

    void NativeEngine::DeleteVertexArray(const Napi::CallbackInfo& info)
    {
        delete info[0].As<Napi::External<VertexArray>>().Data();
    }

    void NativeEngine::BindVertexArray(const Napi::CallbackInfo& info)
    {
        const auto& vertexArray = *(info[0].As<Napi::External<VertexArray>>().Data());

        // a vertex array might not have an index buffer associated with
        m_currentBoundIndexBuffer = vertexArray.indexBuffer.data;

        const auto& vertexBuffers = vertexArray.vertexBuffers;
        for (uint8_t index = 0; index < vertexBuffers.size(); ++index)
        {
            const auto& vertexBuffer = vertexBuffers[index];
            vertexBuffer.data->SetAsBgfxVertexBuffer(index, vertexBuffer.startVertex, vertexBuffer.vertexLayoutHandle);
        }
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

        vertexArray.indexBuffer.data = indexBufferData;
    }

    void NativeEngine::UpdateDynamicIndexBuffer(const Napi::CallbackInfo& info)
    {
        IndexBufferData& indexBufferData = *(info[0].As<Napi::External<IndexBufferData>>().Data());

        const Napi::TypedArray data = info[1].As<Napi::TypedArray>();
        const uint32_t startingIdx = info[2].As<Napi::Number>().Uint32Value();

        indexBufferData.Update(data, startingIdx);
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
        const bgfx::AttribType::Enum attribType = ConvertAttribType(static_cast<WebGLAttribType>(type));
        vertexLayout.add(attrib, static_cast<uint8_t>(numElements), attribType, normalized);
        vertexLayout.m_stride = static_cast<uint16_t>(byteStride);
        vertexLayout.end();

        vertexBufferData->EnsureFinalized(info.Env(), vertexLayout);

        vertexArray.vertexBuffers.push_back({vertexBufferData, byteOffset / byteStride, bgfx::createVertexLayout(vertexLayout) });
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

        vertexBufferData.Update(data, byteOffset, byteLength);
    }

    Napi::Value NativeEngine::CreateProgram(const Napi::CallbackInfo& info)
    {
        const std::string vertexSource{info[0].As<Napi::String>().Utf8Value()};
        const std::string fragmentSource{info[1].As<Napi::String>().Utf8Value()};

        std::unique_ptr<ProgramData> programData{std::make_unique<ProgramData>()};
        ShaderCompiler::BgfxShaderInfo shaderInfo{m_shaderCompiler.Compile(vertexSource, fragmentSource)};

        static auto InitUniformInfos{[](bgfx::ShaderHandle shader, const std::unordered_map<std::string, uint8_t>& uniformStages, std::unordered_map<std::string, UniformInfo>& uniformInfos)
        {
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
        InitUniformInfos(vertexShader, shaderInfo.VertexUniformStages, programData->VertexUniformInfos);
        programData->VertexAttributeLocations = std::move(shaderInfo.VertexAttributeLocations);

        auto fragmentShader = bgfx::createShader(bgfx::copy(shaderInfo.FragmentBytes.data(), static_cast<uint32_t>(shaderInfo.FragmentBytes.size())));
        InitUniformInfos(fragmentShader, shaderInfo.FragmentUniformStages, programData->FragmentUniformInfos);

        programData->Program = bgfx::createProgram(vertexShader, fragmentShader, true);
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
            const auto name = names[index].As<Napi::String>().Utf8Value();
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
        const auto reverseSide = info[2].As<Napi::Boolean>().Value();

        m_engineState &= ~BGFX_STATE_CULL_MASK;
        if (reverseSide)
        {
            m_engineState &= ~BGFX_STATE_FRONT_CCW;

            if (culling)
            {
                m_engineState |= BGFX_STATE_CULL_CW;
            }
        }
        else
        {
            m_engineState |= BGFX_STATE_FRONT_CCW;

            if (culling)
            {
                m_engineState |= BGFX_STATE_CULL_CCW;
            }
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
        const auto blendMode = info[0].As<Napi::Number>().Int32Value();

        m_engineState &= ~BGFX_STATE_BLEND_MASK;
        m_engineState |= ALPHA_MODE[blendMode];
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
        const auto texture = info[0].As<Napi::External<TextureData>>().Data();
        const auto data = info[1].As<Napi::TypedArray>();
        const auto generateMips = info[2].As<Napi::Boolean>().Value();
        const auto invertY = info[3].As<Napi::Boolean>().Value();
        const auto onSuccess = info[4].As<Napi::Function>();
        const auto onError = info[5].As<Napi::Function>();

        const auto dataSpan = gsl::make_span(static_cast<uint8_t*>(data.ArrayBuffer().Data()) + data.ByteOffset(), data.ByteLength());

        arcana::make_task(arcana::threadpool_scheduler, m_cancelSource,
            [this, dataSpan, generateMips, invertY]() {
                bimg::ImageContainer* image = bimg::imageParse(&m_allocator, dataSpan.data(), static_cast<uint32_t>(dataSpan.size()));
                // todo: bimg::imageParse will return nullptr when trying to load a texture with an url that is not a valid texture
                // Like a 404 html page.
                if (invertY)
                {
                    FlipY(image);
                }
                if (generateMips)
                {
                    GenerateMips(&m_allocator, &image);
                }
                return image;
            })
            .then(RuntimeScheduler, m_cancelSource, [texture, dataRef = Napi::Persistent(data)](bimg::ImageContainer* image) {
                CreateTextureFromImage(texture, image);
            })
            .then(arcana::inline_scheduler, m_cancelSource, [onSuccessRef = Napi::Persistent(onSuccess), onErrorRef = Napi::Persistent(onError)](arcana::expected<void, std::exception_ptr> result) {
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

    void NativeEngine::LoadCubeTexture(const Napi::CallbackInfo& info)
    {
        const auto texture = info[0].As<Napi::External<TextureData>>().Data();
        const auto data = info[1].As<Napi::Array>();
        const auto generateMips = info[2].As<Napi::Boolean>().Value();
        const auto onSuccess = info[3].As<Napi::Function>();
        const auto onError = info[4].As<Napi::Function>();

        std::array<arcana::task<bimg::ImageContainer*, std::exception_ptr>, 6> tasks;
        for (uint32_t face = 0; face < data.Length(); face++)
        {
            const auto typedArray = data[face].As<Napi::TypedArray>();
            const auto dataSpan = gsl::make_span(static_cast<uint8_t*>(typedArray.ArrayBuffer().Data()) + typedArray.ByteOffset(), typedArray.ByteLength());
            tasks[face] = arcana::make_task(arcana::threadpool_scheduler, m_cancelSource, [this, dataSpan, generateMips]() {
                bimg::ImageContainer* image = bimg::imageParse(&m_allocator, dataSpan.data(), static_cast<uint32_t>(dataSpan.size()));
                if (generateMips)
                {
                    GenerateMips(&m_allocator, &image);
                }
                return image;
            });
        }

        arcana::when_all(gsl::make_span(tasks))
            .then(RuntimeScheduler, m_cancelSource,
                [texture, dataRef = Napi::Persistent(data), generateMips](const std::vector<bimg::ImageContainer*>& images) {
                    CreateCubeTextureFromImages(texture, images, generateMips);
                })
            .then(arcana::inline_scheduler, m_cancelSource, [this, onSuccessRef = Napi::Persistent(onSuccess)]() {
                onSuccessRef.Call({Napi::Value::From(Env(), true)});
            })
            .then(arcana::inline_scheduler, m_cancelSource, [this, onErrorRef = Napi::Persistent(onError)](arcana::expected<void, std::exception_ptr> result) {
                if (result.has_error())
                {
                    onErrorRef.Call({Napi::Value::From(Env(), true)});
                }
            });
    }

    void NativeEngine::LoadCubeTextureWithMips(const Napi::CallbackInfo& info)
    {
        const auto texture = info[0].As<Napi::External<TextureData>>().Data();
        const auto data = info[1].As<Napi::Array>();
        const auto onSuccess = info[2].As<Napi::Function>();
        const auto onError = info[3].As<Napi::Function>();

        const auto numMips = data.Length();
        std::vector<arcana::task<bimg::ImageContainer*, std::exception_ptr>> tasks(6 * numMips);
        for (uint32_t mip = 0; mip < numMips; mip++)
        {
            const auto faceData = data[mip].As<Napi::Array>();
            for (uint32_t face = 0; face < 6; face++)
            {
                const auto typedArray = faceData[face].As<Napi::TypedArray>();
                const auto dataSpan = gsl::make_span(static_cast<uint8_t*>(typedArray.ArrayBuffer().Data()) + typedArray.ByteOffset(), typedArray.ByteLength());
                tasks[(face * numMips) + mip] = arcana::make_task(arcana::threadpool_scheduler, m_cancelSource, [this, dataSpan]() {
                    bimg::ImageContainer* image = bimg::imageParse(&m_allocator, dataSpan.data(), static_cast<uint32_t>(dataSpan.size()));
                    FlipY(image);
                    return image;
                });
            }
        }

        arcana::when_all(gsl::make_span(tasks))
            .then(RuntimeScheduler, m_cancelSource, [texture, dataRef = Napi::Persistent(data)](std::vector<bimg::ImageContainer*> images) {
                CreateCubeTextureFromImages(texture, images, true);
            })
            .then(RuntimeScheduler, m_cancelSource, [this, onSuccessRef = Napi::Persistent(onSuccess)]() {
                onSuccessRef.Call({Napi::Value::From(Env(), true)});
            })
            .then(arcana::inline_scheduler, m_cancelSource, [this, onErrorRef = Napi::Persistent(onError)](arcana::expected<void, std::exception_ptr> result) {
                if (result.has_error())
                {
                    onErrorRef.Call({Napi::Value::From(Env(), true)});
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

        constexpr std::array<uint32_t, 12> bgfxFiltering = {
            BGFX_SAMPLER_MAG_POINT | BGFX_SAMPLER_MIN_POINT,                          // nearest is mag = nearest and min = nearest and mip = linear
            BGFX_SAMPLER_MIP_POINT,                                                   // Bilinear is mag = linear and min = linear and mip = nearest
            0,                                                                        // Trilinear is mag = linear and min = linear and mip = linear
            BGFX_SAMPLER_MIN_POINT | BGFX_SAMPLER_MAG_POINT | BGFX_SAMPLER_MIP_POINT, // mag = nearest and min = nearest and mip = nearest
            BGFX_SAMPLER_MAG_POINT | BGFX_SAMPLER_MIP_POINT,                          // mag = nearest and min = linear and mip = nearest
            BGFX_SAMPLER_MAG_POINT,                                                   // mag = nearest and min = linear and mip = linear
            BGFX_SAMPLER_MAG_POINT,                                                   // mag = nearest and min = linear and mip = none
            BGFX_SAMPLER_MAG_POINT | BGFX_SAMPLER_MIN_POINT,                          // mag = nearest and min = nearest and mip = none
            BGFX_SAMPLER_MIP_POINT | BGFX_SAMPLER_MIP_POINT,                          // mag = linear and min = nearest and mip = nearest
            BGFX_SAMPLER_MIN_POINT,                                                   // mag = linear and min = nearest and mip = linear
            0,                                                                        // mag = linear and min = linear and mip = none
            BGFX_SAMPLER_MIN_POINT};                                                  // mag = linear and min = nearest and mip = none

        texture->Flags &= ~(BGFX_SAMPLER_MIN_MASK | BGFX_SAMPLER_MAG_MASK | BGFX_SAMPLER_MIP_MASK);

        if (texture->AnisotropicLevel > 1)
        {
            texture->Flags |= BGFX_SAMPLER_MIN_ANISOTROPIC | BGFX_SAMPLER_MAG_ANISOTROPIC;
        }
        else
        {
            texture->Flags |= bgfxFiltering[filter];
        }
    }

    void NativeEngine::SetTextureWrapMode(const Napi::CallbackInfo& info)
    {
        const auto texture = info[0].As<Napi::External<TextureData>>().Data();
        auto addressModeU = static_cast<uint32_t>(info[1].As<Napi::Number>().Uint32Value());
        auto addressModeV = static_cast<uint32_t>(info[2].As<Napi::Number>().Uint32Value());
        auto addressModeW = static_cast<uint32_t>(info[3].As<Napi::Number>().Uint32Value());

        constexpr std::array<uint32_t, 3> bgfxSamplers = {0, BGFX_SAMPLER_U_CLAMP, BGFX_SAMPLER_U_MIRROR};

        uint32_t addressMode = bgfxSamplers[addressModeU] +
            (bgfxSamplers[addressModeV] << BGFX_SAMPLER_V_SHIFT) +
            (bgfxSamplers[addressModeW] << BGFX_SAMPLER_W_SHIFT);

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
        const auto uniformInfo = info[0].As<Napi::External<UniformInfo>>().Data();
        const auto texture = info[1].As<Napi::External<TextureData>>().Data();

        bgfx::setTexture(uniformInfo->Stage, uniformInfo->Handle, texture->Handle, texture->Flags);
    }

    void NativeEngine::DeleteTexture(const Napi::CallbackInfo& info)
    {
        const auto texture = info[0].As<Napi::External<TextureData>>().Data();
        delete texture;
    }

    Napi::Value NativeEngine::CreateFrameBuffer(const Napi::CallbackInfo& info)
    {
        const auto texture = info[0].As<Napi::External<TextureData>>().Data();
        uint16_t width = static_cast<uint16_t>(info[1].As<Napi::Number>().Uint32Value());
        uint16_t height = static_cast<uint16_t>(info[2].As<Napi::Number>().Uint32Value());
        uint32_t formatIndex = info[3].As<Napi::Number>().Uint32Value();
        //int samplingMode = info[4].As<Napi::Number>().Uint32Value();
        bool generateStencilBuffer = info[5].As<Napi::Boolean>();
        bool generateDepth = info[6].As<Napi::Boolean>();
        bool generateMips = info[7].As<Napi::Boolean>();

        bgfx::FrameBufferHandle frameBufferHandle{};
        if (generateStencilBuffer && !generateDepth)
        {
            throw std::exception{/* Does this case even make any sense? */};
        }
        else if (!generateStencilBuffer && !generateDepth)
        {
            frameBufferHandle = bgfx::createFrameBuffer(width, height, TEXTURE_FORMAT[formatIndex], BGFX_TEXTURE_RT);
        }
        else
        {
            auto depthStencilFormat = bgfx::TextureFormat::D32;
            if (generateStencilBuffer)
            {
                depthStencilFormat = bgfx::TextureFormat::D24S8;
            }

            assert(bgfx::isTextureValid(0, false, 1, TEXTURE_FORMAT[formatIndex], BGFX_TEXTURE_RT));
            assert(bgfx::isTextureValid(0, false, 1, depthStencilFormat, BGFX_TEXTURE_RT));

            std::array<bgfx::TextureHandle, 2> textures{
                bgfx::createTexture2D(width, height, generateMips, 1, TEXTURE_FORMAT[formatIndex], BGFX_TEXTURE_RT),
                bgfx::createTexture2D(width, height, generateMips, 1, depthStencilFormat, BGFX_TEXTURE_RT)};
            std::array<bgfx::Attachment, textures.size()> attachments{};
            for (size_t idx = 0; idx < attachments.size(); ++idx)
            {
                attachments[idx].init(textures[idx]);
            }
            frameBufferHandle = bgfx::createFrameBuffer(static_cast<uint8_t>(attachments.size()), attachments.data(), true);
        }

        texture->Handle = bgfx::getTexture(frameBufferHandle);

        return Napi::External<FrameBufferData>::New(info.Env(), m_frameBufferManager.CreateNew(frameBufferHandle, width, height));
    }

    void NativeEngine::DeleteFrameBuffer(const Napi::CallbackInfo& info)
    {
        const auto frameBufferData = info[0].As<Napi::External<FrameBufferData>>().Data();
        delete frameBufferData;
    }

    void NativeEngine::BindFrameBuffer(const Napi::CallbackInfo& info)
    {
        const auto frameBufferData = info[0].As<Napi::External<FrameBufferData>>().Data();
        m_frameBufferManager.Bind(frameBufferData);
    }

    void NativeEngine::UnbindFrameBuffer(const Napi::CallbackInfo& info)
    {
        const auto frameBufferData = info[0].As<Napi::External<FrameBufferData>>().Data();
        m_frameBufferManager.Unbind(frameBufferData);
    }

    void NativeEngine::DrawIndexed(const Napi::CallbackInfo& info)
    {
        const auto fillMode = info[0].As<Napi::Number>().Int32Value();
        const auto elementStart = info[1].As<Napi::Number>().Int32Value();
        const auto elementCount = info[2].As<Napi::Number>().Int32Value();

        // TODO: handle viewport

        if (m_currentBoundIndexBuffer)
        {
            m_currentBoundIndexBuffer->SetBgfxIndexBuffer(elementStart, elementCount);
        }

        // TODO: support other fill modes
        uint64_t fillModeState = 0; //indexed tri list
        switch (fillMode)
        {
        case 2: // MATERIAL_PointFillMode
            fillModeState = BGFX_STATE_PT_POINTS;
            break;
        case 4: // MATERIAL_LineListDrawMode
            fillModeState = BGFX_STATE_PT_LINES;
            break;
        case 6: // MATERIAL_LineStripDrawMode
            fillModeState = BGFX_STATE_PT_LINESTRIP;
            break;
        case 7: // MATERIAL_TriangleStripDrawMode
            fillModeState = BGFX_STATE_PT_TRISTRIP;
            break;
        }

        for (const auto& it : m_currentProgram->Uniforms)
        {
            const ProgramData::UniformValue& value = it.second;
            bgfx::setUniform({it.first}, value.Data.data(), value.ElementLength);
        }

        bgfx::setState(m_engineState | fillModeState);
#if (ANDROID)
        // TODO : find why we need to discard state on Android
        bgfx::submit(m_frameBufferManager.GetBound().ViewId, m_currentProgram->Program, 0, false);
#else
        bgfx::submit(m_frameBufferManager.GetBound().ViewId, m_currentProgram->Program, 0, BGFX_DISCARD_INSTANCE_DATA | BGFX_DISCARD_STATE | BGFX_DISCARD_TRANSFORM);
#endif
    }

    void NativeEngine::Draw(const Napi::CallbackInfo& info)
    {
        bgfx::discard(BGFX_DISCARD_INDEX_BUFFER);
        m_currentBoundIndexBuffer = nullptr;
        DrawIndexed(info);
    }

    void NativeEngine::Clear(const Napi::CallbackInfo& info)
    {
        m_frameBufferManager.GetBound().ViewClearState.UpdateFlags(info);
    }

    void NativeEngine::ClearColor(const Napi::CallbackInfo& info)
    {
        m_frameBufferManager.GetBound().ViewClearState.UpdateColor(info);
    }

    void NativeEngine::ClearStencil(const Napi::CallbackInfo& info)
    {
        m_frameBufferManager.GetBound().ViewClearState.UpdateStencil(info);
    }

    void NativeEngine::ClearDepth(const Napi::CallbackInfo& info)
    {
        m_frameBufferManager.GetBound().ViewClearState.UpdateDepth(info);
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
        const auto x = info[0].As<Napi::Number>().FloatValue();
        const auto y = info[1].As<Napi::Number>().FloatValue();
        const auto width = info[2].As<Napi::Number>().FloatValue();
        const auto height = info[3].As<Napi::Number>().FloatValue();

        const auto backbufferWidth = bgfx::getStats()->width;
        const auto backbufferHeight = bgfx::getStats()->height;
        const float yOrigin = bgfx::getCaps()->originBottomLeft ? y : (1.f - y - height);

        m_frameBufferManager.GetBound().UseViewId(m_frameBufferManager.GetNewViewId());
        const bgfx::ViewId viewId = m_frameBufferManager.GetBound().ViewId;
        bgfx::setViewFrameBuffer(viewId, m_frameBufferManager.GetBound().FrameBuffer);
        bgfx::setViewRect(viewId,
            static_cast<uint16_t>(x * backbufferWidth),
            static_cast<uint16_t>(yOrigin * backbufferHeight),
            static_cast<uint16_t>(width * backbufferWidth),
            static_cast<uint16_t>(height * backbufferHeight));
    }

    void NativeEngine::GetFramebufferData(const Napi::CallbackInfo& info)
    {
        bgfx::FrameBufferHandle fbh = BGFX_INVALID_HANDLE;
        const auto callback = info[0].As<Napi::Function>();

        m_graphicsImpl.Callback.addScreenShotCallback(callback);
        bgfx::requestScreenShot(fbh, "GetImageData");
    }

    Napi::Value NativeEngine::GetRenderAPI(const Napi::CallbackInfo& info)
    {
        return Napi::Value::From(info.Env(), static_cast<int>(bgfx::getRendererType()));
    }

    void NativeEngine::Dispatch(std::function<void()> function)
    {
        m_runtime.Dispatch([function = std::move(function)](Napi::Env) {
            function();
        });
    }
}
