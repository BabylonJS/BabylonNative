#pragma once

#include <CoreGraphics/CoreGraphics.h>
#include <cstdint>
#include <memory>
#include <vector>

namespace xr
{
    inline CGDataProviderRef CreateImageTrackingDataProvider(std::shared_ptr<const std::vector<uint8_t>> pixels)
    {
        using ImageTrackingData = decltype(pixels);
        auto owner{std::make_unique<ImageTrackingData>(std::move(pixels))};
        const auto provider{CGDataProviderCreateWithData(owner.get(), (*owner)->data(), (*owner)->size(),
            [](void* info, const void*, size_t) { delete static_cast<ImageTrackingData*>(info); })};
        if (provider != nullptr)
        {
            owner.release();
        }
        return provider;
    }
}
