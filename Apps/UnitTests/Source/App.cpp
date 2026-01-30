#include <gtest/gtest.h>
#include <Babylon/Graphics/Device.h>
#include <Babylon/Polyfills/Console.h>

Babylon::Graphics::Configuration g_deviceConfig{};

int RunTests(const Babylon::Graphics::Configuration& config, int argc, char* argv[])
{
    g_deviceConfig = config;
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

namespace Babylon::Polyfills::Console
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