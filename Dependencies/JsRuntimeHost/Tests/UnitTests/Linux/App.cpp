#include "../Shared/Shared.h"
#include "Babylon/DebugTrace.h"
#include <cstdio>

int main()
{
    Babylon::DebugTrace::EnableDebugTrace(true);
    Babylon::DebugTrace::SetTraceOutput([](const char* trace) { printf("%s\n", trace); fflush(stdout); });

    return RunTests();
}
