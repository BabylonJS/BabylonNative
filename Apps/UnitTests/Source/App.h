#pragma once

#include <Babylon/Graphics/Device.h>

#include <filesystem>

int RunTests(const Babylon::Graphics::Configuration& config, int argc, char* argv[]);

std::filesystem::path GetExecutableDirectory();
