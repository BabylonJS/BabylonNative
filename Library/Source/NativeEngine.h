#include <napi/napi.h>

#include <memory>

namespace babylon
{
    class RuntimeImpl;

    class NativeEngine final
    {
    public:
        static void Initialize(void* nativeWindowPtr, RuntimeImpl& runtimeImpl);
    };
}
