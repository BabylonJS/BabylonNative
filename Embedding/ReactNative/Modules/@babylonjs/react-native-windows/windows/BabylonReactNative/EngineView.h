#pragma once
#include "EngineView.g.h"
#include <unordered_set>

namespace winrt::BabylonReactNative::implementation {
    struct EngineView : EngineViewT<EngineView>
    {
    public:
        EngineView();

    private:
        void OnSizeChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::SizeChangedEventArgs const& args);
        void OnPointerPressed(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Core::PointerEventArgs const& args);
        void OnPointerMoved(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Core::PointerEventArgs const& args);
        void OnPointerReleased(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Core::PointerEventArgs const& args);
        void OnRendering();
        uint32_t GetButtonId(winrt::Windows::Devices::Input::PointerDeviceType deviceType, winrt::Windows::UI::Input::PointerPointProperties properties);

        size_t _width{ 1 };
        size_t _height{ 1 };
        winrt::Windows::Foundation::IAsyncAction _inputLoopWorker{};
        std::unordered_set<uint32_t> _pressedMouseButtons{};

        struct RevokerData
        {
            winrt::Windows::UI::Xaml::FrameworkElement::SizeChanged_revoker SizeChangedRevoker{};
            winrt::Windows::UI::Core::CoreIndependentInputSource::PointerPressed_revoker PointerPressedRevoker{};
            winrt::Windows::UI::Core::CoreIndependentInputSource::PointerMoved_revoker PointerMovedRevoker{};
            winrt::Windows::UI::Core::CoreIndependentInputSource::PointerReleased_revoker PointerReleasedRevoker{};
            winrt::Windows::UI::Xaml::Media::CompositionTarget::Rendering_revoker RenderingRevoker{};
        };
        RevokerData _revokerData{};
    };
}

namespace winrt::BabylonReactNative::factory_implementation {

struct EngineView : EngineViewT<EngineView, implementation::EngineView> {};

} // namespace winrt::BabylonReactNative::factory_implementation