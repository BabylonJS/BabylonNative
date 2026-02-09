#include <gtest/gtest.h>

#include <Babylon/AppRuntime.h>
#include <Babylon/Plugins/NativeWebGPU.h>
#include <Babylon/Polyfills/Window.h>
#include <Babylon/ScriptLoader.h>

#include <atomic>
#include <chrono>
#include <future>
#include <memory>
#include <string>

using namespace std::chrono_literals;

namespace
{
    void RunNativeWebGpuAsyncScript(const char* scriptSource)
    {
        std::promise<std::string> completionPromise{};
        auto completionFlag = std::make_shared<std::atomic_bool>(false);

        Babylon::AppRuntime::Options options{};
        options.UnhandledExceptionHandler = [&completionPromise, completionFlag](const Napi::Error& error) {
            bool expected = false;
            if (completionFlag->compare_exchange_strong(expected, true))
            {
                completionPromise.set_value(Napi::GetErrorString(error));
            }
        };

        Babylon::AppRuntime runtime{options};
        runtime.Dispatch([&completionPromise, completionFlag](Napi::Env env) {
            Babylon::Polyfills::Window::Initialize(env);
            Babylon::Plugins::NativeWebGPU::Initialize(env);

            env.Global().Set("__nativeWebGpuTestDone", Napi::Function::New(env, [&completionPromise, completionFlag](const Napi::CallbackInfo& info) {
                const bool success = info.Length() > 0 && info[0].IsBoolean() && info[0].As<Napi::Boolean>().Value();
                const std::string details = info.Length() > 1 && info[1].IsString() ? info[1].As<Napi::String>().Utf8Value() : std::string{};

                bool expected = false;
                if (completionFlag->compare_exchange_strong(expected, true))
                {
                    completionPromise.set_value(success ? std::string{} : details);
                }
            }));
        });

        Babylon::ScriptLoader loader{runtime};
        loader.Eval(scriptSource, "nativewebgpu.async.bridge.test.js");

        auto completionFuture = completionPromise.get_future();
        ASSERT_EQ(completionFuture.wait_for(30s), std::future_status::ready) << "Async bridge test timed out.";

        const auto errorText = completionFuture.get();
        EXPECT_TRUE(errorText.empty()) << errorText;
    }
}

TEST(NativeWebGPUAsyncBridge, ResolveIsAsynchronous)
{
    RunNativeWebGpuAsyncScript(R"JS(
        (async () => {
            let settledSynchronously = false;
            let stillSynchronous = true;

            const promise = navigator.gpu._testAsyncResolve("bridge-ok").then((value) => {
                if (value !== "bridge-ok") {
                    throw new Error("Unexpected resolve value: " + value);
                }
                settledSynchronously = stillSynchronous;
            });

            stillSynchronous = false;
            await promise;

            if (settledSynchronously) {
                throw new Error("Promise settled synchronously.");
            }

            __nativeWebGpuTestDone(true, "");
        })().catch((error) => {
            __nativeWebGpuTestDone(false, String(error));
        });
    )JS");
}

TEST(NativeWebGPUAsyncBridge, RejectPropagatesExactMessage)
{
    RunNativeWebGpuAsyncScript(R"JS(
        (async () => {
            try {
                await navigator.gpu._testAsyncReject("boom:async-bridge");
                throw new Error("Expected rejection but promise resolved.");
            } catch (error) {
                const message = String(error);
                if (message.indexOf("boom:async-bridge") === -1) {
                    throw new Error("Missing rejection message: " + message);
                }
            }

            __nativeWebGpuTestDone(true, "");
        })().catch((error) => {
            __nativeWebGpuTestDone(false, String(error));
        });
    )JS");
}

