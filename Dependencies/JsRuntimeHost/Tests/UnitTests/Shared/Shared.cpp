#include "Shared.h"
#include <Babylon/AppRuntime.h>
#include <Babylon/ScriptLoader.h>
#include <Babylon/Polyfills/AbortController.h>
#include <Babylon/Polyfills/Console.h>
#include <Babylon/Polyfills/Performance.h>
#include <Babylon/Polyfills/Scheduling.h>
#include <Babylon/Polyfills/URL.h>
#include <Babylon/Polyfills/WebSocket.h>
#include <Babylon/Polyfills/XMLHttpRequest.h>
#include <Babylon/Polyfills/Fetch.h>
#include <Babylon/Polyfills/Blob.h>
#include <Babylon/Polyfills/File.h>
#include <Babylon/Polyfills/TextDecoder.h>
#include <Babylon/Polyfills/TextEncoder.h>
#include <gtest/gtest.h>
#include <arcana/threading/blocking_concurrent_queue.h>
#include <atomic>
#include <chrono>
#include <cstdint>
#include <future>
#include <iostream>
#include <thread>

namespace
{
    const char* EnumToString(Babylon::Polyfills::Console::LogLevel logLevel)
    {
        switch (logLevel)
        {
            case Babylon::Polyfills::Console::LogLevel::Log:
                return "log";
            case Babylon::Polyfills::Console::LogLevel::Warn:
                return "warn";
            case Babylon::Polyfills::Console::LogLevel::Error:
                return "error";
        }

        return "unknown";
    }
}

TEST(JavaScript, All)
{
    // Change this to true to wait for the JavaScript debugger to attach (only applies to V8)
    constexpr const bool waitForDebugger = false;

    std::promise<int32_t> exitCodePromise;

    Babylon::AppRuntime::Options options{};

    options.UnhandledExceptionHandler = [&exitCodePromise](const Napi::Error& error) {
        std::cerr << "[Uncaught Error] " << Napi::GetErrorString(error) << std::endl;
        std::cerr.flush();

        exitCodePromise.set_value(-1);
    };

    if (waitForDebugger)
    {
        std::cout << "Waiting for debugger..." << std::endl;
        options.WaitForDebugger = true;
    }

    Babylon::AppRuntime runtime{options};

    runtime.Dispatch([&exitCodePromise](Napi::Env env) mutable {
        Babylon::Polyfills::Console::Initialize(env, [env](const char* message, Babylon::Polyfills::Console::LogLevel logLevel) {
            std::cout << "[" << EnumToString(logLevel) << "] " << message;
            if (logLevel == Babylon::Polyfills::Console::LogLevel::Error)
            {
                std::string stack = Babylon::Polyfills::Console::CaptureCurrentJsStack(env);
                if (!stack.empty())
                {
                    std::cout << std::endl << stack;
                }
            }
            std::cout << std::endl;
            std::cout.flush();
        });

        Babylon::Polyfills::AbortController::Initialize(env);
        Babylon::Polyfills::Performance::Initialize(env);
        Babylon::Polyfills::Scheduling::Initialize(env);
        Babylon::Polyfills::URL::Initialize(env);
        Babylon::Polyfills::WebSocket::Initialize(env);
        Babylon::Polyfills::XMLHttpRequest::Initialize(env);
        Babylon::Polyfills::Fetch::Initialize(env);
        Babylon::Polyfills::Blob::Initialize(env);
        Babylon::Polyfills::File::Initialize(env);
        Babylon::Polyfills::TextDecoder::Initialize(env);
        Babylon::Polyfills::TextEncoder::Initialize(env);

        auto setExitCodeCallback = Napi::Function::New(
            env, [&exitCodePromise](const Napi::CallbackInfo& info) {
                Napi::Env env = info.Env();
                exitCodePromise.set_value(info[0].As<Napi::Number>().Int32Value());
            },
            "setExitCode");
        env.Global().Set("setExitCode", setExitCodeCallback);

        env.Global().Set("hostPlatform", Napi::Value::From(env, JSRUNTIMEHOST_PLATFORM));
    });

    Babylon::ScriptLoader loader{runtime};
    loader.Eval("location = { href: '' };", ""); // Required for Mocha.js as we do not have a location
    loader.LoadScript("app:///Scripts/tests.js");

    auto exitCode{exitCodePromise.get_future().get()};

    EXPECT_EQ(exitCode, 0);
}

