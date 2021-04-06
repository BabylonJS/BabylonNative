#include "BabylonNative.h"

#include <Babylon/Graphics.h>
#include <Babylon/JsRuntime.h>
#include <Babylon/Plugins/NativeCapture.h>
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
        std::unique_ptr<Graphics> g_graphics{};
    }

    const uint32_t LEFT_MOUSE_BUTTON_ID{ NativeInput::LEFT_MOUSE_BUTTON_ID };
    const uint32_t MIDDLE_MOUSE_BUTTON_ID{ NativeInput::MIDDLE_MOUSE_BUTTON_ID };
    const uint32_t RIGHT_MOUSE_BUTTON_ID{ NativeInput::RIGHT_MOUSE_BUTTON_ID };

    class ReactNativeModule : public jsi::HostObject
    {
    public:
        ReactNativeModule(jsi::Runtime& jsiRuntime, Dispatcher jsDispatcher)
            : m_env{ Napi::Attach<facebook::jsi::Runtime&>(jsiRuntime) }
            , m_jsDispatcher{ std::move(jsDispatcher) }
            , m_isRunning{ std::make_shared<bool>(true) }
            , m_isXRActive{ std::make_shared<bool>(false) }
        {
            // Initialize a JS promise that will be returned by whenInitialized, and completed when NativeEngine is initialized.
            CreateInitPromise();

            // Initialize Babylon Native core components
            JsRuntime::CreateForJavaScript(m_env, CreateJsRuntimeDispatcher(m_env, jsiRuntime, m_jsDispatcher, m_isRunning));

            // Initialize Babylon Native plugins
            m_nativeXr.emplace(Plugins::NativeXr::Initialize(m_env));
            m_nativeXr->SetSessionStateChangedCallback([isXRActive{ m_isXRActive }](bool isSessionActive) { *isXRActive = isSessionActive; });
            Plugins::NativeCapture::Initialize(m_env);
            m_nativeInput = &Plugins::NativeInput::CreateForJavaScript(m_env);

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

        void UpdateView(void* windowPtr, size_t width, size_t height)
        {
            GraphicsConfiguration graphicsConfig{};
            graphicsConfig.WindowPtr = reinterpret_cast<WindowType>(windowPtr);
            graphicsConfig.Width = width;
            graphicsConfig.Height = height;

            if (!g_graphics)
            {
                g_graphics = Graphics::CreateGraphics(graphicsConfig);
            }
            else
            {
                g_graphics->UpdateWindow(graphicsConfig);
                g_graphics->UpdateSize(width, height);
            }

            g_graphics->EnableRendering();

            std::call_once(m_isGraphicsInitialized, [this]()
            {
                m_jsDispatcher([this]()
                {
                    g_graphics->AddToJavaScript(m_env);
                    Plugins::NativeEngine::Initialize(m_env);
                });
            });

            m_jsDispatcher([this]()
            {
                m_resolveInitPromise();
            });
        }

        void RenderView()
        {
            if (g_graphics)
            {
                g_graphics->StartRenderingCurrentFrame();
                g_graphics->FinishRenderingCurrentFrame();
            }
        }

        void ResetView()
        {
            if (g_graphics)
            {
                g_graphics->DisableRendering();

                m_jsDispatcher([this]()
                {
                    CreateInitPromise();
                });
            }
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

        bool IsXRActive()
        {
            return *m_isXRActive;
        }

        void UpdateXRView(void* windowPtr)
        {
            m_nativeXr->UpdateWindow(windowPtr);
        }

        jsi::Value get(jsi::Runtime& runtime, const jsi::PropNameID& prop) override
        {
            const auto propName{ prop.utf8(runtime) };

            if (propName == "initializationPromise")
            {
                return { runtime, m_initPromise };
            }

            return {};
        }

    private:
        void CreateInitPromise()
        {
            jsi::Runtime& jsiRuntime{static_cast<napi_env>(m_env)->rt};
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
        }

        jsi::Value m_initPromise{};
        std::function<void()> m_resolveInitPromise{};

        Napi::Env m_env;
        Dispatcher m_jsDispatcher{};

        std::shared_ptr<bool> m_isRunning{};
        std::once_flag m_isGraphicsInitialized{};
        Plugins::NativeInput* m_nativeInput{};
        std::optional<Plugins::NativeXr> m_nativeXr{};

        std::shared_ptr<bool> m_isXRActive{};
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
        // Prevent further interactions with the native module from the native interface.
        g_nativeModule.reset();
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

    void RenderView()
    {
        if (auto nativeModule{ g_nativeModule.lock() })
        {
            nativeModule->RenderView();
        }
    }

    void ResetView()
    {
        if (auto nativeModule{ g_nativeModule.lock() })
        {
            nativeModule->ResetView();
        }
        else
        {
            throw std::runtime_error{ "ResetView must not be called before Initialize." };
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

    bool IsXRActive()
    {
        if (auto nativeModule{ g_nativeModule.lock() })
        {
            return nativeModule->IsXRActive();
        }

        return false;
    }

    void UpdateXRView(void* windowPtr)
    {
        if (auto nativeModule{ g_nativeModule.lock() })
        {
            nativeModule->UpdateXRView(windowPtr);
        }
    }
}
