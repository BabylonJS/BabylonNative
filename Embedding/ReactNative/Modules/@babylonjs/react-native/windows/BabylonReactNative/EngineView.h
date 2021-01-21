#pragma once
#include "EngineView.g.h"

namespace winrt::BabylonReactNative::implementation {
    struct EngineView : EngineViewT<EngineView>
    {
    public:
        EngineView();

    private:
        void OnSizeChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::SizeChangedEventArgs const& args);
        void OnPointerPressed(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::Input::PointerRoutedEventArgs const& args);
        void OnPointerMoved(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::Input::PointerRoutedEventArgs const& args);
        void OnPointerReleased(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::Input::PointerRoutedEventArgs const& args);
        void OnRendering();

        size_t _width{ 1 };
        size_t _height{ 1 };

        struct RevokerData
        {
            winrt::Windows::UI::Xaml::FrameworkElement::SizeChanged_revoker SizeChangedRevoker{};
            winrt::Windows::UI::Xaml::FrameworkElement::PointerPressed_revoker PointerPressedRevoker{};
            winrt::Windows::UI::Xaml::FrameworkElement::PointerMoved_revoker PointerMovedRevoker{};
            winrt::Windows::UI::Xaml::FrameworkElement::PointerReleased_revoker PointerReleasedRevoker{};
            winrt::Windows::UI::Xaml::Media::CompositionTarget::Rendering_revoker RenderingRevoker{};
        };
        RevokerData _revokerData{};
    };
}

namespace winrt::BabylonReactNative::factory_implementation {

struct EngineView : EngineViewT<EngineView, implementation::EngineView> {};

} // namespace winrt::BabylonReactNative::factory_implementation