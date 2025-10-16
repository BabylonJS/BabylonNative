#include "node_lite.h"

#include <dlfcn.h>

namespace node_api_tests {

/*static*/ void* NodeLitePlatform::LoadFunction(
    napi_env /*env*/,
    const std::filesystem::path& lib_path,
    const std::string& function_name) noexcept
{
    void* handle = dlopen(lib_path.string().c_str(), RTLD_NOW | RTLD_LOCAL);
    if (handle == nullptr)
    {
        return nullptr;
    }

    void* symbol = dlsym(handle, function_name.c_str());
    return symbol;
}

} // namespace node_api_tests
