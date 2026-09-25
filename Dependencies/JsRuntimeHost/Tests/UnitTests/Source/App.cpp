#include "App.h"
#include <gtest/gtest.h>

#if defined(__ANDROID__) && defined(NODE_API_AVAILABLE_NATIVE_TESTS)
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <android/log.h>
#include <filesystem>
#include <fstream>
#include <mutex>
#include <optional>
#include <sstream>
#include <string>
#include <system_error>
#include <unordered_set>

#include <AndroidExtensions/Globals.h>
#include <AndroidExtensions/JavaWrappers.h>

#include "../../NodeApi/node_lite.h"
#include "../../NodeApi/test_main.h"

namespace
{
    using namespace std::filesystem;

    void CopyAssetsRecursive(AAssetManager* manager, const std::string& asset_path, const path& destination)
    {
        // The NDK AAssetManager cannot enumerate subdirectories -- AAssetDir_getNextFileName
        // returns files in a single directory only, never nested directories -- so the test
        // tree cannot be discovered at runtime. Instead read a build-time manifest (one
        // relative path per line, produced by the copyNodeApiTests Gradle task) and copy each
        // listed file individually (AAssetManager_open works fine for a known file path).
        std::string manifest_asset = asset_path + "/manifest.txt";
        AAsset* manifest = AAssetManager_open(manager, manifest_asset.c_str(), AASSET_MODE_BUFFER);
        if (manifest == nullptr)
        {
            return;
        }

        off_t manifest_length = AAsset_getLength(manifest);
        std::string manifest_text(static_cast<size_t>(manifest_length), '\0');
        AAsset_read(manifest, manifest_text.data(), manifest_length);
        AAsset_close(manifest);

        std::stringstream manifest_stream(manifest_text);
        std::string relative_path;
        while (std::getline(manifest_stream, relative_path))
        {
            if (!relative_path.empty() && relative_path.back() == '\r')
            {
                relative_path.pop_back();
            }
            if (relative_path.empty())
            {
                continue;
            }

            std::string child_asset = asset_path + "/" + relative_path;
            AAsset* asset = AAssetManager_open(manager, child_asset.c_str(), AASSET_MODE_STREAMING);
            if (asset == nullptr)
            {
                continue;
            }

            path output_path = destination / relative_path;
            create_directories(output_path.parent_path());
            std::ofstream output(output_path, std::ios::binary);
            char buffer[8192];
            int read = 0;
            while ((read = AAsset_read(asset, buffer, sizeof(buffer))) > 0)
            {
                output.write(buffer, read);
            }
            AAsset_close(asset);
        }
    }

    path GetFilesDir()
    {
        JNIEnv* env = android::global::GetEnvForCurrentThread();
        jobject context = android::global::GetAppContext();
        jclass contextClass = env->GetObjectClass(context);
        jmethodID getFilesDir = env->GetMethodID(contextClass, "getFilesDir", "()Ljava/io/File;");
        jobject filesDir = env->CallObjectMethod(context, getFilesDir);
        env->DeleteLocalRef(contextClass);

        jclass fileClass = env->GetObjectClass(filesDir);
        jmethodID getAbsolutePath = env->GetMethodID(fileClass, "getAbsolutePath", "()Ljava/lang/String;");
        jstring pathString = static_cast<jstring>(env->CallObjectMethod(filesDir, getAbsolutePath));
        env->DeleteLocalRef(fileClass);

        const char* rawPath = env->GetStringUTFChars(pathString, nullptr);
        path resultPath{rawPath};
        env->ReleaseStringUTFChars(pathString, rawPath);
        env->DeleteLocalRef(pathString);
        env->DeleteLocalRef(filesDir);

        return resultPath;
    }

    std::unordered_set<std::string> ParseNativeSuiteList()
    {
        std::unordered_set<std::string> suites;
#ifdef NODE_API_AVAILABLE_NATIVE_TESTS
        std::stringstream stream(NODE_API_AVAILABLE_NATIVE_TESTS);
        std::string entry;
        while (std::getline(stream, entry, ','))
        {
            if (!entry.empty())
            {
                suites.insert(entry);
            }
        }
#endif
        return suites;
    }

    std::optional<path>& OverrideBaseDir()
    {
        static std::optional<path> baseDirOverride{};
        return baseDirOverride;
    }

    AAssetManager*& OverrideAssetManager()
    {
        static AAssetManager* assetManager{};
        return assetManager;
    }

    void ConfigureNodeApiTests()
    {
        static std::once_flag onceFlag;
        std::call_once(onceFlag, []() {
            path baseDir;
            if (OverrideBaseDir())
            {
                baseDir = *OverrideBaseDir();
            }
            else
            {
                baseDir = GetFilesDir() / "node_api_tests";
            }
            std::error_code ec;
            std::filesystem::remove_all(baseDir, ec);
            std::filesystem::create_directories(baseDir);

            AAssetManager* assetManagerNative = OverrideAssetManager();
            if (assetManagerNative == nullptr)
            {
                auto assetManagerWrapper = android::global::GetAppContext().getAssets();
                assetManagerNative = assetManagerWrapper;
            }

            if (assetManagerNative != nullptr)
            {
                CopyAssetsRecursive(assetManagerNative, "NodeApi/test", baseDir);
            }

            node_api_tests::NodeApiTestConfig config{};
            config.js_root = baseDir;
            config.run_script = [baseDir](const path& script) {
                node_api_tests::NodeLiteRuntime::Callbacks callbacks;
                callbacks.stdout_callback = [](const std::string& message) {
                    __android_log_write(ANDROID_LOG_INFO, "NodeApiTests", message.c_str());
                };
                callbacks.stderr_callback = [](const std::string& message) {
                    __android_log_write(ANDROID_LOG_ERROR, "NodeApiTests", message.c_str());
                };
                return node_api_tests::RunNodeLiteScript(baseDir, script, std::move(callbacks));
            };
            config.enabled_native_suites = ParseNativeSuiteList();

            node_api_tests::InitializeNodeApiTests(config);
        });
    }
}
#endif

int RunTests()
{
#if defined(__ANDROID__) && defined(NODE_API_AVAILABLE_NATIVE_TESTS)
    ConfigureNodeApiTests();
#endif
    testing::InitGoogleTest();
#if defined(__ANDROID__) && defined(NODE_API_AVAILABLE_NATIVE_TESTS)
    node_api_tests::RegisterNodeApiTests();
#endif
    return RUN_ALL_TESTS();
}

#if defined(__ANDROID__) && defined(NODE_API_AVAILABLE_NATIVE_TESTS)
void SetNodeApiTestEnvironment(AAssetManager* assetManager, const std::filesystem::path& baseDir)
{
    OverrideAssetManager() = assetManager;
    OverrideBaseDir() = baseDir;
}
#endif
