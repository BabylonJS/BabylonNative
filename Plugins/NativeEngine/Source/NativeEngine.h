#pragma once

#include "NativeDataStream.h"
#include "PerFrameValue.h"
#include "Program.h"
#include "ShaderProvider.h"
#include "VertexArray.h"

#include <Babylon/JsRuntime.h>
#include <Babylon/JsRuntimeScheduler.h>

#include <Babylon/Graphics/DeviceContext.h>
#include <Babylon/Graphics/BgfxCallback.h>
#include <Babylon/Graphics/FrameBuffer.h>

#include <napi/napi.h>

#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <bimg/bimg.h>
#include <bx/allocator.h>

#include <gsl/gsl>

#include <arcana/threading/cancellation.h>
#include <map>

namespace Babylon
{
    class NativeEngine final : public Napi::ObjectWrap<NativeEngine>
    {
        static constexpr auto JS_CLASS_NAME = "_NativeEngine";
        static constexpr auto JS_CONSTRUCTOR_NAME = "Engine";

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
        void DeleteVertexArray(NativeDataStream::Reader& data);
        void BindVertexArray(NativeDataStream::Reader& data);
        Napi::Value CreateIndexBuffer(const Napi::CallbackInfo& info);
        void DeleteIndexBuffer(NativeDataStream::Reader& data);
        void RecordIndexBuffer(const Napi::CallbackInfo& info);
        void UpdateDynamicIndexBuffer(const Napi::CallbackInfo& info);
        Napi::Value CreateVertexBuffer(const Napi::CallbackInfo& info);
        void DeleteVertexBuffer(NativeDataStream::Reader& data);
        void RecordVertexBuffer(const Napi::CallbackInfo& info);
        void UpdateDynamicVertexBuffer(const Napi::CallbackInfo& info);
        Napi::Value CreateProgram(const Napi::CallbackInfo& info);
        Napi::Value CreateProgramAsync(const Napi::CallbackInfo& info);
        Napi::Value GetUniforms(const Napi::CallbackInfo& info);
        Napi::Value GetAttributes(const Napi::CallbackInfo& info);
        void SetProgram(NativeDataStream::Reader& data);
        void DeleteProgram(NativeDataStream::Reader& data);
        void SetState(NativeDataStream::Reader& data);
        void SetZOffset(NativeDataStream::Reader& data);
        void SetZOffsetUnits(NativeDataStream::Reader& data);
        void SetDepthTest(NativeDataStream::Reader& data);
        void SetDepthWrite(NativeDataStream::Reader& data);
        void SetColorWrite(NativeDataStream::Reader& data);
        void SetBlendMode(NativeDataStream::Reader& data);
        void SetMatrix(NativeDataStream::Reader& data);
        void SetInt(NativeDataStream::Reader& data);
        void SetIntArray(NativeDataStream::Reader& data);
        void SetIntArray2(NativeDataStream::Reader& data);
        void SetIntArray3(NativeDataStream::Reader& data);
        void SetIntArray4(NativeDataStream::Reader& data);
        void SetFloatArray(NativeDataStream::Reader& data);
        void SetFloatArray2(NativeDataStream::Reader& data);
        void SetFloatArray3(NativeDataStream::Reader& data);
        void SetFloatArray4(NativeDataStream::Reader& data);
        void SetMatrices(NativeDataStream::Reader& data);
        void SetMatrix3x3(NativeDataStream::Reader& data);
        void SetMatrix2x2(NativeDataStream::Reader& data);
        void SetFloat(NativeDataStream::Reader& data);
        void SetFloat2(NativeDataStream::Reader& data);
        void SetFloat3(NativeDataStream::Reader& data);
        void SetFloat4(NativeDataStream::Reader& data);
        Napi::Value CreateTexture(const Napi::CallbackInfo& info);
        void InitializeTexture(const Napi::CallbackInfo& info);
        void LoadTexture(const Napi::CallbackInfo& info);
        void CopyTexture(NativeDataStream::Reader& data);
        void LoadRawTexture(const Napi::CallbackInfo& info);
        void LoadRawTexture2DArray(const Napi::CallbackInfo& info);
        void LoadCubeTexture(const Napi::CallbackInfo& info);
        void LoadCubeTextureWithMips(const Napi::CallbackInfo& info);
        Napi::Value GetTextureWidth(const Napi::CallbackInfo& info);
        Napi::Value GetTextureHeight(const Napi::CallbackInfo& info);
        void SetTextureSampling(NativeDataStream::Reader& data);
        void SetTextureWrapMode(NativeDataStream::Reader& data);
        void SetTextureAnisotropicLevel(NativeDataStream::Reader& data);
        void SetTexture(NativeDataStream::Reader& data);
        void UnsetTexture(NativeDataStream::Reader& data);
        void DiscardAllTextures(NativeDataStream::Reader& data);
        void DeleteTexture(const Napi::CallbackInfo& info);
        Napi::Value ReadTexture(const Napi::CallbackInfo& info);
        Napi::Value CreateFrameBuffer(const Napi::CallbackInfo& info);
        void DeleteFrameBuffer(NativeDataStream::Reader& data);
        void BindFrameBuffer(NativeDataStream::Reader& data);
        void UnbindFrameBuffer(NativeDataStream::Reader& data);
        void DrawIndexed(NativeDataStream::Reader& data);
        void DrawIndexedInstanced(NativeDataStream::Reader& data);
        void Draw(NativeDataStream::Reader& data);
        void DrawInstanced(NativeDataStream::Reader& data);
        void Clear(NativeDataStream::Reader& data);
        Napi::Value GetRenderWidth(const Napi::CallbackInfo& info);
        Napi::Value GetRenderHeight(const Napi::CallbackInfo& info);
        Napi::Value GetHardwareScalingLevel(const Napi::CallbackInfo& info);
        void SetHardwareScalingLevel(const Napi::CallbackInfo& info);
        Napi::Value CreateImageBitmap(const Napi::CallbackInfo& info);
        Napi::Value ResizeImageBitmap(const Napi::CallbackInfo& info);
        void GetFrameBufferData(const Napi::CallbackInfo& info);
        void SetRenderResetCallback(const Napi::CallbackInfo& info);
        void SetStencil(NativeDataStream::Reader& data);
        void SetViewPort(NativeDataStream::Reader& data);
        void SetScissor(NativeDataStream::Reader& data);
        void SetCommandDataStream(const Napi::CallbackInfo& info);
        void SubmitCommands(const Napi::CallbackInfo& info);
        void PopulateFrameStats(const Napi::CallbackInfo& info);
        void DrawInternal(bgfx::Encoder* encoder, uint32_t fillMode);

