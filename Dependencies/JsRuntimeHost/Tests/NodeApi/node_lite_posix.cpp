#include <dlfcn.h>
#if defined(__ANDROID__)
#include <android/api-level.h>
#endif
#include "node_lite.h"

namespace node_api_tests {

/*static*/ void* NodeLitePlatform::LoadFunction(
    napi_env env,
    const std::filesystem::path& lib_path,
    const std::string& function_name) noexcept {
#if defined(__ANDROID__) && (__ANDROID_API__ < 29)
  void* library_handle = dlopen(lib_path.string().c_str(), RTLD_NOW | RTLD_LOCAL);
  if (library_handle == nullptr) {
    return nullptr;
  }

  return dlsym(library_handle, function_name.c_str());
#else
  void* library_handle = dlopen(lib_path.string().c_str(), RTLD_NOW | RTLD_LOCAL);
  if (library_handle == nullptr) {
    const char* error_message = dlerror();
    NODE_LITE_ASSERT(false,
                     "Failed to load dynamic library: %s. Error: %s",
                     lib_path.c_str(),
                     error_message != nullptr ? error_message : "Unknown error");
    return nullptr;
  }

  dlerror(); // Clear any existing error state before dlsym.
  void* symbol = dlsym(library_handle, function_name.c_str());
  const char* error_message = dlerror();
  NODE_LITE_ASSERT(error_message == nullptr,
                   "Failed to resolve symbol: %s in %s. Error: %s",
                   function_name.c_str(),
                   lib_path.c_str(),
                   error_message != nullptr ? error_message : "Unknown error");
  return symbol;
#endif
}

}  // namespace node_api_tests
