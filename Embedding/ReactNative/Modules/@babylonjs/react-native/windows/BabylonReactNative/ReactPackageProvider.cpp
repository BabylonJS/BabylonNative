#include "pch.h"
#include "ReactPackageProvider.h"
#if __has_include("ReactPackageProvider.g.cpp")
#include "ReactPackageProvider.g.cpp"
#endif

#include "NativeModules.h"
#include "BabylonModule.h"
#include "EngineViewManager.h"

using namespace winrt::Microsoft::ReactNative;

namespace winrt::BabylonReactNative::implementation {

void ReactPackageProvider::CreatePackage(IReactPackageBuilder const &packageBuilder) noexcept
{
  AddAttributedModules(packageBuilder);

  // Different module providers exist, we could look into codegen to make sure that javascript interfaces map 1:1 with constructed modules at compile time
  packageBuilder.AddModule(L"BabylonModule", MakeModuleProvider<BabylonModule>());
  packageBuilder.AddViewManager(L"EngineViewManager", []() { return winrt::make<EngineViewManager>(); });
}

} // namespace winrt::BabylonReactNative::implementation
