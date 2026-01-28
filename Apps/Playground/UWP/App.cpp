#include "App.h"

#include <winrt/windows.ui.core.h>

#include <sstream>
#include <iostream>

namespace
{
    template <typename T>
    T from_cx(Platform::Object^ from)
    {
        T to{ nullptr };

        if (from != nullptr)
        {
            winrt::check_hresult(reinterpret_cast<::IUnknown*>(from)
                ->QueryInterface(winrt::guid_of<T>(), winrt::put_abi(to)));
        }

        return to;
    }
}

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

// Helper function to handle mouse button routing
void ProcessMouseButtons(Babylon::Plugins::NativeInput* input, PointerUpdateKind updateKind, int x, int y)
{
    switch (updateKind)
    {
        case PointerUpdateKind::LeftButtonPressed:
            input->MouseDown(Babylon::Plugins::NativeInput::LEFT_MOUSE_BUTTON_ID, x, y);
            break;
        case PointerUpdateKind::MiddleButtonPressed:
            input->MouseDown(Babylon::Plugins::NativeInput::MIDDLE_MOUSE_BUTTON_ID, x, y);
            break;
        case PointerUpdateKind::RightButtonPressed:
            input->MouseDown(Babylon::Plugins::NativeInput::RIGHT_MOUSE_BUTTON_ID, x, y);
            break;
        case PointerUpdateKind::LeftButtonReleased:
            input->MouseUp(Babylon::Plugins::NativeInput::LEFT_MOUSE_BUTTON_ID, x, y);
            break;
        case PointerUpdateKind::MiddleButtonReleased:
            input->MouseUp(Babylon::Plugins::NativeInput::MIDDLE_MOUSE_BUTTON_ID, x, y);
            break;
        case PointerUpdateKind::RightButtonReleased:
            input->MouseUp(Babylon::Plugins::NativeInput::RIGHT_MOUSE_BUTTON_ID, x, y);
            break;
    }
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

    window->PointerWheelChanged +=
        ref new TypedEventHandler<CoreWindow^, PointerEventArgs^>(this, &App::OnPointerWheelChanged);

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
        if (m_appContext)
        {
            m_appContext->DeviceUpdate().Finish();
            m_appContext->Device().FinishRenderingCurrentFrame();
            m_appContext->Device().StartRenderingCurrentFrame();
            m_appContext->DeviceUpdate().Start();
        }

        CoreWindow::GetForCurrentThread()->Dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessAllIfPresent);
    }
}

// Required for IFrameworkView.
// Terminate events do not cause Uninitialize to be called. It will be called if your IFrameworkView
// class is torn down while the app is in the foreground.
void App::Uninitialize()
{
    m_appContext.reset();
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

    if (m_appContext)
    {
        m_appContext->DeviceUpdate().Finish();
        m_appContext->Device().FinishRenderingCurrentFrame();

        m_appContext->Runtime().Suspend();
    }

    deferral->Complete();
}

void App::OnResuming(Platform::Object^ sender, Platform::Object^ args)
{
    if (m_appContext)
    {
        // Restore any data or state that was unloaded on suspend. By default, data
        // and state are persisted when resuming from suspend. Note that this event
        // does not occur if the app was previously terminated.
        m_appContext->Runtime().Resume();

        m_appContext->Device().StartRenderingCurrentFrame();
        m_appContext->DeviceUpdate().Start();
    }
}

// Window event handlers.

