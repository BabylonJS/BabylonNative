#pragma once

#include <filesystem>

int RunTests();

#if defined(__ANDROID__)
struct AAssetManager;
void SetNodeApiTestEnvironment(AAssetManager* assetManager, const std::filesystem::path& baseDir);
#endif
