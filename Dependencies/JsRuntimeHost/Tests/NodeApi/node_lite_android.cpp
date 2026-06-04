#include "node_lite.h"

#include <dlfcn.h>

namespace node_api_tests {

/*static*/ void* NodeLitePlatform::LoadFunction(
    napi_env /*env*/,
    const std::filesystem::path& lib_path,
    const std::string& function_name) noexcept
{
    // On Android native addons are packaged as lib<name>.so in the app's nativeLibraryDir -- the
    // only location a native library may be dlopen'd from on API 29+. The resolved lib_path points
    // at a (non-existent) <name>.node under the copied test tree, so load by soname and let the
    // dynamic linker resolve it from nativeLibraryDir.
    //
    // NOTE: the addon imports napi_* from the host (libUnitTestsJNI.so). Because the host is loaded
    // RTLD_LOCAL by System.loadLibrary, bionic's linker-namespace model does not expose its
    // statically linked napi_* symbols to this dlopen'd module, so RTLD_NOW below cannot bind them.
    // Making the in-process addon tests runnable on Android requires building napi as a shared
    // library (libnapi.so) depended on by both the host and the addons -- tracked separately (see
    // NAPI_VERSION_ROADMAP.md). The js-native-api tests are skipped on Android until then.
    std::string soname = "lib" + lib_path.stem().string() + ".so";
    void* handle = dlopen(soname.c_str(), RTLD_NOW | RTLD_LOCAL);
    if (handle == nullptr)
    {
        return nullptr;
    }

    return dlsym(handle, function_name.c_str());
}

} // namespace node_api_tests
