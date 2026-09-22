// Bridge between the Babylon Napi:: helpers and the Hermes static_h NAPI
// implementation.
//
// Hermes's `hermesNapi` static library implements the full set of standard
// `napi_*` C functions on top of a `hermes::vm::Runtime`.  In this TU we:
//   1.  Create a Runtime + napi_env via `hermes_napi_create_env`.
//   2.  Keep the Runtime alive for the lifetime of the env in a process-wide
//       table (the env is opaque to callers; we need somewhere to stash the
//       owning shared_ptr).
//   3.  Provide `Napi::Eval` that calls Hermes's `hermes_run_script` (which
//       takes a `source_url` for stack traces, matching what callers expect
//       from the other engines' `Napi::Eval`).
//   4.  Expose `Napi::DrainJobs` so AppRuntime can pump microtasks after each
//       dispatched callback.
//
// Header layering note:
// Both our shared NAPI headers and Hermes's vendored ones use the same
// include guards (`SRC_JS_NATIVE_API_H_`, `SRC_JS_NATIVE_API_TYPES_H_`).
// Whichever set is included first wins.  We deliberately include our shared
// `<napi/env.h>` chain FIRST so:
//   * the Napi:: C++ wrappers (`Napi::Env`, `Napi::Value`, `Napi::Error`)
//     line up with the rest of the project,
//   * the Babylon-extended 4-arg `napi_run_script` declaration matches the
//     inline `Env::RunScript` body in napi-inl.h (which we never actually
//     call in this engine — Napi::Eval below routes through
//     `hermes_run_script` directly — so the linker is never asked to find
//     the 4-arg symbol).
//
// We keep `NAPI_VERSION` at the shared default (5) so the inline wrappers
// in napi-inl.h that target newer NAPI revisions (e.g.
// `Env::GetModuleFileName` which calls `node_api_get_module_file_name`)
// aren't pulled in — they would reference symbols absent from our shared
// js_native_api.h.
//
// Hermes's `node_api.h` then needs a couple of NAPI v10 types
// (`node_api_basic_env`, `node_api_basic_finalize`) that its own
// js_native_api.h would have defined, but those headers are now guarded
// out.  We supply the typedefs locally: in NAPI v10 they're just type-
// attributed aliases for the regular `napi_env` / `napi_finalize`, so the
// ABI remains compatible with the symbols hermesNapi actually exports.

#include <napi/env.h>

// Forward-declare the NAPI v10 "basic" type aliases that Hermes's node_api.h
// expects to find.  These are ABI-equivalent to the non-basic versions; the
// "basic" marker is purely a documentation/attribute aid in upstream Node.
typedef napi_env node_api_basic_env;
typedef napi_finalize node_api_basic_finalize;

#include "hermes_napi.h"
#include "hermes/Public/RuntimeConfig.h"
#include "hermes/VM/Runtime.h"

#include <cstring>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <string>
#include <unordered_map>

// Maximum GC heap size (in MiB) for the Hermes Runtime created by
// `Napi::Attach`.  Overridable at CMake configure time via
// `-DNAPI_HERMES_MAX_HEAP_SIZE_MB=<value>` (see Core/Node-API/CMakeLists.txt);
// falls back to 512 MiB when the definition isn't supplied.
#ifndef NAPI_HERMES_MAX_HEAP_SIZE_MB
#define NAPI_HERMES_MAX_HEAP_SIZE_MB 512
#endif

namespace
{
    struct HermesEnvState
    {
        std::shared_ptr<hermes::vm::Runtime> runtime;
    };

    std::mutex& StateMutex()
    {
        static std::mutex mutex;
        return mutex;
    }

    std::unordered_map<napi_env, HermesEnvState>& StateMap()
    {
        static std::unordered_map<napi_env, HermesEnvState> map;
        return map;
    }

    hermes::vm::Runtime* LookupRuntime(napi_env env)
    {
        std::scoped_lock lock{StateMutex()};
        auto it = StateMap().find(env);
        if (it == StateMap().end())
        {
            return nullptr;
        }
        return it->second.runtime.get();
    }
}

