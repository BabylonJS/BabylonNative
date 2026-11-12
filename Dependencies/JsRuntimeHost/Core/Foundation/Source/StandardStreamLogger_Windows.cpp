#include "StandardStreamLoggerPlatform.h"

#include <cerrno>
#include <cstdint>
#include <string>

#include <Windows.h>
#include <fcntl.h>
#include <io.h>
#include <share.h>

namespace
{
    void IgnoreInvalidParameter(
        const wchar_t*,
        const wchar_t*,
        const wchar_t*,
        unsigned int,
        uintptr_t)
    {
    }

    // Win32/UWP need to keep GetStdHandle/SetStdHandle in sync with CRT fds.
    struct ChannelPlatformState
    {
        DWORD StandardHandle{};
        HANDLE OriginalHandle{INVALID_HANDLE_VALUE};
        bool OriginalHandleUsesTarget{};
    };

    int OsDuplicate(int fd)
    {
        const auto previousHandler = ::_set_thread_local_invalid_parameter_handler(IgnoreInvalidParameter);
        const int duplicated = ::_dup(fd);
        (void)::_set_thread_local_invalid_parameter_handler(previousHandler);
        return duplicated;
    }

    int OsDuplicateTo(int source, int target)
    {
        return ::_dup2(source, target);
    }

    int OsClose(int fd)
    {
        return ::_close(fd);
    }

    int64_t OsRead(int fd, void* data, size_t size)
    {
        return ::_read(fd, data, static_cast<unsigned int>(size));
    }

    int64_t OsWrite(int fd, const void* data, size_t size)
    {
        return ::_write(fd, data, static_cast<unsigned int>(size));
    }

    int OsCreatePipe(int fds[2])
    {
        // UWP's CRT does not expose _pipe. CreatePipe + _open_osfhandle works on
        // desktop Win32 and UWP, and keeps the ends non-inheritable.
        SECURITY_ATTRIBUTES attributes{};
        attributes.nLength = sizeof(attributes);
        attributes.bInheritHandle = FALSE;

        HANDLE readHandle{INVALID_HANDLE_VALUE};
        HANDLE writeHandle{INVALID_HANDLE_VALUE};
        if (!::CreatePipe(&readHandle, &writeHandle, &attributes, 4096))
        {
            return -1;
        }

        fds[0] = ::_open_osfhandle(reinterpret_cast<intptr_t>(readHandle), _O_BINARY);
        if (fds[0] < 0)
        {
            (void)::CloseHandle(readHandle);
            (void)::CloseHandle(writeHandle);
            return -1;
        }

        fds[1] = ::_open_osfhandle(reinterpret_cast<intptr_t>(writeHandle), _O_BINARY);
        if (fds[1] < 0)
        {
            (void)::_close(fds[0]);
            (void)::CloseHandle(writeHandle);
            return -1;
        }

        return 0;
    }

    bool OsOccupyTarget(int target)
    {
        // Prefer the secure CRT form; UWP treats the deprecated _open as an error.
        int nullFd{-1};
        if (::_sopen_s(&nullFd, "NUL", _O_WRONLY | _O_BINARY, _SH_DENYNO, 0) != 0)
        {
            return false;
        }
        if (nullFd == target)
        {
            return true;
        }

        const bool duplicated = OsDuplicateTo(nullFd, target) == 0;
        (void)OsClose(nullFd);
        return duplicated;
    }

    void OsWritePlatform(bool /*isError*/, const std::string& line)
    {
        std::string output{line};
        output.push_back('\n');
        ::OutputDebugStringA(output.c_str());
    }

    intptr_t GetOsHandle(int fd)
    {
        const auto previousHandler = ::_set_thread_local_invalid_parameter_handler(IgnoreInvalidParameter);
        const intptr_t handle = ::_get_osfhandle(fd);
        (void)::_set_thread_local_invalid_parameter_handler(previousHandler);
        return handle;
    }

    bool OsOnStartChannel(ChannelPlatformState& state, int target, bool isError)
    {
        state.StandardHandle = isError ? STD_ERROR_HANDLE : STD_OUTPUT_HANDLE;
        state.OriginalHandle = ::GetStdHandle(state.StandardHandle);
        const intptr_t targetHandle = GetOsHandle(target);
        state.OriginalHandleUsesTarget =
            targetHandle != -1 &&
            state.OriginalHandle != nullptr &&
            state.OriginalHandle != INVALID_HANDLE_VALUE &&
            state.OriginalHandle == reinterpret_cast<HANDLE>(targetHandle);
        return true;
    }

    bool OsOnRedirected(ChannelPlatformState& state, int target)
    {
        const intptr_t pipeHandle = GetOsHandle(target);
        if (pipeHandle == -1)
        {
            return false;
        }
        return ::SetStdHandle(state.StandardHandle, reinterpret_cast<HANDLE>(pipeHandle)) != FALSE;
    }

    bool OsOnRestore(ChannelPlatformState& state, int target)
    {
        HANDLE handle = state.OriginalHandle;
        if (state.OriginalHandleUsesTarget)
        {
            const intptr_t restoredHandle = GetOsHandle(target);
            if (restoredHandle == -1)
            {
                return false;
            }
            handle = reinterpret_cast<HANDLE>(restoredHandle);
        }
        return ::SetStdHandle(state.StandardHandle, handle) != FALSE;
    }
}

#include "StandardStreamLogger_Shared.inl"