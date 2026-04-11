#pragma once

#include "BgfxCallback.h"
#include <bx/allocator.h>
#include "continuation_scheduler.h"

#include <arcana/threading/task.h>

#include <napi/env.h>

#include <bgfx/bgfx.h>
#include <bgfx/platform.h>

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

    // FrameCompletionScope is an RAII guard that keeps a frame "open" on the JS thread.
    // While any scope is alive, FinishRenderingCurrentFrame() on the main thread will
    // block — it cannot call bgfx::frame() until all scopes are destroyed.
    //
    // This prevents a race where the main thread submits a bgfx frame while the JS
    // thread is still recording encoder commands (which would cause bgfx deadlocks
    // or lost draw calls).
    //
    // Three usage patterns:
    //   1. RAF scheduling: scope acquired on main thread during StartRenderingCurrentFrame,
    //      transferred to JS thread, released after RAF callbacks complete + one extra
    //      dispatch cycle (to cover GC-triggered resource destruction).
    //   2. NativeEngine::GetEncoder(): scope acquired lazily when JS code uses the encoder
    //      outside RAF (e.g., async texture loads, LOD switches). Released on next dispatch.
    //   3. Canvas::Flush(): stack-scoped for the duration of nanovg rendering.
    //
    // Construction blocks if m_frameBlocked is true (frame submission in progress).
    // Destruction decrements counter and wakes main thread via condition variable.
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

        // Acquire a scope that prevents FinishRenderingCurrentFrame from completing.
        // The scope must be held while JS frame callbacks are running.
        FrameCompletionScope AcquireFrameCompletionScope();

        // Active encoder for the current frame. Managed by DeviceImpl in
        // StartRenderingCurrentFrame/FinishRenderingCurrentFrame.
        // Used by NativeEngine, Canvas, and NativeXr.
        void SetActiveEncoder(bgfx::Encoder* encoder);
        bgfx::Encoder* GetActiveEncoder();

        void RequestScreenShot(std::function<void(std::vector<uint8_t>)> callback);
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
