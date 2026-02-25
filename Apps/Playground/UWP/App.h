#pragma once

#include <Shared/AppContext.h>
#include <optional>

#include <winrt/Windows.ApplicationModel.h>
#include <winrt/Windows.ApplicationModel.Activation.h>
#include <winrt/Windows.ApplicationModel.Core.h>
#include <winrt/Windows.Devices.Input.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Graphics.Display.h>
#include <winrt/Windows.Storage.h>
#include <winrt/Windows.System.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Input.h>

// Main entry point for our app. Connects the app with the Windows shell and handles application lifecycle events.
struct App : winrt::implements<App, winrt::Windows::ApplicationModel::Core::IFrameworkView>
{
    App();

    // IFrameworkView Methods.
    void Initialize(winrt::Windows::ApplicationModel::Core::CoreApplicationView const& applicationView);
    void SetWindow(winrt::Windows::UI::Core::CoreWindow const& window);
    void Load(winrt::hstring const& entryPoint);
    void Run();
    void Uninitialize();

private:
    // Application lifecycle event handlers.
    void OnActivated(winrt::Windows::ApplicationModel::Core::CoreApplicationView const& applicationView, winrt::Windows::ApplicationModel::Activation::IActivatedEventArgs const& args);
    void OnSuspending(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::ApplicationModel::SuspendingEventArgs const& args);
    void OnResuming(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::Foundation::IInspectable const& args);

    // Window event handlers.
    void OnWindowSizeChanged(winrt::Windows::UI::Core::CoreWindow const& sender, winrt::Windows::UI::Core::WindowSizeChangedEventArgs const& args);
    void OnVisibilityChanged(winrt::Windows::UI::Core::CoreWindow const& sender, winrt::Windows::UI::Core::VisibilityChangedEventArgs const& args);
    void OnWindowClosed(winrt::Windows::UI::Core::CoreWindow const& sender, winrt::Windows::UI::Core::CoreWindowEventArgs const& args);
    void OnPointerMoved(winrt::Windows::UI::Core::CoreWindow const& sender, winrt::Windows::UI::Core::PointerEventArgs const& args);
    void OnPointerPressed(winrt::Windows::UI::Core::CoreWindow const& sender, winrt::Windows::UI::Core::PointerEventArgs const& args);
    void OnPointerReleased(winrt::Windows::UI::Core::CoreWindow const& sender, winrt::Windows::UI::Core::PointerEventArgs const& args);
    void OnPointerWheelChanged(winrt::Windows::UI::Core::CoreWindow const& sender, winrt::Windows::UI::Core::PointerEventArgs const& args);
    void OnKeyPressed(winrt::Windows::UI::Core::CoreWindow const& sender, winrt::Windows::UI::Core::KeyEventArgs const& args);

    // DisplayInformation event handlers.
    void OnDpiChanged(winrt::Windows::Graphics::Display::DisplayInformation const& sender, winrt::Windows::Foundation::IInspectable const& args);
    void OnOrientationChanged(winrt::Windows::Graphics::Display::DisplayInformation const& sender, winrt::Windows::Foundation::IInspectable const& args);
    void OnDisplayContentsInvalidated(winrt::Windows::Graphics::Display::DisplayInformation const& sender, winrt::Windows::Foundation::IInspectable const& args);

    void RestartRuntime(winrt::Windows::Foundation::Rect bounds);

    std::optional<AppContext> m_appContext{};

    winrt::Windows::Foundation::Collections::IVectorView<winrt::Windows::Storage::IStorageItem> m_files{nullptr};
    bool m_windowClosed;
    bool m_windowVisible;
    float m_displayScale{1.f};
};

struct Direct3DApplicationSource : winrt::implements<Direct3DApplicationSource, winrt::Windows::ApplicationModel::Core::IFrameworkViewSource>
{
    winrt::Windows::ApplicationModel::Core::IFrameworkView CreateView();
};
