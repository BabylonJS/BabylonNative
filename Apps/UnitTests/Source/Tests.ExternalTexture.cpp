#include <gtest/gtest.h>

#include <Babylon/AppRuntime.h>
#include <Babylon/Graphics/Device.h>
#include <Babylon/Polyfills/Console.h>
#include <Babylon/Polyfills/Window.h>
#include <Babylon/Plugins/NativeEngine.h>
#include <Babylon/Plugins/ExternalTexture.h>

#include "Utils.h"

#include <atomic>
#include <iostream>
#include <system_error>

extern Babylon::Graphics::Configuration g_deviceConfig;

TEST(ExternalTexture, Construction)
{
#ifdef SKIP_EXTERNAL_TEXTURE_TESTS
    GTEST_SKIP();
#else
    Babylon::Graphics::Device device{g_deviceConfig};
    Babylon::Graphics::DeviceUpdate update{device.GetUpdate("update")};

    device.StartRenderingCurrentFrame();
    update.Start();

    auto nativeTexture = CreateTestTexture(device.GetPlatformInfo().Device, 256, 256);
    Babylon::Plugins::ExternalTexture externalTexture{nativeTexture};
    DestroyTestTexture(nativeTexture);

    EXPECT_EQ(externalTexture.Width(), 256u);
    EXPECT_EQ(externalTexture.Height(), 256u);

    update.Finish();
    device.FinishRenderingCurrentFrame();
#endif
}

TEST(ExternalTexture, CreateForJavaScript)
{
#ifdef SKIP_EXTERNAL_TEXTURE_TESTS
    GTEST_SKIP();
#else
    Babylon::Graphics::Device device{g_deviceConfig};
    Babylon::Graphics::DeviceUpdate update{device.GetUpdate("update")};

    device.StartRenderingCurrentFrame();
    update.Start();

    auto nativeTexture = CreateTestTexture(device.GetPlatformInfo().Device, 256, 256);
    Babylon::Plugins::ExternalTexture externalTexture{nativeTexture};
    DestroyTestTexture(nativeTexture);

    std::promise<void> done{};

    Babylon::AppRuntime runtime{};
    runtime.Dispatch([&device, &done, externalTexture](Napi::Env env) {
        device.AddToJavaScript(env);

        Babylon::Polyfills::Console::Initialize(env, [](const char* message, auto) {
            std::cout << message << std::endl;
        });

        Babylon::Polyfills::Window::Initialize(env);

        Babylon::Plugins::NativeEngine::Initialize(env);

        auto jsTexture = externalTexture.CreateForJavaScript(env);
        EXPECT_TRUE(jsTexture.IsObject());

        done.set_value();
    });

    done.get_future().wait();

    update.Finish();
    device.FinishRenderingCurrentFrame();
#endif
}

