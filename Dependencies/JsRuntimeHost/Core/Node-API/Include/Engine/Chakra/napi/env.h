#pragma once

#include <napi/napi.h>

namespace Napi
{
  Napi::Env Attach();

  void Detach(Napi::Env);

  Napi::Value Eval(Napi::Env env, const char* source, const char* sourceUrl);
}
