#include <gtest/gtest.h>

#include "InstanceRepacker.h"
#include "VertexArray.h"
#include "VertexBuffer.h"

#include <Babylon/AppRuntime.h>
#include <Babylon/Graphics/BgfxShaderInfo.h>
#include <Babylon/Graphics/Device.h>
#include <Babylon/Graphics/DeviceContext.h>
#include <Babylon/Graphics/FrameBuffer.h>
#include <Babylon/Graphics/Texture.h>

#include <array>
#include <chrono>
#include <cstdlib>
#include <future>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <vector>

extern Babylon::Graphics::Configuration g_deviceConfig;

namespace
{
    using InstanceInfo = Babylon::VertexBuffer::InstanceInfo;

    uint32_t InstanceLocation(uint32_t slot)
    {
        return Babylon::Graphics::INSTANCE_DATA_FIRST_LOCATION - slot;
    }
}

TEST(NativeEngineInstanceData, SparseBuiltInsKeepCompilerAssignedSlots)
{
    std::map<uint32_t, uint32_t> builtInSlots{
        {InstanceLocation(7), 7},
        {InstanceLocation(6), 6},
        {InstanceLocation(5), 5},
        {InstanceLocation(4), 4},
        {InstanceLocation(3), 3},
        {InstanceLocation(2), 2},
        {InstanceLocation(1), 1},
        {InstanceLocation(0), 0},
    };

    std::map<uint32_t, InstanceInfo> instances{
        {InstanceLocation(7), {}},
        {InstanceLocation(6), {}},
        {InstanceLocation(5), {}},
        {InstanceLocation(4), {}},
        {static_cast<uint32_t>(bgfx::Attrib::TexCoord3), {}},
        {static_cast<uint32_t>(bgfx::Attrib::Position), {}},
    };

    const auto layout = Babylon::VertexBuffer::CreateInstanceDataLayout(
        instances, builtInSlots, Babylon::Graphics::MAX_INSTANCE_DATA_SLOT_COUNT);

    EXPECT_EQ(layout.SlotCount, 10u);
    EXPECT_EQ(layout.Slots.at(InstanceLocation(7)), 7u);
    EXPECT_EQ(layout.Slots.at(InstanceLocation(6)), 6u);
    EXPECT_EQ(layout.Slots.at(InstanceLocation(5)), 5u);
    EXPECT_EQ(layout.Slots.at(InstanceLocation(4)), 4u);
    EXPECT_EQ(layout.Slots.at(static_cast<uint32_t>(bgfx::Attrib::TexCoord3)), 8u);
    EXPECT_EQ(layout.Slots.at(static_cast<uint32_t>(bgfx::Attrib::Position)), 9u);
}

TEST(NativeEngineInstanceData, BuiltInsKeepAssignedSlotsAtRealLocations)
{
    std::map<uint32_t, uint32_t> builtInSlots{
        {static_cast<uint32_t>(bgfx::Attrib::Color0), 1},
        {static_cast<uint32_t>(bgfx::Attrib::TexCoord0), 0},
    };
    std::map<uint32_t, InstanceInfo> instances{
        {static_cast<uint32_t>(bgfx::Attrib::Position), {}},
        {static_cast<uint32_t>(bgfx::Attrib::Color0), {}},
        {static_cast<uint32_t>(bgfx::Attrib::TexCoord0), {}},
        {static_cast<uint32_t>(bgfx::Attrib::TexCoord3), {}},
    };

    const auto layout = Babylon::VertexBuffer::CreateInstanceDataLayout(
        instances, builtInSlots, Babylon::Graphics::MAX_INSTANCE_DATA_SLOT_COUNT);

    EXPECT_EQ(layout.SlotCount, 4u);
    EXPECT_EQ(layout.Slots.at(static_cast<uint32_t>(bgfx::Attrib::Color0)), 1u);
    EXPECT_EQ(layout.Slots.at(static_cast<uint32_t>(bgfx::Attrib::TexCoord0)), 0u);
    EXPECT_EQ(layout.Slots.at(static_cast<uint32_t>(bgfx::Attrib::TexCoord3)), 2u);
    EXPECT_EQ(layout.Slots.at(static_cast<uint32_t>(bgfx::Attrib::Position)), 3u);
}

