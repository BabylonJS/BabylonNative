#include "BabylonNative.h"

#include <Babylon/Graphics/Device.h>
#include <Babylon/JsRuntime.h>
#include <Babylon/Plugins/NativeCamera.h>
#include <Babylon/Plugins/NativeCapture.h>
#include <Babylon/Plugins/NativeEngine.h>
#include <Babylon/Plugins/NativeInput.h>
#include <Babylon/Plugins/NativeOptimizations.h>
#include <Babylon/Plugins/NativeTracing.h>
#include <Babylon/Plugins/NativeXr.h>
#include <Babylon/Polyfills/Window.h>
#include <Babylon/Polyfills/XMLHttpRequest.h>
#include <Babylon/Polyfills/Canvas.h>

#include <DispatchFunction.h>

namespace BabylonNative
{
    using namespace Babylon::Plugins;
    using namespace facebook;

    namespace
    {
        Dispatcher g_inlineDispatcher{ [](const std::function<void()>& func) { func(); } };
        std::unique_ptr<Babylon::Graphics::Device> g_graphics{};
        std::unique_ptr<Babylon::Graphics::DeviceUpdate> g_update{};
        std::unique_ptr<Babylon::Polyfills::Canvas> g_nativeCanvas{};
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
            Babylon::JsRuntime::CreateForJavaScript(m_env, Babylon::CreateJsRuntimeDispatcher(m_env, jsiRuntime, m_jsDispatcher, m_isRunning));

            // Initialize Babylon Native plugins
            m_nativeXr.emplace(Babylon::Plugins::NativeXr::Initialize(m_env));
            m_nativeXr->SetSessionStateChangedCallback([isXRActive{ m_isXRActive }](bool isSessionActive) { *isXRActive = isSessionActive; });
            Babylon::Plugins::NativeCapture::Initialize(m_env);
            m_nativeInput = &Babylon::Plugins::NativeInput::CreateForJavaScript(m_env);
            Babylon::Plugins::NativeOptimizations::Initialize(m_env);
            Babylon::Plugins::NativeTracing::Initialize(m_env);
            Babylon::Plugins::Camera::Initialize(m_env);

            // Initialize Babylon Native polyfills
            Babylon::Polyfills::Window::Initialize(m_env);

            // NOTE: React Native's XMLHttpRequest is slow and allocates a lot of memory. This does not override
            // React Native's implementation, but rather adds a second one scoped to Babylon and used by WebRequest.ts.
            Babylon::Polyfills::XMLHttpRequest::Initialize(m_env);

            // Initialize Canvas polyfill for text support
            g_nativeCanvas = std::make_unique<Babylon::Polyfills::Canvas>(Babylon::Polyfills::Canvas::Initialize(m_env));
        }

        ~ReactNativeModule() override
        {
            *m_isRunning = false;
            Napi::Detach(m_env);
        }

        void UpdateView(WindowType window, size_t width, size_t height)
        {
            m_windowConfig.Window = window;
            m_windowConfig.Width = width;
            m_windowConfig.Height = height;
            UpdateGraphicsConfiguration();
        }

        void UpdateGraphicsConfiguration()
        {
            if (!g_graphics)
            {
                g_graphics = Babylon::Graphics::Device::Create(m_windowConfig);
                g_update = std::make_unique<Babylon::Graphics::DeviceUpdate>(g_graphics->GetUpdate("update"));
            }
            else
            {
                g_graphics->UpdateWindow(m_windowConfig);
                g_graphics->UpdateSize(m_windowConfig.Width, m_windowConfig.Height);
            }
            g_graphics->UpdateMSAA(mMSAAValue);
            g_graphics->UpdateAlphaPremultiplied(mAlphaPremultiplied);

            g_graphics->EnableRendering();

            std::call_once(m_isGraphicsInitialized, [this]()
            {
                m_jsDispatcher([this]()
                {
                    g_graphics->AddToJavaScript(m_env);
                    Babylon::Plugins::NativeEngine::Initialize(m_env);
                });
            });

            if (!m_isRenderingEnabled)
            {
                m_jsDispatcher([this]()
                {
                    m_resolveInitPromise();
                });
            }
            m_isRenderingEnabled = true;
            m_pendingReset = false;
        }

        void UpdateMSAA(uint8_t value)
        {
            mMSAAValue = value;
            if (g_graphics)
            {
                g_graphics->UpdateMSAA(value);
            }
        }

        void UpdateAlphaPremultiplied(bool enabled)
        {
            mAlphaPremultiplied = enabled;
            if (g_graphics)
            {
                g_graphics->UpdateAlphaPremultiplied(enabled);
            }
        }

