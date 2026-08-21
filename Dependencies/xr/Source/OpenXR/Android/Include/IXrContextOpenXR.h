#pragma once

#include <openxr/openxr.h>

typedef struct IXrContextOpenXR
{
    virtual bool IsInitialized() const = 0;
    virtual XrInstance Instance() const = 0;
    virtual XrSession Session() const = 0;
    virtual ~IXrContextOpenXR() = default;
} IXrContextOpenXR;
