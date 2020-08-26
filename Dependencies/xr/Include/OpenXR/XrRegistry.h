#pragma once

#include "XrSupportedExtensions.h"

#ifndef XR_REGISTRY
#define XR_REGISTRY
namespace xr
{
    struct XrSessionContext
    {
        XrSessionContext();
        ~XrSessionContext();
        bool IsInitialized() const;
        XrTime GetDisplayTime() const;
        const std::unique_ptr<XrSupportedExtensions>& Extensions() const;
        const XrSession& Session() const;
        const XrSpace& Space() const;

        struct Impl;
        std::unique_ptr<Impl> ContextImpl;
    };

    struct XrRegistry
    {
    public:
        static const XrSessionContext& Context()
        {
            return s_context;
        }

    private:
        static XrSessionContext s_context;
    };
}
#endif