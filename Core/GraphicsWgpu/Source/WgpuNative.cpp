#include "WgpuNative.h"

#include <algorithm>
#include <array>
#include <cstring>

namespace
{
    struct BabylonWgpuConfig final
    {
        uint32_t Width;
        uint32_t Height;
        void* SurfaceLayer;
        uint8_t PreferLowPower;
        uint8_t EnableValidation;
        uint8_t Reserved0;
        uint8_t Reserved1;
    };

    struct BabylonWgpuInfo final
    {
        uint32_t Backend;
        uint32_t VendorId;
        uint32_t DeviceId;
        std::array<char, 128> AdapterName;
    };

    extern "C"
    {
        void* babylon_wgpu_create(const BabylonWgpuConfig* config);
        void babylon_wgpu_destroy(void* context);
        bool babylon_wgpu_resize(void* context, uint32_t width, uint32_t height);
        bool babylon_wgpu_render(void* context);
        bool babylon_wgpu_get_info(const void* context, BabylonWgpuInfo* outputInfo);
        bool babylon_wgpu_get_last_error(char* output, size_t outputLen);
    }
}

namespace Babylon::Graphics
{
    WgpuNative::WgpuNative(const WgpuBootstrapConfig& config)
    {
        BabylonWgpuConfig nativeConfig{};
        nativeConfig.Width = config.Width;
        nativeConfig.Height = config.Height;
        nativeConfig.SurfaceLayer = config.SurfaceLayer;
        nativeConfig.PreferLowPower = static_cast<uint8_t>(config.PreferLowPower);
        nativeConfig.EnableValidation = static_cast<uint8_t>(config.EnableValidation);
        nativeConfig.Reserved0 = 0;
        nativeConfig.Reserved1 = 0;

        m_context = babylon_wgpu_create(&nativeConfig);
        if (m_context == nullptr)
        {
            std::array<char, 1024> buffer{};
            if (babylon_wgpu_get_last_error(buffer.data(), buffer.size()))
            {
                m_lastError = buffer.data();
            }
        }
    }

    WgpuNative::~WgpuNative()
    {
        if (m_context != nullptr)
        {
            babylon_wgpu_destroy(m_context);
            m_context = nullptr;
        }
    }

    bool WgpuNative::IsValid() const
    {
        return m_context != nullptr;
    }

    bool WgpuNative::Resize(uint32_t width, uint32_t height)
    {
        if (m_context == nullptr)
        {
            return false;
        }

        return babylon_wgpu_resize(m_context, width, height);
    }

    bool WgpuNative::Render()
    {
        if (m_context == nullptr)
        {
            return false;
        }

        return babylon_wgpu_render(m_context);
    }

    WgpuBootstrapInfo WgpuNative::GetInfo() const
    {
        WgpuBootstrapInfo info{};

        if (m_context == nullptr)
        {
            return info;
        }

        BabylonWgpuInfo nativeInfo{};
        if (!babylon_wgpu_get_info(m_context, &nativeInfo))
        {
            return info;
        }

        info.Backend = nativeInfo.Backend;
        info.VendorId = nativeInfo.VendorId;
        info.DeviceId = nativeInfo.DeviceId;

        auto nulTerminator = std::find(nativeInfo.AdapterName.cbegin(), nativeInfo.AdapterName.cend(), '\0');
        info.AdapterName.assign(nativeInfo.AdapterName.cbegin(), nulTerminator);

        return info;
    }

    const std::string& WgpuNative::GetLastError() const
    {
        return m_lastError;
    }
}
