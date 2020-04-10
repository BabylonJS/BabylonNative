#pragma once

#include <napi/env.h>

namespace Babylon
{
    void InitializeCmdArgs(Napi::Env env, int argc, char** argv);
}
