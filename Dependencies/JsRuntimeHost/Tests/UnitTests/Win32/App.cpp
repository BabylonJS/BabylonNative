#include "../Shared/Shared.h"
#include <Windows.h>
#include "Babylon/DebugTrace.h"
#include <gtest/gtest.h>

int main(int argc, char** argv)
{
    SetConsoleOutputCP(CP_UTF8);

    Babylon::DebugTrace::EnableDebugTrace(true);
    Babylon::DebugTrace::SetTraceOutput([](const char* trace) { OutputDebugStringA(trace); });

    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
