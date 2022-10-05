#include "Window.h"

#include <Babylon/Graphics/DeviceContext.h>

#include <basen.hpp>
#include <chrono>
#include <iterator>

namespace
{
    using TimeoutId = Babylon::Polyfills::Internal::TimeoutId;
    using Milliseconds = std::chrono::milliseconds::rep;
    using TimePoint = std::chrono::time_point<std::chrono::steady_clock, std::chrono::milliseconds>;

    TimePoint Now()
    {
        return std::chrono::time_point_cast<std::chrono::milliseconds, std::chrono::steady_clock>(std::chrono::steady_clock::now());
    }

    struct Timeout
    {
        TimeoutId id;
        std::shared_ptr<Napi::FunctionReference> function;
        Milliseconds time;
        TimePoint timePoint;

        Timeout(TimeoutId id, std::shared_ptr<Napi::FunctionReference> func, Milliseconds time, TimePoint timePoint)
            : id{ id }
            , function{ std::move(func) }
            , time{ time }
            , timePoint{ timePoint }
        {
        }
    };

    class TimeoutDispatcher
    {
    public:
        TimeoutDispatcher(Babylon::JsRuntime& runtime)
            : m_runtime(runtime)
            , m_thread{&TimeoutDispatcher::WaitThenCallProc, this}
        { }

        TimeoutId Dispatch(std::shared_ptr<Napi::FunctionReference> func, Milliseconds time)
        {
            if (time <= 0)
            {
                CallFunction(std::move(func));
                return 0;
            }

            const auto now = Now();
            std::unique_lock<std::mutex> lk(m_mutex);
            const auto soonestTimePoint =
                m_timeMap.empty()
                    ? now + std::chrono::milliseconds{ 1 }
                    : m_timeMap.cbegin()->second.front().timePoint;

            const auto timePoint = now + std::chrono::milliseconds(time);
            const Timeout timeout = Timeout{ NextTimeoutId(), std::move(func), time, timePoint };
            m_idMap.insert({ timeout.id, timeout });
            m_timeMap[time].push_back(timeout);

            if (timePoint <= soonestTimePoint)
            {
                m_condVariable.notify_one();
            }

            return timeout.id;
        }

        void Clear(TimeoutId id)
        {
            std::unique_lock<std::mutex> lk(m_mutex);
            const auto idIterator = m_idMap.find(id);
            if (idIterator != m_idMap.end())
            {
                const auto& milliseconds = idIterator->second.time;
                const auto& timeoutId = idIterator->second.id;

                const auto timeIterator = m_timeMap.find(milliseconds);
                assert(timeIterator != m_timeMap.end() && "m_idMap and m_timeMap are out of sync");

                auto& timeouts = timeIterator->second;
                for (auto i = timeouts.begin(); i != timeouts.end(); i++)
                {
                    if ((*i).id == timeoutId)
                    {
                        timeouts.erase(i);
                        break;
                    }
                }
                
                m_idMap.erase(idIterator);
            }
        }

        void WaitThenCallProc()
        {
            TimePoint nextTimePoint{};
            while (!m_shutdown) 
            {
                std::unique_lock<std::mutex> lk(m_mutex);
                while (!m_timeMap.empty() && Now() < (nextTimePoint = m_timeMap.begin()->second.front().timePoint))
                {
                    m_condVariable.wait_until(lk, nextTimePoint);
                }

                if (!m_timeMap.empty())
                {
                    auto &timeouts = m_timeMap.begin()->second;
                    while (!timeouts.empty())
                    {
                        auto& timeout = timeouts.front();
                        CallFunction(timeout.function);
                        m_idMap.erase(timeout.id);
                        timeouts.pop_front();
                    }
                    m_timeMap.erase(m_timeMap.begin());
                }

                while (!m_shutdown && m_timeMap.empty())
                {
                    m_condVariable.wait(lk);
                }
            }
        }

        void CallFunction(std::shared_ptr<Napi::FunctionReference> func) {
            m_runtime.Dispatch([func = std::move(func)](Napi::Env) {
                func->Call({});
            });
        }

        ~TimeoutDispatcher() 
        {
            {
                std::unique_lock<std::mutex> lk(m_mutex);
                m_idMap.clear();
                m_timeMap.clear();
                m_shutdown = true;
                m_condVariable.notify_one();
            }
            
            if (m_thread.joinable())
            {
                m_thread.join();
            }
        }

    private:
        TimeoutId NextTimeoutId()
        {
            while (true)
            {
                ++m_lastTimeoutId;

                if (m_lastTimeoutId <= 0)
                {
                    m_lastTimeoutId = 1;
                }

                if (m_idMap.find(m_lastTimeoutId) == m_idMap.end())
                {
                    return m_lastTimeoutId;
                }
            }
        }

        Babylon::JsRuntime& m_runtime;
        std::thread m_thread;
        std::mutex m_mutex{};
        std::condition_variable m_condVariable{};
        TimeoutId m_lastTimeoutId = 0;
        std::unordered_map<TimeoutId, Timeout> m_idMap;
        std::map<Milliseconds, std::list<Timeout>> m_timeMap;
        bool m_shutdown{false};
    };
}

