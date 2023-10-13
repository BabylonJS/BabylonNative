# TODO: compute path based on build type and ABI
set(PREBUILT_DIR "${babylonnative_SOURCE_DIR}/Windows/RelWithDebInfo/x64")
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
import_prebuilt_targets(bgfx bx bimg)
import_prebuilt_targets(astc-encoder edtaa3 etc1 etc2 iqa nvtt pvrtc squish tinyexr)
target_link_libraries(bimg INTERFACE astc-encoder edtaa3 etc1 etc2 iqa nvtt pvrtc squish tinyexr)

# spirv-cross
import_prebuilt_targets(spirv-cross-core spirv-cross-glsl)
target_link_libraries(spirv-cross-glsl INTERFACE spirv-cross-core)
import_prebuilt_target(spirv-cross-hlsl)
target_link_libraries(spirv-cross-hlsl INTERFACE spirv-cross-core spirv-cross-glsl)

# glslang
import_prebuilt_targets(GenericCodeGen glslang OGLCompiler OSDependent MachineIndependent SPIRV glslang-default-resource-limits)
target_link_libraries(glslang INTERFACE OGLCompiler GenericCodeGen OSDependent MachineIndependent SPIRV glslang-default-resource-limits)

# openxr_loader
import_prebuilt_targets(openxr_loader)

# static/no JS babylon native
add_library(babylonnative_static INTERFACE IMPORTED GLOBAL)
target_link_libraries(babylonnative_static INTERFACE openxr_loader glslang spirv-cross-hlsl bimg bx bgfx)
target_include_directories(babylonnative_static INTERFACE "${PREBUILT_HEADERS_DIR}")

# The rest with JS
set(JS_LIBS AppRuntime arcana Canvas Console ExternalTexture Graphics JsRuntime napi NativeCamera NativeCapture NativeEngine NativeInput NativeOptimizations NativeTracing NativeXr ScriptLoader UrlLib Window XMLHttpRequest)
import_prebuilt_targets(${JS_LIBS})

add_library(babylonnative_js INTERFACE IMPORTED GLOBAL)
target_link_libraries(babylonnative_js INTERFACE ${JS_LIBS})

# Aggregate
add_library(babylonnative INTERFACE IMPORTED GLOBAL)
target_link_libraries(babylonnative INTERFACE babylonnative_static babylonnative_js)