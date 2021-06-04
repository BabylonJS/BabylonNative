#include "BgfxCallback.h"
#include <bx/bx.h>
#include <bx/string.h>
#include <bx/platform.h>
#include <bx/debug.h>
#include <stdarg.h>
#include <bgfx/bgfx.h>
#include <assert.h>

namespace Babylon
{
    BgfxCallback::BgfxCallback(std::function<void(const CaptureData&)> captureCallback)
        : m_captureCallback{std::move(captureCallback)}
    {
    }

    void BgfxCallback::AddScreenShotCallback(std::function<void(std::vector<uint8_t>)> callback)
    {
        m_screenShotCallbacks.emplace(std::move(callback));
    }

    void BgfxCallback::SetDiagnosticOutput(std::function<void(const char* output)> outputFunction)
    {
        m_outputFunction = std::move(outputFunction);
    }

    void BgfxCallback::fatal(const char* filePath, uint16_t line, bgfx::Fatal::Enum code, const char* str)
    {
        if (bgfx::Fatal::DebugCheck == code)
        {
            bx::debugBreak();
        }
        else
        {
            trace(filePath, line, "BGFX 0x%08x: %s\n", code, str);
            BX_UNUSED(code, str);
            abort();
        }
    }

    void BgfxCallback::traceVargs(const char* filePath, uint16_t line, const char* format, va_list argList)
    {
        char temp[2048];
        char* out = temp;
        va_list argListCopy;
        va_copy(argListCopy, argList);
        int32_t len = bx::snprintf(out, sizeof(temp), "%s (%d): ", filePath, line);
        int32_t total = len + bx::vsnprintf(out + len, sizeof(temp) - len, format, argListCopy);
        va_end(argListCopy);
        if ((int32_t)sizeof(temp) < total)
        {
            out = (char*)alloca(total + 1);
            bx::memCopy(out, temp, len);
            bx::vsnprintf(out + len, total - len, format, argList);
        }
        out[total] = '\0';
        bx::debugOutput(out);
        if (m_outputFunction)
        {
            m_outputFunction(out);
        }
    }

    void BgfxCallback::profilerBegin(const char* /*name*/, uint32_t /*abgr*/, const char* /*filePath*/, uint16_t /*line*/)
    {
    }

    void BgfxCallback::profilerBeginLiteral(const char* /*name*/, uint32_t /*abgr*/, const char* /*filePath*/, uint16_t /*line*/)
    {
    }

    void BgfxCallback::profilerEnd()
    {
    }

    uint32_t BgfxCallback::cacheReadSize(uint64_t /*id*/)
    {
        return 0;
    }

    bool BgfxCallback::cacheRead(uint64_t /*id*/, void* /*data*/, uint32_t /*size*/)
    {
        return false;
    }

    void BgfxCallback::cacheWrite(uint64_t /*id*/, const void* /*data*/, uint32_t /*size*/)
    {
    }

    void BgfxCallback::screenShot(const char* /*filePath*/, uint32_t width, uint32_t height, uint32_t pitch, const void* data, uint32_t /*size*/, bool yflip)
    {
        assert(!m_screenShotCallbacks.empty()); // addScreenShotCallback not called before doing the screenshot call on bgfx

        std::vector<uint8_t> array(width * height * 4); // do not use pitch to define output size because it's padded
        uint8_t* bitmap{array.data()};

        for (uint32_t py = 0; py < height; py++)
        {
            const uint8_t* ptr = static_cast<const uint8_t*>(data) + (yflip ? (height - py - 1) : py) * pitch;
            for (uint32_t px = 0; px < width; px++)
            {
                // bgfx screenshot is BGRA
                *bitmap++ = ptr[px * 4 + 2];
                *bitmap++ = ptr[px * 4 + 1];
                *bitmap++ = ptr[px * 4 + 0];
                *bitmap++ = ptr[px * 4 + 3];
            }
        }

        m_screenShotCallbacks.front()(std::move(array));
        m_screenShotCallbacks.pop();
    }

    void BgfxCallback::captureBegin(uint32_t width, uint32_t height, uint32_t pitch, bgfx::TextureFormat::Enum format, bool yflip)
    {
        m_captureData.Width = width;
        m_captureData.Height = height;
        m_captureData.Pitch = pitch;
        m_captureData.Format = format;
        m_captureData.YFlip = yflip;
    }

    void BgfxCallback::captureEnd()
    {
    }

    void BgfxCallback::captureFrame(const void* data, uint32_t size)
    {
        m_captureData.Data = data;
        m_captureData.DataSize = size;

        m_captureCallback(m_captureData);
    }

    void BgfxCallback::trace(const char* _filePath, uint16_t _line, const char* _format, ...)
    {
        va_list argList;
        va_start(argList, _format);

        traceVargs(_filePath, _line, _format, argList);

        va_end(argList);
    }
}
