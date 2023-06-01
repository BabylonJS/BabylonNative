#include "pch.h"
#include "BabylonModule.h"
#include "JSI/JsiApiContext.h"

// see https://developercommunity.visualstudio.com/t/-imp-std-init-once-complete-unresolved-external-sy/1684365
#if _MSC_VER >= 1932 // Visual Studio 2022 version 17.2+
#    pragma comment(linker, "/alternatename:__imp___std_init_once_complete=__imp_InitOnceComplete")
#    pragma comment(linker, "/alternatename:__imp___std_init_once_begin_initialize=__imp_InitOnceBeginInitialize")
#endif

using namespace winrt::BabylonReactNative::implementation;

REACT_INIT(Initialize);
void BabylonModule::Initialize(const winrt::Microsoft::ReactNative::ReactContext& reactContext) noexcept
{
    _reactContext = reactContext;
}

REACT_METHOD(CustomInitialize, L"initialize");
void BabylonModule::CustomInitialize(const winrt::Microsoft::ReactNative::ReactPromise<bool>& result) noexcept
{
    winrt::Microsoft::ReactNative::ExecuteJsi(_reactContext, [result, weakThis{ this->weak_from_this() }](facebook::jsi::Runtime& jsiRuntime) {
        if (auto trueThis = weakThis.lock()) {
            auto jsDispatcher = [weakThis{ trueThis->weak_from_this() }](std::function<void()> func)
            {
                if (auto trueThis = weakThis.lock())
                {
                    trueThis->_reactContext.JSDispatcher().Post([weakThis, func{ std::move(func) }]() {
                        func();
                    });
                }
            };
            BabylonNative::Initialize(jsiRuntime, jsDispatcher);
            result.Resolve(true);
        }
    });
}

REACT_METHOD(ResetView, L"resetView");
void BabylonModule::ResetView(const winrt::Microsoft::ReactNative::ReactPromise<bool>& result) noexcept
{
    _reactContext.UIDispatcher().Post([result]() {
        BabylonNative::ResetView();
        result.Resolve(true);
    });
}