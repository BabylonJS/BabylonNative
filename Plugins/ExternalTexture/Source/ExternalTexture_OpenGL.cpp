#include <Babylon/Plugins/ExternalTexture.h>
#include <Babylon/Graphics/Device.h>
#include <Babylon/Graphics/DeviceContext.h>
#include <Babylon/Graphics/RendererType.h>
#include <Babylon/Graphics/Texture.h>
#include <napi/pointer.h>
#include <bx/bx.h>
#include <bgfx/bgfx.h>

#include "ExternalTexture_Base.h"

namespace Babylon::Plugins
{
    // ExternalTexture is not implemented on the OpenGL backend. The Impl
    // stubs throw "not implemented" so callers get an unambiguous diagnostic
    // matching every other backend's unimplemented path. The shared
    // dispatchers in ExternalTexture_Shared.h (included below) call these
    // stubs unconditionally, so MSVC's flow analysis would otherwise flag
    // the dispatcher's post-call code as unreachable (C4702) under /WX. The
    // pragma block around the include suppresses C4702 only for the
    // dispatcher code instantiated in this translation unit; any other code
    // in this file is still subject to /WX C4702 enforcement.
    //
    // Alternatives considered:
    //   - [[noreturn]] on the stubs: MSVC propagates "never returns" through
    //     the shared dispatcher, which flags *more* post-call statements as
    //     unreachable. Tried and rejected (it made the warning worse).
    //   - TU-wide /wd4702 via target_compile_options: silences any future
    //     legitimate C4702 elsewhere in the same TU, defeating /WX.
    //   - Replacing the throws with inert return-default stubs: changes
    //     product behaviour (callers would silently receive a null texture
    //     instead of a clear "not implemented" error) to work around a
    //     compiler warning. Rejected on principle.
    class ExternalTexture::Impl final : public ImplBase
    {
    public:
        // Implemented in ExternalTexture_Shared.h
        Impl(Graphics::TextureT, std::optional<Graphics::TextureFormatT>);
        void Update(Graphics::TextureT, std::optional<Graphics::TextureFormatT>, std::optional<uint16_t>);

        Graphics::TextureT Get() const
        {
            throw std::runtime_error{"not implemented"};
        }

    private:
        static void GetInfo(Graphics::TextureT, std::optional<Graphics::TextureFormatT>, Info&)
        {
            throw std::runtime_error{"not implemented"};
        }

        void Set(Graphics::TextureT)
        {
            throw std::runtime_error{"not implemented"};
        }
    };
}

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable : 4702) // unreachable code (Impl stubs always throw)
#endif

#include "ExternalTexture_Shared.h"

#if defined(_MSC_VER)
#pragma warning(pop)
#endif
