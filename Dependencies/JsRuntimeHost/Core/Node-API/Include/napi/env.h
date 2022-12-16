#pragma once

#include "napi.h"

namespace Napi
{
  Napi::Value Eval(Napi::Env env, const char* source, const char* sourceUrl);
}
