#include "BabylonNative.h"

#include <Babylon/Graphics.h>
#include <Babylon/JsRuntime.h>
#include <Babylon/Plugins/NativeEngine.h>
#include <Babylon/Plugins/NativeInput.h>
#include <Babylon/Plugins/NativeXr.h>
#include <Babylon/Polyfills/Window.h>
#include <Babylon/Polyfills/XMLHttpRequest.h>

#include <DispatchFunction.h>

namespace Babylon
{
    using namespace Babylon::Plugins;
    using namespace facebook;

    namespace
    {
        Dispatcher g_inlineDispatcher{ [](const std::function<void()>& func) { func(); } };
    }

    const uint32_t LEFT_MOUSE_BUTTON_ID{ NativeInput::LEFT_MOUSE_BUTTON_ID };
    const uint32_t MIDDLE_MOUSE_BUTTON_ID{ NativeInput::MIDDLE_MOUSE_BUTTON_ID };
    const uint32_t RIGHT_MOUSE_BUTTON_ID{ NativeInput::RIGHT_MOUSE_BUTTON_ID };

    class ReactNativeModule : public jsi::HostObject
    {
    public:
        ReactNativeModule(jsi::Runtime& jsiRuntime, Dispatcher jsDispatcher, bool autoRender)
            : m_env{ Napi::Attach<facebook::jsi::Runtime&>(jsiRuntime) }
            , m_jsDispatcher{ std::move(jsDispatcher) }
            , m_autoRender{ autoRender }
            , m_isRunning{ std::make_shared<bool>(true) }
        {
            // Initialize a JS promise that will be returned by whenInitialized, and completed when NativeEngine is initialized.
            m_initPromise = jsiRuntime.global().getPropertyAsFunction(jsiRuntime, "Promise").callAsConstructor
            (
                jsiRuntime,
                jsi::Function::createFromHostFunction(jsiRuntime, jsi::PropNameID::forAscii(jsiRuntime, "executor"), 0, [this](jsi::Runtime& rt, const jsi::Value&, const jsi::Value* args, size_t) -> jsi::Value
                {
                    m_resolveInitPromise = [&rt, resolve{ std::make_shared<jsi::Value>(rt, args[0]) }]()
                    {
                        resolve->asObject(rt).asFunction(rt).call(rt);
                    };
                    return {};
                })
            );

            // Initialize Babylon Native core components
            JsRuntime::CreateForJavaScript(m_env, CreateJsRuntimeDispatcher(m_env, jsiRuntime, m_jsDispatcher, m_isRunning));

            // Initialize Babylon Native plugins
            Plugins::NativeXr::Initialize(m_env);
            m_nativeInput = &Babylon::Plugins::NativeInput::CreateForJavaScript(m_env);

            // Initialize Babylon Native polyfills
            Polyfills::Window::Initialize(m_env);

            // NOTE: React Native's XMLHttpRequest is slow and allocates a lot of memory. This does not override
            // React Native's implementation, but rather adds a second one scoped to Babylon and used by WebRequest.ts.
            Polyfills::XMLHttpRequest::Initialize(m_env);
        }

        ~ReactNativeModule() override
        {
            *m_isRunning = false;
            Napi::Detach(m_env);
        }

        // NOTE: This only happens when the JS engine is shutting down (other than when the app exits, this only
        //       happens during a dev mode reload). In this case, EngineHook.ts won't call NativeEngine.dispose,
        //       so we need to manually do it here to properly clean up these resources.
        void Deinitialize()
        {
            if (m_disposeEngine)
            {
                m_disposeEngine();
                m_disposeEngine = {};
            }
        }

        void UpdateView(void* windowPtr, void* windowTypePtr, size_t width, size_t height)
        {
            // TODO: We shouldn't have to dispatch to the JS thread for CreateGraphics/UpdateWindow/UpdateSize, but not doing so results in a crash.
            //       I don't understand the issue yet, but for now just retain the pre-refactor logic. We'll need to resolve this to enable manual
            //       non-JS thread rendering. Note this only repros in release builds where we actually call ResetView.

            // When auto rendering is enabled, we render from the JS thread. In this case, we dispatch to the JS thread to initialize/update graphics,
            // and stay on this thread (with an inline dispatcher) to interact with the JS runtime.
            // When auto rendering is disabled, we render from a different thread. In this case, we assume this function was called from the render thread
            // and do an inline dispatch (e.g. execute synchronously on the calling thread), and switch to the JS thread to interact with the JS runtime.
            auto renderDispatcher = m_autoRender ? m_jsDispatcher : g_inlineDispatcher;
            auto jsDispatcher = m_autoRender ? g_inlineDispatcher : m_jsDispatcher;

            renderDispatcher([this, windowPtr, width, height, windowTypePtr, jsDispatcher{ std::move(jsDispatcher) }]()
            {
                if (!m_graphics)
                {
                    m_graphics = Graphics::CreateGraphics(windowPtr, windowTypePtr, width, height);
                    jsDispatcher([this]()
                    {
                        m_graphics->AddToJavaScript(m_env);
                        Plugins::NativeEngine::Initialize(m_env, m_autoRender);
                        m_resolveInitPromise();
                    });
                }
                else
                {
                    m_graphics->UpdateWindow(windowPtr, windowTypePtr);
                    m_graphics->UpdateSize(width, height);
                    m_graphics->EnableRendering();
                }
            });
        }

        void RenderView()
        {
            if (m_autoRender)
            {
                throw std::runtime_error{ "RenderView can only be called when automatic rendering is disabled." };
            }

            m_graphics->RenderCurrentFrame();
        }

