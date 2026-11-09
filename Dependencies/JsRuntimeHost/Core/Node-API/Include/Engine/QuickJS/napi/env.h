#pragma once

#include <napi/napi.h>
#include <string_view>
struct JSContext;

namespace Napi
{
  Napi::Env Attach(JSContext* context);

  void Detach(Napi::Env);

  Napi::Value Eval(Napi::Env env, const char* source, const char* sourceUrl);
  Napi::Value Eval(Napi::Env env, std::string_view source, const char* sourceUrl);
  
  JSContext* GetContext(Napi::Env);
}
