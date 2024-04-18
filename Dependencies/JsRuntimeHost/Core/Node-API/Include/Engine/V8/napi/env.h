#pragma once

#include <napi/napi.h>

// Enable V8 Pointer Compression
// https://v8.dev/blog/pointer-compression
// https://stackoverflow.com/q/62921373
#ifndef V8_COMPRESS_POINTERS
#define V8_COMPRESS_POINTERS 1
#endif

// Enable V8 Sandbox
// https://v8.dev/blog/sandbox
#ifndef V8_ENABLE_SANDBOX
#define V8_ENABLE_SANDBOX 1
#endif

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
#endif

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
#endif

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4100) // unreferenced formal parameter
#pragma warning(disable: 4127) // Suppress warning in v8-internal.h, Line 317 inside of V8_COMPRESS_POINTERS conditional
#endif

#include <v8.h>

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

#ifdef __clang__
#pragma clang diagnostic pop
#endif

namespace Napi
{
  Napi::Env Attach(v8::Local<v8::Context>);

  void Detach(Napi::Env);

  Napi::Value Eval(Napi::Env env, const char* source, const char* sourceUrl);

  v8::Local<v8::Context> GetContext(Napi::Env);
}