        void ResetView()
        {
            // TODO: We shouldn't have to dispatch to the JS thread for this since we are already on the JS thread,
            //       but there is an issue in NativeEngine where it will Dispatch a call to RenderCurrentFrame, then
            //       get disposed, then try to actually render the frame. This results in immediately re-enabling
            //       graphics after disabling it here. For now, retain the pre-refactor logic (queueing on the JS thread).
            // TODO: This is called from JS code (and therefore the JS thread), so we need to figure out a good way
            //       to get on the proper (render) thread to make this call.
            m_jsDispatcher([this]()
            {
                if (m_graphics)
                {
                    m_graphics->DisableRendering();
                }
            });
        }

        void SetMouseButtonState(uint32_t buttonId, bool isDown, uint32_t x, uint32_t y)
        {
            if (isDown)
            {
                m_nativeInput->MouseDown(buttonId, x, y);
            }
            else
            {
                m_nativeInput->MouseUp(buttonId, x, y);
            }
        }

        void SetMousePosition(uint32_t x, uint32_t y)
        {
            m_nativeInput->MouseMove(x, y);
        }

        void SetTouchButtonState(uint32_t pointerId, bool isDown, uint32_t x, uint32_t y)
        {
            if (isDown)
            {
                m_nativeInput->TouchDown(pointerId, x, y);
            }
            else
            {
                m_nativeInput->TouchUp(pointerId, x, y);
            }
        }

        void SetTouchPosition(uint32_t pointerId, uint32_t x, uint32_t y)
        {
            m_nativeInput->TouchMove(pointerId, x, y);
        }

        jsi::Value get(jsi::Runtime& runtime, const jsi::PropNameID& prop) override
        {
            const auto propName{ prop.utf8(runtime) };

            if (propName == "initializationPromise")
            {
                return { runtime, m_initPromise };
            }
            else if (propName == "reset")
            {
                return jsi::Function::createFromHostFunction(runtime, prop, 0, [this](jsi::Runtime& rt, const jsi::Value&, const jsi::Value*, size_t) -> jsi::Value
                {
                    this->ResetView();
                    return {};
                });
            }
            else if (propName == "setEngineInstance")
            {
                return jsi::Function::createFromHostFunction(runtime, prop, 0, [this](jsi::Runtime& rt, const jsi::Value&, const jsi::Value* args, size_t count) -> jsi::Value
                {
                    if (count == 0 || !args[0].isObject())
                    {
                        m_disposeEngine = {};
                    }
                    else
                    {
                        m_disposeEngine = [&rt, engineInstanceValue{ std::make_shared<jsi::Value>(rt, args[0]) }]()
                        {
                            auto engineInstance{ engineInstanceValue->getObject(rt) };
                            engineInstance.getPropertyAsFunction(rt, "dispose").callWithThis(rt, engineInstance);
                        };
                    }
                    return {};
                });
            }

            return {};
        }

    private:
        jsi::Value m_initPromise{};
        std::function<void()> m_resolveInitPromise{};

        Napi::Env m_env;
        Dispatcher m_jsDispatcher{};
        bool m_autoRender{};

        std::shared_ptr<bool> m_isRunning{};
        std::unique_ptr<Graphics> m_graphics{};
        Plugins::NativeInput* m_nativeInput{};

        std::function<void()> m_disposeEngine{};
    };

    namespace
    {
        constexpr auto JS_INSTANCE_NAME{ "BabylonNative" };
        std::weak_ptr<ReactNativeModule> g_nativeModule{};
    }

    void Initialize(facebook::jsi::Runtime& jsiRuntime, Dispatcher jsDispatcher, bool autoRender)
    {
        if (!jsiRuntime.global().hasProperty(jsiRuntime, JS_INSTANCE_NAME))
        {
            auto nativeModule{ std::make_shared<ReactNativeModule>(jsiRuntime, jsDispatcher, autoRender) };
            jsiRuntime.global().setProperty(jsiRuntime, JS_INSTANCE_NAME, jsi::Object::createFromHostObject(jsiRuntime, nativeModule));
            g_nativeModule = nativeModule;
        }
    }

    void Deinitialize()
    {
        if (auto nativeModule{ g_nativeModule.lock() })
        {
            nativeModule->Deinitialize();
        }
    }

    void UpdateView(void* windowPtr, size_t width, size_t height, void* windowTypePtr)
    {
        if (auto nativeModule{ g_nativeModule.lock() })
        {
            nativeModule->UpdateView(windowPtr, windowTypePtr, width, height);
        }
        else
        {
            throw std::runtime_error{ "UpdateView must not be called before Initialize." };
        }
    }

    void RenderView()
    {
        if (auto nativeModule{ g_nativeModule.lock() })
        {
            nativeModule->RenderView();
        }
        else
        {
            throw std::runtime_error{ "RenderView must not be called before Initialize." };
        }
    }

    void SetMouseButtonState(uint32_t buttonId, bool isDown, uint32_t x, uint32_t y)
    {
        if (auto nativeModule{ g_nativeModule.lock() })
        {
            nativeModule->SetMouseButtonState(buttonId, isDown, x, y);
        }
    }

    void SetMousePosition(uint32_t x, uint32_t y)
    {
        if (auto nativeModule{ g_nativeModule.lock() })
        {
            nativeModule->SetMousePosition(x, y);
        }
    }

    void SetTouchButtonState(uint32_t pointerId, bool isDown, uint32_t x, uint32_t y)
    {
        if (auto nativeModule{ g_nativeModule.lock() })
        {
            nativeModule->SetTouchButtonState(pointerId, isDown, x, y);
        }
    }

    void SetTouchPosition(uint32_t pointerId, uint32_t x, uint32_t y)
    {
        if (auto nativeModule{ g_nativeModule.lock() })
        {
            nativeModule->SetTouchPosition(pointerId, x, y);
        }
    }
}
