#include "TestUtils.h"
#include <filesystem>
#include <Windowsx.h>

namespace
{
    std::filesystem::path GetModulePath()
    {
        char buffer[1024];
        ::GetModuleFileNameA(nullptr, buffer, ARRAYSIZE(buffer));
        return std::filesystem::path{buffer}.parent_path();
    }
}

namespace Babylon::Plugins::TestUtils
{
    int errorCode{};
}

namespace Babylon::Plugins::Internal
{
    void TestUtils::Exit(const Napi::CallbackInfo& info)
    {
        const int32_t exitCode = info[0].As<Napi::Number>().Int32Value();
        Plugins::TestUtils::errorCode = exitCode;
        PostMessageW(_nativeWindowPtr, WM_DESTROY, 0, 0);
    }

    void TestUtils::UpdateSize(const Napi::CallbackInfo& info)
    {
        const int32_t width = info[0].As<Napi::Number>().Int32Value();
        const int32_t height = info[1].As<Napi::Number>().Int32Value();

        HWND hwnd = _nativeWindowPtr;
        RECT rc{ 0, 0, width, height };
        AdjustWindowRectEx(&rc, GetWindowStyle(hwnd), GetMenu(hwnd) != NULL, GetWindowExStyle(hwnd));
        SetWindowPos(hwnd, NULL, 0, 0, rc.right - rc.left, rc.bottom - rc.top, SWP_NOMOVE | SWP_NOZORDER);
    }

    void TestUtils::SetTitle(const Napi::CallbackInfo& info)
    {
        const auto title = info[0].As<Napi::String>().Utf8Value();
        SetWindowTextA(_nativeWindowPtr, title.c_str());
    }

    Napi::Value TestUtils::GetOutputDirectory(const Napi::CallbackInfo& info)
    {
        auto path = GetModulePath().parent_path().generic_string();
        return Napi::Value::From(info.Env(), path);
    }
}