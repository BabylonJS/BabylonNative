#include <gtest/gtest.h>

#include "VertexArray.h"
#include <Babylon/AppRuntime.h>
#include <Babylon/Graphics/Device.h>
#include <Babylon/Graphics/DeviceContext.h>

#include <algorithm>
#include <future>
#include <vector>

extern Babylon::Graphics::Configuration g_deviceConfig;

namespace
{
    class NativeEngineVertexArray : public testing::Test
    {
    protected:
        void SetUp() override
        {
            std::promise<Babylon::Graphics::DeviceContext*> context;
            auto future = context.get_future();
            m_runtime.Dispatch([&](Napi::Env env) {
                m_device.AddToJavaScript(env);
                context.set_value(&Babylon::Graphics::DeviceContext::GetFromJavaScript(env));
            });
            m_context = future.get();
            FlushFrames();
        }

        void FlushFrames()
        {
            // Resource destruction and stats publication cross bgfx frame boundaries.
            for (size_t frame = 0; frame < 3; ++frame)
            {
                m_device.StartRenderingCurrentFrame();
                m_device.FinishRenderingCurrentFrame();
            }
        }

        void Record(Babylon::VertexArray& array, Babylon::VertexBuffer& buffer, uint32_t stride)
        {
            array.RecordVertexBuffer(&buffer, bgfx::Attrib::Position, 0, stride, 3, bgfx::AttribType::Float, false, 0);
        }

        Babylon::Graphics::Device m_device{g_deviceConfig};
        Babylon::AppRuntime m_runtime{};
        Babylon::Graphics::DeviceContext* m_context{};
    };
}

TEST_F(NativeEngineVertexArray, DisposalAndDestructionReleaseLayouts)
{
    const auto baseline = bgfx::getStats()->numVertexLayouts;
    for (bool explicitDispose : {false, true})
    {
        const std::vector<uint8_t> bytes(36);
        {
            Babylon::VertexBuffer buffer{*m_context, gsl::make_span(bytes), false};
            Babylon::VertexArray array{*m_context};
            Record(array, buffer, 12);
            if (explicitDispose)
            {
                array.Dispose();
                array.Dispose();
            }
        }
        FlushFrames();
        EXPECT_EQ(bgfx::getStats()->numVertexLayouts, baseline);
    }
}

TEST_F(NativeEngineVertexArray, SequentialArraysDoNotExhaustLayouts)
{
    const auto baseline = bgfx::getStats()->numVertexLayouts;
    const uint32_t iterations = bgfx::getCaps()->limits.maxVertexLayouts * 2;
    for (uint32_t iteration = 0; iteration < iterations; ++iteration)
    {
        SCOPED_TRACE(iteration);
        const uint32_t stride = 12 + (iteration % 256) * 4;
        const std::vector<uint8_t> bytes(stride * 3);
        {
            Babylon::VertexBuffer buffer{*m_context, gsl::make_span(bytes), false};
            Babylon::VertexArray array{*m_context};
            ASSERT_NO_THROW(Record(array, buffer, stride));
        }
        FlushFrames();
    }
    EXPECT_EQ(bgfx::getStats()->numVertexLayouts, baseline);
}

TEST_F(NativeEngineVertexArray, RejectedDuplicateDoesNotRetainLayout)
{
    const auto baseline = bgfx::getStats()->numVertexLayouts;
    const std::vector<uint8_t> bytes(36);
    {
        Babylon::VertexBuffer buffer{*m_context, gsl::make_span(bytes), false};
        Babylon::VertexArray array{*m_context};
        Record(array, buffer, 12);
        EXPECT_THROW(Record(array, buffer, 12), std::runtime_error);
    }
    FlushFrames();
    EXPECT_EQ(bgfx::getStats()->numVertexLayouts, baseline);
}

TEST_F(NativeEngineVertexArray, DisposedArrayRejectsNewRecords)
{
    const auto baseline = bgfx::getStats()->numVertexLayouts;
    const std::vector<uint8_t> bytes(36);
    Babylon::VertexBuffer buffer{*m_context, gsl::make_span(bytes), false};
    Babylon::VertexArray array{*m_context};
    array.Dispose();
    EXPECT_THROW(Record(array, buffer, 12), std::runtime_error);
    FlushFrames();
    EXPECT_EQ(bgfx::getStats()->numVertexLayouts, baseline);
}

