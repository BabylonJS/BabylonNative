#include "TestUtils.h"
#include <winrt/windows.ui.core.h>

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