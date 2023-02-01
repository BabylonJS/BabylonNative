#pragma once

#include "env.h"
#include <JavaScriptCore/JavaScript.h>

namespace Napi::JavaScriptCore
{
  Napi::Env Attach(JSGlobalContextRef);

  void Detach(Napi::Env);

  JSGlobalContextRef GetContext(Napi::Env);
}