namespace Napi
{
    Napi::Env Attach()
    {
        // Default Hermes config is fine for embedding: MicrotaskQueue is on,
        // ES6 Proxy + generators are on, Intl is on, EnableEval is on.
        // We bump the max GC heap to something reasonable for running our
        // Mocha test suite (the unit-test default of 512 KiB is too small).
        // The ceiling is configurable via NAPI_HERMES_MAX_HEAP_SIZE_MB
        // (defaults to 512 MiB).
        auto config = hermes::vm::RuntimeConfig::Builder()
                          .withGCConfig(hermes::vm::GCConfig::Builder()
                                            .withInitHeapSize(1u << 20)        //   1 MiB
                                            .withMaxHeapSize(static_cast<unsigned>(NAPI_HERMES_MAX_HEAP_SIZE_MB) << 20)
                                            .build())
                          .build();

        auto runtime = hermes::vm::Runtime::create(config);

        // `hermes_napi_create_env` ties the env's lifetime to the runtime —
        // destroying the runtime tears down the env via the runtime's
        // post-shutdown deleter.  We do NOT delete the env ourselves.
        napi_env env = hermes_napi_create_env(runtime.get());
        if (env == nullptr)
        {
            throw std::runtime_error{"hermes_napi_create_env returned null"};
        }

        {
            std::scoped_lock lock{StateMutex()};
            StateMap().emplace(env, HermesEnvState{std::move(runtime)});
        }

        return {env};
    }

    void Detach(Napi::Env env)
    {
        napi_env env_ptr{env};

        HermesEnvState state;
        {
            std::scoped_lock lock{StateMutex()};
            auto it = StateMap().find(env_ptr);
            if (it == StateMap().end())
            {
                return;
            }
            state = std::move(it->second);
            StateMap().erase(it);
        }
    }

    void DrainJobs(Napi::Env env)
    {
        hermes::vm::Runtime* runtime = LookupRuntime(env);
        if (runtime == nullptr)
        {
            return;
        }
        // We intentionally ignore the ExecutionStatus return: any unhandled
        // exception raised by a microtask is surfaced to JS via the standard
        // unhandled-rejection mechanism (Hermes prints it via HermesInternal),
        // and we don't have a meaningful way to bubble it up here.  Real
        // exceptions thrown FROM user callbacks are already handled in
        // AppRuntime::Dispatch's try/catch.
        (void)runtime->drainJobs();
    }

    Napi::Value Eval(Napi::Env env, const char* source, const char* sourceUrl)
    {
        napi_env env_ptr{env};
        const size_t length = std::strlen(source);
        // hermes_run_script supports a zero-copy fast path when the last byte
        // of the buffer is `\0` — pass length+1 and include the null
        // terminator we already have in `source`.
        const size_t size = length + 1;

        hermes_run_script_flags flags{};
        flags.struct_size = sizeof(flags);

        napi_value result = nullptr;

        // Hermes's `hermes_run_script` takes ownership of the source buffer
        // via the finalize callback.  Our `source` is owned by the caller,
        // so we make a copy and let Hermes free it when it's done.
        auto* copy = new uint8_t[size];
        std::memcpy(copy, source, size);
        auto finalize = [](const uint8_t* data, size_t /*size*/, void* /*hint*/) {
            delete[] data;
        };

        const napi_status status = hermes_run_script(
            env_ptr,
            copy,
            size,
            finalize,
            /*finalize_hint=*/nullptr,
            sourceUrl,
            &flags,
            &result);

        if (status != napi_ok)
        {
            // Surface as a Napi::Error so callers see the same shape they get
            // from the other engines' Eval paths.
            const napi_extended_error_info* info = nullptr;
            napi_get_last_error_info(env_ptr, &info);
            const char* message =
                (info && info->error_message) ? info->error_message : "hermes_run_script failed";

            // If a JS exception is pending, prefer that for the error info.
            bool pending = false;
            napi_is_exception_pending(env_ptr, &pending);
            if (pending)
            {
                napi_value exception = nullptr;
                napi_get_and_clear_last_exception(env_ptr, &exception);
                if (exception != nullptr)
                {
                    throw Napi::Error{env, exception};
                }
            }

            throw std::runtime_error{std::string{"Hermes Eval failed: "} + message};
        }

        return Napi::Value{env, result};
    }
}
