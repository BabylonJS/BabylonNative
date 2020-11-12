#include "BabylonNative.h"

#include <Babylon/Graphics.h>
#include <Babylon/JsRuntime.h>
#include <Babylon/Plugins/NativeEngine.h>
#include <Babylon/Plugins/NativeInput.h>
#include <Babylon/Plugins/NativeXr.h>
#include <Babylon/Polyfills/Window.h>
#include <Babylon/Polyfills/XMLHttpRequest.h>

#include <arcana/threading/task_schedulers.h>

#include <jsi/jsi.h>

#include <CoreFoundation/CoreFoundation.h>

#include <optional>
#include <sstream>
#include <unistd.h>

#include <DispatchFunction.h>

namespace Babylon
{
    using namespace facebook;

    namespace
    {
        bool isShuttingDown{false};
    }

    class Native::Impl
    {
    public:
        Impl(facebook::jsi::Runtime& jsiRuntime, std::shared_ptr<facebook::react::CallInvoker> callInvoker)
            : env{ Napi::Attach<facebook::jsi::Runtime&>(jsiRuntime) }
            , jsCallInvoker{ callInvoker }
        {
        }
        
        ~Impl()
        {
            Napi::Detach(env);
        }

        Napi::Env env;
        std::shared_ptr<facebook::react::CallInvoker> jsCallInvoker;
        std::unique_ptr<Graphics> graphics{};
        JsRuntime* runtime{};
        Plugins::NativeInput* nativeInput{};
    };

    Native::Native(facebook::jsi::Runtime& jsiRuntime, std::shared_ptr<facebook::react::CallInvoker> callInvoker, void* windowPtr, size_t width, size_t height)
        : m_impl{ std::make_unique<Native::Impl>(jsiRuntime, callInvoker) }
    {
        isShuttingDown = false;
        m_impl->graphics = Graphics::CreateGraphics(reinterpret_cast<void*>(windowPtr), width, height);

        m_impl->runtime = &JsRuntime::CreateForJavaScript(m_impl->env, CreateJsRuntimeDispatcher(m_impl->env, jsiRuntime, std::move(callInvoker), isShuttingDown));

        m_impl->graphics->AddToJavaScript(m_impl->env);

        Polyfills::Window::Initialize(m_impl->env);
        // NOTE: React Native's XMLHttpRequest is slow and allocates a lot of memory. This does not override
        // React Native's implementation, but rather adds a second one scoped to Babylon and used by WebRequest.ts.
        Polyfills::XMLHttpRequest::Initialize(m_impl->env);

        Plugins::NativeEngine::Initialize(m_impl->env, true);
        Plugins::NativeXr::Initialize(m_impl->env);

        m_impl->nativeInput = &Babylon::Plugins::NativeInput::CreateForJavaScript(m_impl->env);
    }

    // NOTE: This only happens when the JS engine is shutting down (other than when the app exits, this only
    //       happens during a dev mode reload). In this case, EngineHook.ts won't call NativeEngine.dispose,
    //       so we need to manually do it here to properly clean up these resources.
    Native::~Native()
    {
        auto native = JsRuntime::NativeObject::GetFromJavaScript(m_impl->env);
        auto engine = native.Get("engineInstance").As<Napi::Object>();
        auto dispose = engine.Get("dispose").As<Napi::Function>();
        dispose.Call(engine, {});
        isShuttingDown = true;
    }

    void Native::Refresh(void* windowPtr, size_t width, size_t height)
    {
        m_impl->graphics->UpdateWindow<void*>(windowPtr);
        m_impl->graphics->UpdateSize(width, height);
    }

    void Native::Resize(size_t width, size_t height)
    {
        m_impl->graphics->UpdateSize(width, height);
    }

    void Native::SetPointerButtonState(uint32_t pointerId, uint32_t buttonId, bool isDown, uint32_t x, uint32_t y)
    {
        if (isDown)
        {
            m_impl->nativeInput->PointerDown(pointerId, buttonId, x, y);
        }
        else
        {
            m_impl->nativeInput->PointerUp(pointerId, buttonId, x, y);
        }
    }

    void Native::SetPointerPosition(uint32_t pointerId, uint32_t x, uint32_t y)
    {
        m_impl->nativeInput->PointerMove(pointerId, x, y);
    }
}
