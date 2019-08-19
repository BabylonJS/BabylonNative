#
# Copyright (C) The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

cmake_minimum_required(VERSION 3.4.1)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(ANDROID_STL "c++_shared")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17  -latomic")

# build native_app_glue as a static lib
set(${CMAKE_C_FLAGS}, "${CMAKE_C_FLAGS}")
add_library(native_app_glue STATIC
    ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c
        )


# Export ANativeActivity_onCreate(),
# Refer to: https://github.com/android-ndk/ndk/issues/381.
set(CMAKE_SHARED_LINKER_FLAGS
    "${CMAKE_SHARED_LINKER_FLAGS} -u ANativeActivity_onCreate")

add_library(native-activity SHARED main.cpp
        ${CMAKE_CURRENT_LIST_DIR}/../../../../../../../Library/Dependencies/napi/source/js_native_api_v8.cc
        ${CMAKE_CURRENT_LIST_DIR}/../../../../../../../Library/Dependencies/napi/source/env_v8.cc
        ${CMAKE_CURRENT_LIST_DIR}/../../../../../../../Library/Dependencies/napi/source/env.cc
        ${CMAKE_CURRENT_LIST_DIR}/../../../../../Shared/InputManager.cpp
        )

target_include_directories(native-activity PRIVATE
    ${ANDROID_NDK}/sources/android/native_app_glue
        ${CMAKE_CURRENT_LIST_DIR}/../../../../../../../Library/Include
        ${CMAKE_CURRENT_LIST_DIR}/../../../../../../../Library/Dependencies/v8/include
        ${CMAKE_CURRENT_LIST_DIR}/../../../../../../../Library/Dependencies/napi/include
        ${CMAKE_CURRENT_LIST_DIR}/../../../../../Shared
        )
target_compile_definitions(native-activity PUBLIC NODE_ADDON_API_DISABLE_DEPRECATED)

add_definitions(-DANDROID_STL=c++_shared)
# add lib dependencies
target_link_libraries(native-activity
    android
    native_app_glue
    EGL
    GLESv3
    -lz
    m
    log
        ${CMAKE_CURRENT_LIST_DIR}/../../../../../../../build/Android_${CMAKE_ANDROID_ARCH_ABI}/libLibrary.a # BabylonNative
        ${CMAKE_CURRENT_LIST_DIR}/../../../../../../../build/Library/Dependencies/bgfx.cmake/libbgfx.a
        ${CMAKE_CURRENT_LIST_DIR}/../../../../../../../build/Library/Dependencies/bgfx.cmake/libbimg.a
        ${CMAKE_CURRENT_LIST_DIR}/../../../../../../../build/Library/Dependencies/bgfx.cmake/libbx.a
        ${CMAKE_CURRENT_LIST_DIR}/../../../../../../../build/Library/Dependencies/bgfx.cmake/libedtaa3.a
        ${CMAKE_CURRENT_LIST_DIR}/../../../../../../../build/Library/Dependencies/bgfx.cmake/libetc1.a
        ${CMAKE_CURRENT_LIST_DIR}/../../../../../../../build/Library/Dependencies/bgfx.cmake/libetc2.a
        ${CMAKE_CURRENT_LIST_DIR}/../../../../../../../build/Library/Dependencies/bgfx.cmake/libiqa.a
        ${CMAKE_CURRENT_LIST_DIR}/../../../../../../../build/Library/Dependencies/bgfx.cmake/libnvtt.a
        ${CMAKE_CURRENT_LIST_DIR}/../../../../../../../build/Library/Dependencies/bgfx.cmake/libpvrtc.a
        ${CMAKE_CURRENT_LIST_DIR}/../../../../../../../build/Library/Dependencies/bgfx.cmake/libsquish.a
        ${CMAKE_CURRENT_LIST_DIR}/../../../../../../../build/Library/Dependencies/bgfx.cmake/libastc.a
        ${CMAKE_CURRENT_LIST_DIR}/../../../../../../../build/Library/Dependencies/bgfx.cmake/libastc-codec.a
        ${CMAKE_CURRENT_LIST_DIR}/../../../../../../../build/Library/Dependencies/SPIRV-Cross/libspirv-cross-core.a
        ${CMAKE_CURRENT_LIST_DIR}/../../../../../../../build/Library/Dependencies/SPIRV-Cross/libspirv-cross-glsl.a
        ${CMAKE_CURRENT_LIST_DIR}/../../../../../../../build/Library/Dependencies/SPIRV-Cross/libspirv-cross-hlsl.a
        ${CMAKE_CURRENT_LIST_DIR}/../../../../../../../build/Library/Dependencies/glslang/glslang/libglslang.a
        ${CMAKE_CURRENT_LIST_DIR}/../../../../../../../build/Library/Dependencies/glslang/glslang/OSDependent/Unix/libOSDependent.a
        ${CMAKE_CURRENT_LIST_DIR}/../../../../../../../build/Library/Dependencies/glslang/OGLCompilersDLL/libOGLCompiler.a
        ${CMAKE_CURRENT_LIST_DIR}/../../../../../../../build/Library/Dependencies/glslang/hlsl/libHLSL.a
        ${CMAKE_CURRENT_LIST_DIR}/../../../../../../../build/Library/Dependencies/glslang/SPIRV/libSPIRV.a
        ${CMAKE_CURRENT_LIST_DIR}/../../../../../../../build/Library/Dependencies/curl/lib/libcurl.a
        ${CMAKE_CURRENT_LIST_DIR}/../jniLibs/${CMAKE_ANDROID_ARCH_ABI}/libv8.cr.so
        ${CMAKE_CURRENT_LIST_DIR}/../jniLibs/${CMAKE_ANDROID_ARCH_ABI}/libv8_libplatform.cr.so
        ${CMAKE_CURRENT_LIST_DIR}/../jniLibs/${CMAKE_ANDROID_ARCH_ABI}/libc++_shared.so
        )