// The unit test host's UnhandledExceptionHandler fails the whole JavaScript
// suite, so a throwing timer callback cannot be exercised from tests.ts. This
// covers it natively instead.
TEST(Scheduling, IntervalSurvivesThrowingCallback)
{
    // Regression: repeating timeouts are re-armed after their callback returns
    // rather than before it runs, so an exception escaping a tick must not
    // silently stop the interval. Browsers keep the interval running and report
    // the error, and that is also what this dispatcher did previously.
    std::promise<int32_t> tickCountPromise;
    std::atomic<int32_t> unhandledErrorCount{0};

    Babylon::AppRuntime::Options options{};
    options.UnhandledExceptionHandler = [&unhandledErrorCount](const Napi::Error&) {
        ++unhandledErrorCount;
    };

    Babylon::AppRuntime runtime{options};

    runtime.Dispatch([&tickCountPromise](Napi::Env env) {
        Babylon::Polyfills::Scheduling::Initialize(env);

        auto reportTicks = Napi::Function::New(
            env, [&tickCountPromise](const Napi::CallbackInfo& info) {
                tickCountPromise.set_value(info[0].As<Napi::Number>().Int32Value());
            },
            "reportTicks");
        env.Global().Set("reportTicks", reportTicks);
    });

    Babylon::ScriptLoader loader{runtime};
    loader.Eval(R"(
        var ticks = 0;
        var id = setInterval(function () {
            ticks++;
            if (ticks === 3) {
                clearInterval(id);
                reportTicks(ticks);
                return;
            }
            throw new Error('tick failed');
        }, 1);
    )",
        "");

    auto tickCountFuture{tickCountPromise.get_future()};
    ASSERT_EQ(tickCountFuture.wait_for(std::chrono::seconds(10)), std::future_status::ready)
        << "the interval stopped after a tick threw";
    EXPECT_EQ(tickCountFuture.get(), 3);

    // The first two ticks threw, and those errors must still be surfaced.
    EXPECT_EQ(unhandledErrorCount.load(), 2);
}

TEST(Console, Log)
{
    Babylon::AppRuntime runtime{};

    runtime.Dispatch([](Napi::Env env) mutable {
        Babylon::Polyfills::Console::Initialize(env, [](const char* message, Babylon::Polyfills::Console::LogLevel logLevel) {
            const char* test = "foo bar";
            if (strcmp(message, test) != 0)
            {
                std::cout << "Expected: " << test << std::endl;
                std::cout << "Received: " << message << std::endl;
                std::cout.flush();
                ADD_FAILURE();
            }
        });
    });

    std::promise<void> done;

    Babylon::ScriptLoader loader{runtime};
    loader.Eval("console.log('foo', 'bar')", "");
    loader.Dispatch([&done](auto) {
        done.set_value();
    });

    done.get_future().get();
}

