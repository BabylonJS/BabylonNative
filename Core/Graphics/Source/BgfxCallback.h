#pragma once

#include <napi/napi.h>

#include <arcana/containers/ticketed_collection.h>

#include <bgfx/bgfx.h>
#include <bgfx/platform.h>

#include <mutex>
#include <queue>
#include <vector>

namespace Babylon
{
    struct BgfxCallback : public bgfx::CallbackI
    {
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

        void addScreenShotCallback(Napi::Function callback);

        void SetDiagnosticOutput(std::function<void(const char* output)> outputFunction);
    protected:
        void fatal(const char* filePath, uint16_t line, bgfx::Fatal::Enum code, const char* str) override;
        void traceVargs(const char* filePath, uint16_t line, const char* format, va_list argList) override;
        void profilerBegin(const char* name, uint32_t abgr, const char* filePath, uint16_t line) override;
        void profilerBeginLiteral(const char* name, uint32_t abgr, const char* filePath, uint16_t line) override;
        void profilerEnd() override;
        uint32_t cacheReadSize(uint64_t id) override;
        bool cacheRead(uint64_t id, void* data, uint32_t size) override;
        void cacheWrite(uint64_t id, const void* data, uint32_t size) override;
        void screenShot(const char* filePath, uint32_t width, uint32_t height, uint32_t pitch, const void* data, uint32_t size, bool yflip) override;
        void captureBegin(uint32_t width, uint32_t height, uint32_t pitch, bgfx::TextureFormat::Enum format, bool yflip) override;
        void captureEnd() override;
        void captureFrame(const void* _data, uint32_t _size) override;
        void trace(const char* _filePath, uint16_t _line, const char* _format, ...);

        std::function<void(const char* output)> m_outputFunction;
        
        std::mutex m_ssCallbackAccess;
        std::queue<Napi::FunctionReference> m_screenshotCallbacks;
        
        CaptureData m_captureData{};
        const std::function<void(const CaptureData&)> m_captureCallback{};
    };
}