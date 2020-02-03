#include "RuntimeImpl.h"

#include "NativeEngine.h"
#include "NativeWindow.h"
#include "XMLHttpRequest.h"

/*
#define USE_EDGEMODE_JSRT
#include <jsrt.h>
#undef USE_EDGEMODE_JSRT
*/

#include <v8.h>
#include <libplatform/libplatform.h>

#include <curl/curl.h>
#include <napi/env.h>
#include <sstream>

namespace Babylon
{
    namespace
    {
        using DispatchFunction = std::function<void(std::function<void()>)>;

        /*
        void ThrowIfFailed(JsErrorCode errorCode)
        {
            if (errorCode != JsErrorCode::JsNoError)
            {
                throw std::exception();
            }
        }

        void CreateJsRuntimeForThread(DispatchFunction& dispatch)
        {
            // Create the runtime. We're only going to use one runtime for this host.
            JsRuntimeHandle jsRuntime;
            ThrowIfFailed(JsCreateRuntime(JsRuntimeAttributeNone, nullptr, &jsRuntime));

            // Create a single execution context.
            JsContextRef context;
            ThrowIfFailed(JsCreateContext(jsRuntime, &context));

            // Now set the execution context as being the current one on this thread.
            ThrowIfFailed(JsSetCurrentContext(context));

            // Set up ES6 Promise.
            ThrowIfFailed(JsSetPromiseContinuationCallback([](JsValueRef task, void* callbackState)
            {
                ThrowIfFailed(JsAddRef(task, nullptr));
                auto* dispatch = reinterpret_cast<DispatchFunction*>(callbackState);
                dispatch->operator()([task]()
                {
                    JsValueRef undefined;
                    ThrowIfFailed(JsGetUndefinedValue(&undefined));
                    ThrowIfFailed(JsCallFunction(task, &undefined, 1, nullptr));
                    ThrowIfFailed(JsRelease(task, nullptr));
                });
            }, &dispatch));

            // UWP namespace projection; all UWP under Windows namespace should work.
            ThrowIfFailed(JsProjectWinRTNamespace(L"Windows"));

#ifdef _DEBUG
            // Put Chakra in debug mode.
            ThrowIfFailed(JsStartDebugging());
#endif
        }

        void DisposeJsRuntimeForThread()
        {
            // TODO: There's an order-of-teardown dependency right now that gets triggered
            // if we actually dispose the runtime properly. Commenting this out until we
            // find a way to resolve that problem.
            //
            // JsContextRef context;
            // ThrowIfFailed(JsGetCurrentContext(&context));
            // 
            // JsRuntimeHandle runtime;
            // ThrowIfFailed(JsGetRuntime(context, &runtime));
            // 
            // ThrowIfFailed(JsSetCurrentContext(JS_INVALID_REFERENCE));
            // 
            // ThrowIfFailed(JsDisposeRuntime(runtime));
        }
        */

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

        void CreateJsRuntimeForThread(v8::Isolate* isolate)
        {
            v8::Isolate::Scope isolate_scope{ isolate };
            v8::HandleScope isolate_handle_scope{ isolate };
            v8::Local<v8::Context> context = v8::Context::New(isolate);
            v8::Context::Scope context_scope{ context };

            auto env = Napi::Attach();

            // Now we somehow have to do all of this stuff inside here, without ever exiting this scope. :-/
        }

        void DisposeJsRuntimeForThread()
        {

        }
    }

    namespace
    {
        static constexpr auto JS_WINDOW_NAME = "window";
        static constexpr auto JS_NATIVE_NAME = "_native";
        static constexpr auto JS_RUNTIME_NAME = "runtime";
        static constexpr auto JS_NATIVE_WINDOW_NAME = "window";

        static constexpr auto JS_ENGINE_CONSTRUCTOR_NAME = "Engine";
        static constexpr auto JS_XML_HTTP_REQUEST_CONSTRUCTOR_NAME = "XMLHttpRequest";
    }

    RuntimeImpl& RuntimeImpl::GetRuntimeImplFromJavaScript(Napi::Env env)
    {
        const auto& _native{env.Global().Get(JS_NATIVE_NAME).ToObject()};
        return *_native.Get(JS_RUNTIME_NAME).As<Napi::External<RuntimeImpl>>().Data();
    }

