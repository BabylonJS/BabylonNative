#include "node_lite.h"

#include <cstdint>
#include <string>
#include <vector>

namespace node_api_tests {
namespace {

// Registry of the Node-API conformance addons that are statically linked into this host binary. It is
// populated at load time by the per-addon constructors emitted in entry_point.h under
// JSR_NODE_API_STATIC_LINK, each registering its uniquely-suffixed entry points keyed by module name.
// A function-local static (constructed on first registration) avoids any static-initialization-order
// dependency between those constructors and this translation unit.
struct StaticAddon {
    std::string name;
    int32_t (*get_api_version)(void);
    napi_value (*register_module)(napi_env, napi_value);
};

std::vector<StaticAddon>& StaticAddonRegistry() {
    static std::vector<StaticAddon> registry;
    return registry;
}

} // namespace
} // namespace node_api_tests

// Called once per statically-linked addon from its load-time constructor (see entry_point.h). Declared
// extern "C" to match the declaration the C addon translation units compile against.
extern "C" void jsr_register_static_addon(
    const char* name,
    int32_t (*get_api_version)(void),
    napi_value (*register_module)(napi_env, napi_value)) {
    node_api_tests::StaticAddonRegistry().push_back(
        {name, get_api_version, register_module});
}

namespace node_api_tests {

// On Android the conformance addons are statically linked into the host rather than dlopen'd: dynamic
// .node loading is never shipped to the Play / Quest stores, and bionic won't resolve a dlopen'd
// addon's napi_* against the System.loadLibrary-loaded host anyway (see NAPI_VERSION_ROADMAP.md, and
// task #9 for the shared-lib alternative). So resolve the requested entry point from the in-process
// static registry, keyed by the module's file-stem name, instead of dlopen+dlsym.
/*static*/ void* NodeLitePlatform::LoadFunction(
    napi_env /*env*/,
    const std::filesystem::path& lib_path,
    const std::string& function_name) noexcept
{
    const std::string module_name = lib_path.stem().string();
    for (const StaticAddon& addon : StaticAddonRegistry()) {
        if (addon.name != module_name) {
            continue;
        }
        if (function_name == "napi_register_module_v1") {
            return reinterpret_cast<void*>(addon.register_module);
        }
        if (function_name == "node_api_module_get_api_version_v1") {
            return reinterpret_cast<void*>(addon.get_api_version);
        }
        return nullptr;
    }
    return nullptr;
}

} // namespace node_api_tests
