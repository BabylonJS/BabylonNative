#include <gtest/gtest.h>
#include <napi/env.h>
#include <JavaScriptCore/JavaScript.h>
#include "../../../Core/Node-API/Source/js_native_api_javascriptcore.h"

namespace
{
    class JscNodeApi : public testing::Test
    {
    protected:
        JSGlobalContextRef context{};
        napi_env env{};

        virtual const char* BeforeAttach() { return "void 0"; }

        void SetUp() override
        {
            context = JSGlobalContextCreate(nullptr);
            ASSERT_NE(context, nullptr);
            Eval(BeforeAttach());
            env = Napi::Attach(context);
        }

        void TearDown() override
        {
            if (env != nullptr)
            {
                Napi::Detach(env);
            }
            JSGlobalContextRelease(context);
        }

        napi_value Eval(const char* source)
        {
            JSStringRef script{JSStringCreateWithUTF8CString(source)};
            JSValueRef exception{};
            JSValueRef value{JSEvaluateScript(context, script, nullptr, nullptr, 1, &exception)};
            JSStringRelease(script);
            EXPECT_EQ(exception, nullptr) << source;
            return reinterpret_cast<napi_value>(const_cast<OpaqueJSValue*>(value));
        }

        void ExpectDetachedMetadata(napi_value buffer)
        {
            bool detached{};
            ASSERT_EQ(napi_is_detached_arraybuffer(env, buffer, &detached), napi_ok);
            EXPECT_TRUE(detached);
            void* data{this};
            size_t length{99};
            ASSERT_EQ(napi_get_arraybuffer_info(env, buffer, &data, &length), napi_ok);
            EXPECT_EQ(data, nullptr);
            EXPECT_EQ(length, 0u);
            Napi::ArrayBuffer wrapper{env, buffer};
            EXPECT_EQ(wrapper.Data(), nullptr);
            EXPECT_EQ(wrapper.ByteLength(), 0u);
        }
    };

    class JscNodeApiWithoutDetachedGetter : public JscNodeApi
    {
        const char* BeforeAttach() override
        {
            return "delete ArrayBuffer.prototype.detached";
        }
    };
}

TEST(NodeApi, JscRegistryRetainsReplacementOwner)
{
    JSGlobalContextRef context{JSGlobalContextCreate(nullptr)};
    {
        auto older{std::make_unique<napi_env__>(context)};
        auto newer{std::make_unique<napi_env__>(context)};
        EXPECT_EQ(napi_env__::get(context), newer.get());
        older.reset();
        EXPECT_EQ(napi_env__::get(context), newer.get());
    }
    EXPECT_EQ(napi_env__::get(context), nullptr);
    JSGlobalContextRelease(context);
}

TEST_F(JscNodeApi, BigIntIgnoresPatchedIntrinsics)
{
    if (!env->bigint_supported)
    {
        GTEST_SKIP() << "BigInt is unavailable in this engine";
    }
    Eval(R"JS(
        const originalBigInt = BigInt;
        const wrong = originalBigInt(1234);
        originalBigInt.asIntN = originalBigInt.asUintN = function () { return wrong; };
        originalBigInt.prototype.toString = function () { return '1234'; };
        globalThis.BigInt = function () { return wrong; };
        BigInt.asIntN = BigInt.asUintN = function () { return wrong; };
    )JS");
    napi_value value{};
    constexpr int64_t expected{9007199254740993LL};
    ASSERT_EQ(napi_create_bigint_int64(env, expected, &value), napi_ok);
    napi_valuetype type{};
    ASSERT_EQ(napi_typeof(env, value, &type), napi_ok);
    EXPECT_EQ(type, napi_bigint);
    int64_t signedValue{};
    bool lossless{};
    ASSERT_EQ(napi_get_value_bigint_int64(env, value, &signedValue, &lossless), napi_ok);
    EXPECT_EQ(signedValue, expected);
    EXPECT_TRUE(lossless);
    uint64_t unsignedValue{};
    ASSERT_EQ(napi_get_value_bigint_uint64(env, value, &unsignedValue, &lossless), napi_ok);
    EXPECT_EQ(unsignedValue, static_cast<uint64_t>(expected));
    EXPECT_TRUE(lossless);

    // Word construction uses the JS fallback even when native int64 creation is available.
    const uint64_t words[]{0xfedcba9876543210ULL, 0x123456789abcdef0ULL};
    ASSERT_EQ(napi_create_bigint_words(env, 1, 2, words, &value), napi_ok);
    uint64_t actual[2]{};
    size_t count{2};
    int sign{};
    ASSERT_EQ(napi_get_value_bigint_words(env, value, &sign, &count, actual), napi_ok);
    EXPECT_EQ(sign, 1);
    EXPECT_EQ(count, 2u);
    EXPECT_EQ(actual[0], words[0]);
    EXPECT_EQ(actual[1], words[1]);
}