    NativeWindow& RuntimeImpl::GetNativeWindowFromJavaScript(Napi::Env env)
    {
        const auto& _native{env.Global().Get(JS_NATIVE_NAME).ToObject()};
        return *NativeWindow::Unwrap(_native.Get(JS_WINDOW_NAME).ToObject());
    }

    RuntimeImpl::RuntimeImpl(void* nativeWindowPtr, const std::string& rootUrl)
        : m_dispatcher{std::make_unique<DispatcherT>()}
        , m_nativeWindowPtr{nativeWindowPtr}
        , m_thread{[this] { ThreadProcedure(); }}
        , m_rootUrl{rootUrl}
    {
    }

    RuntimeImpl::~RuntimeImpl()
    {
        if (m_suspended)
        {
            Resume();
        }
        m_cancelSource.cancel();
        m_thread.join();
    }

    void RuntimeImpl::UpdateSize(float width, float height)
    {
        m_dispatcher->queue([width, height, this] {
            auto& window = RuntimeImpl::GetNativeWindowFromJavaScript(*m_env);
            window.Resize(static_cast<size_t>(width), static_cast<size_t>(height));
        });
    }

    void RuntimeImpl::Suspend()
    {
        std::unique_lock<std::mutex> lockSuspension(m_suspendMutex);
        // Lock block ticking so no rendering will happen once we exit Suspend method
        std::unique_lock<std::mutex> lockTicking(m_blockTickingMutex);
        m_suspended = true;
        m_suspendVariable.notify_one();
    }

    void RuntimeImpl::Resume()
    {
        std::unique_lock<std::mutex> lock(m_suspendMutex);
        m_suspended = false;
        m_suspendVariable.notify_one();
    }

    void RuntimeImpl::LoadScript(const std::string& url)
    {
        auto lock = AcquireTaskLock();
        auto whenAllTask = arcana::when_all(LoadUrlAsync<std::string>(GetAbsoluteUrl(url).data()), Task);
        Task = whenAllTask.then(*m_dispatcher, m_cancelSource, [this, url](const std::tuple<std::string, arcana::void_placeholder>& args) {
            Napi::Eval(*m_env, std::get<0>(args).data(), url.data());
        });
    }

    void RuntimeImpl::Eval(const std::string& string, const std::string& sourceUrl)
    {
        auto lock = AcquireTaskLock();
        Task = Task.then(*m_dispatcher, m_cancelSource, [this, string, sourceUrl]() {
            Napi::Eval(*m_env, string.data(), sourceUrl.data());
        });
    }

    void RuntimeImpl::Dispatch(std::function<void(Napi::Env)> func)
    {
        auto lock = AcquireTaskLock();
        Task = Task.then(*m_dispatcher, m_cancelSource, [func = std::move(func), this]() {
            func(*m_env);
        });
    }

    const std::string& RuntimeImpl::RootUrl() const
    {
        return m_rootUrl;
    }

    std::string RuntimeImpl::GetAbsoluteUrl(const std::string& url)
    {
        auto curl = curl_url();

        auto code = curl_url_set(curl, CURLUPART_URL, url.data(), 0);

        // If input could not be turned into a valid URL, try using it as a regular URL.
        if (code == CURLUE_MALFORMED_INPUT)
        {
            code = curl_url_set(curl, CURLUPART_URL, (m_rootUrl + "/" + url).data(), 0);
        }

        if (code != CURLUE_OK)
        {
            throw std::exception{};
        }

        char* buf;
        code = curl_url_get(curl, CURLUPART_URL, &buf, 0);

        if (code != CURLUE_OK)
        {
            throw std::exception{};
        }

        std::string absoluteUrl{buf};

        curl_free(buf);
        curl_url_cleanup(curl);

        return std::move(absoluteUrl);
    }

