#include "Worker.h"
#include "WorkerScripts.h"

#include <Babylon/AppRuntime.h>
#include <Babylon/JsRuntime.h>
#include <Babylon/JsRuntimeScheduler.h>
#include <Babylon/Polyfills/AbortController.h>
#include <Babylon/Polyfills/Blob.h>
#include <Babylon/Polyfills/Compression.h>
#include <Babylon/Polyfills/Console.h>
#include <Babylon/Polyfills/Fetch.h>
#include <Babylon/Polyfills/File.h>
#include <Babylon/Polyfills/IndexedDB.h>
#include <Babylon/Polyfills/Performance.h>
#include <Babylon/Polyfills/Scheduling.h>
#include <Babylon/Polyfills/Streams.h>
#include <Babylon/Polyfills/TextDecoder.h>
#include <Babylon/Polyfills/TextEncoder.h>
#include <Babylon/Polyfills/URL.h>
#include <Babylon/Polyfills/WebSocket.h>
#include <Babylon/Polyfills/Worker.h>
#include <Babylon/Polyfills/XMLHttpRequest.h>

#include <napi/js_native_api.h>

#include <atomic>
#include <cctype>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <mutex>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <utility>
#include <vector>

namespace Babylon::Polyfills::Internal
{
    namespace
    {
        using Options = Babylon::Polyfills::Worker::Options;

        bool HasScheme(const std::string& value)
        {
            const auto separator = value.find(':');
            if (separator == std::string::npos || separator == 0)
            {
                return false;
            }
            for (std::size_t i = 0; i < separator; ++i)
            {
                const auto c = static_cast<unsigned char>(value[i]);
                if (!(std::isalnum(c) || c == '+' || c == '-' || c == '.'))
                {
                    return false;
                }
            }
            return true;
        }

        std::string PercentDecode(const std::string& value)
        {
            std::string result;
            result.reserve(value.size());
            for (std::size_t i = 0; i < value.size(); ++i)
            {
                if (value[i] == '%' && i + 2 < value.size())
                {
                    const auto hex = value.substr(i + 1, 2);
                    char* end{};
                    const auto decoded = std::strtoul(hex.c_str(), &end, 16);
                    if (end != nullptr && *end == '\0')
                    {
                        result.push_back(static_cast<char>(decoded));
                        i += 2;
                        continue;
                    }
                }
                result.push_back(value[i]);
            }
            return result;
        }

        std::string ResolveUrl(const std::string& root,
                               const std::string& base,
                               const std::string& requested)
        {
            if (requested.rfind("data:", 0) == 0 || requested.rfind("file://", 0) == 0 ||
                requested.rfind("app:", 0) == 0)
            {
                return requested;
            }

            // A leading slash is origin-relative in Worker/importScripts, not
            // a host-filesystem escape from ScriptRoot.
            if (!requested.empty() && requested.front() == '/')
            {
                return (std::filesystem::path{root} / requested.substr(1)).lexically_normal().string();
            }

            if (HasScheme(requested))
            {
                return requested;
            }

            if (base.rfind("app:", 0) == 0)
            {
                // WHATWG URL serializes a hostless custom scheme such as
                // app:///worker.js as app:/worker.js. Accept both that form
                // and the app://host/path form used by existing hosts.
                const bool hasAuthority = base.rfind("app://", 0) == 0;
                const auto pathStart = hasAuthority ? base.find('/', 6) : base.find('/', 4);
                const std::string prefix = pathStart == std::string::npos ?
                    (hasAuthority ? base + "/" : "app:/") :
                    base.substr(0, pathStart + 1);
                const std::filesystem::path path = pathStart == std::string::npos ?
                    std::filesystem::path{} :
                    std::filesystem::path{base.substr(pathStart + 1)}.parent_path();
                return prefix + (path / requested).lexically_normal().generic_string();
            }

            if (base.rfind("file://", 0) == 0)
            {
                const auto path = std::filesystem::path{base.substr(7)}.parent_path() / requested;
                return "file://" + path.lexically_normal().generic_string();
            }

            if (base.rfind("data:", 0) == 0)
            {
                throw std::runtime_error{"A relative importScripts URL cannot be resolved from a data URL"};
            }

            std::filesystem::path parent = root;
            if (!base.empty() && !HasScheme(base))
            {
                parent = std::filesystem::path{base}.parent_path();
            }
            return (parent / requested).lexically_normal().string();
        }

