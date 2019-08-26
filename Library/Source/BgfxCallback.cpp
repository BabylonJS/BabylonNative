#include "BgfxCallback.h"
#include "Console.h"
#include <bx/bx.h>
#include <bx/string.h>

namespace babylon
{
    void BgfxCallback::fatal(const char* filePath, uint16_t line, bgfx::Fatal::Enum code, const char* str)
    {
        Console::Error(str);
    }

    void BgfxCallback::traceVargs(const char* filePath, uint16_t line, const char* format, va_list argList)
    {
        #define TRACE_MESSAGE_MAX_LENGTH 8192
        char temp[TRACE_MESSAGE_MAX_LENGTH];
        char* out = temp;
        int32_t len = bx::vsnprintf(out, sizeof(temp), format, argList);
        if ( (int32_t)sizeof(temp) < len)
        {
            out = (char*)alloca(len+1);
            len = bx::vsnprintf(out, len, format, argList);
        }
        out[len] = '\0';
                
        Console::Log(out);
    }
    
    void BgfxCallback::profilerBegin(const char* name, uint32_t abgr, const char* filePath, uint16_t line) 
    {
    }

    void BgfxCallback::profilerBeginLiteral(const char* name, uint32_t abgr, const char* filePath, uint16_t line) 
    {
    }

    void BgfxCallback::profilerEnd() 
    {
    }
    
    uint32_t BgfxCallback::cacheReadSize(uint64_t id) 
    { 
        return 0; 
    }
    
    bool BgfxCallback::cacheRead(uint64_t id, void* data, uint32_t size) 
    { 
        return false; 
    }
    
    void BgfxCallback::cacheWrite(uint64_t id, const void* data, uint32_t size) 
    {
    }

    void BgfxCallback::screenShot(const char* filePath, uint32_t width, uint32_t height, uint32_t pitch, const void* data, uint32_t size, bool yflip)
    {
    }

    void BgfxCallback::captureBegin(uint32_t width, uint32_t height, uint32_t pitch, bgfx::TextureFormat::Enum format, bool yflip)
    {
    }

    void BgfxCallback::captureEnd()
    {
    }

    void BgfxCallback::captureFrame(const void* _data, uint32_t _size)
    {
    }
}