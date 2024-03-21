#include "../Shared/Shared.h"
#include <iostream>

int main()
{
    int exitCode = RunTests();

    // CI will pick up the exit code from stderr when running in the iOS Simulator.
    std::cerr << exitCode << std::endl;
    std::cerr.flush();

    return exitCode;
}
