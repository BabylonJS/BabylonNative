#include "AppRuntime.h"
#include "WorkQueue.h"
#include <exception>
#include <sstream>
#include <functional>
#include <napi/js_native_api.h>
#include <napi/js_native_ext_api.h>

namespace Babylon
{
    struct NapiTask
    {
        NapiTask(
            napi_env env,
            napi_ext_task_callback taskCallback,
            void* taskData,
            napi_finalize finalizeCallback,
            void* finalizeHint) noexcept
            : m_env{env}
            , m_taskCallback{taskCallback}
            , m_taskData{taskData}
            , m_finalizeCallback{finalizeCallback}
            , m_finalizeHint{finalizeHint}
        {
        }

        NapiTask(const NapiTask&) = delete;
        NapiTask& operator=(const NapiTask&) = delete;

        ~NapiTask()
        {
            if (m_finalizeCallback)
            {
                m_finalizeCallback(m_env, m_taskData, m_finalizeHint);
            }
        }

        void operator()() noexcept
        {
            m_taskCallback(m_env, m_taskData);
        }

    private:
        napi_env m_env;
        napi_ext_task_callback m_taskCallback;
        void* m_taskData;
        napi_finalize m_finalizeCallback;
        void* m_finalizeHint;
    };

    void __cdecl ScheduleTaskCallback(
        napi_env env,
        napi_ext_task_callback taskCallback,
        void* taskData,
        uint32_t /*delayInMsec*/,
        napi_finalize finalizeCallback,
        void* finalizeHint)
    {
        WorkQueue* worker;
        auto result = napi_get_instance_data(env, (void**)&worker);

        if (result != napi_status::napi_ok)
        {
            std::terminate();
        }

        auto task = std::make_shared<NapiTask>(env, taskCallback, taskData, finalizeCallback, finalizeHint);
        worker->Append([task = std::move(task)](Napi::Env)
            { task->operator()(); });
    }

    void AppRuntime::RunEnvironmentTier(const char*)
    {
        napi_env _env{};
        napi_ext_env_settings settings{};
        settings.this_size = sizeof(settings);
        settings.inspector_port = 5643;
        settings.flags.wait_for_debugger = false;
        settings.foreground_scheduler = &ScheduleTaskCallback;

        auto result = napi_ext_create_env(&settings, &_env);
        assert(result == napi_status::napi_ok);

        result = napi_set_instance_data(_env, this->m_workQueue.get(), nullptr /*finalize_cb*/, nullptr /*finalize_hint*/);
        assert(result == napi_status::napi_ok);

        Napi::Env env = Napi::Env(_env);

        napi_ext_env_scope scope;
        result = napi_ext_open_env_scope(env, &scope);
        assert(result == napi_status::napi_ok);

        Run(env);

        result = napi_ext_close_env_scope(env, scope);
        assert(result == napi_status::napi_ok);
    }
}