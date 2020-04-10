#include <Babylon/JsRuntime.h>

namespace Babylon
{

    class CmdArgs final : public Napi::ObjectWrap<CmdArgs>
    {
    public:
        static inline constexpr char* JS_INSTANCE_NAME{ "CmdArgs" };

        static void Initialize(Napi::Env env, const std::vector<std::string>& args)
        {
            Napi::HandleScope scope{ env };
            Napi::Object cmdArgs = Napi::Array::New(env, args.size());
            for (int index = 0; index < args.size(); index++)
            {
                cmdArgs[index] = Napi::String::New(env, args[index]);
            }
            env.Global().Set(JS_INSTANCE_NAME, cmdArgs);
        }

        CmdArgs::CmdArgs(const Napi::CallbackInfo& info)
            : Napi::ObjectWrap<CmdArgs>{info}
        {
        }
    };

    void InitializeCmdArgs(Napi::Env env, const std::vector<std::string>& args)
    {
        CmdArgs::Initialize(env, args);
    }
}