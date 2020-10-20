#include "BabylonNative.h"

#include <Babylon/Graphics.h>
#include <Babylon/JsRuntime.h>
#include <Babylon/Plugins/NativeWindow.h>
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

    class Native::Impl
    {
    public:
        Impl(facebook::jsi::Runtime& jsiRuntime, std::shared_ptr<facebook::react::CallInvoker> callInvoker)
            : env{ Napi::Attach<facebook::jsi::Runtime&>(jsiRuntime) }
            , jsCallInvoker{ callInvoker }
        {
        }

        Napi::Env env;
        std::shared_ptr<facebook::react::CallInvoker> jsCallInvoker;
        std::unique_ptr<Graphics> m_graphics{};
        JsRuntime* runtime{};
        Plugins::NativeInput* nativeInput{};
    };

    Native::Native(facebook::jsi::Runtime& jsiRuntime, std::shared_ptr<facebook::react::CallInvoker> callInvoker, void* windowPtr, size_t width, size_t height)
        : m_impl{ std::make_unique<Native::Impl>(jsiRuntime, callInvoker) }
    {
        dispatch_sync(dispatch_get_main_queue(), ^{
            m_impl->m_graphics = Graphics::InitializeFromWindow<void*>(windowPtr, width, height);
        });

        m_impl->runtime = &JsRuntime::CreateForJavaScript(m_impl->env, CreateJsRuntimeDispatcher(m_impl->env, jsiRuntime, callInvoker));
        
        m_impl->m_graphics->AddToJavaScript(m_impl->env);

        Polyfills::Window::Initialize(m_impl->env);
        // NOTE: React Native's XMLHttpRequest is slow and allocates a lot of memory. This does not override
        // React Native's implementation, but rather adds a second one scoped to Babylon and used by WebRequest.ts.
        Polyfills::XMLHttpRequest::Initialize(m_impl->env);

        Plugins::NativeWindow::Initialize(m_impl->env, windowPtr, width, height);
        Plugins::NativeEngine::Initialize(m_impl->env);
        Plugins::NativeXr::Initialize(m_impl->env);

        m_impl->nativeInput = &Babylon::Plugins::NativeInput::CreateForJavaScript(m_impl->env);
    }

    Native::~Native()
    {
    }

    void Native::Refresh(void* windowPtr, size_t width, size_t height)
    {
        m_impl->m_graphics->ReinitializeFromWindow<void*>(windowPtr, width, height);
        Plugins::NativeWindow::Reinitialize(m_impl->env, windowPtr, width, height);
    }

    void Native::Resize(size_t width, size_t height)
    {
        Plugins::NativeWindow::UpdateSize(m_impl->env, width, height);
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
