if(WIN32)
    get_filename_component(PREBUILT_SYSTEM_ROOT "${CMAKE_CURRENT_LIST_DIR}/Windows" ABSOLUTE)
    if(${CMAKE_CXX_COMPILER} MATCHES "x86/cl.exe$")
        set(CPU_ARCH "x86")
    elseif(${CMAKE_CXX_COMPILER} MATCHES "x64/cl.exe$")
        set(CPU_ARCH "x64")
    elseif(${CMAKE_CXX_COMPILER} MATCHES "arm/cl.exe$")
        set(CPU_ARCH "ARM")
    elseif(${CMAKE_CXX_COMPILER} MATCHES "arm64/cl.exe$")
        set(CPU_ARCH "ARM64")
    else()
        message(FATAL_ERROR "Unrecognized compiler. Unable to get CPU_ARCH.")
    endif()
else()
    message(FATAL_ERROR "Unrecognized platform: ${CMAKE_SYSTEM_NAME}")
endif()

set(PREBUILT_DIR_DEBUG "${PREBUILT_SYSTEM_ROOT}/Debug/${CPU_ARCH}/lib")
set(PREBUILT_DIR_RELEASE "${PREBUILT_SYSTEM_ROOT}/Release/${CPU_ARCH}/lib")
set(PREBUILT_DIR_RELWITHDEBINFO "${PREBUILT_SYSTEM_ROOT}/RelWithDebInfo/${CPU_ARCH}/lib")

function(import_prebuilt_targets namespace debug_postfix)
    foreach(target IN LISTS ARGN)
        if(namespace STREQUAL "")
            add_library(${target} STATIC IMPORTED GLOBAL)
            set_property(TARGET ${target} PROPERTY IMPORTED_LOCATION_DEBUG "${PREBUILT_DIR_DEBUG}/${target}${debug_postfix}.lib")
            set_property(TARGET ${target} PROPERTY IMPORTED_LOCATION_RELEASE "${PREBUILT_DIR_RELEASE}/${target}.lib")
            set_property(TARGET ${target} PROPERTY IMPORTED_LOCATION_RELWITHDEBINFO "${PREBUILT_DIR_RELWITHDEBINFO}/${target}.lib")
            set_target_properties(${target} PROPERTIES MAP_IMPORTED_CONFIG_MINSIZEREL Release)
        else()
            add_library("${namespace}::${target}" STATIC IMPORTED GLOBAL)
            set_property(TARGET "${namespace}::${target}" PROPERTY IMPORTED_LOCATION_DEBUG "${PREBUILT_DIR_DEBUG}/${target}${debug_postfix}.lib")
            set_property(TARGET "${namespace}::${target}" PROPERTY IMPORTED_LOCATION_RELEASE "${PREBUILT_DIR_RELEASE}/${target}.lib")
            set_property(TARGET "${namespace}::${target}" PROPERTY IMPORTED_LOCATION_RELWITHDEBINFO "${PREBUILT_DIR_RELWITHDEBINFO}/${target}.lib")
            set_target_properties("${namespace}::${target}" PROPERTIES MAP_IMPORTED_CONFIG_MINSIZEREL Release)
        endif()
    endforeach()
endfunction()

function(target_link_libraries_ns interface namespace)
    foreach(target IN LISTS ARGN)
        target_link_libraries(${interface} INTERFACE ${namespace}::${target})
    endforeach()
endfunction()

# bgfx
import_prebuilt_targets("" "" bgfx bx bimg)
set(BIMG_IMAGE_DECODERS astc-encoder edtaa3 etc1 etc2 iqa nvtt pvrtc squish tinyexr)
import_prebuilt_targets("BN" "" ${BIMG_IMAGE_DECODERS})
target_link_libraries_ns(bimg "BN"  ${BIMG_IMAGE_DECODERS})

# spirv-cross
import_prebuilt_targets("" "d" spirv-cross-core spirv-cross-glsl spirv-cross-hlsl)
target_link_libraries(spirv-cross-glsl INTERFACE spirv-cross-core)
target_link_libraries(spirv-cross-hlsl INTERFACE spirv-cross-core spirv-cross-glsl)

# glslang
import_prebuilt_targets("" "d" glslang SPIRV glslang-default-resource-limits)
import_prebuilt_targets("BN" "d" GenericCodeGen OGLCompiler OSDependent MachineIndependent)
target_link_libraries_ns(glslang "BN" OGLCompiler GenericCodeGen OSDependent MachineIndependent)
target_link_libraries(glslang INTERFACE SPIRV glslang-default-resource-limits)

# openxr_loader
import_prebuilt_targets("" "d" openxr_loader)

# arcana
import_prebuilt_targets("BN" "" arcana)

# static/no JS babylon native
add_library(babylonnative_static INTERFACE IMPORTED GLOBAL)
target_link_libraries(babylonnative_static INTERFACE openxr_loader glslang spirv-cross-hlsl bimg bx bgfx BN::arcana)
target_include_directories(babylonnative_static INTERFACE "${PREBUILT_HEADERS_DIR}")

# The rest with JS
set(JS_LIBS AppRuntime Canvas Console ExternalTexture Graphics JsRuntime napi NativeCamera NativeCapture NativeEngine NativeInput NativeOptimizations NativeTracing NativeXr ScriptLoader UrlLib Window XMLHttpRequest)
import_prebuilt_targets("BN" "" ${JS_LIBS})

add_library(babylonnative_js INTERFACE IMPORTED GLOBAL)
target_link_libraries_ns(babylonnative_js "BN" ${JS_LIBS})

# Aggregate
add_library(babylonnative_prebuilt INTERFACE IMPORTED GLOBAL)
target_link_libraries(babylonnative_prebuilt INTERFACE babylonnative_static babylonnative_js)