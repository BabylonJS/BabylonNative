#define USE_EDGEMODE_JSRT
#include <Babylon/AppRuntime.h>
#include <gtest/gtest.h>
#include <jsrt.h>
#include <future>
#include <thread>

TEST(NodeApi, CachedHasOwnPropertySurvivesReplacementAndCollection)
{
    Babylon::AppRuntime runtime{};
    std::promise<bool> result;
    runtime.Dispatch([&result](Napi::Env env) {
        napi_env rawEnv{env};
        Napi::Object prototype{env.Global().Get("Object").As<Napi::Function>().Get("prototype").As<Napi::Object>()};
        prototype.Set("hasOwnProperty", Napi::Function::New(env, [](const Napi::CallbackInfo& info) {
            return Napi::Boolean::New(info.Env(), false);
        }));

        JsContextRef context{};
        JsRuntimeHandle jsRuntime{};
        if (JsGetCurrentContext(&context) != JsNoError ||
            JsGetRuntime(context, &jsRuntime) != JsNoError ||
            JsCollectGarbage(jsRuntime) != JsNoError)
        {
            result.set_value(false);
            return;
        }

        Napi::Object object{Napi::Object::New(env)};
        object.Set("owned", true);
        Napi::String key{Napi::String::New(env, "owned")};
        bool hasOwn{};
        result.set_value(napi_has_own_property(rawEnv, object, key, &hasOwn) == napi_ok && hasOwn);
    });
    EXPECT_TRUE(result.get_future().get());
}

TEST(NodeApi, DirectChakraEmbeddingDisposesBeforeDetach)
{
    std::packaged_task<bool()> lifecycle{[] {
        JsRuntimeHandle runtime{};
        JsContextRef context{};
        if (JsCreateRuntime(JsRuntimeAttributeNone, nullptr, &runtime) != JsNoError ||
            JsCreateContext(runtime, &context) != JsNoError ||
            JsSetCurrentContext(context) != JsNoError)
        {
            return false;
        }

        Napi::Env env{Napi::Attach()};
        struct FinalizerState
        {
            bool called{};
            bool envAlive{};
        } finalizer;
        napi_value external{};
        napi_value global{};
        napi_value objectConstructor{};
        const auto onFinalize = [](napi_env callbackEnv, void* data, void*) {
            auto& state{*static_cast<FinalizerState*>(data)};
            const napi_extended_error_info* error{};
            state.called = true;
            state.envAlive = napi_get_last_error_info(callbackEnv, &error) == napi_ok && error != nullptr;
        };
        if (napi_create_external(env, &finalizer, onFinalize, nullptr, &external) != napi_ok ||
            napi_get_global(env, &global) != napi_ok ||
            napi_get_named_property(env, global, "Object", &objectConstructor) != napi_ok ||
            napi_set_named_property(env, objectConstructor, "teardownExternal", external) != napi_ok ||
            JsSetCurrentContext(JS_INVALID_REFERENCE) != JsNoError ||
            JsDisposeRuntime(runtime) != JsNoError)
        {
            return false;
        }

        Napi::Detach(env);
        return finalizer.called && finalizer.envAlive;
    }};
    auto outcome{lifecycle.get_future()};
    std::thread worker{std::move(lifecycle)};
    worker.join();
    EXPECT_TRUE(outcome.get());
}
