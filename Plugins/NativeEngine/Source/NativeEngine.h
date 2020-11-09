#pragma once

#include "ShaderCompiler.h"
#include "BgfxCallback.h"

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
    class ClearState final
    {
    public:
        void UpdateColor(float r, float g, float b, float a)
        {
            const bool needToUpdate = r != Red || g != Green || b != Blue || a != Alpha;
            if (needToUpdate)
            {
                Red = r;
                Green = g;
                Blue = b;
                Alpha = a;
                Update();
            }
        }

        void UpdateFlags(const Napi::CallbackInfo& info)
        {
            const auto flags = static_cast<uint16_t>(info[0].As<Napi::Number>().Uint32Value());
            Flags = flags;
            Update();
        }

        void UpdateDepth(const Napi::CallbackInfo& info)
        {
            const auto depth = info[0].As<Napi::Number>().FloatValue();
            const bool needToUpdate = Depth != depth;
            if (needToUpdate)
            {
                Depth = depth;
                Update();
            }
        }

        void UpdateStencil(const Napi::CallbackInfo& info)
        {
            const auto stencil = static_cast<uint8_t>(info[0].As<Napi::Number>().Int32Value());
            const bool needToUpdate = Stencil != stencil;
            if (needToUpdate)
            {
                Stencil = stencil;
                Update();
            }
        }

        arcana::weak_table<std::function<void()>>::ticket AddUpdateCallback(std::function<void()> callback)
        {
            return m_callbacks.insert(std::move(callback));
        }

        void Update()
        {
            m_callbacks.apply_to_all([](std::function<void()>& callback) {
                callback();
            });
        }

        uint32_t Color() const
        {
            uint32_t color = 0x0;
            color += static_cast<uint8_t>(Red * std::numeric_limits<uint8_t>::max());
            color = color << 8;
            color += static_cast<uint8_t>(Green * std::numeric_limits<uint8_t>::max());
            color = color << 8;
            color += static_cast<uint8_t>(Blue * std::numeric_limits<uint8_t>::max());
            color = color << 8;
            color += static_cast<uint8_t>(Alpha * std::numeric_limits<uint8_t>::max());
            return color;
        }

        float Red{68.f / 255.f};
        float Green{51.f / 255.f};
        float Blue{85.f / 255.f};
        float Alpha{1.f};
        float Depth{1.f};
        uint16_t Flags{BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH};
        uint8_t Stencil{0};

    private:
        arcana::weak_table<std::function<void()>> m_callbacks{};
    };

    class ViewClearState final
    {
    public:
        ViewClearState(uint16_t viewId, ClearState& clearState)
            : m_viewId{viewId} 
            , m_clearState{clearState}
            , m_callbackTicket{m_clearState.AddUpdateCallback([this]() { Update(); })}
        {
        }

        void UpdateColor(float r, float g, float b, float a = 1.f)
        {
            m_clearState.UpdateColor(r, g, b, a);
        }

        void UpdateColor(const Napi::CallbackInfo& info)
        {
            const auto r = info[0].As<Napi::Number>().FloatValue();
            const auto g = info[1].As<Napi::Number>().FloatValue();
            const auto b = info[2].As<Napi::Number>().FloatValue();
            const auto a = info[3].IsUndefined() ? 1.f : info[3].As<Napi::Number>().FloatValue();
            m_clearState.UpdateColor(r, g, b, a);
        }

        void UpdateFlags(const Napi::CallbackInfo& info)
        {
            m_clearState.UpdateFlags(info);
        }

        void UpdateDepth(const Napi::CallbackInfo& info)
        {
            m_clearState.UpdateDepth(info);
        }

        void UpdateStencil(const Napi::CallbackInfo& info)
        {
            m_clearState.UpdateStencil(info);
        }

        void UpdateViewId(uint16_t viewId)
        {
            m_viewId = viewId;
            Update();
        }

    private:

        void Update() const
        {
            bgfx::setViewClear(m_viewId, m_clearState.Flags, m_clearState.Color(), m_clearState.Depth, m_clearState.Stencil);
            // discard any previous set state
            bgfx::discard();
        }

        uint16_t m_viewId{};
        ClearState& m_clearState;
        arcana::weak_table<std::function<void()>>::ticket m_callbackTicket;
    };

    struct FrameBufferData final
    {
    private:
        std::unique_ptr<ClearState> m_clearState{};

    public:
        FrameBufferData(bgfx::FrameBufferHandle frameBuffer, uint16_t viewId, uint16_t width, uint16_t height, bool actAsBackBuffer = false)
            : m_clearState{std::make_unique<ClearState>()}
            , FrameBuffer{frameBuffer}
            , ViewId{viewId}
            , ViewClearState{ViewId, *m_clearState}
            , Width{width}
            , Height{height}
            , ActAsBackBuffer{actAsBackBuffer}
        {
            assert(ViewId < bgfx::getCaps()->limits.maxViews);
        }

        FrameBufferData(bgfx::FrameBufferHandle frameBuffer, uint16_t viewId, ClearState& clearState, uint16_t width, uint16_t height, bool actAsBackBuffer = false)
            : m_clearState{}
            , FrameBuffer{frameBuffer}
            , ViewId{viewId}
            , ViewClearState{ViewId, clearState}
            , Width{width}
            , Height{height}
            , ActAsBackBuffer{actAsBackBuffer}
        {
            assert(ViewId < bgfx::getCaps()->limits.maxViews);
        }

        FrameBufferData(FrameBufferData&) = delete;

        ~FrameBufferData()
        {
            bgfx::destroy(FrameBuffer);
        }

        void UseViewId(uint16_t viewId)
        {
            ViewId = viewId;
            ViewClearState.UpdateViewId(ViewId);
        }

        void SetUpView(uint16_t viewId)
        {
            bgfx::setViewFrameBuffer(viewId, FrameBuffer);
            UseViewId(viewId);
            bgfx::setViewRect(ViewId, 0, 0, Width, Height);
        }

        bgfx::FrameBufferHandle FrameBuffer{bgfx::kInvalidHandle};
        bgfx::ViewId ViewId{};
        Babylon::ViewClearState ViewClearState;
        uint16_t Width{};
        uint16_t Height{};
        // When a FrameBuffer acts as a back buffer, it means it will not be used as a texture in a shader.
        // For example as a post process. It will be used as-is in a swapchain or for direct rendering (XR)
        // When this flag is true, projection matrix will not be flipped for API that would normaly need it.
        // Namely Direct3D and Metal.
        bool ActAsBackBuffer{false};
    };

    struct FrameBufferManager final
    {
        FrameBufferManager()
        {
            m_boundFrameBuffer = m_backBuffer = new FrameBufferData(BGFX_INVALID_HANDLE, GetNewViewId(), bgfx::getStats()->width, bgfx::getStats()->height);
        }

        FrameBufferData* CreateNew(bgfx::FrameBufferHandle frameBufferHandle, uint16_t width, uint16_t height)
        {
            return new FrameBufferData(frameBufferHandle, GetNewViewId(), width, height);
        }

        FrameBufferData* CreateNew(bgfx::FrameBufferHandle frameBufferHandle, ClearState& clearState, uint16_t width, uint16_t height, bool actAsBackBuffer)
        {
            return new FrameBufferData(frameBufferHandle, GetNewViewId(), clearState, width, height, actAsBackBuffer);
        }

        void Bind(FrameBufferData* data)
        {
            m_boundFrameBuffer = data;

            // TODO: Consider doing this only on bgfx::reset(); the effects of this call don't survive reset, but as
            // long as there's no reset this doesn't technically need to be called every time the frame buffer is bound.
            m_boundFrameBuffer->SetUpView(GetNewViewId());

            // bgfx::setTexture()? Why?
            // TODO: View order?
            m_renderingToTarget = !m_boundFrameBuffer->ActAsBackBuffer;
        }

        FrameBufferData& GetBound() const
        {
            return *m_boundFrameBuffer;
        }

        void Unbind(FrameBufferData* data)
        {
            // this assert is commented because of an issue with XR described here : https://github.com/BabylonJS/BabylonNative/issues/344
            //assert(m_boundFrameBuffer == data);
            (void)data;
            m_boundFrameBuffer = m_backBuffer;
            m_renderingToTarget = false;
        }

        uint16_t GetNewViewId()
        {
            m_nextId++;
            assert(m_nextId < bgfx::getCaps()->limits.maxViews);
            return m_nextId;
        }

        void Reset()
        {
            m_nextId = 0;
        }

        bool IsRenderingToTarget() const
        {
            return m_renderingToTarget;
        }

    private:
        FrameBufferData* m_boundFrameBuffer{nullptr};
        FrameBufferData* m_backBuffer{nullptr};
        uint16_t m_nextId{0};
        bool m_renderingToTarget{false};
    };

    struct TextureData final
    {
        ~TextureData()
        {
            if (bgfx::isValid(Handle))
            {
                bgfx::destroy(Handle);
            }
        }

        bgfx::TextureHandle Handle{bgfx::kInvalidHandle};
        uint32_t Width{0};
        uint32_t Height{0};
        uint32_t Flags{0};
        uint8_t AnisotropicLevel{0};
    };

    struct ImageData final
    {
        ~ImageData()
        {
            if (Image)
            {
                bimg::imageFree(Image.get());
            }
        }
        std::unique_ptr<bimg::ImageContainer> Image;
    };

    struct UniformInfo final
    {
        uint8_t Stage{};
        bgfx::UniformHandle Handle{bgfx::kInvalidHandle};
        bool YFlip{false};
    };

    struct ProgramData final
    {
        ProgramData() = default;
        ProgramData(const ProgramData&) = delete;
        ProgramData(ProgramData&&) = delete;

        ~ProgramData()
        {
            bgfx::destroy(Program);
        }

        std::unordered_map<std::string, uint32_t> VertexAttributeLocations{};
        std::unordered_map<std::string, UniformInfo> VertexUniformInfos{};
        std::unordered_map<std::string, UniformInfo> FragmentUniformInfos{};

        bgfx::ProgramHandle Program{};

        struct UniformValue
        {
            std::vector<float> Data{};
            uint16_t ElementLength{};
            bool YFlip{false};
        };

        std::unordered_map<uint16_t, UniformValue> Uniforms{};

        void SetUniform(bgfx::UniformHandle handle, gsl::span<const float> data, bool YFlip, size_t elementLength = 1)
        {
            UniformValue& value = Uniforms[handle.idx];
            value.Data.assign(data.begin(), data.end());
            value.ElementLength = static_cast<uint16_t>(elementLength);
            value.YFlip = YFlip;
        }
    };

    class IndexBufferData;
    class VertexBufferData;

    struct VertexArray final
    {
        ~VertexArray()
        {
            for (auto& vertexBufferPair : vertexBuffers)
            {
                bgfx::destroy(vertexBufferPair.second.vertexLayoutHandle);
            }
        }

        struct IndexBuffer
        {
            const IndexBufferData* data{};
        };

        IndexBuffer indexBuffer{};

        struct VertexBuffer
        {
            const VertexBufferData* data{};
            uint32_t startVertex{};
            bgfx::VertexLayoutHandle vertexLayoutHandle{};
        };

        std::unordered_map<uint32_t, VertexBuffer> vertexBuffers;
    };

    class NativeEngine final : public Napi::ObjectWrap<NativeEngine>
    {
        static constexpr auto JS_CLASS_NAME = "_NativeEngine";
        static constexpr auto JS_ENGINE_CONSTRUCTOR_NAME = "Engine";
        static constexpr auto JS_AUTO_RENDER_PROPERTY_NAME = "_AUTO_RENDER";

    public:
        NativeEngine(const Napi::CallbackInfo& info);
        NativeEngine(const Napi::CallbackInfo& info, JsRuntime& runtime);
        ~NativeEngine();

        static void Initialize(Napi::Env, bool autoRender);

        FrameBufferManager& GetFrameBufferManager();
        void Dispatch(std::function<void()>);

        // IMPORTANT: Must be called from the JS thread.
        void ScheduleRender();

        const bool AutomaticRenderingEnabled{};
        JsRuntimeScheduler RuntimeScheduler;

    private:
        void Dispose();

        void Dispose(const Napi::CallbackInfo& info);
        Napi::Value GetEngine(const Napi::CallbackInfo& info); // TODO: Hack, temporary method. Remove as part of the change to get rid of NapiBridge.
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
        Napi::Value CreateDepthTexture(const Napi::CallbackInfo& info);
        void LoadTexture(const Napi::CallbackInfo& info);
        void CopyTexture(const Napi::CallbackInfo& info);
        void LoadCubeTexture(const Napi::CallbackInfo& info);
        void LoadCubeTextureWithMips(const Napi::CallbackInfo& info);
        Napi::Value GetTextureWidth(const Napi::CallbackInfo& info);
        Napi::Value GetTextureHeight(const Napi::CallbackInfo& info);
        void SetTextureSampling(const Napi::CallbackInfo& info);
        void SetTextureWrapMode(const Napi::CallbackInfo& info);
        void SetTextureAnisotropicLevel(const Napi::CallbackInfo& info);
        void SetTexture(const Napi::CallbackInfo& info);
        void DeleteTexture(const Napi::CallbackInfo& info);
        Napi::Value CreateFrameBuffer(const Napi::CallbackInfo& info);
        void DeleteFrameBuffer(const Napi::CallbackInfo& info);
        void BindFrameBuffer(const Napi::CallbackInfo& info);
        void UnbindFrameBuffer(const Napi::CallbackInfo& info);
        void DrawIndexed(const Napi::CallbackInfo& info);
        void Draw(const Napi::CallbackInfo& info);
        void Clear(const Napi::CallbackInfo& info);
        void ClearColor(const Napi::CallbackInfo& info);
        void ClearStencil(const Napi::CallbackInfo& info);
        void ClearDepth(const Napi::CallbackInfo& info);
        Napi::Value GetRenderWidth(const Napi::CallbackInfo& info);
        Napi::Value GetRenderHeight(const Napi::CallbackInfo& info);
        void SetViewPort(const Napi::CallbackInfo& info);
        void GetFramebufferData(const Napi::CallbackInfo& info);
        Napi::Value GetRenderAPI(const Napi::CallbackInfo& info);

        template<typename SchedulerT>
        arcana::task<void, std::exception_ptr> GetRequestAnimationFrameTask(SchedulerT&);
        
        bool m_isRenderScheduled{false};

        arcana::cancellation_source m_cancelSource{};

        ShaderCompiler m_shaderCompiler;

        ProgramData* m_currentProgram{nullptr};
        arcana::weak_table<std::unique_ptr<ProgramData>> m_programDataCollection{};

        JsRuntime& m_runtime;
        Graphics::Impl& m_graphicsImpl;

        bx::DefaultAllocator m_allocator;
        uint64_t m_engineState;

        FrameBufferManager m_frameBufferManager{};

        template<int size, typename arrayType>
        void SetTypeArrayN(const Napi::CallbackInfo& info);

        template<int size>
        void SetFloatN(const Napi::CallbackInfo& info);

        template<int size>
        void SetMatrixN(const Napi::CallbackInfo& info);

        // Scratch vector used for data alignment.
        std::vector<float> m_scratch{};
        
        Napi::FunctionReference m_requestAnimationFrameCallback{};

        // webgl/opengl draw call parameters allow to set first index and number of indices used for that call
        // but with bgfx, those parameters must be set when binding the index buffer
        // at the time of webgl binding, we don't know those values yet
        // so a pointer to the to-bind buffer is kept and the buffer is bound to bgfx at the time of the drawcall
        const IndexBufferData* m_currentBoundIndexBuffer{};
    };
}
