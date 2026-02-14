#include "TestUtils.h"
#include <winrt/Windows.ApplicationModel.Core.h>
#include <winrt/windows.storage.h>

namespace Babylon::Plugins::Internal
{
    void TestUtils::Exit(const Napi::CallbackInfo& info)
    {
        auto exitCode = info[0].As<Napi::Number>().Int32Value();
        if (exitCode != 0)
        {
            std::quick_exit(exitCode);
        }

        winrt::Windows::ApplicationModel::Core::CoreApplication::Exit();
    }

    void TestUtils::UpdateSize(const Napi::CallbackInfo& /*info*/)
    {
    }

    void TestUtils::SetTitle(const Napi::CallbackInfo& /*info*/)
    {
    }

    Napi::Value TestUtils::GetOutputDirectory(const Napi::CallbackInfo& info)
    {
        auto localFolder = winrt::Windows::Storage::ApplicationData::Current().LocalFolder();
        return Napi::Value::From(info.Env(), winrt::to_string(localFolder.Path().data()));
    }

    void TestUtils::PostProcessFrameBufferData(std::vector<uint8_t>& /*data*/)
    {
    }
}