#include "../TestUtilsImplData.h"
#include <winrt/windows.storage.h>

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
        auto localFolder = winrt::Windows::Storage::ApplicationData::Current().LocalFolder();
        return Napi::Value::From(info.Env(), winrt::to_string(localFolder.Path().data()));
    }
}

namespace Babylon::Plugins::TestUtils
{
    void Initialize(Napi::Env env, Graphics::WindowT window)
    {
        auto implData{std::make_shared<Internal::TestUtils::ImplData>(window)};
        Internal::TestUtils::CreateInstance(env, implData);
    }
}