set(SOURCES
    "Source/Main.cpp")

add_executable(ShaderTool ${SOURCES})

warnings_as_errors(ShaderTool)

target_link_libraries(ShaderTool
    PRIVATE ShaderCompilerInternal
    PRIVATE ShaderCacheInternal
    PRIVATE ShaderCache)

# See https://gitlab.kitware.com/cmake/cmake/-/issues/23543
# If we can set minimum required to 3.26+, then we can use the `copy -t` syntax instead.
add_custom_command(TARGET ShaderTool POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E $<IF:$<BOOL:$<TARGET_RUNTIME_DLLS:ShaderTool>>,copy,true> $<TARGET_RUNTIME_DLLS:ShaderTool> $<TARGET_FILE_DIR:ShaderTool> COMMAND_EXPAND_LISTS)

set_property(TARGET ShaderTool PROPERTY FOLDER Plugins)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})
