#include "../Shared/Shared.h"

int main()
{
    int exitCode = RunTests();

    // CI will pick up the exit code from stderr when running in the iOS Simulator.
    fprintf(stderr, "%i\n", exitCode);

    return exitCode;
}
