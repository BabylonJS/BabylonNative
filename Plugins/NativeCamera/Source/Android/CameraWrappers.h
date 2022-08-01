#pragma once

#include <android/api-level.h>
#include <dlfcn.h>
#include <map>

// The camera API's in the NDK are only supported in API24 and higher. We
// use dlopen and dlsym to link the camera lib at runtime only if the current
// device supports it. In order to avoid redefining the type information the
// below macro's forcefully import the camera header files as if the min supported
// SDK was set to 24.
#define __ORIG_ANDROID_API__ __ANDROID_API__
#undef __ANDROID_API__
#define __ANDROID_API__ 24
namespace API24 {
    #include <camera/NdkCameraManager.h>
    #include <camera/NdkCameraCaptureSession.h>
    #include <camera/NdkCameraDevice.h>
    #include <camera/NdkCameraError.h>
    #include <camera/NdkCameraManager.h>
    #include <camera/NdkCameraMetadata.h>
    #include <camera/NdkCameraMetadataTags.h>
    #include <camera/NdkCameraWindowType.h>
    #include <camera/NdkCaptureRequest.h>
}
#undef __ANDROID_API__
#define __ANDROID_API__ __ORIG_ANDROID_API__
#undef __ORIG_ANDROID_API__

// Helper macro to make calling camera NDK api's more type safe
#define GET_CAMERA_FUNCTION(function) reinterpret_cast<decltype(&API24::function)>(Babylon::Plugins::GetCameraDynamicFunction(#function))

namespace
{
    const int API_LEVEL{ android_get_device_api_level() };

    // Load the NDK camera lib dynamically. When running on OS 6.0 and below this will return nullptr,
    // on OS 7.0 and up this should return a pointer to access the library using dlsym. It is technically fine
    // to call dlopen when the API_LEVEL is below 24, but it's wasted effort on those devices.
    void* libCamera2NDK{ API_LEVEL >= 24 ? dlopen("libcamera2ndk.so", RTLD_NOW): nullptr };

    std::map<std::string, void*> CameraDynamicFunctions{};
}

namespace Babylon::Plugins {
    inline void* GetCameraDynamicFunction(const char* functionName) {
        if (!libCamera2NDK)
        {
            return nullptr;
        }
        auto cameraDynamicFunctionIt{CameraDynamicFunctions.find(functionName)};
        if (cameraDynamicFunctionIt == CameraDynamicFunctions.end())
        {
            cameraDynamicFunctionIt = CameraDynamicFunctions.emplace(functionName, dlsym(libCamera2NDK, functionName)).first;
        }

        return cameraDynamicFunctionIt->second;
    }
}