#include "AppRuntime.h"
#include "V8Platform.h"
#include <napi/env.h>
#include "js_native_api_v8.h"

#include <libplatform/libplatform.h>

#ifdef ENABLE_V8_INSPECTOR
#include <V8InspectorAgent.h>
#endif

#include <memory>
#include <optional>
#include <stdexcept>

#ifdef JSRUNTIMEHOST_EMBED_V8_SNAPSHOT
#include "AndroidV8Snapshot.h"
#endif

namespace Babylon
{
    namespace
    {
        struct RuntimeNapiEnv final : napi_env__
        {
            RuntimeNapiEnv(v8::Local<v8::Context> context, AppRuntime& runtime)
                : napi_env__{context, NAPI_VERSION}
                , m_runtime{runtime}
            {
            }

            void EnqueueFinalizer(v8impl::RefTracker* finalizer) override
            {
                napi_env__::EnqueueFinalizer(finalizer);
                if (!m_finalizationScheduled && !m_destructing)
                {
                    m_finalizationScheduled = true;
                    m_runtime.Dispatch([this](Napi::Env) {
                        m_finalizationScheduled = false;
                        DrainFinalizerQueue();
                    });
                }
            }

            void DeleteMe() override
            {
                m_destructing = true;
                DrainFinalizerQueue();
                napi_env__::DeleteMe();
            }

        private:
            void DrainFinalizerQueue()
            {
                // Finalizers can enqueue or delete other finalizers.
                while (!pending_finalizers.empty())
                {
                    auto* finalizer = *pending_finalizers.begin();
                    pending_finalizers.erase(finalizer);
                    finalizer->Finalize();
                }
            }

            AppRuntime& m_runtime;
            bool m_finalizationScheduled = false;
            bool m_destructing = false;
        };

        class Module final
        {
        public:
            Module(const char* executablePath)
            {
                v8::V8::InitializeICUDefaultLocation(executablePath);
#ifdef JSRUNTIMEHOST_EMBED_V8_SNAPSHOT
                // APK assets have no filesystem path. V8 borrows this data for the process lifetime.
                static v8::StartupData snapshot{reinterpret_cast<const char*>(AndroidV8Snapshot), sizeof(AndroidV8Snapshot)};
                v8::V8::SetSnapshotDataBlob(&snapshot);
#else
                v8::V8::InitializeExternalStartupData(executablePath);
#endif
                m_platform = std::make_unique<Internal::V8Platform>(v8::platform::NewDefaultPlatform());
                v8::V8::InitializePlatform(m_platform.get());
                v8::V8::Initialize();
            }

            ~Module()
            {
                v8::V8::Dispose();
                v8::V8::DisposePlatform();
            }

            static void Initialize(const char* executablePath)
            {
                if (s_module == nullptr)
                {
                    s_module = std::make_unique<Module>(executablePath);
                }
            }

            static Module& Instance()
            {
                if (!s_module)
                {
                    throw std::runtime_error{"Module not available"};
                }

                return *s_module;
            }

            Internal::V8Platform& Platform()
            {
                return *m_platform;
            }

        private:
            std::unique_ptr<Internal::V8Platform> m_platform;

            static std::unique_ptr<Module> s_module;
        };

        std::unique_ptr<Module> Module::s_module;
    }

    void AppRuntime::RunEnvironmentTier(const char* executablePath)
    {
        Module::Initialize(executablePath);
        auto& platform = Module::Instance().Platform();

        v8::Isolate::CreateParams create_params;
        create_params.array_buffer_allocator = v8::ArrayBuffer::Allocator::NewDefaultAllocator();
        v8::Isolate* isolate = v8::Isolate::Allocate();
        // Initialization can post foreground work, so register before it starts.
        platform.RegisterHost(isolate, *this, GetDelayedTaskScheduler());
        v8::Isolate::Initialize(isolate, create_params);

        {
            v8::Isolate::Scope isolate_scope{isolate};
            v8::HandleScope isolate_handle_scope{isolate};
            v8::Local<v8::Context> context = v8::Context::New(isolate);
            v8::Context::Scope context_scope{context};

            Napi::Env env{new RuntimeNapiEnv{context, *this}};

#ifdef ENABLE_V8_INSPECTOR
            std::optional<V8InspectorAgent> agent;
            if (m_options.EnableDebugger)
            {
                agent.emplace(platform, isolate, context, "JsRuntimeHost");
                agent->Start(5643, "JsRuntimeHost");

                if (m_options.WaitForDebugger)
                {
                    agent->WaitForDebugger();
                }
            }
#endif

            Run(env);

#ifdef ENABLE_V8_INSPECTOR
            if (agent.has_value())
            {
                agent->Stop();
            }
#endif

            Napi::Detach(env);
        }

        // todo : GetArrayBufferAllocator not available?
        // delete isolate->GetArrayBufferAllocator();
        isolate->Dispose();
    }

    void AppRuntime::ShutdownEnvironment(Napi::Env)
    {
        Module::Instance().Platform().UnregisterHost(v8::Isolate::GetCurrent());
    }

    void AppRuntime::DrainMicrotasks(Napi::Env)
    {
        // V8 auto-drains microtasks. Foreground tasks run on AppRuntime's dispatcher.
    }

    void AppRuntime::RunDispatchLoop()
    {
        RunDispatcher();
    }
}
