#include <Babylon/Graphics/Device.h>
#include <gtest/gtest.h>

Babylon::Graphics::Configuration g_deviceConfig{};

int RunTests(const Babylon::Graphics::Configuration& config)
{
    g_deviceConfig = config;
    testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}
