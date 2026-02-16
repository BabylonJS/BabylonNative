#include "WgpuNative.h"
#include "WgpuInterop.h"

#include <algorithm>
#include <array>
#include <cstring>

#if defined(__APPLE__)
extern "C"
{
    void* objc_autoreleasePoolPush();
    void objc_autoreleasePoolPop(void* pool);
}
#endif

namespace
{
#if defined(__APPLE__)
    class ScopedAutoreleasePool final
    {
    public:
        ScopedAutoreleasePool()
            : m_pool{objc_autoreleasePoolPush()}
        {
        }

        ~ScopedAutoreleasePool()
        {
            objc_autoreleasePoolPop(m_pool);
        }

        ScopedAutoreleasePool(const ScopedAutoreleasePool&) = delete;
        ScopedAutoreleasePool& operator=(const ScopedAutoreleasePool&) = delete;

    private:
        void* m_pool{};
    };
#else
    class ScopedAutoreleasePool final
    {
    };
#endif
}

namespace Babylon::Graphics
{
    WgpuNative::WgpuNative(const WgpuBootstrapConfig& config)
    {
#if defined(__APPLE__)
        const ScopedAutoreleasePool pool{};
#endif

        BabylonWgpuConfig nativeConfig{};
        nativeConfig.width = config.Width;
        nativeConfig.height = config.Height;
        nativeConfig.surface_layer = config.SurfaceLayer;
        nativeConfig.prefer_low_power = static_cast<uint8_t>(config.PreferLowPower);
        nativeConfig.enable_validation = static_cast<uint8_t>(config.EnableValidation);
        nativeConfig.reserved0 = 0;
        nativeConfig.reserved1 = 0;

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
#if defined(__APPLE__)
            const ScopedAutoreleasePool pool{};
#endif
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

#if defined(__APPLE__)
        const ScopedAutoreleasePool pool{};
#endif
        return babylon_wgpu_resize(m_context, width, height);
    }

    bool WgpuNative::Render()
    {
        if (m_context == nullptr)
        {
            return false;
        }

#if defined(__APPLE__)
        const ScopedAutoreleasePool pool{};
#endif
        return babylon_wgpu_render(m_context);
    }

    WgpuBootstrapInfo WgpuNative::GetInfo() const
    {
#if defined(__APPLE__)
        const ScopedAutoreleasePool pool{};
#endif
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

        info.Backend = nativeInfo.backend;
        info.VendorId = nativeInfo.vendor_id;
        info.DeviceId = nativeInfo.device_id;

        auto adapterNameBegin = nativeInfo.adapter_name;
        auto adapterNameEnd = nativeInfo.adapter_name + std::size(nativeInfo.adapter_name);
        auto nulTerminator = std::find(adapterNameBegin, adapterNameEnd, '\0');
        info.AdapterName.assign(adapterNameBegin, nulTerminator);

        return info;
    }

    const std::string& WgpuNative::GetLastError() const
    {
        return m_lastError;
    }
}
