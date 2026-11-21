#include <Babylon/AppRuntime.h>
#include <napi/napi.h>
#include <gtest/gtest.h>
#include <chrono>
#include <future>
#include "../../../Dependencies/xr/Include/XR.h"
#include "../../../Plugins/NativeXr/Source/Constants.h"
#include "../../../Plugins/NativeXr/Source/XRRigidTransform.h"
#include "../../../Plugins/NativeXr/Source/XRReferenceSpace.h"

TEST(NativeXr, RequestReferenceSpaceResolvesOrRejectsWithoutTerminatingTheRuntime)
{
    Babylon::AppRuntime runtime{};
    auto done = std::make_shared<std::promise<std::string>>();
    auto result = done->get_future();
    runtime.Dispatch([done](Napi::Env env) {
        try
        {
            Babylon::XRReferenceSpace::Initialize(env);
            env.Global().Set("requestSpace", Napi::Function::New(env, Babylon::XRReferenceSpace::Request));
            env.Global().Set("finishTest", Napi::Function::New(env, [done](const Napi::CallbackInfo& info) {
                done->set_value(info[0].As<Napi::String>().Utf8Value());
            }));
            Napi::Eval(env, R"JS(
                (async () => {
                    const check = (ok, message) => { if (!ok) throw Error(message); };
                    for (const type of ["viewer", "local", "unbounded", {toString: () => "local"}]) {
                        check(await requestSpace(type) instanceof XRReferenceSpace, String(type));
                    }
                    const reject = async (args, name) => {
                        const promise = requestSpace(...args);
                        check(promise instanceof Promise, "Expected a promise");
                        const error = await promise.then(() => null, e => e);
                        check(error && error.name === name, "Expected " + name + " for " + args);
                        if (name === "TypeError") check(error instanceof TypeError, "Expected a TypeError");
                        return error;
                    };
                    for (const type of ["local-floor", "bounded-floor"]) await reject([type], "NotSupportedError");
                    await reject(["floor"], "TypeError");
                    await reject([], "TypeError");
                    check(await requestSpace("viewer") instanceof XRReferenceSpace, "Fallback after rejection");
                })().then(() => finishTest(""), e => finishTest(String(e)));
            )JS", "reference-space-request.js");
        }
        catch (const std::exception& error)
        {
            done->set_value(error.what());
        }
    });
    ASSERT_EQ(result.wait_for(std::chrono::seconds{10}), std::future_status::ready);
    EXPECT_EQ(result.get(), "");
}
