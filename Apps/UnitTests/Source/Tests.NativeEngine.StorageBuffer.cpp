#include <gtest/gtest.h>

#include "StorageBuffer.h"

#include <Babylon/AppRuntime.h>
#include <Babylon/Graphics/Device.h>
#include <Babylon/Graphics/DeviceContext.h>

#include <algorithm>
#include <array>
#include <chrono>
#include <cstdlib>
#include <functional>
#include <future>
#include <iostream>
#include <stdexcept>

extern Babylon::Graphics::Configuration g_deviceConfig;

namespace
{
    void RunStorageTest(const std::function<void(Babylon::Graphics::DeviceContext&)>& test)
    {
        Babylon::Graphics::Device device{g_deviceConfig};
        device.StartRenderingCurrentFrame();
        std::promise<void> completed;
        auto completion = completed.get_future();
        {
            Babylon::AppRuntime runtime;
            runtime.Dispatch([&](Napi::Env env) {
                try
                {
                    device.AddToJavaScript(env);
                    test(Babylon::Graphics::DeviceContext::GetFromJavaScript(env));
                    completed.set_value();
                }
                catch (...)
                {
                    completed.set_exception(std::current_exception());
                }
            });
            if (completion.wait_for(std::chrono::seconds{30}) != std::future_status::ready)
            {
                std::cerr << "Timed out waiting for storage buffer validation" << std::endl;
                std::quick_exit(1);
            }
        }
        device.FinishRenderingCurrentFrame();
        ASSERT_NO_THROW(completion.get());
    }
}

TEST(NativeEngineStorageBuffer, RejectsInvalidSizesBeforeAllocating)
{
    RunStorageTest([](Babylon::Graphics::DeviceContext& context) {
        EXPECT_THROW((Babylon::StorageBuffer{context, 0}), std::invalid_argument);
        EXPECT_THROW((Babylon::StorageBuffer{context, UINT32_MAX}), std::overflow_error);
        EXPECT_THROW((Babylon::StorageBuffer{context, UINT32_MAX - 14}), std::overflow_error);
        EXPECT_THROW((Babylon::StorageBuffer{context, 16, 65536}), std::invalid_argument);
        Babylon::StorageBuffer buffer{context, 17};
        EXPECT_EQ(buffer.ByteLength(), 17u);
        EXPECT_EQ(buffer.ShadowBytes().size(), 32u);
    });
}

TEST(NativeEngineStorageBuffer, RejectedUpdatesPreserveShadow)
{
    RunStorageTest([](Babylon::Graphics::DeviceContext& context) {
        Babylon::StorageBuffer buffer{context, 32};
        std::array<uint8_t, 16> bytes{};
        bytes.fill(91);
        for (const uint32_t offset : {1u, 17u, 32u, UINT32_MAX})
        {
            SCOPED_TRACE(offset);
            EXPECT_THROW(buffer.Update(bytes, offset), std::runtime_error);
            EXPECT_TRUE(std::all_of(buffer.ShadowBytes().begin(), buffer.ShadowBytes().end(),
                [](uint8_t value) { return value == 0; }));
        }
        EXPECT_NO_THROW(buffer.Update({}, 32));
        buffer.Dispose();
        EXPECT_THROW(buffer.Update(bytes, 0), std::runtime_error);
    });
}
