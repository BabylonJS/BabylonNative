#pragma once

#include <napi/napi.h>
#include <string>

namespace Napi
{
    // Gets the `stack` or `message` property from the error object.
    // The `message` property will be returned if the `stack` property is not defined.
    static inline std::string GetErrorString(const Napi::Error& error)
    {
        Napi::Value value = error.Get("stack");
        if (value.IsUndefined())
        {
            value = error.Get("message");
        }

        return value.ToString().Utf8Value();
    }
}
