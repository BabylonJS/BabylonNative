#include <gtest/gtest.h>

#include <Babylon/AppRuntime.h>
#include <Babylon/Graphics/Device.h>
#include <Babylon/Polyfills/XMLHttpRequest.h>
#include <Babylon/Polyfills/Console.h>
#include <Babylon/Polyfills/Window.h>
#include <Babylon/Polyfills/Canvas.h>
#include <Babylon/Polyfills/Blob.h>
#include <Babylon/Plugins/NativeEngine.h>
#include <Babylon/Plugins/NativeEncoding.h>
#ifdef HAS_NATIVE_DRACO
#include <Babylon/Plugins/NativeDraco.h>
#endif
#ifdef HAS_NATIVE_MESHOPT
#include <Babylon/Plugins/NativeMeshopt.h>
#endif
#ifdef HAS_NATIVE_OPTIMIZATIONS
#include <Babylon/Plugins/NativeOptimizations.h>
#endif
#include <Babylon/ScriptLoader.h>
#ifdef HAS_NATIVE_IMAGE_LOADING
#include <UrlLib/UrlLib.h>
#include <gsl/util>
#include <memory>
#include <vector>
#endif

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <future>

extern Babylon::Graphics::Configuration g_deviceConfig;

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

class ScriptLoaderTest : public testing::TestWithParam<bool>
{
protected:
    void Evaluate(Babylon::ScriptLoader& loader, std::string source)
    {
        if (GetParam())
        {
            std::ofstream file{m_path, std::ios::binary};
            ASSERT_TRUE(file.is_open());
            file.write(source.data(), source.size());
            file.close();
            ASSERT_TRUE(file);

            const auto path = m_path.generic_string();
            loader.LoadScript((path.front() == '/' ? "file://" : "file:///") + path);
        }
        else
        {
            loader.Eval(std::move(source), "test.js");
        }
    }

    void TearDown() override
    {
        std::error_code error{};
        std::filesystem::remove(m_path, error);
        EXPECT_FALSE(error) << error.message();
    }

private:
    const std::filesystem::path m_path = std::filesystem::temp_directory_path() / "babylon-native-scriptloader.js";
};

TEST_P(ScriptLoaderTest, RejectsEmbeddedNulBeforeExecution)
{
    struct Failure
    {
        std::string name;
        std::string message;
        bool executed;
    };
    auto done = std::make_shared<std::promise<Failure>>();
    auto completion = done->get_future();
    Babylon::AppRuntime::Options options{};
    options.UnhandledExceptionHandler = [done](const Napi::Error& error) {
        const auto global = error.Env().Global();
        done->set_value({
            error.Value().Get("name").As<Napi::String>().Utf8Value(),
            error.Message(),
            global.Has("beforeNul") || global.Has("afterNul")});
    };

    Babylon::AppRuntime runtime{options};
    Babylon::ScriptLoader loader{runtime};
    static constexpr char source[] = "var beforeNul = 1;//\0\nvar afterNul = 2;";
    Evaluate(loader, std::string{source, sizeof(source) - 1});
    loader.Dispatch([done](Napi::Env env) {
        const auto global = env.Global();
        done->set_value({"", "", global.Has("beforeNul") || global.Has("afterNul")});
    });

    ASSERT_EQ(completion.wait_for(std::chrono::seconds{30}), std::future_status::ready);
    const auto failure = completion.get();
    EXPECT_EQ(failure.name, "TypeError");
    EXPECT_NE(failure.message.find("Script source contains a NUL character"), std::string::npos);
    EXPECT_FALSE(failure.executed);
}

TEST_P(ScriptLoaderTest, PreservesOrdinaryScriptSemantics)
{
    auto done = std::make_shared<std::promise<int32_t>>();
    auto completion = done->get_future();
    Babylon::AppRuntime::Options options{};
    options.UnhandledExceptionHandler = [done](const Napi::Error& error) {
        done->set_exception(std::make_exception_ptr(std::runtime_error{error.Message()}));
    };

    Babylon::AppRuntime runtime{options};
    Babylon::ScriptLoader loader{runtime};
    Evaluate(loader, "let retainedValue = 41;");
    loader.Dispatch([done](Napi::Env env) {
        done->set_value(Napi::Eval(env, "retainedValue + 1", "check.js").As<Napi::Number>().Int32Value());
    });

    ASSERT_EQ(completion.wait_for(std::chrono::seconds{30}), std::future_status::ready);
    EXPECT_EQ(completion.get(), 42);
}

