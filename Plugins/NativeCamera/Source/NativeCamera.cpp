#include "NativeCamera.h"

#include <Babylon/JsRuntime.h>
#include <GraphicsImpl.h>

#include <vector>

namespace Babylon::Plugins::Internal
{
    class NativeCamera : public Napi::ObjectWrap<NativeCamera>
    {

    public:
        static constexpr auto JS_CLASS_NAME = "NativeCamera";

        static void Initialize(Napi::Env env)
        {
            Napi::HandleScope scope{ env };

            Napi::Function func = NativeCamera::DefineClass(
                env,
                JS_CLASS_NAME,
                {
                });

            env.Global().Set(JS_CLASS_NAME, func);
        }

        NativeCamera(const Napi::CallbackInfo& info)
            : Napi::ObjectWrap<NativeCamera>{ info }
        {
        }

        ~NativeCamera()
        {
        }

    private:
    };
}

namespace Babylon::Plugins::NativeCamera
{
    void Initialize(Napi::Env env)
    {
        Babylon::Plugins::Internal::NativeCamera::Initialize(env);
    }
}
