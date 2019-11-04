#include <Babylon/RuntimeAndroid.h>
#include "RuntimeImpl.h"
//#include <filesystem>

namespace babylon
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

    void RuntimeImpl::ThreadProcedure()
    {
        RuntimeImpl::BaseThreadProcedure();
    }

}