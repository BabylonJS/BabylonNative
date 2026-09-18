#include "../Shared/Shared.h"
#include "Babylon/DebugTrace.h"
#include <iostream>
#import <Foundation/Foundation.h>

int main()
{
    Babylon::DebugTrace::EnableDebugTrace(true);
    Babylon::DebugTrace::SetTraceOutput([](const char* trace) { NSLog(@"%@", trace); });

    int exitCode = RunTests();

    // CI will pick up the exit code from stderr when running in the iOS Simulator.
    std::cerr << exitCode << std::endl;
    std::cerr.flush();

    return exitCode;
}
