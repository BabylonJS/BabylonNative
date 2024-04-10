#pragma once

#include <napi/env.h>
#include <Babylon/Graphics/Device.h>

namespace Babylon::Plugins::TestUtils
{
    extern int errorCode;
    void BABYLON_API Initialize(Napi::Env env, Graphics::WindowT window);
}
