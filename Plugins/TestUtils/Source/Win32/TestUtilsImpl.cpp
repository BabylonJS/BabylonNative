#include "../TestUtilsImplData.h"
#include <filesystem>
#include <Windowsx.h>
#include <arcana/threading/task.h>
#include <arcana/threading/task_schedulers.h>

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
        PostMessageW(m_implData->m_window, WM_DESTROY, 0, 0);
    }

    void TestUtils::UpdateSize(const Napi::CallbackInfo& info)
    {
        const int32_t width = info[0].As<Napi::Number>().Int32Value();
        const int32_t height = info[1].As<Napi::Number>().Int32Value();

        auto hWnd = m_implData->m_window;
        RECT rc{0, 0, width, height};
        AdjustWindowRectEx(&rc, GetWindowStyle(hWnd), GetMenu(hWnd) != NULL, GetWindowExStyle(hWnd));
        SetWindowPos(hWnd, NULL, 0, 0, rc.right - rc.left, rc.bottom - rc.top, SWP_NOMOVE | SWP_NOZORDER);
    }

    void TestUtils::SetTitle(const Napi::CallbackInfo& info)
    {
        // SetWindowText sends a window message synchronously and cannot be called the JS thread with the way the code
        // is currently set up. If the main thread is calling FinishRenderingCurrentFrame, this will hang forever as
        // the main message loop is not pumping. Once we fix the rendering code to never block the main thread, this
        // threadpool scheduling will no longer be necessary.
        arcana::threadpool_scheduler([hWnd = m_implData->m_window, title = info[0].As<Napi::String>().Utf8Value()] {
            SetWindowTextA(hWnd, title.c_str());
        });
    }

    Napi::Value TestUtils::GetOutputDirectory(const Napi::CallbackInfo& info)
    {
        auto path = GetModulePath().parent_path().generic_string();
        return Napi::Value::From(info.Env(), path);
    }

    void TestUtils::PostProcessFrameBufferData(std::vector<uint8_t>& /*data*/)
    {
    }
}

namespace Babylon::Plugins::TestUtils
{
    void BABYLON_API Initialize(Napi::Env env, Graphics::WindowT window)
    {
        auto implData{std::make_shared<Internal::TestUtils::ImplData>(window)};
        Internal::TestUtils::CreateInstance(env, implData);
    }
}