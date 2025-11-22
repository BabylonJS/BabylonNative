#include "App.h"
#include <Babylon/DebugTrace.h>
#include <Metal/Metal.h>

int main(int argc, char* argv[])
{
    Babylon::DebugTrace::EnableDebugTrace(true);
    Babylon::DebugTrace::SetTraceOutput([](const char* trace) { NSLog(@"%s", trace); });

    Babylon::Graphics::Configuration config{};

#ifndef USE_NOOP_METAL_DEVICE
    config.Device = MTLCreateSystemDefaultDevice();
    config.Width = 600;
    config.Height = 400;
#endif

    return RunTests(config, argc, argv);
}
