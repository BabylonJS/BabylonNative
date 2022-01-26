#include "App.h"

#include <Babylon/Graphics.h>
#include <Babylon/ScriptLoader.h>
#include <Babylon/Plugins/NativeEngine.h>
#include <Babylon/Plugins/NativeOptimizations.h>
#include <Babylon/Plugins/NativeXr.h>
#include <Babylon/Polyfills/Console.h>
#include <Babylon/Polyfills/Window.h>
#include <Babylon/Polyfills/XMLHttpRequest.h>
#include <Babylon/Polyfills/Canvas.h>

#include <pplawait.h>
#include <winrt/Windows.ApplicationModel.h>

#include <winrt/windows.ui.core.h>

using namespace Windows::ApplicationModel;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::ApplicationModel::Activation;
using namespace Windows::UI::Core;
using namespace Windows::UI::Input;
using namespace Windows::System;
using namespace Windows::Foundation;
using namespace Windows::Graphics::Display;

// The main function is only used to initialize our IFrameworkView class.
[Platform::MTAThread]
int main(Platform::Array<Platform::String^>^)
{
    auto direct3DApplicationSource = ref new Direct3DApplicationSource();
    CoreApplication::Run(direct3DApplicationSource);
    return 0;
}

IFrameworkView^ Direct3DApplicationSource::CreateView()
{
    return ref new App();
}

App::App() :
    m_windowClosed{ false },
    m_windowVisible{ true }
{
}

// The first method called when the IFrameworkView is being created.
void App::Initialize(CoreApplicationView^ applicationView)
{
    // Register event handlers for app lifecycle. This example includes Activated, so that we
    // can make the CoreWindow active and start rendering on the window.
    applicationView->Activated +=
        ref new TypedEventHandler<CoreApplicationView^, IActivatedEventArgs^>(this, &App::OnActivated);

    CoreApplication::Suspending +=
        ref new EventHandler<SuspendingEventArgs^>(this, &App::OnSuspending);

    CoreApplication::Resuming +=
        ref new EventHandler<Platform::Object^>(this, &App::OnResuming);
}

// Called when the CoreWindow object is created (or re-created).
void App::SetWindow(CoreWindow^ window)
{
    window->SizeChanged +=
        ref new TypedEventHandler<CoreWindow^, WindowSizeChangedEventArgs^>(this, &App::OnWindowSizeChanged);

    window->VisibilityChanged +=
        ref new TypedEventHandler<CoreWindow^, VisibilityChangedEventArgs^>(this, &App::OnVisibilityChanged);

    window->Closed +=
        ref new TypedEventHandler<CoreWindow^, CoreWindowEventArgs^>(this, &App::OnWindowClosed);

    DisplayInformation^ currentDisplayInformation = DisplayInformation::GetForCurrentView();

    currentDisplayInformation->DpiChanged +=
        ref new TypedEventHandler<DisplayInformation^, Object^>(this, &App::OnDpiChanged);

    currentDisplayInformation->OrientationChanged +=
        ref new TypedEventHandler<DisplayInformation^, Object^>(this, &App::OnOrientationChanged);

    DisplayInformation::DisplayContentsInvalidated +=
        ref new TypedEventHandler<DisplayInformation^, Object^>(this, &App::OnDisplayContentsInvalidated);

    window->PointerMoved +=
        ref new TypedEventHandler<CoreWindow^, PointerEventArgs^>(this, &App::OnPointerMoved);

    window->PointerPressed +=
        ref new TypedEventHandler<CoreWindow^, PointerEventArgs^>(this, &App::OnPointerPressed);

    window->PointerReleased +=
        ref new TypedEventHandler<CoreWindow^, PointerEventArgs^>(this, &App::OnPointerReleased);

    window->KeyDown +=
        ref new TypedEventHandler<CoreWindow^, KeyEventArgs^>(this, &App::OnKeyPressed);
}

// Initializes scene resources, or loads a previously saved app state.
void App::Load(Platform::String^ entryPoint)
{
}

