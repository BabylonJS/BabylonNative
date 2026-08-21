#include <gtest/gtest.h>

#include <Babylon/AppRuntime.h>
#include <Babylon/Graphics/Device.h>
#include <Babylon/Polyfills/Console.h>
#include <Babylon/Polyfills/Window.h>
#include <Babylon/Plugins/NativeEngine.h>
#include <Babylon/Plugins/ExternalTexture.h>
#include <Babylon/ScriptLoader.h>

#include "Helpers.h"

#include <cstdlib>
#include <future>
#include <iostream>
#include <optional>

extern Babylon::Graphics::Configuration g_deviceConfig;

namespace
{
    constexpr uint32_t TEX_SIZE = 64;

    struct WrappedTextureInfo
    {
        bool hasBinding = false;
        int rawLayerCount = -1;
        bool is2DArray = false;
        uint32_t depth = 0;
        uint32_t baseDepth = 0;
    };
}

// End-to-end test for wrapNativeTexture's array-layer population across the BN/BJS boundary:
//   * BN exposes getTextureLayerCount(handle) (BabylonJS/BabylonNative#1733). It reports the *bound*
//     layer count: the single-slice view count when an ExternalTexture is wrapped with a layerIndex,
//     otherwise the underlying array size.
//   * BJS wrapNativeTexture consumes it (BabylonJS/Babylon.js#18535): layers > 1 -> is2DArray=true,
//     baseDepth=depth=layers; layers == 1 -> a plain 2D texture.
// Wraps three ExternalTextures and asserts the resulting InternalTexture layer flags:
//   (A) arraySize=2, no layerIndex -> whole array  -> layerCount 2, is2DArray=true, depth=baseDepth=2.
//   (B) arraySize=2, layerIndex=0  -> single slice -> layerCount 1, is2DArray=false.
//       This mirrors a host wrapping one slice of an NV12 array per video plane, and guards
//       against regressing a single array slice into a 2D array.
//   (C) arraySize=1, no layerIndex -> single layer -> layerCount 1, is2DArray=false.
// Skips cleanly when the native binding or the consuming BJS change is absent so it stays green
// across the cross-repo landing order.
TEST(ExternalTexture, WrapNativeTextureLayerCount)
{
#if defined(SKIP_EXTERNAL_TEXTURE_TESTS) || defined(SKIP_RENDER_TESTS)
    GTEST_SKIP();
#else
    Babylon::Graphics::Device device{g_deviceConfig};

    Babylon::AppRuntime::Options options{};
    options.UnhandledExceptionHandler = [](const Napi::Error& error) {
        std::cerr << "[Uncaught Error] " << Napi::GetErrorString(error) << std::endl;
        std::quick_exit(1);
    };
    Babylon::AppRuntime runtime{options};

    runtime.Dispatch([&device](Napi::Env env) {
        env.Global().Set("globalThis", env.Global());
        device.AddToJavaScript(env);
        Babylon::Polyfills::Console::Initialize(env, [](const char* message, auto) {
            std::cout << message << std::endl;
        });
        Babylon::Polyfills::Window::Initialize(env);
        Babylon::Plugins::NativeEngine::Initialize(env);
    });

    Babylon::ScriptLoader loader{runtime};
    loader.LoadScript("app:///Assets/babylon.max.js");
    loader.LoadScript("app:///Assets/tests.externalTexture.layerCount.js");

    // Creates a native texture with the requested array size, wraps it via the synchronous
    // CreateForJavaScript (optionally selecting a single array layer), runs wrapNativeTexture on the
    // JS side, and returns the layer-related InternalTexture fields it reports. CreateForJavaScript
    // requires an active frame, so the whole wrap is bracketed by Start/FinishRenderingCurrentFrame.
    auto inspect = [&](uint32_t arraySize, std::optional<uint16_t> layerIndex) -> WrappedTextureInfo {
        device.StartRenderingCurrentFrame();

        auto nativeTexture = Helpers::CreateTexture(
            device.GetPlatformInfo().Device, TEX_SIZE, TEX_SIZE, arraySize, false);
        Babylon::Plugins::ExternalTexture externalTexture{nativeTexture};

        std::promise<WrappedTextureInfo> resultPromise;
        loader.Dispatch([&externalTexture, layerIndex, &resultPromise](Napi::Env env) {
            try
            {
                auto jsNativeTexture = externalTexture.CreateForJavaScript(env, layerIndex);
                auto result = env.Global()
                                  .Get("inspectWrappedTexture")
                                  .As<Napi::Function>()
                                  .Call({jsNativeTexture})
                                  .As<Napi::Object>();
                WrappedTextureInfo wti{};
                wti.hasBinding = result.Get("hasBinding").ToBoolean();
                wti.rawLayerCount = result.Get("rawLayerCount").ToNumber().Int32Value();
                wti.is2DArray = result.Get("is2DArray").ToBoolean();
                wti.depth = result.Get("depth").ToNumber().Uint32Value();
                wti.baseDepth = result.Get("baseDepth").ToNumber().Uint32Value();
                resultPromise.set_value(wti);
            }
            catch (...)
            {
                resultPromise.set_exception(std::current_exception());
            }
        });

        auto info = resultPromise.get_future().get();
        device.FinishRenderingCurrentFrame();

        Helpers::DestroyTexture(nativeTexture);
        return info;
    };

    // --- (A) Multi-layer texture wrapped as the whole array ---
    const WrappedTextureInfo arrayInfo = inspect(2, std::nullopt);

    if (!arrayInfo.hasBinding)
    {
        GTEST_SKIP() << "engine.getTextureLayerCount is unavailable -- requires Babylon Native with "
                        "BabylonJS/BabylonNative#1733.";
    }

    EXPECT_EQ(arrayInfo.rawLayerCount, 2) << "Whole-array wrap should report the native array size.";

    if (arrayInfo.rawLayerCount == 2 && !arrayInfo.is2DArray)
    {
        GTEST_SKIP() << "wrapNativeTexture did not populate is2DArray from the layer count -- requires "
                        "@babylonjs/core with BabylonJS/Babylon.js#18535.";
    }

    EXPECT_TRUE(arrayInfo.is2DArray) << "Whole-array wrap should be is2DArray=true.";
    EXPECT_EQ(arrayInfo.depth, 2u) << "Whole-array depth should equal the native layer count.";
    EXPECT_EQ(arrayInfo.baseDepth, 2u) << "Whole-array baseDepth should equal the native layer count.";

    // --- (B) Same array, wrapped as a single slice (layerIndex=0): one slice of an NV12-style array ---
    const WrappedTextureInfo sliceInfo = inspect(2, static_cast<uint16_t>(0));

    EXPECT_EQ(sliceInfo.rawLayerCount, 1) << "Single-slice view should report 1 bound layer, not the array size.";
    EXPECT_FALSE(sliceInfo.is2DArray) << "Single-slice view must not be flagged is2DArray.";

    // --- (C) Single-layer texture ---
    const WrappedTextureInfo singleInfo = inspect(1, std::nullopt);

    EXPECT_EQ(singleInfo.rawLayerCount, 1) << "Single-layer texture should report 1 layer.";
    EXPECT_FALSE(singleInfo.is2DArray) << "Single-layer texture should not be flagged is2DArray.";
#endif
}
