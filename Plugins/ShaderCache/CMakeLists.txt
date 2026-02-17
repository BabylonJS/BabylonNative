set(SOURCES
    "Include/Babylon/Plugins/ShaderCache.h"
    "InternalInclude/Babylon/Plugins/ShaderCacheInternal.h"
    "Source/ShaderCache.cpp"
    "Source/ShaderCacheImpl.h"
    "Source/ShaderCacheImpl.cpp"
    "Source/xxhash.h"
    "Source/xxhash.c")

add_library(ShaderCache ${SOURCES})

warnings_as_errors(ShaderCache)

target_link_libraries(ShaderCache
    PRIVATE GraphicsDeviceContext)

target_include_directories(ShaderCache
    PUBLIC "Include"
    PRIVATE "InternalInclude")

set_property(TARGET ShaderCache PROPERTY FOLDER Plugins)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

add_library(ShaderCacheInternal INTERFACE)
target_include_directories(ShaderCacheInternal
    INTERFACE "InternalInclude")
target_link_libraries(ShaderCacheInternal
    INTERFACE ShaderCache)
