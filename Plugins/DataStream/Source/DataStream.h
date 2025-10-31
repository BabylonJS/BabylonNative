#pragma once
#include <napi/env.h>

#include <Babylon/JsRuntime.h>

namespace Babylon::Plugins::Internal
{
    class DataStream final : public Napi::ObjectWrap<DataStream>
    {
    public:
        static inline constexpr const char* JS_INSTANCE_NAME{"fflate"};

        using ParentT = Napi::ObjectWrap<DataStream>;

        static void CreateInstance(Napi::Env env)
        {
            Napi::HandleScope scope{env};

            Napi::Object fflate = Napi::Object::New(env);

            fflate.Set("unzipSync", Napi::Function::New(env, DataStream::UnzipSync, "unzipSync"));

            // Attach to global scope
            env.Global().Set("fflate", fflate);
        }

        DataStream(const Napi::CallbackInfo& info)
            : DataStream(info, JsRuntime::GetFromJavaScript(info.Env()))
        {
        }

        explicit DataStream(const Napi::CallbackInfo& info, JsRuntime& runtime)
            : ParentT{info}
            , m_runtime{runtime}
        {
        }

    private:
        JsRuntime& m_runtime;

        static Napi::Value UnzipSync(const Napi::CallbackInfo& info);

    };
} // namespace