// Instances sourced from a buffer promoted to GPU storage (transform feedback output) are repacked by compute
// into the shader's layout slots. Each repack in a frame keeps its own parameters.
TEST(NativeEngineInstanceData, EmbeddedShaderRepacksStridedInstances)
{
    Babylon::Graphics::Device device{g_deviceConfig};
#if defined(USE_NOOP_METAL_DEVICE) || defined(SKIP_RENDER_TESTS)
    GTEST_SKIP() << "GPU rendering/readback is unavailable in this test configuration";
#endif
    device.StartRenderingCurrentFrame();
    if ((bgfx::getCaps()->supported & BGFX_CAPS_COMPUTE) == 0 ||
        !bgfx::isTextureValid(0, false, 1, bgfx::TextureFormat::RGBA32F, BGFX_TEXTURE_READ_BACK | BGFX_TEXTURE_BLIT_DST))
    {
        device.FinishRenderingCurrentFrame();
        GTEST_SKIP() << "Compute and RGBA32F readback are required";
    }

    constexpr uint32_t sourceStride = 20;
    constexpr std::array<uint32_t, 4> locations{3, 7, 13, 29};
    constexpr std::array<uint32_t, 4> offsets{1, 5, 9, 13};
    constexpr std::array<uint32_t, 4> sizes{1, 2, 3, 4};
    {
        Babylon::AppRuntime runtime;
        std::unique_ptr<Babylon::VertexBuffer> source;
        std::unique_ptr<Babylon::VertexArray> vertexArray;
        std::unique_ptr<Babylon::InstanceRepacker> repacker;
        Babylon::VertexBuffer::InstanceDataLayout layout;
        // The second count is repacked into the same destination later in the same frame.
        constexpr std::array<std::pair<uint32_t, uint32_t>, 3> passes{{{65u, 1u}, {129u, 0u}, {1u, 0u}}};
        for (const auto& [count, laterCount] : passes)
        {
            std::vector<float> input(count * sourceStride);
            for (size_t word = 0; word < input.size(); ++word)
            {
                input[word] = static_cast<float>(word + count * 100);
            }
            std::vector<float> output(count * 16, -1.f);
            std::promise<void> completed;
            auto completion = completed.get_future();
            runtime.Dispatch([&](Napi::Env env) {
                try
                {
                    device.AddToJavaScript(env);
                    auto& context = Babylon::Graphics::DeviceContext::GetFromJavaScript(env);
                    auto scope = context.AcquireFrameCompletionScope();
                    if (!repacker)
                    {
                        repacker = std::make_unique<Babylon::InstanceRepacker>(context);
                        const std::vector<uint8_t> initial(129 * sourceStride * sizeof(float));
                        source = std::make_unique<Babylon::VertexBuffer>(context, initial, false);
                        source->PromoteToGpuStorage();
                        vertexArray = std::make_unique<Babylon::VertexArray>(context);
                        for (size_t attr = 0; attr < locations.size(); ++attr)
                        {
                            vertexArray->RecordVertexBuffer(source.get(), locations[attr], offsets[attr] * sizeof(float),
                                sourceStride * sizeof(float), sizes[attr], bgfx::AttribType::Float, false, 1);
                        }
                        // A built-in (like world0) keeps slot 0, ahead of the reverse-ordered generic attributes.
                        layout = Babylon::VertexBuffer::CreateInstanceDataLayout(
                            vertexArray->GetInstances(), {{locations[0], 0}}, Babylon::Graphics::MAX_INSTANCE_DATA_SLOT_COUNT);
                    }
                    source->Update(gsl::make_span(reinterpret_cast<const uint8_t*>(input.data()), input.size() * sizeof(float)), 0);
                    Babylon::Graphics::FrameBuffer frameBuffer{context, BGFX_INVALID_HANDLE, 0, 0, true, false, false};
                    const auto buffer = repacker->Repack(frameBuffer, vertexArray.get(), vertexArray->GetInstances(), layout, count);
                    if (laterCount != 0 &&
                        repacker->Repack(frameBuffer, vertexArray.get(), vertexArray->GetInstances(), layout, laterCount).idx != buffer.idx)
                    {
                        throw std::runtime_error{"Instance repacker replaced the destination buffer"};
                    }
                    if (!bgfx::isValid(buffer))
                    {
                        throw std::runtime_error{"Instance repacker returned an invalid buffer"};
                    }
                    context.ForceMidFrameFlush();
                    auto readback = std::make_shared<Babylon::Graphics::Texture>(context);
                    readback->Create2D(static_cast<uint16_t>(count * 4), 1, false, 1, bgfx::TextureFormat::RGBA32F,
                        BGFX_TEXTURE_READ_BACK | BGFX_TEXTURE_BLIT_DST);
                    bgfx::TextureRegion destination{};
                    destination.init(readback->Handle());
                    bgfx::BufferRegion bufferRegion{};
                    bufferRegion.init(buffer, 0, static_cast<uint32_t>(output.size() * sizeof(float)));
                    context.GetActiveEncoder()->blit(context.AcquireNewViewId(), destination, bufferRegion);
                    context.ReadTextureAsync(readback->Handle(),
                        gsl::make_span(reinterpret_cast<uint8_t*>(output.data()), output.size() * sizeof(float)))
                        .then(arcana::inline_scheduler, arcana::cancellation::none(),
                            [readback, &completed](const arcana::expected<void, std::exception_ptr>& result) {
                                if (result.has_error())
                                {
                                    completed.set_exception(result.error());
                                }
                                else
                                {
                                    completed.set_value();
                                }
                            });
                }
                catch (...)
                {
                    completed.set_exception(std::current_exception());
                }
            });
            const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds{30};
            while (completion.wait_for(std::chrono::milliseconds{1}) != std::future_status::ready)
            {
                if (std::chrono::steady_clock::now() >= deadline)
                {
                    std::cerr << "Timed out waiting for embedded instance repack readback" << std::endl;
                    std::quick_exit(1);
                }
                device.FinishRenderingCurrentFrame();
                device.StartRenderingCurrentFrame();
            }
            ASSERT_NO_THROW(completion.get());
            for (uint32_t instance = 0; instance < count; ++instance)
            {
                for (size_t attr = 0; attr < locations.size(); ++attr)
                {
                    const size_t slot = layout.Slots.at(locations[attr]);
                    for (uint32_t word = 0; word < sizes[attr]; ++word)
                    {
                        EXPECT_EQ(output[instance * 16 + slot * 4 + word], input[instance * sourceStride + offsets[attr] + word])
                            << "count " << count << ", instance " << instance << ", attribute " << attr << ", word " << word;
                    }
                }
            }
        }
    }
    device.FinishRenderingCurrentFrame();
}