        std::string ReadFile(const std::filesystem::path& path)
        {
            std::ifstream stream{path, std::ios::binary};
            if (!stream)
            {
                throw std::runtime_error{"Unable to load Worker script: " + path.string()};
            }
            return {std::istreambuf_iterator<char>{stream}, std::istreambuf_iterator<char>{}};
        }

        std::filesystem::path ResolveWithinRoot(const std::string& root,
                                                const std::filesystem::path& candidate)
        {
            const auto configuredRoot = root.empty() ? std::filesystem::current_path() :
                std::filesystem::path{root};
            const auto rootPath = std::filesystem::weakly_canonical(
                std::filesystem::absolute(configuredRoot));
            const auto candidatePath = std::filesystem::weakly_canonical(
                std::filesystem::absolute(candidate));

            auto rootPart = rootPath.begin();
            auto candidatePart = candidatePath.begin();
            for (; rootPart != rootPath.end(); ++rootPart, ++candidatePart)
            {
                if (candidatePart == candidatePath.end() || *candidatePart != *rootPart)
                {
                    throw std::runtime_error{"Worker script path escapes ScriptRoot: " + candidate.string()};
                }
            }

            return candidatePath;
        }

        std::string LoadSource(const Options& options, const std::string& url)
        {
            if (options.ScriptResolver)
            {
                return options.ScriptResolver(url);
            }

            if (url.rfind("data:", 0) == 0)
            {
                const auto comma = url.find(',');
                if (comma == std::string::npos || url.substr(0, comma).find(";base64") != std::string::npos)
                {
                    throw std::runtime_error{"Only percent-encoded JavaScript data URLs are supported"};
                }
                return PercentDecode(url.substr(comma + 1));
            }

            if (url.rfind("app:", 0) == 0)
            {
                const bool hasAuthority = url.rfind("app://", 0) == 0;
                const auto pathStart = hasAuthority ? url.find('/', 6) : url.find('/', 4);
                const auto relative = pathStart == std::string::npos ?
                    std::string{} :
                    url.substr(pathStart + 1);
                return ReadFile(ResolveWithinRoot(
                    options.ScriptRoot, std::filesystem::path{options.ScriptRoot} / relative));
            }

            if (url.rfind("file://", 0) == 0)
            {
                return ReadFile(PercentDecode(url.substr(7)));
            }

            return ReadFile(ResolveWithinRoot(options.ScriptRoot, url));
        }

#if NAPI_VERSION >= 7
        void ThrowStatus(Napi::Env env, napi_status status, const char* operation)
        {
            if (status == napi_pending_exception && env.IsExceptionPending())
            {
                throw env.GetAndClearPendingException();
            }
            throw Napi::Error::New(env, std::string{operation} + " failed with Node-API status " +
                std::to_string(static_cast<int>(status)));
        }

        [[noreturn]] void ThrowDataCloneError(Napi::Env env, const char* message)
        {
            const auto exception = env.Global()
                                       .Get("DOMException")
                                       .As<Napi::Function>()
                                       .New({Napi::String::New(env, message),
                                             Napi::String::New(env, "DataCloneError")});
            throw Napi::Error{env, exception};
        }
#endif
    }

    struct Worker::State
    {
        Options Config{};
        // A scheduler copy, not the JsRuntime itself: the worker thread dispatches to the parent
        // realm after terminate() and after close(), and either can outlive the parent runtime
        // (a worker terminated as it is created, a host tearing down while workers close). The
        // scheduler keeps the parent's dispatch state alive and discards work once it has shut down.
        std::optional<JsRuntimeScheduler> ParentScheduler{};
        Napi::ObjectReference ParentObject{};
        std::unique_ptr<AppRuntime> Runtime{};
        std::atomic_bool Terminated{false};
        std::atomic_bool Closed{false};
        std::string ActiveUrl{};
        std::string Name{};
        bool Module{};
        std::mutex RuntimeMutex{};
    };

