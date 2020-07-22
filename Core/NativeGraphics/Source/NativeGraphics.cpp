#include "NativeGraphics.h"

#include <bgfx/bgfx.h>
#include <bgfx/platform.h>

#define BGFX_RESET_FLAGS (BGFX_RESET_VSYNC | BGFX_RESET_MSAA_X4 | BGFX_RESET_MAXANISOTROPY)

namespace Babylon
{
    struct NativeGraphics::Impl
    {
        ~Impl();

        // TODO: Populate this with something.
        std::unique_ptr<bgfx::CallbackI> BgfxCallback{};
    };

    NativeGraphics::Impl::~Impl()
    {
        bgfx::shutdown();
    }

    template<>
    NativeGraphics NativeGraphics::InitializeFromWindow<void*>(void* nativeWindowPtr, size_t width, size_t height)
    {
        NativeGraphics graphics{};

        // Initialize bgfx.
        bgfx::Init init{};
        init.platformData.nwh = nativeWindowPtr;
        bgfx::setPlatformData(init.platformData);
#if (ANDROID)
        init.type = bgfx::RendererType::OpenGLES;
#else
        init.type = bgfx::RendererType::Direct3D11;
#endif
        init.resolution.width = static_cast<uint32_t>(width);
        init.resolution.height = static_cast<uint32_t>(height);
        init.resolution.reset = BGFX_RESET_FLAGS;
        init.callback = graphics.m_impl->BgfxCallback.get();
        bgfx::init(init);
        bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x443355FF, 1.0f, 0);
        bgfx::setViewRect(0, 0, 0, static_cast<uint16_t>(init.resolution.width), static_cast<uint16_t>(init.resolution.height));
        bgfx::touch(0);

        return{};
    }
}