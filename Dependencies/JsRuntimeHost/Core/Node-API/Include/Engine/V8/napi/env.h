#pragma once

#include <napi/napi.h>

#include <functional>
#include <stdint.h>

#if INTPTR_MAX == INT64_MAX
#ifndef V8_COMPRESS_POINTERS
#define V8_COMPRESS_POINTERS 1
#endif
#ifndef V8_31BIT_SMIS_ON_64BIT_ARCH
#define V8_31BIT_SMIS_ON_64BIT_ARCH 1
#endif
#if !defined(ANDROID) && !defined(V8_ENABLE_SANDBOX)
#define V8_ENABLE_SANDBOX 1
#endif
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
  // Garbage collection queues finalizers rather than running them. When a finalizer is queued and no
  // drain is pending, postTask receives a task that runs the queue. postTask may be called during
  // garbage collection, so it must only queue the task. The task must run on the JavaScript thread
  // with the context entered, and never after Detach, which runs whatever is still queued. Without
  // postTask, queued finalizers run only at Detach.
  Napi::Env Attach(v8::Local<v8::Context>, std::function<void(std::function<void()>)> postTask = {});

  void Detach(Napi::Env);

  Napi::Value Eval(Napi::Env env, const char* source, const char* sourceUrl);

  v8::Local<v8::Context> GetContext(Napi::Env);
}
