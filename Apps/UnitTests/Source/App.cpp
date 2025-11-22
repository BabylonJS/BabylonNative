#include <gtest/gtest.h>
#include <Babylon/Graphics/Device.h>

Babylon::Graphics::Configuration g_deviceConfig{};

int RunTests(const Babylon::Graphics::Configuration& config, int argc, char* argv[])
{
    g_deviceConfig = config;
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