TEST(NativeWebGPUAsyncBridge, RejectStackPreservesJavaScriptCallsiteFrames)
{
    RunNativeWebGpuAsyncScript(R"JS(
        async function failingPath() {
            return navigator.gpu._testAsyncReject("boom:stack-fidelity");
        }

        async function callerPath() {
            try {
                await failingPath();
                throw new Error("Expected rejection but promise resolved.");
            } catch (error) {
                const stack = String(error && error.stack ? error.stack : "");
                if (stack.indexOf("boom:stack-fidelity") === -1) {
                    throw new Error("Stack missing error message: " + stack);
                }
                if (stack.indexOf("failingPath") === -1) {
                    throw new Error("Stack missing failingPath frame: " + stack);
                }
                if (stack.indexOf("callerPath") === -1) {
                    throw new Error("Stack missing callerPath frame: " + stack);
                }
            }
        }

        callerPath().then(() => {
            __nativeWebGpuTestDone(true, "");
        }).catch((error) => {
            __nativeWebGpuTestDone(false, String(error));
        });
    )JS");
}

TEST(NativeWebGPUAsyncBridge, RejectCarriesOperationMetadataForTelemetry)
{
    RunNativeWebGpuAsyncScript(R"JS(
        async function callPath() {
            try {
                await navigator.gpu._testAsyncReject("boom:telemetry-fidelity");
                throw new Error("Expected rejection but promise resolved.");
            } catch (error) {
                if (!(error instanceof Error)) {
                    throw new Error("Rejection is not an Error instance.");
                }
                if (error.message !== "boom:telemetry-fidelity") {
                    throw new Error("Unexpected error message: " + error.message);
                }
                if (error.nativeOperation !== "NativeWebGPU._testAsyncReject") {
                    throw new Error("Unexpected nativeOperation metadata: " + String(error.nativeOperation));
                }
                const stack = String(error.stack || "");
                if (stack.indexOf("callPath") === -1) {
                    throw new Error("Stack missing JS callsite: " + stack);
                }
                if (stack.indexOf("[native async] NativeWebGPU._testAsyncReject") === -1) {
                    throw new Error("Stack missing native operation frame: " + stack);
                }
            }
        }

        callPath().then(() => {
            __nativeWebGpuTestDone(true, "");
        }).catch((error) => {
            __nativeWebGpuTestDone(false, String(error));
        });
    )JS");
}

TEST(NativeWebGPUAsyncBridge, ResolveFactoryThrowRejectsPromiseWithOperationMetadata)
{
    RunNativeWebGpuAsyncScript(R"JS(
        (async () => {
            try {
                await navigator.gpu._testAsyncResolveFactoryThrows("boom:resolve-factory");
                throw new Error("Expected rejection but promise resolved.");
            } catch (error) {
                if (!(error instanceof Error)) {
                    throw new Error("Rejection is not an Error instance.");
                }
                if (error.message !== "boom:resolve-factory") {
                    throw new Error("Unexpected rejection message: " + error.message);
                }
                if (error.nativeOperation !== "NativeWebGPU._testAsyncResolveFactoryThrows") {
                    throw new Error("Unexpected nativeOperation metadata: " + String(error.nativeOperation));
                }
            }

            __nativeWebGpuTestDone(true, "");
        })().catch((error) => {
            __nativeWebGpuTestDone(false, String(error));
        });
    )JS");
}

TEST(NativeWebGPUAsyncBridge, CreateRenderPipelineAsyncRejectsForInvalidDescriptor)
{
    RunNativeWebGpuAsyncScript(R"JS(
        (async () => {
            const adapter = await navigator.gpu.requestAdapter();
            if (!adapter) {
                throw new Error("requestAdapter returned null.");
            }

            const device = await adapter.requestDevice();
            if (!device) {
                throw new Error("requestDevice returned null.");
            }

            try {
                await device.createRenderPipelineAsync();
                throw new Error("Expected createRenderPipelineAsync to reject.");
            } catch (error) {
                const message = String(error);
                if (message.indexOf("descriptor") === -1) {
                    throw new Error("Unexpected rejection message: " + message);
                }
            }

            __nativeWebGpuTestDone(true, "");
        })().catch((error) => {
            __nativeWebGpuTestDone(false, String(error));
        });
    )JS");
}

