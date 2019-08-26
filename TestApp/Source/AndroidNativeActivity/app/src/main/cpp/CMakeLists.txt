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
set(BABYLON_NATIVE_PLATFORM "Android")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(ANDROID_STL "c++_shared")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 ")

# build native_app_glue as a static lib
set(${CMAKE_C_FLAGS}, "${CMAKE_C_FLAGS}")
add_library(native_app_glue STATIC ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c)

set(BABYLON_NATIVE_TESTAPP_DIR "${CMAKE_CURRENT_LIST_DIR}/../../../../..")
add_subdirectory(${BABYLON_NATIVE_TESTAPP_DIR}/../../Library/ ${BABYLON_NATIVE_TESTAPP_DIR}/../../build/Android_${CMAKE_ANDROID_ARCH_ABI}/)

# Export ANativeActivity_onCreate(),
# Refer to: https://github.com/android-ndk/ndk/issues/381.
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -u ANativeActivity_onCreate")

add_library(native-activity SHARED main.cpp
        ${BABYLON_NATIVE_TESTAPP_DIR}/../../Library/Dependencies/napi/source/js_native_api_v8.cc
        ${BABYLON_NATIVE_TESTAPP_DIR}/../../Library/Dependencies/napi/source/env_v8.cc
        ${BABYLON_NATIVE_TESTAPP_DIR}/../../Library/Dependencies/napi/source/env.cc
        ${BABYLON_NATIVE_TESTAPP_DIR}/Shared/InputManager.cpp
        )

target_include_directories(native-activity PRIVATE
    ${ANDROID_NDK}/sources/android/native_app_glue
        ${BABYLON_NATIVE_TESTAPP_DIR}/../../Library/Include
        ${BABYLON_NATIVE_TESTAPP_DIR}/../../Library/Dependencies/v8/include
        ${BABYLON_NATIVE_TESTAPP_DIR}/../../Library/Dependencies/napi/include
        ${BABYLON_NATIVE_TESTAPP_DIR}/Shared
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
    Library
    ${CMAKE_CURRENT_LIST_DIR}/../jniLibs/${CMAKE_ANDROID_ARCH_ABI}/libv8.cr.so
    ${CMAKE_CURRENT_LIST_DIR}/../jniLibs/${CMAKE_ANDROID_ARCH_ABI}/libv8_libplatform.cr.so
    ${CMAKE_CURRENT_LIST_DIR}/../jniLibs/${CMAKE_ANDROID_ARCH_ABI}/libc++_shared.so)