// Regression test for the recursive-lock deadlock in ExternalTexture::CreateForJavaScript:
// the implementation must not hold m_impl->Mutex() while it makes a JS-side property lookup
// (Graphics::DeviceContext::GetFromJavaScript), because that lookup can run user JS or
// engine GC/finalizers that may re-enter the same mutex from the same thread.
//
// This test redefines `_Graphics` on the global `_native` object as an accessor whose
// getter calls back into native code and asks the ExternalTexture for its Width(), which
// also takes m_impl->Mutex(). With the bug present, the lookup runs while the mutex is
// already held on this thread, so Width()'s scoped_lock throws
// system_error("resource_deadlock_would_occur") on MSVC. With the fix, the lookup runs
// before the mutex is taken, and Width() succeeds.
//
// The test is restricted to Win32 (Chakra + MSVC std::mutex deadlock detection).
TEST(ExternalTexture, CreateForJavaScriptDoesNotHoldImplMutexAcrossJsCallout)
{
#if defined(SKIP_EXTERNAL_TEXTURE_TESTS) || !defined(_WIN32)
    GTEST_SKIP();
#else
    Babylon::Graphics::Device device{g_deviceConfig};
    Babylon::Graphics::DeviceUpdate update{device.GetUpdate("update")};

    device.StartRenderingCurrentFrame();
    update.Start();

    auto nativeTexture = CreateTestTexture(device.GetPlatformInfo().Device, 256, 256);
    Babylon::Plugins::ExternalTexture externalTexture{nativeTexture};
    DestroyTestTexture(nativeTexture);

    std::promise<std::exception_ptr> done{};
    std::atomic<bool> probeGetterHit{false};
    std::atomic<bool> recursiveLockObserved{false};

    Babylon::AppRuntime runtime{};
    runtime.Dispatch([&device, &done, &probeGetterHit, &recursiveLockObserved, externalTexture](Napi::Env env) {
        try
        {
            device.AddToJavaScript(env);

            Babylon::Polyfills::Console::Initialize(env, [](const char* message, auto) {
                std::cout << message << std::endl;
            });

            Babylon::Polyfills::Window::Initialize(env);

            Babylon::Plugins::NativeEngine::Initialize(env);

            // Stash a reference to the real _Graphics value, then redefine _native._Graphics
            // as an accessor whose getter (a) probes the ExternalTexture lock and (b) returns
            // the real value. This intercepts the same lookup that
            // Graphics::DeviceContext::GetFromJavaScript does inside CreateForJavaScript.
            auto nativeObj = env.Global().Get("_native").As<Napi::Object>();
            env.Global().Set("__realGraphicsForTest", nativeObj.Get("_Graphics"));

            auto graphicsGetter = Napi::Function::New(env,
                [tex = externalTexture, &probeGetterHit, &recursiveLockObserved](const Napi::CallbackInfo& info) -> Napi::Value {
                    probeGetterHit = true;
                    try
                    {
                        // Width() also takes m_impl->Mutex(). If CreateForJavaScript is
                        // currently holding it on this thread, this re-entrant lock attempt
                        // throws system_error on MSVC's std::mutex.
                        static_cast<void>(tex.Width());
                    }
                    catch (const std::system_error&)
                    {
                        recursiveLockObserved = true;
                    }
                    catch (const std::exception&)
                    {
                        recursiveLockObserved = true;
                    }
                    return info.Env().Global().Get("__realGraphicsForTest");
                });

            napi_property_descriptor desc{};
            desc.utf8name = "_Graphics";
            desc.getter = [](napi_env e, napi_callback_info cbinfo) -> napi_value {
                size_t argc = 0;
                napi_value thisArg = nullptr;
                void* data = nullptr;
                napi_get_cb_info(e, cbinfo, &argc, nullptr, &thisArg, &data);
                napi_value getter = static_cast<napi_value>(data);
                napi_value result = nullptr;
                napi_call_function(e, thisArg, getter, 0, nullptr, &result);
                return result;
            };
            desc.attributes = static_cast<napi_property_attributes>(napi_configurable);
            desc.data = static_cast<napi_value>(graphicsGetter);
            napi_status status = napi_define_properties(env, nativeObj, 1, &desc);
            if (status != napi_ok)
            {
                throw std::runtime_error{"napi_define_properties failed installing test getter"};
            }

            externalTexture.CreateForJavaScript(env);

            done.set_value(nullptr);
        }
        catch (...)
        {
            done.set_value(std::current_exception());
        }
    });

    auto future = done.get_future();
    ASSERT_EQ(future.wait_for(std::chrono::seconds(60)), std::future_status::ready)
        << "Dispatch did not complete in time; possible deadlock in CreateForJavaScript.";

    if (auto ex = future.get())
    {
        std::rethrow_exception(ex);
    }

    EXPECT_TRUE(probeGetterHit.load())
        << "Test getter for _native._Graphics was not invoked; "
        << "the test did not actually exercise the lock-holding window.";
    EXPECT_FALSE(recursiveLockObserved.load())
        << "ExternalTexture::CreateForJavaScript held m_impl->Mutex() across the JS-side "
        << "Graphics::DeviceContext::GetFromJavaScript lookup. This can deadlock or terminate "
        << "the process when the JS callout triggers a finalizer that re-enters the same mutex.";

    update.Finish();
    device.FinishRenderingCurrentFrame();
#endif
}

TEST(ExternalTexture, Update)
{
#ifdef SKIP_EXTERNAL_TEXTURE_TESTS
    GTEST_SKIP();
#else
    Babylon::Graphics::Device device{g_deviceConfig};
    Babylon::Graphics::DeviceUpdate update{device.GetUpdate("update")};

    device.StartRenderingCurrentFrame();
    update.Start();

    auto nativeTexture = CreateTestTexture(device.GetPlatformInfo().Device, 256, 256);
    Babylon::Plugins::ExternalTexture externalTexture{nativeTexture};
    DestroyTestTexture(nativeTexture);

    EXPECT_EQ(externalTexture.Width(), 256u);
    EXPECT_EQ(externalTexture.Height(), 256u);

    update.Finish();
    device.FinishRenderingCurrentFrame();

    // Update the external texture to point at a new native texture with different dimensions.
    device.StartRenderingCurrentFrame();
    update.Start();

    auto nativeTexture2 = CreateTestTexture(device.GetPlatformInfo().Device, 128, 128);
    externalTexture.Update(nativeTexture2);
    DestroyTestTexture(nativeTexture2);

    EXPECT_EQ(externalTexture.Width(), 128u);
    EXPECT_EQ(externalTexture.Height(), 128u);

    update.Finish();
    device.FinishRenderingCurrentFrame();
#endif
}
