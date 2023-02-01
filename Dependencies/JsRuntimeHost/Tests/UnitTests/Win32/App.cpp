#include "../Shared/Shared.h"

int main()
{
    return RunTests([](const char* message, Babylon::Polyfills::Console::LogLevel logLevel)
    {
        printf("[%s] %s", EnumToString(logLevel), message);
        fflush(stdout);
    });
}
