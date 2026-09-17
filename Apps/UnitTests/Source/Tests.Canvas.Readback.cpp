#include <gtest/gtest.h>

#include <Babylon/AppRuntime.h>
#include <Babylon/Graphics/Device.h>
#include <Babylon/Polyfills/Canvas.h>
#include "../../../Polyfills/Canvas/Source/Canvas.h"
#include "../../../Polyfills/Canvas/Source/Context.h"
#include "../../../Polyfills/Canvas/Source/nanovg/nanovg.h"

#include <array>
#include <chrono>
#include <future>
#include <limits>
#include <memory>
#include <optional>
#include <string>
#include <vector>

extern Babylon::Graphics::Configuration g_deviceConfig;

namespace Babylon::Polyfills::Internal
{
    void gradientSpan(uint32_t* dst, NVGcolor color0, NVGcolor color1, float offset0, float offset1);
    NVGcolor lerpColor(NVGcolor color0, NVGcolor color1, float offset0, float offset1, float position);
}

namespace
{
    template<typename CallbackT>
    void RunCanvasTest(CallbackT callback)
    {
        Babylon::Graphics::Device device{g_deviceConfig};
        device.StartRenderingCurrentFrame();
        std::optional<Babylon::Polyfills::Canvas> canvas;
        Babylon::AppRuntime runtime{};
        std::promise<std::string> completed;
        auto future = completed.get_future();
        runtime.Dispatch([&](Napi::Env env) {
            std::string error;
            try
            {
                device.AddToJavaScript(env);
                canvas.emplace(Babylon::Polyfills::Canvas::Initialize(env));
                callback(env);
            }
            catch (const std::exception& ex)
            {
                error = ex.what();
            }
            completed.set_value(std::move(error));
        });

        while (future.wait_for(std::chrono::milliseconds{16}) != std::future_status::ready)
        {
            device.FinishRenderingCurrentFrame();
            device.StartRenderingCurrentFrame();
        }
        EXPECT_EQ(future.get(), "");
        canvas.reset();
        device.FinishRenderingCurrentFrame();
    }
}

TEST(CanvasGradients, InterpolatesStraightColorAndAlpha)
{
    using namespace Babylon::Polyfills::Internal;
    const auto red = nvgRGBAf(1, 0, 0, 1);
    const auto transparentBlue = nvgRGBAf(0, 0, 1, 0);
    std::array<uint32_t, 256> ramp{};
    gradientSpan(ramp.data(), red, transparentBlue, 0, 1);
    EXPECT_EQ(ramp[0], 0xff0000ffu);
    EXPECT_EQ(ramp[128], 0x7f7f007fu);
    const auto midpoint = lerpColor(red, transparentBlue, 0, 1, 0.5f);
    EXPECT_FLOAT_EQ(midpoint.r, 0.5f);
    EXPECT_FLOAT_EQ(midpoint.g, 0.0f);
    EXPECT_FLOAT_EQ(midpoint.b, 0.5f);
    EXPECT_FLOAT_EQ(midpoint.a, 0.5f);

    gradientSpan(ramp.data(), nvgRGBAf(1, 0, 0, 0), transparentBlue, 0, 1);
    EXPECT_EQ(ramp[128], 0x007f007fu);
    gradientSpan(ramp.data(), nvgRGBAf(0, 1, 0, 1), nvgRGBAf(0, 0, 1, 1), 0, 1);
    EXPECT_EQ(ramp[128], 0xff7f7f00u);
    const auto previous = ramp;
    gradientSpan(ramp.data(), red, transparentBlue, 0.5f, 0.5f);
    EXPECT_EQ(ramp, previous);
}

