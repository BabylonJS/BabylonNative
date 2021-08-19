#pragma once

#include "PerFrameValue.h"
#include "ShaderCompiler.h"

#include <Babylon/JsRuntime.h>
#include <Babylon/JsRuntimeScheduler.h>

#include <GraphicsImpl.h>
#include <BgfxCallback.h>
#include <FrameBuffer.h>
#include <ResourceManagement.h>

#include <napi/napi.h>

#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <bimg/bimg.h>
#include <bx/allocator.h>

#include <gsl/gsl>

#include <assert.h>

#include <arcana/threading/cancellation.h>
#include <unordered_map>

namespace Babylon
{
    struct TextureData final : public NativeResource<TextureData>
    {
        ~TextureData()
        {
            if (OwnsHandle && bgfx::isValid(Handle))
            {
                bgfx::destroy(Handle);
            }
        }

        bgfx::TextureHandle Handle{bgfx::kInvalidHandle};
        bool OwnsHandle{true};
        uint32_t Width{0};
        uint32_t Height{0};
        uint32_t Flags{0};
        uint8_t AnisotropicLevel{0};
    };

    struct UniformInfo final : public NativeResource<UniformInfo>
    {
        UniformInfo(uint8_t stage, bgfx::UniformHandle handle) :
            Stage{stage},
            Handle{handle}
        {
        }

        uint8_t Stage{};
        bgfx::UniformHandle Handle{bgfx::kInvalidHandle};
    };

    struct ProgramData final : public NativeResource<ProgramData>
    {
        ProgramData() = default;
        ProgramData(const ProgramData&) = delete;
        ProgramData(ProgramData&& other) :
            VertexAttributeLocations{std::move(other.VertexAttributeLocations)},
            VertexUniformInfos{std::move(other.VertexUniformInfos)},
            FragmentUniformInfos{std::move(other.FragmentUniformInfos)},
            Handle{other.Handle},
            Uniforms{std::move(other.Uniforms)}
        {
            other.Handle.idx = bgfx::kInvalidHandle;
        };

        ~ProgramData()
        {
            if (bgfx::isValid(Handle))
            {
                bgfx::destroy(Handle);
            }
        }

        std::unordered_map<std::string, uint32_t> VertexAttributeLocations{};
        std::unordered_map<std::string, uint32_t> VertexUniformInfos{};
        std::unordered_map<std::string, uint32_t> FragmentUniformInfos{};

        bgfx::ProgramHandle Handle{bgfx::kInvalidHandle};

        struct UniformValue
        {
            std::vector<float> Data{};
            uint16_t ElementLength{};
        };

        std::unordered_map<uint16_t, UniformValue> Uniforms{};

        void SetUniform(bgfx::UniformHandle handle, gsl::span<const float> data, size_t elementLength = 1)
        {
            UniformValue& value = Uniforms[handle.idx];
            value.Data.assign(data.begin(), data.end());
            value.ElementLength = static_cast<uint16_t>(elementLength);
        }
    };

    template<typename Handle1T, typename Handle2T>
    class VariantHandleHolder
    {
    protected:
        VariantHandleHolder() = default;
        VariantHandleHolder(VariantHandleHolder<Handle1T, Handle2T>&& other)
        {
            m_handle = other.m_handle;

            constexpr auto nonDynamic = [](auto& handle) {
                // TODO: Fix this const cast
                const_cast<Handle1T&>(handle).idx = bgfx::kInvalidHandle;
            };
            constexpr auto dynamic = [](auto& handle) {
                // TODO: Fix this const cast
                const_cast<Handle2T&>(handle).idx = bgfx::kInvalidHandle;
            };
            other.DoForHandleTypes(nonDynamic, dynamic);
        }

        VariantHandleHolder(const VariantHandleHolder<Handle1T, Handle2T>& other) = delete;
        VariantHandleHolder<Handle1T, Handle2T>& operator=(VariantHandleHolder<Handle1T, Handle2T>&& other) = delete;
        VariantHandleHolder<Handle1T, Handle2T>& operator=(const VariantHandleHolder<Handle1T, Handle2T>& other) = delete;

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

        std::variant<Handle1T, Handle2T> m_handle{};
    };

