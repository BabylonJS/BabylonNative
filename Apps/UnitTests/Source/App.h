#pragma once

#include <Babylon/Graphics/Device.h>

#include <filesystem>

int RunTests(const Babylon::Graphics::Configuration& config, int argc, char* argv[]);

std::filesystem::path GetExecutableDirectory();

#ifdef _WIN32
bool ResizeWindowClientArea(Babylon::Graphics::WindowT window, size_t width, size_t height);
#endif
