#pragma once

#include <filesystem>

int RunTests();

#if defined(__ANDROID__) && defined(NODE_API_AVAILABLE_NATIVE_TESTS)
#include <android/asset_manager.h>

// Supplies the in-process Node-API test harness with a native AssetManager and a writable base
// directory (derived from the instrumentation Context in the JNI layer). Without this the harness
// falls back to android::global::GetAppContext(), whose JNI global ref is not valid during the
// instrumented run and aborts with "use of deleted global reference".
void SetNodeApiTestEnvironment(AAssetManager* assetManager, const std::filesystem::path& baseDir);
#endif