TEST_F(NativeEngineVertexArray, SharedLayoutSurvivesUntilLastArrayIsDisposed)
{
    const auto baseline = bgfx::getStats()->numVertexLayouts;
    const std::vector<uint8_t> bytes(36);
    Babylon::VertexBuffer buffer{*m_context, gsl::make_span(bytes), false};
    Babylon::VertexArray first{*m_context};
    Babylon::VertexArray second{*m_context};
    Record(first, buffer, 12);
    Record(second, buffer, 12);
    FlushFrames();
    const auto sharedCount = bgfx::getStats()->numVertexLayouts;
    EXPECT_GT(sharedCount, baseline);
    first.Dispose();
    FlushFrames();
    EXPECT_EQ(bgfx::getStats()->numVertexLayouts, sharedCount);
    second.Dispose();
    buffer.Dispose();
    FlushFrames();
    EXPECT_EQ(bgfx::getStats()->numVertexLayouts, baseline);
}

TEST_F(NativeEngineVertexArray, OldDeviceLayoutsDoNotReleaseNewDeviceHandles)
{
    const std::vector<uint8_t> bytes(36);
    Babylon::VertexBuffer oldBuffer{*m_context, gsl::make_span(bytes), false};
    Babylon::VertexArray oldArray{*m_context};
    Record(oldArray, oldBuffer, 12);
    FlushFrames();
    const auto deviceId = m_context->GetDeviceId();
    m_device.DisableRendering();
    FlushFrames();
    ASSERT_NE(m_context->GetDeviceId(), deviceId);
    const auto baseline = bgfx::getStats()->numVertexLayouts;

    Babylon::VertexBuffer newBuffer{*m_context, gsl::make_span(bytes), false};
    Babylon::VertexArray newArray{*m_context};
    Record(newArray, newBuffer, 12);
    FlushFrames();
    const auto newCount = bgfx::getStats()->numVertexLayouts;
    oldArray.Dispose();
    oldBuffer.Dispose();
    FlushFrames();
    EXPECT_EQ(bgfx::getStats()->numVertexLayouts, newCount);
    newArray.Dispose();
    newBuffer.Dispose();
    FlushFrames();
    EXPECT_EQ(bgfx::getStats()->numVertexLayouts, baseline);
}

TEST_F(NativeEngineVertexArray, OldDeviceArrayRejectsNewRecords)
{
    Babylon::VertexArray array{*m_context};
    const auto deviceId = m_context->GetDeviceId();
    m_device.DisableRendering();
    FlushFrames();
    ASSERT_NE(m_context->GetDeviceId(), deviceId);
    const auto baseline = bgfx::getStats()->numVertexLayouts;
    const auto bufferBaseline = bgfx::getStats()->numVertexBuffers;

    const std::vector<uint8_t> bytes(36);
    Babylon::VertexBuffer buffer{*m_context, gsl::make_span(bytes), false};
    for (uint32_t divisor : {0u, 1u})
    {
        SCOPED_TRACE(divisor);
        EXPECT_THROW(array.RecordVertexBuffer(&buffer, bgfx::Attrib::Position, 0, 12, 3, bgfx::AttribType::Float, false, divisor), std::runtime_error);
    }
    EXPECT_TRUE(array.GetInstances().empty());
    array.Dispose();
    buffer.Dispose();
    FlushFrames();
    EXPECT_EQ(bgfx::getStats()->numVertexLayouts, baseline);
    EXPECT_EQ(bgfx::getStats()->numVertexBuffers, bufferBaseline);
}

TEST_F(NativeEngineVertexArray, LayoutExhaustionIsReportedAndCanRecover)
{
    const auto baseline = bgfx::getStats()->numVertexLayouts;
    const std::vector<uint8_t> bytes(36);
    Babylon::VertexBuffer buffer{*m_context, gsl::make_span(bytes), false};
    buffer.Build(12);
    std::vector<bgfx::VertexLayoutHandle> layouts;
    for (uint32_t index = 0; index < bgfx::getCaps()->limits.maxVertexLayouts; ++index)
    {
        bgfx::VertexLayout layout;
        layout.begin().add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true);
        for (uint32_t padding = index * 4; padding > 0;)
        {
            const auto skip = static_cast<uint8_t>(std::min(padding, uint32_t{255}));
            layout.skip(skip);
            padding -= skip;
        }
        layout.end();
        const auto handle = bgfx::createVertexLayout(layout);
        if (!bgfx::isValid(handle))
        {
            break;
        }
        layouts.push_back(handle);
    }

    Babylon::VertexArray array{*m_context};
    EXPECT_THROW(Record(array, buffer, 12), std::runtime_error);
    for (const auto handle : layouts)
    {
        bgfx::destroy(handle);
    }
    FlushFrames();
    EXPECT_NO_THROW(Record(array, buffer, 12));
    array.Dispose();
    buffer.Dispose();
    FlushFrames();
    EXPECT_EQ(bgfx::getStats()->numVertexLayouts, baseline);
}
