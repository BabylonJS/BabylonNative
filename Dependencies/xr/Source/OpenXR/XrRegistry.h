#pragma once

#include "XrSupportedExtensions.h"

namespace xr
{
    struct XrSessionContext
    {
        XrSessionContext();
        ~XrSessionContext();
        bool IsInitialized() const;
        XrInstance Instance() const;
        XrSystemId SystemId() const;
        XrTime DisplayTime() const;
        const std::unique_ptr<XrSupportedExtensions>& Extensions() const;
        const XrSession Session() const;
        const XrSessionState State() const;
        const XrSpace Space() const;

        struct Impl;
        std::unique_ptr<Impl> ContextImpl;
    };

    struct XrRegistry
    {
        static const XrSessionContext& Context();
    };
}