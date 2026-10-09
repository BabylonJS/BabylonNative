#include <gtest/gtest.h>
#include <gsl/util>

#include <Babylon/AppRuntime.h>
#include <Babylon/Graphics/Device.h>
#include <Babylon/Graphics/DeviceContext.h>
#include <Babylon/Graphics/Texture.h>
#include <Babylon/Polyfills/Canvas.h>
#include <bimg/decode.h>
#include <bx/allocator.h>
#include <bx/error.h>
#include "../../../Polyfills/Canvas/Source/Canvas.h"
#include "../../../Polyfills/Canvas/Source/Context.h"
#include "../../../Polyfills/Canvas/Source/Gradient.h"
#include "../../../Polyfills/Canvas/Source/Image.h"
#include "../../../Polyfills/Canvas/Source/NativeInstanceRegistry.h"
#include "../../../Polyfills/Canvas/Source/Path2D.h"
#include "../../../Polyfills/Canvas/Source/nanovg/nanovg.h"
#include "../../../Polyfills/Canvas/Source/nanovg/nanovg_filterstack.h"
#include <napi/pointer.h>

#include <array>
#include <chrono>
#include <future>
#include <limits>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
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

TEST(CanvasReadback, HostOwnerMayOutliveRuntime)
{
    Babylon::Graphics::Device device{g_deviceConfig};
    std::optional<Babylon::Polyfills::Canvas> canvas;
    {
        Babylon::AppRuntime runtime{};
        std::promise<void> initialized;
        runtime.Dispatch([&](Napi::Env env) {
            device.AddToJavaScript(env);
            canvas.emplace(Babylon::Polyfills::Canvas::Initialize(env));
            initialized.set_value();
        });
        initialized.get_future().get();
    }
    canvas.reset();
}

TEST(CanvasReadback, NativeBrandsRequireTheOriginalReceiver)
{
    RunCanvasTest([](Napi::Env env) {
        using namespace Babylon::Polyfills::Internal;
        const auto native = Babylon::JsRuntime::NativeObject::GetFromJavaScript(env);
        const auto canvas = native.Get("Canvas").As<Napi::Function>().New({});
        const auto image = native.Get("Image").As<Napi::Function>().New({});
        const auto path = native.Get("Path2D").As<Napi::Function>().New({});
        const auto context = canvas.Get("getContext").As<Napi::Function>()
                                 .Call(canvas, {Napi::String::New(env, "2d")}).As<Napi::Object>();
        const auto gradient = context.Get("createLinearGradient").As<Napi::Function>()
                                  .Call(context, {Napi::Number::New(env, 0), Napi::Number::New(env, 0),
                                                     Napi::Number::New(env, 8), Napi::Number::New(env, 8)}).As<Napi::Object>();
        const auto check = [&](const Napi::Object& object, auto unwrap) {
            ASSERT_NE(unwrap(env, object), nullptr);
            auto copied = Napi::Object::New(env);
            copied.Set("__nativeInstance", object.Get("__nativeInstance"));
            EXPECT_EQ(unwrap(env, copied), nullptr);
            const auto objectConstructor = env.Global().Get("Object").As<Napi::Object>();
            const auto inherited = objectConstructor.Get("create").As<Napi::Function>().Call(objectConstructor, {object});
            EXPECT_EQ(unwrap(env, inherited), nullptr);
            EXPECT_NE(unwrap(env, object), nullptr);
        };
        check(canvas, NativeCanvas::TryUnwrap);
        check(image, NativeCanvasImage::TryUnwrap);
        check(path, NativeCanvasPath2D::TryUnwrap);
        check(gradient, CanvasGradient::TryUnwrap);
        EXPECT_EQ(NativeCanvas::TryUnwrap(env, image), nullptr);
        EXPECT_EQ(NativeCanvasImage::TryUnwrap(env, canvas), nullptr);

        // Re-register one native address with a different wrapper, simulating address reuse.
        int token{};
        using Registry = NativeInstanceRegistry<int>;
        const auto remove = gsl::finally([&] { Registry::Remove(&token); });
        const auto registerToken = Napi::Function::New(env, [&](const Napi::CallbackInfo& info) { Registry::Add(info, &token); });
        const auto oldWrapper = Napi::Object::New(env);
        const auto newWrapper = Napi::Object::New(env);
        registerToken.Call(oldWrapper, {});
        EXPECT_EQ(Registry::TryUnwrap(env, oldWrapper), &token);
        Registry::Remove(&token);
        registerToken.Call(newWrapper, {});
        EXPECT_EQ(Registry::TryUnwrap(env, oldWrapper), nullptr);
        EXPECT_EQ(Registry::TryUnwrap(env, newWrapper), &token);
    });
}

