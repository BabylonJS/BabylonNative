#pragma once

#include "BgfxCallback.h"
#include "continuation_scheduler.h"
#include "SafeTimespanGuarantor.h"

#include <napi/env.h>

#include <bgfx/bgfx.h>
#include <bgfx/platform.h>

#include <mutex>
#include <unordered_map>

namespace Babylon
{
    class Update;
    class GraphicsContext;
    class GraphicsImpl;

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
        UpdateToken(UpdateToken&&) = default;

        bgfx::Encoder* GetEncoder();

    private:
        friend class Update;

        UpdateToken(GraphicsContext&, SafeTimespanGuarantor&);

        GraphicsContext& m_context;
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
        friend class GraphicsContext;

        Update(SafeTimespanGuarantor& safeTimespanGuarantor, GraphicsContext& context)
            : m_safeTimespanGuarantor{safeTimespanGuarantor}
            , m_context{context}
        {
        }

        SafeTimespanGuarantor& m_safeTimespanGuarantor;
        GraphicsContext& m_context;
    };

    class GraphicsContext
    {
    public:
        // Deprecated: this is included only as a stopgap during feature integration and will not function long-term
        static GraphicsContext& GetFromJavaScript(Napi::Env);

        GraphicsContext(GraphicsImpl&);

        continuation_scheduler<>& BeforeRenderScheduler();
        continuation_scheduler<>& AfterRenderScheduler();

        Update GetUpdate(const char* updateName);

        void RequestScreenShot(std::function<void(std::vector<uint8_t>)> callback);

        arcana::task<void, std::exception_ptr> ReadTextureAsync(bgfx::TextureHandle handle, gsl::span<uint8_t> data);

        float GetHardwareScalingLevel();
        void SetHardwareScalingLevel(float level);

        size_t GetWidth() const;
        size_t GetHeight() const;
        float GetDevicePixelRatio();

        using CaptureCallbackTicketT = arcana::ticketed_collection<std::function<void(const BgfxCallback::CaptureData&)>>::ticket;
        CaptureCallbackTicketT AddCaptureCallback(std::function<void(const BgfxCallback::CaptureData&)> callback);

        bgfx::ViewId AcquireNewViewId(bgfx::Encoder&);

        void AddTexture(bgfx::TextureHandle handle, uint16_t width, uint16_t height, bool hasMips, uint16_t numLayers, bgfx::TextureFormat::Enum format);
        void RemoveTexture(bgfx::TextureHandle handle);
        TextureInfo GetTextureInfo(bgfx::TextureHandle handle);

    private:
        friend UpdateToken;

        GraphicsImpl& m_graphicsImpl;

        std::unordered_map<uint16_t, TextureInfo> m_textureHandleToInfo{};
        std::mutex m_textureHandleToInfoMutex{};
    };
}
