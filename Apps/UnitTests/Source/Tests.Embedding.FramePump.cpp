#include <gtest/gtest.h>

#include <Babylon/Embedding/Runtime.h>
#include <Babylon/Embedding/View.h>
#include <Babylon/Graphics/Device.h>
#include <Babylon/Graphics/DeviceContext.h>

#include <array>
#include <stdexcept>

#ifdef _WIN32
#include <winrt/base.h>
#endif

extern Babylon::Graphics::Configuration g_deviceConfig;

class EmbeddingFramePump : public testing::Test
{
public:
    static void SetUpTestSuite()
    {
#ifdef _WIN32
        // NativeInput caches WinRT factories. Keep the host apartment alive
        // across the sequential JS runtimes, as an embedding application does.
        winrt::init_apartment();
#endif
    }

    static void TearDownTestSuite()
    {
#ifdef _WIN32
        winrt::clear_factory_cache();
        winrt::uninit_apartment();
#endif
    }
};

TEST_F(EmbeddingFramePump, SynchronousDispatchPreservesOrderingAndContinuesAfterFlush)
{
    Babylon::Embedding::Runtime runtime{};
    EXPECT_THROW(runtime.RunOnJsThreadAndWait([](Napi::Env) {}), std::runtime_error);
    Babylon::Embedding::View view{runtime, g_deviceConfig.Window};
    EXPECT_THROW(runtime.RunOnJsThreadAndWait([](Napi::Env) {}), std::runtime_error);
    view.Resize(32, 32, Babylon::Embedding::CoordinateUnits::Logical);

    runtime.Eval("globalThis.framePumpValue = 41;");
    runtime.RunOnJsThreadAndWait([](Napi::Env env) {
        EXPECT_EQ(env.Global().Get("framePumpValue").As<Napi::Number>().Int32Value(), 41);
        auto& context = Babylon::Graphics::DeviceContext::GetFromJavaScript(env);
        const auto generation = context.ViewIdGeneration();
        for (size_t flush = 0; flush < 3; ++flush)
        {
            const auto threshold = bgfx::getCaps()->limits.maxViews - 16;
            while (context.PeekNextViewId() < threshold)
            {
                context.AcquireNewViewId();
            }
            context.FlushViewsIfNeeded();
        }
        EXPECT_EQ(context.ViewIdGeneration(), generation + 3);
        EXPECT_NE(context.GetActiveEncoder(), nullptr);
        env.Global().Set("framePumpValue", 42);
    }, true);
    runtime.RunOnJsThreadAndWait([](Napi::Env env) {
        EXPECT_EQ(env.Global().Get("framePumpValue").As<Napi::Number>().Int32Value(), 42);
    });
    EXPECT_THROW(runtime.RunOnJsThreadAndWait([](Napi::Env env) {
        throw Napi::Error::New(env, "embedding callback failure");
    }), std::runtime_error);
    view.RenderFrame();

    runtime.Suspend();
    EXPECT_THROW(runtime.RunOnJsThreadAndWait([](Napi::Env) {}), std::runtime_error);
    runtime.Resume();
    runtime.RunOnJsThreadAndWait([](Napi::Env env) {
        EXPECT_TRUE(Babylon::Graphics::DeviceContext::GetFromJavaScript(env).ForceMidFrameFlush());
    });
}

#if BABYLON_NATIVE_POLYFILL_CANVAS
TEST_F(EmbeddingFramePump, SynchronousCanvasReadbackFlushesAndReturnsPixels)
{
    Babylon::Embedding::Runtime runtime{};
    Babylon::Embedding::View view{runtime, g_deviceConfig.Window};
    view.Resize(32, 32, Babylon::Embedding::CoordinateUnits::Logical);
    runtime.RunOnJsThreadAndWait([](Napi::Env env) {
        auto& graphics = Babylon::Graphics::DeviceContext::GetFromJavaScript(env);
        const auto generation = graphics.ViewIdGeneration();
        const auto constructor = Babylon::JsRuntime::NativeObject::GetFromJavaScript(env).Get("Canvas").As<Napi::Function>();
        auto canvas = constructor.New({});
        canvas.Set("width", 2);
        canvas.Set("height", 2);
        auto context = canvas.Get("getContext").As<Napi::Function>().Call(canvas, {Napi::String::New(env, "2d")}).As<Napi::Object>();
        const std::array<std::pair<const char*, std::array<uint8_t, 4>>, 2> colors{{
            {"#ff0000", {255, 0, 0, 255}},
            {"#00ff00", {0, 255, 0, 255}},
        }};
        for (const auto& [color, expected] : colors)
        {
            context.Set("fillStyle", color);
            context.Get("fillRect").As<Napi::Function>().Call(context, {
                Napi::Number::New(env, 0), Napi::Number::New(env, 0),
                Napi::Number::New(env, 2), Napi::Number::New(env, 2)});
            auto image = context.Get("getImageData").As<Napi::Function>().Call(context, {
                Napi::Number::New(env, 0), Napi::Number::New(env, 0),
                Napi::Number::New(env, 2), Napi::Number::New(env, 2)}).As<Napi::Object>();
            auto pixels = image.Get("data").As<Napi::Uint8Array>();
            ASSERT_EQ(pixels.ElementLength(), 16u);
            for (size_t byte = 0; byte < pixels.ElementLength(); ++byte)
            {
                EXPECT_EQ(pixels[byte], expected[byte % 4]);
            }
        }
        EXPECT_GT(graphics.ViewIdGeneration(), generation);
        EXPECT_NE(graphics.GetActiveEncoder(), nullptr);
    });
    view.RenderFrame();
}
#endif
