# TODO: compute path based on build type and ABI
set(PREBUILT_DIR "${CMAKE_CURRENT_BINARY_DIR}/Windows/RelWithDebInfo/x64")
set(PREBUILT_LIBRARY_DIR "${PREBUILT_DIR}/lib")
set(PREBUILT_HEADERS_DIR "${PREBUILT_DIR}/include")

function(import_prebuilt_target target)
    add_library(${target} STATIC IMPORTED GLOBAL)
    set_property(TARGET ${target} PROPERTY IMPORTED_LOCATION "${PREBUILT_LIBRARY_DIR}/${target}.lib")
endfunction()

function(import_prebuilt_targets)
    foreach(target IN LISTS ARGN)
        import_prebuilt_target(${target})
    endforeach()
endfunction()

# bgfx
    #set(BGFX_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/bgfx.cmake/bgfx" CACHE STRING "Location of bgfx." )
    #set(BIMG_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/bgfx.cmake/bimg" CACHE STRING "Location of bimg." )
    #set(BX_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/bgfx.cmake/bx" CACHE STRING "Location of bx." )

    import_prebuilt_targets(bgfx bx bimg)
    import_prebuilt_targets(astc-encoder edtaa3 etc1 etc2 iqa nvtt pvrtc squish tinyexr)

    #target_include_directories(bgfx INTERFACE "${BGFX_DIR}/include")
    #target_include_directories(bx INTERFACE "${BX_DIR}/include" "${BX_DIR}/include/compat/msvc")
    #target_include_directories(bimg INTERFACE "${BIMG_DIR}/include" )

    #if (BGFX_CONFIG_DEBUG)
    #    target_compile_definitions( bgfx INTERFACE "BX_CONFIG_DEBUG=1" )
    #else()
    #    target_compile_definitions( bgfx INTERFACE "BX_CONFIG_DEBUG=0" )
    #endif()

    target_link_libraries(bimg INTERFACE astc-encoder edtaa3 etc1 etc2 iqa nvtt pvrtc squish tinyexr)

# spirv-cross
    #set(SPIRV_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/SPIRV-Cross" CACHE STRING "Location of spirv." )

    import_prebuilt_targets(spirv-cross-core spirv-cross-glsl)

    #target_include_directories(spirv-cross-core INTERFACE "${SPIRV_DIR}/include" "${SPIRV_DIR}")
    target_link_libraries(spirv-cross-glsl INTERFACE spirv-cross-core)

    #if(GRAPHICS_API STREQUAL "D3D11" OR GRAPHICS_API STREQUAL "D3D12")
        import_prebuilt_target(spirv-cross-hlsl)
        target_link_libraries(spirv-cross-hlsl INTERFACE spirv-cross-core spirv-cross-glsl)
    #endif()

# glslang
    #set(GLSLANG_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/glslang" CACHE STRING "Location of glslang." )

    import_prebuilt_targets(GenericCodeGen glslang OGLCompiler OSDependent MachineIndependent SPIRV glslang-default-resource-limits)

    #target_include_directories(glslang INTERFACE "${GLSLANG_DIR}/glslang/Include" "${GLSLANG_DIR}")

    target_link_libraries(glslang INTERFACE OGLCompiler GenericCodeGen OSDependent MachineIndependent)

# openxr_loader
    #set(OPENXR_LOADER_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/xr/Dependencies/OpenXR-SDK" CACHE STRING "Location of open xr loader." )

    import_prebuilt_targets(openxr_loader)

    #target_include_directories(openxr_loader INTERFACE "${OPENXR_LOADER_DIR}/Include")


target_link_libraries(babylonnative_static INTERFACE openxr_loader glslang spirv-cross-hlsl bimg bx bgfx)