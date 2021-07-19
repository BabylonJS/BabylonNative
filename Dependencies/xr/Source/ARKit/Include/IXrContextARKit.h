#pragma once

#include <ARKit/ARKit.h>

typedef struct IXrContextARKit
{
    virtual bool IsInitialized() const = 0;
    virtual ARSession* XrSession() const = 0;
    virtual ARFrame* XrFrame() const = 0;
} IXrContextARKit;