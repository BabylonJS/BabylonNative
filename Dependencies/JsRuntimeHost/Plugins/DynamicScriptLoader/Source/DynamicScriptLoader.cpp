#include <Babylon/Plugins/DynamicScriptLoader.h>
#include <Babylon/JsRuntime.h>

#include <stdexcept>
#include <utility>

namespace Babylon::Plugins::DynamicScriptLoader
{
    namespace
    {
        Napi::Value LoadScript(const Napi::CallbackInfo& info, const ResolverT& resolver)
        {
            auto env = info.Env();
            const auto reject = [env](Napi::Value error) {
                const auto deferred = Napi::Promise::Deferred::New(env);
                deferred.Reject(error);
                return deferred.Promise();
            };
            if (info.Length() != 1 || !info[0].IsString())
            {
                return reject(Napi::TypeError::New(env, "loadScript expects one resource name").Value());
            }

            const auto name = info[0].As<Napi::String>().Utf8Value();
            if (name.empty() || name.find('\0') != std::string::npos)
            {
                return reject(Napi::TypeError::New(env, "loadScript expects a nonempty resource name without NUL characters").Value());
            }
            try
            {
                auto source = resolver(env, name);
                if (env.IsExceptionPending())
                {
                    throw env.GetAndClearPendingException();
                }
                auto promise = env.Global().Get("Promise").As<Napi::Function>();
                auto resolve = promise.Get("resolve").As<Napi::Function>();
                auto resolved = resolve.Call(promise, {source}).As<Napi::Promise>();
                auto evaluate = Napi::Function::New(env, [name](const Napi::CallbackInfo& callback) {
                    if (!callback[0].IsString())
                    {
                        throw Napi::TypeError::New(callback.Env(), "Script source not found: " + name);
                    }
                    auto text = callback[0].As<Napi::String>().Utf8Value();
                    // The engine-independent Eval API takes a null-terminated string.
                    if (text.find('\0') != std::string::npos)
                    {
                        throw Napi::TypeError::New(callback.Env(), "Script source contains a NUL character: " + name);
                    }
                    Napi::Eval(callback.Env(), text.c_str(), name.c_str());
                });
                return resolved.Get("then").As<Napi::Function>().Call(resolved, {evaluate});
            }
            catch (const Napi::Error& error)
            {
                return reject(error.Value());
            }
            catch (const std::exception& error)
            {
                return reject(Napi::Error::New(env, error.what()).Value());
            }
        }
    }

    void BABYLON_API Initialize(Napi::Env env, ResolverT resolver)
    {
        if (!resolver)
        {
            throw std::invalid_argument{"loadScript requires a resource resolver"};
        }

        auto native = JsRuntime::NativeObject::GetFromJavaScript(env);
        if (native.Has("loadScript"))
        {
            throw Napi::Error::New(env, "_native.loadScript is already defined");
        }

        native.Set("loadScript", Napi::Function::New(env, [resolver = std::move(resolver)](const Napi::CallbackInfo& info) {
            return LoadScript(info, resolver);
        }, "loadScript"));
    }
}