        void RenderView()
        {
            // m_pendingReset becomes true when a resetView call has been performed and no UpdateView has been performed.
            // This happens with a fast refresh when the view is not unmounted and it's still available for rendering.
            // UpdateView will set back m_pendingReset to false in case the view is unmounted/mounted with Engine.Dispose for example.
            // With fast refresh, we have to do that work on the UI/render thread, and UpdateView is not called in that case, 
            // so RenderView is pretty much the only option. Specifically, it must be done on the UI/render thread and so RenderView is the only hook we have.
            if (m_pendingReset)
            {
                UpdateGraphicsConfiguration();
            }
            // If rendering has not been explicitly enabled, or has been explicitly disabled, then don't try to render.
            // Otherwise rendering can be implicitly enabled, which may not be desirable (e.g. after the engine is disposed).
            if (g_graphics && m_isRenderingEnabled)
            {
                g_graphics->StartRenderingCurrentFrame();
                g_update->Start();
                g_update->Finish();
                g_graphics->FinishRenderingCurrentFrame();
            }
        }

        void ResetView()
        {
            if (g_graphics)
            {
                g_nativeCanvas->FlushGraphicResources();
                g_graphics->DisableRendering();
                m_pendingReset = true;
            }

            m_isRenderingEnabled = false;
        }

        void SetMouseButtonState(uint32_t buttonId, bool isDown, int32_t x, int32_t y)
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

        void SetMousePosition(int32_t x, int32_t y)
        {
            m_nativeInput->MouseMove(x, y);
        }

        void SetTouchButtonState(uint32_t pointerId, bool isDown, int32_t x, int32_t y)
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

        void SetTouchPosition(uint32_t pointerId, int32_t x, int32_t y)
        {
            m_nativeInput->TouchMove(pointerId, x, y);
        }

        bool IsXRActive()
        {
            return *m_isXRActive;
        }

#if defined(__APPLE__) || defined(ANDROID)
        void UpdateXRView(WindowType window)
        {
            m_nativeXr->UpdateWindow(window);
        }
#endif

        jsi::Value get(jsi::Runtime& runtime, const jsi::PropNameID& prop) override
        {
            const auto propName{ prop.utf8(runtime) };

            if (propName == "initializationPromise")
            {
                return { runtime, m_initPromise };
            }
            else if (propName == "resetInitializationPromise")
            {
                return jsi::Function::createFromHostFunction(runtime, jsi::PropNameID::forAscii(runtime, "executor"), 0, [this](jsi::Runtime& rt, const jsi::Value&, const jsi::Value* args, size_t) -> jsi::Value
                {
                    CreateInitPromise();
                    return {};
                });
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
        bool m_isRenderingEnabled{};
        std::once_flag m_isGraphicsInitialized{};
        Babylon::Plugins::NativeInput* m_nativeInput{};
        std::optional<Babylon::Plugins::NativeXr> m_nativeXr{};

        Babylon::Graphics::WindowConfiguration m_windowConfig{};
        bool m_pendingReset{};

        std::shared_ptr<bool> m_isXRActive{};
        uint8_t mMSAAValue{};
        bool mAlphaPremultiplied{};
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

    void UpdateView(WindowType window, size_t width, size_t height)
    {
        if (auto nativeModule{ g_nativeModule.lock() })
        {
            nativeModule->UpdateView(window, width, height);
        }
        else
        {
            throw std::runtime_error{ "UpdateView must not be called before Initialize." };
        }
    }

    void UpdateMSAA(uint8_t value)
    {
        if (auto nativeModule{ g_nativeModule.lock() })
        {
            nativeModule->UpdateMSAA(value);
        }
        else
        {
            throw std::runtime_error{ "UpdateMSAA must not be called before Initialize." };
        }
    }

    void UpdateAlphaPremultiplied(bool enabled)
    {
        if (auto nativeModule{ g_nativeModule.lock() })
        {
            nativeModule->UpdateAlphaPremultiplied(enabled);
        }
        else
        {
            throw std::runtime_error{ "UpdateAlphaPremultiplied must not be called before Initialize." };
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

    void SetMouseButtonState(uint32_t buttonId, bool isDown, int32_t x, int32_t y)
    {
        if (auto nativeModule{ g_nativeModule.lock() })
        {
            nativeModule->SetMouseButtonState(buttonId, isDown, x, y);
        }
    }

    void SetMousePosition(int32_t x, int32_t y)
    {
        if (auto nativeModule{ g_nativeModule.lock() })
        {
            nativeModule->SetMousePosition(x, y);
        }
    }

    void SetTouchButtonState(uint32_t pointerId, bool isDown, int32_t x, int32_t y)
    {
        if (auto nativeModule{ g_nativeModule.lock() })
        {
            nativeModule->SetTouchButtonState(pointerId, isDown, x, y);
        }
    }

    void SetTouchPosition(uint32_t pointerId, int32_t x, int32_t y)
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

#if defined(__APPLE__) || defined(ANDROID)
    void UpdateXRView(WindowType window)
    {
        if (auto nativeModule{ g_nativeModule.lock() })
        {
            nativeModule->UpdateXRView(window);
        }
    }
#endif
}
