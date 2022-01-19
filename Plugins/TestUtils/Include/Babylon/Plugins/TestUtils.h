#pragma once

#include <napi/env.h>
#include <Babylon/Graphics/Device.h>

namespace Babylon::Plugins::TestUtils
{
    extern int errorCode;
    void Initialize(Napi::Env env, Graphics::WindowType nativeWindowPtr);
}