    template<typename T>
    arcana::task<T, std::exception_ptr> RuntimeImpl::LoadUrlAsync(const std::string& url)
    {
        return arcana::make_task(*m_dispatcher, m_cancelSource, [url]() {
            T data{};

            auto curl = curl_easy_init();
            if (curl)
            {
                curl_easy_setopt(curl, CURLOPT_URL, url.data());
                curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

                curl_write_callback callback = [](char* buffer, size_t size, size_t nitems, void* userData) {
                    auto& data = *static_cast<T*>(userData);
                    data.insert(data.end(), buffer, buffer + nitems);
                    return nitems;
                };

                curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, callback);
                curl_easy_setopt(curl, CURLOPT_WRITEDATA, &data);

#if (ANDROID)
                /*
                 * /!\ warning! this is a security issue
                 * https://github.com/BabylonJS/BabylonNative/issues/96
                 */
                curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0);
                curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
#endif

                auto result = curl_easy_perform(curl);
                if (result != CURLE_OK)
                {
                    throw std::exception();
                }

                curl_easy_cleanup(curl);
            }

            return std::move(data);
        });
    }

    RuntimeImpl::DispatcherT& RuntimeImpl::Dispatcher()
    {
        return *m_dispatcher;
    }

    arcana::cancellation& RuntimeImpl::Cancellation()
    {
        return m_cancelSource;
    }

    std::scoped_lock<std::mutex> RuntimeImpl::AcquireTaskLock()
    {
        return std::scoped_lock{m_taskMutex};
    }

    void RuntimeImpl::InitializeJavaScriptVariables()
    {
        auto env = *m_env;
        auto global = env.Global();

        global.Set(JS_WINDOW_NAME, global);

        auto jsNative = Napi::Object::New(env);
        global.Set(JS_NATIVE_NAME, jsNative);

        auto jsRuntime = Napi::External<RuntimeImpl>::New(env, this);
        jsNative.Set(JS_RUNTIME_NAME, jsRuntime);

        auto jsWindow = NativeWindow::Create(env, m_nativeWindowPtr, 32, 32);
        jsNative.Set(JS_NATIVE_WINDOW_NAME, jsWindow.Value());
        global.Set("setTimeout", NativeWindow::GetSetTimeoutFunction(jsWindow).Value());
        global.Set("atob", NativeWindow::GetAToBFunction(jsWindow).Value());

        auto jsNativeEngineConstructor = NativeEngine::CreateConstructor(env);
        jsNative.Set(JS_ENGINE_CONSTRUCTOR_NAME, jsNativeEngineConstructor.Value());

        auto jsXmlHttpRequestConstructor = XMLHttpRequest::CreateConstructor(env);
        global.Set(JS_XML_HTTP_REQUEST_CONSTRUCTOR_NAME, jsXmlHttpRequestConstructor.Value());
    }

    void RuntimeImpl::BaseThreadProcedure()
    {
        m_dispatcher->set_affinity(std::this_thread::get_id());

        // Create the JavaScript runtime.
        DispatchFunction dispatchFunction{
            [this](std::function<void()> action) {
                Dispatch([action = std::move(action)](auto&) {
                    action();
                });
            }
        };
        CreateJsRuntimeForThread(dispatchFunction);
        auto jsScopeGuard = gsl::finally([]
        {
            DisposeJsRuntimeForThread();
        });

        // Create the N-API environment
        auto env = Napi::Attach();
        m_env = &env;
        auto envScopeGuard = gsl::finally([this, env]
        {
            // Because the dispatcher and task may take references to the N-API environment,
            // they must be cleared before the env itself is destroyed.
            m_dispatcher.reset();
            Task = arcana::task_from_result<std::exception_ptr>();

            m_env = nullptr;
            Napi::Detach(env);
        });

        InitializeJavaScriptVariables();

        // TODO: Handle device lost/restored.

        while (!m_cancelSource.cancelled())
        {
            // check if suspended
            {
                std::unique_lock<std::mutex> lock(m_suspendMutex);
                m_suspendVariable.wait(lock, [this]() { return !m_suspended; });
            }
            {
                std::unique_lock<std::mutex> lock(m_blockTickingMutex);
                m_dispatcher->blocking_tick(m_cancelSource);
            }
        }
    }

    template arcana::task<std::string, std::exception_ptr> RuntimeImpl::LoadUrlAsync(const std::string& url);
    template arcana::task<std::vector<char>, std::exception_ptr> RuntimeImpl::LoadUrlAsync(const std::string& url);
}
