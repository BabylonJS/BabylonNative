// App.cpp : Defines the entry point for the application.
//
// Built on Babylon::Integrations: the cross-platform Runtime + View API
// handles plugin/polyfill setup, GPU device construction, frame rendering,
// and input forwarding.

#include "App.h"

#include <Shared/PlaygroundScripts.h>

#include <iostream>
#include <string>
#include <string_view>

using namespace winrt::Windows::ApplicationModel;
using namespace winrt::Windows::ApplicationModel::Core;
using namespace winrt::Windows::ApplicationModel::Activation;
using namespace winrt::Windows::UI::Core;
using namespace winrt::Windows::UI::Input;
using namespace winrt::Windows::System;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Graphics::Display;

using Babylon::Integrations::CoordinateUnits;
using BNView = Babylon::Integrations::View;

// The main function is only used to initialize our IFrameworkView class.
int __stdcall wWinMain(HINSTANCE, HINSTANCE, PWSTR, int)
{
    winrt::init_apartment();
    CoreApplication::Run(winrt::make<Direct3DApplicationSource>());
    return 0;
}

namespace
{
    void ProcessMouseButtons(BNView& view, PointerUpdateKind updateKind, float x, float y)
    {
        switch (updateKind)
        {
            case PointerUpdateKind::LeftButtonPressed:
                view.OnMouseDown(BNView::LeftMouseButton(), x, y, CoordinateUnits::Logical);
                break;
            case PointerUpdateKind::MiddleButtonPressed:
                view.OnMouseDown(BNView::MiddleMouseButton(), x, y, CoordinateUnits::Logical);
                break;
            case PointerUpdateKind::RightButtonPressed:
                view.OnMouseDown(BNView::RightMouseButton(), x, y, CoordinateUnits::Logical);
                break;
            case PointerUpdateKind::LeftButtonReleased:
                view.OnMouseUp(BNView::LeftMouseButton(), x, y, CoordinateUnits::Logical);
                break;
            case PointerUpdateKind::MiddleButtonReleased:
                view.OnMouseUp(BNView::MiddleMouseButton(), x, y, CoordinateUnits::Logical);
                break;
            case PointerUpdateKind::RightButtonReleased:
                view.OnMouseUp(BNView::RightMouseButton(), x, y, CoordinateUnits::Logical);
                break;
            default:
                break;
        }
    }
}

IFrameworkView Direct3DApplicationSource::CreateView()
{
    return winrt::make<App>();
}

App::App() :
    m_windowClosed{false},
    m_windowVisible{true}
{
}

// The first method called when the IFrameworkView is being created.
void App::Initialize(CoreApplicationView const& applicationView)
{
    // Register event handlers for app lifecycle. This example includes Activated, so that we
    // can make the CoreWindow active and start rendering on the window.
    applicationView.Activated({this, &App::OnActivated});

    CoreApplication::Suspending({this, &App::OnSuspending});

    CoreApplication::Resuming({this, &App::OnResuming});
}

// Called when the CoreWindow object is created (or re-created).
void App::SetWindow(CoreWindow const& window)
{
    window.SizeChanged({this, &App::OnWindowSizeChanged});

    window.VisibilityChanged({this, &App::OnVisibilityChanged});

    window.Closed({this, &App::OnWindowClosed});

    DisplayInformation currentDisplayInformation = DisplayInformation::GetForCurrentView();

    currentDisplayInformation.DpiChanged({this, &App::OnDpiChanged});

    currentDisplayInformation.OrientationChanged({this, &App::OnOrientationChanged});

    DisplayInformation::DisplayContentsInvalidated({this, &App::OnDisplayContentsInvalidated});

    window.PointerMoved({this, &App::OnPointerMoved});

    window.PointerPressed({this, &App::OnPointerPressed});

    window.PointerReleased({this, &App::OnPointerReleased});

    window.PointerWheelChanged({this, &App::OnPointerWheelChanged});

    window.KeyDown({this, &App::OnKeyPressed});
}

// Initializes scene resources, or loads a previously saved app state.
void App::Load(winrt::hstring const& /*entryPoint*/)
{
}

