#include "pch.h"
#include "ReactPackageProvider.h"
#include "NativeModules.h"


namespace winrt::Playground::implementation
{

void ReactPackageProvider::CreatePackage(winrt::Microsoft::ReactNative::IReactPackageBuilder const &packageBuilder) noexcept
{
    AddAttributedModules(packageBuilder);
}

} // namespace winrt::Playground::implementation


