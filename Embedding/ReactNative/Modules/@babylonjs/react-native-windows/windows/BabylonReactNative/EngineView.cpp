#include "pch.h"
#include "EngineView.h"
#include "EngineView.g.cpp"

using namespace winrt::Windows::Devices::Input;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::System::Threading;
using namespace winrt::Windows::UI::Core;
using namespace winrt::Windows::UI::Input;
using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Input;
using namespace winrt::Windows::UI::Xaml::Media;
using namespace winrt::Windows::UI::Xaml::Controls;

namespace winrt::BabylonReactNative::implementation {
    EngineView::EngineView() {

        _revokerData.SizeChangedRevoker = SizeChanged(winrt::auto_revoke, { this, &EngineView::OnSizeChanged });

        WorkItemHandler workItemHandler([weakThis{ this->get_weak() }](IAsyncAction const& /* action */)
        {
            if (auto trueThis = weakThis.get())
            {
                auto deviceTypes = static_cast<CoreInputDeviceTypes>(
                    static_cast<uint32_t>(Windows::UI::Core::CoreInputDeviceTypes::Mouse) |
                    static_cast<uint32_t>(Windows::UI::Core::CoreInputDeviceTypes::Touch) |
                    static_cast<uint32_t>(Windows::UI::Core::CoreInputDeviceTypes::Pen));
                auto coreInput = trueThis->CreateCoreIndependentInputSource(deviceTypes);

                trueThis->_revokerData.PointerPressedRevoker = coreInput.PointerPressed(winrt::auto_revoke, { trueThis.get(), &EngineView::OnPointerPressed });
                trueThis->_revokerData.PointerMovedRevoker = coreInput.PointerMoved(winrt::auto_revoke, { trueThis.get(), &EngineView::OnPointerMoved });
                trueThis->_revokerData.PointerReleasedRevoker = coreInput.PointerReleased(winrt::auto_revoke, { trueThis.get(), &EngineView::OnPointerReleased });

                coreInput.Dispatcher().ProcessEvents(Windows::UI::Core::CoreProcessEventsOption::ProcessUntilQuit);
            }
        });

        _inputLoopWorker = ThreadPool::RunAsync(workItemHandler, WorkItemPriority::High, WorkItemOptions::TimeSliced);

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

    void EngineView::OnPointerPressed(IInspectable const& /*sender*/, PointerEventArgs const& args)
    {
        const auto point = args.CurrentPoint();
        const auto properties = point.Properties();
        const auto deviceType = point.PointerDevice().PointerDeviceType();
        const auto position = point.Position();
        const uint32_t x = position.X < 0 ? 0 : static_cast<uint32_t>(position.X);
        const uint32_t y = position.Y < 0 ? 0 : static_cast<uint32_t>(position.Y);

        if (deviceType == PointerDeviceType::Mouse)
        {
            if (properties.IsLeftButtonPressed())
            {
                _pressedMouseButtons.insert(Babylon::LEFT_MOUSE_BUTTON_ID);
                Babylon::SetMouseButtonState(Babylon::LEFT_MOUSE_BUTTON_ID, true, x, y);
            }

            if (properties.IsMiddleButtonPressed())
            {
                _pressedMouseButtons.insert(Babylon::MIDDLE_MOUSE_BUTTON_ID);
                Babylon::SetMouseButtonState(Babylon::MIDDLE_MOUSE_BUTTON_ID, true, x, y);
            }

            if (properties.IsRightButtonPressed())
            {
                _pressedMouseButtons.insert(Babylon::RIGHT_MOUSE_BUTTON_ID);
                Babylon::SetMouseButtonState(Babylon::RIGHT_MOUSE_BUTTON_ID, true, x, y);
            }
        }
        else
        {
            const auto pointerId = point.PointerId();
            Babylon::SetTouchButtonState(pointerId, true, x, y);
        }
    }

    void EngineView::OnPointerMoved(IInspectable const& /*sender*/, PointerEventArgs const& args)
    {
        const auto point = args.CurrentPoint();
        const auto deviceType = point.PointerDevice().PointerDeviceType();
        const auto position = point.Position();
        const uint32_t x = position.X < 0 ? 0 : static_cast<uint32_t>(position.X);
        const uint32_t y = position.Y < 0 ? 0 : static_cast<uint32_t>(position.Y);

        if (deviceType == PointerDeviceType::Mouse)
        {
            Babylon::SetMousePosition(x, y);
        }
        else
        {
            const auto pointerId = point.PointerId();
            Babylon::SetTouchPosition(pointerId, x, y);
        }
    }

    void EngineView::OnPointerReleased(IInspectable const& /*sender*/, PointerEventArgs const& args)
    {
        const auto point = args.CurrentPoint();
        const auto properties = point.Properties();
        const auto deviceType = point.PointerDevice().PointerDeviceType();
        const auto position = point.Position();
        const uint32_t x = position.X < 0 ? 0 : static_cast<uint32_t>(position.X);
        const uint32_t y = position.Y < 0 ? 0 : static_cast<uint32_t>(position.Y);

        if (point.PointerDevice().PointerDeviceType() == PointerDeviceType::Mouse)
        {
            if (!properties.IsLeftButtonPressed() &&
                _pressedMouseButtons.find(Babylon::LEFT_MOUSE_BUTTON_ID) != _pressedMouseButtons.end())
            {
                _pressedMouseButtons.erase(Babylon::LEFT_MOUSE_BUTTON_ID);
                Babylon::SetMouseButtonState(Babylon::LEFT_MOUSE_BUTTON_ID, false, x, y);
            }

            if (!properties.IsMiddleButtonPressed() &&
                _pressedMouseButtons.find(Babylon::MIDDLE_MOUSE_BUTTON_ID) != _pressedMouseButtons.end())
            {
                _pressedMouseButtons.erase(Babylon::MIDDLE_MOUSE_BUTTON_ID);
                Babylon::SetMouseButtonState(Babylon::MIDDLE_MOUSE_BUTTON_ID, false, x, y);
            }

            if (!properties.IsRightButtonPressed() &&
                _pressedMouseButtons.find(Babylon::RIGHT_MOUSE_BUTTON_ID) != _pressedMouseButtons.end())
            {
                _pressedMouseButtons.erase(Babylon::RIGHT_MOUSE_BUTTON_ID);
                Babylon::SetMouseButtonState(Babylon::RIGHT_MOUSE_BUTTON_ID, false, x, y);
            }
        }
        else
        {
            const auto pointerId = point.PointerId();
            Babylon::SetTouchButtonState(pointerId, false, x, y);
        }
    }

    void EngineView::OnRendering()
    {
        Babylon::RenderView();
    }
}