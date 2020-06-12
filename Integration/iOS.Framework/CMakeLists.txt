set(BABYLONSCRIPTS
    "../../Apps/BabylonScripts/babylon.glTF2FileLoader.js"
    "../../Apps/BabylonScripts/babylon.max.js"
    "../../Apps/BabylonScripts/babylonjs.materials.js"
    "../../Apps/BabylonScripts/ammo.js"
    "../../Apps/BabylonScripts/meshwriter.min.js"
    "../../Apps/BabylonScripts/recast.js")

set(SCRIPTS
    "../../Apps/Playground/Scripts/experience.js"
    "../../Apps/Playground/Scripts/playground_runner.js")

set(SOURCES
    "Source/Framework.cpp"
    "Include/napi.h")

set(RESOURCE_FILES ${STORYBOARD} ${BABYLONSCRIPTS} ${SCRIPTS})

add_library(BabylonNative ${SOURCES} ${RESOURCE_FILES})
warnings_as_errors(BabylonNative)

target_compile_definitions(BabylonNative PRIVATE UNICODE)
target_compile_definitions(BabylonNative PRIVATE _UNICODE)

target_include_directories(BabylonNative PUBLIC "Source" ".")
file(GLOB_RECURSE appruntime_headers ${CMAKE_CURRENT_SOURCE_DIR}/../Core/AppRuntime/Include/Babylon/*.h)
file(GLOB_RECURSE jsruntime_headers ${CMAKE_CURRENT_SOURCE_DIR}/../Core/JsRuntime/Include/Babylon/*.h)
file(GLOB_RECURSE scriptloader_headers ${CMAKE_CURRENT_SOURCE_DIR}/../Core/ScriptLoader/Include/Babylon/*.h)

file(GLOB_RECURSE nativewindow_headers ${CMAKE_CURRENT_SOURCE_DIR}/../Plugins/NativeWindow/Include/Babylon/*.h)
file(GLOB_RECURSE nativeengine_headers ${CMAKE_CURRENT_SOURCE_DIR}/../Plugins/NativeEngine/Include/Babylon/*.h)
file(GLOB_RECURSE nativeinput_headers ${CMAKE_CURRENT_SOURCE_DIR}/../Plugins/NativeInput/Include/Babylon/*.h)
#file(GLOB_RECURSE nativexr_headers ${CMAKE_CURRENT_SOURCE_DIR}/../Plugins/NativeXr/Include/Babylon/*.h)

file(GLOB_RECURSE console_headers ${CMAKE_CURRENT_SOURCE_DIR}/../Polyfills/Console/Include/Babylon/*.h)
file(GLOB_RECURSE window_headers ${CMAKE_CURRENT_SOURCE_DIR}/../Polyfills/Window/Include/Babylon/*.h)
file(GLOB_RECURSE xmlhttprequest_headers ${CMAKE_CURRENT_SOURCE_DIR}/../Polyfills/XMLHttpRequest/Include/Babylon/*.h)

file(GLOB_RECURSE napi_headers ${CMAKE_CURRENT_SOURCE_DIR}/../Dependencies/napi/Include/napi/*.h)

# copy headers
foreach(header ${appruntime_headers} 
        ${jsruntime_headers}
        ${scriptloader_headers}
        ${nativewindow_headers}
        ${nativeengine_headers}
        ${nativeinput_headers}
        #${nativexr_headers}
        ${console_headers}
        ${window_headers}
        ${xmlhttprequest_headers} )
    get_filename_component(HEADER_NAME "${header}" NAME)
    add_custom_command(TARGET BabylonNative PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy
        ${header}
        $<TARGET_FILE_DIR:BabylonNative>/Headers/${HEADER_NAME})
endforeach()

foreach(header ${napi_headers})
    get_filename_component(HEADER_NAME "${header}" NAME)
    if(NOT "${HEADER_NAME}" STREQUAL "napi.h")
        add_custom_command(TARGET BabylonNative PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy
            ${header}
            $<TARGET_FILE_DIR:BabylonNative>/Headers/${HEADER_NAME})
    endif()
endforeach()

# concatenate static libs
add_custom_command(TARGET BabylonNative POST_BUILD
        COMMAND libtool -static -o $<TARGET_FILE:BabylonNative> 
            $<TARGET_FILE:AppRuntime> 
            $<TARGET_FILE:JsRuntime> 
            $<TARGET_FILE:NativeWindow>
            $<TARGET_FILE:NativeEngine> 
            $<TARGET_FILE:Console> 
            $<TARGET_FILE:Window> 
            $<TARGET_FILE:ScriptLoader> 
            $<TARGET_FILE:XMLHttpRequest> 
            $<TARGET_FILE:NativeInput> 
            $<TARGET_FILE:napi> 
            $<TARGET_FILE:UrlLib> 
            $<TARGET_FILE:bgfx> 
            $<TARGET_FILE:bimg> 
            $<TARGET_FILE:bx> 
            $<TARGET_FILE:glslang> 
            $<TARGET_FILE:SPIRV> 
            $<TARGET_FILE:spirv-cross-core> 
            $<TARGET_FILE:spirv-cross-hlsl>
            $<TARGET_FILE:spirv-cross-msl>
            $<TARGET_FILE:spirv-cross-glsl>
            $<TARGET_FILE:OGLCompiler>
            $<TARGET_FILE:OSDependent>
            $<TARGET_FILE:astc>
            $<TARGET_FILE:astc-codec>
            #$<TARGET_FILE:NativeXr>
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        DEPENDS AppRuntime 
            NativeWindow 
            NativeEngine 
            #NativeXr
            Console 
            ScriptLoader 
            XMLHttpRequest 
            NativeInput 
            Window 
            JsRuntime
            napi
            UrlLib
            bgfx
            bimg
            bx
            glslang
            SPIRV
            spirv-cross-core
            spirv-cross-hlsl
            spirv-cross-msl
            spirv-cross-glsl
            OGLCompiler
            OSDependent
            astc
            astc-codec)

set_target_properties(BabylonNative PROPERTIES
    FRAMEWORK TRUE
    FRAMEWORK_VERSION C
    PUBLIC_HEADER "Include/napi.h"
    XCODE_ATTRIBUTE_CLANG_ENABLE_OBJC_ARC YES
    RESOURCE "${RESOURCE_FILES}"

    XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET 12.0
    XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER "com.BabylonNative.iOS"
    XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "iPhone Developer"
    XCODE_ATTRIBUTE_ENABLE_BITCODE NO

    XCODE_ATTRIBUTE_SWIFT_VERSION "4.0"
    XCODE_ATTRIBUTE_LD_RUNPATH_SEARCH_PATHS "@executable_path/Frameworks"
    XCODE_ATTRIBUTE_ALWAYS_EMBED_SWIFT_STANDARD_LIBRARIES YES

    # CMake seems to add a custom flag "-Wno-unknown-pragmas" to the Swift compiler. That flag is used for Clang,
    # So we need to make sure we override it with nothing here in order to compile Swift.
    XCODE_ATTRIBUTE_OTHER_SWIFT_FLAGS "")

    # Swift support
    set(CMAKE_Swift_COMPILER_FORCED TRUE)
    set(CMAKE_Swift_LANGUAGE_VERSION 4.0)
    enable_language(Swift) )