    void Worker::Initialize(Napi::Env env, Options options)
    {
        if (!env.Global().Get("Worker").IsUndefined())
        {
            return;
        }

        Napi::Eval(env, WorkerScripts::Common, "jsruntimehost-worker-common.js");

        auto* classOptions = new Options{std::move(options)};
        auto constructor = DefineClass(
            env,
            "Worker",
            {
                InstanceMethod("__jsrhNativePostMessage", &Worker::PostMessage),
                InstanceMethod("terminate", &Worker::Terminate),
            },
            classOptions);

        // Keep constructor data alive for exactly as long as the constructor.
        constructor.Set(
            "__jsrhOptions",
            Napi::External<Options>::New(env, classOptions, [](Napi::Env, Options* value) { delete value; }));

        env.Global().Get("__jsrhInstallWorker").As<Napi::Function>().Call(env.Global(), {constructor});
        env.Global().Set("Worker", constructor);
    }

    Worker::Worker(const Napi::CallbackInfo& info)
        : Napi::ObjectWrap<Worker>{info}
    {
        if (info.Length() == 0)
        {
            throw Napi::TypeError::New(info.Env(), "Worker requires a script URL");
        }

        auto state = std::make_shared<State>();
        state->Config = *static_cast<const Options*>(info.Data());
        state->ParentScheduler.emplace(JsRuntime::GetFromJavaScript(info.Env()));
        // An active Worker is a browser "active object": it remains alive even
        // if script drops its last reference, until terminate()/close(). A
        // strong reference also works on engines such as QuickJS whose N-API
        // weak-reference adapter cannot materialize a live weak value.
        state->ParentObject = Napi::Persistent(info.This().As<Napi::Object>());

        const auto requestedUrl = info[0].ToString().Utf8Value();
        const auto resolvedUrl = ResolveUrl(state->Config.ScriptRoot, {}, requestedUrl);
        bool module = false;
        if (info.Length() > 1 && info[1].IsObject())
        {
            const auto options = info[1].As<Napi::Object>();
            // WorkerOptions is a WebIDL dictionary: a member set to undefined counts as absent, so
            // `{ type: undefined }` (what a spread of optional options produces) means "classic".
            const auto typeValue = options.Get("type");
            if (!typeValue.IsUndefined())
            {
                const auto type = typeValue.ToString().Utf8Value();
                if (type != "classic" && type != "module")
                {
                    throw Napi::TypeError::New(info.Env(), "Worker type must be 'classic' or 'module'");
                }
                module = type == "module";
            }
            if (options.Has("name"))
            {
                state->Name = options.Get("name").ToString().Utf8Value();
            }
        }
        state->Module = module;

        AppRuntime::Options runtimeOptions{};
        const std::weak_ptr<State> weakState{state};
        runtimeOptions.UnhandledExceptionHandler = [weakState](const Napi::Error& error) {
            const auto locked = weakState.lock();
            if (!locked || locked->Terminated.load())
            {
                return;
            }
            // Some engines expose a stack without its usual "Error: message"
            // header (QuickJS is one). Always retain the exception message:
            // parent Worker.onerror is the only startup diagnostic for a
            // bundle that fails before installing its message protocol.
            auto detail = Napi::GetErrorString(error);
            const auto& message = error.Message();
            if (!message.empty() && detail.find(message) == std::string::npos)
            {
                detail = message + (detail.empty() ? "" : "\n" + detail);
            }
            DispatchErrorToParent(weakState, std::move(detail));
        };
        runtimeOptions.ThreadExitHandler = [weakState] {
            const auto locked = weakState.lock();
            if (!locked)
            {
                return;
            }

            // The Worker wrapper is a parent-realm JS object. Never release
            // its strong reference from the worker thread: WebKit fixed this
            // exact cross-thread destruction pattern after a terminate UAF.
            // Queue this after all same-task message/error deliveries so
            // WorkerGlobalScope.close() preserves their FIFO ordering.
            // https://github.com/WebKit/WebKit/commit/4aaa3c1477e296e67b03e1461479b8caf57c37dd
            (*locked->ParentScheduler)([weakState](Napi::Env) {
                const auto parentState = weakState.lock();
                if (parentState && !parentState->ParentObject.IsEmpty())
                {
                    parentState->ParentObject.Reset();
                }
            });
        };

        state->Runtime = std::make_unique<AppRuntime>(std::move(runtimeOptions));
        state->Runtime->Dispatch([state, resolvedUrl, module](Napi::Env env) {
            InitializeWorker(state, env, resolvedUrl, module);
        });

        m_state = std::move(state);
    }

