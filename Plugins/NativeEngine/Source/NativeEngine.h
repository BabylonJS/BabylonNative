#pragma once

#include "PerFrameValue.h"
#include "ShaderCompiler.h"
#include "VertexArray.h"

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

        // CreationFlags contains flags used at texture creation
        // regarding BLIT support and READBACK
        uint64_t CreationFlags{0};
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
        std::unordered_map<std::string, ResourceTable<UniformInfo>::handle> UniformInfos{};

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

        CommandBufferDecoder(
            const uint32_t commandCount,
            const void* data,
            const uint32_t byteCount, 
            const std::optional<UInt8Buffer> validationBuffer) 
            : m_commandCount{commandCount}
            , m_commandBuffer{gsl::make_span(reinterpret_cast<const uint8_t*>(data), byteCount)}
            , m_uint32ArgBuffer{gsl::make_span(reinterpret_cast<const uint32_t*>(data), byteCount / 4)}
            , m_int32ArgBuffer{gsl::make_span(reinterpret_cast<const int32_t*>(data), byteCount / 4)}
            , m_float32ArgBuffer{gsl::make_span(reinterpret_cast<const float*>(data), byteCount / 4)}
            , m_validationBuffer{validationBuffer}
        {
        }

        bool TryDecodeCommand(uint8_t& command)
        {
            if (m_commandCount == 0)
            {
                return false;
            }

            command = m_commandBuffer[m_commandIndex++];

            ValidateDecodeOperation(COMMAND_VALIDATION_COMMAND);

            if (m_commandIndex % 4 == 0)
            {
                m_commandIndex = m_argumentIndex * 4;
                m_argumentIndex++;
            }

            // NSLog(@"COMMAND BUFFER: Decode command: %u", command);
            m_commandCount--;
            return true;
        }

        uint32_t DecodeCommandArgAsUInt32()
        {
            ValidateDecodeOperation(COMMAND_VALIDATION_UINT32);
            ValidateDecodeOperation(1);

            auto ret = m_uint32ArgBuffer[m_argumentIndex++];
            //NSLog(@"COMMAND BUFFER:   Decode uint32: %u", ret);
            return ret;
        }

        UInt32Buffer DecodeCommandArgAsUInt32s(UInt32Buffer::index_type count)
        {
            ValidateDecodeOperation(COMMAND_VALIDATION_UINT32);
            ValidateDecodeOperation(count);

            auto ret = m_uint32ArgBuffer.subspan((m_argumentIndex += count) - count, count);
            //NSLog(@"COMMAND BUFFER:   Decode uint32s: %u", static_cast<uint32_t>(ret.size()));
            return ret;
        }

        int32_t DecodeCommandArgAsInt32()
        {
            ValidateDecodeOperation(COMMAND_VALIDATION_INT32);
            ValidateDecodeOperation(1);

            auto ret = m_int32ArgBuffer[m_argumentIndex++];
            //NSLog(@"COMMAND BUFFER:   Decode uint32: %u", ret);
            return ret;
        }

        Int32Buffer DecodeCommandArgAsInt32s(UInt32Buffer::index_type count)
        {
            ValidateDecodeOperation(COMMAND_VALIDATION_INT32);
            ValidateDecodeOperation(count);

            auto ret = m_int32ArgBuffer.subspan((m_argumentIndex += count) - count, count);
            //NSLog(@"COMMAND BUFFER:   Decode uint32s: %u", static_cast<uint32_t>(ret.size()));
            return ret;
        }

        float DecodeCommandArgAsFloat32()
        {
            ValidateDecodeOperation(COMMAND_VALIDATION_FLOAT);
            ValidateDecodeOperation(1);

            auto ret = m_float32ArgBuffer[m_argumentIndex++];
            //NSLog(@"COMMAND BUFFER:   Decode float32: %f", ret);
            return ret;
        }

        Float32Buffer DecodeCommandArgAsFloat32s(Float32Buffer::index_type count)
        {
            ValidateDecodeOperation(COMMAND_VALIDATION_FLOAT);
            ValidateDecodeOperation(count);

            auto ret = m_float32ArgBuffer.subspan((m_argumentIndex += count) - count, count);
            //NSLog(@"COMMAND BUFFER:   Decode float32s: %u", static_cast<uint32_t>(ret.size()));
            return ret;
        }

    private:
        uint32_t m_commandCount;

        UInt8Buffer::index_type m_commandIndex{0};
        ptrdiff_t m_argumentIndex{1};
        UInt8Buffer::index_type m_validationBufferIndex{0};

        UInt8Buffer m_commandBuffer;
        UInt32Buffer m_uint32ArgBuffer;
        Int32Buffer m_int32ArgBuffer;
        Float32Buffer m_float32ArgBuffer;
        std::optional<UInt8Buffer> m_validationBuffer;

        void ValidateDecodeOperation(const size_t decodeOperation)
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

        static std::string ConvertOperationCodeToString(const size_t operationCode)
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
        void DeleteProgram(CommandBufferDecoder& decoder);
        void SetState(CommandBufferDecoder& decoder);
        void SetZOffset(CommandBufferDecoder& decoder);
        void SetZOffsetUnits(CommandBufferDecoder& decoder);
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
        void SetTextureSampling(CommandBufferDecoder& decoder);
        void SetTextureWrapMode(CommandBufferDecoder& decoder);
        void SetTextureAnisotropicLevel(CommandBufferDecoder& decoder);
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
        void SetCommandValidationBuffer(const Napi::CallbackInfo& info);
        void SubmitCommandBuffer(const Napi::CallbackInfo& info);
        void Draw(bgfx::Encoder* encoder, uint32_t fillMode);

        std::string ProcessShaderCoordinates(const std::string& vertexSource);

        GraphicsImpl::UpdateToken& GetUpdateToken();
        FrameBuffer& GetBoundFrameBuffer(bgfx::Encoder& encoder);

        std::shared_ptr<arcana::cancellation_source> m_cancellationSource{};

        ShaderCompiler m_shaderCompiler{};

        ProgramData* m_currentProgram{nullptr};
        std::unordered_set<ResourceTable<ProgramData>::handle> m_programDataCollection{};

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

        Napi::Reference<Napi::ArrayBuffer> m_commandBuffer{};
        Napi::Reference<Napi::ArrayBuffer> m_commandValidationBuffer{};
        inline static ResourceTable<void(NativeEngine::*)(CommandBufferDecoder&)> s_commandTable{};

        VertexArray* m_boundVertexArray{};
        FrameBuffer m_defaultFrameBuffer;
        FrameBuffer* m_boundFrameBuffer{};
        PerFrameValue<bool> m_boundFrameBufferNeedsRebinding;
    };
}
