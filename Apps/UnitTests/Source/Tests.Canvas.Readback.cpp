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