INSTANTIATE_TEST_SUITE_P(JavaScript, ScriptLoaderTest, testing::Values(false, true));

#ifdef HAS_NATIVE_MESHOPT
TEST(NativeMeshopt, LegacyEntryPointMatchesGroupedDecoder)
{
    auto done = std::make_shared<std::promise<void>>();
    auto completion = done->get_future();
    Babylon::AppRuntime runtime{};
    runtime.Dispatch([done](Napi::Env env) {
        try
        {
            Babylon::Plugins::NativeMeshopt::Initialize(env);
            Napi::Eval(env, R"(
                (function () {
                    function bytes(hex) {
                        return new Uint8Array(hex.match(/../g).map(function (byte) { return parseInt(byte, 16); }));
                    }
                    var encoded = bytes(
                        "a00000013ff000007fffa0606001380000007e0000013ff0000020ff9070480130800000800000013ff0000080ff" +
                        "a0606001320000007e012aa000000000000000000000000000000000000000000000000000000000800000000000beadde");
                    var expected = bytes(
                        "00000000000000800000000000beadde0000c03f000010c00000403f01beadde00004040000090c00000c03f02beadde" +
                        "000090400000d8c00000104003beadde0000c040000010c10000404004beadde0000f040000034c10000704005beadde");
                    [
                        { name: "_native.decodeMeshopt", decode: _native.decodeMeshopt },
                        { name: "_native.MeshoptCodec.Decode", decode: _native.MeshoptCodec.Decode }
                    ].forEach(function (entry) {
                        if (typeof entry.decode !== "function") {
                            throw new Error(entry.name + ": missing Meshopt entry point");
                        }
                        var result;
                        try {
                            result = entry.decode(encoded, 6, 16, "ATTRIBUTES");
                        } catch (error) {
                            throw new Error(entry.name + ": " + error.message);
                        }
                        if (!(result instanceof Uint8Array) || result.length !== expected.length ||
                            result.some(function (value, index) { return value !== expected[index]; })) {
                            throw new Error(entry.name + ": expected Uint8Array containing the byte-exact reference output");
                        }
                        var rejected = false;
                        try { entry.decode(encoded, 6, 16, "NOT_A_MODE"); } catch (error) { rejected = true; }
                        if (!rejected) {
                            throw new Error(entry.name + ": accepted an invalid mode");
                        }
                    });
                })();
            )", "meshopt-compatibility.js");
            done->set_value();
        }
        catch (...)
        {
            done->set_exception(std::current_exception());
        }
    });
    const auto status = completion.wait_for(std::chrono::seconds{30});
    EXPECT_EQ(status, std::future_status::ready);
    if (status == std::future_status::ready)
    {
        EXPECT_NO_THROW(completion.get());
    }
}
#endif

