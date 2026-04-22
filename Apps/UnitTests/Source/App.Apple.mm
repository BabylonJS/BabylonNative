#include "App.h"
#include <Babylon/DebugTrace.h>

#include <mach-o/dyld.h>

std::filesystem::path GetExecutableDirectory()
{
    uint32_t size = 0;
    _NSGetExecutablePath(nullptr, &size);
    std::string buffer(size, '\0');
    if (_NSGetExecutablePath(buffer.data(), &size) != 0)
    {
        throw std::runtime_error{"_NSGetExecutablePath failed"};
    }
    return std::filesystem::canonical(buffer).parent_path();
}

int main(int argc, char* argv[])
{
    Babylon::DebugTrace::EnableDebugTrace(true);
    Babylon::DebugTrace::SetTraceOutput([](const char* trace) { NSLog(@"%s", trace); });

    Babylon::Graphics::Configuration config{};

#ifndef USE_NOOP_METAL_DEVICE
    config.Device = MTL::CreateSystemDefaultDevice();
    config.Width = 600;
    config.Height = 400;
#endif

    return RunTests(config, argc, argv);
}