    class IndexBufferData final : protected VariantHandleHolder<bgfx::IndexBufferHandle, bgfx::DynamicIndexBufferHandle>, public NativeResource<IndexBufferData>
    {
    public:
        IndexBufferData(const Napi::TypedArray& bytes, uint16_t flags, bool dynamic);
        IndexBufferData(IndexBufferData&& other) = default;
        ~IndexBufferData();
        void Update(Napi::Env env, const Napi::TypedArray& bytes, uint32_t startingIdx);
        void SetBgfxIndexBuffer(bgfx::Encoder* encoder, uint32_t firstIndex, uint32_t numIndices) const;
    };

    class VertexBufferData final : protected VariantHandleHolder<bgfx::VertexBufferHandle, bgfx::DynamicVertexBufferHandle>, public NativeResource<VertexBufferData>
    {
    public:
        VertexBufferData(const Napi::Uint8Array& bytes, bool dynamic);
        VertexBufferData(VertexBufferData&& other) = default;
        ~VertexBufferData();
        template<typename sourceType> void PromoteToFloats(uint32_t numElements, uint32_t byteOffset, uint32_t byteStride);
        void PromoteToFloats(bgfx::AttribType::Enum attribType, uint32_t numElements, uint32_t byteOffset, uint32_t byteStride);
        void EnsureFinalized(Napi::Env /*env*/, const bgfx::VertexLayout& layout);
        void Update(Napi::Env env, const Napi::Uint8Array& bytes, uint32_t offset, uint32_t byteLength);
        void SetAsBgfxVertexBuffer(bgfx::Encoder* encoder, uint8_t index, uint32_t startVertex, uint32_t numVertices, bgfx::VertexLayoutHandle layout) const;

    private:
        std::vector<uint8_t> m_bytes{};
    };

    struct VertexArray final : public NativeResource<VertexArray>
    {
        ~VertexArray()
        {
            for (auto& vertexBufferPair : VertexBuffers)
            {
                bgfx::destroy(vertexBufferPair.second.VertexLayoutHandle);
            }
        }

        struct IndexBuffer
        {
            const IndexBufferData* Data{};
        };

        IndexBuffer indexBuffer{};

        struct VertexBuffer
        {
            const VertexBufferData* Data{};
            uint32_t StartVertex{};
            bgfx::VertexLayoutHandle VertexLayoutHandle{};
        };

        std::unordered_map<uint32_t, VertexBuffer> VertexBuffers;
    };

    class CommandBufferDecoder final
    {
    public:
        using UInt8Buffer = gsl::span<const uint8_t>;
        using UInt32Buffer = gsl::span<const uint32_t>;
        using Float32Buffer = gsl::span<const float>;

        CommandBufferDecoder(UInt8Buffer commandBuffer, UInt32Buffer uint32ArgBuffer, Float32Buffer float32ArgBuffer) :
            m_commandBuffer{commandBuffer},
            m_uint32ArgBuffer{uint32ArgBuffer},
            m_float32ArgBuffer{float32ArgBuffer}
        {
        }

        bool TryDecodeCommand(uint8_t& command)
        {
            if (m_commandBufferIndex >= m_commandBuffer.size())
            {
                return false;
            }

            command = m_commandBuffer[m_commandBufferIndex++];
            return true;
        }

        uint32_t DecodeCommandArgAsUInt32()
        {
            return m_uint32ArgBuffer[m_uint32ArgBufferIndex++];
        }

        UInt32Buffer DecodeCommandArgAsUInt32s(UInt32Buffer::index_type count)
        {
            return m_uint32ArgBuffer.subspan((m_uint32ArgBufferIndex += count) - count, count);
        }

        float DecodeCommandArgAsFloat32()
        {
            return m_float32ArgBuffer[m_float32ArgBufferIndex++];
        }

        Float32Buffer DecodeCommandArgAsFloat32s(Float32Buffer::index_type count)
        {
            return m_float32ArgBuffer.subspan((m_float32ArgBufferIndex += count) - count, count);
        }

    private:
        UInt8Buffer::index_type m_commandBufferIndex{};
        UInt32Buffer::index_type m_uint32ArgBufferIndex{};
        Float32Buffer::index_type m_float32ArgBufferIndex{};

        UInt8Buffer m_commandBuffer;
        UInt32Buffer m_uint32ArgBuffer;
        Float32Buffer m_float32ArgBuffer;
    };

    class NativeEngine final : public Napi::ObjectWrap<NativeEngine>
    {
        static constexpr auto JS_CLASS_NAME = "_NativeEngine";
        static constexpr auto JS_ENGINE_CONSTRUCTOR_NAME = "Engine";

