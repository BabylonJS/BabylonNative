#include "pch.h"
#include "EngineViewManager.h"

#include "JSValueReader.h"
#include "JSValueXaml.h"

using namespace winrt::Microsoft::ReactNative;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Foundation::Collections;

using namespace winrt::Windows::UI::Xaml;

namespace winrt::BabylonReactNative::implementation {
    EngineViewManager::EngineViewManager() {}

    // IViewManager
    hstring EngineViewManager::Name() const noexcept {
        return L"EngineView";
    }

    FrameworkElement EngineViewManager::CreateView() noexcept {
        _engineView = make<EngineView>();
        return _engineView;
    }

    // IViewManagerWithReactContext
    IReactContext EngineViewManager::ReactContext() noexcept {
        return _reactContext;
    }

    void EngineViewManager::ReactContext(IReactContext reactContext) noexcept {
        _reactContext = reactContext;
    }

    // IViewManagerWithNativeProperties
    IMapView<hstring, ViewManagerPropertyType> EngineViewManager::NativeProps() noexcept {
        auto nativeProps = winrt::single_threaded_map<hstring, ViewManagerPropertyType>();

        nativeProps.Insert(L"isTransparent", ViewManagerPropertyType::Boolean);
        nativeProps.Insert(L"antiAliasing", ViewManagerPropertyType::Number);

        return nativeProps.GetView();
    }

    void EngineViewManager::UpdateProperties(
        FrameworkElement const& view,
        IJSValueReader const& propertyMapReader) noexcept {

        if (auto engineView = view.try_as<EngineView>()) {
            engineView->UpdateProperties(propertyMapReader);
        } else {
            OutputDebugStringW(L"Type deduction for EngineView failed.");
        }
    }

    // IViewManagerWithExportedEventTypeConstants
    ConstantProviderDelegate EngineViewManager::ExportedCustomBubblingEventTypeConstants() noexcept {
        return nullptr;
    }

    ConstantProviderDelegate EngineViewManager::ExportedCustomDirectEventTypeConstants() noexcept {
        return [](winrt::Microsoft::ReactNative::IJSValueWriter const& constantWriter) {
            WriteCustomDirectEventTypeConstant(constantWriter, "onSnapshotDataReturned");
        };
    }
} // namespace winrt::BabylonReactNative::implementation
