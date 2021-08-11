#pragma once

#include "BgfxCallback.h"
#include "FrameBuffer.h"
#include "PerFrameValue.h"
#include "ShaderCompiler.h"

#include <Babylon/JsRuntime.h>
#include <Babylon/JsRuntimeScheduler.h>

#include <GraphicsImpl.h>

#include <napi/napi.h>

#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <bimg/bimg.h>
#include <bx/allocator.h>

#include <gsl/gsl>

#include <assert.h>

#include <arcana/containers/weak_table.h>
#include <arcana/threading/cancellation.h>
#include <unordered_map>

namespace Babylon
{
    struct TextureData final
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

    struct UniformInfo final
    {
        uint8_t Stage{};
        bgfx::UniformHandle Handle{bgfx::kInvalidHandle};
    };

    struct ProgramData final
    {
        ProgramData() = default;
        ProgramData(const ProgramData&) = delete;
        ProgramData(ProgramData&&) = delete;

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

    class IndexBufferData;
    class VertexBufferData;

    struct VertexArray final
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

    template<typename T>
    class ResourceTable final {
    public:
        uint32_t Add(T&& resource)
        {
            const uint32_t resourceHandle{m_nextResourceId};
            m_resources.insert({resourceHandle, std::forward<T>(resource)});
            m_nextResourceId++;
            return resourceHandle;
        }

        T& Get(uint32_t resourceHandle)
        {
            return m_resources.at(resourceHandle);
        }

        void Remove(uint32_t resourceHandle)
        {
            m_resources.erase(resourceHandle);
        }

    private:
        uint32_t m_nextResourceId{1};
        std::unordered_map<uint32_t, T> m_resources{};
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
        void BindVertexArray(const Napi::CallbackInfo& info);
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
        void SetState(const Napi::CallbackInfo& info);
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
        void SetMatrices(const Napi::CallbackInfo& info);
        void SetMatrix3x3(const Napi::CallbackInfo& info);
        void SetMatrix2x2(const Napi::CallbackInfo& info);
        void SetFloat(const Napi::CallbackInfo& info);
        void SetFloat2(const Napi::CallbackInfo& info);
        void SetFloat3(const Napi::CallbackInfo& info);
        void SetFloat4(const Napi::CallbackInfo& info);
        Napi::Value CreateTexture(const Napi::CallbackInfo& info);
        void LoadTexture(const Napi::CallbackInfo& info);
        void CopyTexture(const Napi::CallbackInfo& info);
        void LoadRawTexture(const Napi::CallbackInfo& info);
        void LoadCubeTexture(const Napi::CallbackInfo& info);
        void LoadCubeTextureWithMips(const Napi::CallbackInfo& info);
        Napi::Value GetTextureWidth(const Napi::CallbackInfo& info);
        Napi::Value GetTextureHeight(const Napi::CallbackInfo& info);
        void SetTextureSampling(const Napi::CallbackInfo& info);
        void SetTextureWrapMode(const Napi::CallbackInfo& info);
        void SetTextureAnisotropicLevel(const Napi::CallbackInfo& info);
        void SetTexture(const Napi::CallbackInfo& info);
        void SetTexture2(const Napi::CallbackInfo& info);
        void SetTexture3(const Napi::CallbackInfo& info);
        void DeleteTexture(const Napi::CallbackInfo& info);
        Napi::Value CreateFrameBuffer(const Napi::CallbackInfo& info);
        void DeleteFrameBuffer(const Napi::CallbackInfo& info);
        void BindFrameBuffer(const Napi::CallbackInfo& info);
        void UnbindFrameBuffer(const Napi::CallbackInfo& info);
        void DrawIndexed(const Napi::CallbackInfo& info);
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
        void Draw(bgfx::Encoder* encoder, int fillMode);

        std::string ProcessShaderCoordinates(const std::string& vertexSource);

        GraphicsImpl::UpdateToken& GetUpdateToken();
        FrameBuffer& GetBoundFrameBuffer(bgfx::Encoder& encoder);

        std::shared_ptr<arcana::cancellation_source> m_cancellationSource{};

        ShaderCompiler m_shaderCompiler{};

        ProgramData* m_currentProgram{nullptr};
        arcana::weak_table<std::unique_ptr<ProgramData>> m_programDataCollection{};

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
        void SetFloatN(const Napi::CallbackInfo& info);

        template<int size>
        void SetMatrixN(const Napi::CallbackInfo& info);

        // Scratch vector used for data alignment.
        std::vector<float> m_scratch{};

        std::vector<Napi::FunctionReference> m_requestAnimationFrameCallbacks{};

        ResourceTable<UniformInfo> m_uniformInfos{};
        ResourceTable<TextureData> m_textures{};
        ResourceTable<VertexArray> m_vertexArrays{};
        ResourceTable<IndexBufferData*> m_indexBuffers{};

        const VertexArray* m_boundVertexArray{};
        FrameBuffer m_defaultFrameBuffer;
        FrameBuffer* m_boundFrameBuffer{};
        PerFrameValue<bool> m_boundFrameBufferNeedsRebinding;
    };
}