// This method is called after the window becomes active.
void App::Run()
{
    while (!m_windowClosed)
    {
        if (m_graphics)
        {
            m_update->Finish();
            m_graphics->FinishRenderingCurrentFrame();
            m_graphics->StartRenderingCurrentFrame();
            m_update->Start();
        }

        CoreWindow::GetForCurrentThread()->Dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessAllIfPresent);
    }
}

// Required for IFrameworkView.
// Terminate events do not cause Uninitialize to be called. It will be called if your IFrameworkView
// class is torn down while the app is in the foreground.
void App::Uninitialize()
{
    if (m_graphics)
    {
        m_update->Finish();
        m_graphics->FinishRenderingCurrentFrame();
    }

    m_chromeDevTools.reset();
    m_nativeInput = {};
    m_runtime.reset();
    m_graphics.reset();
}

// Application lifecycle event handlers.

void App::OnActivated(CoreApplicationView^ applicationView, IActivatedEventArgs^ args)
{
    // Run() won't start until the CoreWindow is activated.
    CoreWindow::GetForCurrentThread()->Activate();

    if (args->Kind == Activation::ActivationKind::File)
    {
        m_files = static_cast<FileActivatedEventArgs^>(args)->Files;
    }
    else
    {
        m_files = nullptr;
    }

    RestartRuntime(applicationView->CoreWindow->Bounds);
}

void App::OnSuspending(Platform::Object^ sender, SuspendingEventArgs^ args)
{
    // Save app state after requesting a deferral. Holding a deferral
    // indicates that the application is busy performing suspending operations. Be
    // aware that a deferral may not be held indefinitely. After about five seconds,
    // the app will be forced to exit.
    auto deferral = args->SuspendingOperation->GetDeferral();

    if (m_graphics)
    {
        m_update->Finish();
        m_graphics->FinishRenderingCurrentFrame();
    }

    m_runtime->Suspend();

    deferral->Complete();
}

void App::OnResuming(Platform::Object^ sender, Platform::Object^ args)
{
    // Restore any data or state that was unloaded on suspend. By default, data
    // and state are persisted when resuming from suspend. Note that this event
    // does not occur if the app was previously terminated.
    m_runtime->Resume();

    if (m_graphics)
    {
        m_graphics->StartRenderingCurrentFrame();
        m_update->Start();
    }
}

// Window event handlers.

void App::OnWindowSizeChanged(CoreWindow^ /*sender*/, WindowSizeChangedEventArgs^ args)
{
    size_t width = static_cast<size_t>(args->Size.Width * m_displayScale);
    size_t height = static_cast<size_t>(args->Size.Height * m_displayScale);
    m_graphics->UpdateSize(width, height);
}

void App::OnVisibilityChanged(CoreWindow^ sender, VisibilityChangedEventArgs^ args)
{
    m_windowVisible = args->Visible;
}

void App::OnWindowClosed(CoreWindow^ sender, CoreWindowEventArgs^ args)
{
    m_windowClosed = true;
    Uninitialize();
}

void App::OnPointerMoved(CoreWindow^, PointerEventArgs^ args)
{
    if (m_nativeInput != nullptr)
    {
        const auto& point = args->CurrentPoint->RawPosition;
        m_nativeInput->MouseMove(static_cast<int>(point.X), static_cast<int>(point.Y));
    }
}

void App::OnPointerPressed(CoreWindow^, PointerEventArgs^ args)
{
    if (m_nativeInput != nullptr)
    {
        const auto& point = args->CurrentPoint->RawPosition;
        m_nativeInput->MouseDown(0, static_cast<int>(point.X), static_cast<int>(point.Y));
    }
}

void App::OnPointerReleased(CoreWindow^, PointerEventArgs^ args)
{
    if (m_nativeInput != nullptr)
    {
        const auto& point = args->CurrentPoint->RawPosition;
        m_nativeInput->MouseUp(0, static_cast<int>(point.X), static_cast<int>(point.Y));
    }
}

void App::OnKeyPressed(CoreWindow^ window, KeyEventArgs^ args)
{
    if (args->VirtualKey == VirtualKey::R)
    {
        RestartRuntime(window->Bounds);
    }
}

// DisplayInformation event handlers.

