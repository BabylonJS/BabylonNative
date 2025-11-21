#include "App.h"
#include <Babylon/DebugTrace.h>
#include <Metal/Metal.h>

int main()
{
    Babylon::DebugTrace::EnableDebugTrace(true);
    Babylon::DebugTrace::SetTraceOutput([](const char* trace) { NSLog(@"%s", trace); });

    Babylon::Graphics::Configuration config{};

#ifndef BABYLON_NATIVE_TESTS_USE_NOOP_METAL_DEVICE
    config.Device = MTLCreateSystemDefaultDevice();
    config.Width = 600;
    config.Height = 400;
#endif

    return RunTests(config);
}
