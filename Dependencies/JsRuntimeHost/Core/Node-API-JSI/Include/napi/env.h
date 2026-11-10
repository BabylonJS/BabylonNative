#pragma once

#include "napi.h"
#include <jsi/jsi.h>

namespace Napi
{
  Napi::Env Attach(facebook::jsi::Runtime&);

  void Detach(Napi::Env);

  Napi::Value Eval(Napi::Env env, const char* source, const char* sourceUrl);
}
