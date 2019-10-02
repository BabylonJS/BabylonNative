#include <Babylon/RuntimeAndroid.h>
#include "RuntimeImpl.h"
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>

namespace babylon
{

    RuntimeAndroid::RuntimeAndroid(ANativeWindow* nativeWindowPtr)
<<<<<<< HEAD
        : RuntimeAndroid{nativeWindowPtr, GetUrlFromPath("."), 16, 16 } // todo : GetModulePath().parent_path() std::fs experimental not available with ndk
=======
        : RuntimeAndroid{nativeWindowPtr, "." } // todo : GetModulePath().parent_path() std::fs experimental not available with ndk
>>>>>>> af5c2d45b1a62f7faacd54f3f23b33a39731c46c
    {
    }

    RuntimeAndroid::RuntimeAndroid(ANativeWindow* nativeWindowPtr, const std::string& rootUrl, uint32_t width, uint32_t height)
        : Runtime{ std::make_unique<RuntimeImpl>(nativeWindowPtr, rootUrl, width, height) }
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
        bgfx::reset(128,128);
    }

    void RuntimeImpl::ThreadProcedure()
    {
        RuntimeImpl::BaseThreadProcedure();
    }

}