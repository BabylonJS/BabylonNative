#include "TestUtilsImplData.h"
#include <Windows.h>
#include <winrt/Windows.ApplicationModel.h>
#include <winrt/windows.ui.core.h>


using namespace winrt::Windows;

namespace Babylon::Plugins::Internal
{
    void TestUtils::Exit(const Napi::CallbackInfo& info)
    {
        const int32_t exitCode = info[0].As<Napi::Number>().Int32Value();
        // ceguille: I didn't find a better way to do it for UWP
        exit(exitCode);
    }

    void TestUtils::UpdateSize(const Napi::CallbackInfo& /*info*/)
    {
    }

    void TestUtils::SetTitle(const Napi::CallbackInfo& /*info*/)
    {
    }

    Napi::Value TestUtils::GetOutputDirectory(const Napi::CallbackInfo& info)
    {
        using namespace Windows::Storage;
        StorageFolder^ localFolder = ApplicationData::Current->LocalFolder;
        std::wstring wpath = localFolder->Path->Data();
        std::string path{winrt::to_string(wpath)};
        return Napi::Value::From(info.Env(), path);
    }
}

namespace Babylon::Plugins::TestUtils
{
    void Initialize(Napi::Env env, WindowType nativeWindowPtr)
    {
        auto implData{std::make_shared<Internal::TestUtils::ImplData>(nativeWindowPtr)};
        Internal::TestUtils::CreateInstance(env, implData);
    }
}