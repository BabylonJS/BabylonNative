#include "../Shared/Shared.h"

int main()
{
    int exitCode = RunTests([](const char* message, Babylon::Polyfills::Console::LogLevel logLevel)
    {
        fprintf(stdout, "[%s] %s", EnumToString(logLevel), message);
        fflush(stdout);
    });

    // CI will pick up the exit code from stderr when running in the iOS Simulator.
    fprintf(stderr, "%i\n", exitCode);

    return exitCode;
}