TEST(Console, CaptureCurrentJsStack)
{
    // Regression: Console::CaptureCurrentJsStack must return a non-empty stack when called from
    // within a callback fired by `console.error`, and when called from `console.log` (any frame
    // produced by JS execution).
    Babylon::AppRuntime runtime{};

    std::promise<std::string> errorStackPromise;
    std::promise<std::string> logStackPromise;

    runtime.Dispatch([&errorStackPromise, &logStackPromise](Napi::Env env) mutable {
        Babylon::Polyfills::Console::Initialize(env, [env, &errorStackPromise, &logStackPromise](const char* /*message*/, Babylon::Polyfills::Console::LogLevel logLevel) {
            std::string stack = Babylon::Polyfills::Console::CaptureCurrentJsStack(env);
            if (logLevel == Babylon::Polyfills::Console::LogLevel::Error)
            {
                errorStackPromise.set_value(std::move(stack));
            }
            else if (logLevel == Babylon::Polyfills::Console::LogLevel::Log)
            {
                logStackPromise.set_value(std::move(stack));
            }
        });
    });

    Babylon::ScriptLoader loader{runtime};
    loader.Eval("console.log('log message');", "");
    loader.Eval("function inner() { console.error('error message'); } inner();", "");

    auto errorFuture = errorStackPromise.get_future();
    auto logFuture = logStackPromise.get_future();
    constexpr auto timeout = std::chrono::seconds(30);
    ASSERT_EQ(errorFuture.wait_for(timeout), std::future_status::ready)
        << "console.error callback did not fire within timeout";
    ASSERT_EQ(logFuture.wait_for(timeout), std::future_status::ready)
        << "console.log callback did not fire within timeout";

    std::string errorStack = errorFuture.get();
    std::string logStack = logFuture.get();

    EXPECT_FALSE(errorStack.empty()) << "console.error path must capture a non-empty JS stack";
    EXPECT_FALSE(logStack.empty()) << "console.log path must capture a non-empty JS stack";
}

TEST(AppRuntime, DestroyDoesNotDeadlock)
{
    // Regression test verifying AppRuntime destruction doesn't deadlock.
    // Uses a global arcana hook to sleep while holding the queue mutex
    // before wait(), ensuring the worker is in the vulnerable window
    // when the destructor fires. See #147 for details on the bug and fix.
    //
    // The entire test runs on a separate thread so the gtest thread can
    // detect a deadlock via timeout without hanging the process.
    //
    // Test flow:
    //
    //   Test Thread                    Worker Thread
    //   -----------                    -------------
    //   1. Create AppRuntime           Worker starts, enters blocking_tick
    //      Wait for init to complete
    //   2. Install hook
    //      Dispatch(no-op)             Worker wakes, runs no-op,
    //                                  returns to blocking_tick
    //                                  Hook fires:
    //                                    signal workerInHook
    //                                    sleep 200ms (holding mutex!)
    //   3. workerInHook.wait()
    //      Worker is sleeping in hook
    //   4. ~AppRuntime():
    //          cancel()
    //          Append(no-op):
    //            push() blocks ------> (worker holds mutex)
    //                                  200ms sleep ends
    //                                  wait(lock) releases mutex
    //            push() acquires mutex
    //            pushes, notifies ---> wakes up!
    //            join() waits          drains no-op, cancelled -> exit
    //            join() returns <----- thread exits
    //   5. destroy completes -> PASS

    bool hookSignaled{false};
    std::promise<void> workerInHook;
    std::promise<void> testDone;

    // Run the full lifecycle on a separate thread so the gtest thread
    // can detect a deadlock via timeout.
    std::thread testThread([&]() {
        auto runtime = std::make_unique<Babylon::AppRuntime>();

        // Wait for the runtime to fully initialize. The constructor dispatches
        // CreateForJavaScript which must complete before we install the hook
        // so the worker is idle and ready to enter the hook on the next wait.
        std::promise<void> ready;
        runtime->Dispatch([&ready](Napi::Env) {
            ready.set_value();
        });
        ready.get_future().wait();

        // Install the hook and dispatch a no-op to wake the worker,
        // ensuring it cycles through the hook on its way back to idle.
        arcana::test_hooks::blocking_concurrent_queue::set_before_wait_callback([&]() {
            if (hookSignaled)
            {
                return;
            }
            hookSignaled = true;
            workerInHook.set_value();
            // This sleep is not truly deterministic. Its purpose is to hold the
            // mutex long enough for runtime.reset() (called by the test thread
            // after workerInHook signals) to reach push() while the mutex is
            // still held. When the sleep ends, the worker enters wait() which
            // releases the mutex, allowing push() to acquire it and deliver the
            // wake-up notification. If runtime.reset() hasn't reached push()
            // by the time the sleep ends, the test still passes but doesn't
            // exercise the intended contention window.
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        });
        runtime->Dispatch([](Napi::Env) {});

        // Wait for the worker to be in the hook (holding mutex, sleeping)
        workerInHook.get_future().wait();

        // Destroy — if the fix works, the destructor completes.
        // If broken, it deadlocks and the timeout detects it.
        runtime.reset();
        testDone.set_value();
    });

    auto status = testDone.get_future().wait_for(std::chrono::seconds(5));

    arcana::test_hooks::blocking_concurrent_queue::set_before_wait_callback([]() {});

    if (status == std::future_status::timeout)
    {
        testThread.detach();
        FAIL() << "Deadlock detected: AppRuntime destructor did not complete within 5 seconds";
    }

    testThread.join();
}

