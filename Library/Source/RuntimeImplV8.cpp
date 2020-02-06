#include "RuntimeImpl.h"

#include <napi/env.h>

#include <v8.h>
#include <libplatform/libplatform.h>

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

            static void Initialize(const char* executablePath)
            {
                if (s_module == nullptr)
                {
                    s_module = std::make_unique<Module>(executablePath);
                }
            }

        private:
            std::unique_ptr<v8::Platform> m_platform;

            static std::unique_ptr<Module> s_module;
        };

        std::unique_ptr<Module> Module::s_module;

        v8::Isolate* CreateIsolate(const char* executablePath)
        {
            Module::Initialize(executablePath);

            v8::Isolate::CreateParams create_params;
            create_params.array_buffer_allocator = v8::ArrayBuffer::Allocator::NewDefaultAllocator();
            return v8::Isolate::New(create_params);
        }

        void DestroyIsolate(v8::Isolate* isolate)
        {
            // todo : GetArrayBufferAllocator not available?
            //delete isolate->GetArrayBufferAllocator();
            isolate->Dispose();
        }
    }

    void RuntimeImpl::BaseThreadProcedure()
    {
        v8::Isolate* isolate = CreateIsolate(GetModulePath().u8string().data());
        {
            v8::Isolate::Scope isolate_scope{ isolate };
            v8::HandleScope isolate_handle_scope{ isolate };
            v8::Local<v8::Context> context = v8::Context::New(isolate);
            v8::Context::Scope context_scope{ context };

            Napi::Env env = Napi::Attach(context);
            RunJavaScript(env);
            Napi::Detach(env);
        }
        DestroyIsolate(isolate);
    }
}