#pragma once

#ifndef __clang__
#pragma warning(push)
#pragma warning(disable: 4100 4267 4127)
#endif

#include <v8.h>
#ifdef V8_USE_INSPECTOR
#include <v8-inspector.h>
#endif
#include <libplatform/libplatform.h>

#ifndef __clang__
#pragma warning(pop)
#endif