#include "ExternalBackBufferD3D11.h"

#include <Babylon/Graphics/D3D11Format.h>

#include <gsl/util>
#include <winrt/base.h>

#include <algorithm>
#include <array>
#include <cstdint>
#include <stdexcept>

namespace
{
    void ThrowIfFailed(HRESULT result, const char* message)
    {
        if (FAILED(result))
        {
            throw std::runtime_error{message};
        }
    }

    uint64_t GetMsaaFlags(uint32_t sampleCount)
    {
        switch (sampleCount)
        {
            case 1:
                return BGFX_TEXTURE_NONE;
            case 2:
                return BGFX_TEXTURE_RT_MSAA_X2;
            case 4:
                return BGFX_TEXTURE_RT_MSAA_X4;
            case 8:
                return BGFX_TEXTURE_RT_MSAA_X8;
            case 16:
                return BGFX_TEXTURE_RT_MSAA_X16;
            default:
                throw std::runtime_error{"Unsupported D3D11 external back buffer sample count."};
        }
    }

    struct ViewInfo
    {
        winrt::com_ptr<ID3D11Texture2D> Texture;
        D3D11_TEXTURE2D_DESC TextureDesc{};
        DXGI_FORMAT ViewFormat{DXGI_FORMAT_UNKNOWN};
        uint32_t Mip{};
        uint32_t FirstLayer{};
        uint32_t NumLayers{1};
        uint32_t Width{};
        uint32_t Height{};
        uint8_t AttachmentFlags{BGFX_ATTACHMENT_NONE};
        bgfx::TextureFormat::Enum BgfxFormat{bgfx::TextureFormat::Unknown};
        bool Srgb{};
    };

    ViewInfo GetTextureInfo(ID3D11View* view)
    {
        winrt::com_ptr<ID3D11Resource> resource;
        view->GetResource(resource.put());

        D3D11_RESOURCE_DIMENSION dimension{};
        resource->GetType(&dimension);
        if (dimension != D3D11_RESOURCE_DIMENSION_TEXTURE2D)
        {
            throw std::runtime_error{"D3D11 external back buffers must reference a Texture2D resource."};
        }

        ViewInfo info{};
        info.Texture = resource.as<ID3D11Texture2D>();
        info.Texture->GetDesc(&info.TextureDesc);
        return info;
    }

    void SetViewDimensions(ViewInfo& info)
    {
        info.Width = std::max(1u, info.TextureDesc.Width >> info.Mip);
        info.Height = std::max(1u, info.TextureDesc.Height >> info.Mip);
        if (info.FirstLayer + info.NumLayers > info.TextureDesc.ArraySize)
        {
            throw std::runtime_error{"D3D11 external back buffer view exceeds its texture array."};
        }
    }

    ViewInfo GetColorInfo(ID3D11RenderTargetView* view)
    {
        ViewInfo info = GetTextureInfo(view);

        D3D11_RENDER_TARGET_VIEW_DESC desc{};
        view->GetDesc(&desc);
        info.ViewFormat = desc.Format;
        switch (desc.ViewDimension)
        {
            case D3D11_RTV_DIMENSION_TEXTURE2D:
                info.Mip = desc.Texture2D.MipSlice;
                break;
            case D3D11_RTV_DIMENSION_TEXTURE2DARRAY:
                info.Mip = desc.Texture2DArray.MipSlice;
                info.FirstLayer = desc.Texture2DArray.FirstArraySlice;
                info.NumLayers = desc.Texture2DArray.ArraySize;
                break;
            case D3D11_RTV_DIMENSION_TEXTURE2DMS:
                break;
            case D3D11_RTV_DIMENSION_TEXTURE2DMSARRAY:
                info.FirstLayer = desc.Texture2DMSArray.FirstArraySlice;
                info.NumLayers = desc.Texture2DMSArray.ArraySize;
                break;
            default:
                throw std::runtime_error{"Unsupported D3D11 external color back buffer view dimension."};
        }

        const auto format = Babylon::Graphics::D3D11TextureFormats::TryGetBgfxTextureFormat(info.ViewFormat);
        if (!format || format->Format == bgfx::TextureFormat::Unknown)
        {
            throw std::runtime_error{"Unsupported D3D11 external color back buffer format."};
        }
        info.BgfxFormat = format->Format;
        info.Srgb = format->Srgb;
        SetViewDimensions(info);
        return info;
    }

