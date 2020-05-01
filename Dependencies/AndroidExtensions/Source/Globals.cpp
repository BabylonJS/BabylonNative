#include <AndroidExtensions/Globals.h>
#include <stdexcept>

namespace android::global
{
    namespace
    {
        JavaVM* g_javaVM{};
        jobject g_appContext{};

        thread_local struct Env
        {
            ~Env()
            {
                if (m_attached)
                {
                    g_javaVM->DetachCurrentThread();
                }
            }

            bool m_attached{};
        } g_env{};

        template<typename ... Args>
        class Event
        {
            using Handler = std::function<void(Args ...)>;

        public:
            typename arcana::ticketed_collection<Handler>::ticket AddHandler(Handler&& handler)
            {
                std::lock_guard<std::mutex> guard{m_mutex};
                return m_handlers.insert(handler, m_mutex);
            }

            void Fire(Args ... args)
            {
                std::lock_guard<std::mutex> guard{m_mutex};
                for (auto& handler : m_handlers)
                {
                    handler(args ...);
                }
            }

        private:
            std::mutex m_mutex{};
            arcana::ticketed_collection<Handler> m_handlers{};
        };

        Event g_pausedEvent{};
        Event g_resumedEvent{};

        Event<int32_t, const std::vector<std::string>&, const std::vector<int32_t>&> g_requestPermissionsResultEvent{};
    }

    void Initialize(JavaVM* javaVM, jobject appContext)
    {
        g_javaVM = javaVM;
        g_appContext = appContext;
    }

    JNIEnv* GetEnvForCurrentThread()
    {
        JNIEnv* env{};

        if (g_javaVM->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) == JNI_EDETACHED)
        {
            if (g_javaVM->AttachCurrentThread(&env, nullptr) != 0) {
                throw std::runtime_error("Failed to attach JavaScript thread to Java VM");
            }

            g_env.m_attached = true;
        }

        return env;
    }

    android::content::Context GetAppContext()
    {
        return {g_appContext};
    }

    void Pause()
    {
        g_pausedEvent.Fire();
    }

    AppStateChangedCallbackTicket AddPausedCallback(std::function<void()>&& onPaused)
    {
        return g_pausedEvent.AddHandler(std::move(onPaused));
    }

    void Resume()
    {
        g_resumedEvent.Fire();
    }

    AppStateChangedCallbackTicket AddResumedCallback(std::function<void()>&& onResumed)
    {
        return g_resumedEvent.AddHandler(std::move(onResumed));
    }

    void RequestPermissionsResult(int32_t requestCode, const std::vector<std::string>& permissions, const std::vector<int32_t>& grantResults)
    {
        g_requestPermissionsResultEvent.Fire(requestCode, permissions, grantResults);
    }

    RequestPermissionsResultCallbackTicket AddRequestPermissionsResultCallback(std::function<void(int32_t, const std::vector<std::string>&, const std::vector<int32_t>&)>&& onAddRequestPermissionsResult)
    {
        return g_requestPermissionsResultEvent.AddHandler(std::move(onAddRequestPermissionsResult));
    }
}
