#pragma once

#include <napi/env.h>
#include <napi/napi.h>
#include <Babylon/Api.h>
#include <vector>
#include <string>

namespace Babylon::Polyfills::Blob
{
    void BABYLON_API Initialize(Napi::Env env);

    Napi::Value BABYLON_API CreateInstance(
        Napi::Env env,
        std::vector<std::byte> data,
        std::string type);
}
