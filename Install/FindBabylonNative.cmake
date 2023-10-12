set(PREBUILT_LIBRARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/install/lib")

function(import_prebuilt_target target)
    add_library(${target} STATIC IMPORTED GLOBAL)
    set_property(TARGET ${target} PROPERTY IMPORTED_LOCATION "${PREBUILT_LIBRARY_DIR}/${target}.lib")
endfunction()

function(import_prebuilt_targets)
    foreach(target IN LISTS ARGN)
        import_prebuilt_target(${target})
    endforeach()
endfunction()

if(EXISTS "${PREBUILT_LIBRARY_DIR}/bgfx.lib")
    set(BGFX_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/bgfx.cmake/bgfx" CACHE STRING "Location of bgfx." )
    set(BIMG_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/bgfx.cmake/bimg" CACHE STRING "Location of bimg." )
    set(BX_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/bgfx.cmake/bx" CACHE STRING "Location of bx." )

    import_prebuilt_targets(bgfx bx bimg)
    import_prebuilt_targets(astc-encoder edtaa3 etc1 etc2 iqa nvtt pvrtc squish tinyexr)

    target_include_directories(bgfx INTERFACE "${BGFX_DIR}/include")
    target_include_directories(bx INTERFACE "${BX_DIR}/include" "${BX_DIR}/include/compat/msvc")
    target_include_directories(bimg INTERFACE "${BIMG_DIR}/include" )

    if (BGFX_CONFIG_DEBUG)
        target_compile_definitions( bgfx INTERFACE "BX_CONFIG_DEBUG=1" )
    else()
        target_compile_definitions( bgfx INTERFACE "BX_CONFIG_DEBUG=0" )
    endif()

    target_link_libraries(bimg INTERFACE astc-encoder edtaa3 etc1 etc2 iqa nvtt pvrtc squish tinyexr)

    set(bgfx_CACHED_BUILD true GLOBAL)
    message("Using bgfx cache build")
endif()

if(EXISTS "${PREBUILT_LIBRARY_DIR}/spirv-cross-core.lib")
    set(SPIRV_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/SPIRV-Cross" CACHE STRING "Location of spirv." )

    import_prebuilt_targets(spirv-cross-core spirv-cross-glsl)

    target_include_directories(spirv-cross-core INTERFACE "${SPIRV_DIR}/include" "${SPIRV_DIR}")
    target_link_libraries(spirv-cross-glsl INTERFACE spirv-cross-core)

    if(GRAPHICS_API STREQUAL "D3D11" OR GRAPHICS_API STREQUAL "D3D12")
        import_prebuilt_target(spirv-cross-hlsl)
        target_link_libraries(spirv-cross-hlsl INTERFACE spirv-cross-core spirv-cross-glsl)
    endif()

    set(spirvcross_CACHED_BUILD true GLOBAL)
    message("Using spirv cross cache build")
endif()

if(EXISTS "${PREBUILT_LIBRARY_DIR}/glslang.lib")
    set(GLSLANG_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/glslang" CACHE STRING "Location of glslang." )

    import_prebuilt_targets(GenericCodeGen glslang OGLCompiler OSDependent MachineIndependent SPIRV glslang-default-resource-limits)

    target_include_directories(glslang INTERFACE "${GLSLANG_DIR}/glslang/Include" "${GLSLANG_DIR}")

    target_link_libraries(glslang INTERFACE OGLCompiler GenericCodeGen OSDependent MachineIndependent)

    set(glslang_CACHED_BUILD true GLOBAL)
    message("Using glslang cross cache build")
endif()


if(EXISTS "${PREBUILT_LIBRARY_DIR}/openxr_loader.lib")
    set(OPENXR_LOADER_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/xr/Dependencies/OpenXR-SDK" CACHE STRING "Location of open xr loader." )

    import_prebuilt_targets(openxr_loader)

    target_include_directories(openxr_loader INTERFACE "${OPENXR_LOADER_DIR}/Include")

    set(openxr_loader_CACHED_BUILD true GLOBAL)
    message("Using openxr_loader cache build")
endif()


if(EXISTS "${PREBUILT_LIBRARY_DIR}/gtest_main.lib")
    import_prebuilt_targets(gtest_main gtest)

    target_include_directories(gtest_main INTERFACE "${googletest_SOURCE_DIR}/googletest/include" "${googletest_SOURCE_DIR}/googletest/include")

    target_link_libraries(gtest_main INTERFACE gtest)

    set(gtest_CACHED_BUILD true GLOBAL)
    message("Using GoogleTests cache build")
endif()
