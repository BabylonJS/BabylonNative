#include <Babylon/Plugins/ExternalTexture.h>
#include <Babylon/Graphics/DeviceQueries.h>
#include <Babylon/Graphics/DeviceContext.h>
#include <Babylon/Graphics/Texture.h>
#include <napi/pointer.h>
#include <bx/bx.h>
#include <winrt/base.h>

#include "ExternalTexture_Base.h"

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
            return m_ptr.get();
        }

    private:
        static void GetInfo(Graphics::TextureT ptr, std::optional<Graphics::TextureFormatT> overrideFormat, Info& info)
        {
            winrt::com_ptr<ID3D11Resource> resource;
            resource.copy_from(ptr);

            D3D11_RESOURCE_DIMENSION type;
            resource->GetType(&type);
            if (type != D3D11_RESOURCE_DIMENSION_TEXTURE2D)
            {
                throw std::runtime_error{"Unsupported texture type"};
            }

            D3D11_TEXTURE2D_DESC desc;
            resource.as<ID3D11Texture2D>()->GetDesc(&desc);

            info.Width = static_cast<uint16_t>(desc.Width);
            info.Height = static_cast<uint16_t>(desc.Height);
            info.MipLevels = static_cast<uint16_t>(desc.MipLevels);
            info.NumLayers = static_cast<uint16_t>(desc.ArraySize);

            if ((desc.BindFlags & D3D11_BIND_RENDER_TARGET) != 0)
            {
                info.Flags |= BGFX_TEXTURE_RT;

                if (desc.SampleDesc.Count > 1)
                {
                    info.Flags |= BGFX_TEXTURE_MSAA_SAMPLE | RenderTargetSamplesToBgfxMsaaFlag(desc.SampleDesc.Count);
                }
            }

            DXGI_FORMAT targetFormat = overrideFormat.has_value() ? overrideFormat.value() : desc.Format;
            if (const auto format = Graphics::D3D11TextureFormats::TryGetBgfxTextureFormat(targetFormat))
            {
                info.Format = format->Format;
                if (format->Srgb)
                {
                    info.Flags |= BGFX_TEXTURE_SRGB;
                }
            }
        }

        void Set(Graphics::TextureT ptr)
        {
            m_ptr.copy_from(ptr);
        }

        winrt::com_ptr<ID3D11Resource> m_ptr{};
    };
}

#include "ExternalTexture_Shared.h"
