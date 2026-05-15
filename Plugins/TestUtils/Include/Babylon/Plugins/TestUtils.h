#pragma once

#include <napi/env.h>
#include <Babylon/Graphics/Device.h>

#include <functional>

namespace Babylon::Plugins::TestUtils
{
    void BABYLON_API Initialize(Napi::Env env, Graphics::WindowT window);

    // Optional hook invoked when JS calls TestUtils.exit(code). Called on
    // the JS thread before the platform's default termination. Pass an empty
    // std::function to clear.
    using ExitCallback = std::function<void(int /*exitCode*/)>;
    void BABYLON_API SetExitCallback(ExitCallback callback);
}