// The V8JSI Node-API shim does not implement napi_create_dataview /
// napi_get_dataview_info (its DataView::New throws "TODO"), so this native test
// only builds on the Chakra, V8, and JavaScriptCore backends. The size_t-width
// guard is required because the overflow scenario below needs a 64-bit size_t.
#if (SIZE_MAX > 0xFFFFFFFFu) && !defined(JSRUNTIMEHOST_NAPI_ENGINE_JSI)
TEST(NodeApi, CreateDataViewRejectsOverflowingRange)
{
    // Regression: napi_create_dataview must reject a (byte_offset, byte_length)
    // pair whose sum overflows size_t. The pre-fix code performed an unchecked
    // `byte_offset + byte_length > bufferLength` comparison; with the inputs
    // below the 64-bit sum wraps to 8 and slips past it. It then truncated the
    // values to 32-bit (offset -> 0, length -> 8) and created a valid 8-byte
    // DataView, but stored the ORIGINAL 64-bit offset/length in DataViewInfo,
    // which napi_get_dataview_info hands back alongside the small real buffer --
    // an out-of-bounds access primitive. This path is not reachable from JS
    // `new DataView`, so it is covered natively here. The scenario requires a
    // 64-bit size_t (where the 32-bit truncation diverged from the stored value),
    // hence the size_t-width guard.
    Babylon::AppRuntime runtime{};

    std::promise<bool> overflowSafe;
    std::promise<bool> validAccepted;

    runtime.Dispatch([&overflowSafe, &validAccepted](Napi::Env env) {
        napi_env nenv{env};

        Napi::ArrayBuffer arrayBuffer{Napi::ArrayBuffer::New(env, 16)};
        napi_value arrayBufferValue{arrayBuffer};

        // Low 32 bits are individually valid for the 16-byte buffer (offset 0,
        // length 8), but the full 64-bit values are enormous and their sum wraps
        // around size_t to 8.
        const size_t hugeOffset{0xFFFFFFFF00000000ull};
        const size_t hugeLength{0x0000000100000008ull};

        napi_value result{nullptr};
        napi_status status{napi_create_dataview(nenv, hugeLength, arrayBufferValue, hugeOffset, &result)};

        bool safe;
        if (status != napi_ok || result == nullptr)
        {
            // Fixed path: the out-of-range request is rejected outright.
            safe = true;
        }
        else
        {
            // If creation unexpectedly succeeds, the reported extents must still
            // lie within the 16-byte backing buffer (i.e. not the raw 64-bit
            // inputs). The pre-fix code reported the huge stored values here.
            size_t reportedLength{0};
            size_t reportedOffset{0};
            void* data{nullptr};
            napi_get_dataview_info(nenv, result, &reportedLength, &data, nullptr, &reportedOffset);
            safe = reportedOffset <= 16 && reportedLength <= 16 && reportedOffset + reportedLength <= 16;
        }

        // Clear any pending range error so it doesn't surface as an unhandled error.
        napi_value pendingException{nullptr};
        napi_get_and_clear_last_exception(nenv, &pendingException);
        overflowSafe.set_value(safe);

        // A legitimate offset/length pair must still succeed.
        napi_value validResult{nullptr};
        napi_status validStatus{napi_create_dataview(nenv, 8, arrayBufferValue, 4, &validResult)};
        validAccepted.set_value(validStatus == napi_ok && validResult != nullptr);
    });

    EXPECT_TRUE(overflowSafe.get_future().get());
    EXPECT_TRUE(validAccepted.get_future().get());
}
#endif

