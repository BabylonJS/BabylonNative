#pragma once

#include "env.h"

namespace Napi::Chakra
{
  Napi::Env Attach();

  void Detach(Napi::Env);
}
