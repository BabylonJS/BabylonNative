#include <gtest/gtest.h>
#include <arcana/threading/task.h>

#include <cassert>
#include <deque>
#include <functional>
#include <memory>
#include <optional>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

// Only the XR backend, graphics scheduler, and JS references are replaced. The lifecycle
// methods below are taken directly from NativeXrImpl.cpp at CMake configure time.
namespace NativeXrShutdownHarness
{
    namespace
    {
        using Task = arcana::task<void, std::exception_ptr>;
        using Completion = arcana::task_completion_source<void, std::exception_ptr>;

        struct Scheduler
        {
            std::deque<std::function<void()>> Work{};

            template<typename Callable>
            void operator()(Callable&& callable)
            {
                auto owned = std::make_shared<std::decay_t<Callable>>(std::forward<Callable>(callable));
                Work.emplace_back([owned] { (*owned)(); });
            }

            void Drain()
            {
                while (!Work.empty())
                {
                    auto callback = std::move(Work.front());
                    Work.pop_front();
                    callback();
                }
            }
        };

        namespace Graphics
        {
            struct DeviceContext
            {
                Scheduler RenderScheduler{};
                Scheduler& AfterRenderScheduler() { return RenderScheduler; }
                static DeviceContext& GetFromJavaScript(DeviceContext* context) { return *context; }
            };
        }

        namespace bgfx
        {
            struct InternalData
            {
                void* context{};
                void* commandQueue{};
            };

            InternalData* getInternalData()
            {
                static InternalData data{};
                return &data;
            }
        }

        namespace xr
        {
            struct System
            {
                bool IsInitialized() { return true; }
                bool TryInitialize() { return true; }

                struct Session
                {
                    struct Frame
                    {
                        void Render() {}
                    };

                    int EndRequests{};
                    std::exception_ptr Failure{};
                    void RequestEndSession() { ++EndRequests; }
                    std::shared_ptr<Frame> GetNextFrame(bool& shouldEnd, bool&)
                    {
                        if (Failure)
                        {
                            std::rethrow_exception(Failure);
                        }
                        shouldEnd = true;
                        return std::make_shared<Frame>();
                    }

                    static arcana::task<std::shared_ptr<Session>, std::exception_ptr> CreateAsync(
                        System&, void*, void*, std::function<void*()>)
                    {
                        return arcana::task_from_result<std::exception_ptr>(std::make_shared<Session>());
                    }
                };
            };
        }

        struct Reference
        {
            void Reset() {}
        };
    }

    struct NativeXr
    {
        struct Impl : std::enable_shared_from_this<Impl>
        {
            struct SessionState
            {
                explicit SessionState(Graphics::DeviceContext& context) : GraphicsContext{context} {}
                Graphics::DeviceContext& GraphicsContext;
                std::vector<int> ActiveViewConfigurations{};
                std::vector<int> ViewConfigurationStartViewIdx{};
                std::vector<int> TextureToViewConfigurationMap{};
                std::vector<int> ScheduleFrameCallbacks{};
                Reference CreateRenderTexture{};
                std::shared_ptr<xr::System::Session> Session{};
                std::shared_ptr<xr::System::Session::Frame> Frame{};
                arcana::cancellation_source CancellationSource{};
                Task FrameTask{arcana::task_from_result<std::exception_ptr>()};
            };

            Graphics::DeviceContext* m_env{};
            void* m_windowPtr{};
            std::optional<Task> m_beginTask{};
            Task m_endTask{arcana::task_from_result<std::exception_ptr>()};
            bool m_sessionEnding{false};
            std::unique_ptr<SessionState> m_sessionState{};
            xr::System m_system{};
            int Started{};
            int Ended{};
            std::function<void()> OnEnded{};

            void NotifySessionStateChanged(bool active)
            {
                if (active)
                {
                    ++Started;
                }
                else
                {
                    ++Ended;
                    if (OnEnded)
                    {
                        OnEnded();
                    }
                }
            }

            Task BeginSessionAsync();
            Task EndSessionAsync();
        };
    };

#include "NativeXrSessionMethods.inc"

    namespace
    {
        class NativeXrShutdown : public testing::Test
        {
        protected:
            Graphics::DeviceContext Context{};
            std::shared_ptr<NativeXr::Impl> Impl{std::make_shared<NativeXr::Impl>()};
            int Completed{};
            std::vector<std::exception_ptr> Errors{};

            NativeXrShutdown() { Impl->m_env = &Context; }

            void Observe(Task task)
            {
                task.then(arcana::inline_scheduler, arcana::cancellation::none(),
                    [this](const arcana::expected<void, std::exception_ptr>& result) {
                        ++Completed;
                        if (result.has_error())
                        {
                            Errors.push_back(result.error());
                        }
                    });
            }

            std::shared_ptr<xr::System::Session> Start()
            {
                Observe(Impl->BeginSessionAsync());
                Context.RenderScheduler.Drain();
                EXPECT_NE(Impl->m_sessionState, nullptr);
                EXPECT_EQ(Impl->Started, 1);
                EXPECT_EQ(Completed, 1);
                Completed = 0;
                return Impl->m_sessionState->Session;
            }
        };

        TEST_F(NativeXrShutdown, EndWithoutSessionReusesCompletedTask)
        {
            Observe(Impl->EndSessionAsync());
            Observe(Impl->EndSessionAsync());
            EXPECT_EQ(Completed, 2);
            EXPECT_TRUE(Errors.empty());
            EXPECT_TRUE(Context.RenderScheduler.Work.empty());
        }