namespace Babylon::Polyfills::Internal
{
    namespace
    {
        constexpr auto JS_CLASS_NAME = "Window";
        constexpr auto JS_SET_TIMEOUT_NAME = "setTimeout";
        constexpr auto JS_CLEAR_TIMEOUT_NAME = "clearTimeout";
        constexpr auto JS_A_TO_B_NAME = "atob";
        constexpr auto JS_ADD_EVENT_LISTENER_NAME = "addEventListener";
        constexpr auto JS_REMOVE_EVENT_LISTENER_NAME = "removeEventListener";
        constexpr auto JS_DEVICE_PIXEL_RATIO_NAME = "devicePixelRatio";
    }

    void Window::Initialize(Napi::Env env)
    {
        Napi::HandleScope scope{env};

        Napi::Function constructor = DefineClass(
            env,
            JS_CLASS_NAME,
            {});

        auto global = env.Global();
        auto jsNative = JsRuntime::NativeObject::GetFromJavaScript(env);
        auto jsWindow = constructor.New({});

        jsNative.Set(JS_WINDOW_NAME, jsWindow);

        if (global.Get(JS_SET_TIMEOUT_NAME).IsUndefined())
        {
            global.Set(JS_SET_TIMEOUT_NAME, Napi::Function::New(env, &Window::SetTimeout, JS_SET_TIMEOUT_NAME, Window::Unwrap(jsWindow)));
        }

        if (global.Get(JS_CLEAR_TIMEOUT_NAME).IsUndefined())
        {
            global.Set(JS_CLEAR_TIMEOUT_NAME, Napi::Function::New(env, &Window::ClearTimeout, JS_CLEAR_TIMEOUT_NAME, Window::Unwrap(jsWindow)));
        }

        if (global.Get(JS_A_TO_B_NAME).IsUndefined())
        {
            global.Set(JS_A_TO_B_NAME, Napi::Function::New(env, &Window::DecodeBase64, JS_A_TO_B_NAME));
        }

        if (global.Get(JS_ADD_EVENT_LISTENER_NAME).IsUndefined())
        {
            global.Set(JS_ADD_EVENT_LISTENER_NAME, Napi::Function::New(env, &Window::AddEventListener, JS_ADD_EVENT_LISTENER_NAME));
        }

        if (global.Get(JS_REMOVE_EVENT_LISTENER_NAME).IsUndefined())
        {
            global.Set(JS_REMOVE_EVENT_LISTENER_NAME, Napi::Function::New(env, &Window::RemoveEventListener, JS_REMOVE_EVENT_LISTENER_NAME));
        }

        if (global.Get(JS_DEVICE_PIXEL_RATIO_NAME).IsUndefined()){
            // Create an accessor to add to the window object to define window.devicePixelRatio
            Napi::Object descriptor{Napi::Object::New(env)};
            descriptor.Set("enumerable", Napi::Value::From(env, true));
            descriptor.Set("get", Napi::Function::New(env, &Window::GetDevicePixelRatio, JS_DEVICE_PIXEL_RATIO_NAME, &jsWindow));
            Napi::Object object{global.Get("Object").As<Napi::Object>()};
            Napi::Function defineProperty{object.Get("defineProperty").As<Napi::Function>()};
            defineProperty.Call(object, {global, Napi::String::New(env, JS_DEVICE_PIXEL_RATIO_NAME), descriptor});
        }
    }

    Window& Window::GetFromJavaScript(Napi::Env env)
    {
        return *Window::Unwrap(JsRuntime::NativeObject::GetFromJavaScript(env).Get(JS_WINDOW_NAME).As<Napi::Object>());
    }

    Window::Window(const Napi::CallbackInfo& info)
        : Napi::ObjectWrap<Window>{info}
        , m_runtime{JsRuntime::GetFromJavaScript(info.Env())}
        , m_timeoutDispatcher{std::make_unique<TimeoutDispatcher>(m_runtime)}
    {
    }

    Napi::Value Window::SetTimeout(const Napi::CallbackInfo& info)
    {
        auto function = Napi::Persistent(info[0].As<Napi::Function>());
        auto milliseconds = std::chrono::milliseconds{info[1].As<Napi::Number>().Int32Value()}.count();

        auto functionRef = std::make_shared<Napi::FunctionReference>(std::move(function));

        auto& window = *static_cast<Window*>(info.Data());
        return Napi::Value::From(info.Env(), window.m_timeoutDispatcher->Dispatch(functionRef, milliseconds));
    }

    void Window::ClearTimeout(const Napi::CallbackInfo& info)
    {
        auto timeoutId = info[0].As<Napi::Number>().Int32Value();
        auto& window = *static_cast<Window*>(info.Data());
        window.m_timeoutDispatcher->Clear(timeoutId);
    }

    Napi::Value Window::DecodeBase64(const Napi::CallbackInfo& info)
    {
        std::string encodedData = info[0].As<Napi::String>().Utf8Value();
        std::u16string decodedData;
        bn::decode_b64(encodedData.begin(), encodedData.end(), std::back_inserter(decodedData));
        return Napi::Value::From(info.Env(), decodedData);
    }

    void Window::AddEventListener(const Napi::CallbackInfo& /*info*/)
    {
        // TODO: handle events
    }

    void Window::RemoveEventListener(const Napi::CallbackInfo& /*info*/)
    {
        // TODO: handle events
    }

    Napi::Value Window::GetDevicePixelRatio(const Napi::CallbackInfo& info)
    {
        auto env{info.Env()};
        return Napi::Value::From(env, Graphics::DeviceContext::GetFromJavaScript(env).GetDevicePixelRatio());
    }
}

namespace Babylon::Polyfills::Window
{
    void Initialize(Napi::Env env)
    {
        Internal::Window::Initialize(env);
    }
}
