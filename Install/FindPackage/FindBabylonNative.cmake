# TODO: compute path based on build type and ABI
set(PREBUILT_DIR "${CMAKE_CURRENT_LIST_DIR}/Windows/RelWithDebInfo/x64")
set(PREBUILT_LIBRARY_DIR "${PREBUILT_DIR}/lib")
set(PREBUILT_HEADERS_DIR "${PREBUILT_DIR}/include")

function(import_prebuilt_target target)
    add_library(BN::${target} STATIC IMPORTED GLOBAL)
    set_property(TARGET BN::${target} PROPERTY IMPORTED_LOCATION "${PREBUILT_LIBRARY_DIR}/${target}.lib")
endfunction()

function(import_prebuilt_targets)
    foreach(target IN LISTS ARGN)
        import_prebuilt_target(${target})
    endforeach()
endfunction()

function(import_prebuilt_target_nonamespace target)
    add_library(${target} STATIC IMPORTED GLOBAL)
    set_property(TARGET ${target} PROPERTY IMPORTED_LOCATION "${PREBUILT_LIBRARY_DIR}/${target}.lib")
endfunction()

function(import_prebuilt_targets_nonamespace)
    foreach(target IN LISTS ARGN)
        import_prebuilt_target_nonamespace(${target})
    endforeach()
endfunction()

# bgfx
import_prebuilt_targets_nonamespace(bgfx bx bimg)
import_prebuilt_targets(astc-encoder edtaa3 etc1 etc2 iqa nvtt pvrtc squish tinyexr)
target_link_libraries(bimg INTERFACE BN::astc-encoder BN::edtaa3 BN::etc1 BN::etc2 BN::iqa BN::nvtt BN::pvrtc BN::squish BN::tinyexr)

# spirv-cross
import_prebuilt_targets_nonamespace(spirv-cross-core spirv-cross-glsl spirv-cross-hlsl)
target_link_libraries(spirv-cross-glsl INTERFACE spirv-cross-core)
target_link_libraries(spirv-cross-hlsl INTERFACE spirv-cross-core spirv-cross-glsl)

# glslang
import_prebuilt_targets_nonamespace(glslang SPIRV glslang-default-resource-limits)
import_prebuilt_targets(GenericCodeGen OGLCompiler OSDependent MachineIndependent)
target_link_libraries(glslang INTERFACE BN::OGLCompiler BN::GenericCodeGen BN::OSDependent BN::MachineIndependent SPIRV glslang-default-resource-limits)

# openxr_loader
import_prebuilt_targets_nonamespace(openxr_loader)

# arcana
import_prebuilt_targets(arcana)

# static/no JS babylon native
add_library(babylonnative_static INTERFACE IMPORTED GLOBAL)
target_link_libraries(babylonnative_static INTERFACE openxr_loader glslang spirv-cross-hlsl bimg bx bgfx BN::arcana)
target_include_directories(babylonnative_static INTERFACE "${PREBUILT_HEADERS_DIR}")

# The rest with JS
set(JS_LIBS AppRuntime Canvas Console ExternalTexture Graphics JsRuntime napi NativeCamera NativeCapture NativeEngine NativeInput NativeOptimizations NativeTracing NativeXr ScriptLoader UrlLib Window XMLHttpRequest)
import_prebuilt_targets(${JS_LIBS})

add_library(babylonnative_js INTERFACE IMPORTED GLOBAL)
target_link_libraries(babylonnative_js INTERFACE ${JS_LIBS})

# Aggregate
add_library(babylonnative_prebuilt INTERFACE IMPORTED GLOBAL)
target_link_libraries(babylonnative_prebuilt INTERFACE babylonnative_static babylonnative_js)