        TEST_F(NativeXrShutdown, PendingFrameCoalescesConcurrentRequests)
        {
            auto backend = Start();
            Completion frame{};
            Impl->m_sessionState->FrameTask = frame.as_task();
            Observe(Impl->EndSessionAsync());
            Observe(Impl->EndSessionAsync());
            Observe(Impl->EndSessionAsync());
            Context.RenderScheduler.Drain();
            EXPECT_EQ(Completed, 0);
            EXPECT_EQ(backend->EndRequests, 0);
            frame.complete();
            Context.RenderScheduler.Drain();
            EXPECT_EQ(Completed, 3);
            EXPECT_EQ(backend->EndRequests, 1);
            EXPECT_EQ(Impl->Ended, 1);
            EXPECT_EQ(Impl->m_sessionState, nullptr);
            EXPECT_FALSE(Impl->m_beginTask);
            EXPECT_TRUE(Errors.empty());
            Observe(Impl->EndSessionAsync());
            EXPECT_EQ(Completed, 4);
            EXPECT_TRUE(Context.RenderScheduler.Work.empty());
        }

        TEST_F(NativeXrShutdown, CancellationReentrancyReceivesPendingSharedTask)
        {
            auto backend = Start();
            int cancelled{};
            {
                auto ticket = Impl->m_sessionState->CancellationSource.add_listener([&] {
                    ++cancelled;
                    Observe(Impl->EndSessionAsync());
                    EXPECT_EQ(Completed, 0);
                });
                Observe(Impl->EndSessionAsync());
            }
            EXPECT_EQ(cancelled, 1);
            EXPECT_EQ(Completed, 0);
            Context.RenderScheduler.Drain();
            EXPECT_EQ(Completed, 2);
            EXPECT_EQ(backend->EndRequests, 1);
            EXPECT_EQ(Impl->Ended, 1);
            EXPECT_TRUE(Errors.empty());
        }

        TEST_F(NativeXrShutdown, RejectedBeginDoesNotResetEndingGuard)
        {
            auto backend = Start();
            Observe(Impl->EndSessionAsync());
            Observe(Impl->BeginSessionAsync());
            EXPECT_TRUE(Impl->m_sessionEnding);
            EXPECT_EQ(Completed, 1);
            ASSERT_EQ(Errors.size(), 1);
            Observe(Impl->EndSessionAsync());
            Context.RenderScheduler.Drain();
            EXPECT_EQ(Completed, 3);
            EXPECT_EQ(backend->EndRequests, 1);
            EXPECT_EQ(Impl->Ended, 1);
        }

        TEST_F(NativeXrShutdown, NewSessionCanEndAfterPreviousShutdown)
        {
            auto first = Start();
            Observe(Impl->EndSessionAsync());
            Context.RenderScheduler.Drain();
            EXPECT_TRUE(Impl->m_sessionEnding);
            Observe(Impl->BeginSessionAsync());
            EXPECT_FALSE(Impl->m_sessionEnding);
            Context.RenderScheduler.Drain();
            ASSERT_NE(Impl->m_sessionState, nullptr);
            auto second = Impl->m_sessionState->Session;
            Observe(Impl->EndSessionAsync());
            Observe(Impl->EndSessionAsync());
            Context.RenderScheduler.Drain();
            EXPECT_EQ(Completed, 4);
            EXPECT_EQ(first->EndRequests, 1);
            EXPECT_EQ(second->EndRequests, 1);
            EXPECT_EQ(Impl->Started, 2);
            EXPECT_EQ(Impl->Ended, 2);
            EXPECT_TRUE(Errors.empty());
        }

        TEST_F(NativeXrShutdown, TeardownFailureIsSharedWithLaterRequests)
        {
            auto backend = Start();
            auto failure = std::make_exception_ptr(std::runtime_error{"teardown failure"});
            backend->Failure = failure;
            Observe(Impl->EndSessionAsync());
            Observe(Impl->EndSessionAsync());
            Context.RenderScheduler.Drain();
            Observe(Impl->EndSessionAsync());
            EXPECT_EQ(Completed, 3);
            ASSERT_EQ(Errors.size(), 3);
            for (const auto& error : Errors)
            {
                EXPECT_EQ(error, failure);
            }
            EXPECT_TRUE(Impl->m_sessionEnding);
            EXPECT_EQ(backend->EndRequests, 1);
            EXPECT_EQ(Impl->Ended, 0);
        }

        TEST_F(NativeXrShutdown, FailureAfterStateResetIsNotReplacedWithSuccess)
        {
            auto backend = Start();
            auto failure = std::make_exception_ptr(std::runtime_error{"notification failure"});
            Impl->OnEnded = [failure] { std::rethrow_exception(failure); };
            Observe(Impl->EndSessionAsync());
            Context.RenderScheduler.Drain();
            ASSERT_EQ(Impl->m_sessionState, nullptr);
            ASSERT_FALSE(Impl->m_beginTask);
            Observe(Impl->EndSessionAsync());
            EXPECT_EQ(Completed, 2);
            ASSERT_EQ(Errors.size(), 2);
            EXPECT_EQ(Errors[0], failure);
            EXPECT_EQ(Errors[1], failure);
            EXPECT_EQ(backend->EndRequests, 1);
        }
    }
}
