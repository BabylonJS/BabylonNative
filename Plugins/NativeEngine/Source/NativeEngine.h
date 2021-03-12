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

        void UpdateFlags(uint16_t flags)
        {
            Flags = flags;
            Update();
        }

        void UpdateDepth(float depth)
        {
            const bool needToUpdate = Depth != depth;
            if (needToUpdate)
            {
                Depth = depth;
                Update();
            }
        }

        void UpdateStencil(uint8_t stencil)
        {
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

        void UpdateFlags(uint16_t flags)
        {
            m_clearState.UpdateFlags(flags);
        }

        void UpdateDepth(float depth)
        {
            m_clearState.UpdateDepth(depth);
        }

        void UpdateStencil(uint8_t stencil)
        {
            m_clearState.UpdateStencil(stencil);
        }

        void UpdateViewId(uint16_t viewId)
        {
            m_viewId = viewId;
            Update(false);
        }

    private:

        void Update(bool forceClear = true) const
        {
            bgfx::setViewClear(m_viewId, m_clearState.Flags, m_clearState.Color(), m_clearState.Depth, m_clearState.Stencil);
            if (forceClear)
            {
                // Discard any previously set state
                bgfx::discard();
                // Submit an empty primitive so we always clear the framebuffer on bgfx::frame,
                // even if no other geometry is rendered to this view.
                bgfx::touch(m_viewId);
            }
        }

        uint16_t m_viewId{};
        ClearState& m_clearState;
        arcana::weak_table<std::function<void()>>::ticket m_callbackTicket;
    };

    struct FrameBufferData final
    {
    private:
        arcana::weak_table<FrameBufferData*>::ticket m_managerTicket;
        std::unique_ptr<ClearState> m_clearState{};

    public:
        FrameBufferData(bgfx::FrameBufferHandle frameBuffer, arcana::weak_table<FrameBufferData*>& managerTable, uint16_t viewId, uint16_t width, uint16_t height, bool actAsBackBuffer = false, bool sizeViewToWindow = false)
            : m_managerTicket{managerTable.insert(this)}
            , m_clearState{std::make_unique<ClearState>()}
            , FrameBuffer{frameBuffer}
            , ViewClearState{viewId, *m_clearState}
            , Width{width}
            , Height{height}
            , SizeViewToWindow{sizeViewToWindow}
            , ActAsBackBuffer{actAsBackBuffer}
        {
            UseViewId(viewId);
        }

        FrameBufferData(bgfx::FrameBufferHandle frameBuffer, arcana::weak_table<FrameBufferData*>& managerTable, uint16_t viewId, ClearState& clearState, uint16_t width, uint16_t height, bool actAsBackBuffer = false, bool sizeViewToWindow = false)
            : m_managerTicket{managerTable.insert(this)}
            , m_clearState{}
            , FrameBuffer{frameBuffer}
            , ViewClearState{viewId, clearState}
            , Width{width}
            , Height{height}
            , SizeViewToWindow{sizeViewToWindow}
            , ActAsBackBuffer{actAsBackBuffer}
        {
            UseViewId(viewId);
        }

        FrameBufferData(FrameBufferData&) = delete;

        ~FrameBufferData()
        {
            bgfx::destroy(FrameBuffer);
        }

        void UseViewId(uint16_t viewId)
        {
            assert(viewId < bgfx::getCaps()->limits.maxViews);
            // Only update if we need to.
            if (IsViewIdDirty || viewId != ViewId)
            {
                ViewId = viewId;
                bgfx::setViewFrameBuffer(ViewId, FrameBuffer);
                SetViewPort(0, 0, 1, 1); // Default to full viewport
                ViewClearState.UpdateViewId(ViewId);
                IsViewIdDirty = false;
            }
        }

        void SetViewPort(const float x, const float y, const float width, const float height)
        {
            const auto viewRectWidth = SizeViewToWindow ? bgfx::getStats()->width : Width;
            const auto viewRectHeight = SizeViewToWindow ? bgfx::getStats()->height : Height;
            bgfx::setViewRect(ViewId,
                static_cast<uint16_t>(x * viewRectWidth),
                static_cast<uint16_t>(y * viewRectHeight),
                static_cast<uint16_t>(width * viewRectWidth),
                static_cast<uint16_t>(height * viewRectHeight));
        }

        bgfx::FrameBufferHandle FrameBuffer{bgfx::kInvalidHandle};
        bgfx::ViewId ViewId{uint16_t(~0)};
        bool IsViewIdDirty{true};
        Babylon::ViewClearState ViewClearState;
        uint16_t Width{};
        uint16_t Height{};
        bool SizeViewToWindow{false};
        // When a FrameBuffer acts as a back buffer, it means it will not be used as a texture in a shader.
        // For example as a post process. It will be used as-is in a swapchain or for direct rendering (XR)
        // When this flag is true, projection matrix will not be flipped for API that would normaly need it.
        // Namely Direct3D and Metal.
        bool ActAsBackBuffer{false};

        // This is a hack to keep track of whether this frame buffer has been passed back to Babylon.js
        // and if its deletion should be owned by Javascript and tied to the lifetime of a texture or whether
        // only BabylonNative knows about its existence, and should own deletion. Blame Gary.
        bool OwnedByJS{true};
    };

    struct FrameBufferManager final
    {
        FrameBufferManager()
        {
            Bind(m_defaultBackBuffer = new FrameBufferData(BGFX_INVALID_HANDLE, m_activeFrameBuffers, GetNewViewId(), 0, 0, true, true));
        }

        FrameBufferData* CreateNew(bgfx::FrameBufferHandle frameBufferHandle, uint16_t width, uint16_t height)
        {
            return new FrameBufferData(frameBufferHandle, m_activeFrameBuffers, GetNewViewId(), width, height);
        }

        FrameBufferData* CreateNew(bgfx::FrameBufferHandle frameBufferHandle, ClearState& clearState, uint16_t width, uint16_t height, bool actAsBackBuffer)
        {
            return new FrameBufferData(frameBufferHandle, m_activeFrameBuffers, GetNewViewId(), clearState, width, height, actAsBackBuffer);
        }

        void Bind(FrameBufferData* data)
        {
            m_boundFrameBuffer = data;

            const auto fbViewId = m_boundFrameBuffer->IsViewIdDirty ? GetNewViewId() : m_boundFrameBuffer->ViewId;
            m_boundFrameBuffer->UseViewId(fbViewId);

            // bgfx::setTexture()? Why?
            // TODO: View order?
            m_renderingToTarget = !m_boundFrameBuffer->ActAsBackBuffer;
        }

        FrameBufferData& GetBound()
        {
            // Babylon.js minimizes how frequently Bind() is called, causing there
            // to be some scenarios where the currently bound frame buffer can retain
            // an invalid view id when another FrameBuffer is created but not bound.
            // To prevent this, make sure the bound frameBuffer has a clean ViewId.
            if (m_boundFrameBuffer->IsViewIdDirty)
            {
                Bind(m_boundFrameBuffer);
            }
            return *m_boundFrameBuffer;
        }

        void Unbind(FrameBufferData* data)
        {
            (void)data;
            if (m_boundFrameBuffer != m_defaultBackBuffer)
            {
                assert(m_boundFrameBuffer == data);
                Bind(m_defaultBackBuffer);
            }
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
            m_activeFrameBuffers.apply_to_all([](auto frameBufferData) {
                frameBufferData->IsViewIdDirty = true;
            });
        }

        bool IsRenderingToTarget() const
        {
            return m_renderingToTarget;
        }

    private:
        FrameBufferData* m_boundFrameBuffer{nullptr};
        FrameBufferData* m_defaultBackBuffer{nullptr};
        arcana::weak_table<FrameBufferData*> m_activeFrameBuffers{};
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
        Napi::Value HomogeneousDepth(const Napi::CallbackInfo& info);
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
        void GetFramebufferData(const Napi::CallbackInfo& info);
        Napi::Value GetRenderAPI(const Napi::CallbackInfo& info);
        Napi::Value GetHardwareScalingLevel(const Napi::CallbackInfo& info);
        void SetHardwareScalingLevel(const Napi::CallbackInfo& info);
        Napi::Value CreateImageBitmap(const Napi::CallbackInfo& info);
        Napi::Value ResizeImageBitmap(const Napi::CallbackInfo& info);

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
