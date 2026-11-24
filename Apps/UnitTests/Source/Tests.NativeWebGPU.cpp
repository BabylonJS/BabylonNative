#include <gtest/gtest.h>

#include <Babylon/AppRuntime.h>
#include <Babylon/Graphics/Device.h>
#include <Babylon/Plugins/NativeWebGPU.h>

#include <chrono>
#include <future>
#include <string>

extern Babylon::Graphics::Configuration g_deviceConfig;

namespace
{
    void RunWebGPUTest(const char* script)
    {
        Babylon::AppRuntime runtime{};
        std::promise<std::string> completed;
        auto future = completed.get_future();
        runtime.Dispatch([&](Napi::Env env) {
            try
            {
                Babylon::Plugins::NativeWebGPU::Initialize(env, g_deviceConfig.Window, 16, 16);
                env.Global().Set("_testDone", Napi::Function::New(env, [&](const Napi::CallbackInfo& info) {
                    completed.set_value(info[0].ToString().Utf8Value());
                }));
                env.RunScript(std::string{"(async function() {"} + script +
                    "})().then(function() {_testDone('');}, function(e) {_testDone(String(e.stack || e));});");
            }
            catch (const std::exception& ex)
            {
                completed.set_value(ex.what());
            }
        });
        const auto status = future.wait_for(std::chrono::seconds{60});
        EXPECT_EQ(status, std::future_status::ready);
        const auto error = status == std::future_status::ready ? future.get() : "WebGPU test timed out";
        std::promise<void> disposed;
        runtime.Dispatch([&](Napi::Env env) {
            Babylon::Plugins::NativeWebGPU::Deinitialize(env);
            disposed.set_value();
        });
        disposed.get_future().get();
        EXPECT_EQ(error, "");
    }
}

TEST(NativeWebGPU, CanvasReadbackPreservesStateAcrossRuntimeLifetimes)
{
    for (int lifetime = 0; lifetime < 2; ++lifetime)
    {
        RunWebGPUTest(R"JS(
            const canvas = new _native.Canvas();
            canvas.width = 32;
            canvas.height = 16;
            const ctx = canvas.getContext("2d");
            ctx.translate(10, 0);
            ctx.fillStyle = "red";
            ctx.save();
            ctx.translate(8, 0);
            ctx.fillStyle = "blue";
            ctx.getImageData(0, 0, 32, 16);
            ctx.fillRect(0, 0, 4, 4);
            ctx.restore();
            ctx.fillRect(0, 8, 4, 4);
            const pixels = ctx.getImageData(0, 0, 32, 16).data;
            function check(x, y, r, g, b, a) {
                const i = (y * 32 + x) * 4;
                const expected = [r, g, b, a];
                for (let c = 0; c < 4; ++c) {
                    if (pixels[i + c] !== expected[c]) {
                        throw new Error("Wrong pixel at " + x + "," + y + ": " +
                            Array.from(pixels.slice(i, i + 4)));
                    }
                }
            }
            check(19, 1, 0, 0, 255, 255);
            check(11, 9, 255, 0, 0, 255);
            check(1, 1, 0, 0, 0, 0);
            ctx.dispose();
            canvas.dispose();
            if (TestUtils.getUncapturedErrorCount()) throw new Error("Canvas GPU error");
        )JS");
    }
}

TEST(NativeWebGPU, ExternalImagePacksRedAndRgChannels)
{
    RunWebGPUTest(R"JS(
        const device = await (await navigator.gpu.requestAdapter()).requestDevice();
        const source = {width: 2, height: 2, __pixels: new Uint8Array([
            11, 21, 31, 255, 41, 51, 61, 255,
            71, 81, 91, 255, 101, 111, 121, 255
        ])};
        for (const channels of [1, 2]) {
            for (const flipY of [false, true]) {
                const texture = device.createTexture({
                    size: [2, 2], format: channels === 1 ? "r8unorm" : "rg8unorm",
                    usage: GPUTextureUsage.COPY_DST | GPUTextureUsage.COPY_SRC
                });
                device.queue.copyExternalImageToTexture({source, flipY}, {texture}, [2, 2]);
                const buffer = device.createBuffer({size: 512, usage: GPUBufferUsage.COPY_DST | GPUBufferUsage.MAP_READ});
                const encoder = device.createCommandEncoder();
                encoder.copyTextureToBuffer({texture}, {buffer, bytesPerRow: 256}, [2, 2]);
                device.queue.submit([encoder.finish()]);
                await buffer.mapAsync(GPUMapMode.READ);
                const data = new Uint8Array(buffer.getMappedRange());
                for (let y = 0; y < 2; ++y) {
                    for (let x = 0; x < 2; ++x) {
                        for (let c = 0; c < channels; ++c) {
                            const expected = source.__pixels[((flipY ? 1 - y : y) * 2 + x) * 4 + c];
                            if (data[y * 256 + x * channels + c] !== expected) {
                                throw new Error("Incorrect packed channel for " + channels + " channels");
                            }
                        }
                    }
                }
                buffer.unmap();
                buffer.destroy();
                texture.destroy();
            }
        }
        if (TestUtils.getUncapturedErrorCount()) throw new Error("Image upload GPU error");
    )JS");
}