    ViewInfo GetDepthInfo(ID3D11DepthStencilView* view)
    {
        ViewInfo info = GetTextureInfo(view);

        D3D11_DEPTH_STENCIL_VIEW_DESC desc{};
        view->GetDesc(&desc);
        info.ViewFormat = desc.Format;
        switch (desc.ViewDimension)
        {
            case D3D11_DSV_DIMENSION_TEXTURE2D:
                info.Mip = desc.Texture2D.MipSlice;
                break;
            case D3D11_DSV_DIMENSION_TEXTURE2DARRAY:
                info.Mip = desc.Texture2DArray.MipSlice;
                info.FirstLayer = desc.Texture2DArray.FirstArraySlice;
                info.NumLayers = desc.Texture2DArray.ArraySize;
                break;
            case D3D11_DSV_DIMENSION_TEXTURE2DMS:
                break;
            case D3D11_DSV_DIMENSION_TEXTURE2DMSARRAY:
                info.FirstLayer = desc.Texture2DMSArray.FirstArraySlice;
                info.NumLayers = desc.Texture2DMSArray.ArraySize;
                break;
            default:
                throw std::runtime_error{"Unsupported D3D11 external depth back buffer view dimension."};
        }

        const auto format = Babylon::Graphics::D3D11TextureFormats::TryGetBgfxDepthFormat(info.ViewFormat);
        if (!format)
        {
            throw std::runtime_error{"Unsupported D3D11 external depth back buffer format."};
        }
        info.BgfxFormat = *format;
        info.AttachmentFlags = static_cast<uint8_t>(
            ((desc.Flags & D3D11_DSV_READ_ONLY_DEPTH) ? BGFX_ATTACHMENT_READ_ONLY_DEPTH : 0) |
            ((desc.Flags & D3D11_DSV_READ_ONLY_STENCIL) ? BGFX_ATTACHMENT_READ_ONLY_STENCIL : 0));
        SetViewDimensions(info);
        return info;
    }

    void ValidateDevice(const ViewInfo& info, ID3D11Device* bgfxDevice)
    {
        winrt::com_ptr<ID3D11Device> resourceDevice;
        info.Texture->GetDevice(resourceDevice.put());
        const auto resourceIdentity = resourceDevice.as<IUnknown>();

        winrt::com_ptr<ID3D11Device> retainedBgfxDevice;
        retainedBgfxDevice.copy_from(bgfxDevice);
        const auto bgfxIdentity = retainedBgfxDevice.as<IUnknown>();
        if (resourceIdentity.get() != bgfxIdentity.get())
        {
            throw std::runtime_error{"D3D11 external back buffer belongs to a different device."};
        }
    }

    bgfx::TextureHandle ImportTexture(const ViewInfo& info)
    {
        uint64_t flags = BGFX_TEXTURE_RT_WRITE_ONLY | GetMsaaFlags(info.TextureDesc.SampleDesc.Count);
        if (info.Srgb)
        {
            flags |= BGFX_TEXTURE_SRGB;
        }

        const auto handle = bgfx::createTexture2D(
            static_cast<uint16_t>(info.TextureDesc.Width),
            static_cast<uint16_t>(info.TextureDesc.Height),
            info.TextureDesc.MipLevels > 1,
            static_cast<uint16_t>(info.TextureDesc.ArraySize),
            info.BgfxFormat,
            flags,
            nullptr,
            static_cast<uint64_t>(reinterpret_cast<uintptr_t>(info.Texture.get())));
        if (!bgfx::isValid(handle))
        {
            throw std::runtime_error{"Failed to import D3D11 external back buffer texture."};
        }
        return handle;
    }

    bgfx::Attachment MakeAttachment(bgfx::TextureHandle handle, const ViewInfo& info)
    {
        bgfx::Attachment attachment{};
        attachment.init(
            handle,
            bgfx::Access::Write,
            static_cast<uint16_t>(info.FirstLayer),
            static_cast<uint16_t>(info.NumLayers),
            static_cast<uint16_t>(info.Mip),
            info.AttachmentFlags);
        return attachment;
    }