// The V8JSI Node-API shim does not expose napi_get_value_string_utf16, so this
// native test only builds on the Chakra, V8, and JavaScriptCore backends.
#if !defined(JSRUNTIMEHOST_NAPI_ENGINE_JSI)
TEST(NodeApi, GetValueStringUtf16HandlesZeroBufsize)
{
    // Regression: napi_get_value_string_utf16 with a non-null buffer and
    // bufsize == 0 must not evaluate bufsize - 1. On the Chakra backend the
    // pre-fix code forwarded bufsize - 1 (== SIZE_MAX) to JsCopyStringUtf16 as
    // the destination capacity, copying the entire JS string into the
    // zero-length buffer, and then stored the terminator at buf[bufsize - 1]
    // (== buf[SIZE_MAX]). The call must instead write nothing and report zero.
    Babylon::AppRuntime runtime{};

    std::promise<bool> zeroSafe;
    std::promise<bool> normalWorks;

    runtime.Dispatch([&zeroSafe, &normalWorks](Napi::Env env) {
        napi_env nenv{env};

        napi_value strValue{Napi::String::New(env, "hello world")};

        // Sentinel-filled buffer. With bufsize == 0 nothing may be written, so
        // every element must survive unchanged (a SIZE_MAX-capacity copy would
        // clobber it / overflow).
        char16_t guard[8];
        for (auto& c : guard)
        {
            c = static_cast<char16_t>(0x7FFF);
        }

        size_t copied{0xDEAD};
        napi_status status{napi_get_value_string_utf16(nenv, strValue, guard, 0, &copied)};

        bool safe{status == napi_ok && copied == 0};
        for (auto c : guard)
        {
            safe = safe && (c == static_cast<char16_t>(0x7FFF));
        }
        zeroSafe.set_value(safe);

        // A sufficiently-sized buffer must still copy and null-terminate.
        char16_t buf[32];
        size_t copied2{0};
        napi_status status2{napi_get_value_string_utf16(nenv, strValue, buf, 32, &copied2)};
        normalWorks.set_value(status2 == napi_ok && copied2 == 11 && buf[copied2] == 0);
    });

    EXPECT_TRUE(zeroSafe.get_future().get());
    EXPECT_TRUE(normalWorks.get_future().get());
}

// Closes an escapable handle scope however the test leaves it. Without this, a
// failing assertion returns with the scope still open, the enclosing
// Napi::HandleScope then fails to close, and Napi::Error::Fatal throws out of its
// implicitly-noexcept destructor -- so the process terminates with no FAILED line
// instead of reporting the assertion.
class ScopedEscapableHandleScope
{
public:
    ScopedEscapableHandleScope(napi_env env, napi_escapable_handle_scope scope)
        : m_env{env}
        , m_scope{scope}
    {
    }

    ~ScopedEscapableHandleScope()
    {
        Close();
    }

    ScopedEscapableHandleScope(const ScopedEscapableHandleScope&) = delete;
    ScopedEscapableHandleScope& operator=(const ScopedEscapableHandleScope&) = delete;

    napi_status Close()
    {
        if (m_scope == nullptr)
        {
            return napi_ok;
        }

        const napi_escapable_handle_scope scope{m_scope};
        m_scope = nullptr;
        return napi_close_escapable_handle_scope(m_env, scope);
    }

private:
    napi_env m_env;
    napi_escapable_handle_scope m_scope;
};

