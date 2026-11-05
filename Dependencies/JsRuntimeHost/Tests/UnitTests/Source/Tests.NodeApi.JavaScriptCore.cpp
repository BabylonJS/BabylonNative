#include <gtest/gtest.h>
#include <napi/env.h>
#include <JavaScriptCore/JavaScript.h>

TEST(NodeApi, ReferenceSentinelCanFinalizeAfterDetach)
{
    JSGlobalContextRef context{JSGlobalContextCreate(nullptr)};
    ASSERT_NE(context, nullptr);
    Napi::Env env{Napi::Attach(context)};
    napi_env rawEnv{env};
    napi_value object{};
    napi_value global{};
    napi_ref ref{};
    const bool created{
        napi_create_object(rawEnv, &object) == napi_ok &&
        napi_get_global(rawEnv, &global) == napi_ok &&
        napi_set_named_property(rawEnv, global, "retained", object) == napi_ok &&
        napi_create_reference(rawEnv, object, 1, &ref) == napi_ok};
    EXPECT_TRUE(created);
    if (ref != nullptr)
    {
        EXPECT_EQ(napi_delete_reference(rawEnv, ref), napi_ok);
    }
    Napi::Detach(env);

    JSStringRef name{JSStringCreateWithUTF8CString("retained")};
    EXPECT_TRUE(JSObjectDeleteProperty(context, JSContextGetGlobalObject(context), name, nullptr));
    JSStringRelease(name);
    JSGarbageCollect(context);
    JSGarbageCollect(context);
    JSGlobalContextRelease(context);
}