    bgfx::TextureFormat::Enum GetCaptureFormat(DXGI_FORMAT format)
    {
        switch (format)
        {
            case DXGI_FORMAT_B8G8R8A8_UNORM:
            case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
                return bgfx::TextureFormat::BGRA8;
            case DXGI_FORMAT_R8G8B8A8_UNORM:
            case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
                return bgfx::TextureFormat::RGBA8;
            default:
                throw std::runtime_error{"Unsupported D3D11 external back buffer capture format."};
        }
    }

    struct DepthFormats
    {
        DXGI_FORMAT Resource;
        DXGI_FORMAT View;
    };

    DepthFormats GetDepthFormats(bgfx::TextureFormat::Enum format)
    {
        switch (format)
        {
            case bgfx::TextureFormat::D16:
                return {DXGI_FORMAT_R16_TYPELESS, DXGI_FORMAT_D16_UNORM};
            case bgfx::TextureFormat::D24S8:
                return {DXGI_FORMAT_R24G8_TYPELESS, DXGI_FORMAT_D24_UNORM_S8_UINT};
            case bgfx::TextureFormat::D32F:
                return {DXGI_FORMAT_R32_TYPELESS, DXGI_FORMAT_D32_FLOAT};
            case bgfx::TextureFormat::D32FS8:
                return {DXGI_FORMAT_R32G8X24_TYPELESS, DXGI_FORMAT_D32_FLOAT_S8X24_UINT};
            default:
                throw std::runtime_error{"Unsupported generated D3D11 back buffer depth format."};
        }
    }

    ViewInfo CreateDepthInfo(
        ID3D11Device* device,
        const ViewInfo& colorInfo,
        bgfx::TextureFormat::Enum format,
        winrt::com_ptr<ID3D11Texture2D>& texture,
        winrt::com_ptr<ID3D11DepthStencilView>& view)
    {
        const DepthFormats formats = GetDepthFormats(format);

        D3D11_TEXTURE2D_DESC textureDesc{};
        textureDesc.Width = colorInfo.Width;
        textureDesc.Height = colorInfo.Height;
        textureDesc.MipLevels = 1;
        textureDesc.ArraySize = colorInfo.NumLayers;
        textureDesc.Format = formats.Resource;
        textureDesc.SampleDesc = colorInfo.TextureDesc.SampleDesc;
        textureDesc.Usage = D3D11_USAGE_DEFAULT;
        textureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
        ThrowIfFailed(
            device->CreateTexture2D(&textureDesc, nullptr, texture.put()),
            "Failed to create native depth texture for D3D11 external back buffer.");

        D3D11_DEPTH_STENCIL_VIEW_DESC viewDesc{};
        viewDesc.Format = formats.View;
        if (textureDesc.SampleDesc.Count > 1)
        {
            if (textureDesc.ArraySize > 1)
            {
                viewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMSARRAY;
                viewDesc.Texture2DMSArray.ArraySize = textureDesc.ArraySize;
            }
            else
            {
                viewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
            }
        }
        else if (textureDesc.ArraySize > 1)
        {
            viewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
            viewDesc.Texture2DArray.ArraySize = textureDesc.ArraySize;
        }
        else
        {
            viewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
        }

        ThrowIfFailed(
            device->CreateDepthStencilView(texture.get(), &viewDesc, view.put()),
            "Failed to create native depth view for D3D11 external back buffer.");
        return GetDepthInfo(view.get());
    }
}

