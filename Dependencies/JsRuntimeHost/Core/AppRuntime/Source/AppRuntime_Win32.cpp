#include "AppRuntime.h"
#include <Objbase.h>
#include <Windows.h>
#include <gsl/gsl>
#include <cassert>
#include <sstream>

namespace Babylon
{
    void AppRuntime::DefaultUnhandledExceptionHandler(const Napi::Error& error)
    {
        std::ostringstream ss{};
        ss << "[Uncaught Error] " << error.Get("stack").As<Napi::String>().Utf8Value() << std::endl;
        OutputDebugStringA(ss.str().data());
    }

    void AppRuntime::RunPlatformTier()
    {
        HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
        assert(SUCCEEDED(hr));
        _CRT_UNUSED(hr);
        auto coInitScopeGuard = gsl::finally([] { CoUninitialize(); });

        char filename[1024];
        auto result = GetModuleFileNameA(nullptr, filename, static_cast<DWORD>(std::size(filename)));
        assert(result != 0);
        (void)result;
        RunEnvironmentTier(filename);
    }

    void AppRuntime::Execute(Dispatchable<void()> callback)
    {
        callback();
    }
}
