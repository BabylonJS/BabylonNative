#include "AppRuntime.h"

#include <Objbase.h>
#include <Windows.h>

#include <gsl/gsl>
#include <cassert>
#include <exception>
#include <sstream>

namespace Babylon
{
    namespace
    {
        constexpr size_t FILENAME_BUFFER_SIZE = 1024;
    }

    void AppRuntime::RunPlatformTier()
    {
        HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
        assert(SUCCEEDED(hr));
        _CRT_UNUSED(hr);
        auto coInitScopeGuard = gsl::finally([] { CoUninitialize(); });

        char filename[FILENAME_BUFFER_SIZE];
        auto result = GetModuleFileNameA(nullptr, filename, static_cast<DWORD>(std::size(filename)));
        assert(result != 0);
        (void)result;
        RunEnvironmentTier(filename);
    }

    void AppRuntime::DefaultUnhandledExceptionHandler(std::exception_ptr ptr)
    {
        try
        {
            std::rethrow_exception(ptr);
        }
        catch (const Napi::Error& error)
        {
            std::stringstream ss{};
            ss << "Uncaught Error: " << error.Message() << std::endl;
            OutputDebugStringA(ss.str().data());
        }
    }

    void AppRuntime::Execute(std::function<void()> callback)
    {
        callback();
    }
}
