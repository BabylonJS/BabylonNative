#include "pch.h"
#include "EngineView.h"
#include "EngineView.g.cpp"

using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Input;
using namespace winrt::Windows::UI::Xaml::Media;
using namespace winrt::Windows::UI::Xaml::Controls;

namespace winrt::BabylonReactNative::implementation {
    EngineView::EngineView() {
        _revokerData.SizeChangedRevoker = SizeChanged(winrt::auto_revoke, { this, &EngineView::OnSizeChanged });
        _revokerData.PointerPressedRevoker = PointerPressed(winrt::auto_revoke, { this, &EngineView::OnPointerPressed });
        _revokerData.PointerMovedRevoker = PointerMoved(winrt::auto_revoke, { this, &EngineView::OnPointerMoved });
        _revokerData.PointerReleasedRevoker = PointerReleased(winrt::auto_revoke, { this, &EngineView::OnPointerReleased });

        _revokerData.RenderingRevoker = CompositionTarget::Rendering(winrt::auto_revoke, [weakThis{ this->get_weak() }](auto const&, auto const&)
        {
            if (auto trueThis = weakThis.get())
            {
                trueThis->OnRendering();
            }
        });
    }

    void EngineView::OnSizeChanged(IInspectable const& /*sender*/, SizeChangedEventArgs const& args)
    {
        const auto size = args.NewSize();
        _width = static_cast<size_t>(size.Width);
        _height = static_cast<size_t>(size.Height);

        // Use windowTypePtr == 2 for xaml swap chain panels
        auto windowTypePtr = reinterpret_cast<void*>(2);
        auto windowPtr = get_abi(static_cast<winrt::Windows::UI::Xaml::Controls::SwapChainPanel>(*this));
        Babylon::UpdateView(windowPtr, _width, _height, windowTypePtr);
    }

    void EngineView::OnPointerPressed(IInspectable const& /*sender*/, PointerRoutedEventArgs const& args)
    {
        const auto pointerId = args.Pointer().PointerId();
        const auto buttonId = 0; // Update as needed
        const auto point = args.GetCurrentPoint(*this);
        const auto position = point.Position();
        const uint32_t x = position.X < 0 ? 0 : static_cast<uint32_t>(position.X);
        const uint32_t y = position.Y < 0 ? 0 : static_cast<uint32_t>(position.Y);
        Babylon::SetPointerButtonState(pointerId, buttonId, true, x, y);
    }

    void EngineView::OnPointerMoved(IInspectable const& /*sender*/, PointerRoutedEventArgs const& args)
    {
        const auto pointerId = args.Pointer().PointerId();
        const auto point = args.GetCurrentPoint(*this);
        const auto position = point.Position();
        const uint32_t x = position.X < 0 ? 0 : static_cast<uint32_t>(position.X);
        const uint32_t y = position.Y < 0 ? 0 : static_cast<uint32_t>(position.Y);
        Babylon::SetPointerPosition(pointerId, x, y);
    }

    void EngineView::OnPointerReleased(IInspectable const& /*sender*/, PointerRoutedEventArgs const& args)
    {
        const auto pointerId = args.Pointer().PointerId();
        const auto buttonId = 0; // Update as needed
        const auto point = args.GetCurrentPoint(*this);
        const auto position = point.Position();
        const uint32_t x = position.X < 0 ? 0 : static_cast<uint32_t>(position.X);
        const uint32_t y = position.Y < 0 ? 0 : static_cast<uint32_t>(position.Y);
        Babylon::SetPointerButtonState(pointerId, buttonId, false, x, y);
    }

    void EngineView::OnRendering()
    {
        Babylon::RenderView();
    }
}