#include <Babylon/ScriptLoader.h>
#include <UrlLib/UrlLib.h>
#include <arcana/threading/task.h>
#include <arcana/tracing/trace_region.h>
#include <sstream>
#include "Babylon/DebugTrace.h"

namespace Babylon
{
    class ScriptLoader::Impl
    {
    public:
        Impl(DispatchFunctionT dispatchFunction)
            : m_dispatchFunction{dispatchFunction}
            , m_task{arcana::task_from_result<std::exception_ptr>()}
        {
        }

        void LoadScript(std::string url)
        {
            UrlLib::UrlRequest request;
            std::string traceName = (std::ostringstream{} << "Loading script at url " << url).str();
            DEBUG_TRACE(traceName);
            arcana::trace_region requestRegion{traceName.c_str()};
            request.Open(UrlLib::UrlMethod::Get, url);
            request.ResponseType(UrlLib::UrlResponseType::String);
            const auto requestTask = request.SendAsync().then(arcana::inline_scheduler, arcana::cancellation::none(), [requestRegion{std::move(requestRegion)}]() { });
            m_task = arcana::when_all(m_task, requestTask).then(arcana::inline_scheduler, arcana::cancellation::none(), [dispatchFunction = m_dispatchFunction, request = std::move(request), url = std::move(url)](auto) mutable {
                arcana::task_completion_source<void, std::exception_ptr> taskCompletionSource{};
                dispatchFunction([taskCompletionSource, request = std::move(request), url = std::move(url)](Napi::Env env) mutable {
                    std::string traceName = (std::ostringstream{} << "Evaluating script at url " << url << " (LoadScript)").str();
                    DEBUG_TRACE(traceName.c_str());
                    arcana::trace_region evalRegion{traceName.c_str()};
                    Napi::Eval(env, request.ResponseString().data(), url.data());
                    taskCompletionSource.complete();
                });
                return taskCompletionSource.as_task();
            });
        }

        void Eval(std::string source, std::string url)
        {
            m_task = m_task.then(arcana::inline_scheduler, arcana::cancellation::none(),
                [dispatchFunction = m_dispatchFunction, source = std::move(source), url = std::move(url)](auto) mutable {
                    arcana::task_completion_source<void, std::exception_ptr> taskCompletionSource{};
                    dispatchFunction([taskCompletionSource, source = std::move(source), url = std::move(url)](Napi::Env env) mutable {
                        std::string traceName = (std::ostringstream{} << "Evaluating script at url " << url << " (Eval)").str();
                        arcana::trace_region evalRegion{traceName.c_str()};
                        Napi::Eval(env, source.data(), url.data());
                        taskCompletionSource.complete();
                    });
                    return taskCompletionSource.as_task();
                });
        }

        void Dispatch(std::function<void(Napi::Env)> callback)
        {
            m_task = m_task.then(arcana::inline_scheduler, arcana::cancellation::none(),
                [dispatchFunction = m_dispatchFunction, callback = std::move(callback)](auto) mutable {
                    arcana::task_completion_source<void, std::exception_ptr> taskCompletionSource{};
                    dispatchFunction([taskCompletionSource, callback = std::move(callback)](Napi::Env env) mutable {
                        callback(env);
                        taskCompletionSource.complete();
                    });
                    return taskCompletionSource.as_task();
                });
        }

    private:
        DispatchFunctionT m_dispatchFunction{};
        arcana::task<void, std::exception_ptr> m_task{};
    };

    ScriptLoader::ScriptLoader(DispatchFunctionT dispatchFunction)
        : m_impl{std::make_unique<ScriptLoader::Impl>(std::move(dispatchFunction))}
    {
    }

    ScriptLoader::~ScriptLoader()
    {
    }

    // Move semantics
    ScriptLoader::ScriptLoader(ScriptLoader&&) noexcept = default;
    ScriptLoader& ScriptLoader::operator=(ScriptLoader&&) noexcept = default;

    void ScriptLoader::LoadScript(std::string url)
    {
        m_impl->LoadScript(std::move(url));
    }

    void ScriptLoader::Eval(std::string source, std::string url)
    {
        m_impl->Eval(std::move(source), std::move(url));
    }

    void ScriptLoader::Dispatch(std::function<void BABYLON_API (Napi::Env)> callback)
    {
        m_impl->Dispatch(std::move(callback));
    }
}
