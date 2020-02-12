#include "RuntimeImpl.h"

#include "NativeEngine.h"
#include "NativeWindow.h"
#include "XMLHttpRequest.h"

#include <curl/curl.h>
#include <napi/env.h>
#include <sstream>

namespace Babylon
{
    namespace
    {
        static constexpr auto JS_WINDOW_NAME = "window";

        std::string GetAbsoluteUrl(const std::string& url, const std::string& rootUrl)
        {
            auto curl = curl_url();

            auto code = curl_url_set(curl, CURLUPART_URL, url.data(), 0);

            // If input could not be turned into a valid URL, try using it as a regular URL.
            if (code == CURLUE_MALFORMED_INPUT)
            {
                code = curl_url_set(curl, CURLUPART_URL, (rootUrl + "/" + url).data(), 0);
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

            std::string absoluteUrl{ buf };

            curl_free(buf);
            curl_url_cleanup(curl);

            return std::move(absoluteUrl);
        }
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
            auto& window = NativeWindow::GetFromJavaScript(*m_env);
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
        auto whenAllTask = arcana::when_all(LoadUrlAsync<std::string>(GetAbsoluteUrl(url, m_rootUrl).data()), Task);
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

        JsRuntime::Initialize(env, [this](std::function<void(Napi::Env)> func){ Dispatch(std::move(func)); });
        NativeWindow::Initialize(env, m_nativeWindowPtr, 32, 32);
        NativeEngine::Initialize(env);
        XMLHttpRequest::Initialize(env, *this);
    }

    void RuntimeImpl::RunJavaScript(Napi::Env env)
    {
        m_dispatcher->set_affinity(std::this_thread::get_id());

        m_env = &env;
        auto envScopeGuard = gsl::finally([this, env]
        {
            // Because the dispatcher and task may take references to the N-API environment,
            // they must be cleared before the env itself is destroyed.
            m_dispatcher.reset();
            Task = arcana::task_from_result<std::exception_ptr>();

            m_env = nullptr;
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
