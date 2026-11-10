#ifdef HAS_SHADER_COMPILER
#include <gtest/gtest.h>
#include "StorageBuffer.h"
#include "VertexBuffer.h"

#include <Babylon/AppRuntime.h>
#include <Babylon/Graphics/Device.h>
#include <Babylon/Graphics/DeviceContext.h>
#include <Babylon/Plugins/NativeEngine.h>
#include <napi/pointer.h>

#include <chrono>
#include <cstdlib>
#include <future>
#include <iostream>
#include <string>

extern Babylon::Graphics::Configuration g_deviceConfig;

namespace
{
    bool RendererSupportsCompute()
    {
        Babylon::Graphics::Device device{g_deviceConfig};
        device.StartRenderingCurrentFrame();
        const bool supported = (bgfx::getCaps()->supported & BGFX_CAPS_COMPUTE) != 0;
        device.FinishRenderingCurrentFrame();
        return supported;
    }
}

// WebGL reads transform feedback results back with a synchronous getBufferSubData. The native engine
// mirrors that with a compute copy plus mid-frame flushes, so the call must return the GPU contents.
TEST(NativeEngineTransformFeedback, ReadTransformFeedbackBufferIsSynchronous)
{
#if defined(USE_NOOP_METAL_DEVICE) || defined(SKIP_RENDER_TESTS)
    GTEST_SKIP();
#else
    if (!RendererSupportsCompute())
    {
        GTEST_SKIP() << "Renderer does not support compute";
    }
    constexpr uint32_t kCount = 300;
    Babylon::Graphics::Device device{g_deviceConfig};
    device.StartRenderingCurrentFrame();
    std::promise<std::string> completed;
    auto future = completed.get_future();
    // Frames keep cycling only after the JavaScript holds a frame completion scope, so its calls run in an active frame.
    std::promise<void> scoped;
    auto scopedFuture = scoped.get_future();
    {
        Babylon::AppRuntime runtime{};
        runtime.Dispatch([&](Napi::Env env) {
            try
            {
                device.AddToJavaScript(env);
                Babylon::Plugins::NativeEngine::Initialize(env);
                auto& context = Babylon::Graphics::DeviceContext::GetFromJavaScript(env);
                auto scope = context.AcquireFrameCompletionScope();
                scoped.set_value();
                auto engine = env.Global().Get("_native").As<Napi::Object>().Get("Engine").As<Napi::Function>().New({});
                auto result = Napi::Eval(env, R"(
                    (function(engine, count) {
                        const data = new Float32Array(count);
                        for (let i = 0; i < count; ++i) { data[i] = i * 0.5 - 7; }
                        const buffer = engine.createVertexBuffer(data.buffer, 0, data.byteLength, false);
                        const target = new Float32Array(count + 2).fill(-1);
                        engine.readTransformFeedbackBuffer(buffer, target.buffer, 4, count * 4);
                        if (target[0] !== -1 || target[count + 1] !== -1) { return "wrote outside the target range"; }
                        // Byte-component data (e.g. three normalized unsigned bytes) is not float-sized.
                        const bytes = new Uint8Array([1, 2, 3]);
                        engine.readTransformFeedbackBuffer(engine.createVertexBuffer(bytes.buffer, 0, 3, false), target.buffer, 0, 0);
                        for (let i = 0; i < count; ++i) {
                            if (target[i + 1] !== data[i]) { return "value " + i + " is " + target[i + 1]; }
                        }
                        return "";
                    })
                )", "read-transform-feedback.js").As<Napi::Function>().Call({engine, Napi::Number::New(env, kCount)});
                completed.set_value(result.As<Napi::String>().Utf8Value());
            }
            catch (const std::exception& ex)
            {
                completed.set_value(ex.what());
            }
        });
        while (scopedFuture.wait_for(std::chrono::milliseconds{1}) != std::future_status::ready && future.wait_for(std::chrono::seconds{0}) != std::future_status::ready)
        {
        }
        const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds{30};
        while (future.wait_for(std::chrono::milliseconds{1}) != std::future_status::ready)
        {
            if (std::chrono::steady_clock::now() >= deadline)
            {
                std::cerr << "Timed out waiting for readTransformFeedbackBuffer" << std::endl;
                std::quick_exit(1);
            }
            device.FinishRenderingCurrentFrame();
            device.StartRenderingCurrentFrame();
        }
        EXPECT_EQ(future.get(), "");
    }
    device.FinishRenderingCurrentFrame();
#endif
}
// A transform feedback program captures its varyings for every vertex through the emulating compute shader.
TEST(NativeEngineTransformFeedback, CapturesVaryingsFromVertexInputs)
{
#if defined(USE_NOOP_METAL_DEVICE) || defined(SKIP_RENDER_TESTS)
    GTEST_SKIP();
#else
    if (!RendererSupportsCompute())
    {
        GTEST_SKIP() << "Renderer does not support compute";
    }
    constexpr uint32_t kCount = 100;
    Babylon::Graphics::Device device{g_deviceConfig};
    device.StartRenderingCurrentFrame();
    std::promise<std::string> completed;
    auto future = completed.get_future();
    {
        Babylon::AppRuntime runtime{};
        runtime.Dispatch([&](Napi::Env env) {
            try
            {
                device.AddToJavaScript(env);
                Babylon::Plugins::NativeEngine::Initialize(env);
                auto& context = Babylon::Graphics::DeviceContext::GetFromJavaScript(env);
                auto scope = context.AcquireFrameCompletionScope();
                auto engine = env.Global().Get("_native").As<Napi::Object>().Get("Engine").As<Napi::Function>().New({});
                auto result = Napi::Eval(env, R"(
                    (function(engine, count, floatType) {
                        const program = engine.createProgram(
                            "#version 300 es\nprecision highp float;\nin vec2 position;\nin float weight;\n" +
                            "out vec2 doubled;\nout float sum;\n" +
                            "void main() { doubled = position * 2.0; sum = position.x + position.y + weight; gl_Position = vec4(0.0); }\n",
                            "", ["sum", "doubled"]);
                        const [position, weight] = engine.getAttributes(program, ["position", "weight"]);
                        if (position < 0 || weight < 0) { return "missing input locations " + position + ", " + weight; }

                        const input = new Float32Array(count * 3);
                        for (let i = 0; i < count; ++i) { input.set([i, i * 0.25 - 3, i % 7], i * 3); }
                        const inputBuffer = engine.createVertexBuffer(input.buffer, 0, input.byteLength, false);
                        const output = new Float32Array(count * 3);
                        const outputBuffer = engine.createVertexBuffer(output.buffer, 0, output.byteLength, false);
                        const vertexArray = engine.createVertexArray();
                        engine.recordVertexBuffer(vertexArray, inputBuffer, position, 0, 12, 2, floatType, false, 0);
                        engine.recordVertexBuffer(vertexArray, inputBuffer, weight, 8, 12, 1, floatType, false, 0);

                        const stream = new _native.NativeDataStream(function() {});
                        engine.setCommandDataStream({_nativeDataStream: stream});
                        const words = new Uint32Array([
                            ..._native.Engine.COMMAND_SETPROGRAM, ...program,
                            ..._native.Engine.COMMAND_BINDVERTEXARRAY, ...vertexArray,
                            ..._native.Engine.COMMAND_DRAWTRANSFORMFEEDBACK, ...outputBuffer, 0, count,
                        ]);
                        stream.writeBuffer(words.buffer, words.length);
                        engine.readTransformFeedbackBuffer(outputBuffer, output.buffer, 0, output.byteLength);
                        for (let i = 0; i < count; ++i) {
                            const x = input[i * 3], y = input[i * 3 + 1], w = input[i * 3 + 2];
                            const expected = [x + y + w, x * 2, y * 2];
                            for (let c = 0; c < 3; ++c) {
                                if (output[i * 3 + c] !== expected[c]) {
                                    return "vertex " + i + " component " + c + " is " + output[i * 3 + c] + ", expected " + expected[c];
                                }
                            }
                        }
                        return "";
                    })
                )", "transform-feedback-capture.js").As<Napi::Function>().Call({engine, Napi::Number::New(env, kCount),
                    Napi::Number::New(env, static_cast<double>(bgfx::AttribType::Float))});
                completed.set_value(result.As<Napi::String>().Utf8Value());
            }
            catch (const std::exception& ex)
            {
                completed.set_value(ex.what());
            }
        });
        const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds{30};
        while (future.wait_for(std::chrono::milliseconds{1}) != std::future_status::ready)
        {
            if (std::chrono::steady_clock::now() >= deadline)
            {
                std::cerr << "Timed out waiting for transform feedback capture" << std::endl;
                std::quick_exit(1);
            }
            device.FinishRenderingCurrentFrame();
            device.StartRenderingCurrentFrame();
        }
        EXPECT_EQ(future.get(), "");
    }
    device.FinishRenderingCurrentFrame();
