#pragma once

#include "XrSupportedExtensions.h"
#include "Include/IXrContext.h"

namespace xr
{
    struct SceneUnderstanding;

    struct XrSessionContext : public IXrContext
    {
        XrSessionContext();
        ~XrSessionContext();

        virtual bool OPENXR_CONTEXT_INTERFACE_API IsInitialized() const override;
        virtual xr::ExtensionDispatchTable* OPENXR_CONTEXT_INTERFACE_API ExtensionDispatchTable() const override;
        virtual XrInstance OPENXR_CONTEXT_INTERFACE_API Instance() const override;
        virtual XrSystemId OPENXR_CONTEXT_INTERFACE_API SystemId() const override;
        virtual XrTime OPENXR_CONTEXT_INTERFACE_API DisplayTime() const override;
        virtual bool OPENXR_CONTEXT_INTERFACE_API IsExtensionEnabled(const char* name) const override;
        virtual XrSession OPENXR_CONTEXT_INTERFACE_API Session() const override;
        virtual XrSessionState OPENXR_CONTEXT_INTERFACE_API State() const override;
        virtual XrSpace OPENXR_CONTEXT_INTERFACE_API Space() const override;
        virtual bool OPENXR_CONTEXT_INTERFACE_API IsSessionRunning() const override;
        virtual XrResult OPENXR_CONTEXT_INTERFACE_API GetInstanceProcAddr(const char* name, PFN_xrVoidFunction* function) const override;

        const std::unique_ptr<XrSupportedExtensions>& XrSessionContext::Extensions() const;
        const SceneUnderstanding& SceneUnderstanding() const;

        struct Impl;
        std::unique_ptr<Impl> ContextImpl;
    };

    struct XrRegistry
    {
        static const XrSessionContext& Context();
        static uintptr_t GetNativeExtension();
        static std::string GetNativeExtensionName();
    };
}