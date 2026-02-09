#pragma once

#include <cstdint>
#include <string>

namespace Babylon::Graphics
{
    struct WgpuBootstrapConfig final
    {
        uint32_t Width{1};
        uint32_t Height{1};
        void* SurfaceLayer{nullptr};
        bool PreferLowPower{false};
        bool EnableValidation{false};
    };

    struct WgpuBootstrapInfo final
    {
        uint32_t Backend{};
        uint32_t VendorId{};
        uint32_t DeviceId{};
        std::string AdapterName{};
    };

    class WgpuNative final
    {
    public:
        explicit WgpuNative(const WgpuBootstrapConfig& config);
        ~WgpuNative();

        WgpuNative(const WgpuNative&) = delete;
        WgpuNative& operator=(const WgpuNative&) = delete;

        WgpuNative(WgpuNative&&) noexcept = delete;
        WgpuNative& operator=(WgpuNative&&) noexcept = delete;

        bool IsValid() const;
        bool Resize(uint32_t width, uint32_t height);
        bool Render();
        WgpuBootstrapInfo GetInfo() const;

    private:
        void* m_context{};
    };
}