TEST_F(JscNodeApi, DetachIgnoresPatchedIntrinsicsAndNormalizesMetadata)
{
    if (!Napi::Value(env, Eval("typeof ArrayBuffer.prototype.transfer === 'function'")).ToBoolean())
    {
        GTEST_SKIP() << "ArrayBuffer transfer is unavailable in this engine";
    }
    auto buffer{Eval(R"JS(
        globalThis.buffer = new ArrayBuffer(32);
        ArrayBuffer.prototype.transfer = function () { throw new Error('patched transfer'); };
        ArrayBuffer.prototype.slice = function () { throw new Error('patched slice'); };
        Object.defineProperty(ArrayBuffer.prototype, 'detached', {get() { return true; }, configurable: true});
        Object.defineProperty(buffer, 'detached', {value: true, configurable: true});
        buffer.transfer = function () { throw new Error('own transfer'); };
        buffer;
    )JS")};
    // Reading metadata without requesting a byte pointer must not pin the buffer.
    bool detached{true};
    ASSERT_EQ(napi_is_detached_arraybuffer(env, buffer, &detached), napi_ok);
    EXPECT_FALSE(detached);
    size_t length{};
    ASSERT_EQ(napi_get_arraybuffer_info(env, buffer, nullptr, &length), napi_ok);
    EXPECT_EQ(length, 32u);
    ASSERT_EQ(napi_detach_arraybuffer(env, buffer), napi_ok);
    ExpectDetachedMetadata(buffer);
}

TEST_F(JscNodeApi, ObjectTypeofSkipsBigIntPredicateWhenNativeTypeIsAvailable)
{
    if (!env->value_type_reports_bigint)
    {
        GTEST_SKIP() << "The runtime does not expose a native BigInt type";
    }
    JSValueUnprotect(context, env->is_bigint_function);
    env->is_bigint_function = reinterpret_cast<JSValueRef>(Eval("(function () { return true; })"));
    JSValueProtect(context, env->is_bigint_function);
    napi_valuetype type{};
    ASSERT_EQ(napi_typeof(env, Eval("({})"), &type), napi_ok);
    EXPECT_EQ(type, napi_object);
    ASSERT_EQ(napi_typeof(env, Eval("(function () {})"), &type), napi_ok);
    EXPECT_EQ(type, napi_function);
    napi_value bigint{};
    ASSERT_EQ(napi_create_bigint_int64(env, 1, &bigint), napi_ok);
    ASSERT_EQ(napi_typeof(env, bigint, &type), napi_ok);
    EXPECT_EQ(type, napi_bigint);
}

TEST_F(JscNodeApiWithoutDetachedGetter, SliceFallbackIgnoresPatchedMethods)
{
    if (!Napi::Value(env, Eval("typeof ArrayBuffer.prototype.transfer === 'function'")).ToBoolean())
    {
        GTEST_SKIP() << "ArrayBuffer transfer is unavailable in this engine";
    }
    auto buffer{Eval(R"JS(
        globalThis.buffer = new ArrayBuffer(32);
        ArrayBuffer.prototype.slice = function () { throw new Error('patched slice'); };
        buffer.slice = ArrayBuffer.prototype.slice;
        buffer;
    )JS")};
    bool detached{true};
    ASSERT_EQ(napi_is_detached_arraybuffer(env, buffer, &detached), napi_ok);
    EXPECT_FALSE(detached);
    ASSERT_EQ(napi_detach_arraybuffer(env, buffer), napi_ok);
    ExpectDetachedMetadata(buffer);
    const auto empty{Eval("new ArrayBuffer(0)")};
    ASSERT_EQ(napi_is_detached_arraybuffer(env, empty, &detached), napi_ok);
    EXPECT_FALSE(detached);
}

TEST_F(JscNodeApi, DetachNeverReportsSuccessForPinnedBuffer)
{
    if (!Napi::Value(env, Eval("typeof ArrayBuffer.prototype.transfer === 'function'")).ToBoolean())
    {
        GTEST_SKIP() << "ArrayBuffer transfer is unavailable in this engine";
    }
    auto buffer{Eval("new ArrayBuffer(32)")};
    void* data{};
    ASSERT_EQ(napi_get_arraybuffer_info(env, buffer, &data, nullptr), napi_ok);
    ASSERT_NE(data, nullptr);
    const auto status{napi_detach_arraybuffer(env, buffer)};
    if (status == napi_pending_exception)
    {
        napi_value exception{};
        ASSERT_EQ(napi_get_and_clear_last_exception(env, &exception), napi_ok);
    }
    bool detached{};
    ASSERT_EQ(napi_is_detached_arraybuffer(env, buffer, &detached), napi_ok);
    if (status == napi_ok)
    {
        EXPECT_TRUE(detached);
        ExpectDetachedMetadata(buffer);
    }
    else
    {
        EXPECT_TRUE(status == napi_pending_exception || status == napi_detachable_arraybuffer_expected);
        EXPECT_FALSE(detached);
    }
}

TEST_F(JscNodeApi, PropertyOperationsCoercePrimitiveReceivers)
{
    const auto text{Eval("'hello'")};
    const auto number{Eval("42")};
    const auto key{Eval("'length'")};
    napi_value value{};
    ASSERT_EQ(napi_get_property(env, text, key, &value), napi_ok);
    EXPECT_EQ(Napi::Number(env, value).Uint32Value(), 5u);
    ASSERT_EQ(napi_get_named_property(env, text, "length", &value), napi_ok);
    EXPECT_EQ(Napi::Number(env, value).Uint32Value(), 5u);
    ASSERT_EQ(napi_get_element(env, text, 1, &value), napi_ok);
    EXPECT_EQ(Napi::String(env, value).Utf8Value(), "e");
    bool present{};
    EXPECT_EQ(napi_has_property(env, text, key, &present), napi_ok);
    EXPECT_TRUE(present);
    EXPECT_EQ(napi_has_named_property(env, text, "length", &present), napi_ok);
    EXPECT_TRUE(present);
    EXPECT_EQ(napi_has_element(env, text, 1, &present), napi_ok);
    EXPECT_TRUE(present);
    EXPECT_EQ(napi_set_property(env, number, key, number), napi_ok);
    EXPECT_EQ(napi_set_named_property(env, number, "other", number), napi_ok);
    EXPECT_EQ(napi_set_element(env, number, 1, number), napi_ok);
    EXPECT_EQ(napi_delete_property(env, number, key, &present), napi_ok);
    EXPECT_TRUE(present);
    EXPECT_EQ(napi_delete_element(env, number, 1, &present), napi_ok);
    EXPECT_TRUE(present);
    ASSERT_EQ(napi_get_prototype(env, text, &value), napi_ok);
    EXPECT_TRUE(Napi::Value(env, value).StrictEquals(Napi::Value(env, Eval("String.prototype"))));
    ASSERT_EQ(napi_get_prototype(env, Eval("Object.create(null)"), &value), napi_ok);
    EXPECT_TRUE(Napi::Value(env, value).IsNull());
    for (auto receiver : {Eval("null"), Eval("undefined")})
    {
        EXPECT_EQ(napi_get_named_property(env, receiver, "length", &value), napi_object_expected);
        bool pending{};
        ASSERT_EQ(napi_is_exception_pending(env, &pending), napi_ok);
        EXPECT_TRUE(pending);
        ASSERT_EQ(napi_get_and_clear_last_exception(env, &value), napi_ok);
    }
}

TEST_F(JscNodeApi, CallsPreserveStrictPrimitiveReceivers)
{
    const auto function{Eval("(function () { 'use strict'; return this; })")};
    for (auto receiver : {Eval("'hello'"), Eval("42"), Eval("null"), Eval("undefined")})
    {
        napi_value result{};
        ASSERT_EQ(napi_call_function(env, receiver, function, 0, nullptr, &result), napi_ok);
        EXPECT_TRUE(Napi::Value(env, result).StrictEquals(Napi::Value(env, receiver)));
    }
}

TEST_F(JscNodeApi, ObjectOnlyOperationsRejectPrimitives)
{
    const auto primitive{Eval("42")};
    const auto object{Eval("({})")};
    napi_value value{};
    void* data{};
    uint32_t length{};
    bool matches{};
    EXPECT_EQ(napi_get_value_external(env, primitive, &data), napi_invalid_arg);
    EXPECT_EQ(napi_wrap(env, primitive, this, nullptr, nullptr, nullptr), napi_invalid_arg);
    EXPECT_EQ(napi_unwrap(env, primitive, &data), napi_invalid_arg);
    EXPECT_EQ(napi_add_finalizer(env, primitive, this, [](napi_env, void*, void*) {}, nullptr, nullptr), napi_invalid_arg);
    EXPECT_EQ(napi_get_array_length(env, primitive, &length), napi_array_expected);
    EXPECT_EQ(napi_call_function(env, object, primitive, 0, nullptr, &value), napi_function_expected);
    EXPECT_EQ(napi_new_instance(env, primitive, 0, nullptr, &value), napi_function_expected);
    EXPECT_EQ(napi_new_instance(env, object, 0, nullptr, &value), napi_function_expected);
    EXPECT_EQ(napi_instanceof(env, object, primitive, &matches), napi_function_expected);
    EXPECT_EQ(napi_instanceof(env, object, object, &matches), napi_function_expected);
}

TEST_F(JscNodeApi, PrimitiveReferenceLifetimeFollowsApiVersion)
{
    for (auto value : {Eval("'text'"), Eval("42"), Eval("true"), Eval("null"), Eval("undefined")})
    {
        napi_ref ref{};
#if NAPI_VERSION < 10
        EXPECT_EQ(napi_create_reference(env, value, 1, &ref), napi_invalid_arg);
        EXPECT_EQ(ref, nullptr);
#else
        ASSERT_EQ(napi_create_reference(env, value, 1, &ref), napi_ok);
        napi_value result{};
        ASSERT_EQ(napi_get_reference_value(env, ref, &result), napi_ok);
        EXPECT_EQ(result, value);
        uint32_t count{};
        ASSERT_EQ(napi_reference_unref(env, ref, &count), napi_ok);
        EXPECT_EQ(count, 0u);
        result = value;
        EXPECT_EQ(napi_get_reference_value(env, ref, &result), napi_ok);
        EXPECT_EQ(result, nullptr);
        EXPECT_EQ(napi_reference_ref(env, ref, &count), napi_ok);
        EXPECT_EQ(count, 0u);
        EXPECT_EQ(napi_delete_reference(env, ref), napi_ok);
#endif
    }
    const auto symbol{Eval("Symbol('retained')")};
    napi_ref ref{};
    ASSERT_EQ(napi_create_reference(env, symbol, 0, &ref), napi_ok);
    napi_value result{};
    EXPECT_EQ(napi_get_reference_value(env, ref, &result), napi_ok);
    EXPECT_EQ(result, symbol);
    uint32_t count{99};
    EXPECT_EQ(napi_reference_ref(env, ref, &count), napi_ok);
    EXPECT_EQ(count, 1u);
    EXPECT_EQ(napi_reference_unref(env, ref, &count), napi_ok);
    EXPECT_EQ(count, 0u);
    EXPECT_EQ(napi_reference_unref(env, ref, nullptr), napi_generic_failure);
    EXPECT_EQ(napi_get_reference_value(env, ref, &result), napi_ok);
    EXPECT_EQ(result, symbol);
    EXPECT_EQ(napi_delete_reference(env, ref), napi_ok);
}

TEST_F(JscNodeApi, DeadWeakReferenceCannotBePromoted)
{
    const auto object{Eval("({})")};
    napi_ref ref{};
    ASSERT_EQ(napi_create_reference(env, object, 0, &ref), napi_ok);
    // Model the sentinel's completed finalization without relying on conservative GC timing.
    env->reference_tracking_state->active_ref_values.erase(object);
    napi_value result{object};
    EXPECT_EQ(napi_get_reference_value(env, ref, &result), napi_ok);
    EXPECT_EQ(result, nullptr);
    uint32_t count{99};
    EXPECT_EQ(napi_reference_ref(env, ref, &count), napi_ok);
    EXPECT_EQ(count, 0u);
    EXPECT_EQ(napi_delete_reference(env, ref), napi_ok);
}

TEST_F(JscNodeApi, ReferenceCreationPropagatesSentinelFailure)
{
    const auto object{Eval("new Proxy({}, {has() { throw new Error('sentinel lookup'); }})")};
    napi_ref ref{};
    EXPECT_EQ(napi_create_reference(env, object, 1, &ref), napi_pending_exception);
    EXPECT_EQ(ref, nullptr);
    napi_value exception{};
    EXPECT_EQ(napi_get_and_clear_last_exception(env, &exception), napi_ok);
}

TEST_F(JscNodeApi, WeakReferenceLookupPropagatesSentinelFailure)
{
    const auto object{Eval(R"JS(
        globalThis.failSentinelLookup = false;
        new Proxy({}, {has(target, key) {
            if (failSentinelLookup) { throw new Error('sentinel lookup'); }
            return Reflect.has(target, key);
        }});
    )JS")};
    napi_ref ref{};
    ASSERT_EQ(napi_create_reference(env, object, 0, &ref), napi_ok);
    Eval("failSentinelLookup = true");
    napi_value value{};
    EXPECT_EQ(napi_get_reference_value(env, ref, &value), napi_pending_exception);
    EXPECT_EQ(napi_get_and_clear_last_exception(env, &value), napi_ok);
    EXPECT_EQ(napi_reference_ref(env, ref, nullptr), napi_pending_exception);
    EXPECT_EQ(napi_get_and_clear_last_exception(env, &value), napi_ok);
    EXPECT_EQ(napi_delete_reference(env, ref), napi_ok);
}

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
