#pragma once

#include <openxr/openxr.h>
#include <openxr/openxr_platform.h>
#include <XrUtility/XrExtensions.h>

#if defined(__CYGWIN32__)
    #define OPENXR_CONTEXT_INTERFACE_API __stdcall
#elif defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(_WIN64) || defined(WINAPI_FAMILY)
    #define OPENXR_CONTEXT_INTERFACE_API __stdcall
#elif defined(__MACH__) || defined(__ANDROID__) || defined(__linux__) || defined(LUMIN)
    #define OPENXR_CONTEXT_INTERFACE_API
#else
    #define OPENXR_CONTEXT_INTERFACE_API
#endif

typedef struct IXrContext
{
    bool (OPENXR_CONTEXT_INTERFACE_API * IsInitialized)();
    xr::ExtensionDispatchTable (OPENXR_CONTEXT_INTERFACE_API* Extensions)();
    XrInstance (OPENXR_CONTEXT_INTERFACE_API * Instance)();
    XrSystemId (OPENXR_CONTEXT_INTERFACE_API * SystemId)();
    XrTime (OPENXR_CONTEXT_INTERFACE_API * DisplayTime)();
    bool (OPENXR_CONTEXT_INTERFACE_API * TryEnableExtension)(const char* name);
    XrSession (OPENXR_CONTEXT_INTERFACE_API * Session)();
    XrSessionState (OPENXR_CONTEXT_INTERFACE_API * State)();
    XrSpace (OPENXR_CONTEXT_INTERFACE_API * Space)();
    bool (OPENXR_CONTEXT_INTERFACE_API * IsSessionRunning)();
} IXrContext;