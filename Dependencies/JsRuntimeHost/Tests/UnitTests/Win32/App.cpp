#include "../Shared/Shared.h"

namespace
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

int main()
{
    return RunTests([](const char* message, Babylon::Polyfills::Console::LogLevel logLevel)
    {
        printf("[%s] %s", EnumToString(logLevel), message);
        fflush(stdout);
    });
}