TEST(CanvasReadback, TranslucentGradientsOverOpaqueBackdrop)
{
#ifdef USE_NOOP_METAL_DEVICE
    GTEST_SKIP() << "GPU readback requires a rendering device";
#endif
    RunCanvasTest([](Napi::Env env) {
        const auto constructor = Babylon::JsRuntime::NativeObject::GetFromJavaScript(env).Get("Canvas").As<Napi::Function>();
        for (const bool radial : {false, true})
        {
            auto canvas = constructor.New({});
            canvas.Set("width", 64);
            canvas.Set("height", 64);
            auto context = canvas.Get("getContext").As<Napi::Function>().Call(canvas, {Napi::String::New(env, "2d")}).As<Napi::Object>();
            auto fillRect = context.Get("fillRect").As<Napi::Function>();
            const std::vector<Napi::Value> rectangle{
                Napi::Number::New(env, 0), Napi::Number::New(env, 0),
                Napi::Number::New(env, 64), Napi::Number::New(env, 64)};
            context.Set("fillStyle", "white");
            fillRect.Call(context, rectangle);
            Napi::Object gradient;
            if (radial)
            {
                gradient = context.Get("createRadialGradient").As<Napi::Function>().Call(context, {
                    Napi::Number::New(env, 32), Napi::Number::New(env, 32), Napi::Number::New(env, 0),
                    Napi::Number::New(env, 32), Napi::Number::New(env, 32), Napi::Number::New(env, 32)}).As<Napi::Object>();
            }
            else
            {
                gradient = context.Get("createLinearGradient").As<Napi::Function>().Call(context, {
                    Napi::Number::New(env, 0), Napi::Number::New(env, 0),
                    Napi::Number::New(env, 64), Napi::Number::New(env, 0)}).As<Napi::Object>();
            }
            auto addColorStop = gradient.Get("addColorStop").As<Napi::Function>();
            addColorStop.Call(gradient, {Napi::Number::New(env, 0), Napi::String::New(env, "red")});
            addColorStop.Call(gradient, {Napi::Number::New(env, 1), Napi::String::New(env, "rgba(0,0,255,0)")});
            context.Set("fillStyle", gradient);
            fillRect.Call(context, rectangle);
            auto image = context.Get("getImageData").As<Napi::Function>().Call(context, {
                Napi::Number::New(env, radial ? 48 : 32), Napi::Number::New(env, 32),
                Napi::Number::New(env, 1), Napi::Number::New(env, 1)}).As<Napi::Object>();
            auto pixels = image.Get("data").As<Napi::Uint8Array>();
            ASSERT_EQ(pixels.ElementLength(), 4u);
            const std::array<uint8_t, 3> expected = radial
                ? std::array<uint8_t, 3>{191, 132, 195}
                : std::array<uint8_t, 3>{191, 129, 193};
            for (size_t channel = 0; channel < expected.size(); ++channel)
            {
                EXPECT_NEAR(pixels[channel], expected[channel], 3) << "radial=" << radial << ", channel=" << channel;
            }
            EXPECT_EQ(pixels[3], 255);
        }
    });
}

TEST(CanvasReadback, DrawImageRejectsArityAndNoOpGeometryBeforeReadbackOrUpload)
{
    RunCanvasTest([](Napi::Env env) {
        const auto constructor = Babylon::JsRuntime::NativeObject::GetFromJavaScript(env).Get("Canvas").As<Napi::Function>();
        const auto createCanvas = [&] {
            auto canvas = constructor.New({});
            canvas.Set("width", 8);
            canvas.Set("height", 8);
            canvas.Get("getContext").As<Napi::Function>().Call(canvas, {Napi::String::New(env, "2d")});
            return canvas;
        };
        auto source = createCanvas();
        auto destination = createCanvas();
        auto* sourceCanvas = Babylon::Polyfills::Internal::NativeCanvas::Unwrap(source);
        auto* destinationCanvas = Babylon::Polyfills::Internal::NativeCanvas::Unwrap(destination);
        auto context = destination.Get("_context").As<Napi::Object>();
        auto drawImage = context.Get("drawImage").As<Napi::Function>();

        auto bitmap = Napi::Object::New(env);
        bitmap.Set("width", 8);
        bitmap.Set("height", 8);
        bitmap.Set("format", 74);
        bitmap.Set("data", Napi::Uint8Array::New(env, 8 * 8 * 4));

        auto* params = nvgInternalParams(destinationCanvas->GetBoundContext()->GetNVGContext());
        const auto restore = [createTexture = params->renderCreateTexture](NVGparams* value) {
            value->renderCreateTexture = createTexture;
        };
        std::unique_ptr<NVGparams, decltype(restore)> restoreTextureCallback{params, restore};
        // A no-op must never reach an upload, even when texture allocation would fail.
        params->renderCreateTexture = [](void*, int, int, int, int, const unsigned char*) -> int {
            ADD_FAILURE() << "drawImage attempted a texture upload for an invalid or no-op call";
            return 0;
        };

        EXPECT_THROW(drawImage.Call(context, {}), Napi::Error);
        const std::vector<std::vector<double>> noOpArguments{
            {std::numeric_limits<double>::quiet_NaN(), 0},
            {0, std::numeric_limits<double>::infinity()},
            {0, 0, 0, 4},
            {0, 0, 4, 0},
            {0, 0, 0, 4, 0, 0, 4, 4},
            {0, 0, 4, 0, 0, 0, 4, 4},
            {20, 0, 4, 4, 0, 0, 4, 4},
            {0, 20, 4, 4, 0, 0, 4, 4},
            {0, 0, 1e300, 4},
            {0, 0, 1e-300, 4},
        };
        for (const auto& image : std::array<Napi::Object, 2>{source, bitmap})
        {
            EXPECT_THROW(drawImage.Call(context, {image, Napi::Number::New(env, 0)}), Napi::Error);
            for (const auto& numbers : noOpArguments)
            {
                std::vector<Napi::Value> args{image};
                for (const double number : numbers)
                {
                    args.push_back(Napi::Number::New(env, number));
                }
                EXPECT_NO_THROW(drawImage.Call(context, args));
            }
        }
        EXPECT_FALSE(sourceCanvas->HasFrameBuffer()) << "no-op drawImage must not flush/read the source";
        EXPECT_FALSE(destinationCanvas->HasFrameBuffer());
    });
}

