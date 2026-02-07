set(SOURCES
    "Source/Main.cpp")

add_executable(ShaderTool ${SOURCES})

warnings_as_errors(ShaderTool)

target_link_libraries(ShaderTool
    PRIVATE ShaderCompilerInternal
    PRIVATE ShaderCacheInternal
    PRIVATE ShaderCache)

set_property(TARGET ShaderTool PROPERTY FOLDER Plugins)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})
