#include "pch.h"
#include "EngineView.h"
#include "EngineView.g.cpp"
#include "JSValueXaml.h"

using namespace winrt::Windows::Devices::Input;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::System::Threading;
using namespace winrt::Windows::UI::Core;
using namespace winrt::Windows::UI::Input;
using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Input;
using namespace winrt::Windows::UI::Xaml::Media;
using namespace winrt::Windows::UI::Xaml::Controls;
using namespace winrt::Microsoft::ReactNative;

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
                trueThis->_inputSource = trueThis->CreateCoreIndependentInputSource(deviceTypes);

                trueThis->_revokerData.PointerPressedRevoker = trueThis->_inputSource.PointerPressed(winrt::auto_revoke, { trueThis.get(), &EngineView::OnPointerPressed });
                trueThis->_revokerData.PointerMovedRevoker = trueThis->_inputSource.PointerMoved(winrt::auto_revoke, { trueThis.get(), &EngineView::OnPointerMoved });
                trueThis->_revokerData.PointerReleasedRevoker = trueThis->_inputSource.PointerReleased(winrt::auto_revoke, { trueThis.get(), &EngineView::OnPointerReleased });

                trueThis->_inputSource.Dispatcher().ProcessEvents(Windows::UI::Core::CoreProcessEventsOption::ProcessUntilQuit);
            }
        });

        // TODO: move to std::thread compared to consuming ThreadPool resources once engine lifecycle bugs are addressed and EngineView's destructor can be successfully invoked.
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

        BabylonNative::UpdateView(static_cast<winrt::Windows::UI::Xaml::Controls::SwapChainPanel>(*this), _width, _height);
    }

    void EngineView::OnPointerPressed(IInspectable const& /*sender*/, PointerEventArgs const& args)
    {
        const auto point = args.CurrentPoint();
        const auto properties = point.Properties();
        const auto deviceType = point.PointerDevice().PointerDeviceType();
        const auto position = point.Position();
        const int32_t x = static_cast<int32_t>(position.X);
        const int32_t y = static_cast<int32_t>(position.Y);
        const auto pointerId = point.PointerId();

        if (!_inputSource.HasCapture())
        {
            _inputSource.SetPointerCapture();
        }

        _pressedPointers.insert(pointerId);

        if (deviceType == PointerDeviceType::Mouse)
        {
            if (properties.IsLeftButtonPressed())
            {
                _pressedMouseButtons.insert(BabylonNative::LEFT_MOUSE_BUTTON_ID);
                BabylonNative::SetMouseButtonState(BabylonNative::LEFT_MOUSE_BUTTON_ID, true, x, y);
            }

            if (properties.IsMiddleButtonPressed())
            {
                _pressedMouseButtons.insert(BabylonNative::MIDDLE_MOUSE_BUTTON_ID);
                BabylonNative::SetMouseButtonState(BabylonNative::MIDDLE_MOUSE_BUTTON_ID, true, x, y);
            }

            if (properties.IsRightButtonPressed())
            {
                _pressedMouseButtons.insert(BabylonNative::RIGHT_MOUSE_BUTTON_ID);
                BabylonNative::SetMouseButtonState(BabylonNative::RIGHT_MOUSE_BUTTON_ID, true, x, y);
            }
        }
        else
        {
            BabylonNative::SetTouchButtonState(pointerId, true, x, y);
        }
    }

    void EngineView::OnPointerMoved(IInspectable const& /*sender*/, PointerEventArgs const& args)
    {
        const auto point = args.CurrentPoint();
        const auto deviceType = point.PointerDevice().PointerDeviceType();
        const auto position = point.Position();
        const int32_t x = static_cast<int32_t>(position.X);
        const int32_t y = static_cast<int32_t>(position.Y);

        if (deviceType == PointerDeviceType::Mouse)
        {
            BabylonNative::SetMousePosition(x, y);
        }
        else
        {
            const auto pointerId = point.PointerId();
            BabylonNative::SetTouchPosition(pointerId, x, y);
        }
    }

    void EngineView::OnPointerReleased(IInspectable const& /*sender*/, PointerEventArgs const& args)
    {
        const auto point = args.CurrentPoint();
        const auto properties = point.Properties();
        const auto deviceType = point.PointerDevice().PointerDeviceType();
        const auto position = point.Position();
        const int32_t x = static_cast<int32_t>(position.X);
        const int32_t y = static_cast<int32_t>(position.Y);
        const auto pointerId = point.PointerId();

        _pressedPointers.erase(pointerId);
        if (_pressedPointers.size() == 0 &&
            _inputSource.HasCapture())
        {
            _inputSource.ReleasePointerCapture();
        }

        if (point.PointerDevice().PointerDeviceType() == PointerDeviceType::Mouse)
        {
            if (!properties.IsLeftButtonPressed() &&
                _pressedMouseButtons.find(BabylonNative::LEFT_MOUSE_BUTTON_ID) != _pressedMouseButtons.end())
            {
                _pressedMouseButtons.erase(BabylonNative::LEFT_MOUSE_BUTTON_ID);
                BabylonNative::SetMouseButtonState(BabylonNative::LEFT_MOUSE_BUTTON_ID, false, x, y);
            }

            if (!properties.IsMiddleButtonPressed() &&
                _pressedMouseButtons.find(BabylonNative::MIDDLE_MOUSE_BUTTON_ID) != _pressedMouseButtons.end())
            {
                _pressedMouseButtons.erase(BabylonNative::MIDDLE_MOUSE_BUTTON_ID);
                BabylonNative::SetMouseButtonState(BabylonNative::MIDDLE_MOUSE_BUTTON_ID, false, x, y);
            }

            if (!properties.IsRightButtonPressed() &&
                _pressedMouseButtons.find(BabylonNative::RIGHT_MOUSE_BUTTON_ID) != _pressedMouseButtons.end())
            {
                _pressedMouseButtons.erase(BabylonNative::RIGHT_MOUSE_BUTTON_ID);
                BabylonNative::SetMouseButtonState(BabylonNative::RIGHT_MOUSE_BUTTON_ID, false, x, y);
            }
        }
        else
        {
            BabylonNative::SetTouchButtonState(pointerId, false, x, y);
        }
    }

    void EngineView::OnRendering()
    {
        BabylonNative::RenderView();
    }

    void EngineView::UpdateProperties(IJSValueReader const& reader)
    {
        auto const& propertyMap = JSValueObject::ReadFrom(reader);

        for (auto const& pair : propertyMap)
        {
            auto const& propertyName = pair.first;
            auto const& propertyValue = pair.second;

            if (propertyName == "isTransparent")
            {
                bool isTransparent = propertyValue.AsBoolean();
                BabylonNative::UpdateAlphaPremultiplied(isTransparent);
            } else if (propertyName == "antiAliasing")
            {
                auto value = propertyValue.AsUInt8();
                BabylonNative::UpdateMSAA(value);
            } else if (propertyName == "isTopMost")
            {
                // todo: implementation
            }
        }
    }
}