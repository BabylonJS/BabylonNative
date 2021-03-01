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

typedef struct IXrContextOpenXR
{
    virtual bool OPENXR_CONTEXT_INTERFACE_API IsInitialized() const = 0;
    virtual xr::ExtensionDispatchTable* OPENXR_CONTEXT_INTERFACE_API ExtensionDispatchTable() const = 0;
    virtual XrInstance OPENXR_CONTEXT_INTERFACE_API Instance() const = 0;
    virtual XrSystemId OPENXR_CONTEXT_INTERFACE_API SystemId() const = 0;
    virtual XrTime OPENXR_CONTEXT_INTERFACE_API DisplayTime() const = 0;
    virtual bool OPENXR_CONTEXT_INTERFACE_API IsExtensionEnabled(const char* name) const = 0;
    virtual XrSession OPENXR_CONTEXT_INTERFACE_API Session() const = 0;
    virtual XrSessionState OPENXR_CONTEXT_INTERFACE_API State() const = 0;
    virtual XrSpace OPENXR_CONTEXT_INTERFACE_API Space() const = 0;
    virtual bool OPENXR_CONTEXT_INTERFACE_API IsSessionRunning() const = 0;
    virtual XrResult OPENXR_CONTEXT_INTERFACE_API GetInstanceProcAddr(const char* name, PFN_xrVoidFunction* function) const = 0;
} IXrContext;