        Graphics::UpdateToken& GetUpdateToken();
        Graphics::FrameBuffer& GetBoundFrameBuffer(bgfx::Encoder& encoder);

        std::shared_ptr<arcana::cancellation_source> m_cancellationSource{};

        ShaderProvider m_shaderProvider{};

        Program* m_currentProgram{nullptr};

        JsRuntime& m_runtime;
        Graphics::DeviceContext& m_deviceContext;
        Graphics::Update m_update;

        JsRuntimeScheduler m_runtimeScheduler;

        std::optional<Graphics::UpdateToken> m_updateToken{};

        void ScheduleRequestAnimationFrameCallbacks();
        bool m_requestAnimationFrameCallbacksScheduled{};

        uint64_t m_engineState{BGFX_STATE_DEFAULT};
        uint32_t m_stencilState{BGFX_STENCIL_TEST_ALWAYS | BGFX_STENCIL_FUNC_REF(0) | BGFX_STENCIL_FUNC_RMASK(0xFF) | BGFX_STENCIL_OP_FAIL_S_KEEP | BGFX_STENCIL_OP_FAIL_Z_KEEP | BGFX_STENCIL_OP_PASS_Z_REPLACE};

        template<int size, typename arrayType>
        void SetTypeArrayN(const UniformInfo& uniformInfo, const uint32_t elementLength, const arrayType& array);

        template<int size>
        void SetIntArrayN(NativeDataStream::Reader& data);

        template<int size>
        void SetFloatArrayN(NativeDataStream::Reader& data);

        template<int size>
        void SetFloatN(NativeDataStream::Reader& data);

        template<int size>
        void SetMatrixN(NativeDataStream::Reader& data);

        // Scratch vector used for data alignment.
        std::vector<float> m_scratch{};

        std::vector<Napi::FunctionReference> m_requestAnimationFrameCallbacks{};

        VertexArray* m_boundVertexArray{};
        Graphics::FrameBuffer m_defaultFrameBuffer;
        Graphics::FrameBuffer* m_boundFrameBuffer{};
        PerFrameValue<bool> m_boundFrameBufferNeedsRebinding;

        // TODO: This should be changed to a non-owning ref once multi-update is available.
        NativeDataStream* m_commandStream{};

        // Information from the JS side used for backwards compatibility.
        struct
        {
            bool NonFloatVertexBuffers{};
        } m_jsInfo;
    };
}