    Worker::~Worker()
    {
        Stop();
    }

    void Worker::Stop()
    {
        auto state = std::move(m_state);
        if (!state)
        {
            return;
        }

        state->Terminated.store(true);
        state->Closed.store(true);
        {
            std::scoped_lock lock{state->RuntimeMutex};
            if (state->Runtime)
            {
                state->Runtime->Terminate();
            }
        }
        state->ParentObject.Reset();
        state->Runtime.reset();
    }

    void Worker::Terminate(const Napi::CallbackInfo&)
    {
        if (!m_state || m_state->Terminated.exchange(true))
        {
            return;
        }

        m_state->Closed.store(true);
        std::scoped_lock lock{m_state->RuntimeMutex};
        if (m_state->Runtime)
        {
            m_state->Runtime->Terminate();
        }
        m_state->ParentObject.Reset();
    }

    Worker::Message Worker::Serialize(Napi::Env env,
                                      const Napi::Value& value,
                                      const Napi::Value& transfer)
    {
        const auto encoded = env.Global()
                                 .Get("__jsrhSerialize")
                                 .As<Napi::Function>()
                                 .Call(env.Global(), {value, transfer})
                                 .As<Napi::Object>();

        Message result{};
        result.Json = encoded.Get("json").As<Napi::String>().Utf8Value();
        const auto buffers = encoded.Get("buffers").As<Napi::Array>();
        const auto transferBuffers = encoded.Get("transferBuffers").As<Napi::Array>();
#if NAPI_VERSION >= 7
        // Validate all transferables before copying or detaching any of them.
        // Some engines do not expose detached state as a JavaScript property,
        // so the Node-API check is the portable source of truth.
        for (std::uint32_t i = 0; i < transferBuffers.Length(); ++i)
        {
            bool detached{};
            const napi_status status = napi_is_detached_arraybuffer(env, transferBuffers.Get(i), &detached);
            if (status != napi_ok)
            {
                ThrowStatus(env, status, "napi_is_detached_arraybuffer");
            }
            if (detached)
            {
                ThrowDataCloneError(env, "An ArrayBuffer in the transfer list is already detached");
            }
        }
#endif

        result.Buffers.reserve(buffers.Length());
        for (std::uint32_t i = 0; i < buffers.Length(); ++i)
        {
            const auto buffer = buffers.Get(i).As<Napi::ArrayBuffer>();
            std::vector<std::uint8_t> bytes(buffer.ByteLength());
            if (!bytes.empty())
            {
                std::memcpy(bytes.data(), buffer.Data(), bytes.size());
            }
            result.Buffers.emplace_back(std::move(bytes));
        }

        // Duplicate and invalid entries were rejected by the JS serializer;
        // after validation and copying, detach each original transfer target.
        // The native byte-copy loop above only inspects JS-side snapshots, so
        // JavaScriptCore never exposes the original backing pointer before its
        // standards-track transfer() implementation detaches it.
        for (std::uint32_t i = 0; i < transferBuffers.Length(); ++i)
        {
#if NAPI_VERSION >= 7
            const napi_status status = napi_detach_arraybuffer(env, transferBuffers.Get(i));
            if (status != napi_ok)
            {
                ThrowStatus(env, status, "napi_detach_arraybuffer");
            }
#else
            (void)i;
            throw Napi::Error::New(env, "Transferable ArrayBuffers require N-API v7");
#endif
        }

        return result;
    }

