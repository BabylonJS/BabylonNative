#include "android_tests.h"
#include "node_lite.h"
#include "test_main.h"

#include <android/asset_manager_jni.h>
#include <android/log.h>
#include <fstream>
#include <future>
#include <sstream>
#include <stdexcept>

#if defined(JSR_NAPI_ENGINE_V8)
#include <Babylon/AppRuntime.h>
#include <v8.h>
#endif

namespace node_api_tests {
namespace {

template <typename T>
T RequireJni(JNIEnv* env, T value, const char* operation) {
  if (env->ExceptionCheck() || !value) {
    throw std::runtime_error{operation};
  }
  return value;
}

std::string ReadAsset(AAssetManager* manager, const std::string& path) {
  std::unique_ptr<AAsset, decltype(&AAsset_close)> asset{
      AAssetManager_open(manager, path.c_str(), AASSET_MODE_BUFFER),
      AAsset_close};
  if (!asset) {
    throw std::runtime_error{"Cannot open test asset: " + path};
  }
  std::string contents(static_cast<size_t>(AAsset_getLength(asset.get())), '\0');
  size_t offset = 0;
  while (offset < contents.size()) {
    int read = AAsset_read(asset.get(), contents.data() + offset,
                          contents.size() - offset);
    if (read <= 0) {
      throw std::runtime_error{"Cannot read test asset: " + path};
    }
    offset += static_cast<size_t>(read);
  }
  return contents;
}

}  // namespace

bool InitializeAndroidNodeApiTests(JNIEnv* env, jobject context) {
  if (env->PushLocalFrame(8) < 0) {
    __android_log_write(ANDROID_LOG_ERROR, "NodeApiTests",
                        "Cannot allocate JNI local references");
    return false;
  }

  bool initialized = false;
  try {
    auto contextClass = RequireJni(
        env, env->GetObjectClass(context), "Cannot find Context class");
    auto getAssets = RequireJni(env, env->GetMethodID(
        contextClass, "getAssets", "()Landroid/content/res/AssetManager;"),
        "Cannot find Context.getAssets");
    auto getFilesDir = RequireJni(env, env->GetMethodID(
        contextClass, "getFilesDir", "()Ljava/io/File;"),
        "Cannot find Context.getFilesDir");
    auto assetManagerObject = RequireJni(
        env, env->CallObjectMethod(context, getAssets), "Cannot get assets");
    auto filesDir = RequireJni(
        env, env->CallObjectMethod(context, getFilesDir), "Cannot get files directory");
    auto fileClass = RequireJni(
        env, env->GetObjectClass(filesDir), "Cannot find File class");
    auto getAbsolutePath = RequireJni(env, env->GetMethodID(
        fileClass, "getAbsolutePath", "()Ljava/lang/String;"),
        "Cannot find File.getAbsolutePath");
    auto pathString = static_cast<jstring>(RequireJni(
        env, env->CallObjectMethod(filesDir, getAbsolutePath), "Cannot get files path"));
    const char* path = RequireJni(
        env, env->GetStringUTFChars(pathString, nullptr), "Cannot read files path");
    std::string filesPath{path};
    env->ReleaseStringUTFChars(pathString, path);
    std::filesystem::path root = std::filesystem::path{filesPath} / "node_api_tests";
    auto* manager = AAssetManager_fromJava(env, assetManagerObject);
    if (!manager) {
      throw std::runtime_error{"Cannot access native test asset manager"};
    }
    std::filesystem::remove_all(root);
    std::istringstream manifest{ReadAsset(manager, "NodeApi/manifest.txt")};
    std::string relativePath;
    size_t copied = 0;
    while (std::getline(manifest, relativePath)) {
      auto destination = root / relativePath;
      std::filesystem::create_directories(destination.parent_path());
      std::ofstream output{destination, std::ios::binary};
      output.exceptions(std::ios::badbit | std::ios::failbit);
      output << ReadAsset(manager, "NodeApi/test/" + relativePath);
      ++copied;
    }
    if (copied == 0) {
      throw std::runtime_error{"Node-API test asset manifest is empty"};
    }
#if defined(JSR_NAPI_ENGINE_V8)
    // Initialize the shared platform even when only conformance tests are selected.
    v8::V8::SetFlagsFromString("--expose-gc");
    std::promise<void> ready;
    auto future = ready.get_future();
    {
      Babylon::AppRuntime runtime;
      runtime.Dispatch([&ready](Napi::Env) { ready.set_value(); });
      future.get();
    }
#endif
    NodeApiTestConfig config{};
    config.js_root = root;
    config.run_script = [root](const std::filesystem::path& script) {
      ProcessResult result = RunNodeLiteScript(root, script);
      // Node runs each script in a fresh process, so unload the addons to
      // give the next script fresh static state.
      NodeLitePlatform::UnloadLibraries();
      return result;
    };
    std::istringstream suites{NODE_API_AVAILABLE_NATIVE_TESTS};
    std::string suite;
    while (std::getline(suites, suite, ',')) {
      config.enabled_native_suites.insert(suite);
    }
    InitializeNodeApiTests(config);
    initialized = true;
  } catch (const std::exception& error) {
    __android_log_write(ANDROID_LOG_ERROR, "NodeApiTests", error.what());
  }
  env->PopLocalFrame(nullptr);
  return initialized;
}
}  // namespace node_api_tests
