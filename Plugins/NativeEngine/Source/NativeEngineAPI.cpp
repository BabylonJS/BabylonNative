#include <Babylon/Plugins/NativeEngine.h>
#include "NativeEngine.h"

namespace Babylon::Plugins::NativeEngine
{
    void Initialize(Napi::Env env, bool renderAutomatically)
    {
        Babylon::NativeEngine::Initialize(env, renderAutomatically);
    }
}
