#include <Babylon/RuntimeAndroid.h>
#include "RuntimeImpl.h"
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>

namespace babylon
{

    RuntimeAndroid::RuntimeAndroid(ANativeWindow* nativeWindowPtr, LogCallback callback)
        : RuntimeAndroid{ nativeWindowPtr, ".", std::move(callback) } // todo : GetModulePath().parent_path() std::fs experimental not available with ndk
    {
    }

    RuntimeAndroid::RuntimeAndroid(ANativeWindow* nativeWindowPtr, const std::string& rootUrl, LogCallback callback)
        : Runtime{ std::make_unique<RuntimeImpl>(nativeWindowPtr, rootUrl, std::move(callback)) }
    {
        // android stub
    }

    void RuntimeAndroid::SetWindow(ANativeWindow* nativeWindowPtr)
    {
        bgfx::PlatformData pd;
        pd.ndt          = NULL;
        pd.nwh          = nativeWindowPtr;
        pd.context      = NULL;
        pd.backBuffer   = NULL;
        pd.backBufferDS = NULL;
        bgfx::setPlatformData(pd);
        bgfx::reset(128, 128);
    }

    void RuntimeImpl::ThreadProcedure()
    {
        RuntimeImpl::BaseThreadProcedure();
    }

}