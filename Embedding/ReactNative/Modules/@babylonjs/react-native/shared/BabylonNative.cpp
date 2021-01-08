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
    using namespace facebook;

    class ReactNativeModule : public jsi::HostObject
    {
    public:
        ReactNativeModule(jsi::Runtime& jsiRuntime, Dispatcher jsDispatcher)
            : m_env{ Napi::Attach<facebook::jsi::Runtime&>(jsiRuntime) }
            , m_jsDispatcher{ std::move(jsDispatcher) }
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

        void UpdateView(void* windowPtr, size_t width, size_t height)
        {
            // TODO: We shouldn't have to dispatch to the JS thread most of this, but not doing so results in a crash.
            //       I don't understand the issue yet, but for now just retain the pre-refactor logic. We'll need to
            //       resolve this to enable manual non-JS thread rendering. Note this only repros in release builds
            //       where we actually call ResetView.
            m_jsDispatcher([this, windowPtr, width, height]()
            {
                if (!m_graphics)
                {
                    m_graphics = Graphics::CreateGraphics(windowPtr, width, height);
                    m_graphics->AddToJavaScript(m_env);
                    Plugins::NativeEngine::Initialize(m_env, true);
                    m_resolveInitPromise();
                }
                else
                {
                    m_graphics->UpdateWindow(windowPtr);
                    m_graphics->UpdateSize(width, height);
                    m_graphics->EnableRendering();
                }
            });
        }

        void ResetView()
        {
            // TODO: We shouldn't have to dispatch to the JS thread for this since we are already on the JS thread,
            //       but there is an issue in NativeEngine where it will Dispatch a call to RenderCurrentFrame, then
            //       get disposed, then try to actually render the frame. This results in immediately re-enabling
            //       graphics after disabling it here. For now, retain the pre-refactor logic (queueing on the JS thread).
            m_jsDispatcher([this]()
            {
                if (m_graphics)
                {
                    m_graphics->DisableRendering();
                }
            });
        }

        void SetPointerButtonState(uint32_t pointerId, uint32_t buttonId, bool isDown, uint32_t x, uint32_t y)
        {
            if (isDown)
            {
                m_nativeInput->PointerDown(pointerId, buttonId, x, y);
            }
            else
            {
                m_nativeInput->PointerUp(pointerId, buttonId, x, y);
            }
        }

        void SetPointerPosition(uint32_t pointerId, uint32_t x, uint32_t y)
        {
            m_nativeInput->PointerMove(pointerId, x, y);
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

    void Initialize(facebook::jsi::Runtime& jsiRuntime, Dispatcher jsDispatcher)
    {
        if (!jsiRuntime.global().hasProperty(jsiRuntime, JS_INSTANCE_NAME))
        {
            auto nativeModule{ std::make_shared<ReactNativeModule>(jsiRuntime, jsDispatcher) };
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

    void UpdateView(void* windowPtr, size_t width, size_t height)
    {
        if (auto nativeModule{ g_nativeModule.lock() })
        {
            nativeModule->UpdateView(windowPtr, width, height);
        }
        else
        {
            throw std::runtime_error{ "UpdateView must not be called before Initialize." };
        }
    }

    void SetPointerButtonState(uint32_t pointerId, uint32_t buttonId, bool isDown, uint32_t x, uint32_t y)
    {
        if (auto nativeModule{ g_nativeModule.lock() })
        {
            nativeModule->SetPointerButtonState(pointerId, buttonId, isDown, x, y);
        }
    }

    void SetPointerPosition(uint32_t pointerId, uint32_t x, uint32_t y)
    {
        if (auto nativeModule{ g_nativeModule.lock() })
        {
            nativeModule->SetPointerPosition(pointerId, x, y);
        }
    }
}