// This method is called after the window becomes active.
void App::Run()
{
    while (!m_windowClosed)
    {
        if (m_view)
        {
            m_view->RenderFrame();
        }

        CoreWindow::GetForCurrentThread().Dispatcher().ProcessEvents(CoreProcessEventsOption::ProcessAllIfPresent);
    }
}

// Required for IFrameworkView.
// Terminate events do not cause Uninitialize to be called. It will be called if your IFrameworkView
// class is torn down while the app is in the foreground.
void App::Uninitialize()
{
    // View first (unbinds surface, closes in-flight frame), then Runtime
    // (joins JS thread).
    m_view.reset();
    m_runtime.reset();
}

// Application lifecycle event handlers.

void App::OnActivated(CoreApplicationView const& applicationView, IActivatedEventArgs const& args)
{
    // Run() won't start until the CoreWindow is activated.
    CoreWindow::GetForCurrentThread().Activate();

    if (args.Kind() == ActivationKind::File)
    {
        m_files = args.as<FileActivatedEventArgs>().Files();
    }
    else
    {
        m_files = nullptr;
    }

    RestartRuntime(applicationView.CoreWindow().Bounds());
}

void App::OnSuspending(IInspectable const& /*sender*/, SuspendingEventArgs const& args)
{
    // Save app state after requesting a deferral. Holding a deferral
    // indicates that the application is busy performing suspending operations. Be
    // aware that a deferral may not be held indefinitely. After about five seconds,
    // the app will be forced to exit.
    auto deferral = args.SuspendingOperation().GetDeferral();

    if (m_runtime)
    {
        // Closes any in-flight frame on the attached View internally.
        m_runtime->Suspend();
    }

    deferral.Complete();
}

void App::OnResuming(IInspectable const& /*sender*/, IInspectable const& /*args*/)
{
    if (m_runtime)
    {
        // Re-opens the frame on the attached View internally.
        m_runtime->Resume();
    }
}

// Window event handlers.

void App::OnWindowSizeChanged(CoreWindow const& /*sender*/, WindowSizeChangedEventArgs const& args)
{
    if (m_view)
    {
        m_view->Resize(static_cast<uint32_t>(args.Size().Width),
                       static_cast<uint32_t>(args.Size().Height),
                       CoordinateUnits::Logical);
    }
}

void App::OnVisibilityChanged(CoreWindow const& /*sender*/, VisibilityChangedEventArgs const& args)
{
    m_windowVisible = args.Visible();
}

void App::OnWindowClosed(CoreWindow const& /*sender*/, CoreWindowEventArgs const& /*args*/)
{
    m_windowClosed = true;
    Uninitialize();
}

void App::OnPointerMoved(CoreWindow const&, PointerEventArgs const& args)
{
    if (!m_view) return;

    const auto position = args.CurrentPoint().RawPosition();
    const auto deviceType = args.CurrentPoint().PointerDevice().PointerDeviceType();
    const auto deviceSlot = args.CurrentPoint().PointerId();
    const auto updateKind = args.CurrentPoint().Properties().PointerUpdateKind();
    const auto x = position.X;
    const auto y = position.Y;

    if (deviceType == winrt::Windows::Devices::Input::PointerDeviceType::Mouse)
    {
        m_view->OnMouseMove(x, y, CoordinateUnits::Logical);

        if (args.CurrentPoint().IsInContact())
        {
            ProcessMouseButtons(*m_view, updateKind, x, y);
        }
    }
    else
    {
        m_view->OnPointerMove(static_cast<int32_t>(deviceSlot), x, y, CoordinateUnits::Logical);
    }
}

void App::OnPointerPressed(CoreWindow const&, PointerEventArgs const& args)
{
    if (!m_view) return;

    const auto position = args.CurrentPoint().RawPosition();
    const auto deviceType = args.CurrentPoint().PointerDevice().PointerDeviceType();
    const auto deviceSlot = args.CurrentPoint().PointerId();
    const auto updateKind = args.CurrentPoint().Properties().PointerUpdateKind();
    const auto x = position.X;
    const auto y = position.Y;

    if (deviceType == winrt::Windows::Devices::Input::PointerDeviceType::Mouse)
    {
        ProcessMouseButtons(*m_view, updateKind, x, y);
    }
    else
    {
        m_view->OnPointerDown(static_cast<int32_t>(deviceSlot), x, y, CoordinateUnits::Logical);
    }
}