    public:
        NativeEngine(const Napi::CallbackInfo& info);
        NativeEngine(const Napi::CallbackInfo& info, JsRuntime& runtime);
        ~NativeEngine();

        static void Initialize(Napi::Env env);

    private:
        void Dispose();

        void Dispose(const Napi::CallbackInfo& info);
        void RequestAnimationFrame(const Napi::CallbackInfo& info);
        Napi::Value CreateVertexArray(const Napi::CallbackInfo& info);
        void DeleteVertexArray(const Napi::CallbackInfo& info);
        //void BindVertexArray(const Napi::CallbackInfo& info);
        void BindVertexArray(CommandBufferDecoder& decoder);
        Napi::Value CreateIndexBuffer(const Napi::CallbackInfo& info);
        void DeleteIndexBuffer(const Napi::CallbackInfo& info);
        void RecordIndexBuffer(const Napi::CallbackInfo& info);
        void UpdateDynamicIndexBuffer(const Napi::CallbackInfo& info);
        Napi::Value CreateVertexBuffer(const Napi::CallbackInfo& info);
        void DeleteVertexBuffer(const Napi::CallbackInfo& info);
        void RecordVertexBuffer(const Napi::CallbackInfo& info);
        void UpdateDynamicVertexBuffer(const Napi::CallbackInfo& info);
        Napi::Value CreateProgram(const Napi::CallbackInfo& info);
        Napi::Value GetUniforms(const Napi::CallbackInfo& info);
        Napi::Value GetAttributes(const Napi::CallbackInfo& info);
        void SetProgram(const Napi::CallbackInfo& info);
        //void SetState(const Napi::CallbackInfo& info);
        void SetState(CommandBufferDecoder& decoder);
        void SetZOffset(const Napi::CallbackInfo& info);
        Napi::Value GetZOffset(const Napi::CallbackInfo& info);
        void SetDepthTest(const Napi::CallbackInfo& info);
        Napi::Value GetDepthWrite(const Napi::CallbackInfo& info);
        void SetDepthWrite(const Napi::CallbackInfo& info);
        void SetColorWrite(const Napi::CallbackInfo& info);
        void SetBlendMode(const Napi::CallbackInfo& info);
        void SetMatrix(const Napi::CallbackInfo& info);
        void SetInt(const Napi::CallbackInfo& info);
        void SetIntArray(const Napi::CallbackInfo& info);
        void SetIntArray2(const Napi::CallbackInfo& info);
        void SetIntArray3(const Napi::CallbackInfo& info);
        void SetIntArray4(const Napi::CallbackInfo& info);
        void SetFloatArray(const Napi::CallbackInfo& info);
        void SetFloatArray2(const Napi::CallbackInfo& info);
        void SetFloatArray3(const Napi::CallbackInfo& info);
        void SetFloatArray4(const Napi::CallbackInfo& info);
        //void SetMatrices(const Napi::CallbackInfo& info);
        void SetMatrices(CommandBufferDecoder& decoder);
        void SetMatrix3x3(const Napi::CallbackInfo& info);
        void SetMatrix2x2(const Napi::CallbackInfo& info);
        //void SetFloat(const Napi::CallbackInfo& info);
        //void SetFloat2(const Napi::CallbackInfo& info);
        //void SetFloat3(const Napi::CallbackInfo& info);
        //void SetFloat4(const Napi::CallbackInfo& info);
        void SetFloat(CommandBufferDecoder& decoder);
        void SetFloat2(CommandBufferDecoder& decoder);
        void SetFloat3(CommandBufferDecoder& decoder);
        void SetFloat4(CommandBufferDecoder& decoder);
        Napi::Value CreateTexture(const Napi::CallbackInfo& info);
        void LoadTexture(const Napi::CallbackInfo& info);
        void CopyTexture(const Napi::CallbackInfo& info);
        void LoadRawTexture(const Napi::CallbackInfo& info);
        void LoadCubeTexture(const Napi::CallbackInfo& info);
        void LoadCubeTextureWithMips(const Napi::CallbackInfo& info);
        Napi::Value GetTextureWidth(const Napi::CallbackInfo& info);
        Napi::Value GetTextureHeight(const Napi::CallbackInfo& info);
        void SetTextureSampling(const Napi::CallbackInfo& info);
        //void SetTextureWrapMode(const Napi::CallbackInfo& info);
        void SetTextureWrapMode(CommandBufferDecoder& decoder);
        void SetTextureAnisotropicLevel(const Napi::CallbackInfo& info);
        //void SetTexture(const Napi::CallbackInfo& info);
        void SetTexture(CommandBufferDecoder& decoder);
        void SetTexture2(const Napi::CallbackInfo& info);
        void SetTexture3(const Napi::CallbackInfo& info);
        void DeleteTexture(const Napi::CallbackInfo& info);
        Napi::Value CreateFrameBuffer(const Napi::CallbackInfo& info);
        void DeleteFrameBuffer(const Napi::CallbackInfo& info);
        void BindFrameBuffer(const Napi::CallbackInfo& info);
        void UnbindFrameBuffer(const Napi::CallbackInfo& info);
        //void DrawIndexed(const Napi::CallbackInfo& info);
        void DrawIndexed(CommandBufferDecoder& decoder);
        void Draw(const Napi::CallbackInfo& info);
        void Clear(const Napi::CallbackInfo& info);
        Napi::Value GetRenderWidth(const Napi::CallbackInfo& info);
        Napi::Value GetRenderHeight(const Napi::CallbackInfo& info);
        void SetViewPort(const Napi::CallbackInfo& info);
        Napi::Value GetHardwareScalingLevel(const Napi::CallbackInfo& info);
        void SetHardwareScalingLevel(const Napi::CallbackInfo& info);
        Napi::Value CreateImageBitmap(const Napi::CallbackInfo& info);
        Napi::Value ResizeImageBitmap(const Napi::CallbackInfo& info);
        void GetFrameBufferData(const Napi::CallbackInfo& info);
        void SetStencil(const Napi::CallbackInfo& info);
        void SetCommandBuffer(const Napi::CallbackInfo& info);
        void SetCommandUint32Buffer(const Napi::CallbackInfo& info);
        void SetCommandFloat32Buffer(const Napi::CallbackInfo& info);
        void SubmitCommandBuffer(const Napi::CallbackInfo& info);
        void Draw(bgfx::Encoder* encoder, int fillMode);

