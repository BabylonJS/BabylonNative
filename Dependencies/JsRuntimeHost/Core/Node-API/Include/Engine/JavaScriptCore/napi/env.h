#pragma once

#include <napi/napi.h>
#include <JavaScriptCore/JavaScript.h>

namespace Napi
{
  Napi::Env Attach(JSGlobalContextRef);

  void Detach(Napi::Env);

  Napi::Value Eval(Napi::Env env, const char* source, const char* sourceUrl);

  JSGlobalContextRef GetContext(Napi::Env);
}