#endif
}

// A buffer read by transform feedback with one stride and then drawn with another is drawn with the
// draw stride, keeping the contents the GPU holds.
TEST(NativeEngineTransformFeedback, DrawsInputWithDifferentStride)
{
#if defined(USE_NOOP_METAL_DEVICE) || defined(SKIP_RENDER_TESTS)
    GTEST_SKIP();
#else
    if (!RendererSupportsCompute())
    {
        GTEST_SKIP() << "Renderer does not support compute";
    }
    constexpr uint32_t kCount = 12;
    Babylon::Graphics::Device device{g_deviceConfig};
    device.StartRenderingCurrentFrame();
    std::promise<std::string> completed;
    auto future = completed.get_future();
    {
        Babylon::AppRuntime runtime{};
        runtime.Dispatch([&](Napi::Env env) {
            try
            {
                device.AddToJavaScript(env);
                Babylon::Plugins::NativeEngine::Initialize(env);
                auto& context = Babylon::Graphics::DeviceContext::GetFromJavaScript(env);
                auto scope = context.AcquireFrameCompletionScope();
                auto engine = env.Global().Get("_native").As<Napi::Object>().Get("Engine").As<Napi::Function>().New({});
                auto result = Napi::Eval(env, R"(
                    (function(engine, count, floatType) {
                        const stream = new _native.NativeDataStream(function() {});
                        engine.setCommandDataStream({_nativeDataStream: stream});
                        const submit = (words) => stream.writeBuffer(new Uint32Array(words).buffer, words.length);

                        const feedback = engine.createProgram(
                            "#version 300 es\nprecision highp float;\nin vec3 position;\nout float sum;\n" +
                            "void main() { sum = position.x + position.y + position.z; gl_Position = vec4(0.0); }\n",
                            "", ["sum"]);
                        const [feedbackPosition] = engine.getAttributes(feedback, ["position"]);
                        const input = new Float32Array(count);
                        for (let i = 0; i < count; ++i) { input[i] = i * 1.5 - 4; }
                        const inputBuffer = engine.createVertexBuffer(input.buffer, 0, input.byteLength, false);
                        const output = new Float32Array(count / 3);
                        const outputBuffer = engine.createVertexBuffer(output.buffer, 0, output.byteLength, false);
                        const feedbackArray = engine.createVertexArray();
                        engine.recordVertexBuffer(feedbackArray, inputBuffer, feedbackPosition, 0, 12, 3, floatType, false, 0);
                        submit([
                            ..._native.Engine.COMMAND_SETPROGRAM, ...feedback,
                            ..._native.Engine.COMMAND_BINDVERTEXARRAY, ...feedbackArray,
                            ..._native.Engine.COMMAND_DRAWTRANSFORMFEEDBACK, ...outputBuffer, 0, count / 3,
                        ]);
                        engine.readTransformFeedbackBuffer(outputBuffer, output.buffer, 0, output.byteLength);

                        const draw = engine.createProgram(
                            "#version 300 es\nprecision highp float;\nin float value;\n" +
                            "void main() { gl_Position = vec4(value, 0.0, 0.0, 1.0); gl_PointSize = 1.0; }\n",
                            "#version 300 es\nprecision highp float;\nout vec4 color;\nvoid main() { color = vec4(1.0); }\n");
                        const [drawValue] = engine.getAttributes(draw, ["value"]);
                        const drawArray = engine.createVertexArray();
                        engine.recordVertexBuffer(drawArray, inputBuffer, drawValue, 0, 4, 1, floatType, false, 0);
                        submit([
                            ..._native.Engine.COMMAND_SETPROGRAM, ...draw,
                            ..._native.Engine.COMMAND_BINDVERTEXARRAY, ...drawArray,
                            ..._native.Engine.COMMAND_DRAW, 3, 0, count,
                        ]);

                        const contents = new Float32Array(count);
                        engine.readTransformFeedbackBuffer(inputBuffer, contents.buffer, 0, contents.byteLength);
                        for (let i = 0; i < count; ++i) {
                            if (contents[i] !== input[i]) { return ["value " + i + " is " + contents[i], inputBuffer]; }
                        }
                        return ["", inputBuffer];
                    })
                )", "transform-feedback-restride.js").As<Napi::Function>().Call({engine, Napi::Number::New(env, kCount),
                    Napi::Number::New(env, static_cast<double>(bgfx::AttribType::Float))}).As<Napi::Array>();
                std::string message = result.Get(0u).As<Napi::String>().Utf8Value();
                const auto* storage = result.Get(1u).As<Napi::Pointer<Babylon::VertexBuffer>>().Get()->GpuStorage();
                if (message.empty() && (storage == nullptr || storage->ByteStride() != 4))
                {
                    message = "storage stride is " + std::to_string(storage == nullptr ? 0 : storage->ByteStride()) + ", expected 4";
                }
                completed.set_value(message);
            }
            catch (const std::exception& ex)
            {
                completed.set_value(ex.what());
            }
        });
        const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds{30};
        while (future.wait_for(std::chrono::milliseconds{1}) != std::future_status::ready)
        {
            if (std::chrono::steady_clock::now() >= deadline)
            {
                std::cerr << "Timed out waiting for transform feedback restride" << std::endl;
                std::quick_exit(1);
            }
            device.FinishRenderingCurrentFrame();
            device.StartRenderingCurrentFrame();
        }
        EXPECT_EQ(future.get(), "");
    }
    device.FinishRenderingCurrentFrame();
#endif
}
#endif
