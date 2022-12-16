#pragma once

#include "env.h"

namespace Napi::JavaScriptCore
{
  Napi::Env Attach(JSGlobalContextRef);

  void Detach(Napi::Env);

  JSGlobalContextRef GetContext(Napi::Env);
}
