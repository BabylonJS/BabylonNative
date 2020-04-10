#pragma once

#include <napi/env.h>

namespace Babylon
{
    void InitializeCmdArgs(Napi::Env env, const std::vector<std::string>& args);
}
