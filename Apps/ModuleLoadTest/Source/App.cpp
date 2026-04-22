#include "App.h"

#include <Babylon/AppRuntime.h>
#include <Babylon/Graphics/Device.h>
#include <Babylon/Plugins/NativeEncoding.h>
#include <Babylon/Plugins/NativeEngine.h>
#include <Babylon/Polyfills/Blob.h>
#include <Babylon/Polyfills/Canvas.h>
#include <Babylon/Polyfills/Console.h>
#include <Babylon/Polyfills/Window.h>
#include <Babylon/Polyfills/XMLHttpRequest.h>

#include <algorithm>
#include <future>
#include <iostream>
#include <iterator>
#include <optional>

namespace ModuleLoadTest
{
    bool ShouldSkipEnvironment()
    {
#if !defined(NDEBUG)
        // Debug builds load a different set of modules than optimized builds
        // (debug CRT, heavier diagnostic DLLs, etc). The platform golden lists
        // target Release/RelWithDebInfo. Rather than produce a confusing FAIL,
        // make the skip explicit. CI runs only in optimized configs (see
        // CMakeLists.txt's `add_test ... CONFIGURATIONS Release RelWithDebInfo`).
        std::cout << "ModuleLoadTest: SKIP - Debug config is not supported. "
                     "Build with Release or RelWithDebInfo." << std::endl;
        return true;
#else
        // Running under a debugger injects additional modules (and changes
        // some timing) that would cause spurious FAIL diagnostics. CI runs
        // headless from the CLI, so this only affects local debugging.
        if (IsBeingTraced())
        {
            std::cout << "ModuleLoadTest: SKIP - running under a debugger." << std::endl;
            return true;
        }
        return false;
#endif
    }

    ModuleSnapshot RunBoot(const Babylon::Graphics::Configuration& config)
    {
        // Bring up the real graphics device. This triggers the backend
        // (D3D11/D3D12/Metal/OpenGL) and GPU ICD DLLs/dylibs/sos to load.
        Babylon::Graphics::Device device{config};
        device.StartRenderingCurrentFrame();

        std::optional<Babylon::Polyfills::Canvas> nativeCanvas;

        Babylon::AppRuntime::Options options{};
        options.UnhandledExceptionHandler = [](const Napi::Error& error) {
            std::cerr << "[Uncaught Error] " << Napi::GetErrorString(error) << std::endl;
            std::quick_exit(1);
        };

        Babylon::AppRuntime runtime{options};

        std::promise<void> initDone;

        runtime.Dispatch([&device, &nativeCanvas, &initDone](Napi::Env env) {
            device.AddToJavaScript(env);

            Babylon::Polyfills::XMLHttpRequest::Initialize(env);
            Babylon::Polyfills::Console::Initialize(env, [](const char* message, Babylon::Polyfills::Console::LogLevel) {
                std::cout << message << std::endl;
            });
            Babylon::Polyfills::Window::Initialize(env);
            Babylon::Polyfills::Blob::Initialize(env);
            nativeCanvas.emplace(Babylon::Polyfills::Canvas::Initialize(env));
            Babylon::Plugins::NativeEngine::Initialize(env);
            Babylon::Plugins::NativeEncoding::Initialize(env);

            initDone.set_value();
        });

        initDone.get_future().get();

        device.FinishRenderingCurrentFrame();

        // Snapshot immediately after the first frame completes. At this point
        // the graphics backend and every polyfill/plugin has been initialized,
        // but no user JS has executed.
        return CaptureSnapshot();
    }

    ModuleSnapshot Subtract(const ModuleSnapshot& lhs, const ModuleSnapshot& rhs)
    {
        ModuleSnapshot result;
        std::set_difference(lhs.begin(), lhs.end(), rhs.begin(), rhs.end(),
            std::inserter(result, result.end()));
        return result;
    }

    void PrintList(const char* label, const ModuleSnapshot& items)
    {
        std::cout << label << " (" << items.size() << "):" << std::endl;
        for (const auto& item : items)
        {
            std::cout << "  " << item << std::endl;
        }
    }

    int CompareAndReport(const ModuleSnapshot& postBoot)
    {
        const ModuleSnapshot& baseline = GetPreInitBaseline();

        if (baseline.empty())
        {
            // The baseline is captured by a platform pre-static-init hook
            // (TLS callback on Windows, __attribute__((constructor)) on
            // Linux/macOS). If it's empty the hook did not run, which means
            // the entire delta is "new" modules relative to an empty set,
            // and IsAllowedOptionalModule would silently filter most of
            // them and report a spurious PASS. Fail loudly instead.
            std::cerr << "ModuleLoadTest FAIL: pre-init baseline was not captured. "
                         "The platform pre-init hook did not run." << std::endl;
            return 1;
        }

        // Delta = what boot caused to load. Filter out GPU ICD and
        // launch-environment noise (see platform IsAllowedOptionalModule).
        ModuleSnapshot delta = Subtract(postBoot, baseline);
        for (auto it = delta.begin(); it != delta.end();)
        {
            if (IsAllowedOptionalModule(*it))
            {
                it = delta.erase(it);
            }
            else
            {
                ++it;
            }
        }

        const ModuleSnapshot& expected = GetExpectedBootModules();

        // Only fail on NEW modules. Missing-from-delta is environmental variance
        // (GPU SKU, OS patch, launch env, config) and is not a regression.
        const ModuleSnapshot unexpected = Subtract(delta, expected);

        // Always print both snapshots so CI logs can be used to extend the
        // golden list. The actual pass/fail is decided by the `unexpected`
        // diff below.
        PrintList("Baseline (modules loaded before C++ static init)", baseline);
        PrintList("Delta (modules loaded during BN boot)", delta);

        if (!unexpected.empty())
        {
            std::cout << std::endl;
            std::cout << "FAIL: ModuleLoadTest detected unexpected modules loaded on boot." << std::endl;
            PrintList("Unexpected new modules", unexpected);
            std::cout << std::endl;
            std::cout << "If these are intentional, add them to GetExpectedBootModules() "
                         "in the platform-specific App source." << std::endl;
            return 1;
        }

        std::cout << "ModuleLoadTest: PASS" << std::endl;
        return 0;
    }
}

int main(int /*argc*/, char* /*argv*/[])
{
    if (ModuleLoadTest::ShouldSkipEnvironment())
    {
        return 0;
    }

    auto config = ModuleLoadTest::CreateGraphicsConfig();
    if (!config)
    {
        return 0;
    }

    return ModuleLoadTest::CompareAndReport(ModuleLoadTest::RunBoot(*config));
}
