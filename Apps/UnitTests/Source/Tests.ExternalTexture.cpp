#include <gtest/gtest.h>

#include <Babylon/AppRuntime.h>
#include <Babylon/Graphics/Device.h>
#include <Babylon/Polyfills/Console.h>
#include <Babylon/Polyfills/Window.h>
#include <Babylon/Plugins/NativeEngine.h>
#include <Babylon/Plugins/ExternalTexture.h>

#include "Utils.h"

#include <iostream>

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

    // Wait for CreateForJavaScript to complete.
    done.get_future().wait();

    update.Finish();
    device.FinishRenderingCurrentFrame();
#endif
}