void App::OnPointerReleased(CoreWindow const&, PointerEventArgs const& args)
{
    if (!m_view) return;

    const auto position = args.CurrentPoint().RawPosition();
    const auto deviceType = args.CurrentPoint().PointerDevice().PointerDeviceType();
    const auto deviceSlot = args.CurrentPoint().PointerId();
    const auto updateKind = args.CurrentPoint().Properties().PointerUpdateKind();
    const auto x = position.X;
    const auto y = position.Y;

    if (deviceType == winrt::Windows::Devices::Input::PointerDeviceType::Mouse)
    {
        ProcessMouseButtons(*m_view, updateKind, x, y);
    }
    else
    {
        m_view->OnPointerUp(static_cast<int32_t>(deviceSlot), x, y, CoordinateUnits::Logical);
    }
}

void App::OnPointerWheelChanged(CoreWindow const&, PointerEventArgs const& args)
{
    if (m_view)
    {
        const auto delta = args.CurrentPoint().Properties().MouseWheelDelta();
        m_view->OnMouseWheel(BNView::MouseWheelY(), -delta);
    }
}

void App::OnKeyPressed(CoreWindow const& window, KeyEventArgs const& args)
{
    if (args.VirtualKey() == VirtualKey::R)
    {
        RestartRuntime(window.Bounds());
    }
}

// DisplayInformation event handlers.

void App::OnDpiChanged(DisplayInformation const& /*sender*/, IInspectable const& /*args*/)
{
    // DPR is queried by the View through Babylon::Graphics::GetDevicePixelRatio
    // on each Resize, so we don't cache it here. SizeChanged fires after a DPI
    // change, which re-drives Resize.
}

void App::OnOrientationChanged(DisplayInformation const& /*sender*/, IInspectable const& /*args*/)
{
    // TODO: Implement.
    //m_deviceResources->SetCurrentOrientation(sender->CurrentOrientation);
}

void App::OnDisplayContentsInvalidated(DisplayInformation const& /*sender*/, IInspectable const& /*args*/)
{
    // TODO: Implement.
    //m_deviceResources->ValidateDevice();
}

void App::RestartRuntime(Rect bounds)
{
    Uninitialize();

    Babylon::Integrations::RuntimeOptions runtimeOptions{};
    runtimeOptions.enableDebugger = true;
    runtimeOptions.log = [](Babylon::Integrations::LogLevel /*level*/, std::string_view message) {
        std::string line{message};
        if (line.empty() || line.back() != '\n')
        {
            line.push_back('\n');
        }
        OutputDebugStringA(line.c_str());
        std::cout << line;
    };

    m_runtime.emplace(std::move(runtimeOptions));

    PlaygroundOptions playgroundOptions{};
    Playground::Initialize(playgroundOptions);
    Playground::LoadBootstrapScripts(*m_runtime);

    if (m_files == nullptr)
    {
        m_runtime->LoadScript("app:///Scripts/experience.js");
    }
    else
    {
        for (uint32_t idx = 0; idx < m_files.Size(); idx++)
        {
            auto file = m_files.GetAt(idx).as<winrt::Windows::Storage::IStorageFile>();

            // There is no built-in way to convert a local file path to a url in UWP, but
            // Foundation::Uri works with a url constructed using "file:///" with a local path.
            m_runtime->LoadScript("file:///" + winrt::to_string(file.Path()));
        }

        m_runtime->LoadScript("app:///Scripts/playground_runner.js");
    }

    // First View attach triggers Device construction, plugin init, and
    // flushes the queued scripts. The CoreWindow is passed as IInspectable
    // (matching Babylon::Graphics::WindowT on WinRT).
    IInspectable window{CoreWindow::GetForCurrentThread()};
    m_view.emplace(*m_runtime, window);

    // Drive the first Resize with the initial window bounds (logical pixels);
    // the View handles physical conversion via GetDevicePixelRatio.
    m_view->Resize(static_cast<uint32_t>(bounds.Width),
                   static_cast<uint32_t>(bounds.Height),
                   CoordinateUnits::Logical);
}
