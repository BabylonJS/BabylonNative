#include <Babylon/AppRuntime.h>
#include <Babylon/JsRuntime.h>
#include <Babylon/Plugins/DynamicScriptLoader.h>
#include <Babylon/ScriptLoader.h>
#include <gtest/gtest.h>

#include <chrono>
#include <future>
#include <stdexcept>
#include <string>

TEST(DynamicScriptLoader, SupportsImmediateAndDeferredResources)
{
    std::promise<std::string> result;
    Babylon::AppRuntime runtime{};
    runtime.Dispatch([&result](Napi::Env env) {
        Babylon::Plugins::DynamicScriptLoader::Initialize(env, [](Napi::Env env, const std::string& name) -> Napi::Value {
            if (name == "immediate.js")
            {
                return Napi::String::New(env, "window.loaded = 1;");
            }
            if (name == "deferred.js")
            {
                auto deferred = Napi::Promise::Deferred::New(env);
                Babylon::JsRuntime::GetFromJavaScript(env).Dispatch([deferred](Napi::Env callbackEnv) {
                    deferred.Resolve(Napi::String::New(callbackEnv, "window.loaded += 41;"));
                });
                return deferred.Promise();
            }
            return env.Null();
        });
        env.Global().Set("reportResult", Napi::Function::New(env, [&result](const Napi::CallbackInfo& info) {
            result.set_value(info[0].As<Napi::String>().Utf8Value());
        }));
    });

    Babylon::ScriptLoader loader{runtime};
    loader.Eval(R"(
        _native.loadScript("immediate.js")
            .then(function () { return _native.loadScript("deferred.js"); })
            .then(function () { return _native.loadScript("missing.js"); })
            .then(function () { reportResult("missing script loaded"); },
                  function (error) { reportResult(window.loaded + "|" + error.message); })
            .catch(function (error) { reportResult("unexpected: " + error.message); });
    )", "entry.js");

    auto future = result.get_future();
    ASSERT_EQ(future.wait_for(std::chrono::seconds(10)), std::future_status::ready);
    EXPECT_EQ(future.get(), "42|Script source not found: missing.js");
}

TEST(DynamicScriptLoader, RejectsResolverAndScriptErrors)
{
    std::promise<std::string> result;
    Babylon::AppRuntime runtime{};
    runtime.Dispatch([&result](Napi::Env env) {
        Babylon::Plugins::DynamicScriptLoader::Initialize(env, [](Napi::Env env, const std::string& name) -> Napi::Value {
            if (name == "broken.js")
            {
                return Napi::String::New(env, "throw new Error('script failed')");
            }
            if (name == "rejected.js")
            {
                return Napi::Eval(env, "Promise.reject(new Error('lookup failed'))", "resolver.js");
            }
            throw std::runtime_error{"resolver failed"};
        });
        env.Global().Set("reportResult", Napi::Function::New(env, [&result](const Napi::CallbackInfo& info) {
            result.set_value(info[0].As<Napi::String>().Utf8Value());
        }));
    });

    Babylon::ScriptLoader loader{runtime};
    loader.Eval(R"(
        Promise.all([
            _native.loadScript("broken.js").then(null, function (error) { return error.message; }),
            _native.loadScript("rejected.js").then(null, function (error) { return error.message; }),
            _native.loadScript("throw.js").then(null, function (error) { return error.message; }),
            _native.loadScript().then(null, function (error) { return error.name; })
        ]).then(function (errors) { reportResult(errors.join("|")); });
    )", "entry.js");

    auto future = result.get_future();
    ASSERT_EQ(future.wait_for(std::chrono::seconds(10)), std::future_status::ready);
    EXPECT_EQ(future.get(), "script failed|lookup failed|resolver failed|TypeError");
}

TEST(DynamicScriptLoader, RejectsInvalidNamesAndSources)
{
    std::promise<std::string> result;
    Babylon::AppRuntime runtime{};
    runtime.Dispatch([&result](Napi::Env env) {
        Babylon::Plugins::DynamicScriptLoader::Initialize(env, [](Napi::Env env, const std::string& name) -> Napi::Value {
            if (name == "nul-source")
            {
                std::string source{"window.truncated = true;"};
                source.push_back('\0');
                source += "throw new Error('ignored');";
                return Napi::String::New(env, source);
            }
            if (name == "pending-error")
            {
                Napi::Error::New(env, "pending resolver error").ThrowAsJavaScriptException();
                return env.Undefined();
            }
            if (name == "number")
            {
                return Napi::Number::New(env, 42);
            }
            return Napi::String::New(env, "");
        });
        env.Global().Set("reportResult", Napi::Function::New(env, [&result](const Napi::CallbackInfo& info) {
            result.set_value(info[0].As<Napi::String>().Utf8Value());
        }));
        Napi::Eval(env, R"(
            function rejected(promise) {
                return promise.then(function () { return "accepted"; }, function (error) { return error.name; });
            }
            Promise.all([
                rejected(_native.loadScript("")),
                rejected(_native.loadScript("nul\u0000name")),
                rejected(_native.loadScript(1)),
                rejected(_native.loadScript("one", "two")),
                rejected(_native.loadScript("nul-source")),
                rejected(_native.loadScript("number")),
                _native.loadScript("pending-error").then(null, function (error) { return error.message; }),
                _native.loadScript("empty-source").then(function (value) { return typeof value; })
            ]).then(function (results) {
                results.push(typeof window.truncated);
                reportResult(results.join("|"));
            });
        )", "invalid-input.js");
    });
    auto future = result.get_future();
    ASSERT_EQ(future.wait_for(std::chrono::seconds(10)), std::future_status::ready);
    EXPECT_EQ(future.get(), "TypeError|TypeError|TypeError|TypeError|TypeError|TypeError|pending resolver error|undefined|undefined");
}

TEST(DynamicScriptLoader, PreservesExistingNativeProperties)
{
    std::promise<bool> result;
    Babylon::AppRuntime runtime{};
    runtime.Dispatch([&result](Napi::Env env) {
        auto native = Babylon::JsRuntime::NativeObject::GetFromJavaScript(env);
        native.Set("existing", Napi::Number::New(env, 42));
        auto resolver = [](Napi::Env env, const std::string&) -> Napi::Value { return env.Undefined(); };
        EXPECT_THROW(Babylon::Plugins::DynamicScriptLoader::Initialize(env, {}), std::invalid_argument);
        Babylon::Plugins::DynamicScriptLoader::Initialize(env, resolver);
        EXPECT_THROW(Babylon::Plugins::DynamicScriptLoader::Initialize(env, resolver), Napi::Error);
        result.set_value(native.Get("existing").As<Napi::Number>().Int32Value() == 42 &&
            env.Global().Get("loadScript").IsUndefined() && env.Global().Get("importScripts").IsUndefined());
    });
    auto future = result.get_future();
    ASSERT_EQ(future.wait_for(std::chrono::seconds(10)), std::future_status::ready);
    EXPECT_TRUE(future.get());
}
