set(SOURCES
    "include/napi/env.h"
    "include/napi/compat.h"
    "include/napi/js_native_api.h"
    "include/napi/js_native_ext_api.h"
    "include/napi/js_native_api_types.h"
    "include/napi/napi.h"
    "include/napi/napi-inl.h"
    "include/napi/napi_pointer.h"
    "source/env.cc"
    "source/env_external.h"
    "source/env_external.cc")

add_library(napi ${SOURCES})

target_compile_definitions(napi PUBLIC NODE_ADDON_API_DISABLE_DEPRECATED)
target_compile_definitions(napi PUBLIC NODE_ADDON_API_DISABLE_NODE_SPECIFIC)

target_include_directories(napi PUBLIC "include")