        std::string ProcessShaderCoordinates(const std::string& vertexSource);

        GraphicsImpl::UpdateToken& GetUpdateToken();
        FrameBuffer& GetBoundFrameBuffer(bgfx::Encoder& encoder);

        std::shared_ptr<arcana::cancellation_source> m_cancellationSource{};

        ShaderCompiler m_shaderCompiler{};

        uint32_t m_currentProgram{};
        std::unordered_set<uint32_t> m_programDataCollection{};

        JsRuntime& m_runtime;
        GraphicsImpl& m_graphicsImpl;

        JsRuntimeScheduler m_runtimeScheduler;

        std::optional<GraphicsImpl::UpdateToken> m_updateToken{};

        void ScheduleRequestAnimationFrameCallbacks();
        bool m_requestAnimationFrameCallbacksScheduled{};

        bx::DefaultAllocator m_allocator{};
        uint64_t m_engineState{BGFX_STATE_DEFAULT};
        uint32_t m_stencilState{BGFX_STENCIL_TEST_ALWAYS | BGFX_STENCIL_FUNC_REF(0) | BGFX_STENCIL_FUNC_RMASK(0xFF) | BGFX_STENCIL_OP_FAIL_S_KEEP | BGFX_STENCIL_OP_FAIL_Z_KEEP | BGFX_STENCIL_OP_PASS_Z_REPLACE};

        template<int size, typename arrayType>
        void SetTypeArrayN(const Napi::CallbackInfo& info);

        template<int size>
        //void SetFloatN(const Napi::CallbackInfo& info);
        void SetFloatN(CommandBufferDecoder& decoder);

        template<int size>
        void SetMatrixN(const Napi::CallbackInfo& info);

        // Scratch vector used for data alignment.
        std::vector<float> m_scratch{};

        std::vector<Napi::FunctionReference> m_requestAnimationFrameCallbacks{};

        Napi::Reference<Napi::Uint8Array> m_commandBuffer{};
        Napi::Reference<Napi::Uint32Array> m_commandUint32Buffer{};
        Napi::Reference<Napi::Float32Array> m_commandFloat32Buffer{};
        inline static ResourceTable<void(NativeEngine::*)(CommandBufferDecoder&)> s_commandTable{};

        const VertexArray* m_boundVertexArray{};
        FrameBuffer m_defaultFrameBuffer;
        FrameBuffer* m_boundFrameBuffer{};
        PerFrameValue<bool> m_boundFrameBufferNeedsRebinding;
    };
}