TEST(NativeWebGPUAsyncBridge, SetPipelineWithoutDrawActivatesNativeDrawPath)
{
    RunNativeWebGpuAsyncScript(R"JS(
        (async () => {
            if (typeof navigator.gpu._testResetDebugStats !== "function") {
                throw new Error("Missing _testResetDebugStats test hook.");
            }
            navigator.gpu._testResetDebugStats();

            const adapter = await navigator.gpu.requestAdapter();
            if (!adapter) {
                throw new Error("requestAdapter returned null.");
            }

            const device = await adapter.requestDevice();
            if (!device) {
                throw new Error("requestDevice returned null.");
            }

            const context = navigator.gpu._createCanvasContext();
            context.configure({
                device,
                format: navigator.gpu.getPreferredCanvasFormat(),
                width: 64,
                height: 64
            });

            const before = navigator.gpu._debugStats();
            const encoder = device.createCommandEncoder();
            const pass = encoder.beginRenderPass({
                colorAttachments: [{
                    view: context.getCurrentTexture().createView(),
                    loadOp: "clear",
                    storeOp: "store",
                    clearValue: { r: 0, g: 0, b: 0, a: 1 }
                }]
            });
            pass.setPipeline(device.createRenderPipeline({}));
            pass.end();
            device.queue.submit([encoder.finish()]);

            const after = navigator.gpu._debugStats();
            if (after.drawPathActive !== true) {
                throw new Error("Expected draw path to become active after setPipeline.");
            }
            if (after.drawCallCount !== before.drawCallCount) {
                throw new Error("setPipeline-only pass should not increment drawCallCount.");
            }
            if (after.queueSubmitCount <= before.queueSubmitCount) {
                throw new Error("Expected queue submit count to increment.");
            }

            __nativeWebGpuTestDone(true, "");
        })().catch((error) => {
            __nativeWebGpuTestDone(false, String(error));
        });
    )JS");
}

TEST(NativeWebGPUAsyncBridge, DrawIndirectIncrementsNativeDrawCounters)
{
    RunNativeWebGpuAsyncScript(R"JS(
        (async () => {
            if (typeof navigator.gpu._testResetDebugStats !== "function") {
                throw new Error("Missing _testResetDebugStats test hook.");
            }
            navigator.gpu._testResetDebugStats();

            const adapter = await navigator.gpu.requestAdapter();
            if (!adapter) {
                throw new Error("requestAdapter returned null.");
            }

            const device = await adapter.requestDevice();
            if (!device) {
                throw new Error("requestDevice returned null.");
            }

            const context = navigator.gpu._createCanvasContext();
            context.configure({
                device,
                format: navigator.gpu.getPreferredCanvasFormat(),
                width: 64,
                height: 64
            });

            const before = navigator.gpu._debugStats();
            const encoder = device.createCommandEncoder();
            const pass = encoder.beginRenderPass({
                colorAttachments: [{
                    view: context.getCurrentTexture().createView(),
                    loadOp: "clear",
                    storeOp: "store",
                    clearValue: { r: 0, g: 0, b: 0, a: 1 }
                }]
            });
            pass.setPipeline(device.createRenderPipeline({}));

            const indirectBuffer = device.createBuffer({
                size: 16,
                usage: 1
            });
            pass.drawIndirect(indirectBuffer, 0);
            pass.end();
            device.queue.submit([encoder.finish()]);

            const after = navigator.gpu._debugStats();
            if (after.drawPathActive !== true) {
                throw new Error("Expected draw path to become active after drawIndirect.");
            }
            if (after.drawCallCount <= before.drawCallCount) {
                throw new Error("Expected drawCallCount to increment after drawIndirect.");
            }

            __nativeWebGpuTestDone(true, "");
        })().catch((error) => {
            __nativeWebGpuTestDone(false, String(error));
        });
    )JS");
}
