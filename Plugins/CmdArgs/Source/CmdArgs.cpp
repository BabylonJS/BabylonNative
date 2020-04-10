#include <Babylon/JsRuntime.h>

namespace Babylon
{

    class CmdArgs final : public Napi::ObjectWrap<CmdArgs>
    {
    public:
        static inline constexpr char* JS_INSTANCE_NAME{ "CmdArgs" };

        static void Initialize(Napi::Env env, int argc, char** argv)
        {
            Napi::HandleScope scope{ env };
            Napi::Object cmdArgs = Napi::Array::New(env, argc);
            for (int index = 0; index < argc; index++)
            {
                cmdArgs[index] = Napi::String::New(env, argv[index]);
            }
            env.Global().Set(JS_INSTANCE_NAME, cmdArgs);
        }

        CmdArgs::CmdArgs(const Napi::CallbackInfo& info)
            : Napi::ObjectWrap<CmdArgs>{info}
        {
        }
    };

    void InitializeCmdArgs(Napi::Env env, int argc, char** argv)
    {
        CmdArgs::Initialize(env, argc, argv);
    }
}