namespace Babylon::Graphics
{
    class ExternalBackBufferD3D11::Impl
    {
    public:
        Impl(BackBufferColorT color, BackBufferDepthStencilT depth, const bgfx::SwapChain& descriptor)
        {
            const auto* internalData = bgfx::getInternalData();
            auto* bgfxDevice = internalData == nullptr ? nullptr : static_cast<ID3D11Device*>(internalData->context);
            if (bgfxDevice == nullptr)
            {
                throw std::runtime_error{"D3D11 device is unavailable while importing an external back buffer."};
            }

            bool cleanupNeeded = true;
            const auto cleanup = gsl::finally([this, &cleanupNeeded] {
                if (cleanupNeeded)
                {
                    Destroy();
                }
            });

            std::optional<ViewInfo> colorInfo;
            if (color != nullptr)
            {
                m_colorView.copy_from(color);
                colorInfo = GetColorInfo(color);
                ValidateDevice(*colorInfo, bgfxDevice);
                m_colorTexture = colorInfo->Texture;
                m_colorInfo = *colorInfo;
                m_colorTextureHandle = ImportTexture(*colorInfo);
            }

            std::optional<ViewInfo> depthInfo;
            if (depth != nullptr)
            {
                m_depthView.copy_from(depth);
                depthInfo = GetDepthInfo(depth);
                ValidateDevice(*depthInfo, bgfxDevice);
                m_depthTexture = depthInfo->Texture;
                m_depthTextureHandle = ImportTexture(*depthInfo);
            }

            if (colorInfo && depthInfo &&
                (colorInfo->Width != depthInfo->Width ||
                 colorInfo->Height != depthInfo->Height ||
                 colorInfo->NumLayers != depthInfo->NumLayers ||
                 colorInfo->TextureDesc.SampleDesc.Count != depthInfo->TextureDesc.SampleDesc.Count ||
                 colorInfo->TextureDesc.SampleDesc.Quality != depthInfo->TextureDesc.SampleDesc.Quality))
            {
                throw std::runtime_error{"D3D11 external color and depth back buffer views do not match."};
            }

            if (colorInfo && !depthInfo && descriptor.formatDepthStencil != bgfx::TextureFormat::Count)
            {
                depthInfo = CreateDepthInfo(
                    bgfxDevice,
                    *colorInfo,
                    descriptor.formatDepthStencil,
                    m_depthTexture,
                    m_depthView);
                m_depthTextureHandle = ImportTexture(*depthInfo);
            }

            std::array<bgfx::Attachment, 2> attachments{};
            uint8_t attachmentCount{};
            if (colorInfo)
            {
                attachments[attachmentCount++] = MakeAttachment(m_colorTextureHandle, *colorInfo);
            }
            if (depthInfo && (colorInfo || descriptor.nwh == nullptr))
            {
                attachments[attachmentCount++] = MakeAttachment(m_depthTextureHandle, *depthInfo);
            }

            if (attachmentCount != 0)
            {
                m_frameBuffer = bgfx::createFrameBuffer(attachmentCount, attachments.data(), false);
                if (!bgfx::isValid(m_frameBuffer))
                {
                    throw std::runtime_error{"Failed to create D3D11 external back buffer framebuffer."};
                }
            }

            cleanupNeeded = false;
        }

        ~Impl()
        {
            Destroy();
        }

        bgfx::FrameBufferHandle GetFrameBuffer() const
        {
            return m_frameBuffer;
        }

        bgfx::TextureHandle GetDepthTexture() const
        {
            return m_depthTextureHandle;
        }

