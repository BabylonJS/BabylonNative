#include "TestUtils.h"

namespace Babylon::Plugins::Internal
{
    void TestUtils::Exit(const Napi::CallbackInfo& /*info*/)
    {
        // Not implemented for iOS
    }

    void TestUtils::UpdateSize(const Napi::CallbackInfo& /*info*/)
    {
        // Not implemented for iOS
    }

    void TestUtils::SetTitle(const Napi::CallbackInfo& /*info*/)
    {
        // Not implemented for iOS
    }

    Napi::Value TestUtils::GetOutputDirectory(const Napi::CallbackInfo& /*info*/)
    {
        // Not implemented for iOS
        return {};
    }
}
