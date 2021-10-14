#include <Babylon/Plugins/NativeEngine.h>
#include "NativeDataStream.h"
#include "NativeEngine.h"

namespace Babylon::Plugins::NativeEngine
{
    void Initialize(Napi::Env env)
    {
        Babylon::NativeDataStream::Initialize(env);
        Babylon::NativeEngine::Initialize(env);
    }
}
