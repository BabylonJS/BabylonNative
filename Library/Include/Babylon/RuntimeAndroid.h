#pragma once

#include "Runtime.h"

class ANativeWindow;

namespace Babylon
{
    class RuntimeAndroid final : public Runtime
    {
    public:

        explicit RuntimeAndroid(ANativeWindow* nativeWindowPtr, float width, float height, ResourceLoadingCallback resourceLoadingCallback);
        explicit RuntimeAndroid(ANativeWindow* nativeWindowPtr, const std::string& rootUrl, float width, float height, ResourceLoadingCallback resourceLoadingCallback);
        RuntimeAndroid(const RuntimeAndroid&) = delete;
        void UpdateSurface(float width, float height, ANativeWindow* nativeWindowPtr);
    };
}
