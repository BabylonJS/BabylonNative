#include "../Shared/Shared.h"

int main()
{
    return RunTests([](const char* message, Babylon::Polyfills::Console::LogLevel logLevel)
    {
        fprintf(stdout, "[%s] %s", EnumToString(logLevel), message);
        fflush(stdout);
    });
}
