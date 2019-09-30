#include <napi/napi.h>

#include <memory>

namespace babylon
{
    class RuntimeImpl;

    class NativeEngine final
    {
    public:
        NativeEngine(void* nativeWindowPtr, RuntimeImpl&, uint32_t width, uint32_t height);
        ~NativeEngine();

        void Initialize(Napi::Env& env);
        void UpdateSize(float width, float height);
        void UpdateRenderTarget();

    private:
        class Impl;
        std::unique_ptr<Impl> m_impl;

        friend class XrPlugin;
    };
}
