#pragma once

#include "XrSupportedExtensions.h"
#include "Include/IXrContextOpenXR.h"

namespace xr
{
    struct SceneUnderstanding;

    struct XrSessionContext : public IXrContextOpenXR
    {
        XrSessionContext();
        ~XrSessionContext();

        bool OPENXR_CONTEXT_INTERFACE_API IsInitialized() const override;
        xr::ExtensionDispatchTable* OPENXR_CONTEXT_INTERFACE_API ExtensionDispatchTable() const override;
        XrInstance OPENXR_CONTEXT_INTERFACE_API Instance() const override;
        XrSystemId OPENXR_CONTEXT_INTERFACE_API SystemId() const override;
        XrTime OPENXR_CONTEXT_INTERFACE_API DisplayTime() const override;
        bool OPENXR_CONTEXT_INTERFACE_API TryEnableExtension(const char* name) const override;
        bool OPENXR_CONTEXT_INTERFACE_API IsExtensionEnabled(const char* name) const override;
        XrSession OPENXR_CONTEXT_INTERFACE_API Session() const override;
        XrSessionState OPENXR_CONTEXT_INTERFACE_API State() const override;
        XrSpace OPENXR_CONTEXT_INTERFACE_API Space() const override;
        bool OPENXR_CONTEXT_INTERFACE_API IsSessionRunning() const override;
        XrResult OPENXR_CONTEXT_INTERFACE_API GetInstanceProcAddr(const char* name, PFN_xrVoidFunction* function) const override;

        const std::unique_ptr<XrSupportedExtensions>& XrSessionContext::Extensions() const;
        const SceneUnderstanding& SceneUnderstanding() const;

        struct Impl;
        std::unique_ptr<Impl> ContextImpl;
    };

    struct XrRegistry
    {
        static const XrSessionContext& Context();
        static uintptr_t GetNativeXrContext();
        static std::string GetNativeXrContextType();
        static void Reset();
    };
}