void App::OnWindowSizeChanged(CoreWindow^ /*sender*/, WindowSizeChangedEventArgs^ args)
{
    if (m_appContext)
    {
        size_t width = static_cast<size_t>(args->Size.Width * m_displayScale);
        size_t height = static_cast<size_t>(args->Size.Height * m_displayScale);
        m_appContext->Device().UpdateSize(width, height);
    }
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
    if (m_appContext && m_appContext->Input())
    {
        const auto& position = args->CurrentPoint->RawPosition;
        const auto deviceType = args->CurrentPoint->PointerDevice->PointerDeviceType;
        const auto deviceSlot = args->CurrentPoint->PointerId;
        const auto updateKind = args->CurrentPoint->Properties->PointerUpdateKind;
        const auto x = static_cast<int>(position.X * m_displayScale);
        const auto y = static_cast<int>(position.Y * m_displayScale);

        if (deviceType == Windows::Devices::Input::PointerDeviceType::Mouse)
        {
            m_appContext->Input()->MouseMove(x, y);

            if (args->CurrentPoint->IsInContact)
            {
                ProcessMouseButtons(m_appContext->Input(), updateKind, x, y);
            }
        }
        else
        {
            m_appContext->Input()->TouchMove(deviceSlot, x, y);
        }
    }
}

void App::OnPointerPressed(CoreWindow^, PointerEventArgs^ args)
{
    if (m_appContext && m_appContext->Input())
    {
        const auto& position = args->CurrentPoint->RawPosition;
        const auto deviceType = args->CurrentPoint->PointerDevice->PointerDeviceType;
        const auto deviceSlot = args->CurrentPoint->PointerId;
        const auto updateKind = args->CurrentPoint->Properties->PointerUpdateKind;
        const auto x = static_cast<int>(position.X * m_displayScale);
        const auto y = static_cast<int>(position.Y * m_displayScale);

        if (deviceType == Windows::Devices::Input::PointerDeviceType::Mouse)
        {
            ProcessMouseButtons(m_appContext->Input(), updateKind, x, y);
        }
        else
        {
            m_appContext->Input()->TouchDown(deviceSlot, x, y);
        }
    }
}

void App::OnPointerReleased(CoreWindow^, PointerEventArgs^ args)
{
    if (m_appContext && m_appContext->Input())
    {
        const auto& position = args->CurrentPoint->RawPosition;
        const auto deviceType = args->CurrentPoint->PointerDevice->PointerDeviceType;
        const auto deviceSlot = args->CurrentPoint->PointerId;
        const auto updateKind = args->CurrentPoint->Properties->PointerUpdateKind;
        const auto x = static_cast<int>(position.X * m_displayScale);
        const auto y = static_cast<int>(position.Y * m_displayScale);

        if (deviceType == Windows::Devices::Input::PointerDeviceType::Mouse)
        {
            ProcessMouseButtons(m_appContext->Input(), updateKind, x, y);
        }
        else
        {
            m_appContext->Input()->TouchUp(deviceSlot, x, y);
        }
    }
}
void App::OnPointerWheelChanged(CoreWindow^, PointerEventArgs^ args)
{
    if (m_appContext && m_appContext->Input())
    {
        const auto delta = args->CurrentPoint->Properties->MouseWheelDelta;
        m_appContext->Input()->MouseWheel(Babylon::Plugins::NativeInput::MOUSEWHEEL_Y_ID, -delta);
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
    auto window = from_cx<winrt::Windows::Foundation::IInspectable>(CoreWindow::GetForCurrentThread());

    m_appContext.emplace(
        window,
        width,
        height,
        [](const char* message) {
            std::ostringstream ss{};
            ss << message << std::endl;
            OutputDebugStringA(ss.str().data());
            std::cout << ss.str();
        });

    if (m_files == nullptr)
    {
        m_appContext->ScriptLoader().LoadScript("app:///Scripts/experience.js");
    }
    else
    {
        for (unsigned int idx = 0; idx < m_files->Size; idx++)
        {
            auto file{static_cast<Windows::Storage::IStorageFile^>(m_files->GetAt(idx))};

            // There is no built-in way to convert a local file path to a url in UWP, but
            // Foundation::Uri works with a url constructed using "file:///" with a local path.
            m_appContext->ScriptLoader().LoadScript("file:///" + winrt::to_string(file->Path->Data()));
        }

        m_appContext->ScriptLoader().LoadScript("app:///Scripts/playground_runner.js");
    }
}
