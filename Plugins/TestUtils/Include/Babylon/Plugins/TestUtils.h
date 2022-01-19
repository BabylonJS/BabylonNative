#pragma once

#include <napi/env.h>
#include <Babylon/Graphics/Device.h>

namespace Babylon::Plugins::TestUtils
{
    extern int errorCode;
<<<<<<< HEAD
    void Initialize(Napi::Env env, Graphics::WindowType nativeWindowPtr);
=======
    void Initialize(Napi::Env env, WindowType window);
>>>>>>> master
}
