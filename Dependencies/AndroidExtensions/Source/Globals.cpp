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

        std::mutex g_pauseMutex{};
        arcana::ticketed_collection<std::function<void()>> g_pauseHandlers{};
        std::mutex g_resumeMutex{};
        arcana::ticketed_collection<std::function<void()>> g_resumeHandlers{};
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
        std::lock_guard<std::mutex> guard{g_pauseMutex};
        for (auto& onPaused : g_pauseHandlers)
        {
            onPaused();
        }
    }

    AppStateChangedCallbackTicket AddPausedCallback(std::function<void()>&& onPaused)
    {
        std::lock_guard<std::mutex> guard{g_pauseMutex};
        return g_pauseHandlers.insert(onPaused, g_pauseMutex);
    }

    void Resume()
    {
        std::lock_guard<std::mutex> guard{g_resumeMutex};
        for (auto& onResumed : g_resumeHandlers)
        {
            onResumed();
        }
    }

    AppStateChangedCallbackTicket AddResumedCallback(std::function<void()>&& onResumed)
    {
        std::lock_guard<std::mutex> guard{g_resumeMutex};
        return g_resumeHandlers.insert(onResumed, g_resumeMutex);
    }
}
