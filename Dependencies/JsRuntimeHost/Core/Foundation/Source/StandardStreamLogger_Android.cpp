#include "StandardStreamLoggerPlatform.h"

#include <android/log.h>
#include <cerrno>
#include <cstdint>
#include <string>

#include <fcntl.h>
#include <unistd.h>

namespace
{
// POSIX fd helpers (dup/pipe/CLOEXEC/devnull); sink is OsWritePlatform below.
#include "StandardStreamLogger_PosixOps.inl"

    void OsWritePlatform(bool isError, const std::string& line)
    {
        const int priority = isError ? ANDROID_LOG_ERROR : ANDROID_LOG_INFO;
        __android_log_write(priority, "JsRuntimeHost", line.c_str());
    }
}

#include "StandardStreamLogger_Shared.inl"