    Napi::Value Worker::Deserialize(Napi::Env env, const Message& message)
    {
        const auto buffers = Napi::Array::New(env, message.Buffers.size());
        for (std::size_t i = 0; i < message.Buffers.size(); ++i)
        {
            const auto& bytes = message.Buffers[i];
            auto buffer = Napi::ArrayBuffer::New(env, bytes.size());
            if (!bytes.empty())
            {
                std::memcpy(buffer.Data(), bytes.data(), bytes.size());
            }
            buffers.Set(static_cast<std::uint32_t>(i), buffer);
        }

        return env.Global()
            .Get("__jsrhDeserialize")
            .As<Napi::Function>()
            .Call(env.Global(), {Napi::String::New(env, message.Json), buffers});
    }

    Napi::Value Worker::PostMessage(const Napi::CallbackInfo& info)
    {
        if (!m_state || m_state->Terminated.load() || m_state->Closed.load())
        {
            return info.Env().Undefined();
        }

        const auto envelope = info[0].As<Napi::Object>();
        const auto value = envelope.Get("__jsrhMessage");
        const auto transfer = envelope.Get("__jsrhTransfer");
        auto message = Serialize(info.Env(), value, transfer);
        const std::weak_ptr<State> weakState{m_state};

        std::scoped_lock lock{m_state->RuntimeMutex};
        if (m_state->Runtime && !m_state->Terminated.load())
        {
            m_state->Runtime->Dispatch([weakState, message = std::move(message)](Napi::Env env) mutable {
                const auto state = weakState.lock();
                if (!state || state->Terminated.load() || state->Closed.load())
                {
                    return;
                }
                const auto value = Deserialize(env, message);
                env.Global().Get("__jsrhDispatchMessage").As<Napi::Function>().Call(
                    env.Global(), {env.Global(), value});
            });
        }

        return info.Env().Undefined();
    }

