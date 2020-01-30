#pragma once

#include "napi.h"

namespace Napi
{
    // TODO: Move this to a JS-specific location.
    Napi::Env Attach();

    void Detach(Napi::Env);

    Napi::Value Eval(Napi::Env env, const char* source, const char* sourceUrl);
}