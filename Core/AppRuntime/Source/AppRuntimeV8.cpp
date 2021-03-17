#include "AppRuntime.h"
#include "AppRuntimeV8.h"

#ifndef __clang__
#pragma warning(disable : 4100 4267 4127)
#endif
#include <v8.h>
#include <libplatform/libplatform.h>

#if __has_include(<V8InspectorAgent.h>)
#define V8_INSPECTOR_INCLUDED
#include <V8InspectorAgent.h>
#endif

namespace Babylon
{
    namespace
    {
        class Module final
        {
        public:
            Module(const char* executablePath)
            {
                v8::V8::InitializeICUDefaultLocation(executablePath);
                v8::V8::InitializeExternalStartupData(executablePath);
                m_platform = v8::platform::NewDefaultPlatform();
                v8::V8::InitializePlatform(m_platform.get());
                v8::V8::Initialize();
            }

            ~Module()
            {
                v8::V8::Dispose();
                v8::V8::ShutdownPlatform();
            }

            static Module& Instance()
            {
                return *s_module;
            }

            static void Initialize(const char* executablePath)
            {
                if (s_module == nullptr)
                {
                    s_module = std::make_unique<Module>(executablePath);
                }
            }

            v8::Platform* Platform() const { return m_platform.get(); }

        private:
            std::unique_ptr<v8::Platform> m_platform;

            static std::unique_ptr<Module> s_module;
        };

        std::unique_ptr<Module> Module::s_module;
    }

    static unsigned short s_inspectorPort = 0;

    void AppRuntime::RunEnvironmentTier(const char* executablePath)
    {
        // Create the isolate.
        Module::Initialize(executablePath);
        v8::Isolate::CreateParams create_params;
        create_params.array_buffer_allocator = v8::ArrayBuffer::Allocator::NewDefaultAllocator();
        v8::Isolate* isolate = v8::Isolate::New(create_params);

        // Use the isolate within a scope.
        {
            v8::Isolate::Scope isolate_scope{isolate};
            v8::HandleScope isolate_handle_scope{isolate};
            v8::Local<v8::Context> context = v8::Context::New(isolate);
            v8::Context::Scope context_scope{context};

#ifdef V8_INSPECTOR_INCLUDED
            if (s_inspectorPort != 0)
            {
                Babylon::V8InspectorAgent inspector{*Module::Instance().Platform(), isolate, context, "BabylonNative", s_inspectorPort};
                inspector.start();
            }
#endif

            Napi::Env env = Napi::Attach(context);
            Run(env);
            Napi::Detach(env);
        }

        // Destroy the isolate.
        // todo : GetArrayBufferAllocator not available?
        // delete isolate->GetArrayBufferAllocator();
        isolate->Dispose();
    }

    bool AppRuntimeV8::EnableInspector(unsigned short port)
    {
#ifdef V8_INSPECTOR_INCLUDED
        s_inspectorPort = port;
        return true;
#else
        return false;
#endif
    }

    void AppRuntimeV8::DisableInspector()
    {
        s_inspectorPort = 0;
    }
}