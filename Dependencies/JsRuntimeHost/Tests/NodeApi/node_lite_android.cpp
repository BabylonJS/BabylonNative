#include "node_lite.h"

#include <dlfcn.h>

namespace node_api_tests {

/*static*/ void* NodeLitePlatform::LoadFunction(
    napi_env /*env*/,
    const std::filesystem::path& lib_path,
    const std::string& function_name) noexcept
{
    // On Android the conformance addons are packaged as lib<name>.so in the app's nativeLibraryDir --
    // the only location a native library may be dlopen'd from on API 29+. The resolved lib_path points
    // at a (non-existent) <name>.node under the copied test tree, so load by soname and let the dynamic
    // linker resolve it from nativeLibraryDir. The addon's napi_* imports resolve from libnapi.so -- a
    // DT_NEEDED of both the addon and the host -- so RTLD_NOW binds them at load time.
    std::string soname = "lib" + lib_path.stem().string() + ".so";
    void* handle = dlopen(soname.c_str(), RTLD_NOW | RTLD_LOCAL);
    if (handle == nullptr)
    {
        return nullptr;
    }

    return dlsym(handle, function_name.c_str());
}

} // namespace node_api_tests
