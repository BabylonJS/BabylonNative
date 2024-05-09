
#pragma once

#include <ARKit/ARKit.h>

typedef struct IXrContextARKit
{
    virtual bool IsInitialized() const = 0;
    virtual ar_session_t XrSession() const = 0;
} IXrContextARKit;