TEST(CanvasReadback, ImageCallbacksAreJavaScriptOwned)
{
    RunCanvasTest([](Napi::Env env) {
        const auto constructor = Babylon::JsRuntime::NativeObject::GetFromJavaScript(env).Get("Image").As<Napi::Function>();
        auto image = constructor.New({});
        const auto object = env.Global().Get("Object").As<Napi::Object>();
        const auto descriptor = object.Get("getOwnPropertyDescriptor").As<Napi::Function>();
        const auto callback = Napi::Function::New(env, [](const Napi::CallbackInfo&) {});
        for (const auto* event : {"onload", "onerror"})
        {
            EXPECT_TRUE(image.Get(event).IsNull());
            image.Set(event, callback);
            const auto property = descriptor.Call(object, {image, Napi::String::New(env, event)}).As<Napi::Object>();
            EXPECT_EQ(property.Get("value"), callback);
            image.Set(event, env.Null());
            EXPECT_TRUE(image.Get(event).IsNull());
        }
    });
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

TEST(CanvasReadback, GaussianBlurPadsUniformUploads)
{
    RunCanvasTest([](Napi::Env env) {
        const auto constructor = Babylon::JsRuntime::NativeObject::GetFromJavaScript(env).Get("Canvas").As<Napi::Function>();
        auto canvas = constructor.New({});
        canvas.Get("getContext").As<Napi::Function>().Call(canvas, {Napi::String::New(env, "2d")});
        auto* nativeCanvas = Babylon::Polyfills::Internal::NativeCanvas::Unwrap(canvas);
        nativeCanvas->UpdateRenderTarget();
        auto* frameBuffer = &nativeCanvas->GetFrameBuffer();
        nanovg_filterstack filters;
        filters.ParseString("blur(1px)");
        size_t uploads{};
        filters.Render(BGFX_INVALID_HANDLE,
            [&](bgfx::UniformHandle handle, const void* data, uint16_t count) {
                if (handle.idx != nanovg_filterstack::m_uniforms.u_weights.idx)
                {
                    return;
                }
                ++uploads;
                ASSERT_EQ(count, 5u);
                const auto* weights = static_cast<const float*>(data);
                float sum{};
                for (size_t i = 0; i < 13; ++i)
                {
                    sum += weights[i];
                    EXPECT_FLOAT_EQ(weights[i], weights[12 - i]);
                }
                EXPECT_NEAR(sum, 1.0f, 1e-6f);
                for (size_t i = 13; i < count * 4u; ++i)
                {
                    EXPECT_FLOAT_EQ(weights[i], 0.0f);
                }
            },
            [](bgfx::ProgramHandle, Babylon::Graphics::FrameBuffer*) {},
            [](bgfx::ProgramHandle, Babylon::Graphics::FrameBuffer*, Babylon::Graphics::FrameBuffer*) {},
            [](bgfx::ProgramHandle, Babylon::Graphics::FrameBuffer*, Babylon::Graphics::FrameBuffer*) {},
            frameBuffer,
            [frameBuffer] { return frameBuffer; },
            [](Babylon::Graphics::FrameBuffer*) {});
        EXPECT_EQ(uploads, 2u);
    });
}

#ifdef HAS_NATIVE_IMAGE_LOADING
TEST(CanvasImages, SvgUsesBimgParserAndRgbaPixels)
{
    bx::DefaultAllocator allocator;
    constexpr std::string_view svg{R"(<svg xmlns="http://www.w3.org/2000/svg" width="4" height="2"><rect width="2" height="2" fill="red"/></svg>)"};
    const std::unique_ptr<bimg::ImageContainer, decltype(&bimg::imageFree)> image{
        bimg::imageParse(&allocator, svg.data(), static_cast<uint32_t>(svg.size())), bimg::imageFree};
    ASSERT_NE(image, nullptr);
    EXPECT_EQ(image->m_parser, bimg::ImageParser::Svg);
    EXPECT_EQ(image->m_format, bimg::TextureFormat::RGBA8);
    EXPECT_EQ(image->m_width, 4);
    EXPECT_EQ(image->m_height, 2);
    ASSERT_EQ(image->m_size, 32);
    EXPECT_TRUE(image->m_hasAlpha);
    const auto* pixels = static_cast<const uint8_t*>(image->m_data);
    EXPECT_EQ(pixels[0], 255);
    EXPECT_EQ(pixels[1], 0);
    EXPECT_EQ(pixels[2], 0);
    EXPECT_EQ(pixels[3], 255);
    EXPECT_EQ(pixels[15], 0);
}

TEST(CanvasImages, SvgUsesBimgDimensionPolicy)
{
    bx::DefaultAllocator allocator;
    struct Fixture
    {
        const char* dimensions;
        uint32_t width;
        uint32_t height;
    };
    for (const auto& fixture : std::array<Fixture, 4>{{
             {"width=\"4.25\" height=\"2.25\"", 4, 2},
             {"width=\"4.5\" height=\"2.5\"", 5, 3},
             {"width=\"64.0005\" height=\"64.0007\"", 64, 64},
             {"width=\"8192\" height=\"2\"", 4096, 1},
         }})
    {
        const std::string svg = std::string{"<svg xmlns=\"http://www.w3.org/2000/svg\" "} + fixture.dimensions + "></svg>";
        const std::unique_ptr<bimg::ImageContainer, decltype(&bimg::imageFree)> image{
            bimg::imageParse(&allocator, svg.data(), static_cast<uint32_t>(svg.size())), bimg::imageFree};
        ASSERT_NE(image, nullptr) << fixture.dimensions;
        EXPECT_EQ(image->m_parser, bimg::ImageParser::Svg);
        EXPECT_EQ(image->m_width, fixture.width);
        EXPECT_EQ(image->m_height, fixture.height);
    }
}

TEST(CanvasImages, SvgRejectsInvalidDimensions)
{
    bx::DefaultAllocator allocator;
    bx::Error error;
    constexpr std::string_view svg{R"(<svg xmlns="http://www.w3.org/2000/svg" width="0" height="2"></svg>)"};
    const std::unique_ptr<bimg::ImageContainer, decltype(&bimg::imageFree)> image{
        bimg::imageParse(&allocator, svg.data(), static_cast<uint32_t>(svg.size()), bimg::TextureFormat::Count, &error), bimg::imageFree};
    EXPECT_EQ(image, nullptr);
    EXPECT_FALSE(error.isOk());
}
#endif

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

TEST(CanvasReadback, CanvasTexturePreservesLegacyPremultipliedSource)
{
    RunCanvasTest([](Napi::Env env) {
        const auto constructor = Babylon::JsRuntime::NativeObject::GetFromJavaScript(env).Get("Canvas").As<Napi::Function>();
        auto canvas = constructor.New({});
        canvas.Set("width", 4);
        canvas.Set("height", 4);
        auto context = canvas.Get("getContext").As<Napi::Function>().Call(canvas, {Napi::String::New(env, "2d")}).As<Napi::Object>();
        auto getTexture = canvas.Get("getCanvasTexture").As<Napi::Function>();
        EXPECT_THROW(getTexture.Call(canvas, {}), Napi::Error);
        context.Get("flush").As<Napi::Function>().Call(context, {});
        auto* nativeCanvas = Babylon::Polyfills::Internal::NativeCanvas::Unwrap(canvas);
        const auto source = bgfx::getTexture(nativeCanvas->GetFrameBuffer().Handle());
        for (const std::vector<Napi::Value>& arguments : std::vector<std::vector<Napi::Value>>{
                 {}, {env.Undefined()}, {Napi::Boolean::New(env, true)}})
        {
            auto* texture = getTexture.Call(canvas, arguments).As<Napi::Pointer<Babylon::Graphics::Texture>>().Get();
            EXPECT_EQ(texture->Handle().idx, source.idx);
        }
        auto* straight = getTexture.Call(canvas, {Napi::Boolean::New(env, false)}).As<Napi::Pointer<Babylon::Graphics::Texture>>().Get();
        EXPECT_NE(straight->Handle().idx, source.idx);
        EXPECT_EQ(straight->Width(), 4u);
        EXPECT_EQ(straight->Height(), 4u);
        EXPECT_EQ(bgfx::getTexture(nativeCanvas->GetFrameBuffer().Handle()).idx, source.idx);
    });
}

TEST(CanvasReadback, QueuedRepresentationsPreservePixelsAndMips)
{
#if defined(USE_NOOP_METAL_DEVICE) || defined(SKIP_RENDER_TESTS)
    GTEST_SKIP();
#else
    RunCanvasTest([](Napi::Env env) {
        auto canvas = Babylon::JsRuntime::NativeObject::GetFromJavaScript(env).Get("Canvas").As<Napi::Function>().New({});
        canvas.Set("width", 4);
        canvas.Set("height", 4);
        auto context = canvas.Get("getContext").As<Napi::Function>().Call(canvas, {Napi::String::New(env, "2d")}).As<Napi::Object>();
        context.Set("fillStyle", "rgba(200, 100, 50, 0.5)");
        context.Get("fillRect").As<Napi::Function>().Call(context, {
            Napi::Number::New(env, 0), Napi::Number::New(env, 0), Napi::Number::New(env, 4), Napi::Number::New(env, 4)});
        context.Get("flush").As<Napi::Function>().Call(context, {});
        auto& graphics = Babylon::Graphics::DeviceContext::GetFromJavaScript(env);
        auto pixels = std::make_shared<std::array<uint8_t, 16 * 4 * 4>>();
        auto completed = std::make_shared<std::promise<void>>();
        auto future = completed->get_future();
        {
            auto frameScope = graphics.AcquireFrameCompletionScope();
            const auto getTexture = canvas.Get("getCanvasTexture").As<Napi::Function>();
            std::array<Babylon::Graphics::Texture*, 3> textures{};
            for (size_t i = 0; i < textures.size(); ++i)
            {
                textures[i] = getTexture.Call(canvas, {Napi::Boolean::New(env, i == 2), Napi::Boolean::New(env, i != 0)})
                                  .As<Napi::Pointer<Babylon::Graphics::Texture>>().Get();
            }
            auto readback = std::make_shared<Babylon::Graphics::Texture>(graphics);
            readback->Create2D(16, 4, false, 1, bgfx::TextureFormat::RGBA8, BGFX_TEXTURE_BLIT_DST | BGFX_TEXTURE_READ_BACK);
            for (uint16_t i = 0; i < 5; ++i)
            {
                const uint8_t mip = i < 3 ? 0 : 1;
                const uint16_t size = 4 >> mip;
                const uint16_t x = i < 3 ? i * 4 : 12 + (i - 3) * 2;
                bgfx::TextureRegion source{};
                source.init(textures[i < 3 ? i : i - 2]->Handle(), 0, 0, size, size);
                source.mip = mip;
                bgfx::TextureRegion destination{};
                destination.init(readback->Handle(), x, 0, size, size);
                graphics.GetActiveEncoder()->blit(graphics.AcquireNewViewId(), destination, source);
            }
            graphics.ReadTextureAsync(readback->Handle(), gsl::make_span(*pixels))
                .then(arcana::inline_scheduler, arcana::cancellation::none(),
                    [readback, pixels, completed](arcana::expected<void, std::exception_ptr> result) {
                        readback->Dispose();
                        if (result.has_error())
                        {
                            completed->set_exception(result.error());
                        }
                        else
                        {
                            completed->set_value();
                        }
                    });
        }
        ASSERT_EQ(future.wait_for(std::chrono::seconds{30}), std::future_status::ready);
        ASSERT_NO_THROW(future.get());
        for (size_t i = 0; i < 5; ++i)
        {
            SCOPED_TRACE(i);
            const size_t size = i < 3 ? 4 : 2;
            const size_t offsetX = i < 3 ? i * 4 : 12 + (i - 3) * 2;
            const bool premultiplied = i == 2 || i == 4;
            for (size_t y = 0; y < size; ++y)
            {
                for (size_t x = 0; x < size; ++x)
                {
                    const size_t offset = (y * 16 + offsetX + x) * 4;
                    EXPECT_NEAR((*pixels)[offset], premultiplied ? 100 : 200, 2);
                    EXPECT_NEAR((*pixels)[offset + 1], premultiplied ? 50 : 100, 2);
                    EXPECT_NEAR((*pixels)[offset + 2], premultiplied ? 25 : 50, 2);
                    EXPECT_NEAR((*pixels)[offset + 3], 128, 1);
                }
            }
        }
    });
#endif
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