TEST(CanvasReadback, UntouchedCanvasReadbackCreatesRenderTarget)
{
    RunCanvasTest([](Napi::Env env) {
        const auto constructor = Babylon::JsRuntime::NativeObject::GetFromJavaScript(env).Get("Canvas").As<Napi::Function>();
        for (const bool readImageData : {true, false})
        {
            auto canvas = constructor.New({});
            const auto blankPng = canvas.Get("toDataURL").As<Napi::Function>().Call(canvas, {}).As<Napi::String>().Utf8Value();
            auto context = canvas.Get("getContext").As<Napi::Function>().Call(canvas, {Napi::String::New(env, "2d")}).As<Napi::Object>();
            auto* nativeCanvas = Babylon::Polyfills::Internal::NativeCanvas::Unwrap(canvas);
            ASSERT_FALSE(nativeCanvas->HasFrameBuffer());

            if (readImageData)
            {
                const auto image = context.Get("getImageData").As<Napi::Function>().Call(context, {
                    Napi::Number::New(env, 0), Napi::Number::New(env, 0),
                    Napi::Number::New(env, 1), Napi::Number::New(env, 1)}).As<Napi::Object>();
                EXPECT_EQ(image.Get("width").As<Napi::Number>().Uint32Value(), 1u);
                EXPECT_EQ(image.Get("height").As<Napi::Number>().Uint32Value(), 1u);
                const auto pixels = image.Get("data").As<Napi::Uint8Array>();
                ASSERT_EQ(pixels.ElementLength(), 4u);
                for (size_t index = 0; index < pixels.ElementLength(); ++index)
                {
                    EXPECT_EQ(pixels[index], 0u);
                }
            }
            else
            {
                const auto png = canvas.Get("toDataURL").As<Napi::Function>().Call(canvas, {}).As<Napi::String>().Utf8Value();
                EXPECT_EQ(png.find("data:image/png;base64,iVBORw0KGgo"), 0u);
                EXPECT_EQ(png, blankPng);
            }
            EXPECT_TRUE(nativeCanvas->HasFrameBuffer());
        }
    });
}

TEST(CanvasReadback, EncodesPngWithoutInputImageLoading)
{
    RunCanvasTest([](Napi::Env env) {
        const auto constructor = Babylon::JsRuntime::NativeObject::GetFromJavaScript(env).Get("Canvas").As<Napi::Function>();
        auto canvas = constructor.New({});
        canvas.Set("width", 2);
        canvas.Set("height", 2);
        auto toDataURL = canvas.Get("toDataURL").As<Napi::Function>();
        for (const char* mime : {"", "image/png", "IMAGE/PNG", "image/jpeg"})
        {
            const auto url = toDataURL.Call(canvas, {Napi::String::New(env, mime)}).As<Napi::String>().Utf8Value();
            EXPECT_EQ(url.find("data:image/png;base64,iVBORw0KGgo"), 0u);
            EXPECT_GT(url.size(), 32u);
        }
    });
}