TEST(NativeWebGPU, RenderAndComputePassQueriesAreForwarded)
{
    RunWebGPUTest(R"JS(
        const device = await (await navigator.gpu.requestAdapter()).requestDevice();
        const timestamps = device.features.has("timestamp-query")
            ? device.createQuerySet({type: "timestamp", count: 4}) : null;
        const occlusion = device.createQuerySet({type: "occlusion", count: 1});
        const texture = device.createTexture({
            size: [4, 4], format: "rgba8unorm", usage: GPUTextureUsage.RENDER_ATTACHMENT
        });
        const encoder = device.createCommandEncoder();
        for (let i = 0; i < 2; ++i) {
            const writes = timestamps ? {querySet: timestamps} : undefined;
            if (writes) writes[i ? "endOfPassWriteIndex" : "beginningOfPassWriteIndex"] = i;
            encoder.beginComputePass({timestampWrites: writes}).end();
            if (writes) {
                delete writes[i ? "endOfPassWriteIndex" : "beginningOfPassWriteIndex"];
                writes[i ? "beginningOfPassWriteIndex" : "endOfPassWriteIndex"] = i + 2;
            }
            const pass = encoder.beginRenderPass({
                colorAttachments: [{view: texture.createView(), loadOp: "clear", storeOp: "store"}],
                occlusionQuerySet: occlusion,
                timestampWrites: writes
            });
            pass.beginOcclusionQuery(0);
            pass.endOcclusionQuery();
            pass.end();
        }
        const buffer = device.createBuffer({size: 512, usage: GPUBufferUsage.QUERY_RESOLVE | GPUBufferUsage.COPY_SRC});
        encoder.resolveQuerySet(occlusion, 0, 1, buffer, 0);
        if (timestamps) encoder.resolveQuerySet(timestamps, 0, 4, buffer, 256);
        const readback = device.createBuffer({size: 512, usage: GPUBufferUsage.MAP_READ | GPUBufferUsage.COPY_DST});
        encoder.copyBufferToBuffer(buffer, 0, readback, 0, 512);
        device.queue.submit([encoder.finish()]);
        await readback.mapAsync(GPUMapMode.READ);
        if (timestamps) {
            const data = new Uint32Array(readback.getMappedRange());
            for (let i = 0; i < 4; ++i) {
                if (!data[64 + i * 2] && !data[65 + i * 2]) {
                    throw new Error("Missing timestamp for query " + i);
                }
            }
        }
        readback.unmap();
        readback.destroy();
        if (TestUtils.getUncapturedErrorCount()) throw new Error("Pass query GPU error");
        buffer.destroy();
        texture.destroy();
        occlusion.destroy();
        if (timestamps) timestamps.destroy();
    )JS");
}

TEST(NativeWebGPU, UncapturedGpuErrorsAreCounted)
{
    RunWebGPUTest(R"JS(
        const device = await (await navigator.gpu.requestAdapter()).requestDevice();
        const before = TestUtils.getUncapturedErrorCount();
        const encoder = device.createCommandEncoder();
        encoder.beginRenderPass({colorAttachments: []}).end();
        device.queue.submit([encoder.finish()]);
        await device.queue.onSubmittedWorkDone();
        if (TestUtils.getUncapturedErrorCount() <= before) {
            throw new Error("Uncaptured validation errors were not observable");
        }
    )JS");
}
