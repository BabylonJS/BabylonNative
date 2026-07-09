#pragma once

#include "BgfxCallback.h"
#include <bx/allocator.h>
#include "continuation_scheduler.h"

#include <arcana/threading/task.h>

#include <napi/env.h>

#include <bgfx/bgfx.h>

#include <mutex>
#include <unordered_map>

namespace Babylon::Graphics
{
    class DeviceContext;
    class DeviceImpl;

    struct TextureInfo final
    {
    public:
        uint16_t Width{};
        uint16_t Height{};
        bool HasMips{};
        uint16_t NumLayers{};
        bgfx::TextureFormat::Enum Format{};
    };

    // FrameCompletionScope is an RAII guard that prevents the render thread from
    // closing a bgfx frame while JS-thread work is still in flight. While any
    // scope is alive, FinishRenderingCurrentFrame() blocks before bgfx::frame()
    // — so all encoder commands recorded while a scope was held land in the
    // same bgfx frame, never split across two.
    //
    // Acquisition blocks if the gate is closed (m_frameBlocked == true), so a
    // JS thread that picks up work between frames waits for the next Start
    // before proceeding.
    //
    // Two scoping patterns are used:
    //   1. JS-frame scoped: capture the scope into an m_runtime.Dispatch lambda
    //      so it survives until the JS-thread queue services the next
    //      continuation. Use this when subsequent work in the same JS task may
    //      also touch bgfx (chained submitCommands, RAF callbacks, scene
    //      cleanup, etc.). NativeEngine::SubmitCommands and
    //      ScheduleRequestAnimationFrameCallbacks both do this.
    //   2. Block scoped: hold the scope on the stack across a single self-
    //      contained bgfx phase. Used for one-shot operations like
    //      Canvas::Flush() called outside an active frame, and
    //      ReadTextureAsync.
    class FrameCompletionScope final
    {
    public:
        FrameCompletionScope(const FrameCompletionScope&) = delete;
        FrameCompletionScope& operator=(const FrameCompletionScope&) = delete;
        FrameCompletionScope& operator=(FrameCompletionScope&&) = delete;

        FrameCompletionScope(FrameCompletionScope&&) noexcept;
        ~FrameCompletionScope();

    private:
        friend class DeviceContext;
        FrameCompletionScope(DeviceImpl&);
        DeviceImpl* m_impl;
    };

    class DeviceContext
    {
    public:
        // TODO: Move this to private a soon as the GraphicsImpl no longer owns and needs to create a DeviceContext
        DeviceContext(DeviceImpl&);

        // TODO: Deprecated: this is included only as a stopgap during feature integration and will not function long-term
        static DeviceContext& GetFromJavaScript(Napi::Env);

        static Napi::Value Create(Napi::Env, DeviceImpl&);
        static DeviceContext& GetFromJavaScript(Napi::Value);

        continuation_scheduler<>& BeforeRenderScheduler();
        continuation_scheduler<>& AfterRenderScheduler();

        // Scheduler that fires when StartRenderingCurrentFrame ticks the frame start dispatcher.
        // Use this to schedule work (e.g., requestAnimationFrame callbacks) that should run each frame.
        continuation_scheduler<>& FrameStartScheduler();

        // Acquire a scope that prevents FinishRenderingCurrentFrame from
        // completing until the scope is destroyed. JS-thread callers that
        // need coverage across the whole current JS task should capture the
        // scope into an m_runtime.Dispatch lambda; callers needing only a
        // single phase can hold it stack-scoped. See the class comment on
        // FrameCompletionScope above for the two patterns.
        FrameCompletionScope AcquireFrameCompletionScope();

        // Active encoder for the current frame. Managed by DeviceImpl in
        // StartRenderingCurrentFrame/FinishRenderingCurrentFrame.
        // Used by NativeEngine, Canvas, and NativeXr.
        void SetActiveEncoder(bgfx::Encoder* encoder);
        bgfx::Encoder* GetActiveEncoder();

        void RequestScreenShot(std::function<void(std::vector<uint8_t>)> callback);
        void RequestCaptureNextFrame();
        void SetRenderResetCallback(std::function<void()> callback);

        arcana::task<void, std::exception_ptr> ReadTextureAsync(bgfx::TextureHandle handle, gsl::span<uint8_t> data, uint8_t mipLevel = 0);

        float GetHardwareScalingLevel();
        void SetHardwareScalingLevel(float level);

        size_t GetWidth() const;
        size_t GetHeight() const;
        float GetDevicePixelRatio();

        //Note: This is an index that changes when bgfx gets reset. It should be used to validate that resource handles created using bgfx remain valid on destruction.
        uintptr_t GetDeviceId() const;

        using CaptureCallbackTicketT = arcana::ticketed_collection<std::function<void(const BgfxCallback::CaptureData&)>>::ticket;
        CaptureCallbackTicketT AddCaptureCallback(std::function<void(const BgfxCallback::CaptureData&)> callback);

        bgfx::ViewId AcquireNewViewId();
        bgfx::ViewId PeekNextViewId() const;

        // TODO: find a different way to get the texture info for frame capture
        void AddTexture(bgfx::TextureHandle handle, uint16_t width, uint16_t height, bool hasMips, uint16_t numLayers, bgfx::TextureFormat::Enum format);
        void RemoveTexture(bgfx::TextureHandle handle);
        TextureInfo GetTextureInfo(bgfx::TextureHandle handle);
        static bx::AllocatorI& GetDefaultAllocator() { return m_allocator; }

    private:
        DeviceImpl& m_graphicsImpl;

        std::unordered_map<uint16_t, TextureInfo> m_textureHandleToInfo{};
        std::mutex m_textureHandleToInfoMutex{};

        static inline bx::DefaultAllocator m_allocator{};
    };
}
