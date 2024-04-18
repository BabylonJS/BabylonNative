#pragma once

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

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4100 4267 4127)
#endif

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
#endif

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
#endif

#include <v8.h>
#include <v8-inspector.h>
#include <libplatform/libplatform.h>

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

#ifdef __clang__
#pragma clang diagnostic pop
#endif