#pragma once

#include <napi/napi.h>
#include <v8.h>

namespace Napi
{
  Napi::Env Attach(v8::Local<v8::Context>);

  void Detach(Napi::Env);

  Napi::Value Eval(Napi::Env env, const char* source, const char* sourceUrl);

  v8::Local<v8::Context> GetContext(Napi::Env);
}
