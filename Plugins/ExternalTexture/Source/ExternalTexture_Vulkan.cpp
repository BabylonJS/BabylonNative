#include <Babylon/Plugins/ExternalTexture.h>
#include <Babylon/Graphics/DeviceContext.h>
#include <Babylon/Graphics/Texture.h>
#include <napi/pointer.h>

#include "ExternalTexture_Base.h"

#include <optional>
#include <stdexcept>

namespace Babylon::Plugins
{
    uintptr_t NativeTextureHandle(Graphics::TextureT ptr)
    {
        return reinterpret_cast<uintptr_t>(ptr);
    }

    class ExternalTexture::Impl final : public ImplBase
        {
        public:
            // Implemented in ExternalTexture_Shared.h
            Impl(Graphics::TextureT, std::optional<Graphics::TextureFormatT>);
            void Update(Graphics::TextureT, std::optional<Graphics::TextureFormatT>, std::optional<uint16_t>);

            Graphics::TextureT Get() const
            {
                return m_ptr;
            }

        private:
            // Shared ctor/Update call GetInfo first; throw here so Vulkan stays unsupported
            // without redefining the Shared out-of-line Impl/Update bodies.
            static void GetInfo(Graphics::TextureT, std::optional<Graphics::TextureFormatT>, Info&)
            {
                throw std::runtime_error{"ExternalTexture is not supported on Vulkan yet"};
            }

            void Set(Graphics::TextureT ptr)
            {
                m_ptr = ptr;
            }

            Graphics::TextureT m_ptr{};
        };
    }

#include "ExternalTexture_Shared.h"