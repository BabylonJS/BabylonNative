#pragma once

#include <arcore_c_api.h>

typedef struct IXrContextARCore
{
    virtual bool IsInitialized() const = 0;
    virtual ArSession* XrSession() const = 0;
    virtual ArFrame* XrFrame() const = 0;
} IXrContextARCore;