void App::OnDpiChanged(DisplayInformation^ /*sender*/, Object^ /*args*/)
{
    DisplayInformation^ displayInformation = DisplayInformation::GetForCurrentView();
    m_displayScale = static_cast<float>(displayInformation->RawPixelsPerViewPixel);
    // resize event happens after. No need to force resize here.
}

void App::OnOrientationChanged(DisplayInformation^ sender, Object^ args)
{
    // TODO: Implement.
    //m_deviceResources->SetCurrentOrientation(sender->CurrentOrientation);
}

void App::OnDisplayContentsInvalidated(DisplayInformation^ sender, Object^ args)
{
    // TODO: Implement.
    //m_deviceResources->ValidateDevice();
}

void App::RestartRuntime(Windows::Foundation::Rect bounds)
{
    Uninitialize();

    DisplayInformation^ displayInformation = DisplayInformation::GetForCurrentView();
    m_displayScale = static_cast<float>(displayInformation->RawPixelsPerViewPixel);
    size_t width = static_cast<size_t>(bounds.Width * m_displayScale);
    size_t height = static_cast<size_t>(bounds.Height * m_displayScale);
    auto* window = reinterpret_cast<winrt::Windows::UI::Core::ICoreWindow*>(CoreWindow::GetForCurrentThread());

    Babylon::WindowConfiguration graphicsConfig{};
    graphicsConfig.Window = window;
    graphicsConfig.Width = width;
    graphicsConfig.Height = height;
    m_graphics = Babylon::Graphics::CreateGraphics(graphicsConfig);
    m_update = std::make_unique<Babylon::Graphics::Update>(m_graphics->GetUpdate("update"));
    m_graphics->StartRenderingCurrentFrame();
    m_update->Start();

    m_runtime = std::make_unique<Babylon::AppRuntime>();

    m_runtime->Dispatch([this](Napi::Env env) {
        m_graphics->AddToJavaScript(env);

        Babylon::Polyfills::Console::Initialize(env, [](const char* message, auto) {
            OutputDebugStringA(message);
        });

        Babylon::Polyfills::Window::Initialize(env);

        Babylon::Polyfills::XMLHttpRequest::Initialize(env);

        Babylon::Plugins::NativeEngine::Initialize(env);

        Babylon::Plugins::NativeOptimizations::Initialize(env);

        m_nativeCanvas = std::make_unique <Babylon::Polyfills::Canvas>(Babylon::Polyfills::Canvas::Initialize(env));

        Babylon::Plugins::NativeXr::Initialize(env);

        m_nativeInput = &Babylon::Plugins::NativeInput::CreateForJavaScript(env);

        m_chromeDevTools = std::make_unique<Babylon::Plugins::ChromeDevTools>(Babylon::Plugins::ChromeDevTools::Initialize(env));
        if (m_chromeDevTools->SupportsInspector())
        {
            m_chromeDevTools->StartInspector(5643, "BabylonNative Playground");
        }
    });

    Babylon::ScriptLoader loader{*m_runtime};
    loader.Eval("document = {}", "");
    loader.LoadScript("app:///Scripts/ammo.js");
    // Commenting out recast.js for now as v8jsi is incompatible with asm.js.
    // loader.LoadScript("app:///Scripts/recast.js");
    loader.LoadScript("app:///Scripts/babylon.max.js");
    loader.LoadScript("app:///Scripts/babylonjs.loaders.js");
    loader.LoadScript("app:///Scripts/babylonjs.materials.js");
    loader.LoadScript("app:///Scripts/babylon.gui.js");

    if (m_files == nullptr)
    {
        loader.LoadScript("app:///Scripts/experience.js");
    }
    else
    {
        for (unsigned int idx = 0; idx < m_files->Size; idx++)
        {
            auto file{static_cast<Windows::Storage::IStorageFile^>(m_files->GetAt(idx))};

            // There is no built-in way to convert a local file path to a url in UWP, but
            // Foundation::Uri works with a url constructed using "file:///" with a local path.
            loader.LoadScript("file:///" + winrt::to_string(file->Path->Data()));
        }

        loader.LoadScript("app:///Scripts/playground_runner.js");
    }
}