// Regression: a handle returned by napi_escape_handle must stay alive after its
// escapable scope is closed. The escaped handle is stored in the parent scope, so
// closing the scope must not free it along with the scope's own handles. This is the
// contract Napi::ObjectReference::Get relies on, which in turn is what
// Napi::Error::Message and Napi::Error::what use, so getting it wrong turns any
// report of a native error message into a use-after-free.
TEST(NodeApi, EscapedHandleOutlivesItsScope)
{
    Babylon::AppRuntime runtime{};

    std::promise<bool> escapedValueIsIntact;

    runtime.Dispatch([&escapedValueIsIntact](Napi::Env env) mutable {
        napi_env nenv{env};

        // Assertions stay on the test thread: the dispatched lambda reports through the
        // promise and returns early on failure so the waiter can never deadlock.
        napi_escapable_handle_scope scope{};
        if (napi_open_escapable_handle_scope(nenv, &scope) != napi_ok)
        {
            escapedValueIsIntact.set_value(false);
            return;
        }
        ScopedEscapableHandleScope scopeGuard{nenv, scope};

        napi_value inner{};
        if (napi_create_string_utf8(nenv, "escape me", NAPI_AUTO_LENGTH, &inner) != napi_ok)
        {
            escapedValueIsIntact.set_value(false);
            return;
        }

        napi_value escaped{};
        if (napi_escape_handle(nenv, scope, inner, &escaped) != napi_ok)
        {
            escapedValueIsIntact.set_value(false);
            return;
        }

        if (scopeGuard.Close() != napi_ok)
        {
            escapedValueIsIntact.set_value(false);
            return;
        }

        // Allocate through the parent scope so a dangling escaped handle is likely to
        // have been reused by the time it is read back.
        for (int i = 0; i < 32; ++i)
        {
            napi_value filler{};
            napi_create_string_utf8(nenv, "filler filler filler", NAPI_AUTO_LENGTH, &filler);
        }

        char buffer[32]{};
        size_t copied{0};
        const napi_status status{napi_get_value_string_utf8(nenv, escaped, buffer, sizeof(buffer), &copied)};
        escapedValueIsIntact.set_value(status == napi_ok && copied == 9 && std::string{buffer} == "escape me");
    });

    EXPECT_TRUE(escapedValueIsIntact.get_future().get());
}

