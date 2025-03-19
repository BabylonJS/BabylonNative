#pragma once

#include "BgfxCallback.h"
#include <bx/allocator.h>
#include "continuation_scheduler.h"
#include "SafeTimespanGuarantor.h"

#include <napi/env.h>

#include <bgfx/bgfx.h>
#include <bgfx/platform.h>

#include <mutex>
#include <unordered_map>

namespace Babylon::Graphics
{
    class Update;
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

    class UpdateToken final
    {
    public:
        UpdateToken(const UpdateToken& other) = delete;
        UpdateToken& operator=(const UpdateToken& other) = delete;

        UpdateToken(UpdateToken&&) noexcept = default;

        // The move assignment of `SafeTimespanGuarantor::SafetyGuarantee` is marked as delete.
        // See https://github.com/Microsoft/GSL/issues/705.
        //UpdateToken& operator=(UpdateToken&& other) = delete;

        bgfx::Encoder* GetEncoder();

    private:
        friend class Update;

        UpdateToken(DeviceContext&, SafeTimespanGuarantor&);

        DeviceContext& m_context;
        SafeTimespanGuarantor::SafetyGuarantee m_guarantee;
    };

    class Update
    {
    public:
        continuation_scheduler<>& Scheduler()
        {
            return m_safeTimespanGuarantor.OpenScheduler();
        }

        UpdateToken GetUpdateToken()
        {
            return {m_context, m_safeTimespanGuarantor};
        }

    private:
        friend class DeviceContext;

        Update(SafeTimespanGuarantor& safeTimespanGuarantor, DeviceContext& context)
            : m_safeTimespanGuarantor{safeTimespanGuarantor}
            , m_context{context}
        {
        }

        SafeTimespanGuarantor& m_safeTimespanGuarantor;
        DeviceContext& m_context;
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

        Update GetUpdate(const char* updateName);

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

        bgfx::ViewId AcquireNewViewId(bgfx::Encoder&);

        // TODO: find a different way to get the texture info for frame capture
        void AddTexture(bgfx::TextureHandle handle, uint16_t width, uint16_t height, bool hasMips, uint16_t numLayers, bgfx::TextureFormat::Enum format);
        void RemoveTexture(bgfx::TextureHandle handle);
        TextureInfo GetTextureInfo(bgfx::TextureHandle handle);
        static bx::AllocatorI& GetDefaultAllocator() { return m_allocator; }

        void Blit(bgfx::Encoder& encoder, bgfx::TextureHandle dst, uint16_t dstX, uint16_t dstY, bgfx::TextureHandle src, uint16_t srcX = 0, uint16_t srcY = 0, uint16_t width = UINT16_MAX, uint16_t height = UINT16_MAX);
    private:
        friend UpdateToken;

        DeviceImpl& m_graphicsImpl;

        std::unordered_map<uint16_t, TextureInfo> m_textureHandleToInfo{};
        std::mutex m_textureHandleToInfoMutex{};

        static inline bx::DefaultAllocator m_allocator{};
    };
}