    void Worker::InitializeWorker(const std::shared_ptr<State>& state,
                                  Napi::Env env,
                                  const std::string& url,
                                  bool module)
    {
        (void)module;
        Babylon::Polyfills::Console::Initialize(env, [weakState = std::weak_ptr<State>{state}](const char* message,
                                                        Babylon::Polyfills::Console::LogLevel) {
            const auto locked = weakState.lock();
            if (locked && locked->Config.ConsoleCallback)
            {
                locked->Config.ConsoleCallback(message);
            }
        });
        Babylon::Polyfills::AbortController::Initialize(env);
        Babylon::Polyfills::Performance::Initialize(env);
        Babylon::Polyfills::Scheduling::Initialize(env);
        Babylon::Polyfills::URL::Initialize(env);
        Babylon::Polyfills::WebSocket::Initialize(env);
        Babylon::Polyfills::XMLHttpRequest::Initialize(env);
        Babylon::Polyfills::Streams::Initialize(env);
        Babylon::Polyfills::Blob::Initialize(env);
        Babylon::Polyfills::File::Initialize(env);
        Babylon::Polyfills::TextDecoder::Initialize(env);
        Babylon::Polyfills::TextEncoder::Initialize(env);
        Babylon::Polyfills::Fetch::Initialize(env);
        Babylon::Polyfills::Compression::Initialize(env);
        Babylon::Polyfills::IndexedDB::Initialize(env);

        const std::weak_ptr<State> weakState{state};
        env.Global().Set("__jsrhNativePostMessage", Napi::Function::New(
            env,
            [weakState](const Napi::CallbackInfo& info) {
                const auto locked = weakState.lock();
                if (!locked || locked->Terminated.load())
                {
                    return info.Env().Undefined();
                }
                const auto envelope = info[0].As<Napi::Object>();
                const auto value = envelope.Get("__jsrhMessage");
                const auto transfer = envelope.Get("__jsrhTransfer");
                DispatchMessageToParent(weakState, Serialize(info.Env(), value, transfer));
                return info.Env().Undefined();
            },
            "postMessage"));

        env.Global().Set("__jsrhNativeClose", Napi::Function::New(
            env,
            [weakState](const Napi::CallbackInfo& info) {
                const auto locked = weakState.lock();
                if (locked && !locked->Terminated.load() && !locked->Closed.exchange(true))
                {
                    std::scoped_lock lock{locked->RuntimeMutex};
                    if (locked->Runtime)
                    {
                        // WorkerGlobalScope.close() discards future tasks but
                        // must let this task finish (including postMessage and
                        // an uncaught error). Parent terminate() remains the
                        // immediate-interrupt path for tight loops.
                        locked->Runtime->Close();
                    }
                }
                return info.Env().Undefined();
            },
            "close"));

        env.Global().Set("__jsrhNativeImportScripts", Napi::Function::New(
            env,
            [weakState](const Napi::CallbackInfo& info) {
                const auto locked = weakState.lock();
                if (!locked || locked->Terminated.load() || locked->Closed.load())
                {
                    return info.Env().Undefined();
                }
                if (locked->Module)
                {
                    throw Napi::TypeError::New(info.Env(), "importScripts is unavailable in a module Worker");
                }

                for (std::size_t i = 0; i < info.Length(); ++i)
                {
                    const auto requested = info[i].ToString().Utf8Value();
                    const auto resolved = ResolveUrl(locked->Config.ScriptRoot, locked->ActiveUrl, requested);
                    try
                    {
                        const auto source = LoadSource(locked->Config, resolved);
                        const auto previous = std::exchange(locked->ActiveUrl, resolved);
                        try
                        {
                            Napi::Eval(info.Env(), source.c_str(), resolved.c_str());
                        }
                        catch (...)
                        {
                            locked->ActiveUrl = previous;
                            throw;
                        }
                        locked->ActiveUrl = previous;
                    }
                    catch (const Napi::Error&)
                    {
                        throw;
                    }
                    catch (const std::exception& error)
                    {
                        throw Napi::Error::New(info.Env(), error.what());
                    }
                }
                return info.Env().Undefined();
            },
            "importScripts"));

        env.Global().Set("__jsrhWorkerLocation", Napi::String::New(env, url));
        env.Global().Set("__jsrhWorkerName", Napi::String::New(env, state->Name));
        Napi::Eval(env, WorkerScripts::Common, "jsruntimehost-worker-common.js");
        Napi::Eval(env, WorkerScripts::WorkerGlobal, "jsruntimehost-worker-global.js");

        try
        {
            state->ActiveUrl = url;
            const auto source = LoadSource(state->Config, url);
            // JavaScriptCore's C API exposes script evaluation but no module
            // loader. A script-compatible module bundle is already
            // self-contained and can use the same isolated realm; remaining
            // import/export declarations surface as a worker error.
            Napi::Eval(env, source.c_str(), url.c_str());
        }
        catch (const Napi::Error&)
        {
            throw;
        }
        catch (const std::exception& error)
        {
            throw Napi::Error::New(env, error.what());
        }
    }

    void Worker::DispatchMessageToParent(const std::weak_ptr<State>& weakState, Message message)
    {
        const auto state = weakState.lock();
        if (!state || state->Terminated.load())
        {
            return;
        }

        (*state->ParentScheduler)([weakState, message = std::move(message)](Napi::Env env) mutable {
            const auto locked = weakState.lock();
            if (!locked || locked->Terminated.load())
            {
                return;
            }
            const auto target = locked->ParentObject.Value();
            if (target.IsEmpty())
            {
                return;
            }
            const auto value = Deserialize(env, message);
            env.Global().Get("__jsrhDispatchMessage").As<Napi::Function>().Call(
                env.Global(), {target, value});
        });
    }

    void Worker::DispatchErrorToParent(const std::weak_ptr<State>& weakState, std::string message)
    {
        const auto state = weakState.lock();
        if (!state || state->Terminated.load())
        {
            return;
        }

        (*state->ParentScheduler)([weakState, message = std::move(message)](Napi::Env env) {
            const auto locked = weakState.lock();
            if (!locked || locked->Terminated.load())
            {
                return;
            }
            const auto target = locked->ParentObject.Value();
            if (!target.IsEmpty())
            {
                env.Global().Get("__jsrhDispatchError").As<Napi::Function>().Call(
                    env.Global(), {target, Napi::String::New(env, message)});
            }
        });
    }
}

namespace Babylon::Polyfills::Worker
{
    void BABYLON_API Initialize(Napi::Env env, Options options)
    {
        Internal::Worker::Initialize(env, std::move(options));
    }
}