// Regression: two escapable scopes open at once, both escaping before either closes,
// then closed innermost first. An implementation that stores an escaped handle by
// inserting it into the middle of the handle stack shifts every entry above it,
// silently invalidating the start index the still-open inner scope was handed. Closing
// the inner scope then keeps the wrong slot and frees the inner escaped handle,
// reintroducing the dangling napi_value this fix is about.
//
// Engines differ on whether the outer scope may escape while an inner one is open, so
// the test only requires that of the engines that allow it.
TEST(NodeApi, NestedEscapableScopesBothEscape)
{
    Babylon::AppRuntime runtime{};

    std::promise<bool> bothValuesIntact;

    runtime.Dispatch([&bothValuesIntact](Napi::Env env) mutable {
        napi_env nenv{env};

        const auto fail = [&bothValuesIntact]() { bothValuesIntact.set_value(false); };

        napi_escapable_handle_scope outerScope{};
        if (napi_open_escapable_handle_scope(nenv, &outerScope) != napi_ok)
        {
            return fail();
        }
        ScopedEscapableHandleScope outerGuard{nenv, outerScope};

        // Give the outer scope handles of its own, so the inner scope starts at a
        // different index and the shifting bug is observable.
        for (int i = 0; i < 4; ++i)
        {
            napi_value outerFiller{};
            if (napi_create_string_utf8(nenv, "outer filler", NAPI_AUTO_LENGTH, &outerFiller) != napi_ok)
            {
                return fail();
            }
        }

        napi_value outerSource{};
        if (napi_create_string_utf8(nenv, "outer value", NAPI_AUTO_LENGTH, &outerSource) != napi_ok)
        {
            return fail();
        }

        napi_escapable_handle_scope innerScope{};
        if (napi_open_escapable_handle_scope(nenv, &innerScope) != napi_ok)
        {
            return fail();
        }
        ScopedEscapableHandleScope innerGuard{nenv, innerScope};

        napi_value innerSource{};
        if (napi_create_string_utf8(nenv, "inner value", NAPI_AUTO_LENGTH, &innerSource) != napi_ok)
        {
            return fail();
        }

        // Inner escapes first, then the still-open outer scope escapes.
        napi_value innerEscaped{};
        if (napi_escape_handle(nenv, innerScope, innerSource, &innerEscaped) != napi_ok)
        {
            return fail();
        }

        // Hermes only permits escaping from the innermost open scope and reports
        // napi_handle_scope_mismatch here. That is a legitimate refusal rather than a
        // failure, so record whether the engine allows this and keep checking the part
        // that applies either way.
        napi_value outerEscaped{};
        const napi_status outerEscapeStatus{napi_escape_handle(nenv, outerScope, outerSource, &outerEscaped)};
        const bool outerEscapeSupported{outerEscapeStatus == napi_ok};
        if (!outerEscapeSupported && outerEscapeStatus != napi_handle_scope_mismatch)
        {
            return fail();
        }

        // Close innermost first, as the scopes must be.
        if (innerGuard.Close() != napi_ok)
        {
            return fail();
        }

        // The inner escaped handle now belongs to the outer scope and must still read
        // back while that scope is open. Churn allocations first: a wrongly freed handle
        // only reads back wrong once its block has been reused, so allocate enough to
        // make that near certain rather than a matter of luck.
        for (int i = 0; i < 512; ++i)
        {
            napi_value filler{};
            napi_create_string_utf8(nenv, "filler filler filler", NAPI_AUTO_LENGTH, &filler);
        }

        char innerBuffer[32]{};
        size_t innerCopied{0};
        if (napi_get_value_string_utf8(nenv, innerEscaped, innerBuffer, sizeof(innerBuffer), &innerCopied) != napi_ok ||
            std::string{innerBuffer} != "inner value")
        {
            return fail();
        }

        if (outerGuard.Close() != napi_ok)
        {
            return fail();
        }

        for (int i = 0; i < 512; ++i)
        {
            napi_value filler{};
            napi_create_string_utf8(nenv, "filler filler filler", NAPI_AUTO_LENGTH, &filler);
        }

        if (!outerEscapeSupported)
        {
            // Nothing escaped from the outer scope, so the inner check above is the whole
            // result on this engine.
            bothValuesIntact.set_value(true);
            return;
        }

        char outerBuffer[32]{};
        size_t outerCopied{0};
        const napi_status status{napi_get_value_string_utf8(nenv, outerEscaped, outerBuffer, sizeof(outerBuffer), &outerCopied)};
        bothValuesIntact.set_value(status == napi_ok && std::string{outerBuffer} == "outer value");
    });

    EXPECT_TRUE(bothValuesIntact.get_future().get());
}

