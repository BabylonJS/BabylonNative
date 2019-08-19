#pragma once

#include "Runtime.h"

class ANativeWindow;

namespace babylon
{
    class RuntimeAndroid final : public Runtime
    {
    public:
        explicit RuntimeAndroid(ANativeWindow* nativeWindowPtr);
        explicit RuntimeAndroid(ANativeWindow* nativeWindowPtr, const std::string& rootUrl, uint32_t width, uint32_t height);
        RuntimeAndroid(const RuntimeAndroid&) = delete;
        void SetWindow(ANativeWindow* nativeWindowPtr);
    };
}
