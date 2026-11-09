#pragma once

#include <napi/napi.h>
#include <string_view>

namespace Napi
{
  Napi::Env Attach();

  void Detach(Napi::Env);

  Napi::Value Eval(Napi::Env env, const char* source, const char* sourceUrl);
  Napi::Value Eval(Napi::Env env, std::string_view source, const char* sourceUrl);
}