// Node-API permits at most one escape per escapable scope. The second call must be
// rejected with napi_escape_called_twice, and must leave the first escaped handle
// untouched rather than replacing or freeing it.
TEST(NodeApi, SecondEscapeIsRejected)
{
    Babylon::AppRuntime runtime{};

    std::promise<bool> secondEscapeRejected;
    std::promise<bool> firstValueIntact;

    runtime.Dispatch([&secondEscapeRejected, &firstValueIntact](Napi::Env env) mutable {
        napi_env nenv{env};

        const auto fail = [&secondEscapeRejected, &firstValueIntact]() {
            secondEscapeRejected.set_value(false);
            firstValueIntact.set_value(false);
        };

        napi_escapable_handle_scope scope{};
        if (napi_open_escapable_handle_scope(nenv, &scope) != napi_ok)
        {
            return fail();
        }
        ScopedEscapableHandleScope scopeGuard{nenv, scope};

        napi_value first{};
        napi_value second{};
        if (napi_create_string_utf8(nenv, "first", NAPI_AUTO_LENGTH, &first) != napi_ok ||
            napi_create_string_utf8(nenv, "second", NAPI_AUTO_LENGTH, &second) != napi_ok)
        {
            return fail();
        }

        napi_value firstEscaped{};
        if (napi_escape_handle(nenv, scope, first, &firstEscaped) != napi_ok)
        {
            return fail();
        }

        napi_value secondEscaped{};
        secondEscapeRejected.set_value(
            napi_escape_handle(nenv, scope, second, &secondEscaped) == napi_escape_called_twice);

        if (scopeGuard.Close() != napi_ok)
        {
            firstValueIntact.set_value(false);
            return;
        }

        for (int i = 0; i < 32; ++i)
        {
            napi_value filler{};
            napi_create_string_utf8(nenv, "filler filler filler", NAPI_AUTO_LENGTH, &filler);
        }

        char buffer[32]{};
        size_t copied{0};
        const napi_status status{napi_get_value_string_utf8(nenv, firstEscaped, buffer, sizeof(buffer), &copied)};
        firstValueIntact.set_value(status == napi_ok && std::string{buffer} == "first");
    });

    EXPECT_TRUE(secondEscapeRejected.get_future().get());
    EXPECT_TRUE(firstValueIntact.get_future().get());
}

// Regression: two escapable scopes opened with no handle allocated between them.
// An implementation whose opaque token is derived from a position in the handle
// stack hands both scopes the same token, so the second scope to escape is refused
// with napi_escape_called_twice despite never having escaped. Deriving the token
// from a counter instead keeps the two apart.
TEST(NodeApi, AdjacentEscapableScopesEscapeIndependently)
{
    Babylon::AppRuntime runtime{};

    std::promise<bool> bothEscapesAccepted;

    runtime.Dispatch([&bothEscapesAccepted](Napi::Env env) mutable {
        napi_env nenv{env};

        const auto fail = [&bothEscapesAccepted]() { bothEscapesAccepted.set_value(false); };

        napi_escapable_handle_scope outerScope{};
        if (napi_open_escapable_handle_scope(nenv, &outerScope) != napi_ok)
        {
            return fail();
        }
        ScopedEscapableHandleScope outerGuard{nenv, outerScope};

        // Deliberately allocate nothing here: this is what makes the two scopes share a
        // position in the handle stack.
        napi_escapable_handle_scope innerScope{};
        if (napi_open_escapable_handle_scope(nenv, &innerScope) != napi_ok)
        {
            return fail();
        }
        ScopedEscapableHandleScope innerGuard{nenv, innerScope};

        napi_value innerSource{};
        if (napi_create_string_utf8(nenv, "inner value", NAPI_AUTO_LENGTH, &innerSource) != napi_ok)
        {
            return fail();
        }

        napi_value innerEscaped{};
        if (napi_escape_handle(nenv, innerScope, innerSource, &innerEscaped) != napi_ok)
        {
            return fail();
        }

        napi_value outerSource{};
        if (napi_create_string_utf8(nenv, "outer value", NAPI_AUTO_LENGTH, &outerSource) != napi_ok)
        {
            return fail();
        }

        // The outer scope has not escaped yet, so this must not be refused.
        napi_value outerEscaped{};
        const napi_status outerEscapeStatus{napi_escape_handle(nenv, outerScope, outerSource, &outerEscaped)};
        if (outerEscapeStatus == napi_handle_scope_mismatch)
        {
            // Engines that only allow escaping from the innermost open scope cannot
            // exercise this case at all; the inner escape above is the whole result.
            bothEscapesAccepted.set_value(true);
            return;
        }
        if (outerEscapeStatus != napi_ok)
        {
            return fail();
        }

        if (innerGuard.Close() != napi_ok || outerGuard.Close() != napi_ok)
        {
            return fail();
        }

        bothEscapesAccepted.set_value(true);
    });

    EXPECT_TRUE(bothEscapesAccepted.get_future().get());
}

#endif

int RunTests()
{
    testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}
