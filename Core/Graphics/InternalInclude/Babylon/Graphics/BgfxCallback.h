#pragma once

#include <atomic>
#include <exception>
#include <functional>
#include <queue>
#include <vector>

#include <arcana/expected.h>
#include <bgfx/bgfx.h>

namespace Babylon::Graphics
{
    class BgfxCallback : public bgfx::CallbackI
    {
    public:
        struct CaptureData
        {
            uint32_t Width{};
            uint32_t Height{};
            uint32_t Pitch{};
            bgfx::TextureFormat::Enum Format{};
            bool YFlip{};
            const void* Data{};
            uint32_t DataSize{};
        };

        BgfxCallback(std::function<void(const CaptureData&)>);
        virtual ~BgfxCallback() = default;

        bool IsDeviceLost() const;
        void ClearDeviceLost();

        using ScreenShotResult = arcana::expected<std::vector<uint8_t>, std::exception_ptr>;
        using ScreenShotCallback = std::function<void(ScreenShotResult)>;

        void AddScreenShotCallback(ScreenShotCallback callback);
        bool HasPendingScreenShotCallbacks() const;
        void CancelScreenShots(std::exception_ptr error);
        void CaptureNextScreenShot();
        void CompleteScreenShot(const CaptureData& data);
        void SetDiagnosticOutput(std::function<void(const char* output)> outputFunction);
        void trace(const char* _filePath, uint16_t _line, const char* _format, ...);

    protected:
        void fatal(const char* filePath, uint16_t line, bgfx::Fatal::Enum code, const char* str) override;
        void traceVargs(const char* filePath, uint16_t line, const char* format, va_list argList) override;
        void profilerBegin(const char* name, uint32_t abgr, const char* filePath, uint16_t line) override;
        void profilerBeginLiteral(const char* name, uint32_t abgr, const char* filePath, uint16_t line) override;
        void profilerEnd() override;
        uint32_t cacheReadSize(uint64_t id) override;
        bool cacheRead(uint64_t id, void* data, uint32_t size) override;
        void cacheWrite(uint64_t id, const void* data, uint32_t size) override;
        void screenShot(const char* filePath, uint32_t width, uint32_t height, uint32_t pitch, bgfx::TextureFormat::Enum format, const void* data, uint32_t size, bool yflip) override;
        void captureBegin(uint32_t width, uint32_t height, uint32_t pitch, bgfx::TextureFormat::Enum format, bool yflip) override;
        void captureEnd() override;
        void captureFrame(const void* _data, uint32_t _size) override;

    private:
        std::atomic<bool> m_deviceLost{false};
        std::function<void(const char* output)> m_outputFunction;

        std::queue<ScreenShotCallback> m_screenShotCallbacks;
        bool m_captureScreenShot{};

        CaptureData m_captureData{};
        const std::function<void(const CaptureData&)> m_captureCallback{};
    };
}
