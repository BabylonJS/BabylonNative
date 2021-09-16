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
#include <sstream>

#include <arcana/threading/cancellation.h>
#include <unordered_map>

// #include <Foundation/Foundation.h>

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
        ProgramData(ProgramData&& other) = delete;
        ProgramData(const ProgramData&) = delete;
        ProgramData& operator=(ProgramData&& other) = delete;
        ProgramData& operator=(const ProgramData& other) = delete;

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
        VariantHandleHolder(VariantHandleHolder<Handle1T, Handle2T>&& other) = delete;
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
        ~IndexBufferData();
        void Update(Napi::Env env, const Napi::TypedArray& bytes, uint32_t startingIdx);
        void SetBgfxIndexBuffer(bgfx::Encoder* encoder, uint32_t firstIndex, uint32_t numIndices) const;
    };

    class VertexBufferData final : protected VariantHandleHolder<bgfx::VertexBufferHandle, bgfx::DynamicVertexBufferHandle>, public NativeResource<VertexBufferData>
    {
    public:
        VertexBufferData(const Napi::Uint8Array& bytes, bool dynamic);
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
        using Int32Buffer = gsl::span<const int32_t>;
        using Float32Buffer = gsl::span<const float>;

        static constexpr uint8_t COMMAND_VALIDATION_COMMAND{0};
        static constexpr uint8_t COMMAND_VALIDATION_UINT32{1};
        static constexpr uint8_t COMMAND_VALIDATION_INT32{2};
        static constexpr uint8_t COMMAND_VALIDATION_FLOAT{3};

        CommandBufferDecoder(UInt8Buffer commandBuffer, UInt32Buffer uint32ArgBuffer, Int32Buffer int32ArgBuffer, Float32Buffer float32ArgBuffer, std::optional<UInt8Buffer> validationBuffer) :
            m_commandBuffer{commandBuffer},
            m_uint32ArgBuffer{uint32ArgBuffer},
            m_int32ArgBuffer{int32ArgBuffer},
            m_float32ArgBuffer{float32ArgBuffer},
            m_validationBuffer{validationBuffer}
        {
        }

        bool TryDecodeCommand(uint8_t& command)
        {
            if (m_commandBufferIndex >= m_commandBuffer.size())
            {
                return false;
            }

            ValidateDecodeOperation(COMMAND_VALIDATION_COMMAND);

            command = m_commandBuffer[m_commandBufferIndex++];
            //NSLog(@"COMMAND BUFFER: Decode command: %u", command);
            return true;
        }

        uint32_t DecodeCommandArgAsUInt32()
        {
            ValidateDecodeOperation(COMMAND_VALIDATION_UINT32);
            ValidateDecodeOperation(1);

            auto ret = m_uint32ArgBuffer[m_uint32ArgBufferIndex++];
            //NSLog(@"COMMAND BUFFER:   Decode uint32: %u", ret);
            return ret;
        }

        UInt32Buffer DecodeCommandArgAsUInt32s(UInt32Buffer::index_type count)
        {
            ValidateDecodeOperation(COMMAND_VALIDATION_UINT32);
            ValidateDecodeOperation(count);

            auto ret = m_uint32ArgBuffer.subspan((m_uint32ArgBufferIndex += count) - count, count);
            //NSLog(@"COMMAND BUFFER:   Decode uint32s: %u", static_cast<uint32_t>(ret.size()));
            return ret;
        }

        int32_t DecodeCommandArgAsInt32()
        {
            ValidateDecodeOperation(COMMAND_VALIDATION_INT32);
            ValidateDecodeOperation(1);

            auto ret = m_int32ArgBuffer[m_int32ArgBufferIndex++];
            //NSLog(@"COMMAND BUFFER:   Decode uint32: %u", ret);
            return ret;
        }

        Int32Buffer DecodeCommandArgAsInt32s(UInt32Buffer::index_type count)
        {
            ValidateDecodeOperation(COMMAND_VALIDATION_INT32);
            ValidateDecodeOperation(count);

            auto ret = m_int32ArgBuffer.subspan((m_int32ArgBufferIndex += count) - count, count);
            //NSLog(@"COMMAND BUFFER:   Decode uint32s: %u", static_cast<uint32_t>(ret.size()));
            return ret;
        }

        float DecodeCommandArgAsFloat32()
        {
            ValidateDecodeOperation(COMMAND_VALIDATION_FLOAT);
            ValidateDecodeOperation(1);

            auto ret = m_float32ArgBuffer[m_float32ArgBufferIndex++];
            //NSLog(@"COMMAND BUFFER:   Decode float32: %f", ret);
            return ret;
        }

        Float32Buffer DecodeCommandArgAsFloat32s(Float32Buffer::index_type count)
        {
            ValidateDecodeOperation(COMMAND_VALIDATION_FLOAT);
            ValidateDecodeOperation(count);

            auto ret = m_float32ArgBuffer.subspan((m_float32ArgBufferIndex += count) - count, count);
            //NSLog(@"COMMAND BUFFER:   Decode float32s: %u", static_cast<uint32_t>(ret.size()));
            return ret;
        }

    private:
        UInt8Buffer::index_type m_commandBufferIndex{};
        UInt32Buffer::index_type m_uint32ArgBufferIndex{};
        Int32Buffer::index_type m_int32ArgBufferIndex{};
        Float32Buffer::index_type m_float32ArgBufferIndex{};
        UInt8Buffer::index_type m_validationBufferIndex{};

        UInt8Buffer m_commandBuffer;
        UInt32Buffer m_uint32ArgBuffer;
        Int32Buffer m_int32ArgBuffer;
        Float32Buffer m_float32ArgBuffer;
        std::optional<UInt8Buffer> m_validationBuffer;

        void ValidateDecodeOperation(const uint8_t decodeOperation)
        {
            if (m_validationBuffer)
            {
                const uint8_t encodeOperation{m_validationBuffer.value()[m_validationBufferIndex++]};
                if (encodeOperation != decodeOperation)
                {
                    std::ostringstream message;
                    message << "Decoding " << ConvertOperationCodeToString(decodeOperation) << " when " << ConvertOperationCodeToString(encodeOperation) << " was encoded.";
                    throw std::runtime_error{message.str()};
                }
            }
        }

        static std::string ConvertOperationCodeToString(const uint8_t operationCode)
        {
            switch(operationCode)
            {
                case COMMAND_VALIDATION_COMMAND: return "command";
                case COMMAND_VALIDATION_UINT32: return "uint32 argument";
                case COMMAND_VALIDATION_INT32: return "int32 argument";
                case COMMAND_VALIDATION_FLOAT: return "float argument";

                default:
                    std::ostringstream message;
                    message << "argument count of " << operationCode;
                    return message.str();
            }
        }
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
        void DeleteVertexArray(CommandBufferDecoder& decoder);
        void BindVertexArray(CommandBufferDecoder& decoder);
        Napi::Value CreateIndexBuffer(const Napi::CallbackInfo& info);
        void DeleteIndexBuffer(CommandBufferDecoder& decoder);
        void RecordIndexBuffer(const Napi::CallbackInfo& info);
        void UpdateDynamicIndexBuffer(const Napi::CallbackInfo& info);
        Napi::Value CreateVertexBuffer(const Napi::CallbackInfo& info);
        void DeleteVertexBuffer(CommandBufferDecoder& decoder);
        void RecordVertexBuffer(const Napi::CallbackInfo& info);
        void UpdateDynamicVertexBuffer(const Napi::CallbackInfo& info);
        Napi::Value CreateProgram(const Napi::CallbackInfo& info);
        Napi::Value GetUniforms(const Napi::CallbackInfo& info);
        Napi::Value GetAttributes(const Napi::CallbackInfo& info);
        void SetProgram(CommandBufferDecoder& decoder);
        void SetState(CommandBufferDecoder& decoder);
        void SetZOffset(CommandBufferDecoder& decoder);
        void SetDepthTest(CommandBufferDecoder& decoder);
        void SetDepthWrite(CommandBufferDecoder& decoder);
        void SetColorWrite(CommandBufferDecoder& decoder);
        void SetBlendMode(CommandBufferDecoder& decoder);
        void SetMatrix(CommandBufferDecoder& decoder);
        void SetInt(CommandBufferDecoder& decoder);
        void SetIntArray(CommandBufferDecoder& decoder);
        void SetIntArray2(CommandBufferDecoder& decoder);
        void SetIntArray3(CommandBufferDecoder& decoder);
        void SetIntArray4(CommandBufferDecoder& decoder);
        void SetFloatArray(CommandBufferDecoder& decoder);
        void SetFloatArray2(CommandBufferDecoder& decoder);
        void SetFloatArray3(CommandBufferDecoder& decoder);
        void SetFloatArray4(CommandBufferDecoder& decoder);
        void SetMatrices(CommandBufferDecoder& decoder);
        void SetMatrix3x3(CommandBufferDecoder& decoder);
        void SetMatrix2x2(CommandBufferDecoder& decoder);
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
        void SetTextureSampling(const Napi::CallbackInfo& info); //
        void SetTextureWrapMode(CommandBufferDecoder& decoder);
        void SetTextureAnisotropicLevel(const Napi::CallbackInfo& info); //
        void SetTexture(CommandBufferDecoder& decoder);
        void DeleteTexture(const Napi::CallbackInfo& info);
        Napi::Value CreateFrameBuffer(const Napi::CallbackInfo& info);
        void DeleteFrameBuffer(CommandBufferDecoder& decoder);
        void BindFrameBuffer(CommandBufferDecoder& decoder);
        void UnbindFrameBuffer(CommandBufferDecoder& decoder);
        void DrawIndexed(CommandBufferDecoder& decoder);
        void Draw(CommandBufferDecoder& decoder);
        void Clear(CommandBufferDecoder& decoder);
        Napi::Value GetRenderWidth(const Napi::CallbackInfo& info);
        Napi::Value GetRenderHeight(const Napi::CallbackInfo& info);
        void SetViewPort(const Napi::CallbackInfo& info);
        Napi::Value GetHardwareScalingLevel(const Napi::CallbackInfo& info);
        void SetHardwareScalingLevel(const Napi::CallbackInfo& info);
        Napi::Value CreateImageBitmap(const Napi::CallbackInfo& info);
        Napi::Value ResizeImageBitmap(const Napi::CallbackInfo& info);
        void GetFrameBufferData(const Napi::CallbackInfo& info);
        void SetStencil(CommandBufferDecoder& decoder);
        void SetCommandBuffer(const Napi::CallbackInfo& info);
        void SetCommandUint32Buffer(const Napi::CallbackInfo& info);
        void SetCommandInt32Buffer(const Napi::CallbackInfo& info);
        void SetCommandFloat32Buffer(const Napi::CallbackInfo& info);
        void SetCommandValidationBuffer(const Napi::CallbackInfo& info);
        void SubmitCommandBuffer(const Napi::CallbackInfo& info);
        void Draw(bgfx::Encoder* encoder, int fillMode);

        std::string ProcessShaderCoordinates(const std::string& vertexSource);

        GraphicsImpl::UpdateToken& GetUpdateToken();
        FrameBuffer& GetBoundFrameBuffer(bgfx::Encoder& encoder);

        std::shared_ptr<arcana::cancellation_source> m_cancellationSource{};

        ShaderCompiler m_shaderCompiler{};

        ProgramData* m_currentProgram{nullptr};
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
        void SetTypeArrayN(const UniformInfo& uniformInfo, const uint32_t elementLength, const arrayType& array);

        template<int size>
        void SetIntArrayN(CommandBufferDecoder& decoder);

        template<int size>
        void SetFloatArrayN(CommandBufferDecoder& decoder);

        template<int size>
        void SetFloatN(CommandBufferDecoder& decoder);

        template<int size>
        void SetMatrixN(CommandBufferDecoder& decoder);

        // Scratch vector used for data alignment.
        std::vector<float> m_scratch{};

        std::vector<Napi::FunctionReference> m_requestAnimationFrameCallbacks{};

        Napi::Reference<Napi::Uint8Array> m_commandBuffer{};
        Napi::Reference<Napi::Uint32Array> m_commandUint32Buffer{};
        Napi::Reference<Napi::Int32Array> m_commandInt32Buffer{};
        Napi::Reference<Napi::Float32Array> m_commandFloat32Buffer{};
        Napi::Reference<Napi::Uint8Array> m_commandValidationBuffer{};
        inline static ResourceTable<void(NativeEngine::*)(CommandBufferDecoder&)> s_commandTable{};

        const VertexArray* m_boundVertexArray{};
        FrameBuffer m_defaultFrameBuffer;
        FrameBuffer* m_boundFrameBuffer{};
        PerFrameValue<bool> m_boundFrameBufferNeedsRebinding;
    };
}