TEST(JavaScript, All)
{
    // Change this to true to wait for the JavaScript debugger to attach (only applies to V8)
    constexpr const bool waitForDebugger = false;

#ifdef HAS_NATIVE_IMAGE_LOADING
    const auto unregisterImageTestScheme = gsl::finally([] {
        UrlLib::UrlRequest::UnregisterSchemeResolver("image-reload-test");
    });
#endif

    Babylon::Graphics::Device device{g_deviceConfig};

    // Start rendering a frame to unblock the JavaScript from queuing graphics
    // commands. The frame is held open through script load and the test pump
    // (which only ticks bgfx via Finish; Start) so the JS thread can submit
    // at any time without racing the gate. A final Finish closes it after
    // runtime teardown.
    device.StartRenderingCurrentFrame();

    std::optional<Babylon::Polyfills::Canvas> nativeCanvas;

    Babylon::AppRuntime::Options options{};

    options.UnhandledExceptionHandler = [](const Napi::Error& error) {
        std::cerr << "[Uncaught Error] " << Napi::GetErrorString(error) << std::endl;
        std::quick_exit(1);
    };

    if (waitForDebugger)
    {
        std::cout << "Waiting for debugger..." << std::endl;
        options.WaitForDebugger = true;
    }

    Babylon::AppRuntime runtime{options};

    std::promise<int32_t> exitCodePromise;

    runtime.Dispatch([&exitCodePromise, &device, &nativeCanvas](Napi::Env env) {
        device.AddToJavaScript(env);
#ifdef HAS_NATIVE_IMAGE_LOADING
        env.Global().Set("hasNativeImageLoading", true);
#else
        env.Global().Set("hasNativeImageLoading", false);
#endif
#ifdef USE_NOOP_METAL_DEVICE
        env.Global().Set("hasAttributeLessInstancing", false);
#else
        env.Global().Set("hasAttributeLessInstancing", true);
#endif
#ifdef USE_NOOP_METAL_DEVICE
        env.Global().Set("hasGpuRendering", false);
#else
        env.Global().Set("hasGpuRendering", true);
#endif
#if defined(SKIP_RENDER_TESTS) || defined(SKIP_EXTERNAL_TEXTURE_TESTS)
        env.Global().Set("skipCanvasGpuTests", true);
        env.Global().Set("hasGpuRendering", false);
#else
        env.Global().Set("skipCanvasGpuTests", false);
        env.Global().Set("hasGpuRendering", true);
#endif

        Babylon::Polyfills::XMLHttpRequest::Initialize(env);
        Babylon::Polyfills::Console::Initialize(env, [](const char* message, Babylon::Polyfills::Console::LogLevel logLevel) {
            std::cout << "[" << EnumToString(logLevel) << "] " << message << std::endl;
        });
        Babylon::Polyfills::Window::Initialize(env);
        Babylon::Polyfills::Blob::Initialize(env);
        nativeCanvas.emplace(Babylon::Polyfills::Canvas::Initialize(env));
        Babylon::Plugins::NativeEngine::Initialize(env);
        Babylon::Plugins::NativeEncoding::Initialize(env);
#ifdef HAS_NATIVE_DRACO
        Babylon::Plugins::NativeDraco::Initialize(env);
#endif
#ifdef HAS_NATIVE_MESHOPT
        Babylon::Plugins::NativeMeshopt::Initialize(env);
#endif
#ifdef HAS_NATIVE_OPTIMIZATIONS
        Babylon::Plugins::NativeOptimizations::Initialize(env);
#endif

        auto setExitCodeCallback = Napi::Function::New(
            env, [&exitCodePromise](const Napi::CallbackInfo& info) {
                Napi::Env env = info.Env();
                exitCodePromise.set_value(info[0].As<Napi::Number>().Int32Value());
            },
            "setExitCode");
        env.Global().Set("setExitCode", setExitCodeCallback);
#ifdef HAS_NATIVE_IMAGE_LOADING
        env.Global().Set("setImageReloadTestResponse", Napi::Function::New(env, [](const Napi::CallbackInfo& info) {
            const auto bytes = info[0].As<Napi::Uint8Array>();
            const auto begin = reinterpret_cast<const std::byte*>(bytes.Data());
            auto body = std::make_shared<const std::vector<std::byte>>(begin, begin + bytes.ElementLength());
            UrlLib::UrlRequest::RegisterSchemeResolver("image-reload-test", [body](const std::string&) {
                UrlLib::UrlSchemeResolverResult result;
                result.handled = true;
                result.statusCode = UrlLib::UrlStatusCode::Ok;
                result.contentType = "image/png";
                result.body = body;
                return result;
            });
        }));
#endif
    });

    Babylon::ScriptLoader loader{runtime};
    loader.Eval("location = { href: '' };", ""); // Required for Mocha.js as we do not have a location in Babylon Native
    loader.LoadScript("app:///Assets/babylon.es5.js");
    loader.LoadScript("app:///Assets/babylonjs.materials.es5.js");
    loader.LoadScript("app:///Assets/tests.javaScript.all.js");

    // Pump frames while JS tests run — tests use RAF internally and
    // SubmitCommands requires an active frame. The frame was opened
    // immediately after device creation; the loop just ticks bgfx
    // (Finish; Start) once per iteration so commands can advance.
    auto exitCodeFuture = exitCodePromise.get_future();
    while (exitCodeFuture.wait_for(std::chrono::milliseconds(16)) != std::future_status::ready)
    {
        device.FinishRenderingCurrentFrame();
        device.StartRenderingCurrentFrame();
    }

    auto exitCode = exitCodeFuture.get();
    EXPECT_EQ(exitCode, 0);

    // Runtime destructor joins the JS thread; must happen before Finish.
    nativeCanvas.reset();

    device.FinishRenderingCurrentFrame();
}