        void ReadPixels(const std::function<void(const BgfxCallback::CaptureData&)>& callback) const
        {
            if (!m_colorInfo || !m_colorTexture)
            {
                throw std::runtime_error{"Cannot capture a D3D11 external back buffer without a color view."};
            }

            const bgfx::TextureFormat::Enum captureFormat = GetCaptureFormat(m_colorInfo->ViewFormat);

            winrt::com_ptr<ID3D11Device> device;
            m_colorTexture->GetDevice(device.put());
            winrt::com_ptr<ID3D11DeviceContext> context;
            device->GetImmediateContext(context.put());

            D3D11_TEXTURE2D_DESC copyDesc{};
            copyDesc.Width = m_colorInfo->Width;
            copyDesc.Height = m_colorInfo->Height;
            copyDesc.MipLevels = 1;
            copyDesc.ArraySize = 1;
            copyDesc.Format = m_colorInfo->ViewFormat;
            copyDesc.SampleDesc.Count = 1;
            copyDesc.Usage = D3D11_USAGE_STAGING;
            copyDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

            winrt::com_ptr<ID3D11Texture2D> staging;
            ThrowIfFailed(
                device->CreateTexture2D(&copyDesc, nullptr, staging.put()),
                "Failed to create staging texture for D3D11 external back buffer capture.");

            const uint32_t sourceSubresource = D3D11CalcSubresource(
                m_colorInfo->Mip,
                m_colorInfo->FirstLayer,
                m_colorInfo->TextureDesc.MipLevels);

            if (m_colorInfo->TextureDesc.SampleDesc.Count > 1)
            {
                D3D11_TEXTURE2D_DESC resolveDesc = copyDesc;
                resolveDesc.Usage = D3D11_USAGE_DEFAULT;
                resolveDesc.CPUAccessFlags = 0;

                winrt::com_ptr<ID3D11Texture2D> resolved;
                ThrowIfFailed(
                    device->CreateTexture2D(&resolveDesc, nullptr, resolved.put()),
                    "Failed to create resolve texture for D3D11 external back buffer capture.");
                context->ResolveSubresource(
                    resolved.get(),
                    0,
                    m_colorTexture.get(),
                    sourceSubresource,
                    m_colorInfo->ViewFormat);
                context->CopyResource(staging.get(), resolved.get());
            }
            else
            {
                context->CopySubresourceRegion(
                    staging.get(),
                    0,
                    0,
                    0,
                    0,
                    m_colorTexture.get(),
                    sourceSubresource,
                    nullptr);
            }

            D3D11_MAPPED_SUBRESOURCE mapped{};
            ThrowIfFailed(
                context->Map(staging.get(), 0, D3D11_MAP_READ, 0, &mapped),
                "Failed to map D3D11 external back buffer capture.");
            const auto unmap = gsl::finally([&] { context->Unmap(staging.get(), 0); });

            callback(BgfxCallback::CaptureData{
                m_colorInfo->Width,
                m_colorInfo->Height,
                mapped.RowPitch,
                captureFormat,
                false,
                mapped.pData,
                mapped.RowPitch * m_colorInfo->Height});
        }

    private:
        void Destroy()
        {
            if (bgfx::isValid(m_frameBuffer))
            {
                bgfx::destroy(m_frameBuffer);
                m_frameBuffer = BGFX_INVALID_HANDLE;
            }
            if (bgfx::isValid(m_colorTextureHandle))
            {
                bgfx::destroy(m_colorTextureHandle);
                m_colorTextureHandle = BGFX_INVALID_HANDLE;
            }
            if (bgfx::isValid(m_depthTextureHandle))
            {
                bgfx::destroy(m_depthTextureHandle);
                m_depthTextureHandle = BGFX_INVALID_HANDLE;
            }
        }

        winrt::com_ptr<ID3D11RenderTargetView> m_colorView;
        winrt::com_ptr<ID3D11DepthStencilView> m_depthView;
        winrt::com_ptr<ID3D11Texture2D> m_colorTexture;
        winrt::com_ptr<ID3D11Texture2D> m_depthTexture;
        std::optional<ViewInfo> m_colorInfo;
        bgfx::FrameBufferHandle m_frameBuffer{BGFX_INVALID_HANDLE};
        bgfx::TextureHandle m_colorTextureHandle{BGFX_INVALID_HANDLE};
        bgfx::TextureHandle m_depthTextureHandle{BGFX_INVALID_HANDLE};
    };

    ExternalBackBufferD3D11::ExternalBackBufferD3D11(
        BackBufferColorT color,
        BackBufferDepthStencilT depth,
        const bgfx::SwapChain& descriptor)
        : m_impl{std::make_unique<Impl>(color, depth, descriptor)}
    {
    }

    ExternalBackBufferD3D11::~ExternalBackBufferD3D11() = default;

    bgfx::FrameBufferHandle ExternalBackBufferD3D11::GetFrameBuffer() const
    {
        return m_impl->GetFrameBuffer();
    }

    bgfx::TextureHandle ExternalBackBufferD3D11::GetDepthTexture() const
    {
        return m_impl->GetDepthTexture();
    }

    void ExternalBackBufferD3D11::ReadPixels(
        const std::function<void(const BgfxCallback::CaptureData&)>& callback) const
    {
        m_impl->ReadPixels(callback);
    }
}
