cmake_minimum_required(VERSION 3.13.2)
# set targetPlatform, will be passed in from gradle when this sample is completed
# openGL Supportability
# platform         status
#   (0 12)           ES2/ES3 not supported
#   [12, 18)         ES2 only; for ES3, app do dynamic load/detection
#                    this applies to the situations that:
#                        - minimum API is set to less than 18. In this case
#                          there is no ES3 header/lib support inside NDK
#                        - the built APK might be running on newer API phones
#                    with dynamic loading of ES3, the same APK would still be able
#                    to use ES3. Otherwise, app would stuck with ES2 even phone is
#                    is newer than the minimum API level (for example, Android-27 etc).
#
#   [18, 24)         ES2 & ES3
#                    If app is built to only support API-18 or later,
#                    set minimum api level to 18 is good enough, NDK supprts ES3
#                    with the right header and lib files. No need to use ES3 dynamic
#                    detection.
#   [24, infinite)   ES2 & ES3 & Vulkan
project(ReactNativeBabylon)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
set(BABYLON_NATIVE_PLATFORM "Android")
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
file(TO_CMAKE_PATH "${REACTNATIVE_DIR}" REACTNATIVE_DIR_CMAKE)

# Configure Babylon Native to use JSI
set(NAPI_JAVASCRIPT_ENGINE "JSI" CACHE STRING "The JavaScript engine to power N-API")
add_subdirectory(${REACTNATIVE_DIR_CMAKE}/ReactCommon/jsi/jsi ${CMAKE_CURRENT_BINARY_DIR}/jsi)
target_include_directories(jsi INTERFACE ${REACTNATIVE_DIR_CMAKE}/ReactCommon/jsi)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/src/")

set(BABYLON_NATIVE_DIR "${CMAKE_CURRENT_LIST_DIR}/../submodules/BabylonNative")
add_subdirectory(${BABYLON_NATIVE_DIR} ${BABYLON_NATIVE_DIR}/build/Android_${CMAKE_ANDROID_ARCH_ABI}/)

set(BABYLON_REACT_NATIVE_SHARED_DIR "${CMAKE_CURRENT_LIST_DIR}/../shared")
add_subdirectory(${BABYLON_REACT_NATIVE_SHARED_DIR} ${CMAKE_CURRENT_BINARY_DIR}/shared)

add_library(fbjni SHARED IMPORTED)
set_target_properties(fbjni PROPERTIES
    IMPORTED_LOCATION ${FBJNI_LIBPATH}/${ANDROID_ABI}/libfbjni.so
    INTERFACE_INCLUDE_DIRECTORIES ${FBJNI_INCPATH})

# Define a minimal version of libturbomodulejsijni.so that includes CallInvokerHolder.cpp.
# This is the only part of the TurboModule system we need for now. Eventually when TurboModule
# support ships with React Native, we'll need to strip this back out.
list(APPEND TURBOMODULE_INC_DIRS "${REACTNATIVE_DIR_CMAKE}/ReactCommon/callinvoker")
list(APPEND TURBOMODULE_INC_DIRS "${REACTNATIVE_DIR_CMAKE}/ReactAndroid/src/main/java/com/facebook/react/turbomodule/core/jni")
add_library(turbomodulejsijni SHARED
    ${REACTNATIVE_DIR_CMAKE}/ReactAndroid/src/main/java/com/facebook/react/turbomodule/core/jni/ReactCommon/CallInvokerHolder.cpp)
target_include_directories(turbomodulejsijni PUBLIC "${TURBOMODULE_INC_DIRS}")
target_link_libraries(turbomodulejsijni
    fbjni)

add_library(BabylonNative SHARED
    src/main/cpp/BabylonNativeInterop.cpp)

target_link_libraries(BabylonNative
    GLESv3
    android
    EGL
    log
    -lz
    arcana
    fbjni
    jsi
    turbomodulejsijni
    BabylonReactNativeShared
    AndroidExtensions
    Graphics
    JsRuntime
    NativeEngine
    NativeInput
    NativeXr
    Window
    XMLHttpRequest)
