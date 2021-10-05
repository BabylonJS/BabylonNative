#include "TestUtils.h"

namespace Babylon::Plugins::Internal
{
    void TestUtils::Exit(const Napi::CallbackInfo& /*info*/)
    {
    }

    void TestUtils::UpdateSize(const Napi::CallbackInfo& /*info*/)
    {
    }

    void TestUtils::SetTitle(const Napi::CallbackInfo& /*info*/)
    {
    }

    Napi::Value TestUtils::GetOutputDirectory(const Napi::CallbackInfo& info)
    {
        auto path = "/data/data/com.android.babylonnative.validationtests/cache";
        return Napi::Value::From(info.Env(), path);
    }
}