#pragma once

#include <napi/napi.h>
struct JSContext;

namespace Napi
{
  Napi::Env Attach(JSContext* context);

  void Detach(Napi::Env);

  Napi::Value Eval(Napi::Env env, const char* source, const char* sourceUrl);
  
  JSContext* GetContext(Napi::Env);
}
