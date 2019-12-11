#include <Babylon/RuntimeAndroid.h>
#include "RuntimeImpl.h"
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>

namespace Babylon
{
    RuntimeAndroid::RuntimeAndroid(ANativeWindow* nativeWindowPtr, LogCallback callback, ResourceLoadingCallback resourceLoadingCallback)
        : RuntimeAndroid{ nativeWindowPtr, ".", std::move(callback), std::move(resourceLoadingCallback) } // todo : GetModulePath().parent_path() std::fs experimental not available with ndk
    {
    }

    RuntimeAndroid::RuntimeAndroid(ANativeWindow* nativeWindowPtr, const std::string& rootUrl, LogCallback callback, ResourceLoadingCallback resourceLoadingCallback)
        : Runtime{ std::make_unique<RuntimeImpl>(nativeWindowPtr, rootUrl, std::move(callback), std::move(resourceLoadingCallback)) }
    {
        // android stub
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
