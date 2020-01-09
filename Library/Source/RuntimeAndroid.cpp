#include <Babylon/RuntimeAndroid.h>
#include "RuntimeImpl.h"

#include "NativeEngine.h"

namespace Babylon
{

    RuntimeAndroid::RuntimeAndroid(ANativeWindow* nativeWindowPtr, float width, float height, ResourceLoadingCallback resourceLoadingCallback)
        : RuntimeAndroid{nativeWindowPtr, ".", width, height, std::move(resourceLoadingCallback)} // todo : GetModulePath().parent_path() std::fs experimental not available with ndk
    {
    }

    RuntimeAndroid::RuntimeAndroid(ANativeWindow* nativeWindowPtr, const std::string& rootUrl, float width, float height, ResourceLoadingCallback resourceLoadingCallback)
        : Runtime{std::make_unique<RuntimeImpl>(nativeWindowPtr, rootUrl, std::move(resourceLoadingCallback))}
    {
        //NativeEngine::InitializeWindow(nativeWindowPtr, static_cast<uint32_t>(width), static_cast<uint32_t>(height));
    }

    void RuntimeAndroid::UpdateSurface(float width, float height, ANativeWindow* nativeWindowPtr)
    {
        m_impl->UpdateSurface(width, height, nativeWindowPtr);
    }

    void RuntimeImpl::ThreadProcedure()
    {
        RuntimeImpl::BaseThreadProcedure();
    }
}
